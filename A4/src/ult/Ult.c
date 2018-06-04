#include "../../include/ult/Ult.h"
#include "../../include/common/Vector.h"
#include "../../include/common/Utils.h"

#define _XOPEN_SOURCE

#include <ucontext.h>
#include <assert.h>

#define STACK_SIZE 100000

#define VectorItem_ToSize_t(item) (*(size_t *) item)

/* thread control block */
typedef struct
{
    int id;
    ucontext_t context;
    char * stack;
    int status;
} Thread;

typedef struct
{
    // Vector of Thread
    Vector threads;
    // Vector of Thread
    Vector zombies;
    size_t currThreadIndex;
    ucontext_t mainContext;
} Scheduler;

static Scheduler scheduler;

static void ThreadItemDestroyer(VectorItem item);

static void Thread_Init(Thread * self, ult_f f);

static void Thread_Destroy(Thread * self);

static void Scheduler_Init(Scheduler * self);

static void Scheduler_ReturnToMainContext(Scheduler * self);

Thread * Scheduler_NewThread(Scheduler * self, ult_f f);

static Thread * Scheduler_FindThread(Scheduler * self, int threadId, bool * alive);

void Scheduler_ScheduleNext(Scheduler * self);

void Scheduler_ExitCurrThread(Scheduler * self, int status);

void ult_init(ult_f f)
{
    Scheduler_Init(&scheduler);
    Thread * initThread = Scheduler_NewThread(&scheduler, f);
    swapcontext(&scheduler.mainContext, &initThread->context);
}

int ult_spawn(ult_f f)
{
    Thread * t = Scheduler_NewThread(&scheduler, f);
    return t->id;
}

void ult_yield()
{
    Scheduler_ScheduleNext(&scheduler);
}

void ult_exit(int status)
{
    Scheduler_ExitCurrThread(&scheduler, status);
}

int ult_join(int tid, int * status)
{
    for (;;)
    {
        bool alive;
        Thread * thread = Scheduler_FindThread(&scheduler, tid, &alive);
        if (!thread)
            return -1;

        if (!alive)
        {
            *status = thread->status;
            return 0;
        }

        Scheduler_ScheduleNext(&scheduler);
    }
}

ssize_t ult_read(int fd, void * buf, size_t size)
{
    // TODO: implementation
    return 0;
}

static void ThreadItemDestroyer(VectorItem item)
{
    Thread_Destroy((Thread *) item);
}

static void Thread_Init(Thread * self, ult_f f)
{
    static int currId = 0;
    self->id = currId++;
    self->stack = malloc(STACK_SIZE);
    if (!self->stack)
        terminate();
    ucontext_t * context = &self->context;
    getcontext(context);
    context->uc_link = 0;
    context->uc_stack.ss_flags = 0;
    context->uc_stack.ss_size = STACK_SIZE;
    context->uc_stack.ss_sp = self->stack;
    makecontext(context, f, 0);
    self->status = 0;
}

static void Thread_Destroy(Thread * self)
{
    if (self->stack)
        free(self->stack);
}

static void Scheduler_Init(Scheduler * self)
{
    Vector_Init(&self->threads, sizeof(Thread), ThreadItemDestroyer);
    Vector_Init(&self->zombies, sizeof(Thread), ThreadItemDestroyer);
    self->currThreadIndex = 0;
}

Thread * Scheduler_NewThread(Scheduler * self, ult_f f)
{
    Thread thread;
    Thread_Init(&thread, f);

    size_t currThreadIndex = self->currThreadIndex;
    size_t newThreadIndex = Vector_IsEmpty(&self->threads) ? 0 : currThreadIndex + 1;

    Vector_Insert(&self->threads, &thread, currThreadIndex);
    self->currThreadIndex = newThreadIndex;
    return Vector_At(&self->threads, currThreadIndex);
}

void Scheduler_ExitCurrThread(Scheduler * self, int status)
{
    if (Vector_Size(&self->threads) == 1)
        Scheduler_ReturnToMainContext(&scheduler);

    size_t currThreadIndex = self->currThreadIndex;
    Thread * currThread = Vector_At(&self->threads, currThreadIndex);

    currThread->status = status;
    free(currThread->stack);
    currThread->stack = NULL;

    Vector_Append(&self->zombies, currThread);
    Vector_Remove(&self->threads, currThreadIndex);

    size_t nextThreadIndex = currThreadIndex % Vector_Size(&self->threads);
    Thread * nextThread = Vector_At(&self->threads, nextThreadIndex);

    self->currThreadIndex = nextThreadIndex;
    setcontext(&nextThread->context);
}

static void Scheduler_ReturnToMainContext(Scheduler * self)
{
    Vector_Destroy(&self->threads);
    Vector_Destroy(&self->zombies);
    setcontext(&self->mainContext);
    // if setcontext fails, the function returns
    terminate();
}

static Thread * Scheduler_FindThread(Scheduler * self, int threadId, bool * alive)
{
    Thread * thread;

    Vector_ForeachBegin(&self->threads, thread, i)
        if (thread->id == threadId)
        {
            if (alive)
                *alive = true;
            return thread;
        }
    Vector_ForeachEnd

    Vector_ForeachBegin(&self->zombies, thread, j)
        if (thread->id == threadId)
        {
            if (alive)
                *alive = false;
            return thread;
        }
    Vector_ForeachEnd

    return NULL;
}

void Scheduler_ScheduleNext(Scheduler * self)
{
    assert(Vector_Size(&self->threads) != 0);

    size_t currThreadIndex = self->currThreadIndex;
    size_t nextThreadIndex = (currThreadIndex + 1) % Vector_Size(&self->threads);

    if (currThreadIndex == nextThreadIndex)
        return;

    Thread * currThread = Vector_At(&self->threads, currThreadIndex);
    Thread * nextThread = Vector_At(&self->threads, nextThreadIndex);

    self->currThreadIndex = nextThreadIndex;
    swapcontext(&currThread->context, &nextThread->context);
}
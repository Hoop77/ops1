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
    char stack[STACK_SIZE];
    int status;
} Thread;

typedef struct
{
    // Vector of Thread
    Vector threads;
    // Vector of size_t
    Vector aliveIndices;
    // Vector of size_t
    Vector zombieIndices;
    size_t currThreadPos;
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
    return Scheduler_NewThread(&scheduler, f)->id;
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
            return 0;

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
    // nothing to do
}

static void Scheduler_Init(Scheduler * self)
{
    Vector_Init(&scheduler.threads, sizeof(Scheduler), ThreadItemDestroyer);
    Vector_Init(&scheduler.aliveIndices, sizeof(size_t), NULL);
    Vector_Init(&scheduler.zombieIndices, sizeof(size_t), NULL);
    scheduler.currThreadPos = 0;
}

Thread * Scheduler_NewThread(Scheduler * self, ult_f f)
{
    Thread thread;
    Thread_Init(&thread, f);

    size_t threadIndex = Vector_Size(&scheduler.threads);
    Vector_Append(&scheduler.threads, &thread);
    Vector_Insert(&scheduler.aliveIndices, &threadIndex, scheduler.currThreadPos++);

    return Vector_At(&self->threads, Vector_Size(&self->threads) - 1);
}

void Scheduler_ExitCurrThread(Scheduler * self, int status)
{
    if (Vector_Size(&scheduler.threads) == 1)
        Scheduler_ReturnToMainContext(&scheduler);

    size_t currThreadPos = scheduler.currThreadPos;
    size_t currThreadIndex = VectorItem_ToSize_t(Vector_At(&scheduler.aliveIndices, currThreadPos));
    Thread * currThread = Vector_At(&self->threads, currThreadIndex);

    currThread->status = status;

    Vector_Append(&scheduler.zombieIndices, &currThreadIndex);
    Vector_Remove(&scheduler.aliveIndices, currThreadPos);

    size_t nextThreadPos = currThreadPos % Vector_Size(&scheduler.aliveIndices);
    size_t nextThreadIndex = VectorItem_ToSize_t(Vector_At(&scheduler.aliveIndices, nextThreadPos));
    Thread * nextThread = Vector_At(&scheduler.threads, nextThreadIndex);

    scheduler.currThreadPos = nextThreadPos;
    setcontext(&nextThread->context);
}

static void Scheduler_ReturnToMainContext(Scheduler * self)
{
    Vector_Destroy(&scheduler.threads);
    Vector_Destroy(&scheduler.aliveIndices);
    Vector_Destroy(&scheduler.zombieIndices);
    setcontext(&scheduler.mainContext);
    // if setcontext fails, the function returns
    terminate();
}

static Thread * Scheduler_FindThread(Scheduler * self, int threadId, bool * alive)
{
    Thread * thread;
    size_t * index;

    Vector_ForeachBegin(&scheduler.aliveIndices, index, i)
        thread = Vector_At(&scheduler.threads, *index);
        if (thread->id == threadId)
        {
            if (alive)
                *alive = true;
            return thread;
        }
    Vector_ForeachEnd

    Vector_ForeachBegin(&scheduler.zombieIndices, index, j)
        thread = Vector_At(&scheduler.threads, *index);
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
    assert(Vector_Size(&scheduler.aliveIndices) != 0);

    size_t currThreadPos = self->currThreadPos;
    size_t nextThreadPos = (currThreadPos + 1) % Vector_Size(&self->aliveIndices);

    if (currThreadPos == nextThreadPos)
        return;

    size_t currThreadIndex = VectorItem_ToSize_t(Vector_At(&self->aliveIndices, currThreadPos));
    size_t nextThreadIndex = VectorItem_ToSize_t(Vector_At(&self->aliveIndices, nextThreadPos));

    Thread * currThread = Vector_At(&self->threads, currThreadIndex);
    Thread * nextThread = Vector_At(&self->threads, nextThreadIndex);

    self->currThreadPos = nextThreadPos;
    swapcontext(&currThread->context, &nextThread->context);
}
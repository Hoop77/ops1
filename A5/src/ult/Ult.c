#include "../../include/ult/Ult.h"
#include "../../include/common/Vector.h"
#include "../../include/common/Utils.h"

#define _XOPEN_SOURCE

#include <ucontext.h>
#include <assert.h>
#include <sys/select.h>
#include <fcntl.h>

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
    Vector runThreads;
    // Vector of Thread
    Vector blockThreads;
    // Vector of Thread
    Vector zombieThreads;
    size_t currRunThreadIndex;
    size_t currBlockThreadIndex;
    size_t currNumBlockingThreads;
    ucontext_t mainContext;
} Scheduler;

static Scheduler scheduler;

static void ThreadItemDestroyer(VectorItem item);

static void Thread_Init(Thread * self, ult_f f);

static void Thread_Destroy(Thread * self);

static void Scheduler_Init(Scheduler * self);

static void Scheduler_Destroy(Scheduler * self);

static void Scheduler_ReturnToMainContext(Scheduler * self);

Thread * Scheduler_NewThread(Scheduler * self, ult_f f);

static Thread * Scheduler_FindThread(Scheduler * self, int threadId, bool * alive);

void Scheduler_Yield(Scheduler * self);

void Scheduler_ExitCurrThread(Scheduler * self, int status);

void Scheduler_MoveToBlocking(Scheduler * self);

void Scheduler_MoveToRunning(Scheduler * self);

void Scheduler_Block(Scheduler * self);

void ult_init(ult_f f)
{
    Scheduler_Init(&scheduler);
    Thread * initThread = Scheduler_NewThread(&scheduler, f);
    swapcontext(&scheduler.mainContext, &initThread->context);
    Scheduler_Destroy(&scheduler);
}

int ult_spawn(ult_f f)
{
    Thread * t = Scheduler_NewThread(&scheduler, f);
    return t->id;
}

void ult_yield()
{
    Scheduler_Yield(&scheduler);
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

        Scheduler_Yield(&scheduler);
    }
}

ssize_t ult_read(int fd, void * buf, size_t size)
{
    bool wasBlocked = false;
    for (;;)
    {
	    fd_set readfds;
	    FD_ZERO(&readfds);
	    FD_SET(fd, &readfds);
	    struct timeval timeout = { .tv_sec = 0, .tv_usec = 0 };
	    if (select(fd + 1, &readfds, NULL, NULL, &timeout) > 0)
        {
            if (wasBlocked)
            {
	            // TODO: consider this:
	            // self->currNumBlockingThreads = 0;
	            Scheduler_MoveToRunning(&scheduler);
            }

            return read(fd, buf, size);
        }

        if (!wasBlocked)
            Scheduler_MoveToBlocking(&scheduler);

        wasBlocked = true;
        Scheduler_Block(&scheduler);
    }
}

void Scheduler_MoveToBlocking(Scheduler * self)
{
    size_t currRunThreadIndex = self->currRunThreadIndex;
    size_t currBlockThreadIndex = self->currBlockThreadIndex;

    Thread * currRunThread = Vector_At(&self->runThreads, currRunThreadIndex);
    Vector_Insert(&self->blockThreads, currRunThread, currBlockThreadIndex);
    Vector_Remove(&self->runThreads, currRunThreadIndex);

	if (Vector_IsEmpty(&self->runThreads))
		self->currRunThreadIndex = 0;
	else
		self->currRunThreadIndex = currRunThreadIndex % Vector_Size(&self->runThreads);
}

void Scheduler_MoveToRunning(Scheduler * self)
{
    size_t currRunThreadIndex = self->currRunThreadIndex;
    size_t currBlockThreadIndex = self->currBlockThreadIndex;

    Thread * currBlockThread = Vector_At(&self->blockThreads, currBlockThreadIndex);
    Vector_Insert(&self->runThreads, currBlockThread, currRunThreadIndex);
    Vector_Remove(&self->blockThreads, currBlockThreadIndex);

	if (Vector_IsEmpty(&self->blockThreads))
		self->currBlockThreadIndex = 0;
	else
		self->currBlockThreadIndex = currBlockThreadIndex % Vector_Size(&self->blockThreads);
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
    Vector_Init(&self->runThreads, sizeof(Thread), ThreadItemDestroyer);
    Vector_Init(&self->blockThreads, sizeof(Thread), ThreadItemDestroyer);
    Vector_Init(&self->zombieThreads, sizeof(Thread), ThreadItemDestroyer);
    self->currRunThreadIndex = 0;
    self->currBlockThreadIndex = 0;
    self->currNumBlockingThreads = 0;
}

static void Scheduler_Destroy(Scheduler * self)
{
    Vector_Destroy(&self->runThreads);
    Vector_Destroy(&self->blockThreads);
    Vector_Destroy(&self->zombieThreads);
}

Thread * Scheduler_NewThread(Scheduler * self, ult_f f)
{
    Thread thread;
    Thread_Init(&thread, f);

    size_t currThreadIndex = self->currRunThreadIndex;
    size_t newThreadIndex = Vector_IsEmpty(&self->runThreads) ? 0 : currThreadIndex + 1;

    Vector_Insert(&self->runThreads, &thread, currThreadIndex);
    self->currRunThreadIndex = newThreadIndex;
    return Vector_At(&self->runThreads, currThreadIndex);
}

void Scheduler_ExitCurrThread(Scheduler * self, int status)
{
    if (Vector_Size(&self->runThreads) == 1)
        Scheduler_ReturnToMainContext(self);

    size_t currThreadIndex = self->currRunThreadIndex;
    Thread * currThread = Vector_At(&self->runThreads, currThreadIndex);

    currThread->status = status;

    Vector_Append(&self->zombieThreads, currThread);
    Vector_Remove(&self->runThreads, currThreadIndex);

    size_t nextThreadIndex = currThreadIndex % Vector_Size(&self->runThreads);
    Thread * nextThread = Vector_At(&self->runThreads, nextThreadIndex);

    self->currRunThreadIndex = nextThreadIndex;
    setcontext(&nextThread->context);
}

static void Scheduler_ReturnToMainContext(Scheduler * self)
{
    setcontext(&self->mainContext);
    // if setcontext fails, the function returns
    terminate();
}

static Thread * Scheduler_FindThread(Scheduler * self, int threadId, bool * alive)
{
    Thread * thread;

    Vector_ForeachBegin(&self->runThreads, thread, i)
        if (thread->id == threadId)
        {
            if (alive)
                *alive = true;
            return thread;
        }
    Vector_ForeachEnd

    Vector_ForeachBegin(&self->blockThreads, thread, j)
        if (thread->id == threadId)
        {
            if (alive)
                *alive = true;
            return thread;
        }
    Vector_ForeachEnd

    Vector_ForeachBegin(&self->zombieThreads, thread, k)
        if (thread->id == threadId)
        {
            if (alive)
                *alive = false;
            return thread;
        }
    Vector_ForeachEnd

    return NULL;
}

void Scheduler_Yield(Scheduler * self)
{
    assert(Vector_Size(&self->runThreads) != 0);

    size_t currRunThreadIndex = self->currRunThreadIndex;
    size_t nextRunThreadIndex = (currRunThreadIndex + 1) % Vector_Size(&self->runThreads);
    Thread * currRunThread = Vector_At(&self->runThreads, currRunThreadIndex);
    Thread * nextRunThread = Vector_At(&self->runThreads, nextRunThreadIndex);

    if (Vector_IsEmpty(&self->blockThreads))
    {
        if (currRunThreadIndex == nextRunThreadIndex)
            return;

        self->currRunThreadIndex = nextRunThreadIndex;
        swapcontext(&currRunThread->context, &nextRunThread->context);
        return;
    }
    else
    {
        self->currRunThreadIndex = nextRunThreadIndex;
        if (self->currNumBlockingThreads >= Vector_Size(&self->blockThreads))
        {
            self->currNumBlockingThreads = 0;
            swapcontext(&currRunThread->context, &nextRunThread->context);
        }
        else
        {
            size_t currBlockingThreadIndex = self->currBlockThreadIndex;
            Thread * currBlockingThread = Vector_At(&self->blockThreads, currBlockingThreadIndex);
            swapcontext(&currRunThread->context, &currBlockingThread->context);
        }
    }
}

void Scheduler_Block(Scheduler * self)
{
    assert(Vector_Size(&self->blockThreads) != 0);

    size_t currBlockThreadIndex = self->currBlockThreadIndex;
    size_t nextBlockThreadIndex = (currBlockThreadIndex + 1) % Vector_Size(&self->blockThreads);
    Thread * currBlockThread = Vector_At(&self->blockThreads, currBlockThreadIndex);
    Thread * nextBlockThread = Vector_At(&self->blockThreads, nextBlockThreadIndex);

    if (Vector_IsEmpty(&self->runThreads) ||
        self->currNumBlockingThreads < Vector_Size(&self->blockThreads))
    {
        self->currNumBlockingThreads++;

        if (currBlockThreadIndex == nextBlockThreadIndex)
            return;

        self->currBlockThreadIndex = nextBlockThreadIndex;
        swapcontext(&currBlockThread->context, &nextBlockThread->context);
        return;
    }

    size_t nextRunThreadIndex = self->currRunThreadIndex;
    Thread * nextRunThread = Vector_At(&self->runThreads, nextRunThreadIndex);
    self->currNumBlockingThreads = 0;
    swapcontext(&currBlockThread->context, &nextRunThread->context);
}
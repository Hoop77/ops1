#include "../../include/ult/Ult.h"
#include "../../include/common/Array.h"
#include "../../include/common/Vector.h"
#include "../../include/common/Utils.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>

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
} ThreadControlBlock;

typedef struct
{
    // Vector of ThreadControlBlock
    Vector threads;
    // Vector of size_t
    Vector aliveIndices;
    // Vector of size_t
    Vector zombieIndices;
    size_t currThreadPos;
    ucontext_t mainContext;
} Scheduler;

static Scheduler scheduler;

static void Scheduler_ReturnToMainContext(Scheduler * self)
{
    Vector_Destroy(&scheduler.threads);
    Vector_Destroy(&scheduler.aliveIndices);
    Vector_Destroy(&scheduler.zombieIndices);
    setcontext(&scheduler.mainContext);
    // if setcontext fails, the function returns
    terminate();
}

static void ThreadControlBlock_Destroy(ThreadControlBlock * self)
{
    // nothing to do
}

static void ThreadControlBlockItemDestroyer(VectorItem * item)
{
    ThreadControlBlock_Destroy(item);
}

static void PrimitiveItemDestroyer()
{
    // nothing to do
}

static void ThreadControlBlock_Init(ThreadControlBlock * self, ult_f f)
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

static void Scheduler_Init(Scheduler * self)
{
    Vector_Init(&scheduler.threads, sizeof(Scheduler), ThreadControlBlockItemDestroyer);
    Vector_Init(&scheduler.aliveIndices, sizeof(size_t), PrimitiveItemDestroyer);
    Vector_Init(&scheduler.zombieIndices, sizeof(size_t), PrimitiveItemDestroyer);
    scheduler.currThreadPos = 0;
}

void ult_init(ult_f f)
{
    Scheduler_Init(&scheduler);

    ThreadControlBlock initThread;
    ThreadControlBlock_Init(&initThread, f);

    size_t threadIndex = Vector_Size(&scheduler.threads);
    Vector_Append(&scheduler.threads, &initThread);
    Vector_Append(&scheduler.aliveIndices, threadIndex);

    swapcontext(&scheduler.mainContext, &initThread.context);
}

int ult_spawn(ult_f f)
{
    ThreadControlBlock thread;
    ThreadControlBlock_Init(&thread, f);

    size_t threadIndex = Vector_Size(&scheduler.threads);
    Vector_Append(&scheduler.threads, &thread);
    Vector_Insert(&scheduler.aliveIndices, &threadIndex, scheduler.currThreadPos++);

	return thread.id;
}

void ult_yield()
{
    assert(Vector_Size(&scheduler.aliveIndices) != 0);

    if (Vector_Size(&scheduler.aliveIndices) == 1)
        return;

    size_t currThreadPos = scheduler.currThreadPos;
    size_t nextThreadPos = (currThreadPos + 1) % Vector_Size(&scheduler.aliveIndices);

    size_t currThreadIndex = VectorItem_ToSize_t(Vector_At(&scheduler.aliveIndices, currThreadPos));
    size_t nextThreadIndex = VectorItem_ToSize_t(Vector_At(&scheduler.aliveIndices, nextThreadPos));

    ucontext_t * currContext = Vector_At(&scheduler.threads, currThreadIndex);
    ucontext_t * nextContext = Vector_At(&scheduler.threads, nextThreadIndex);

    scheduler.currThreadPos = nextThreadPos;
    swapcontext(currContext, nextContext);
}

void ult_exit(int status)
{
    if (Vector_Size(&scheduler.threads) == 1)
        Scheduler_ReturnToMainContext(&scheduler);

    size_t currThreadIndex = scheduler.currThreadPos;
    ThreadControlBlock * currThread = Vector_At(&scheduler.threads, currThreadIndex);
    Vector_Append(&scheduler.zombies, currThread);
    Vector_Remove(&scheduler.threads, currThreadIndex);
    scheduler.currThreadPos = currThreadIndex % Vector_Size(&scheduler.threads);
    ThreadControlBlock * nextThread = Vector_At(&scheduler.threads, scheduler.currThreadPos);
    setcontext(&nextThread->context);
}

int ult_join(int tid, int* status)
{
	return -1;
}

ssize_t ult_read(int fd, void* buf, size_t size)
{
	return 0;
}

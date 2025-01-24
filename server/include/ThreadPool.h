// Thread Pool ADT Interface

#ifndef THREAD_POOL_H
#define THREAD_POOL_H

#include "TaskQueue.h"

typedef struct thread_pool *ThreadPool; 

/**
 * Creates a new thread pool.
 */
ThreadPool ThreadPoolNew(void); 

/**
 * Frees a thread pool.
 */
void ThreadPoolFree(ThreadPool pool); 

/**
 * Adds the given task to the task queue of the thread pool.
 */
void ThreadPoolAddTask(ThreadPool pool, Task task);

/**
 * Continuously executes tasks from the task pool, used by each worker thread.
 */
void *ThreadPoolWorker(void *arg);

#endif

// Thread Pool Interface

/**
 * A thread pool consists of multiple worker threads that execute tasks 
 * from a shared task queue. Tasks can be added to the task queue, 
 * and worker threads continuously fetch and execute tasks.
 */

#ifndef THREAD_POOL_H
#define THREAD_POOL_H

#include "task_queue.h"

typedef struct thread_pool *ThreadPool; 

/**
 * Creates a new thread pool.
 * Returns NULL on error.
 */
ThreadPool ThreadPoolNew(int thread_count); 

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

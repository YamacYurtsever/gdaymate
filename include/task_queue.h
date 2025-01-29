// Task Queue Interface

/**
 * A task queue is a thread-safe queue that stores tasks.
 * Tasks can be enqueued, dequeued, and the queue can be checked for emptiness.
 */

#ifndef TASK_QUEUE_H
#define TASK_QUEUE_H

#include <stdbool.h>

#include "task.h"

typedef struct task_queue *TaskQueue; 

/**
 * Creates a new task queue.
 * Returns NULL on error.
 */
TaskQueue TaskQueueNew(void); 

/**
 * Frees a task queue.
 */
void TaskQueueFree(TaskQueue q); 

/**
 * Enqueues a new task into a task queue.
 */
void TaskQueueEnqueue(TaskQueue q, Task task); 

/**
 * Dequeues a task from a task queue. Assumes the task queue is not empty.
 */
Task TaskQueueDequeue(TaskQueue q); 

/**
 * Checks if a task queue is empty.
 */
bool TaskQueueIsEmpty(TaskQueue q); 

#endif

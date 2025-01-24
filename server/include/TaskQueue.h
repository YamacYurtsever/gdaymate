// Task Queue ADT Interface

#ifndef TASK_QUEUE_H
#define TASK_QUEUE_H

#include <stdbool.h>

typedef struct task_queue *TaskQueue; 

/**
 * Creates a new task queue.
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

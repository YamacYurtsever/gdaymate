// Task Interface

/**
 * A task consists of a function and an integer argument. 
 * Tasks can be created, executed, and freed dynamically.
 */

#ifndef TASK_H
#define TASK_H

#include "task.h"
#include "server.h"

typedef struct task *Task;
typedef struct task_arg *TaskArg;

/**
 * Creates a new task.
 * Returns NULL on error.
 */
Task TaskNew(void *(*function)(void *arg), void *arg);

/**
 * Frees a task.
 */
void TaskFree(Task task);

/**
 * Executes a task, calling its function with its argument.
 */
void TaskExecute(Task task);

#endif

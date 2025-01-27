// Task Interface

/**
 * A task consists of a function and an integer argument. 
 * Tasks can be created, executed, and freed dynamically.
 */

#ifndef TASK_H
#define TASK_H

typedef struct task *Task;

/**
 * Creates a new task.
 */
Task TaskNew(void (*function)(int arg), int arg);

/**
 * Frees a task.
 */
void TaskFree(Task task);

/**
 * Executes a task, calling its function with its argument.
 */
void TaskExecute(Task task);

#endif

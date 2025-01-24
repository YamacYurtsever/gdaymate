// Task ADT Interface

#ifndef TASK_H
#define TASK_H

/**
 * Creates a new task.
 */
Task TaskNew(void (*function)(int arg), int arg);

/**
 * Frees a task.
 */
void TaskFree(Task task);

/**
 * Executes a task.
 */
void TaskExecute(Task task);

#ifndef

// Task Queue Tests

#include <stdio.h>
#include <assert.h>

#include "task_queue.h"
#include "task.h"

void test_TaskQueueNew(void);
void test_TaskQueueEnqueueDequeue(void);
void test_TaskQueueIsEmpty(void);
void log_task(int arg);

int main(void) {
    test_TaskQueueNew();
    test_TaskQueueEnqueueDequeue();
    test_TaskQueueIsEmpty();

    printf("All TaskQueue tests passed\n");
    return 0;
}

void test_TaskQueueNew(void) {
    TaskQueue q = TaskQueueNew();
    assert(q != NULL);
    assert(TaskQueueIsEmpty(q));
    TaskQueueFree(q);
}

void test_TaskQueueEnqueueDequeue(void) {
    TaskQueue q = TaskQueueNew();
    Task task1 = TaskNew(log_task, 1);
    Task task2 = TaskNew(log_task, 2);
    Task task3 = TaskNew(log_task, 3);

    TaskQueueEnqueue(q, task1);
    TaskQueueEnqueue(q, task2);
    TaskQueueEnqueue(q, task3);

    Task dequeuedTask = TaskQueueDequeue(q);
    assert(dequeuedTask == task1);
    TaskExecute(dequeuedTask);
    TaskFree(dequeuedTask);

    dequeuedTask = TaskQueueDequeue(q);
    assert(dequeuedTask == task2);
    TaskExecute(dequeuedTask);
    TaskFree(dequeuedTask);

    dequeuedTask = TaskQueueDequeue(q);
    assert(dequeuedTask == task3);
    TaskExecute(dequeuedTask);
    TaskFree(dequeuedTask);

    TaskQueueFree(q);
}

void test_TaskQueueIsEmpty(void) {
    TaskQueue q = TaskQueueNew();
    assert(TaskQueueIsEmpty(q));

    Task task = TaskNew(log_task, 42);
    TaskQueueEnqueue(q, task);
    assert(!TaskQueueIsEmpty(q));

    Task dequeuedTask = TaskQueueDequeue(q);
    assert(dequeuedTask == task);
    assert(TaskQueueIsEmpty(q));

    TaskFree(dequeuedTask);
    TaskQueueFree(q);
}

void log_task(int arg) {
    printf("Executing task with argument: %d\n", arg);
}

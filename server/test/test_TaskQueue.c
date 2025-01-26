// Task Queue ADT Tests

#include <stdio.h>
#include <assert.h>

#include "TaskQueue.h"
#include "Task.h"

void testTaskQueueNew(void);
void testTaskQueueEnqueueDequeue(void);
void testTaskQueueIsEmpty(void);
void testFunction(int arg);

int main(void) {
    testTaskQueueNew();
    testTaskQueueEnqueueDequeue();
    testTaskQueueIsEmpty();

    printf("All TaskQueue tests passed!\n");
    return 0;
}

void testTaskQueueNew(void) {
    TaskQueue q = TaskQueueNew();
    assert(q != NULL);
    assert(TaskQueueIsEmpty(q));
    TaskQueueFree(q);
}

void testTaskQueueEnqueueDequeue(void) {
    TaskQueue q = TaskQueueNew();
    Task task1 = TaskNew(testFunction, 1);
    Task task2 = TaskNew(testFunction, 2);
    Task task3 = TaskNew(testFunction, 3);

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

void testTaskQueueIsEmpty(void) {
    TaskQueue q = TaskQueueNew();
    assert(TaskQueueIsEmpty(q));

    Task task = TaskNew(testFunction, 42);
    TaskQueueEnqueue(q, task);
    assert(!TaskQueueIsEmpty(q));

    Task dequeuedTask = TaskQueueDequeue(q);
    assert(dequeuedTask == task);
    assert(TaskQueueIsEmpty(q));

    TaskFree(dequeuedTask);
    TaskQueueFree(q);
}

void testFunction(int arg) {
    printf("Executing task with argument: %d\n", arg);
}

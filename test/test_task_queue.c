// Task Queue Tests

#include <stdio.h>
#include <assert.h>

#include "task_queue.h"
#include "task.h"

void test_TaskQueueNew(void);
void test_TaskQueueEnqueueDequeue(void);
void test_TaskQueueIsEmpty(void);
void *log_task(void *arg);

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

    int task1_arg = 1;
    int task2_arg = 2;
    int task3_arg = 3;
    
    Task task1 = TaskNew(log_task, &task1_arg);
    Task task2 = TaskNew(log_task, &task2_arg);
    Task task3 = TaskNew(log_task, &task3_arg);

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

    int task_arg = 42;
    Task task = TaskNew(log_task, &task_arg);
    TaskQueueEnqueue(q, task);
    assert(!TaskQueueIsEmpty(q));

    Task dequeuedTask = TaskQueueDequeue(q);
    assert(dequeuedTask == task);
    assert(TaskQueueIsEmpty(q));

    TaskFree(dequeuedTask);
    TaskQueueFree(q);
}

void *log_task(void *arg) {
    int log_task_arg = *(int *)arg;
    printf("Executing task with argument: %d\n", log_task_arg);
    return NULL;
}

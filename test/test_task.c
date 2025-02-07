// Task Tests

#include <stdio.h>
#include <assert.h>

#include "task.h"

void test_TaskNew(void);
void test_TaskExecute(void);
void *log_task(void *arg);

int main(void) {
    test_TaskNew();
    test_TaskExecute();

    printf("All Task tests passed\n");

    return 0;
}

void test_TaskNew(void) {
    int task_arg = 1;
    Task task = TaskNew(log_task, &task_arg);
    assert(task != NULL);
    TaskFree(task);
}

void test_TaskExecute(void) {
    int task_arg = 1;
    Task task = TaskNew(log_task, &task_arg);
    TaskExecute(task);
    TaskFree(task);
}

void *log_task(void *arg) {
    int log_task_arg = *(int *)arg;
    printf("Executing task with argument: %d\n", log_task_arg);
    return NULL;
}

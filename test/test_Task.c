// Task ADT Tests

#include <stdio.h>
#include <assert.h>

#include "task.h"

void test_TaskNew(void);
void test_TaskExecute(void);
void log_task(int arg);

int main(void) {
    test_TaskNew();
    test_TaskExecute();

    printf("All Task tests passed\n");

    return 0;
}

void test_TaskNew(void) {
    Task task = TaskNew(log_task, 1);
    assert(task != NULL);
    TaskFree(task);
}

void test_TaskExecute(void) {
    Task task = TaskNew(log_task, 1);
    TaskExecute(task);
    TaskFree(task);
}

void log_task(int arg) {
    printf("Executing task with argument: %d\n", arg);
}

// Task ADT Tests

#include <stdio.h>
#include <assert.h>

#include "Task.h"

void testTaskNew(void);
void testTaskExecute(void);
void testFunction(int arg);

int main(void) {
    testTaskNew();
    testTaskExecute();

    printf("All Task ADT tests passed!\n");

    return 0;
}

void testTaskNew(void) {
    Task task = TaskNew(testFunction, 1);
    assert(task != NULL);
    TaskFree(task);
}

void testTaskExecute(void) {
    Task task = TaskNew(testFunction, 1);
    TaskExecute(task);
    TaskFree(task);
}

void testFunction(int arg) {
    printf("Task executed with arg: %d\n", arg);
}

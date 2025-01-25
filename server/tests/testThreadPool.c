// Thread Pool ADT Tests

#include <stdio.h>
#include <assert.h>
#include <unistd.h>

#include "ThreadPool.h"
#include "TaskQueue.h"
#include "Task.h"

void testThreadPoolNew(void);
void testThreadPoolAddTask(void);
void testFunction(int arg);

int main(void) {
    testThreadPoolNew();
    testThreadPoolAddTask();

    printf("All ThreadPool ADT tests passed!\n");
    return 0;
}

void testThreadPoolNew(void) {
    ThreadPool pool = ThreadPoolNew();
    assert(pool != NULL);
    ThreadPoolFree(pool);
}

void testThreadPoolAddTask(void) {
    ThreadPool pool = ThreadPoolNew();
    
    Task task1 = TaskNew(testFunction, 1);
    Task task2 = TaskNew(testFunction, 2);
    Task task3 = TaskNew(testFunction, 3);
    
    ThreadPoolAddTask(pool, task1);
    ThreadPoolAddTask(pool, task2);
    ThreadPoolAddTask(pool, task3);
    
    sleep(2);
    
    ThreadPoolFree(pool);
}

void testFunction(int arg) {
    printf("Executing task with argument: %d\n", arg);
}

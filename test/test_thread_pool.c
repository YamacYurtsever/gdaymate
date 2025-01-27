// Thread Pool Tests

#include <stdio.h>
#include <assert.h>
#include <unistd.h>

#include "thread_pool.h"
#include "task_queue.h"
#include "task.h"

void test_ThreadPoolNew(void);
void test_ThreadPoolAddTask(void);
void log_task(int arg);

int main(void) {
    test_ThreadPoolNew();
    test_ThreadPoolAddTask();

    printf("All ThreadPool tests passed\n");
    return 0;
}

void test_ThreadPoolNew(void) {
    ThreadPool pool = ThreadPoolNew(5);
    assert(pool != NULL);
    ThreadPoolFree(pool);
}

void test_ThreadPoolAddTask(void) {
    ThreadPool pool = ThreadPoolNew(5);
    
    Task task1 = TaskNew(log_task, 1);
    Task task2 = TaskNew(log_task, 2);
    Task task3 = TaskNew(log_task, 3);
    
    ThreadPoolAddTask(pool, task1);
    ThreadPoolAddTask(pool, task2);
    ThreadPoolAddTask(pool, task3);
    
    sleep(2);
    
    ThreadPoolFree(pool);
}

void log_task(int arg) {
    printf("Executing task with argument: %d\n", arg);
}

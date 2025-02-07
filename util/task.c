// Task Implementation

#include <stdio.h>
#include <stdlib.h>

#include "task.h"

struct task {
    void *(*function)(void *arg);
    void *arg;
};

Task TaskNew(void *(*function)(void *arg), void *arg) {
    Task task = malloc(sizeof(struct task));
    if (task == NULL) {
        perror("malloc");
        return NULL;
    }

    task->function = function;
    task->arg = arg;

    return task;
}

void TaskFree(Task task) {
    free(task);
}

void TaskExecute(Task task) {
    task->function(task->arg);
}

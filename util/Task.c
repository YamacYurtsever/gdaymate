// Task Implementation

#include <stdio.h>
#include <stdlib.h>

#include "task.h"

struct task {
    void (*function)(int arg);
    int arg;
};

Task TaskNew(void (*function)(int arg), int arg) {
    Task task = malloc(sizeof(*task));
    if (task == NULL) {
        perror("malloc");
        exit(EXIT_FAILURE);
    }

    task->function = function;
    task->arg = arg;

    return task;
}

void TaskExecute(Task task) {
    task->function(task->arg);
}

void TaskFree(Task task) {
    free(task);
}

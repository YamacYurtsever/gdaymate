// Task Implementation

#include <stdio.h>
#include <stdlib.h>

#include "task.h"

struct task {
    void (*function)(Server srv, int poll_idx);
    Server srv;
    int poll_idx;
};

Task TaskNew(
    void (*function)(Server srv, int poll_idx), 
    Server srv, 
    int poll_idx
) {
    Task task = malloc(sizeof(*task));
    if (task == NULL) {
        perror("malloc");
        return NULL;
    }

    task->function = function;
    task->srv = srv;
    task->poll_idx = poll_idx;

    return task;
}

void TaskExecute(Task task) {
    task->function(task->srv, task->poll_idx);
}

void TaskFree(Task task) {
    free(task);
}

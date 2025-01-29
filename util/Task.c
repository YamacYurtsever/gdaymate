// Task Implementation

#include <stdio.h>
#include <stdlib.h>

#include "task.h"

struct task {
    void (*function)(Server srv, int client_sockfd);
    Server srv;
    int client_sockfd;
};

Task TaskNew(
    void (*function)(Server srv, int client_sockfd), 
    Server srv, 
    int client_sockfd
) {
    Task task = malloc(sizeof(*task));
    if (task == NULL) {
        perror("malloc");
        return NULL;
    }

    task->function = function;
    task->srv = srv;
    task->client_sockfd = client_sockfd;

    return task;
}

void TaskExecute(Task task) {
    task->function(task->srv, task->client_sockfd);
}

void TaskFree(Task task) {
    free(task);
}

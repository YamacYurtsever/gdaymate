// Task Queue ADT Implementation

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <pthread.h>

#include "task_queue.h"
#include "task.h"

struct task_queue {
    struct node *front;
    struct node *back;
    pthread_mutex_t lock;
};

struct node {
    Task task;
    struct node *next;
};

TaskQueue TaskQueueNew(void) {
    TaskQueue q = malloc(sizeof(struct task_queue));
    if (q == NULL) {
		fprintf(stderr, "error: out of memory\n");
		exit(EXIT_FAILURE);
	}

    q->front = NULL;
    q->back = NULL;
    pthread_mutex_init(&q->lock, NULL); 

    return q;
}

void TaskQueueFree(TaskQueue q) {
    pthread_mutex_lock(&q->lock);

    struct node* curr = q->front;
    while (curr != NULL) {
        struct node *temp = curr;
        curr = curr->next;
        TaskFree(temp->task);
        free(temp);
    }

    pthread_mutex_unlock(&q->lock);
    pthread_mutex_destroy(&q->lock);
    
    free(q);
}

void TaskQueueEnqueue(TaskQueue q, Task task) {
    pthread_mutex_lock(&q->lock);

    struct node *new = malloc(sizeof(struct node));
    if (new == NULL) {
		fprintf(stderr, "error: out of memory\n");
		exit(EXIT_FAILURE);
	}

    new->task = task;
    new->next = NULL;

    if (q->front == NULL) {
        q->front = new;
        q->back = new;
    } else {
        q->back->next = new;
        q->back = new;
    }

    pthread_mutex_unlock(&q->lock);
}

Task TaskQueueDequeue(TaskQueue q) {
    pthread_mutex_lock(&q->lock);

    Task task = q->front->task;
    struct node *temp = q->front;
    q->front = q->front->next;
    free(temp);

    pthread_mutex_unlock(&q->lock);

    return task;
}

bool TaskQueueIsEmpty(TaskQueue q) {
    pthread_mutex_lock(&q->lock);

    bool is_empty =  (q->front == NULL);

    pthread_mutex_unlock(&q->lock);

    return is_empty;
}

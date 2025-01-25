// Thread Pool ADT Implementation

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <stdbool.h>
#include <unistd.h>

#include "ThreadPool.h"
#include "TaskQueue.h"
#include "Task.h"

struct thread_pool {
    TaskQueue task_queue;
    int thread_count;
    pthread_t *threads;
    bool shutdown;
    pthread_mutex_t lock;
    pthread_cond_t cond;
};

ThreadPool ThreadPoolNew(int thread_count) {
    ThreadPool pool = malloc(sizeof(*pool));
    if (pool == NULL) {
        perror("malloc");
        exit(EXIT_FAILURE);
    }

    pool->task_queue = TaskQueueNew();
    if (pool->task_queue == NULL) {
        free(pool->threads);
        free(pool);
        exit(EXIT_FAILURE);
    }

    pool->thread_count = thread_count;

    pool->threads = malloc(sizeof(pthread_t) * thread_count);
    if (pool->threads == NULL) {
        perror("malloc");
        free(pool);
        exit(EXIT_FAILURE);
    }

    pool->shutdown = false;

    pthread_mutex_init(&pool->lock, NULL);
    pthread_cond_init(&pool->cond, NULL);

    // Create worker threads
    for (int i = 0; i < pool->thread_count; i++) {
        pthread_create(&pool->threads[i], NULL, ThreadPoolWorker, pool);
    }

    return pool;
}

void ThreadPoolFree(ThreadPool pool) {
    // Signal shutdown
    pthread_mutex_lock(&pool->lock);
    pool->shutdown = true;
    pthread_cond_broadcast(&pool->cond);
    pthread_mutex_unlock(&pool->lock);

    pthread_cond_destroy(&pool->cond);
    pthread_mutex_destroy(&pool->lock);

    for (int i = 0; i < pool->thread_count; i++) {
        pthread_join(pool->threads[i], NULL);
    }

    TaskQueueFree(pool->task_queue);
    free(pool->threads);
    free(pool);
}

void ThreadPoolAddTask(ThreadPool pool, Task task) {
    pthread_mutex_lock(&pool->lock);
    TaskQueueEnqueue(pool->task_queue, task);
    pthread_cond_signal(&pool->cond);
    pthread_mutex_unlock(&pool->lock);
}

void *ThreadPoolWorker(void *arg) {
    ThreadPool pool = (ThreadPool)arg;

    while (true) {
        pthread_mutex_lock(&pool->lock);

        if (pool->shutdown) {
            pthread_mutex_unlock(&pool->lock);
            break;
        } 

        if (!TaskQueueIsEmpty(pool->task_queue)) {
            Task task = TaskQueueDequeue(pool->task_queue);
            TaskExecute(task);
            TaskFree(task);
        } else {
            pthread_cond_wait(&pool->cond, &pool->lock);
        }

        pthread_mutex_unlock(&pool->lock);
    }

    return NULL;
}

// Task ADT Implementation

#include "Task.h"

typedef struct task *Task;

struct task {
    void (*function)(int arg);
    int arg;
};

#define _GNU_SOURCE

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <string.h>
#include <unistd.h>

#include "spinlock.h"

#define COUNT_ITERATION 100

typedef struct {
    spinlock_t *lock;
    int *counter;
} thread_storage_t;

void set_cpu(int n) {
    int err;
    cpu_set_t cpuset;
    pthread_t tid = pthread_self();

    CPU_ZERO(&cpuset);
    CPU_SET(n, &cpuset);

    err = pthread_setaffinity_np(tid, sizeof(cpu_set_t), &cpuset);
    if (err) {
        printf("set_cpu [%d %d %d]: pthread_setaffinity failed for cpu %d\n", getpid(), getppid(), gettid(), n);
        return;
    }

    printf("set_cpu [%d %d %d]: set cpu %d\n", getpid(), getppid(), gettid(), n);
}

void *routine1(void *args) {
    set_cpu(2);
    thread_storage_t *storage = (thread_storage_t *) args;
    for (int i = 0; i < COUNT_ITERATION; i++) {
        spinlock_lock(storage->lock);
        (*storage->counter)++;
        usleep(100);
        spinlock_unlock(storage->lock);
        printf("thread1: [%d,%d]\n", i, *storage->counter);
    }

    return NULL;
}

void *routine2(void *args) {
    set_cpu(1);
    thread_storage_t *storage = (thread_storage_t *) args;
    for (int i = 0; i < COUNT_ITERATION; i++) {
        spinlock_lock(storage->lock);
        (*storage->counter)++;
        usleep(100);
        spinlock_unlock(storage->lock);
        printf("thread2: [%d,%d]\n", i, *storage->counter);
    }
    return NULL;
}

int main() {
    spinlock_t lock;
    spinlock_init(&lock);
    int counter = 0;

    thread_storage_t storage = {&lock, &counter};

    pthread_t tid1, tid2;
    int err = pthread_create(&tid1, NULL, routine1, &storage);
    if (err) {
        printf("main [%d %d %d]: pthread_create: %s\n", getpid(), getppid(), gettid(), strerror(err));
        return EXIT_FAILURE;
    }

    err = pthread_create(&tid2, NULL, routine2, &storage);
    if (err) {
        printf("main [%d %d %d]: pthread_create: %s\n", getpid(), getppid(), gettid(), strerror(err));
        return EXIT_FAILURE;
    }

    pthread_join(tid1, NULL);
    pthread_join(tid2, NULL);

    printf("counter %d\n", counter);
    return EXIT_SUCCESS;
}

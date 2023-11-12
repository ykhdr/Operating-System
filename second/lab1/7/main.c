#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>

#include "uthreads/uthread.h"

#define THREADS_SIZE 3

void thread_1(uthread_config_t *config,void *arg) {
    char *str = (char *) arg;
    for (int i = 0; i < 5; i++) {
        printf("hello in thread_1: %s\n", str);
        sleep(1);
        uthread_scheduler(config);
    }
    puts("thread_1 finished");
}

void thread_2(uthread_config_t *config, void *arg) {
    char *str = (char *) arg;
    for (int i = 0; i < 7; i++) {
        printf("hello in thread_2: %s\n", str);
        sleep(1);
        uthread_scheduler(config);
    }
    puts("thread_2 finished");
}

void thread_3(uthread_config_t *config, void *arg) {
    char *str = (char *) arg;
    for (int i = 0; i < 10; i++) {
        printf("hello in thread_3: %s\n", str);
        sleep(1);
        uthread_scheduler(config);
    }
    puts("thread_3 finished");
}


int main() {

    uthread_struct_t *utid[THREADS_SIZE];
    uthread_struct_t main_thread;
    uthread_config_t* config = uthread_init(&main_thread);

    int err = uthread_create(config,&utid[0], thread_1, "Hello from 1");
    if (err == -1) {
        puts("Create 1 failed");
        exit(1);
    }

    err = uthread_create(config, &utid[1], thread_2, "Hello from 2");
    if (err == -1) {
        puts("Create 2 failed");
        exit(1);
    }

    err = uthread_create(config, &utid[2], thread_3, "Hello from 3");
    if (err == -1) {
        puts("Create 3 failed");
        exit(1);
    }

    while (1) {
        uthread_scheduler(config);
        int count = 0;
        for (int i = 0; i < THREADS_SIZE; i++) {
            if ((utid[i])->is_finished) {
                count++;
            } else {
                break;
            }
        }
        if (count == THREADS_SIZE) {
            break;
        }
    }
    puts("main finished");

    uthread_finalize(config);
    return 0;
}

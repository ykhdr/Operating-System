#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>

#include "uthreads/uthread.h"

#define THREADS_SIZE 3

void thread_1(void *arg) {
    char *str = (char *) arg;
    for (int i = 0; i < 5; i++) {
        printf("hello in thread_1: %s\n", str);
        sleep(1);
        uthread_scheduler();
    }
    puts("thread_1 finished");
}

void thread_2(void *arg) {
    char *str = (char *) arg;
    for (int i = 0; i < 7; i++) {
        printf("hello in thread_2: %s\n", str);
        sleep(1);
        uthread_scheduler();
    }
    puts("thread_2 finished");
}

void thread_3(void *arg) {
    char *str = (char *) arg;
    for (int i = 0; i < 10; i++) {
        printf("hello in thread_3: %s\n", str);
        sleep(1);
        uthread_scheduler();
    }
    puts("thread_3 finished");
}


int main() {

    uthread_struct_t *utid[THREADS_SIZE];
    uthread_struct_t main_thread;
    init_uthread(&main_thread);

    int err = uthread_create(&utid[0], thread_1, "Hello from 1");
    if (err == -1) {
        puts("Create 1 failed");
        exit(1);
    }

    err = uthread_create(&utid[1], thread_2, "Hello from 2");
    if (err == -1) {
        puts("Create 2 failed");
        exit(1);
    }

    err = uthread_create(&utid[2], thread_3, "Hello from 3");
    if (err == -1) {
        puts("Create 3 failed");
        exit(1);
    }

    while (1) {
        uthread_scheduler();
        int count = 0;
        for (int i = 0; i < THREADS_SIZE; i++) {
            if ((utid[i])->is_finished) {
                count++;
            }
            else{
                break;
            }
        }
        if (count == THREADS_SIZE) {
            break;
        }
    }
    puts("main finished");
    return 0;
}

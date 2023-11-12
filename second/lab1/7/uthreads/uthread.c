//
// Created by ykhdr on 29.10.23.
//

#include "uthread.h"

#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <sys/mman.h>
#include <fcntl.h>
#include <ucontext.h>
#include <stdlib.h>

#define PAGE 4096
#define STACK_SIZE (PAGE * 8)


uthread_config_t *uthread_init(uthread_struct_t *main_thread) {
    uthread_config_t *config = (uthread_config_t *) malloc(sizeof(uthread_config_t));
    config->uthreads[0] = main_thread;
    config->uthread_count = 1;
    config->uthread_cur = 0;

    return config;
}

void uthread_scheduler(uthread_config_t *config) {
    int err;
    ucontext_t *cur_context, *next_context;

    cur_context = &(config->uthreads[config->uthread_cur]->ucontext);

    config->uthread_cur = (config->uthread_cur + 1) % config->uthread_count;

    next_context = &(config->uthreads[config->uthread_cur]->ucontext);

    err = swapcontext(cur_context, next_context);

    if (err == -1) {
        printf("Bad swap context");
        exit(1);
    }
}


int create_stack(void **stack) {
    *stack = mmap(NULL, STACK_SIZE, PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
    if (*stack == MAP_FAILED) {
        perror("create_stack: failed mmap");
        return -1;
    }

    memset(*stack, 0x00, STACK_SIZE);
    mprotect(stack + PAGE, STACK_SIZE - 2 * PAGE, PROT_READ | PROT_WRITE);
    return EXIT_SUCCESS;
}

void uthread_startup(uthread_config_t *config, void *uthread) {
    uthread_struct_t *thread = (uthread_struct_t *) uthread;
    thread->start_routine(config, thread->arg);
    thread->is_finished = true;
}

int uthread_create(uthread_config_t *config, uthread_struct_t **thread, void *(start_routine), void *arg) {
    if (config->uthread_count == 0) {
        printf("uthread_create: uthread isn't initialized");
        return -1;
    }

    printf("uthread_create: creating thread %d\n", config->uthread_count);
    void *stack;
    int err = create_stack(&stack);
    if (err == -1) {
        printf("uthread_create: failed to create stack");
        return -1;
    }

    uthread_struct_t *mythread = (uthread_struct_t *) (stack + STACK_SIZE - sizeof(uthread_struct_t));

    err = getcontext(&mythread->ucontext);
    if (err == -1) {
        printf("Bad getcontext in thread %d", config->uthread_count);
        return -1;
    }
    mythread->ucontext.uc_stack.ss_sp = stack;
    mythread->ucontext.uc_stack.ss_size = STACK_SIZE - 2 * PAGE;
    mythread->ucontext.uc_link = &(config->uthreads[0]->ucontext);

    makecontext(&mythread->ucontext, (void (*)(void)) uthread_startup, 2,config, mythread);

    mythread->uthread_id = config->uthread_count;
    mythread->start_routine = start_routine;
    mythread->arg = arg;
    mythread->is_finished = false;

    config->uthreads[config->uthread_count] = mythread;
    config->uthread_count++;

    *thread = mythread;
    return EXIT_SUCCESS;
}

void uthread_finalize(uthread_config_t *config) {
    free(config);
}
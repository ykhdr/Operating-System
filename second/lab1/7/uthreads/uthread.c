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
#define MAX_THREADS_COUNT 8

uthread_struct_t *uthreads[MAX_THREADS_COUNT];
int uthread_count = 0;
int uthread_cur = 0;

void init_uthread(uthread_struct_t *main_thread) {
    uthreads[0] = main_thread;
    uthread_count = 1;
}

void uthread_scheduler() {
    int err;
    ucontext_t *cur_context, *next_context;

    cur_context = &(uthreads[uthread_cur]->ucontext);

    uthread_cur = (uthread_cur + 1) % uthread_count;

    next_context = &(uthreads[uthread_cur]->ucontext);

    err = swapcontext(cur_context, next_context);

    if (err == -1) {
        printf("Bad swap context");
        exit(1);
    }
}


int create_stack(void **stack, off_t size, int thread_num) {
    char stack_file[128];
    int stack_fd;
    snprintf(stack_file, sizeof(stack_file), "stack-%d", thread_num);

    stack_fd = open(stack_file, O_RDWR | O_CREAT, 0660);
    if (stack_fd == -1) {
        printf("create_stack: failed to open file");
        return -1;
    }

    int err = ftruncate(stack_fd, 0);
    if (err == -1) {
        printf("create_stack: failed to ftruncate");
        return -1;
    }

    err = ftruncate(stack_fd, size);
    if (err == -1) {
        printf("create_stack: failed to ftruncate");
        return -1;
    }

    *stack = mmap(NULL, size, PROT_WRITE, MAP_SHARED | MAP_STACK, stack_fd, 0);
    if (*stack == MAP_FAILED) {
        printf("create_stack: failed mmap");
        return -1;
    }

    err = close(stack_fd);
    if (err == -1) {
        printf("create_stack: failed mmap");
        return -1;
    }

    memset(*stack, 0x00, size);
    mprotect(stack + PAGE, STACK_SIZE - 2 * PAGE, PROT_READ | PROT_WRITE);
    return EXIT_SUCCESS;
}

void uthread_startup(void *arg) {
    uthread_struct_t *mythread = (uthread_struct_t *) arg;
    mythread->start_routine(mythread->arg);
    mythread->is_finished = true;
}

int uthread_create(uthread_struct_t **thread, void *(start_routine), void *arg) {
    if(uthread_count == 0){
        printf("uthread_create: uthread isn't initialized");
        return -1;
    }

    printf("uthread_create: creating thread %d\n", uthread_count);
    void *stack;
    int err = create_stack(&stack, STACK_SIZE, uthread_count);
    if (err == -1) {
        printf("uthread_create: failed to create stack");
        return -1;
    }
   uthread_struct_t *mythread = (uthread_struct_t *) (stack + STACK_SIZE - sizeof(uthread_struct_t));

    err = getcontext(&mythread->ucontext);
    if (err == -1) {
        printf("Bad getcontext in thread %d", uthread_count);
        return -1;
    }
    mythread->ucontext.uc_stack.ss_sp = stack;
    mythread->ucontext.uc_stack.ss_size = STACK_SIZE - 2 * PAGE;
    mythread->ucontext.uc_link = &uthreads[0]->ucontext;

    makecontext(&mythread->ucontext, (void (*)(void)) uthread_startup, 1, mythread);

    mythread->uthread_id = uthread_count;
    mythread->start_routine = start_routine;
    mythread->arg = arg;
    mythread->is_finished = false;

    uthreads[uthread_count] = mythread;
    uthread_count++;

    *thread = mythread;
    return EXIT_SUCCESS;
}
//
// Created by ykhdr on 29.10.23.
//

#ifndef LAB7_UTHREAD_H
#define LAB7_UTHREAD_H

#include <sys/ucontext.h>
#include <stdbool.h>

typedef struct {
    int uthread_id;
    void (*start_routine)(void *);
    void *arg;
    ucontext_t ucontext;
    bool is_finished;
} uthread_struct_t;

void init_uthread(uthread_struct_t *main_thread);

void uthread_scheduler();

int uthread_create(uthread_struct_t **thread, void *(start_routine), void *arg);

#endif //LAB7_UTHREAD_H

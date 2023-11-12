//
// Created by ykhdr on 29.10.23.
//

#ifndef LAB7_UTHREAD_H
#define LAB7_UTHREAD_H

#include <sys/ucontext.h>
#include <stdbool.h>

#define MAX_THREADS_COUNT 8

struct uthread_config_t;
struct uthread_struct_t;

typedef struct uthread_struct_t{
    int uthread_id;

    void (*start_routine)(struct uthread_config_t *config, void *);

    void *arg;
    ucontext_t ucontext;
    bool is_finished;
} uthread_struct_t;

typedef struct uthread_config_t {
    uthread_struct_t *uthreads[MAX_THREADS_COUNT];
    int uthread_count;
    int uthread_cur;
} uthread_config_t;

uthread_config_t *uthread_init(uthread_struct_t *main_thread);

void uthread_scheduler(uthread_config_t *config);

int uthread_create(uthread_config_t *config, uthread_struct_t **thread, void *(start_routine), void *arg);

void uthread_finalize(uthread_config_t *config);

#endif //LAB7_UTHREAD_H

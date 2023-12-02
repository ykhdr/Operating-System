#ifndef __FITOS_BLOCKING_QUEUE_H__
#define __FITOS_BLOCKING_QUEUE_H__

#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <sys/types.h>
#include <unistd.h>

typedef struct _QueueNode {
	int val;
	struct _QueueNode *next;
} qnode_t;

typedef struct _Queue {
    qnode_t *first;
    qnode_t *last;

    pthread_t qmonitor_tid;
    pthread_mutex_t lock;
    pthread_cond_t not_empty;
    pthread_cond_t not_full;

    int count;
    int max_count;

    // queue statistics
    long add_attempts;
    long get_attempts;
    long add_count;
    long get_count;
} blocking_queue_t;

blocking_queue_t* blocking_queue_init(int max_count);
void blocking_queue_destroy(blocking_queue_t **q);
int blocking_queue_add(blocking_queue_t *q, int val);
int blocking_queue_get(blocking_queue_t *q, int *val);
void blocking_queue_print_stats(blocking_queue_t *q);

#endif        // __FITOS_BLOCKING_QUEUE_H__

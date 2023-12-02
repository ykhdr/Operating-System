#include <pthread.h>
#include <semaphore.h>
#include <assert.h>

#include "blocking-queue.h"

void *qmonitor(void *arg) {
    blocking_queue_t *q = (blocking_queue_t *)arg;

    printf("qmonitor: [%d %d %d]\n", getpid(), getppid(), gettid());

    while (1) {
        blocking_queue_print_stats(q);
        sleep(1);
    }

    return NULL;
}

blocking_queue_t* blocking_queue_init(int max_count) {
    int err;

    blocking_queue_t *q = malloc(sizeof(blocking_queue_t));
    if (!q) {
        printf("Cannot allocate memory for a queue\n");
        abort();
    }

    q->first = NULL;
    q->last = NULL;
    q->max_count = max_count;
    q->count = 0;

    q->add_attempts = q->get_attempts = 0;
    q->add_count = q->get_count = 0;

    sem_init(&q->empty, 0, max_count);
    sem_init(&q->full, 0, 0);

    if (pthread_mutex_init(&q->lock, NULL) != 0) {
        printf("Failed to initialize the mutex\n");
        abort();
    }

    err = pthread_create(&q->qmonitor_tid, NULL, qmonitor, q);
    if (err) {
        printf("blocking_queue_init: pthread_create() failed: %s\n", strerror(err));
        abort();
    }

    return q;
}

void blocking_queue_destroy(blocking_queue_t **q) {
    pthread_mutex_lock(&(*q)->lock);
    if (*q == NULL) {
        return;
    }

    sem_destroy(&(*q)->empty);
    sem_destroy(&(*q)->full);

    pthread_cancel((*q)->qmonitor_tid);
    pthread_join((*q)->qmonitor_tid, NULL);

    while ((*q)->first) {
        qnode_t* tmp = (*q)->first;
        (*q)->first = (*q)->first->next;
        free(tmp);
    }

    pthread_mutex_t lock = (*q)->lock;
    pthread_mutex_destroy(&(*q)->lock);

    free(*q);
    *q = NULL;

    pthread_mutex_unlock(&lock);
}

int blocking_queue_add(blocking_queue_t *q, int val) {
    q->add_attempts++;

    assert(q->count <= q->max_count);

    sem_wait(&q->empty);

    qnode_t *new = malloc(sizeof(qnode_t));
    if (!new) {
        printf("Cannot allocate memory for a new node\n");
        abort();
    }

    new->val = val;
    new->next = NULL;

    pthread_mutex_lock(&q->lock);

    if (!q->first)
        q->first = q->last = new;
    else {
        q->last->next = new;
        q->last = q->last->next;
    }

    q->count++;
    q->add_count++;

    sem_post(&q->full);

    pthread_mutex_unlock(&q->lock);

    return 1;
}

int blocking_queue_get(blocking_queue_t *q, int *val) {
    q->get_attempts++;

    assert(q->count >= 0);

    sem_wait(&q->full);

    pthread_mutex_lock(&q->lock);

    qnode_t *tmp = q->first;

    *val = tmp->val;
    q->first = q->first->next;

    free(tmp);
    q->count--;
    q->get_count++;

    sem_post(&q->empty);

    pthread_mutex_unlock(&q->lock);

    return 1;
}

void blocking_queue_print_stats(blocking_queue_t *q) {
    printf("queue stats: current size %d; attempts: (%ld %ld %ld); counts (%ld %ld %ld)\n",
           q->count,
           q->add_attempts, q->get_attempts, q->add_attempts - q->get_attempts,
           q->add_count, q->get_count, q->add_count - q->get_count);
}

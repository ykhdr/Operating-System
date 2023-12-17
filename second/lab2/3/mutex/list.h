#ifndef __LAB2__
#define __LAB2__

#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <string.h>

#define MAX_STRING_LENGTH 100

typedef struct _Node {
    char value[MAX_STRING_LENGTH];

    struct _Node *next;
    pthread_mutex_t sync;
} node_t;

typedef struct _Storage {
    node_t *first;
    int capacity;
} storage_t;

typedef struct _ThreadData {
    storage_t *storage;
    int *counter;
    char *name;
} thread_data_t;


storage_t* initialize_storage(int capacity);

void add_node(storage_t *storage, const char *value);

void fill_storage(storage_t *storage);

void print_storage(const storage_t *storage);

#endif// __LAB2__

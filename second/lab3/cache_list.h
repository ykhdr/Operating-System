#ifndef LAB3_PROXY_CACHE_LIST_H
#define LAB3_PROXY_CACHE_LIST_H

#include <pthread.h>
#include <stdlib.h>
#include "logger.h"

#define CACHE_BUFFER_SIZE (1024 * 1024 * 64*64) // 64 MB

typedef struct cache {
    unsigned long request;
    char *response;
    ssize_t response_len;
    pthread_mutex_t *lock;
    struct cache *next;
} Cache;

int init_cache_record(Cache *record, pthread_mutex_t *lock);
ssize_t find_in_cache(Cache *start, char *req, char *copy);
void add_request(Cache *record, char *req);
void add_response(Cache *record, char *resp, unsigned long cur_position, unsigned long resp_size);
void add_size(Cache *record, ssize_t size);
void delete_cache_record(Cache *record);
Cache *push_record(Cache *start, Cache *record);


#endif //LAB3_PROXY_CACHE_LIST_H

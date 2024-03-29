#include <unistd.h>
#include <pthread.h>
#include "list.h"

#define STORAGE_CAPACITY 1000
#define THREAD_COUNT 6
#define ASC 0
#define DESC 1
#define EQ 2
#define SWAP1 3
#define SWAP2 4
#define SWAP3 5


void* ascending_thread(void* data) {
    thread_data_t* thread_data = (thread_data_t *)data;
    storage_t* storage = thread_data->storage;
    int* counter = thread_data->counter;

    while (1) {
        node_t* curr = storage->first;
        if (curr == NULL || curr->next == NULL) {
            printf("Too few elements in queue to asc\n");
            break;
        }
        node_t *curr2, *tmp;
        int pair_count = 0;
        while (1) {
            if (curr != NULL && pthread_rwlock_tryrdlock(&curr->sync) == 0) {
                if (curr->next != NULL && pthread_rwlock_tryrdlock(&curr->next->sync) == 0) {
                    curr2 = curr->next;
                    if (strlen(curr->value) < strlen(curr2->value)) {
                        pair_count++;
                    }
                    tmp = curr;
                    curr = curr->next;
                    pthread_rwlock_unlock(&tmp->sync);
                    pthread_rwlock_unlock(&curr->sync);
                }
                else {
                    tmp = curr;
                    curr = curr->next;
                    pthread_rwlock_unlock(&tmp->sync);
                }
            }
            else if (curr == NULL) {
                break;
            }
            else {
                curr = curr->next;
            }
        }
        printf("[%s] : %d\n",thread_data->name, pair_count);
        (*counter)++;
    }
    return NULL;
}

void* descending_thread(void* data) {
    thread_data_t* thread_data = (thread_data_t *)data;
    storage_t* storage = thread_data->storage;
    int* counter = thread_data->counter;
    if (storage->first == NULL || storage->first->next == NULL) {
        printf("Too few elements in queue to desc\n");
        return NULL;
    }
    while (1) {
        node_t* curr = storage->first;
        node_t *curr2, *tmp;
        int pair_count = 0;
        while (1) {
            if (curr != NULL && pthread_rwlock_tryrdlock(&curr->sync) == 0) {
                if (curr->next != NULL && pthread_rwlock_tryrdlock(&curr->next->sync) == 0) {
                    curr2 = curr->next;
                    if (strlen(curr->value) > strlen(curr2->value)) {
                        pair_count++;
                    }
                    tmp = curr;
                    curr = curr->next;
                    pthread_rwlock_unlock(&tmp->sync);
                    pthread_rwlock_unlock(&curr->sync);
                }
                else {
                    tmp = curr;
                    curr = curr->next;
                    pthread_rwlock_unlock(&tmp->sync);
                }
            }
            else if (curr == NULL) {
                break;
            }
            else {
                curr = curr->next;
            }
        }
        printf("[%s] : %d\n",thread_data->name, pair_count);
        (*counter)++;
    }
    return NULL;
}

void* equal_length_thread(void* data) {
    thread_data_t* thread_data = (thread_data_t *)data;
    storage_t* storage = thread_data->storage;
    int* counter = thread_data->counter;
    if (storage->first == NULL || storage->first->next == NULL) {
        printf("Too few elements in queue to asc\n");
        return NULL;
    }

    while (1) {
        node_t* curr = storage->first;
        node_t *curr2, *tmp;
        int pair_count = 0;
        while (1) {
            if (curr != NULL && pthread_rwlock_tryrdlock(&curr->sync) == 0) {
                if (curr->next != NULL && pthread_rwlock_tryrdlock(&curr->next->sync) == 0) {

                    curr2 = curr->next;
                    if (strlen(curr->value) == strlen(curr2->value)) {
                        pair_count++;
                    }
                    tmp = curr;
                    curr = curr->next;
                    pthread_rwlock_unlock(&tmp->sync);
                    pthread_rwlock_unlock(&curr->sync);
                }
                else {
                    tmp = curr;
                    curr = curr->next;
                    pthread_rwlock_unlock(&tmp->sync);
                }
            }
            else if (curr == NULL) {
                break;
            }
            else {
                curr = curr->next;
            }
        }
        printf("[%s] : %d\n",thread_data->name, pair_count);
        (*counter)++;
    }
    return NULL;
}


void* swap_thread(void* data) {
    thread_data_t* thread_data = (thread_data_t *)data;
    storage_t* storage = thread_data->storage;
    int* counter = thread_data->counter;
    while (1) {
        node_t* curr1 = storage->first;
        if (curr1 == NULL || curr1->next == NULL || curr1->next->next == NULL) {
            printf("Too few elements in queue to swap\n");
            break;
        }
        node_t *curr2, *curr3, *tmp;
        int swap_count = 0;
        while (1) {
            int isSwap = rand() % 2 == 0;

            if (curr1 != NULL && pthread_rwlock_trywrlock(&curr1->sync) == 0) {
                if (curr1->next != NULL && pthread_rwlock_trywrlock(&curr1->next->sync) == 0) {
                    if (curr1->next->next != NULL && pthread_rwlock_trywrlock(&curr1->next->next->sync) == 0) {
                        curr2 = curr1->next;
                        curr3 = curr1->next->next;
                        if (isSwap) {
                            curr2->next = curr3->next;
                            curr3->next = curr2;
                            curr1->next = curr3;
                            (*counter)++;
                            swap_count++;
                        }
                        tmp = curr1;
                        curr1 = tmp->next;
                        curr2 = curr1->next;
                        pthread_rwlock_unlock(&tmp->sync);
                        pthread_rwlock_unlock(&curr1->sync);
                        pthread_rwlock_unlock(&curr2->sync);
                    }
                    else {
                        tmp = curr1;
                        curr1 = curr1->next;
                        pthread_rwlock_unlock(&tmp->sync);
                        pthread_rwlock_unlock(&curr1->sync);
                    }
                }
                else {
                    tmp = curr1;
                    curr1 = curr1->next;
                    pthread_rwlock_unlock(&tmp->sync);
                }
            }
            else if (curr1 == NULL) {
                break;
            }
            else {
                curr1 = curr1->next;
            }
        }
        printf("[%s] : %d\n",thread_data->name, swap_count);
    }

    return NULL;
}


void* count_monitor(void* arg) {
    int* counters = (int *)arg;
    int iteration = 0;
    while (1) {
        printf("[%d] ASC: %d, DESC: %d, EQ: %d, SWAP1: %d, SWAP2: %d, SWAP3: %d\n",
               iteration, counters[ASC], counters[DESC], counters[EQ], counters[SWAP1], counters[SWAP2], counters[SWAP3]);
        iteration++;
        usleep(1000);
    }
    return NULL;
}

int main() {
    storage_t* storage = initialize_storage(STORAGE_CAPACITY);
    fill_storage(storage);
    print_storage(storage);

    pthread_t ascending_tid, descending_tid, equal_length_tid, swap_tid1, swap_tid2, swap_tid3, monitor;

    int* counters = calloc(THREAD_COUNT, sizeof(int));

    thread_data_t ascending_data = {storage, &counters[ASC], "ASC"};
    thread_data_t descending_data = {storage, &counters[DESC], "DESC"};
    thread_data_t equal_data = {storage, &counters[EQ], "EQ"};
    thread_data_t swap_data1 = {storage, &counters[SWAP1], "SWAP1"};
    thread_data_t swap_data2 = {storage, &counters[SWAP2], "SWAP2"};
    thread_data_t swap_data3 = {storage, &counters[SWAP3], "SWAP3"};

    pthread_create(&ascending_tid, NULL, ascending_thread, &ascending_data);
    pthread_create(&descending_tid, NULL, descending_thread, &descending_data);
    pthread_create(&equal_length_tid, NULL, equal_length_thread, &equal_data);
    pthread_create(&swap_tid1, NULL, swap_thread, &swap_data1);
    pthread_create(&swap_tid2, NULL, swap_thread, &swap_data2);
    pthread_create(&swap_tid3, NULL, swap_thread, &swap_data3);
    pthread_create(&monitor, NULL, count_monitor, counters);

    pthread_join(ascending_tid, NULL);
    pthread_join(descending_tid, NULL);
    pthread_join(equal_length_tid, NULL);
    pthread_join(monitor, NULL);
    pthread_join(swap_tid1, NULL);
    pthread_join(swap_tid2, NULL);
    pthread_join(swap_tid3, NULL);

    return 0;
}

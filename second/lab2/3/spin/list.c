#include "list.h"


storage_t *initialize_storage(const int capacity) {
    storage_t *storage = malloc(sizeof(storage_t));
    if (!storage) {
        printf("Failed to allocate memory for a queue\n");
        abort();
    }
    storage->capacity = capacity;
    storage->first = NULL;
    return storage;
}

void add_node(storage_t *storage, const char *value) {
    node_t *new_node = malloc(sizeof(node_t));

    if (!new_node) {
        perror("Failed to allocate memory for a new node");
        exit(EXIT_FAILURE);
    }
    if (storage->first != NULL) {
        node_t *node = storage->first;
        while (node->next != NULL) {
            node = node->next;
        }
        node->next = new_node;
    } else {
        storage->first = new_node;
    }
    strcpy(new_node->value, value);
    new_node->next = NULL;
    pthread_spin_init(&new_node->sync, PTHREAD_PROCESS_PRIVATE);
}

void fill_storage(storage_t *storage) {
    for (int i = 0; i < storage->capacity; ++i) {
//        char buff[100];


        int len = rand() % 100;
        char *buff = malloc(sizeof(char)*len);
        
        for(int j = 0; j < len; j++){
            buff[j] = i % 10 + '0';
        }

        add_node(storage, buff);
    }
}

void print_storage(const storage_t *storage) {
    node_t *current = storage->first;
    while (current != NULL) {
        printf("%s ", current->value);
        current = current->next;
    }
    printf("\n");
}

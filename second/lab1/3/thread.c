#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <string.h>
#include <errno.h>
#include <sys/types.h>
#include <unistd.h>


struct field_t{
    int number;
    char* str;
};


void *thread(void* arg){
    struct field_t* my_field = (struct field_t*) arg;
    printf("Number: %d\nString: %s\n",my_field->number,my_field->str);
}

int main(){
    pthread_t tid;
    int err;
    
    struct field_t *my_field = (struct field_t*)malloc(sizeof(struct field_t));

    my_field->number = 23;
    my_field->str = "Hello world!";

    pthread_attr_t attr;
    pthread_attr_init(&attr);
    pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED);

    err = pthread_create(&tid, &attr, thread, my_field);
    if (err) {
        printf("main: pthread_create() failed: %s\n", strerror(err));
        return -1;
    }

    pthread_join(tid, NULL);
    free(my_field);

    return 0;
}

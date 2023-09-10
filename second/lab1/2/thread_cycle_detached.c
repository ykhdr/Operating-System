#define _GNU_SOURCE
#include <stdio.h>
#include <pthread.h>
#include <string.h>
#include <errno.h>
#include <sys/types.h>
#include <unistd.h>

void *print_thread(void *arg){
    printf("tid: %d\n", gettid());
    pthread_exit(NULL);
}

int main(){
    pthread_t tid;
    pthread_attr_t attr;
    int err;

    pthread_attr_init(&attr);
    pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED);

    while(1){
        err = pthread_create(&tid, &attr, print_thread, NULL);
       // pthread_join(tid,NULL);
    }

    return 0;
}


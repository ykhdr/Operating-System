#define _GNU_SOURCE
#include <stdio.h>
#include <pthread.h>
#include <string.h>
#include <errno.h>
#include <sys/types.h>
#include <unistd.h>

void *print_thread(void *arg){
    printf("tid: %d\n", pthread_self());
  //  pthread_detach(pthread_self());
    pthread_exit(NULL);
}

int main(){
    pthread_t tid;
    int err;

    while(1){
        err = pthread_create(&tid, NULL, print_thread,NULL);
       // pthread_join(tid,NULL);
    }
    
    return 0;
}

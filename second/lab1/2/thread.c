#define _GNU_SOURCE
#include <stdio.h>
#include <pthread.h>
#include <string.h>
#include <errno.h>
#include <sys/types.h>
#include <unistd.h>

void *number_thread(void *arg){
	static int number = 42;
	pthread_exit(&number);
}

void *hello_thread(void *arg){
    pthread_exit(&"hello world");
}

int main(){
    pthread_t tid;
    int err;

    err = pthread_create(&tid, NULL, number_thread, NULL);
    if (err) {
        printf("main: pthread_create() failed: %s\n", strerror(err));
        return -1;
    }


    int* number_thread_result = 0;

    pthread_join(tid, &number_thread_result);
    printf("Number thread result: %d\n", * number_thread_result);

    ////////////////

    err = pthread_create(&tid, NULL, hello_thread, NULL);
    if (err) {
        printf("main: pthread_create() failed: %s\n", strerror(err));
        return -1;
    }   

    char* hello_thread_result = 0;
    
    pthread_join(tid, &hello_thread_result);
    printf("hello thread result: %s\n", hello_thread_result);
}

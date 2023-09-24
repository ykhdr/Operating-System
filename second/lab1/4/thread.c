#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <string.h>
#include <errno.h>
#include <sys/types.h>
#include <unistd.h>



void *thread(void* arg){
	while(1){
		printf("Hello world!\n");;
		//pthread_testcancel();
	}
	return NULL;
}

void *thread_increment(void* arg){
	int i = 0;
    while(1){
		i++;
		pthread_testcancel();		
    }
}

void my_free(void* arg){
	free(arg);
	printf("Successful cleaning memory\n");
}

void *thread_malloc(void* arg){
	char* hello = (char*)malloc(sizeof(char)*13);
	strcpy(hello,"Hello world\n");
	
	pthread_cleanup_push(my_free,hello);
	while(1){
		printf("%s",hello);
	}

	pthread_cleanup_pop(1);
}

int main(){
    pthread_t tid;
    int err;
    
    err = pthread_create(&tid,NULL, thread_malloc, NULL);
    if (err) {
        printf("main: pthread_create() failed: %s\n", strerror(err));
        return -1;
    }
	
    sleep(2);
	pthread_cancel(tid);

    pthread_join(tid, NULL);

	pthread_exit(0);
    return 0;
}

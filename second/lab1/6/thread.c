#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <string.h>
#include <sched.h>
#include <errno.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

#define STACK_SIZE (1024 * 1024)

typedef struct {
    void *(*start_routine)(void *); 
    void *arg;
} thread_data_t;

static int thread_function(void *arg) {
    thread_data_t *data = (thread_data_t *)arg;
    data->start_routine(data->arg);
    return 0;
}

int mythread_create(pthread_t *thread, void *(*start_routine)(void *), void *arg) {
    thread_data_t data;
    data.start_routine = start_routine;
    data.arg = arg;

    void *stack = malloc(STACK_SIZE);
    if (stack == NULL) {
        perror("Error allocating stack");
        return -1;
    }

    *thread = clone(thread_function,
		   	stack + STACK_SIZE,
		   	CLONE_VM | CLONE_FS | CLONE_FILES | CLONE_SIGHAND | CLONE_PARENT_SETTID | CLONE_CHILD_CLEARTID,
		   	&data,
		   	NULL, NULL, NULL);
    
    if (*thread == -1) {
        perror("Error creating thread");
        free(stack);
        return -1;
    }

    return 0;
}

void * thread_runnable(void* arg){
	printf(arg);
}

int main() {
    pthread_t my_thread;
    int value = 42;
	int err;


    err = mythread_create(&my_thread, thread_runnable, (void *)"Hello world!!\n");
      
	if(err == 0){  
		printf("mythread_create succeeded\n");
    } else {
        printf("mythread_create failed\n");
        return 1;
    }

    waitpid(my_thread, NULL, 0);

    return 0;
}

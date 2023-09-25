#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <string.h>
#include <errno.h>
#include <sys/types.h>
#include <unistd.h>
#include <signal.h>


void *thread1(void* arg){
	sigset_t mask;
	sigfillset(&mask);
	pthread_sigmask(SIG_BLOCK, &mask, NULL);
	printf("First thread blocked signals\n");

	sleep(100);
	printf("First thread has finished its work\n");
	return NULL;
}

void handler(int sig){
	printf("Signal received: %d\n",sig);
}

void *thread2(void * arg){
	signal(SIGINT, handler);
    sleep(100);
	return NULL;
}

void *thread3(void * arg){
	int sig;
	
	sigset_t mask;
	sigemptyset(&mask);
	sigaddset(&mask, SIGQUIT);
	pthread_sigmask(SIG_BLOCK,&mask,NULL);
	
	printf("Third thread is waitng a SIGQUIT signal\n");
	
	sigwait(&mask,&sig);
	
	printf("Third thread received SIGQUIT signal: %d\n",sig);

	return NULL;
}


int main(){
    pthread_t tid1; 
    pthread_t tid2;
    pthread_t tid3;

	printf("Pid:%d\n",getpid());

    pthread_create(&tid1, NULL, thread1, NULL);	
    pthread_create(&tid2, NULL, thread2, NULL);
    pthread_create(&tid3, NULL, thread3, NULL);

    sleep(100);


//    pthread_kill(tid1,SIGINT);
//    sleep(2);
//    pthread_kill(tid2,SIGINT);
//    sleep(2);
//    pthread_kill(tid3,SIGQUIT);
//    sleep(2);

    //pthread_join(tid, NULL);
    //pthread_join(tid, NULL);
    //free(my_field);
    pthread_exit(0);
    return 0;
}

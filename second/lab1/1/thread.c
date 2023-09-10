#define _GNU_SOURCE
#include <stdio.h>
#include <pthread.h>
#include <string.h>
#include <errno.h>
#include <sys/types.h>
#include <unistd.h>

int global = 0;

void *mythread(void *arg) {
	printf("\nmythread:\n\tpid: %d\n\tppid: %d\n\ttid: %d\n", getpid(), getppid(), gettid());
	printf("\tpthread_self: %ld\n",pthread_self());

	int local  = 0;
	static int local_static = 0;
    const int local_const = 0;	

	if(gettid()%2==0){
	    local++;
   	    global++;	
	 
	    sleep(10);
	}

    printf("\ntid: %d\nvars:\n\tlocal: %p\n\tstatic local: %p\n\tconst local: %p\n\tglobal: %p\n",gettid(),&local,&local_static,&local_const,&global);

	
	if(gettid()%2 == 1){
	    sleep(5);
            printf("\ntid: %d\nlocal value: %d\n",gettid(),local);
	    printf("global value: %d\n",global);	
	}
	
	
	
	return 0;
}

int main() {
	pthread_t tid1;
	pthread_t tid2;
	pthread_t tid3;
	pthread_t tid4;
	pthread_t tid5;
	int err;

	printf("main [%d %d %d]: Hello from main!\n", getpid(), getppid(), gettid());

    sleep(10);
    err = pthread_create(&tid1, NULL, mythread, NULL);
 	if (err) {
    	printf("main: pthread_create() failed: %s\n", strerror(err));
	    return -1;
    }

	err = pthread_create(&tid2, NULL, mythread, NULL);
	err = pthread_create(&tid3, NULL, mythread, NULL);
	err = pthread_create(&tid4, NULL, mythread, NULL);
	err = pthread_create(&tid5, NULL, mythread, NULL);

	printf("\nthread1 id: %ld\n",tid1);
	printf("thread2 id: %ld\n",tid2);
	printf("thread3 id: %ld\n",tid3);
	printf("thread4 id: %ld\n",tid4);
	printf("thread5 id: %ld\n",tid5);
	
	pthread_join(tid1,NULL);
	pthread_join(tid2,NULL);
	pthread_join(tid3,NULL);
	pthread_join(tid4,NULL);
	pthread_join(tid5,NULL);



	return 0;
}


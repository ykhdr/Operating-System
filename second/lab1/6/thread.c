#define _GNU_SOURCE
#include <stdio.h>
#include <sched.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <stdlib.h>
#include <sys/mman.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>


#define PAGE 4096
#define STACK_SIZE PAGE*8

typedef void *(*start_routine_t)(void *);

typedef struct _mythread {
    int                 mythread_id;
    start_routine_t     start_routine;
    void*               arg;
    void*               retval;
    volatile int        joined;          
    volatile int        finished;   
} mythread_struct_t;

typedef mythread_struct_t* mythread_t;

void* create_stack(size_t size, int tid){
    char stack_file[128];
    int stack_fd;
    void *stack;

    snprintf(stack_file, sizeof(stack_file),"stack-%d",tid);

    stack_fd = open(stack_file, O_RDWR | O_CREAT, 0660);
    ftruncate(stack_fd,0);
    ftruncate(stack_fd,size);

    stack = mmap(NULL,size,PROT_READ|PROT_WRITE, MAP_SHARED, stack_fd, 0);
    
    mprotect(stack+PAGE, STACK_SIZE - PAGE, PROT_READ|PROT_WRITE);
    memset(stack+PAGE,0x7f,STACK_SIZE-PAGE);
    
    close(stack_fd);

    return stack;
}


int mythread_startup(void *arg){
    mythread_t tid = (mythread_t) arg;
    mythread_struct_t *mythread = tid;

    printf("thread_startup: start thread function for thread %d\n",mythread->mythread_id);

    mythread->retval = mythread->start_routine(mythread->arg);

    mythread->finished = 1;

    printf("thread_startup: waiting for join() the thread %d\n",mythread->mythread_id);
    while(mythread->joined){
        usleep(1);
    }

    printf("thread_startup: the thread function finished for the thread %d\n",mythread->mythread_id);

    return 0;
}

int mythread_create(mythread_t *mytid, start_routine_t start_routine, void* arg){
    static int tid = 0;
    mythread_struct_t *mythread;
    void* child_stack;
    int child_pid;

    tid++;
    
    printf("mythread_create: creating thread %d\n", tid);

    child_stack = create_stack(STACK_SIZE, tid);
     
    mythread = (mythread_struct_t*)(child_stack + STACK_SIZE - sizeof(mythread_struct_t));
    mythread->mythread_id = tid;
    mythread->start_routine = start_routine;
    mythread->arg = arg; 
    mythread->retval = NULL;
    mythread->finished = 0;
    mythread->joined = 0;


    child_stack = (void*)mythread;

    printf("child stack %p; mythread_struct %p; \n", child_stack, mythread);

    child_pid =  clone(mythread_startup,child_stack, CLONE_VM | CLONE_FILES | CLONE_THREAD | CLONE_SIGHAND, (void*)mythread);
    if(child_pid == -1){
        fprintf(stderr,"clone failed: %s\n",strerror(errno));
        return -1;
    }

    *mytid = mythread;

    return 0;
}

void mythread_join(mythread_t mytid, void** retval){
    mythread_struct_t *mythread = mytid;

    printf("mythread_join: waiting for the therad %d to finish\n", mythread->mythread_id);

    while(!mythread->finished){
        usleep(1);
    }

    printf("mythread_join: the thread %d finished\n", mythread->mythread_id);

    *retval = mythread->retval;
    
    mythread->joined = 1;
}

void* mythread(void *arg){
    char* str = (char*) arg;
    for(int i = 0; i < 5; i++){
        printf("hello: '%s'\n", str);
        sleep(1);
    }

    return "retval!";
}

int main(){
    mythread_t tid1;
    mythread_t tid2;
     
    void* retval1;
    void* retval2;
    
    mythread_create(&tid1,mythread, "hello from thread1");
    sleep(2);
    mythread_create(&tid2,mythread, "hello from thread2");
    
    mythread_join(tid1,&retval1);

    printf("main: thread1 returned value : %s\n", (char*) retval1);

    mythread_join(tid2,&retval2);

    printf("main: thread2 returned value : %s\n", (char*) retval2);
}

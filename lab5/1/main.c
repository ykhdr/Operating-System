#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <wait.h>

int globalVar = 1000;

int main() {
    
    int localVar = 1500;
    
    printf("[P] Address of local variable: %p, with value: %d.\n", &localVar, localVar);
    printf("[P] Address of global variable: %p, with value: %d.\n", &globalVar, globalVar);
    printf("[P] Pid: %d.\n", getpid());
    
    int pid = fork();
    
    if (pid == 0) {
    	printf("[C] Child pid from child: %d.\n", getpid());
    	printf("[C] Parent pid from child: %d.\n", getppid());
    	printf("[C] Address of global variable: %p, with value: %d.\n", &globalVar, globalVar);
    	printf("[C] Address of local variable: %p, with value: %d.\n", &localVar, localVar);

    	globalVar = 10;
    	localVar = 15;

    	printf("[C] New value of global variable: %d.\n", globalVar);
    	printf("[C] New value of local variable: %d.\n", localVar);

	sleep(5);
    	exit(5);
    	
    } else if (pid > 0) {
    	printf("[P] Address of local variable: %p, with value: %d.\n", &localVar, localVar);
    	printf("[P] Address of global variable: %p, with value: %d.\n", &globalVar, globalVar);

    	sleep(30);
    	
    	int status;
    	wait(&status);
    	
    	if (WIFEXITED(status)) {
    		printf("[P] Child process exited with code %d.\n", WEXITSTATUS(status));
    	} else {
    		printf("[P] Child process didn't exit normally.\n");
    	}
    } else {
        printf("Invalid fork.\n");
    }
    return 0;
}

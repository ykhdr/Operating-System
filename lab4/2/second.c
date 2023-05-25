#include <stdio.h>
#include <stdlib.h>
#include <sys/mman.h>
#include <unistd.h>
#include <signal.h>

#define SUCCESS 0
#define ERROR (-1)
#define PAGE_SIZE 4096

void allocate_on_stack();
void allocate_on_heap();
void allocate_address_region();
void sigsegv_handler(int signal);

int main(int argc, char **argv) {
    printf("PID: %d\n", getpid());
    sleep(20);

//    allocate_on_stack();
//    allocate_on_heap();
    allocate_address_region();

    return EXIT_SUCCESS;
}

void allocate_on_stack() {
    char buff[PAGE_SIZE * 50];
    printf("Allocation: %p\n", &buff[0]);
    sleep(1);

    allocate_on_stack();
}

void allocate_on_heap() {
    while (1) {
        char *buff = malloc(PAGE_SIZE * 10);
        if (buff == NULL) {
            perror("malloc");
            exit(EXIT_FAILURE);
        }

        printf("Allocation: %p\n", &buff[0]);

        sleep(1);
    }
}

void allocate_address_region() {
    int ret = SUCCESS;
    char *addr_region;

    signal(SIGSEGV,sigsegv_handler);

    addr_region = mmap(NULL, 10 * PAGE_SIZE, PROT_READ | PROT_WRITE | PROT_EXEC, MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
    if (addr_region == MAP_FAILED) {
        perror("mmap");
        exit(EXIT_FAILURE);
    }
    printf("Created address region: %p\n", &addr_region[0]);
    sleep(10);

    ret = mprotect(addr_region, 10 * PAGE_SIZE, PROT_NONE);
    if (ret == ERROR) {
        perror("mprotect");
        exit(EXIT_FAILURE);
    }
    printf("Changed memory protection: PROT_NONE\n");
    //*addr_region = 'a';

    sleep(10);

    ret = munmap(addr_region + 3 * PAGE_SIZE, 2 * PAGE_SIZE);
    if (ret == ERROR) {
        perror("munmap");
        exit(EXIT_FAILURE);
    }
    printf("Deallocated address region\n");
    sleep(10);
}

void sigsegv_handler(int signal){
    printf("Received SIGSEGV signal\n");
    exit(signal);
}

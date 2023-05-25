#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#define SUCCESS 0
#define ERROR (-1)

int global_not_inited_var;
int global_inited_var = 1;
const int global_const_var = 2;

int* init_local_var();

void init_buffer_in_heap();

void change_env_var();

int main() {
    int local_var;
    static int static_var;
    const int local_const_var = 1;

    printf("pid: %d\n\n", getpid());

    printf("global_not_inited_var:  %p, %d\n", &global_not_inited_var, global_not_inited_var);
    printf("global_inited_var:      %p, %d\n", &global_inited_var, global_inited_var);
    printf("global_const_var:       %p, %d\n", &global_const_var, global_const_var);
    printf("local_var:              %p, %d\n", &local_var, local_var);
    printf("static_var:             %p, %d\n", &static_var, static_var);
    printf("local_const_var:        %p, %d\n", &local_const_var, local_const_var);

    //sleep(20);

    int *local_var_from_fun = init_local_var();
    printf("local_var_from_fun:     %p\n",local_var_from_fun);

    init_buffer_in_heap();

    change_env_var();

//    sleep(60);

    return EXIT_SUCCESS;
}

int *init_local_var() {
    int local_inited_var = 4;
    printf("local_inited_var:       %p, %d\n", &local_inited_var, local_inited_var);
    return &local_inited_var;
}

void init_buffer_in_heap() {
    int buffer_size = 100;
    char *local_buffer = malloc(buffer_size * sizeof(char));
    if (local_buffer == NULL) {
        perror("malloc");
        exit(EXIT_FAILURE);
    }

    strcpy(local_buffer, "Hello, World!");
    printf("local_buffer:           %s, %p, %p\n", local_buffer, &local_buffer, &local_buffer[50]);

    free(local_buffer);
    printf("local_buffer:           %s, %p, %p\n", local_buffer, &local_buffer, &local_buffer[50]);

    local_buffer = malloc(buffer_size * sizeof(char));
    if (local_buffer == NULL) {
        perror("malloc");
        exit(EXIT_FAILURE);
    }

    strcpy(local_buffer, "Hello, World!");
    printf("local_buffer:           %s, %p, %p\n", local_buffer, &local_buffer, &local_buffer[50]);

   // local_buffer += 50;
    free(local_buffer);
    printf("local_buffer:           %s, %p, %p\n", local_buffer, &local_buffer, &local_buffer[50]);
}

void change_env_var() {
    int ret = SUCCESS;
    char *env_var_name = "ENV_VAR";
    char *env_var_value;
    char *new_env_var_value = "new value";

    env_var_value = getenv(env_var_name);
    printf("env_var:                %s\n", env_var_value);

    ret = setenv(env_var_name, new_env_var_value, true);
    if (ret == ERROR) {
        perror("setenv");
        exit(EXIT_FAILURE);
    }

    env_var_value = getenv(env_var_name);
    printf("env_var:                %s\n", env_var_value);
}

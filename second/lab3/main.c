#include <stdio.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <unistd.h>
#include <string.h>
#include <pthread.h>
#include <netdb.h>
#include <signal.h>
#include <arpa/inet.h>
#include <semaphore.h>
#include "logger.h"

#define FAIL (-1)
#define PORT 80
#define MAX_USERS_COUNT 15
#define BUFFER_SIZE 2048

typedef struct {
    int client_socket;
    char *request;
} context;

int server_is_on = 1;

void sigint_handler(int signo) {
    if (signo == SIGINT) {
        logg("Shutting down the server", BLUE);
        server_is_on = 0;
    }
}

int create_server_socket() {
    struct sockaddr_in server_addr;
    int server_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (server_socket == -1) {
        logg("Error while creating server socket", RED);
        return FAIL;
    }

    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_UNSPEC;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(PORT);
    logg("Server socket created", GREEN);

    int err = bind(server_socket, (struct sockaddr *) &server_addr, sizeof(server_addr));
    if (err == FAIL) {
        logg("Failed to bind server socket", RED);
        close(server_socket);
        return FAIL;
    }

    logg_int("Server socket bound to ", server_addr.sin_addr.s_addr, GREEN);

    err = listen(server_socket, MAX_USERS_COUNT);
    if (err == FAIL) {
        logg("Server socket failed to listen", RED);
        close(server_socket);
        return FAIL;
    }
    return server_socket;
}

int read_request(int client_socket, char *request) {
    ssize_t bytes_read = read(client_socket, request, BUFFER_SIZE);
    if (bytes_read < 0) {
        logg("Error while read request", RED);
        close(client_socket);
        return EXIT_FAILURE;
    }
    if (bytes_read == 0) {
        logg("Connection closed from client", RED);
        close(client_socket);
        return EXIT_FAILURE;
    }
    request[bytes_read] = '\0';
    logg_char("Received request:\n", request, GREEN);
    return EXIT_SUCCESS;
}

int connect_to_remote(char *host) {
    struct addrinfo hints, *res0;
    memset(&hints, 0, sizeof hints);
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;

    int status = getaddrinfo((char *) host, "http", &hints, &res0);
    if (status != 0) {
        logg("getaddrinAfo error", RED);
        return FAIL;
    }

    int dest_socket = socket(res0->ai_family, res0->ai_socktype, res0->ai_protocol);
    if (dest_socket == FAIL) {
        logg("Error while creating remote server socket", RED);
        return FAIL;
    }

    int err = connect(dest_socket, res0->ai_addr, res0->ai_addrlen);
    if (err == FAIL) {
        logg("Error while connecting to remote server", RED);
        close(dest_socket);
        return FAIL;
    }

    return dest_socket;
}

void parse_url(char *request, char *url) {
    char *url_tmp = strstr(request, "http");

    int i = 0;
    while (url_tmp[i] != ' ') {
        url[i] = url_tmp[i];
        i++;
    }
    url[i] = '\0';
}

void parse_request(char *request, int *request_len, char *parsed_request, char *serv_hostname, char *url) {
    parse_url(request, url);
    int i = 0;
    char method[10] = {0};
    while (request[i] != ' ') {
        method[i] = request[i];
        i++;
    }

    strncpy(parsed_request, method, strlen(method));
    strcat(parsed_request, " ");

    int host_len = 0;
    i += 8;
    while (request[i] != '/') {
        serv_hostname[host_len] = request[i];
        host_len++;
        i++;
    }

    strcat(parsed_request, request + i);
    char *version = strstr(parsed_request, "HTTP/1");
    version[7] = '0';
    serv_hostname[host_len] = '\0';
    *request_len = (int) strlen(parsed_request);
}


void *client_handler(void *arg) {
    context *ctx = (context *) arg;
    int client_socket = ctx->client_socket;
    char request0[BUFFER_SIZE];
    strncpy(request0, ctx->request, strlen(ctx->request));
    free(ctx->request);

    char request[BUFFER_SIZE];
    char host[50];
    char url[200];
    int request_len = 0;
    parse_request(request0, &request_len, request, host, url);
    logg_char("Remote server host name: ", (char *) host, GREEN);

    logg_char("Parsed request:\n", request, GREEN);

    int dest_socket = connect_to_remote((char *) host);
    if (dest_socket == FAIL) {
        close(client_socket);
        free(ctx);
        pthread_exit(NULL);
    }
    logg("Create new connection with remote server", PURPLE);

    ssize_t bytes_sent = write(dest_socket, request, strlen(request));
    if (bytes_sent == FAIL) {
        logg("Error while sending request to remote server", RED);
        close(client_socket);
        close(dest_socket);
        free(ctx);
        pthread_exit(NULL);
    }
    logg_int("Send request to remote server, len = ", bytes_sent, GREEN);

    char *buffer = calloc(BUFFER_SIZE, sizeof(char));
    ssize_t bytes_read = 0;
    while (1) {
        bytes_read = recv(dest_socket, buffer, BUFFER_SIZE, MSG_NOSIGNAL);
        if (bytes_read < 0) {
//            logg("Error while receiving data from dest", RED);
            free(buffer);
            close(client_socket);
            free(ctx);
            pthread_exit(NULL);
        } else if (bytes_read == 0) {
            break;
        }

        bytes_sent = send(client_socket, buffer, bytes_read, MSG_NOSIGNAL);
        if (bytes_sent < 0) {
//            logg("Error while sending data to client", RED);
            free(buffer);
            close(dest_socket);
            free(ctx);
            pthread_exit(NULL);
        }
    }

    close(client_socket);
    close(dest_socket);
    free(buffer);
    free(ctx);

    logg("All data sent to client. Connection closed", BLUE);

    pthread_exit(NULL);
}

int main() {
    logg("SERVER START", BACK_PURP);
    signal(SIGINT, sigint_handler);

    int server_socket = create_server_socket();
    if (server_socket == FAIL) {
        logg("Error to create server socket", RED);
        exit(EXIT_FAILURE);
    }

    logg_int("Server listening on port ", PORT, PURPLE);

    while (1) {
        int client_socket;
        struct sockaddr_in client_addr;
        socklen_t client_addr_size = sizeof(client_addr);
        client_socket = accept(server_socket, (struct sockaddr *) &client_addr, &client_addr_size);

        if (!server_is_on) {
            close(client_socket);
            break;
        }

        if (client_socket == FAIL) {
            logg("Failed to accept", RED);
            close(server_socket);
            exit(EXIT_FAILURE);
        }
        char *buff = calloc(BUFFER_SIZE, sizeof(char));
        sprintf(buff, "Client connected from %s:%d", inet_ntoa(client_addr.sin_addr), ntohs(client_addr.sin_port));
        logg(buff, BLUE);
        free(buff);

        char *request = calloc(BUFFER_SIZE, sizeof(char));
        int err = read_request(client_socket, request);
        if (err == EXIT_FAILURE) {
            logg("Failed to read request", RED);
            free(request);
            close(client_socket);
            continue;
        }

        logg("Init new connection", PURPLE);

        context *ctx = malloc(sizeof (context));
        ctx->request = request;
        ctx->client_socket = client_socket;

        pthread_t handler_thread;
        err = pthread_create(&handler_thread, NULL, &client_handler, ctx);
        if (err == -1) {
            logg("Failed to create thread", RED);
            close(client_socket);
            close(server_socket);
            exit(EXIT_FAILURE);
        }
    }


    close(server_socket);
    exit(EXIT_SUCCESS);
}

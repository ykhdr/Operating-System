#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <arpa/inet.h>

#define SERVER_PORT 8080
#define BUF_SIZE 1024
#define CLIENTS_COUNT 1

void handle_client(struct sockaddr_in *client_addr, int client_sock_fd) {
  char buffer[BUF_SIZE];
  
  printf("Connected with %s:%d\n", inet_ntoa(client_addr->sin_addr), ntohs(client_addr->sin_port));
  
  while (1) {
    memset(buffer, 0, BUF_SIZE);
    size_t count_of_read_bytes = read(client_sock_fd, buffer, BUF_SIZE);
    
    if (count_of_read_bytes == 0) {
      printf("Disconnected from %s:%d\n", inet_ntoa(client_addr->sin_addr), ntohs(client_addr->sin_port));
      break;
    }
    
    if (count_of_read_bytes == -1) {
      perror("read");
      close(client_sock_fd);
      exit(EXIT_FAILURE);
    }
    
    size_t count_of_wrote_bytes = write(client_sock_fd, buffer, strlen(buffer));
    
    printf("Message (%zu bytes from %s:%d): %s\n",
           count_of_wrote_bytes,
           inet_ntoa(client_addr->sin_addr),
           ntohs(client_addr->sin_port),
           buffer);
    
    if (count_of_wrote_bytes == 0) {
      printf("Disconnected from %s:%d\n", inet_ntoa(client_addr->sin_addr), ntohs(client_addr->sin_port));
      break;
    }
    
    if (count_of_wrote_bytes == -1) {
      perror("write");
      close(client_sock_fd);
      exit(EXIT_FAILURE);
    }
  }

  close(client_sock_fd);
}

int main() {
  int server_sock_fd, client_sock_fd;
  struct sockaddr_in server_addr, client_addr;
  
  socklen_t cli_len = sizeof(client_addr);
  server_sock_fd = socket(AF_INET, SOCK_STREAM, 0);
  
  if (server_sock_fd == -1) {
    perror("socket");
    exit(1);
  }
  
  memset(&server_addr, 0, sizeof(server_addr));
  server_addr.sin_family = AF_INET;
  server_addr.sin_addr.s_addr = INADDR_ANY;
  server_addr.sin_port = htons(SERVER_PORT);

  if (bind(server_sock_fd, (struct sockaddr *) &server_addr, sizeof(server_addr)) == -1) {
    perror("bind");
    close(server_sock_fd);
    exit(1);
  }

  if (listen(server_sock_fd, CLIENTS_COUNT) == -1) {
    perror("listen");
    close(server_sock_fd);
    exit(EXIT_FAILURE);
  }

  while (1) {
    client_sock_fd = accept(server_sock_fd, (struct sockaddr *) &client_addr, &cli_len);
    
    if (client_sock_fd == -1) {
      perror("accept");
      continue;
    }
    
    int pid = fork();
    if (pid == 0) {
      break;
    }
    
    close(client_sock_fd);
  }

  close(server_sock_fd);
  handle_client(&client_addr, client_sock_fd);
  close(client_sock_fd);
  return 0;
}

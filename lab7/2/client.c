#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <arpa/inet.h>

#define SERVER_PORT 8080
#define BUF_SIZE 1024

int main() {
  int client_sock_fd;
  struct sockaddr_in server_addr;
  char buffer[BUF_SIZE];
  client_sock_fd = socket(AF_INET, SOCK_STREAM, 0);
  if (client_sock_fd < 0) {
    perror("socket");
    exit(1);
  }
  memset(&server_addr, 0, sizeof(server_addr));
  server_addr.sin_family = AF_INET;
  server_addr.sin_port = htons(SERVER_PORT);
  if (inet_pton(AF_INET, "127.0.0.1", &server_addr.sin_addr) <= 0) {
    perror("inet_pton");
    exit(1);
  }
  if (connect(client_sock_fd, (struct sockaddr *) &server_addr, sizeof(server_addr)) < 0) {
    perror("connect");
    exit(1);
  }
  while (1) {
    printf("Enter message: ");
    memset(buffer, 0, BUF_SIZE);
    fgets(buffer, BUF_SIZE, stdin);
    write(client_sock_fd, buffer, strlen(buffer));
    size_t count_of_wrote_bytes = read(client_sock_fd, buffer, strlen(buffer));
    if (count_of_wrote_bytes == 0) {
      printf("Disconnected from %s:%d\n", inet_ntoa(server_addr.sin_addr), ntohs(server_addr.sin_port));
      break;
    }
    if (count_of_wrote_bytes == -1) {
      perror("write");
      exit(EXIT_FAILURE);
    }
    printf("Echo: %s", buffer);
  }
  close(client_sock_fd);
  return 0;
}

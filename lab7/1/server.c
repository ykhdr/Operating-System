#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>

#define MAX_BUFFER_SIZE 1024
#define SERVER_PORT 12345

int main() {
    int sockfd;
    struct sockaddr_in serverAddr;
    struct sockaddr_in clientAddr;
    char buffer[MAX_BUFFER_SIZE];

    sockfd = socket(AF_INET, SOCK_DGRAM, 0);
    if (sockfd < 0) {
        perror("socket");
        exit(1);
    }

    memset(&serverAddr, 0, sizeof(serverAddr));
    memset(&clientAddr, 0, sizeof(clientAddr));

    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(SERVER_PORT);
    serverAddr.sin_addr.s_addr = htonl(INADDR_ANY);

    if (bind(sockfd, (struct sockaddr *)&serverAddr, sizeof(serverAddr)) < 0) {
        perror("bind");
    	close(sockfd);
    	exit(1);
    }

    printf("UDP server is running and waiting for clients...\n");

    while (1) {
        socklen_t addrLen = sizeof(clientAddr);

        ssize_t recvLen = recvfrom(sockfd, buffer, MAX_BUFFER_SIZE, 0, (struct sockaddr *)&clientAddr, &addrLen);
        if (recvLen < 0) {
            perror("recvfrom");
            close(sockfd);
	    exit(1);
        }

        ssize_t sendLen = sendto(sockfd, buffer, recvLen, 0, (struct sockaddr *)&clientAddr, addrLen);
        if (sendLen < 0) {
            perror("sendto");
            close(sockfd);
	    exit(1);
        }
    }

    close(sockfd);
    return 0;
}

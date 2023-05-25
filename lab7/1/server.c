#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <arpa/inet.h>

#define MAX_BUFFER_SIZE 1024
#define SERVER_PORT 12345

int main() {
    int sockfd;
    struct sockaddr_in serverAddr, clientAddr;
    char buffer[MAX_BUFFER_SIZE];

    // Создание UDP сокета
    sockfd = socket(AF_INET, SOCK_DGRAM, 0);
    if (sockfd < 0) {
        perror("Ошибка при создании сокета");
        exit(1);
    }

    memset(&serverAddr, 0, sizeof(serverAddr));
    memset(&clientAddr, 0, sizeof(clientAddr));

    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(SERVER_PORT);
    serverAddr.sin_addr.s_addr = htonl(INADDR_ANY);

    // Привязка сокета к IP-адресу и порту сервера
    if (bind(sockfd, (struct sockaddr *)&serverAddr, sizeof(serverAddr)) < 0) {
        perror("Ошибка при привязке сокета");
        exit(1);
    }

    printf("UDP сервер запущен и ожидает клиентов...\n");

    while (1) {
        socklen_t addrLen = sizeof(clientAddr);

        // Получение данных от клиента
        ssize_t recvLen = recvfrom(sockfd, buffer, MAX_BUFFER_SIZE, 0, (struct sockaddr *)&clientAddr, &addrLen);
        if (recvLen < 0) {
            perror("Ошибка при получении данных");
            exit(1);
        }

        // Отправка данных обратно клиенту
        ssize_t sendLen = sendto(sockfd, buffer, recvLen, 0, (struct sockaddr *)&clientAddr, addrLen);
        if (sendLen < 0) {
            perror("Ошибка при отправке данных");
            exit(1);
        }
    }

    return 0;
}

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <arpa/inet.h>

#define MAX_BUFFER_SIZE 1024
#define SERVER_IP "127.0.0.1"
#define SERVER_PORT 12345

int main() {
    int sockfd;
    struct sockaddr_in serverAddr;
    char buffer[MAX_BUFFER_SIZE];

    // Создание UDP сокета
    sockfd = socket(AF_INET, SOCK_DGRAM, 0);
    if (sockfd < 0) {
        perror("Ошибка при создании сокета");
        exit(1);
    }

    memset(&serverAddr, 0, sizeof(serverAddr));

    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(SERVER_PORT);
    serverAddr.sin_addr.s_addr = inet_addr(SERVER_IP);

    while (1) {
        printf("Введите сообщение (или 'q' для выхода): ");
        fgets(buffer, MAX_BUFFER_SIZE, stdin);

        // Отправка данных на сервер
        ssize_t sendLen = sendto(sockfd, buffer, strlen(buffer), 0, (struct sockaddr *)&serverAddr, sizeof(serverAddr));
        if (sendLen < 0) {
            perror("Ошибка при отправке данных");
            exit(1);
        }

        // Завершение программы, если введен символ 'q'
        if (buffer[0] == 'q')
            break;

        socklen_t addrLen = sizeof(serverAddr);

        // Получение данных от сервера
        ssize_t recvLen = recvfrom(sockfd, buffer, MAX_BUFFER_SIZE, 0, (struct sockaddr *)&serverAddr, &addrLen);
        if (recvLen < 0) {
            perror("Ошибка при получении данных");
            exit(1);
        }

        // Вывод полученных данных
        printf("Получено от сервера: %s\n", buffer);
    }

    close(sockfd);

    return 0;
}

#include "pop.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>

Pop3Client* createPop3Client(void) {
    Pop3Client* client = malloc(sizeof(Pop3Client));
    if (!client) {
        perror("Allocation failed");
        return NULL;
    }

    client->sock = socket(AF_INET, SOCK_STREAM, 0);
    if (client->sock < 0) {
        perror("Creating socket error");
        free(client);
        return NULL;
    }

    return client;
}

void destroyPop3Client(Pop3Client** client) {
    if (*client) {
        close((*client)->sock);
        free(*client);
        *client = NULL;
    }
}

int connectToPop3Server(Pop3Client* client, const char* server, int port) {
    struct hostent* d_addr = gethostbyname(server);
    if (!d_addr) {
        perror("Resolving host error");
        return -1;
    }

    struct sockaddr_in addr;
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = *((unsigned long*)d_addr->h_addr);
    addr.sin_port = htons(port);

    if (connect(client->sock, (struct sockaddr*)&addr, sizeof(addr)) < 0) {
        perror("Connecting to server error");
        return -1;
    }

    char buf[1024];
    recv(client->sock, buf, sizeof(buf), 0);
    printf("Received: %s\n", buf);

    return 0;
}

int authenticatePop3(Pop3Client* client, const char* username, const char* password) {
    char buf[1024];

    snprintf(buf, sizeof(buf), "USER %s\r\n", username);
    send(client->sock, buf, strlen(buf), 0);
    recv(client->sock, buf, sizeof(buf), 0);
    printf("Received: %s\n", buf);

    snprintf(buf, sizeof(buf), "PASS %s\r\n", password);
    send(client->sock, buf, strlen(buf), 0);
    recv(client->sock, buf, sizeof(buf), 0);
    printf("Received: %s\n", buf);

    if (strstr(buf, "-ERR")) {
        fprintf(stderr, "Authentication failed\n");
        return -1;
    }

    return 0;
}

int retrieveEmail(Pop3Client* client, int emailIndex) {
    char buf[1024];
    snprintf(buf, sizeof(buf), "RETR %d\r\n", emailIndex);
    send(client->sock, buf, strlen(buf), 0);

    printf("Message content:\n");
    while (recv(client->sock, buf, sizeof(buf), 0) > 0) {
        printf("%s", buf);
        if (strstr(buf, "\r\n.\r\n")) break;  // End of message
    }

    return 0;
}

int quitPop3Server(Pop3Client* client) {
    char buf[1024];
    snprintf(buf, sizeof(buf), "QUIT\r\n");
    send(client->sock, buf, strlen(buf), 0);
    recv(client->sock, buf, sizeof(buf), 0);
    printf("Received: %s\n", buf);

    return 0;
}

char** listEmails(Pop3Client* client, int* emailCount) {
    char buf[1024];
    snprintf(buf, sizeof(buf), "LIST\r\n");
    send(client->sock, buf, strlen(buf), 0);

    char response[1024];
    recv(client->sock, response, sizeof(response), 0);
    if (strstr(response, "-ERR")) {
        fprintf(stderr, "LIST command failed: %s\n", response);
        return NULL;
    }

    // Считаем количество писем
    *emailCount = 0;
    char** emailList = malloc(sizeof(char*) * 100); // Предположим максимум 100 писем

    while (1) {
        recv(client->sock, buf, sizeof(buf), 0);
        if (strcmp(buf, ".\r\n") == 0) { // Конец списка
            break;
        }
        emailList[*emailCount] = strdup(buf);
        (*emailCount)++;
    }

    return emailList;
}

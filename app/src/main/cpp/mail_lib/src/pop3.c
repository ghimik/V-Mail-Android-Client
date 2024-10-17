//
// Created by Алексей on 08.10.2024.
//
#include "pop.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>

#define BUFFER_SIZE 1024

Pop3Client* pop3_client_init(const char *server_ip, const char *username, const char *password) {
    Pop3Client *client = (Pop3Client *)malloc(sizeof(Pop3Client));
    client->server_ip = strdup(server_ip);
    client->username = strdup(username);
    client->password = strdup(password);
    client->sockfd = -1;
    return client;
}

static void send_command(int sockfd, const char *command) {
    send(sockfd, command, strlen(command), 0);
}

char* pop3_client_receive_response(Pop3Client *client) {
    char buffer[BUFFER_SIZE];
    int bytes_received;

    bytes_received = recv(client->sockfd, buffer, sizeof(buffer) - 1, 0);
    if (bytes_received < 0) {
        return NULL;
    }

    buffer[bytes_received] = '\0';
    return strdup(buffer);
}

void pop3_client_connect(Pop3Client *client, int port) {
    struct sockaddr_in server_addr;

    client->sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (client->sockfd < 0) {
        perror("Ошибка при создании сокета");
        return;
    }

    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(port);
    inet_pton(AF_INET, client->server_ip, &server_addr.sin_addr);

    if (connect(client->sockfd, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
        perror("Ошибка подключения");
        close(client->sockfd);
        client->sockfd = -1;
        return;
    }

    char* response = pop3_client_receive_response(client);
    free(response);

    send_command(client->sockfd, "USER ");
    send_command(client->sockfd, client->username);
    send_command(client->sockfd, "\r\n");
    free(pop3_client_receive_response(client)); // Освобождаем ответ

    send_command(client->sockfd, "PASS ");
    send_command(client->sockfd, client->password);
    send_command(client->sockfd, "\r\n");
    free(pop3_client_receive_response(client)); // Освобождаем ответ
}

char** pop3_client_list(Pop3Client *client, int *message_count) {
    send_command(client->sockfd, "LIST\r\n");

    char* response = pop3_client_receive_response(client);
    if (!response) {
        *message_count = 0;
        return NULL;
    }

    char* line = strtok(response, "\r\n");
    while (line != NULL) {
        (*message_count)++;
        line = strtok(NULL, "\r\n");
    }

    char** messages = (char**)malloc((*message_count) * sizeof(char*));
    if (!messages) {
        free(response);
        *message_count = 0;
        return NULL;
    }

    line = strtok(response, "\r\n");
    int index = 0;
    while (line != NULL) {
        messages[index] = strdup(line);
        if (!messages[index]) {
            for (int j = 0; j < index; j++) {
                free(messages[j]);
            }
            free(messages);
            free(response);
            *message_count = 0;
            return NULL;
        }
        index++;
        line = strtok(NULL, "\r\n");
    }

    free(response);
    return messages;
}
void pop3_client_disconnect(Pop3Client *client) {
    send_command(client->sockfd, "QUIT\r\n");
    free(pop3_client_receive_response(client));
    close(client->sockfd);
    client->sockfd = -1;
}

void pop3_client_free(Pop3Client *client) {
    if (client) {
        free(client->server_ip);
        free(client->username);
        free(client->password);
        free(client);
    }
}

//
// Created by Алексей on 08.10.2024.
//
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include "smtp.h"


int smtp_client_init(SmtpClient *client, const char *server, int port) {
    client->server = strdup(server);
    if (!client->server) return -1;
    client->port = port;
    return 0;
}

int smtp_client_connect(SmtpClient *client) {
    struct sockaddr_in server_addr;

    client->sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (client->sockfd < 0) {
        return -1;
    }

    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(client->port);

    if (inet_pton(AF_INET, client->server, &server_addr.sin_addr) <= 0) {
        return -1;
    }

    if (connect(client->sockfd, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
        return -1;
    }

    return 0;
}

void smtp_client_send_command(int sockfd, const char *command) {
    send(sockfd, command, strlen(command), 0);
}

char* smtp_client_receive_response(int sockfd) {
    char *response = malloc(1024);
    if (!response) return NULL;

    int len = recv(sockfd, response, 1023, 0);
    if (len < 0) {
        free(response);
        return NULL;
    }

    response[len] = '\0'; // Завершаем строку
    return response;
}

int smtp_client_check_response(const char *response) {
    return response[0] == '2';
}

int smtp_client_helo(SmtpClient *client) {
    smtp_client_send_command(client->sockfd, "HELO localhost\r\n");
    char *response = smtp_client_receive_response(client->sockfd);

    int success = smtp_client_check_response(response);
    free(response);

    return success ? 0 : -1;
}

int smtp_client_mail_from(SmtpClient *client, const char *from) {
    char command[1024];
    snprintf(command, sizeof(command), "MAIL FROM:<%s>\r\n", from);
    smtp_client_send_command(client->sockfd, command);

    char *response = smtp_client_receive_response(client->sockfd);

    int success = smtp_client_check_response(response);
    free(response);

    return success ? 0 : -1;
}

int smtp_client_rcpt_to(SmtpClient *client, const char *to) {
    char command[1024];
    snprintf(command, sizeof(command), "RCPT TO:<%s>\r\n", to);
    smtp_client_send_command(client->sockfd, command);

    char *response = smtp_client_receive_response(client->sockfd);

    int success = smtp_client_check_response(response);
    free(response);

    return success ? 0 : -1;
}

int smtp_client_data(SmtpClient *client) {
    smtp_client_send_command(client->sockfd, "DATA\r\n");

    char *response = smtp_client_receive_response(client->sockfd);

    int success = smtp_client_check_response(response);
    free(response);

    return success ? 0 : -1;
}

int smtp_client_send_message(SmtpClient *client, const char *subject, const char *body) {
    char command[1024];
    snprintf(command, sizeof(command), "Subject: %s\r\n\r\n%s\r\n.\r\n", subject, body);

    smtp_client_send_command(client->sockfd, command);

    char *response = smtp_client_receive_response(client->sockfd);

    int success = smtp_client_check_response(response);
    free(response);

    return success ? 0 : -1;
}

int smtp_client_quit(SmtpClient *client) {
    smtp_client_send_command(client->sockfd, "QUIT\r\n");

    char *response = smtp_client_receive_response(client->sockfd);

    int success = smtp_client_check_response(response);
    free(response);
    return success ? 0 : -1;
}

int smtp_client_send_email(SmtpClient *client, const char *from, const char *to, const char *subject, const char *body) {
    if (smtp_client_helo(client) != 0) return -1;
    if (smtp_client_mail_from(client, from) != 0) return -1;
    if (smtp_client_rcpt_to(client, to) != 0) return -1;
    if (smtp_client_data(client) != 0) return -1;
    if (smtp_client_send_message(client, subject, body) != 0) return -1;
    if (smtp_client_quit(client) != 0) return -1;

    close(client->sockfd);
    return 0;
}

void smtp_client_free(SmtpClient *client) {
    free(client->server);
}

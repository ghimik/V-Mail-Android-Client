#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <unistd.h>
#include "smtp.h"

#define SMTP_SERVER "82.179.140.18"
#define SMTP_PORT 25

SmtpClient* createSmtpClient(void) {
    SmtpClient* client = malloc(sizeof(SmtpClient));
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

void destroySmtpClient(SmtpClient** client) {
    if (*client) {
        close((*client)->sock);
        free(*client);
        *client = NULL;
    }
}

int connectToSmtpServer(SmtpClient* client) {
    struct hostent* d_addr = gethostbyname(SMTP_SERVER);
    if (d_addr == NULL) {
        perror("Getting host error");
        return -1;
    }

    struct sockaddr_in addr;
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = *((unsigned long*)d_addr->h_addr);
    addr.sin_port = htons(SMTP_PORT);

    if (connect(client->sock, (struct sockaddr*)&addr, sizeof(addr)) < 0) {
        perror("Connecting error");
        return -1;
    }

    char buf[1024];
    recv(client->sock, buf, sizeof(buf), 0);
    printf("Connected to server: %s\n", buf);
    return 0;
}

int authenticateSmtp(SmtpClient* client, const char* username, const char* password) {
    char buf[1024];
    const char* commands[] = {
            "EHLO localhost\r\n",
            "AUTH LOGIN\r\n",
            username,
            password,
            NULL
    };

    for (int i = 0; commands[i] != NULL; i++) {
        send(client->sock, commands[i], strlen(commands[i]), 0);
        printf("Sent: %s", commands[i]);

        recv(client->sock, buf, sizeof(buf), 0);
        printf("Received: %s\n", buf);
    }

    return 0;
}

int sendEmail(SmtpClient* client, const char* sender, const char* receiver, const char* subject, const char* body) {
    char buf[1024];
    char command[512];

    snprintf(command, sizeof(command), "MAIL FROM: <%s>\r\n", sender);
    send(client->sock, command, strlen(command), 0);
    recv(client->sock, buf, sizeof(buf), 0);
    printf("Sent: %sReceived: %s\n", command, buf);

    snprintf(command, sizeof(command), "RCPT TO: <%s>\r\n", receiver);
    send(client->sock, command, strlen(command), 0);
    recv(client->sock, buf, sizeof(buf), 0);
    printf("Sent: %sReceived: %s\n", command, buf);

    send(client->sock, "DATA\r\n", strlen("DATA\r\n"), 0);
    recv(client->sock, buf, sizeof(buf), 0);
    printf("Sent: DATA\nReceived: %s\n", buf);

    snprintf(command, sizeof(command),
             "From: %s\r\nTo: %s\r\nSubject: %s\r\n\r\n%s\r\n.\r\n",
             sender, receiver, subject, body);
    send(client->sock, command, strlen(command), 0);
    recv(client->sock, buf, sizeof(buf), 0);
    printf("Sent: Email content\nReceived: %s\n", buf);

    return 0;
}

int closeSmtpConnection(SmtpClient* client) {
    return send(client->sock, "QUIT\r\n", strlen("QUIT\r\n"), 0);
}

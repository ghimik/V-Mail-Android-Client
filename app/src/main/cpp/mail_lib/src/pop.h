//
// Created by Алексей on 08.10.2024.
//

#ifndef V_MAIL_POP_H
#define V_MAIL_POP_H
typedef struct {
    int sockfd;
    char *server_ip;
    char *username;
    char *password;
} Pop3Client;

Pop3Client *pop3_client_init(const char *server_ip, const char *username, const char *password);

char *pop3_client_receive_response(Pop3Client *client);

void pop3_client_connect(Pop3Client *client, int port);

char **pop3_client_list(Pop3Client *client, int *message_count);

void pop3_client_disconnect(Pop3Client *client);

void pop3_client_free(Pop3Client *client);


#endif //V_MAIL_POP_H

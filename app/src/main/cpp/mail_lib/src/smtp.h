//
// Created by Алексей on 08.10.2024.
//

#ifndef V_MAIL_SMTP_H
#define V_MAIL_SMTP_H

typedef struct {
    int sockfd;
    char *server;
    int port;
} SmtpClient;

int smtp_client_init(SmtpClient *client, const char *server, int port);

int smtp_client_connect(SmtpClient *client);

void smtp_client_send_command(int sockfd, const char *command);

char* smtp_client_receive_response(int sockfd);

int smtp_client_check_response(const char *response);

int smtp_client_helo(SmtpClient *client);

int smtp_client_mail_from(SmtpClient *client, const char *from);

int smtp_client_rcpt_to(SmtpClient *client, const char *to);

int smtp_client_data(SmtpClient *client);

int smtp_client_send_message(SmtpClient *client, const char *subject, const char *body);

int smtp_client_quit(SmtpClient *client);

int smtp_client_send_email(SmtpClient *client, const char *from, const char *to, const char *subject, const char *body);

void smtp_client_free(SmtpClient *client);
#endif //V_MAIL_SMTP_H

#ifndef SMTP_H
#define SMTP_H

#include <stddef.h>

typedef struct SmtpClient {
    int sock;
} SmtpClient;

SmtpClient* createSmtpClient(void);

void destroySmtpClient(SmtpClient** client);

int connectToSmtpServer(SmtpClient* client);

int authenticateSmtp(SmtpClient* client, const char* username, const char* password);

int sendEmail(
        SmtpClient* client,
        const char* sender,
        const char* recipient,
        const char* subject,
        const char* body
);

int closeSmtpConnection(SmtpClient* client);

#endif // SMTP_H

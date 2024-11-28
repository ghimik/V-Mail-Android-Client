#ifndef SMTP_H
#define SMTP_H

#include <stddef.h>

// Структура клиента SMTP
typedef struct SmtpClient {
    int sock; // Сокет соединения
} SmtpClient;

// Создание клиента SMTP
SmtpClient* createSmtpClient(void);

// Уничтожение клиента SMTP
void destroySmtpClient(SmtpClient** client);

// Подключение к серверу SMTP
int connectToSmtpServer(SmtpClient* client);

// Авторизация на сервере SMTP
int authenticateSmtp(SmtpClient* client, const char* username, const char* password);

// Отправка письма через SMTP
int sendEmail(
        SmtpClient* client,
        const char* sender,
        const char* recipient,
        const char* subject,
        const char* body
);

// Завершение соединения SMTP
int closeSmtpConnection(SmtpClient* client);

#endif // SMTP_H

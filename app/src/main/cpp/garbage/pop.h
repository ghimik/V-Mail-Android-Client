#ifndef POP3_H
#define POP3_H

// Структура клиента POP3
typedef struct Pop3Client {
    int sock; // Сокет соединения
} Pop3Client;

// Создание клиента POP3
Pop3Client* createPop3Client(void);

// Уничтожение клиента POP3
void destroyPop3Client(Pop3Client** client);

// Подключение к серверу POP3
int connectToPop3Server(Pop3Client* client, const char* server, int port);

// Авторизация на сервере POP3
int authenticatePop3(Pop3Client* client, const char* username, const char* password);

// Получение письма по индексу
int retrieveEmail(Pop3Client* client, int emailIndex);

// Завершение работы с сервером POP3
int quitPop3Server(Pop3Client* client);

char** listEmails(Pop3Client* client, int* emailCount);

#endif // POP3_H

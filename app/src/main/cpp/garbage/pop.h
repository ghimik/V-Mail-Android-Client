#ifndef POP3_H
#define POP3_H

typedef struct Pop3Client {
    int sock;
} Pop3Client;

Pop3Client* createPop3Client(void);

void destroyPop3Client(Pop3Client** client);

int connectToPop3Server(Pop3Client* client, const char* server, int port);

int authenticatePop3(Pop3Client* client, const char* username, const char* password);

int retrieveEmail(Pop3Client* client, int emailIndex);

int quitPop3Server(Pop3Client* client);

char** listEmails(Pop3Client* client, int* emailCount);

#endif // POP3_H

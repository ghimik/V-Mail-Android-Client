#ifndef V_MAIL_POP3CLIENT_C
#define V_MAIL_POP3CLIENT_C

#include <jni.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <android/log.h>

#define BUFFER_SIZE 16384

#define ENABLE_LOGGING 1

#if ENABLE_LOGGING
    #define LOG_TAG "Pop3Client"
    #define LOGD(msg) __android_log_write(ANDROID_LOG_DEBUG, LOG_TAG, msg)
    #define LOGE(msg) __android_log_write(ANDROID_LOG_ERROR, LOG_TAG, msg)
#else
#define LOGD(msg) do {} while (0)
    #define LOGE(msg) do {} while (0)
#endif

typedef struct {
    int sockfd;
} Pop3Client;

// Connect
Pop3Client *pop3_connect(const char *server, int port) {
    Pop3Client *client = malloc(sizeof(Pop3Client));
    struct sockaddr_in server_addr;

    client->sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (client->sockfd < 0) {
        perror("Socket creation failed");
        free(client);
        return NULL;
    }

    char xx[255];
    sprintf(xx, "%d", client->sockfd);
    LOGE(xx); // socket fd
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(port);
    inet_pton(AF_INET, server, &server_addr.sin_addr);

    if (connect(client->sockfd, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
        perror("Connection failed");
        free(client);
        return NULL;
    }

    char response[BUFFER_SIZE];
    read(client->sockfd, response, BUFFER_SIZE);

    LOGD(response); // Log server response

    printf("POP3 Server: %s", response);

    return client;
}

int pop3_send_command(Pop3Client *client, const char *command, char *response) {
    write(client->sockfd, command, strlen(command));
    write(client->sockfd, "\r\n", 2);

    int bytes_read = read(client->sockfd, response, BUFFER_SIZE);
    if (bytes_read < 0) {
        perror("Failed to read response");
        return -1;
    }

    response[bytes_read] = '\0';

    return 0;
}

int pop3_authenticate(Pop3Client *client, const char *username, const char *password) {
    char xx[255];
    sprintf(xx, "%d", client->sockfd);
    LOGD(xx); // Log socket fd in auth

    char response[BUFFER_SIZE];

    char user_command[BUFFER_SIZE];
    snprintf(user_command, BUFFER_SIZE, "USER %s", username);
    if (pop3_send_command(client, user_command, response) < 0 || strncmp(response, "+OK", 3) != 0) {
        fprintf(stderr, "USER command failed\n");
        return -1;
    }

    char pass_command[BUFFER_SIZE];
    snprintf(pass_command, BUFFER_SIZE, "PASS %s", password);
    if (pop3_send_command(client, pass_command, response) < 0 || strncmp(response, "+OK", 3) != 0) {
        fprintf(stderr, "PASS command failed\n");
        return -1;
    }

    return 0;
}

// Disconnect
void pop3_disconnect(Pop3Client *client) {
    if (client) {
        write(client->sockfd, "QUIT\r\n", 6);
        close(client->sockfd);
        free(client);
    }
}

// get emails
JNIEXPORT jobjectArray JNICALL
Java_com_example_v_1mail_mail_jni_Pop3ClientJNI_getInboxNative
        (JNIEnv *env, jobject obj, jlong clientPtr) {
    Pop3Client *client = (Pop3Client *)clientPtr;
    char response[BUFFER_SIZE];
    jobjectArray serverResponses;

    if (pop3_send_command(client, "LIST", response) < 0) {
        LOGE("Failed to send LIST command");
        return NULL;
    }

    LOGD("LIST command response:");
    LOGD(response);

    char *responseCopy = strdup(response);

    int responseCount = 0;
    char *line = strtok(responseCopy, "\r\n");
    while (line != NULL) {
        if (strcmp(line, ".") != 0 && line[0] != '+') {
            responseCount++;
        }
        line = strtok(NULL, "\r\n");
    }

    serverResponses = (*env)->NewObjectArray(env, responseCount, (*env)->FindClass(env, "java/lang/String"), NULL);

    strcpy(responseCopy, response);
    line = strtok(responseCopy, "\r\n");
    int index = 0;
    while (line != NULL) {
        if (strcmp(line, ".") == 0 || line[0] == '+') {
            line = strtok(NULL, "\r\n");
            continue;
        }

        jstring javaLine = (*env)->NewStringUTF(env, line);
        (*env)->SetObjectArrayElement(env, serverResponses, index++, javaLine);

        line = strtok(NULL, "\r\n");
    }

    free(responseCopy);
    LOGD("Finished sending server responses to Java");
    return serverResponses;
}


JNIEXPORT jstring JNICALL
Java_com_example_v_1mail_mail_jni_Pop3ClientJNI_retrNative
        (JNIEnv *env, jobject obj, jlong clientPtr, jint emailIndex) {
    Pop3Client *client = (Pop3Client *)clientPtr;
    char command[BUFFER_SIZE];
    char response[BUFFER_SIZE];
    int emailSize = 0;

    // Отправляем команду RETR для получения письма по индексу
    snprintf(command, BUFFER_SIZE, "RETR %d\r\n", emailIndex);
    if (write(client->sockfd, command, strlen(command)) < 0) {
        LOGE("Failed to send RETR command");
        return (*env)->NewStringUTF(env, "Error sending RETR command");
    }

    // Считываем первую строку посимвольно до новой строки
    int i = 0;
    while (1) {
        int byteRead = read(client->sockfd, &response[i], 1);
        if (byteRead <= 0) {
            LOGE("Failed to read size response");
            return (*env)->NewStringUTF(env, "Error reading email size response");
        }

        if (response[i] == '\n' || i >= BUFFER_SIZE - 1) {
            response[i] = '\0';  // Завершаем строку
            break;
        }
        i++;
    }

    // Разбираем строку ответа, чтобы получить размер письма
    if (sscanf(response, "+OK %d octets", &emailSize) != 1) {
        LOGE("Failed to parse email size");
        LOGE(response);
        return (*env)->NewStringUTF(env, "Error parsing email size");
    }


    // Выделяем буфер нужного размера для письма
    char *emailContent = malloc(emailSize + 1);
    if (emailContent == NULL) {
        LOGE("Memory allocation failed for email content");
        return (*env)->NewStringUTF(env, "Memory allocation failed");
    }

    // Считываем содержимое письма
    int totalBytesRead = 0;
    while (totalBytesRead < emailSize) {
        int bytesRead = read(client->sockfd, emailContent + totalBytesRead, emailSize - totalBytesRead);
        if (bytesRead <= 0) {
            LOGE("Error reading email content");
            free(emailContent);
            return (*env)->NewStringUTF(env, "Error reading email content");
        }
        totalBytesRead += bytesRead;
    }

    // Завершаем строку нулевым байтом
    emailContent[emailSize] = '\0';

    char termination[2];
    while (read(client->sockfd, termination, 1) > 0) {
        if (termination[0] == '.') {
            while (read(client->sockfd, termination, 1) > 0 && termination[0] != '\n') {
            }
            break;
        }
    }

    // Логируем содержимое письма
    LOGD("email content:");
    LOGD(emailContent);

    // Создаем Java строку из считанного содержимого письма
    jstring emailResult = (*env)->NewStringUTF(env, emailContent);

    // Освобождаем память и возвращаем результат
    free(emailContent);
    return emailResult;
}





JNIEXPORT jlong JNICALL
Java_com_example_v_1mail_mail_jni_Pop3ClientJNI_authenticateNative(JNIEnv *env, jobject obj, jlong clientPtr, jstring username, jstring password) {
    Pop3Client *client = (Pop3Client *)clientPtr;
    const char *c_username = (*env)->GetStringUTFChars(env, username, NULL);
    const char *c_password = (*env)->GetStringUTFChars(env, password, NULL);

    LOGE(c_username); // Log username
    LOGE(c_password); // Log password

    long result = pop3_authenticate(client, c_username, c_password);

    (*env)->ReleaseStringUTFChars(env, username, c_username);
    (*env)->ReleaseStringUTFChars(env, password, c_password);

    return result;
}

JNIEXPORT jlong JNICALL
Java_com_example_v_1mail_mail_jni_Pop3ClientJNI_connectNative
        (JNIEnv *env, jobject obj, jstring server, jint port) {
    const char *c_server = (*env)->GetStringUTFChars(env, server, NULL);
    Pop3Client *client = pop3_connect(c_server, port);
    (*env)->ReleaseStringUTFChars(env, server, c_server);
    return (jlong)client;
}

JNIEXPORT void JNICALL
Java_com_example_v_1mail_mail_jni_Pop3ClientJNI_disconnectNative
        (JNIEnv *env, jobject obj, jlong clientPtr) {
    Pop3Client *client = (Pop3Client *)clientPtr;
    pop3_disconnect(client);
}



#endif //V_MAIL_POP3CLIENT_C


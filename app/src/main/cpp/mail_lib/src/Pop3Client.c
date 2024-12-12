#ifndef V_MAIL_POP3CLIENT_C
#define V_MAIL_POP3CLIENT_C

#include <jni.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <android/log.h>


#define BUFFER_SIZE 1024
#define MAX_EMAILS 100

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
    __android_log_write(ANDROID_LOG_ERROR, "socket fd", xx);
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

    char xxx[255];
    sprintf(xxx, "%s", response);
    __android_log_write(ANDROID_LOG_ERROR, "server response", xxx);

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

    char xxx[255];
    sprintf(xxx, "%s", response);
    __android_log_write(ANDROID_LOG_ERROR, "server response", xxx);

    return 0;
}

int pop3_authenticate(Pop3Client *client, const char *username, const char *password) {
    char xx[255];
    sprintf(xx, "%d", client->sockfd);
    __android_log_write(ANDROID_LOG_ERROR, "socket fd in auth", xx);

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
    jobjectArray emailStrings;

    if (pop3_send_command(client, "LIST", response) < 0) {
        return NULL;
    }

    char **emails = malloc(sizeof(char *) * MAX_EMAILS);
    int emailCount = 0;

    while (read(client->sockfd, response, BUFFER_SIZE) > 0) {
        if (strcmp(response, ".\r\n") == 0) break;

        emails[emailCount] = strdup(response);
        emailCount++;
    }

    emailStrings = (*env)->NewObjectArray(env, emailCount, (*env)->FindClass(env, "java/lang/String"), NULL);
    for (int i = 0; i < emailCount; i++) {
        jstring email = (*env)->NewStringUTF(env, emails[i]);
        (*env)->SetObjectArrayElement(env, emailStrings, i, email);
        free(emails[i]);
    }

    free(emails);
    return emailStrings;
}

JNIEXPORT jlong JNICALL
Java_com_example_v_1mail_mail_jni_Pop3ClientJNI_authenticateNative(JNIEnv *env, jobject obj, jlong clientPtr, jstring username, jstring password) {
    Pop3Client *client = (Pop3Client *)clientPtr;
    const char *c_username = (*env)->GetStringUTFChars(env, username, NULL);
    const char *c_password = (*env)->GetStringUTFChars(env, password, NULL);

    __android_log_write(ANDROID_LOG_ERROR, "username", c_username);//Or ANDROID_LOG_INFO, ...
    __android_log_write(ANDROID_LOG_ERROR, "password", c_password);//Or ANDROID_LOG_INFO, ...

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

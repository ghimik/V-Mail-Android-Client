#include <jni.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>

#define BUFFER_SIZE 1024

typedef struct {
    int sockfd;
} SmtpClient;

SmtpClient* smtp_connect(const char* server, int port) {
    SmtpClient* client = malloc(sizeof(SmtpClient));
    struct sockaddr_in server_addr;

    client->sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (client->sockfd < 0) {
        perror("Socket creation failed");
        free(client);
        return NULL;
    }

    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(port);
    inet_pton(AF_INET, server, &server_addr.sin_addr);

    if (connect(client->sockfd, (struct sockaddr*)&server_addr, sizeof(server_addr)) < 0) {
        perror("Connection failed");
        free(client);
        return NULL;
    }

    // Read the server response after connection
    char response[BUFFER_SIZE];
    read(client->sockfd, response, BUFFER_SIZE);
    printf("SMTP Server: %s", response);

    return client;
}

int smtp_send_command(SmtpClient* client, const char* command) {
    write(client->sockfd, command, strlen(command));   // Send command
    write(client->sockfd, "\r\n", 2);                    // Newline after command

    char response[BUFFER_SIZE];
    int bytes_read = read(client->sockfd, response, BUFFER_SIZE); // Read response
    if (bytes_read < 0) {
        perror("Failed to read response");
        return -1;
    }

    response[bytes_read] = '\0';  // Null-terminate response
    printf("Server: %s", response);
    return 0;
}

void smtp_disconnect(SmtpClient* client) {
    if (client) {
        smtp_send_command(client, "QUIT");
        close(client->sockfd);
        free(client);
    }
}

JNIEXPORT jlong JNICALL
Java_com_example_v_1mail_mail_jni_SmtpClientJNI_connectNative
        (JNIEnv* env, jobject obj, jstring server, jint port) {
    const char* c_server = (*env)->GetStringUTFChars(env, server, NULL);
    SmtpClient* client = smtp_connect(c_server, port);
    (*env)->ReleaseStringUTFChars(env, server, c_server);
    return (jlong)client;
}

JNIEXPORT void JNICALL
Java_com_example_v_1mail_mail_jni_SmtpClientJNI_authenticateNative
        (JNIEnv* env, jobject obj, jlong clientPtr, jstring username, jstring password) {
    SmtpClient* client = (SmtpClient*)clientPtr;
    const char* c_username = (*env)->GetStringUTFChars(env, username, NULL);
    const char* c_password = (*env)->GetStringUTFChars(env, password, NULL);

    smtp_send_command(client, "EHLO localhost");
    smtp_send_command(client, "AUTH LOGIN");
    smtp_send_command(client, c_username);
    smtp_send_command(client, c_password);

    (*env)->ReleaseStringUTFChars(env, username, c_username);
    (*env)->ReleaseStringUTFChars(env, password, c_password);
}

JNIEXPORT void JNICALL
Java_com_example_v_1mail_mail_jni_SmtpClientJNI_sendEmailNative
        (JNIEnv* env, jobject obj, jlong clientPtr, jstring from, jstring to, jstring subject, jstring body) {
    SmtpClient* client = (SmtpClient*)clientPtr;
    const char* c_from = (*env)->GetStringUTFChars(env, from, NULL);
    const char* c_to = (*env)->GetStringUTFChars(env, to, NULL);
    const char* c_subject = (*env)->GetStringUTFChars(env, subject, NULL);
    const char* c_body = (*env)->GetStringUTFChars(env, body, NULL);

    char mail_from[BUFFER_SIZE];
    snprintf(mail_from, BUFFER_SIZE, "MAIL FROM:<%s>\r\n", c_from);
    smtp_send_command(client, mail_from);

    char rcpt_to[BUFFER_SIZE];
    snprintf(rcpt_to, BUFFER_SIZE, "RCPT TO:<%s>\r\n", c_to);
    smtp_send_command(client, rcpt_to);

    smtp_send_command(client, "DATA\r\n");

    char data[BUFFER_SIZE];
    snprintf(data, BUFFER_SIZE,
             "From: %s\r\n"
             "To: %s\r\n"
             "Subject: %s\r\n"
             "\r\n%s\r\n", c_from, c_to, c_subject, c_body);
    smtp_send_command(client, data);

    smtp_send_command(client, ".\r\n");

    (*env)->ReleaseStringUTFChars(env, from, c_from);
    (*env)->ReleaseStringUTFChars(env, to, c_to);
    (*env)->ReleaseStringUTFChars(env, subject, c_subject);
    (*env)->ReleaseStringUTFChars(env, body, c_body);
}


JNIEXPORT void JNICALL
Java_com_example_v_1mail_mail_jni_SmtpClientJNI_disconnectNative
        (JNIEnv* env, jobject obj, jlong clientPtr) {
    SmtpClient* client = (SmtpClient*)clientPtr;
    smtp_disconnect(client);
}

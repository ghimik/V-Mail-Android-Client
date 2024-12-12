#include <jni.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <android/log.h>

#define ENABLE_LOGGING 1

#if ENABLE_LOGGING
#define LOG_TAG "SmtpClient"
#define LOGD(msg) __android_log_write(ANDROID_LOG_DEBUG, LOG_TAG, msg)
#define LOGE(msg) __android_log_write(ANDROID_LOG_ERROR, LOG_TAG, msg)
#else
#define LOGD(msg) do {} while (0)
    #define LOGE(msg) do {} while (0)
#endif

#define BUFFER_SIZE 1024

typedef struct {
    int sockfd;
} SmtpClient;

SmtpClient* smtp_connect(const char* server, int port) {
    LOGD("Attempting to connect to SMTP server");
    SmtpClient* client = malloc(sizeof(SmtpClient));
    struct sockaddr_in server_addr;

    client->sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (client->sockfd < 0) {
        LOGE("Socket creation failed");
        free(client);
        return NULL;
    }

    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(port);
    inet_pton(AF_INET, server, &server_addr.sin_addr);

    if (connect(client->sockfd, (struct sockaddr*)&server_addr, sizeof(server_addr)) < 0) {
        LOGE("Connection failed");
        free(client);
        return NULL;
    }

    char response[BUFFER_SIZE];
    int bytes_read = read(client->sockfd, response, BUFFER_SIZE);
    if (bytes_read > 0) {
        response[bytes_read] = '\0';
        LOGD("SMTP Server response:");
        LOGD(response);
    }

    return client;
}

int smtp_send_command(SmtpClient* client, const char* command) {
    usleep(50000);
    LOGD("Sending command:");
    LOGD(command);

    char full_command[BUFFER_SIZE];
    snprintf(full_command, sizeof(full_command), "%s\r\n", command);

    ssize_t bytes_written = write(client->sockfd, full_command, strlen(full_command));
    if (bytes_written < 0) {
        LOGE("Failed to send command");
        return -1;
    }

    char response[BUFFER_SIZE];
    int bytes_read = read(client->sockfd, response, BUFFER_SIZE);
    if (bytes_read < 0) {
        LOGE("Failed to read response from server");
        return -1;
    }

    response[bytes_read] = '\0';
    LOGD("Server response:");
    LOGD(response);

    return 0;
}


int smtp_send_command_no_response(SmtpClient* client, const char* command) {
    usleep(50000);
    LOGD("Sending command:");
    LOGD(command);

    char full_command[BUFFER_SIZE];
    snprintf(full_command, sizeof(full_command), "%s\r\n", command);

    ssize_t bytes_written = write(client->sockfd, full_command, strlen(full_command));
    if (bytes_written < 0) {
        LOGE("Failed to send command");
        return -1;
    }

    return 0;
}



void smtp_disconnect(SmtpClient* client) {
    if (client) {
        LOGD("Disconnecting from SMTP server");
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
        (JNIEnv* env, jobject obj, jlong clientPtr, jstring from, jstring to,
         jstring subject, jstring body, jstring date) {
    SmtpClient* client = (SmtpClient*)clientPtr;

    const char* c_from = (*env)->GetStringUTFChars(env, from, NULL);
    const char* c_to = (*env)->GetStringUTFChars(env, to, NULL);
    const char* c_subject = (*env)->GetStringUTFChars(env, subject, NULL);
    const char* c_body = (*env)->GetStringUTFChars(env, body, NULL);
    const char* c_date = (*env)->GetStringUTFChars(env, date, NULL);

    // Отправляем команду MAIL FROM
    char mail_from[BUFFER_SIZE];
    snprintf(mail_from, BUFFER_SIZE, "MAIL FROM:<%s>", c_from);
    smtp_send_command(client, mail_from);

    // Отправляем команду RCPT TO
    char rcpt_to[BUFFER_SIZE];
    snprintf(rcpt_to, BUFFER_SIZE, "RCPT TO:<%s>", c_to);
    smtp_send_command(client, rcpt_to);

    // Отправляем команду DATA
    smtp_send_command(client, "DATA");

    // Отправляем отдельные заголовки

    char from_command[BUFFER_SIZE];
    snprintf(from_command, BUFFER_SIZE, "From: %s", c_from);
    smtp_send_command_no_response(client, from_command);

    char to_command[BUFFER_SIZE];
    snprintf(to_command, BUFFER_SIZE, "To: %s", c_to);
    smtp_send_command_no_response(client, to_command);

    char subject_command[BUFFER_SIZE];
    snprintf(subject_command, BUFFER_SIZE, "Subject: %s", c_subject);
    smtp_send_command_no_response(client, subject_command);

    char date_command[BUFFER_SIZE];
    snprintf(date_command, BUFFER_SIZE, "Date: %s", c_date);
    smtp_send_command_no_response(client, date_command);


    // Отправляем тело письма
    char body_command[BUFFER_SIZE];
    snprintf(body_command, BUFFER_SIZE, "\r\n%s", c_body);
    smtp_send_command_no_response(client, body_command);

    // Завершаем отправку данных
    smtp_send_command(client, ".");

    LOGD(from_command);
    LOGD(to_command);
    LOGD(subject_command);
    LOGD(date_command);
    LOGD(body_command);
    LOGD(".");



    // Освобождаем память
    (*env)->ReleaseStringUTFChars(env, from, c_from);
    (*env)->ReleaseStringUTFChars(env, to, c_to);
    (*env)->ReleaseStringUTFChars(env, subject, c_subject);
    (*env)->ReleaseStringUTFChars(env, body, c_body);
    (*env)->ReleaseStringUTFChars(env, date, c_date);
}


JNIEXPORT void JNICALL
Java_com_example_v_1mail_mail_jni_SmtpClientJNI_disconnectNative
        (JNIEnv* env, jobject obj, jlong clientPtr) {
    SmtpClient* client = (SmtpClient*)clientPtr;
    smtp_disconnect(client);
}

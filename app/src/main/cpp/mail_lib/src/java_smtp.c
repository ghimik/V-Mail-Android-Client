//
// Created by Алексей on 23.10.2024.
//
#include <jni.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include "smtp.h"
#include <jni.h>

JNIEXPORT jlong
Java_com_example_v_1mail_smtp_SmtpClient_nativeInit(JNIEnv *env, jobject obj, jstring server_ip, jint port) {
    const char *nativeServerIp = (*env)->GetStringUTFChars(env, server_ip, 0);

    SmtpClient *client = (SmtpClient *)malloc(sizeof(SmtpClient));
    if (smtp_client_init(client, nativeServerIp, port) != 0) {
        (*env)->ReleaseStringUTFChars(env, server_ip, nativeServerIp);
        free(client);
        return (jlong)0;
    }

    (*env)->ReleaseStringUTFChars(env, server_ip, nativeServerIp);
    return (jlong)(uintptr_t)client;
}

JNIEXPORT jint
Java_com_example_v_1mail_smtp_SmtpClient_nativeConnect(JNIEnv *env, jobject obj, jlong clientPtr) {
    SmtpClient *client = (SmtpClient *)(uintptr_t)clientPtr;
    return smtp_client_connect(client);
}

JNIEXPORT jint
Java_com_example_v_1mail_smtp_SmtpClient_nativeSendEmail(JNIEnv *env, jobject obj, jlong clientPtr, jstring from, jstring to, jstring subject, jstring body) {
    SmtpClient *client = (SmtpClient *)(uintptr_t)clientPtr;

    const char *nativeFrom = (*env)->GetStringUTFChars(env, from, 0);
    const char *nativeTo = (*env)->GetStringUTFChars(env, to, 0);
    const char *nativeSubject = (*env)->GetStringUTFChars(env, subject, 0);
    const char *nativeBody = (*env)->GetStringUTFChars(env, body, 0);

    int result = smtp_client_send_email(client, nativeFrom, nativeTo, nativeSubject, nativeBody);

    (*env)->ReleaseStringUTFChars(env, from, nativeFrom);
    (*env)->ReleaseStringUTFChars(env, to, nativeTo);
    (*env)->ReleaseStringUTFChars(env, subject, nativeSubject);
    (*env)->ReleaseStringUTFChars(env, body, nativeBody);

    return result;
}

JNIEXPORT void JNICALL
Java_com_example_v_1mail_smtp_SmtpClient_nativeDisconnect(JNIEnv *env, jobject obj, jlong clientPtr) {
    SmtpClient *client = (SmtpClient *)(uintptr_t)clientPtr;
    smtp_client_quit(client);
    close(client->sockfd);
}

JNIEXPORT void JNICALL
Java_com_example_v_1mail_smtp_SmtpClient_nativeFree(JNIEnv *env, jobject obj, jlong clientPtr) {
    SmtpClient *client = (SmtpClient *)(uintptr_t)clientPtr;
    smtp_client_free(client);
    free(client);
}


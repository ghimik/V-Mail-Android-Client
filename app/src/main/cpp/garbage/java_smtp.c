#include <jni.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include "smtp.h"

JNIEXPORT jlong JNICALL
Java_com_example_v_1mail_mail_SmtpClient_nativeCreate(JNIEnv *env, jobject obj) {
    SmtpClient *client = createSmtpClient();
    if (!client) {
        fprintf(stderr, "Failed to create SmtpClient\n");
        return 0;
    }
    return (jlong)(uintptr_t)client;
}

JNIEXPORT jint JNICALL
Java_com_example_v_1mail_mail_SmtpClient_nativeConnect(JNIEnv *env, jobject obj, jlong clientPtr) {
    SmtpClient *client = (SmtpClient *)(uintptr_t)clientPtr;
    return connectToSmtpServer(client);
}

JNIEXPORT jint JNICALL
Java_com_example_v_1mail_mail_SmtpClient_nativeAuthenticate(JNIEnv *env, jobject obj, jlong clientPtr, jstring username, jstring password) {
    SmtpClient *client = (SmtpClient *)(uintptr_t)clientPtr;

    const char *nativeUsername = (*env)->GetStringUTFChars(env, username, 0);
    const char *nativePassword = (*env)->GetStringUTFChars(env, password, 0);

    int result = authenticateSmtp(client, nativeUsername, nativePassword);

    (*env)->ReleaseStringUTFChars(env, username, nativeUsername);
    (*env)->ReleaseStringUTFChars(env, password, nativePassword);

    return result;
}

JNIEXPORT jint JNICALL
Java_com_example_v_1mail_mail_SmtpClient_nativeSendEmail(JNIEnv *env, jobject obj, jlong clientPtr, jstring sender, jstring recipient, jstring subject, jstring body) {
    SmtpClient *client = (SmtpClient *)(uintptr_t)clientPtr;

    const char *nativeSender = (*env)->GetStringUTFChars(env, sender, 0);
    const char *nativeRecipient = (*env)->GetStringUTFChars(env, recipient, 0);
    const char *nativeSubject = (*env)->GetStringUTFChars(env, subject, 0);
    const char *nativeBody = (*env)->GetStringUTFChars(env, body, 0);

    int result = sendEmail(client, nativeSender, nativeRecipient, nativeSubject, nativeBody);

    (*env)->ReleaseStringUTFChars(env, sender, nativeSender);
    (*env)->ReleaseStringUTFChars(env, recipient, nativeRecipient);
    (*env)->ReleaseStringUTFChars(env, subject, nativeSubject);
    (*env)->ReleaseStringUTFChars(env, body, nativeBody);

    return result;
}

JNIEXPORT jint JNICALL
Java_com_example_v_1mail_mail_SmtpClient_nativeClose(JNIEnv *env, jobject obj, jlong clientPtr) {
    SmtpClient *client = (SmtpClient *)(uintptr_t)clientPtr;
    return closeSmtpConnection(client);
}

JNIEXPORT void JNICALL
Java_com_example_v_1mail_mail_SmtpClient_nativeDestroy(JNIEnv *env, jobject obj, jlong clientPtr) {
    SmtpClient *client = (SmtpClient *)(uintptr_t)clientPtr;
    destroySmtpClient(&client);
}


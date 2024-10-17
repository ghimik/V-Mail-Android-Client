#include <jni.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include "pop.h"

JNIEXPORT jlong
Java_com_example_v_1mail_pop_Pop3Client_nativeInit(JNIEnv *env, jobject obj, jstring server_ip, jstring username, jstring password) {
    const char *nativeServerIp = (*env)->GetStringUTFChars(env, server_ip, 0);
    const char *nativeUsername = (*env)->GetStringUTFChars(env, username, 0);
    const char *nativePassword = (*env)->GetStringUTFChars(env, password, 0);

    Pop3Client *client = pop3_client_init(nativeServerIp, nativeUsername, nativePassword);

    (*env)->ReleaseStringUTFChars(env, server_ip, nativeServerIp);
    (*env)->ReleaseStringUTFChars(env, username, nativeUsername);
    (*env)->ReleaseStringUTFChars(env, password, nativePassword);

    return (jlong)(uintptr_t)client;
}

JNIEXPORT void JNICALL
Java_com_example_v_1mail_pop_Pop3Client_nativeConnect(JNIEnv *env, jobject obj, jlong clientPtr, jint port) {
    Pop3Client *client = (Pop3Client *)(uintptr_t)clientPtr;
    pop3_client_connect(client, port);
}

JNIEXPORT jobjectArray
Java_com_example_v_1mail_pop_Pop3Client_nativeList(JNIEnv *env, jobject obj, jlong clientPtr) {
    Pop3Client *client = (Pop3Client *)(uintptr_t)clientPtr;
    int message_count = 0;
    char **messages = pop3_client_list(client, &message_count);

    jobjectArray result = (*env)->NewObjectArray(env, message_count, (*env)->FindClass(env, "java/lang/String"), NULL);
    for (int i = 0; i < message_count; i++) {
        (*env)->SetObjectArrayElement(env, result, i, (*env)->NewStringUTF(env, messages[i]));
        free(messages[i]);
    }
    free(messages);
    return result;
}

JNIEXPORT void JNICALL
Java_com_example_v_1mail_pop_Pop3Client_nativeDisconnect(JNIEnv *env, jobject obj, jlong clientPtr) {
    Pop3Client *client = (Pop3Client *)(uintptr_t)clientPtr;
    pop3_client_disconnect(client);
}

JNIEXPORT void JNICALL
Java_com_example_v_1mail_pop_Pop3Client_nativeFree(JNIEnv *env, jobject obj, jlong clientPtr) {
    Pop3Client *client = (Pop3Client *)(uintptr_t)clientPtr;
    pop3_client_free(client);
}

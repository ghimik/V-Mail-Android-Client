#include <jni.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include "pop.h"

JNIEXPORT jlong JNICALL
Java_com_example_v_1mail_mail_Pop3ClientPureJava_nativeCreate(JNIEnv *env, jobject obj) {
    Pop3Client *client = createPop3Client();
    if (!client) {
        fprintf(stderr, "Failed to create Pop3Client\n");
        return 0;
    }
    return (jlong)(uintptr_t)client;
}

JNIEXPORT jint JNICALL
Java_com_example_v_1mail_mail_Pop3ClientPureJava_nativeConnect(JNIEnv *env, jobject obj, jlong clientPtr, jstring server, jint port) {
    Pop3Client *client = (Pop3Client *)(uintptr_t)clientPtr;
    const char *nativeServer = (*env)->GetStringUTFChars(env, server, 0);

    int result = connectToPop3Server(client, nativeServer, (int)port);

    (*env)->ReleaseStringUTFChars(env, server, nativeServer);
    return result;
}

JNIEXPORT jint JNICALL
Java_com_example_v_1mail_mail_Pop3ClientPureJava_nativeAuthenticate(JNIEnv *env, jobject obj, jlong clientPtr, jstring username, jstring password) {
    Pop3Client *client = (Pop3Client *)(uintptr_t)clientPtr;

    const char *nativeUsername = (*env)->GetStringUTFChars(env, username, 0);
    const char *nativePassword = (*env)->GetStringUTFChars(env, password, 0);

    int result = authenticatePop3(client, nativeUsername, nativePassword);

    (*env)->ReleaseStringUTFChars(env, username, nativeUsername);
    (*env)->ReleaseStringUTFChars(env, password, nativePassword);

    return result;
}

JNIEXPORT jint JNICALL
Java_com_example_v_1mail_mail_Pop3ClientPureJava_nativeRetrieveEmail(JNIEnv *env, jobject obj, jlong clientPtr, jint emailIndex) {
    Pop3Client *client = (Pop3Client *)(uintptr_t)clientPtr;
    return retrieveEmail(client, (int)emailIndex);
}

JNIEXPORT jint JNICALL
Java_com_example_v_1mail_mail_Pop3ClientPureJava_nativeQuit(JNIEnv *env, jobject obj, jlong clientPtr) {
    Pop3Client *client = (Pop3Client *)(uintptr_t)clientPtr;
    return quitPop3Server(client);
}

JNIEXPORT void JNICALL
Java_com_example_v_1mail_mail_Pop3ClientPureJava_nativeDestroy(JNIEnv *env, jobject obj, jlong clientPtr) {
    Pop3Client *client = (Pop3Client *)(uintptr_t)clientPtr;
    destroyPop3Client(&client);
}

JNIEXPORT jobjectArray JNICALL
Java_com_example_v_1mail_mail_Pop3ClientPureJava_nativeListEmails(JNIEnv *env, jobject obj, jlong clientPtr) {
    Pop3Client *client = (Pop3Client *)(uintptr_t)clientPtr;

    int emailCount = 0;
    char **emailList = listEmails(client, &emailCount);
    if (!emailList) {
        return NULL;
    }

    jclass stringClass = (*env)->FindClass(env, "java/lang/String");
    jobjectArray result = (*env)->NewObjectArray(env, emailCount, stringClass, NULL);

    for (int i = 0; i < emailCount; i++) {
        jstring email = (*env)->NewStringUTF(env, emailList[i]);
        (*env)->SetObjectArrayElement(env, result, i, email);
        free(emailList[i]);
    }
    free(emailList);

    return result;
}

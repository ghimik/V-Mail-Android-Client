package com.example.v_mail.mail;

public class SmtpClient {
    static {
        System.loadLibrary("v_mail"); // Загружаем нативную библиотеку
    }

    private long nativeClientPtr;

    public SmtpClient() {
        nativeClientPtr = nativeCreate();
        if (nativeClientPtr == 0) {
            throw new RuntimeException("Failed to create native SmtpClient");
        }
    }

    public void connect() {
        int result = nativeConnect(nativeClientPtr);
        if (result != 0) {
            throw new RuntimeException("Failed to connect to server");
        }
    }

    public void authenticate(String username, String password) {
        int result = nativeAuthenticate(nativeClientPtr, username, password);
        if (result != 0) {
            throw new RuntimeException("Authentication failed");
        }
    }

    public void sendEmail(String sender, String recipient, String subject, String body) {
        int result = nativeSendEmail(nativeClientPtr, sender, recipient, subject, body);
        if (result != 0) {
            throw new RuntimeException("Failed to send email");
        }
    }

    public void close() {
        int result = nativeClose(nativeClientPtr);
        if (result != 0) {
            throw new RuntimeException("Failed to close connection");
        }
    }

    public void destroy() {
        if (nativeClientPtr != 0) {
            nativeDestroy(nativeClientPtr);
            nativeClientPtr = 0;
        }
    }

    private native long nativeCreate();
    private native int nativeConnect(long clientPtr);
    private native int nativeAuthenticate(long clientPtr, String username, String password);
    private native int nativeSendEmail(long clientPtr, String sender, String recipient, String subject, String body);
    private native int nativeClose(long clientPtr);
    private native void nativeDestroy(long clientPtr);
}

package com.example.v_mail.mail;

public class Pop3Client {
    static {
        System.loadLibrary("v_mail"); // Загружаем нативную библиотеку
    }

    private long nativeClientPtr;

    public Pop3Client() {
        nativeClientPtr = nativeCreate();
        if (nativeClientPtr == 0) {
            throw new RuntimeException("Failed to create native Pop3Client");
        }
    }

    public void connect(String server, int port) {
        int result = nativeConnect(nativeClientPtr, server, port);
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

    public void retrieveEmail(int emailIndex) {
        int result = nativeRetrieveEmail(nativeClientPtr, emailIndex);
        if (result != 0) {
            throw new RuntimeException("Failed to retrieve email");
        }
    }

    public void quit() {
        int result = nativeQuit(nativeClientPtr);
        if (result != 0) {
            throw new RuntimeException("Failed to quit from server");
        }
    }

    public void close() {
        if (nativeClientPtr != 0) {
            nativeDestroy(nativeClientPtr);
            nativeClientPtr = 0;
        }
    }

    public String[] listEmails() {
        String[] result = nativeListEmails(nativeClientPtr);
        if (result == null) {
            throw new RuntimeException("Failed to list emails");
        }
        return result;
    }



    private native long nativeCreate();
    private native int nativeConnect(long clientPtr, String server, int port);
    private native int nativeAuthenticate(long clientPtr, String username, String password);
    private native int nativeRetrieveEmail(long clientPtr, int emailIndex);
    private native int nativeQuit(long clientPtr);
    private native void nativeDestroy(long clientPtr);
    private native String[] nativeListEmails(long clientPtr);

}

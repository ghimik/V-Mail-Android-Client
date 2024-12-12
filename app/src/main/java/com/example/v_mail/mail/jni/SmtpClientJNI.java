package com.example.v_mail.mail.jni;

import android.os.Build;

import com.example.v_mail.mail.Email;
import com.example.v_mail.mail.SmtpClient;

import java.time.ZonedDateTime;
import java.time.format.DateTimeFormatter;

public class SmtpClientJNI implements SmtpClient {
    static {
        System.loadLibrary("v_mail");
    }

    private long nativeClientPtr;

    private final String server;

    private final int port;

    public SmtpClientJNI(String server, int port) {
        this.server = server;
        this.port = port;
    }

    @Override
    public void connect() {
        nativeClientPtr = connectNative(server, port);
    }

    @Override
    public void authenticate(String username, String password) {
        authenticateNative(nativeClientPtr, username, password);
    }

    @Override
    public void sendEmail(Email email) {
        if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.O) {
            sendEmailNative(nativeClientPtr, email.getFrom(), email.getTo(), email.getSubject(), email.getBody(),
                    DateTimeFormatter.RFC_1123_DATE_TIME
                            .format(ZonedDateTime.now()));
        }
    }

    @Override
    public void close() {
        disconnectNative(nativeClientPtr);
    }

    private native long connectNative(String server, int port);

    private native void authenticateNative(long clientPtr, String username, String password);

    private native void sendEmailNative(long clientPtr, String from, String to, String subject, String body, String date);

    private native void disconnectNative(long clientPtr);
}

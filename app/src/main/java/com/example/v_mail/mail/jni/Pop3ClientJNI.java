package com.example.v_mail.mail.jni;

import com.example.v_mail.mail.Email;
import com.example.v_mail.mail.Pop3Client;

import java.io.IOException;
import java.util.ArrayList;
import java.util.List;

public class Pop3ClientJNI implements Pop3Client {
    static {
        System.loadLibrary("v_mail");
    }

    private long nativeClientPtr;

    private final String server;

    private final int port;

    public Pop3ClientJNI(String server, int port) {
        this.server = server;
        this.port = port;
    }

    @Override
    public void connect() {
        nativeClientPtr = connectNative(server, port);
    }

    @Override
    public void authenticate(String username, String password) throws IOException {
        if (authenticateNative(nativeClientPtr, username, password) < 0)
            throw new IOException("Invalid username or password");

    }

    @Override
    public List<Email> getInbox() {
        String[] rawEmails = getInboxNative(nativeClientPtr);
        List<Email> emails = new ArrayList<>();
        for (String rawEmail : rawEmails) {
            emails.add(parseEmail(rawEmail));
        }
        return emails;
    }

    private Email parseEmail(String rawEmail) {
        String from = "", to = "", subject = "", body = "";
        String[] lines = rawEmail.split("\n");

        for (String line : lines) {
            if (line.startsWith("From:")) from = line.substring(6).trim();
            else if (line.startsWith("To:")) to = line.substring(4).trim();
            else if (line.startsWith("Subject:")) subject = line.substring(9).trim();
            else body += line + "\n";
        }

        return new Email(from, to, subject, body);
    }

    @Override
    public void close() {
        disconnectNative(nativeClientPtr);
    }

    private native long connectNative(String server, int port);
    private native String[] getInboxNative(long clientPtr);
    private native void disconnectNative(long clientPtr);
    private native long authenticateNative(long nativeClientPtr, String username, String password);
}

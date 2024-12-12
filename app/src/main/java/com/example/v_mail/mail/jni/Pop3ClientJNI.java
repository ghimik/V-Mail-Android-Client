package com.example.v_mail.mail.jni;

import com.example.v_mail.mail.Email;
import com.example.v_mail.mail.Pop3Client;

import java.io.IOException;
import java.util.ArrayList;
import java.util.Arrays;
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
        System.out.println("started getting native inbox");
        String[] rawResponses = getInboxNative(nativeClientPtr);
        System.out.println("raw responses: " + Arrays.toString(rawResponses));

        List<String> indexes = new ArrayList<>();
        for (String response : rawResponses) {
            if (response == null || response.isEmpty() || response.startsWith("+") || response.equals(".")) {
                continue;
            }
            indexes.add(response);
        }
        System.out.println("indexes: " + indexes);

        List<Email> emails = new ArrayList<>();
        for (String indexLine : indexes) {
            String[] parts = indexLine.split(" ");
            int index = Integer.parseInt(parts[0]);
            emails.add(parseEmail(retrNative(nativeClientPtr, index)));
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

    private native String retrNative(long clientPtr, int index);
    private native long connectNative(String server, int port);
    private native String[] getInboxNative(long clientPtr);
    private native void disconnectNative(long clientPtr);
    private native long authenticateNative(long nativeClientPtr, String username, String password);
}

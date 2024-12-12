package com.example.v_mail.mail;

import android.os.Build;

import com.example.v_mail.mail.java.Pop3ClientJava;
import com.example.v_mail.mail.java.SmtpClientJava;
import com.example.v_mail.mail.jni.Pop3ClientJNI;
import com.example.v_mail.mail.jni.SmtpClientJNI;

import java.io.IOException;
import java.util.Base64;
import java.util.List;

public class MailService {
    private final SmtpClient smtpClient;
    private final Pop3Client pop3Client;

    public MailService(String smtpServer, int smtpPort, String pop3Server, int pop3Port) {
        this.smtpClient = new SmtpClientJNI(smtpServer, smtpPort);
        this.pop3Client = new Pop3ClientJNI(pop3Server, pop3Port);
    }

    private String encodeBase64(String input) {
        if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.O) {
            return Base64.getEncoder().encodeToString(input.getBytes());
        }
        throw new RuntimeException("SDK Vesion is not correct");
    }

    public void sendEmail(String from, String to, String subject, String body, String username, String password) throws IOException {

        try {
            smtpClient.connect();
            smtpClient.authenticate(encodeBase64(username), encodeBase64(password));
            smtpClient.sendEmail(new Email(from, to, subject, body));
            smtpClient.close();
        }
        finally {
            smtpClient.close();
        }
    }

    public List<Email> getInbox(String username, String password) throws IOException {
        try {
            pop3Client.connect();
            pop3Client.authenticate(username, password);
            List<Email> emails = pop3Client.getInbox();
            return emails;
        }
        catch (Exception e) {
            e.printStackTrace();
            return List.of();
        }
        finally {
            pop3Client.close();
        }
    }

}

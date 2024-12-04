package com.example.v_mail.java_mail;

import android.os.Build;

import java.io.IOException;
import java.util.Base64;
import java.util.List;
import java.util.Optional;

public class MailService {
    private final SmtpClient smtpClient;
    private final Pop3Client pop3Client;

    public MailService(String smtpServer, int smtpPort, String pop3Server, int pop3Port) {
        this.smtpClient = new SmtpClient(smtpServer, smtpPort);
        this.pop3Client = new Pop3Client(pop3Server, pop3Port);
    }

    private String encodeBase64(String input) {
        if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.O) {
            return Base64.getEncoder().encodeToString(input.getBytes());
        }
        throw new RuntimeException("SDK Vesion is not correct");
    }

    public void sendEmail(String from, String to, String subject, String body, String username, String password) throws IOException {
        smtpClient.connect();
        smtpClient.authenticate(encodeBase64(username), encodeBase64(password));
        smtpClient.sendEmail(new Email(from, to, subject, body));
        smtpClient.close();
    }

    public List<Email> getInbox(String username, String password) throws IOException {
        pop3Client.connect();
        pop3Client.authenticate(username, password);
        List<Email> emails = pop3Client.getInbox();
        pop3Client.close();
        return emails;
    }

}

package com.example.v_mail.java_mail;

import android.os.Build;

import androidx.annotation.RequiresApi;

import java.io.BufferedReader;
import java.io.BufferedWriter;
import java.io.IOException;
import java.io.InputStreamReader;
import java.io.OutputStreamWriter;
import java.net.Socket;
import java.time.ZonedDateTime;
import java.time.format.DateTimeFormatter;
import java.util.Base64;

public class SmtpClient {
    private final String smtpServer;
    private final int port;
    private Socket socket;
    private BufferedReader reader;
    private BufferedWriter writer;

    public SmtpClient(String smtpServer, int port) {
        this.smtpServer = smtpServer;
        this.port = port;
    }

    public void connect() throws IOException {
        socket = new Socket(smtpServer, port);
        reader = new BufferedReader(new InputStreamReader(socket.getInputStream()));
        writer = new BufferedWriter(new OutputStreamWriter(socket.getOutputStream()));
        System.out.println("SMTP Server: " + reader.readLine());
    }

    public void authenticate(String username, String password) throws IOException {
        sendCommand("EHLO localhost");
        sendCommand("AUTH LOGIN");
        sendCommand(username);
        sendCommand(password);
    }

    @RequiresApi(api = Build.VERSION_CODES.O)
    public void sendEmail(Email email) throws IOException {
        sendCommand("MAIL FROM:<" + email.getFrom() + ">");
        sendCommand("RCPT TO:<" + email.getTo() + ">");
        sendCommand("DATA");

        String message =
                "From: " + email.getFrom() + "\r\n" +
                        "To: " + email.getTo() + "\r\n" +
                        "Subject: " + email.getSubject() + "\r\n" +
                        "Date: " + DateTimeFormatter.RFC_1123_DATE_TIME
                        .format(ZonedDateTime.now()) + "\r\n" +
                        "\r\n" +
                        email.getBody() + "\r\n.";

        sendCommand(message);
    }

    private void sendCommand(String command) throws IOException {
        writer.write(command + "\r\n");
        writer.flush();
        System.out.println("Client: " + command);
        readServerResponse();
    }

    private void readServerResponse() throws IOException {
        String line;
        while ((line = reader.readLine()) != null) {
            System.out.println("Server: " + line);
            if (line.matches("^\\d{3} .*")) {
                break;
            }
        }
    }


    public void close() throws IOException {
        sendCommand("QUIT");
        socket.close();
    }
}


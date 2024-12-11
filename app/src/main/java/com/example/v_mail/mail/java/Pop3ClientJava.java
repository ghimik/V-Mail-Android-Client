package com.example.v_mail.mail.java;

import com.example.v_mail.mail.Email;
import com.example.v_mail.mail.Pop3Client;

import java.io.BufferedReader;
import java.io.BufferedWriter;
import java.io.IOException;
import java.io.InputStreamReader;
import java.io.OutputStreamWriter;
import java.net.Socket;
import java.util.ArrayList;
import java.util.List;

public class Pop3ClientJava implements Pop3Client {
    private final String pop3Server;
    private final int port;
    private Socket socket;
    private BufferedReader reader;
    private BufferedWriter writer;

    public Pop3ClientJava(String pop3Server, int port) {
        this.pop3Server = pop3Server;
        this.port = port;
    }

    public void connect() throws IOException {
        socket = new Socket(pop3Server, port);
        reader = new BufferedReader(new InputStreamReader(socket.getInputStream()));
        writer = new BufferedWriter(new OutputStreamWriter(socket.getOutputStream()));
        System.out.println("POP3 Server: " + reader.readLine());
    }

    public void authenticate(String username, String password) throws IOException {
        sendCommand("USER " + username);
        sendCommand("PASS " + password);
    }

    public List<Email> getInbox() throws IOException {
        sendCommand("LIST");
        List<Email> emails = new ArrayList<>();
        String line;
        int emailCounter = 1;

        while (!(line = reader.readLine()).equals(".")) {
            System.out.println("Server: " + line);
            String[] parts = line.split(" ");
            emailCounter++;
        }

        for (int i = 1; i < emailCounter; i++) {
            emails.add(retrieveEmail(i));
        }
        return emails;
    }

    private Email retrieveEmail(int emailNumber) throws IOException {
        sendCommand("RETR " + emailNumber);
        String from = "", to = "", subject = "", body = "";
        String line;
        while (!(line = reader.readLine()).equals(".")) {
            if (line.startsWith("From:")) from = line.substring(6);
            else if (line.startsWith("To:")) to = line.substring(4);
            else if (line.startsWith("Subject:")) subject = line.substring(9);
            else body += line + "\n";
        }
        return new Email(from, to, subject, body);
    }

    private void sendCommand(String command) throws IOException {
        writer.write(command + "\r\n");
        writer.flush();
        System.out.println("Client: " + command);
        System.out.println("Server: " + reader.readLine());
    }

    public void close() throws IOException {
        sendCommand("QUIT");
        socket.close();
    }
}

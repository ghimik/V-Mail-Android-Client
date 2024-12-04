package com.example.v_mail.java_mail;



public class Email {
    private String from;
    private String to;
    private String subject;
    private String body;

    public Email(String from, String to, String subject, String body) {
        this.from = from;
        this.to = to;
        this.subject = subject;
        this.body = body;
    }

    // Getters
    public String getFrom() { return from; }
    public String getTo() { return to; }
    public String getSubject() { return subject; }
    public String getBody() { return body; }

    @Override
    public String toString() {
        return "From: " + from + "\nTo: " + to + "\nSubject: " + subject + "\n\n" + body;
    }
}


package com.example.v_mail.mail;

import java.io.IOException;

public interface SmtpClient {

    void connect() throws IOException;

    void authenticate(String username, String password) throws IOException;

    void sendEmail(Email email) throws IOException ;

    void close() throws IOException;
}

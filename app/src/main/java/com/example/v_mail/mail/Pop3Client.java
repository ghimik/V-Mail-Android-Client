package com.example.v_mail.mail;

import java.io.IOException;
import java.util.List;

public interface Pop3Client {

    void connect() throws IOException;

    void authenticate(String username, String password) throws IOException;

    List<Email> getInbox() throws IOException;

    void close() throws IOException;

}

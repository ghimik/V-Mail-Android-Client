package com.example.v_mail.mail

import android.content.Context
import android.util.Base64

class MailService private constructor(context: Context) {

    private val sharedPreferences = context.getSharedPreferences("vmail_prefs", Context.MODE_PRIVATE)

    private var pop3Client: Pop3Client? = null
    private var smtpClient: SmtpClient? = null
    private var isSmtpConnected: Boolean = false

    init {
        val usernameBase64 = sharedPreferences.getString("username", null)
        val passwordBase64 = sharedPreferences.getString("password", null)

        if (usernameBase64 != null && passwordBase64 != null) {
            // Декодируем имя пользователя и пароль из Base64
            val username = String(Base64.decode(usernameBase64, Base64.DEFAULT))
            val password = String(Base64.decode(passwordBase64, Base64.DEFAULT))

            pop3Client = Pop3Client()
            smtpClient = SmtpClient()

            // Подключаем POP3 и SMTP с использованием декодированных данных
            pop3Client?.connect("pop3.example.com", 110)
            pop3Client?.authenticate(username, password)

            smtpClient?.connect()
            smtpClient?.authenticate(username, password)
        }
    }

    // Подключение к POP3 серверу
    fun connectPop3(port: Int) {
        pop3Client?.connect("pop3.example.com", port)
    }

    // Получение списка сообщений
    fun listMessages(): Array<String>? {
        return pop3Client?.listEmails()
    }

    // Подключение к SMTP серверу
    fun connectSmtp(): Boolean {
        try {smtpClient?.connect(); }
        catch (e: Exception) {
            isSmtpConnected = false
            return false
        }
        isSmtpConnected = true
        return true
    }

    // Отправка письма
    fun sendEmail(from: String, to: String, subject: String, body: String): Boolean {
        try {
            if (!isSmtpConnected) {
                connectSmtp();
            }

            smtpClient?.sendEmail(from, to, subject, body)
        }
        catch (e: Exception) {
            return false;
        }
        return true
    }

    // Отключение от SMTP сервера
    fun disconnectSmtp() {
        smtpClient?.close()
        isSmtpConnected = false
    }

    // Отключение от POP3 сервера
    fun disconnectPop3() {
        pop3Client?.quit()
    }

    companion object {
        private var instance: MailService? = null

        // Метод для получения единственного экземпляра MailService
        fun getInstance(context: Context): MailService {
            if (instance == null) {
                instance = MailService(context)
            }
            return instance!!
        }
    }
}

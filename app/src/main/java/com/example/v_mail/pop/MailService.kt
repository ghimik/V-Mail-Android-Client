package com.example.v_mail.pop

import android.content.Context
import com.example.v_mail.smtp.SmtpClient

class MailService private constructor(context: Context) {

    private val sharedPreferences = context.getSharedPreferences("vmail_prefs", Context.MODE_PRIVATE)

    private var pop3Client: Pop3Client? = null
    private var smtpClient: SmtpClient? = null
    private var isSmtpConnected: Boolean = false

    init {
        val username = sharedPreferences.getString("username", null)
        val password = sharedPreferences.getString("password", null)
        if (username != null && password != null) {
            pop3Client = Pop3Client("pop3.server.com", username, password)
            smtpClient = SmtpClient("smtp.server.com", 587)
        }
    }

    fun connectPop3(port: Int) {
        pop3Client?.connect(port)
    }

    fun listMessages(): Array<String>? {
        return pop3Client?.listMessages()
    }

    fun connectSmtp(): Boolean {
        isSmtpConnected = smtpClient?.connect() == 0
        return isSmtpConnected
    }

    fun sendEmail(from: String, to: String, subject: String, body: String): Boolean {
        if (!isSmtpConnected) {
            connectSmtp()
        }
        return smtpClient?.sendEmail(from, to, subject, body) == 0
    }

    fun disconnectSmtp() {
        smtpClient?.disconnect()
        isSmtpConnected = false
    }

    fun disconnectPop3() {
        pop3Client?.disconnect()
    }

    companion object {
        private var instance: MailService? = null

        fun getInstance(context: Context): MailService {
            if (instance == null) {
                instance = MailService(context)
            }
            return instance!!
        }
    }
}

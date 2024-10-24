package com.example.v_mail.pop

import android.content.Context

class MailService private constructor(context: Context) {

    private val sharedPreferences = context.getSharedPreferences("vmail_prefs", Context.MODE_PRIVATE)

    private var pop3Client: Pop3Client? = null

    init {
        val username = sharedPreferences.getString("username", null)
        val password = sharedPreferences.getString("password", null)
        if (username != null && password != null) {
            pop3Client = Pop3Client("server_ip", username, password)
        }
    }

    fun connectPop3(port: Int) {
        pop3Client?.connect(port)
    }

    fun listMessages(): Array<String>? {
        return pop3Client?.listMessages()
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

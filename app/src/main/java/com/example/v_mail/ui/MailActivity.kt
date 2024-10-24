package com.example.v_mail.ui

import android.os.Bundle
import android.widget.*
import androidx.appcompat.app.AppCompatActivity
import com.example.v_mail.R
import com.example.v_mail.pop.MailService
import com.example.v_mail.smtp.SmtpClient

class MailActivity : AppCompatActivity() {

    private lateinit var mailListView: ListView
    private lateinit var emailTo: EditText
    private lateinit var emailSubject: EditText
    private lateinit var emailBody: EditText
    private lateinit var sendButton: Button

    private lateinit var mailService: MailService
    private lateinit var smtpClient: SmtpClient

    override fun onCreate(savedInstanceState: Bundle?) {
        super.onCreate(savedInstanceState)
        setContentView(R.layout.activity_mail)

        mailListView = findViewById(R.id.mailListView)
        emailTo = findViewById(R.id.emailTo)
        emailSubject = findViewById(R.id.emailSubject)
        emailBody = findViewById(R.id.emailBody)
        sendButton = findViewById(R.id.sendButton)

        mailService = MailService.getInstance(this)

        val sharedPreferences = getSharedPreferences("vmail_prefs", MODE_PRIVATE)
        val smtpServer = "smtp.server.com"
        val username = sharedPreferences.getString("username", null)
        val password = sharedPreferences.getString("password", null)

        if (username != null && password != null) {
            // smtpClient = SmtpClient(smtpServer, username, password)

            loadMessages()

            sendButton.setOnClickListener {
                val to = emailTo.text.toString()
                val subject = emailSubject.text.toString()
                val body = emailBody.text.toString()

                if (to.isNotEmpty() && subject.isNotEmpty() && body.isNotEmpty()) {
                    // smtpClient.sendEmail(to, subject, body)
                    Toast.makeText(this, "Email sent successfully!", Toast.LENGTH_SHORT).show()
                    clearFields()
                } else {
                    Toast.makeText(this, "Please fill all fields", Toast.LENGTH_SHORT).show()
                }
            }
        } else {
            Toast.makeText(this, "Missing user credentials", Toast.LENGTH_SHORT).show()
        }
    }

    private fun loadMessages() {
        mailService.connectPop3(110)
        val messages = mailService.listMessages()

        if (messages != null) {
            val adapter = ArrayAdapter(this, android.R.layout.simple_list_item_1, messages)
            mailListView.adapter = adapter
        } else {
            Toast.makeText(this, "Failed to load messages", Toast.LENGTH_SHORT).show()
        }
    }

    private fun clearFields() {
        emailTo.text.clear()
        emailSubject.text.clear()
        emailBody.text.clear()
    }
}


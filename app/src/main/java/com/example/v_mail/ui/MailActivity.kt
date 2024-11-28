package com.example.v_mail.ui

import android.os.Bundle
import android.widget.*
import androidx.appcompat.app.AppCompatActivity
import com.example.v_mail.R
import com.example.v_mail.mail.MailService

class MailActivity : AppCompatActivity() {

    private lateinit var mailListView: ListView
    private lateinit var emailTo: EditText
    private lateinit var emailSubject: EditText
    private lateinit var emailBody: EditText
    private lateinit var sendButton: Button

    private lateinit var mailService: MailService

    override fun onCreate(savedInstanceState: Bundle?) {
        super.onCreate(savedInstanceState)
        setContentView(R.layout.activity_mail)

        mailListView = findViewById(R.id.mailListView)
        emailTo = findViewById(R.id.emailTo)
        emailSubject = findViewById(R.id.emailSubject)
        emailBody = findViewById(R.id.emailBody)
        sendButton = findViewById(R.id.sendButton)

        mailService = MailService.getInstance(this)

        loadMessages()

        sendButton.setOnClickListener {
            sendEmail()
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

    private fun sendEmail() {
        val to = emailTo.text.toString()
        val subject = emailSubject.text.toString()
        val body = emailBody.text.toString()
        val username = getSharedPreferences("vmail_prefs", MODE_PRIVATE).getString("username", null)

        if (to.isNotEmpty() && subject.isNotEmpty() && body.isNotEmpty() && username != null) {
            if (mailService.connectSmtp()) {
                val sent = mailService.sendEmail(username, to, subject, body)
                if (sent) {
                    Toast.makeText(this, "Email sent successfully!", Toast.LENGTH_SHORT).show()
                    clearFields()
                } else {
                    Toast.makeText(this, "Failed to send email", Toast.LENGTH_SHORT).show()
                }
                mailService.disconnectSmtp()
            } else {
                Toast.makeText(this, "SMTP connection failed", Toast.LENGTH_SHORT).show()
            }
        } else {
            Toast.makeText(this, "Please fill all fields", Toast.LENGTH_SHORT).show()
        }
    }

    private fun clearFields() {
        emailTo.text.clear()
        emailSubject.text.clear()
        emailBody.text.clear()
    }
}

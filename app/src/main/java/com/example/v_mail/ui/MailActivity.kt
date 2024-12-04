package com.example.v_mail.ui

import android.content.Context
import android.os.Bundle
import android.widget.*
import androidx.appcompat.app.AlertDialog
import androidx.appcompat.app.AppCompatActivity
import androidx.swiperefreshlayout.widget.SwipeRefreshLayout
import com.example.v_mail.R
import com.example.v_mail.java_mail.Email
import com.example.v_mail.java_mail.MailService
import kotlinx.coroutines.*
import java.io.IOException

class MailActivity : AppCompatActivity() {

    private lateinit var mailService: MailService
    private lateinit var username: String
    private lateinit var password: String
    private lateinit var emailFrom: String

    override fun onCreate(savedInstanceState: Bundle?) {
        super.onCreate(savedInstanceState)
        setContentView(R.layout.activity_mail)

        loadUserData()

        mailService = MailService(
            "82.179.140.18",
            25,
            "82.179.140.18",
            110
        )

        val mailListView = findViewById<ListView>(R.id.mailListView)
        val emailTo = findViewById<EditText>(R.id.emailTo)
        val emailSubject = findViewById<EditText>(R.id.emailSubject)
        val emailBody = findViewById<EditText>(R.id.emailBody)
        val sendButton = findViewById<Button>(R.id.sendButton)
        val swipeRefreshLayout = findViewById<SwipeRefreshLayout>(R.id.swipeRefreshLayout)


        loadInbox(mailListView)

        swipeRefreshLayout.setOnRefreshListener {
            loadInbox(mailListView)
            swipeRefreshLayout.isRefreshing = false
        }

        sendButton.setOnClickListener {
            val to = emailTo.text.toString()
            val subject = emailSubject.text.toString()
            val body = emailBody.text.toString()

            if (to.isBlank() || subject.isBlank() || body.isBlank()) {
                Toast.makeText(this, "Please fill in all fields", Toast.LENGTH_SHORT).show()
            } else {
                sendEmail(to, subject, body)
            }
        }
    }

    private fun loadUserData() {
        val sharedPreferences = getSharedPreferences("vmail_prefs", Context.MODE_PRIVATE)
        username = sharedPreferences.getString("username", "") ?: ""
        password = sharedPreferences.getString("password", "") ?: ""
        emailFrom = sharedPreferences.getString("email", "") ?: ""
    }

    private fun loadInbox(mailListView: ListView) {
        CoroutineScope(Dispatchers.IO).launch {
            try {
                val emails = mailService.getInbox(username, password)
                val emailSubjects = emails.map { email -> email.subject }

                withContext(Dispatchers.Main) {
                    val adapter = ArrayAdapter(
                        this@MailActivity,
                        android.R.layout.simple_list_item_1,
                        emailSubjects
                    )
                    mailListView.adapter = adapter

                    mailListView.setOnItemClickListener { _, _, position, _ ->
                        val selectedEmail = emails[position]
                        showEmailDetails(selectedEmail)
                    }
                }
            } catch (e: IOException) {
                e.printStackTrace()
                withContext(Dispatchers.Main) {
                    Toast.makeText(
                        this@MailActivity,
                        "Failed to load inbox: ${e.message}",
                        Toast.LENGTH_SHORT
                    ).show()
                }
            }
        }
    }

    private fun sendEmail(to: String, subject: String, body: String) {
        CoroutineScope(Dispatchers.IO).launch {
            try {
                mailService.sendEmail(
                    emailFrom,
                    to,
                    subject,
                    body,
                    username,
                    password
                )

                withContext(Dispatchers.Main) {
                    Toast.makeText(this@MailActivity, "Email sent successfully!", Toast.LENGTH_SHORT).show()
                    clearComposeFields()
                }
            } catch (e: IOException) {
                e.printStackTrace()
                withContext(Dispatchers.Main) {
                    Toast.makeText(
                        this@MailActivity,
                        "Failed to send email: ${e.message}",
                        Toast.LENGTH_SHORT
                    ).show()
                }
            }
        }
    }

    private fun clearComposeFields() {
        findViewById<EditText>(R.id.emailTo).text.clear()
        findViewById<EditText>(R.id.emailSubject).text.clear()
        findViewById<EditText>(R.id.emailBody).text.clear()
    }

    private fun showEmailDetails(email: Email) {
        val dialog = AlertDialog.Builder(this)
            .setTitle(email.subject)
            .setMessage("From: ${email.from}\n\n${email.body}")
            .setPositiveButton("OK", null)
            .create()
        dialog.show()
    }
}

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

    override fun onResume() {
        super.onResume()

        val mailListView = findViewById<ListView>(R.id.mailListView)
        loadInbox(mailListView)
    }

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
        val dialogView = layoutInflater.inflate(R.layout.dialog_email_details, null)

        val fromTextView = dialogView.findViewById<TextView>(R.id.emailFrom)
        val toTextView = dialogView.findViewById<TextView>(R.id.emailTo)
        val bodyTextView = dialogView.findViewById<TextView>(R.id.emailBody)
        val decodeButton = dialogView.findViewById<Button>(R.id.decodeButton)
        val subjectView = dialogView.findViewById<TextView>(R.id.emailSubject)

        fromTextView.text = "From: ${email.from}"
        toTextView.text = "To: ${email.to}"
        subjectView.text = "Subject: ${email.subject}"
        bodyTextView.text = email.body

        decodeButton.setOnClickListener {
            try {
                val decodedBody = String(android.util.Base64.decode(email.body, android.util.Base64.DEFAULT))
                bodyTextView.text = decodedBody
                Toast.makeText(this, "Decoded successfully", Toast.LENGTH_SHORT).show()
            } catch (e: IllegalArgumentException) {
                Toast.makeText(this, "Failed to decode: Not a valid Base64 string", Toast.LENGTH_SHORT).show()
            }
        }

        AlertDialog.Builder(this)
            .setView(dialogView)
            .setPositiveButton("OK", null)
            .create()
            .show()
    }

}

package com.example.v_mail.ui

import android.os.Bundle
import android.widget.Button
import android.widget.EditText
import android.widget.Toast
import androidx.appcompat.app.AppCompatActivity
import com.example.v_mail.api.ApiClient
import com.example.v_mail.dto.request.AuthorizationRequestDto
import com.example.v_mail.dto.response.AuthorizationResponse
import retrofit2.Call
import retrofit2.Callback
import retrofit2.Response
import com.example.v_mail.R

class LoginActivity : AppCompatActivity() {

    private lateinit var usernameEditText: EditText
    private lateinit var passwordEditText: EditText
    private lateinit var loginButton: Button

    override fun onCreate(savedInstanceState: Bundle?) {
        super.onCreate(savedInstanceState)
        setContentView(R.layout.activity_login)

        usernameEditText = findViewById(R.id.email_input)
        passwordEditText = findViewById(R.id.password_input)
        loginButton = findViewById(R.id.login_button)

        loginButton.setOnClickListener {
            login()
        }
    }

    private fun login() {
        val username = usernameEditText.text.toString()
        val password = passwordEditText.text.toString()

        val requestDto = AuthorizationRequestDto(username, password)

        ApiClient.authApi.login(requestDto).enqueue(object : Callback<AuthorizationResponse> {
            override fun onResponse(
                call: Call<AuthorizationResponse>,
                response: Response<AuthorizationResponse>
            ) {
                if (response.isSuccessful) {
                    val authResponse = response.body()
                    Toast.makeText(
                        this@LoginActivity,
                        "Logged in successfully!",
                        Toast.LENGTH_SHORT
                    ).show()
                } else {
                    Toast.makeText(this@LoginActivity, "Login failed!",
                        Toast.LENGTH_SHORT).show()
                }
            }

            override fun onFailure(call: Call<AuthorizationResponse>, t: Throwable) {
                Toast.makeText(
                    this@LoginActivity,
                    "Network error: ${t.message}",
                    Toast.LENGTH_SHORT
                ).show()
            }
        })
    }
}

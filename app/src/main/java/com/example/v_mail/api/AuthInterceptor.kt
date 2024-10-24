package com.example.v_mail.api

import android.content.Context
import android.content.SharedPreferences
import com.example.v_mail.dto.request.TokenRequest
import com.example.v_mail.dto.response.AuthenticationResponse
import okhttp3.Interceptor
import okhttp3.Response
import java.io.IOException

class AuthInterceptor(private val context: Context) : Interceptor {

    private val sharedPreferences: SharedPreferences =
        context.getSharedPreferences("vmail_prefs", Context.MODE_PRIVATE)

    @Throws(IOException::class)
    override fun intercept(chain: Interceptor.Chain): Response {
        val accessToken = sharedPreferences.getString("auth_token", null)
        val originalRequest = chain.request()

        val builder = originalRequest.newBuilder()
        accessToken?.let {
            builder.header("Authorization", "Bearer $it")
        }

        val requestWithToken = builder.build()
        val response = chain.proceed(requestWithToken)

        if (response.code == 401) {
            synchronized(this) {
                val refreshToken = sharedPreferences.getString("refreshToken", null)
                if (refreshToken != null) {
                    val tokenRequest = TokenRequest(refreshToken)
                    val refreshResponse: retrofit2.Response<AuthenticationResponse> =
                        ApiClient.getAuthApi().refreshToken(tokenRequest).execute()

                    if (refreshResponse.isSuccessful) {
                        val authResponse = refreshResponse.body()
                        authResponse?.let {
                            ApiClient.saveTokens(it.token, it.refreshToken)

                            val newRequest = originalRequest.newBuilder()
                                .header("Authorization", "Bearer ${it.token}")
                                .build()
                            return chain.proceed(newRequest)
                        }
                    }
                }
            }
        }
        return response
    }
}

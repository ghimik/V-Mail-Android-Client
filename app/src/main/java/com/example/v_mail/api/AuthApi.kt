package com.example.v_mail.api

import com.example.v_mail.dto.request.AuthorizationRequestDto
import com.example.v_mail.dto.request.SignUpRequestDto
import com.example.v_mail.dto.request.TokenRequest
import com.example.v_mail.dto.response.AuthenticationResponse
import com.example.v_mail.dto.response.AuthorizationResponse
import retrofit2.Call
import retrofit2.http.Body
import retrofit2.http.POST

interface AuthApi {

    @POST("login")
    fun login(@Body requestDto: AuthorizationRequestDto): Call<AuthorizationResponse>

    @POST("signup")
    fun signup(@Body requestDto: SignUpRequestDto): Call<Void>

    @POST("refresh")
    fun refreshToken(@Body tokenRequest: TokenRequest): Call<AuthenticationResponse>

}
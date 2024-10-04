package com.example.v_mail.dto.response

import kotlinx.serialization.Serializable

@Serializable
data class AuthenticationResponse(
    val token: String,
    val refreshToken: String
)
package com.example.v_mail.dto.request

import kotlinx.serialization.Serializable

@Serializable
data class TokenRequest(
    val refreshToken: String
)
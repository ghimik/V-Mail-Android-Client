package com.example.v_mail.dto.request
import kotlinx.serialization.Serializable


@Serializable
data class AuthorizationRequestDto(
    val username: String,
    val password: String
)
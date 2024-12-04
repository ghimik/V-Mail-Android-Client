package com.example.v_mail.dto.response

import kotlinx.serialization.Serializable

@Serializable
data class AuthorizationResponse(
    val headerName: String = "Authorization",
    val headerValue: String,
    val username: String,
    val password: String,
    val email: String
) {
    companion object {
        const val BEARER = "Bearer "
    }
}
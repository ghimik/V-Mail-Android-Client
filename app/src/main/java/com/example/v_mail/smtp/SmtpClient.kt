package com.example.v_mail.smtp

class SmtpClient(serverIp: String, port: Int) {

    private var nativePtr: Long = 0

    init {
        System.loadLibrary("v_mail")
        nativePtr = nativeInit(serverIp, port)
    }

    private external fun nativeInit(serverIp: String, port: Int): Long
    private external fun nativeConnect(nativePtr: Long): Int
    private external fun nativeSendEmail(nativePtr: Long, from: String, to: String, subject: String, body: String): Int
    private external fun nativeDisconnect(nativePtr: Long)
    private external fun nativeFree(nativePtr: Long)

    fun connect(): Int {
        return nativeConnect(nativePtr)
    }

    fun sendEmail(from: String, to: String, subject: String, body: String): Int {
        return nativeSendEmail(nativePtr, from, to, subject, body)
    }

    fun disconnect() {
        nativeDisconnect(nativePtr)
    }

    fun free() {
        nativeFree(nativePtr)
    }

    protected fun finalize() {
        free()
    }
}

package com.example.v_mail.pop

class Pop3Client(private val serverIp: String, private val username: String, private val password: String) {

    private var nativePtr: Long = 0

    init {
        System.loadLibrary("v_mail")
        nativePtr = nativeInit(serverIp, username, password)
    }

    private external fun nativeInit(serverIp: String, username: String, password: String): Long
    private external fun nativeConnect(nativePtr: Long, port: Int)
    private external fun nativeList(nativePtr: Long): Array<String>
    private external fun nativeDisconnect(nativePtr: Long)
    private external fun nativeFree(nativePtr: Long)

    fun connect(port: Int) {
        nativeConnect(nativePtr, port)
    }

    fun listMessages(): Array<String> {
        return nativeList(nativePtr)
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

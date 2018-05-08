package com.kwange.libffmpeg

import android.view.Surface

object FfmpegUtils {
    init {
        System.loadLibrary("native-lib")
    }

    external fun stringFromJNI(): String

    external fun release(decoder: Long)

    external fun getDecoder(): Long

    external fun decodeFrame(data: ByteArray, length: Int, decoder: Long, surface: Surface)


}


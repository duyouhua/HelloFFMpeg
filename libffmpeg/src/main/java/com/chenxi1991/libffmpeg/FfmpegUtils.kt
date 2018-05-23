package com.chenxi1991.libffmpeg

import android.view.Surface

object FfmpegUtils {
    init {
        System.loadLibrary("ffmpegutils")
    }

    external fun init(): Boolean
    external fun parse(buf: ByteArray, size: Int): Boolean
    external fun release(): Boolean
    external fun setSurface(surface: Surface)
}


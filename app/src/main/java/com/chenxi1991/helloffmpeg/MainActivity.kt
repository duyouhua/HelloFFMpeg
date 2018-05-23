package com.chenxi1991.helloffmpeg

import android.support.v7.app.AppCompatActivity
import android.os.Bundle
import com.chenxi1991.libffmpeg.FfmpegUtils

class MainActivity : AppCompatActivity() {

    override fun onCreate(savedInstanceState: Bundle?) {
        super.onCreate(savedInstanceState)
        setContentView(R.layout.activity_main)
        FfmpegUtils.init()
    }


}

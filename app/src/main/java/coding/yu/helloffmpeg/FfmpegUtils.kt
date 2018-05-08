package coding.yu.helloffmpeg

import android.view.Surface

object FfmpegUtils {
    init {
        System.loadLibrary("native-lib")
    }

    external fun stringFromJNI(): String

    external fun release(decoder: Long)

    external fun getDecoder():Long

    external fun decodeStream(data:ByteArray,length:Int,decoder:Long,surface:Surface)


}

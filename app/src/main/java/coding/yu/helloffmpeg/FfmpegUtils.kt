package coding.yu.helloffmpeg

class FfmpegUtils private constructor() {
    companion object {

        private var singleTon: FfmpegUtils? = null

        fun instance(): FfmpegUtils {
            if (singleTon == null) {
                synchronized(FfmpegUtils::class.java) {
                    if (singleTon == null)
                        singleTon = FfmpegUtils()
                }
            }
            return singleTon!!
        }

        init {
            System.loadLibrary("native-lib")
        }
    }

    external fun stringFromJNI(): String

    external fun initialize()

    external fun close()
}

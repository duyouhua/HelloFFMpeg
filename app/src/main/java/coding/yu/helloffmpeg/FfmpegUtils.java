package coding.yu.helloffmpeg;

public class FfmpegUtils {
    private FfmpegUtils() {
    }

    private static FfmpegUtils singleTon = null;

    public static FfmpegUtils instance() {
        if (singleTon == null) {
            synchronized (FfmpegUtils.class) {
                if (singleTon == null)
                    singleTon = new FfmpegUtils();
            }
        }
        return singleTon;
    }

    static {
        System.loadLibrary("native-lib");
    }

    public native String stringFromJNI();

    public native String string2FromJNI();

    public native void initialize();
}

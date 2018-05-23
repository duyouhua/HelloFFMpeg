#include <jni.h>
#include <string>
#include <sys/types.h>
#include <pthread.h>
#include <android/native_window.h>
#include <android/native_window_jni.h>

extern "C" {
#include "libavcodec/avcodec.h"
#include "libavformat/avformat.h"
#include "libavfilter/avfilter.h"
#include "libswscale/swscale.h"
#include "libavutil/imgutils.h"
}
// Log
#ifdef ANDROID

#include <jni.h>
#include <android/log.h>

#define LOGE(format, ...)  __android_log_print(ANDROID_LOG_ERROR, "DJI", format, ##__VA_ARGS__)
#define LOGD(format, ...)  __android_log_print(ANDROID_LOG_DEBUG, "DJI", format, ##__VA_ARGS__)
#else
#define LOGE(format, ...)  printf("DJI" format "\n", ##__VA_ARGS__)
#define LOGD(format, ...)  printf("DJI" format "\n", ##__VA_ARGS__)
#endif

int isFFmpegInitialized;

AVFrame *m_pYUVFrame;
AVFrame *m_pRGBAFrame;
AVCodecContext *m_pCodecCtx;
AVCodec *m_pAVCodec;
AVCodecParserContext *m_pCodecPaser;

uint8_t *buffer;
struct SwsContext *sws_ctx;

//FIX
struct URLProtocol;

uint8_t audbuffer2[] = {0x00, 0x00, 0x00, 0x01, 0x09, 0x10};
uint8_t audsize2 = 6;
uint8_t fillerbuffer2[] = {0x00, 0x00, 0x00, 0x01, 0x0C, 0x00, 0x00, 0x00, 0x01, 0x09, 0x10};
uint8_t fillersize2 = 11;
uint8_t audaudbuffer2[] = {0x00, 0x00, 0x00, 0x01, 0x09, 0x10, 0x00, 0x00, 0x00, 0x01, 0x09, 0x10};
uint8_t audaudsize2 = 12;

ANativeWindow *mNativeWindow;
ANativeWindow_Buffer mWindwoBuffer;

extern "C"
JNIEXPORT jboolean JNICALL
Java_com_chenxi1991_libffmpeg_FfmpegUtils_init(JNIEnv *env, jobject instance);

extern "C"
JNIEXPORT jboolean JNICALL
Java_com_chenxi1991_libffmpeg_FfmpegUtils_parse(JNIEnv *env, jobject instance, jbyteArray pBuff,
                                            int size);

extern "C"
JNIEXPORT jboolean JNICALL
Java_com_chenxi1991_libffmpeg_FfmpegUtils_release(JNIEnv *env, jobject instance);

extern "C"
JNIEXPORT void JNICALL
Java_com_chenxi1991_libffmpeg_FfmpegUtils_setSurface(JNIEnv *env, jobject instance, jobject jsurface);
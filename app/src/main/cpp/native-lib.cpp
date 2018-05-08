#include <jni.h>
#include <string>
#include "decoder.h"
#include "yuv_2_rgb.h"
#include "android_native_window.h"


typedef struct _EnvPackage {
    JNIEnv *env;
    jobject *obj;
    jobject *surface;
} EnvPackage;


void handleData(AVFrame *pFrame, void *param, void *ctx) {
    RenderParam *renderParam = static_cast<RenderParam *>(param);
    AVFrame *rgbFrame = yuv420p_2_argb(pFrame, renderParam->swsContext, renderParam->avCodecContext,
                                       AV_PIX_FMT_VDPAU_H264);
    EnvPackage *envPackage = static_cast<EnvPackage *>(ctx);
    ANativeWindow *aNativeWindow = ANativeWindow_fromSurface(envPackage->env,
                                                             *(envPackage->surface));
    VoutInfo voutInfo;
    voutInfo.buffer = rgbFrame->data[0];
    voutInfo.buffer_width = static_cast<uint32_t>(rgbFrame->width);
    voutInfo.buffer_height = static_cast<uint32_t>(rgbFrame->height);
    voutInfo.pix_format = PIXEL_FORMAT_RGB_565;
    android_native_window_display(aNativeWindow, &voutInfo);
    ANativeWindow_release(aNativeWindow);
    av_free(rgbFrame->data[0]);
    av_free(rgbFrame);
}

extern "C"
JNIEXPORT jstring JNICALL
Java_coding_yu_helloffmpeg_FfmpegUtils_stringFromJNI(JNIEnv *env, jobject instance) {
    std::string hello = "Hello from C++";
    return env->NewStringUTF(hello.c_str());
}

//关闭解码器释放资源
extern "C"
JNIEXPORT void JNICALL
Java_coding_yu_helloffmpeg_FfmpegUtils_release(JNIEnv *env, jobject instance, jlong this_obj_long) {
    decoder *decoder = reinterpret_cast<decoder *>(this_obj_long);
    decoder->close();
    delete (decoder);
}

//获取解码器
extern "C"
JNIEXPORT long JNICALL
Java_coding_yu_helloffmpeg_FfmpegUtils_getDecoder(JNIEnv *env, jobject instance) {
    decoder *decoder = new decoder();
    decoder->initialize(AV_PIX_FMT_VDPAU_H264);
    return reinterpret_cast<long>(decoder);
}

//解码数据并显示
extern "C"
JNIEXPORT void JNICALL
Java_coding_yu_helloffmpeg_FfmpegUtils_deoodeFrame(JNIEnv *env, jobject instance, jbyteArray data,
                                                   jint length, jlong this_obj_long,
                                                   jobject surface) {
    decoder *decoder = reinterpret_cast<decoder *>(this_obj_long);
    jbyte *cdata = env->GetByteArrayElements(data, JNI_FALSE);
    jbyte *cdata_rec = cdata;
    if (cdata != NULL) {
        EnvPackage package;
        package.env = env;
        package.obj = &instance;
        package.surface = &surface;
        int len = 0;
        while (1) {
            if (length > INBUF_SIZE) {
                len = INBUF_SIZE;
                length -= INBUF_SIZE;
            } else if (length > 0 && length <= INBUF_SIZE) {
                len = length;
                length = 0;
            } else {
                break;
            }
            decoder->decodeFrame(reinterpret_cast<const char *>(cdata), len, handleData, &package);
        }
    } else {

    }
    env->ReleaseByteArrayElements(data, cdata_rec, 0);
}




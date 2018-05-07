#include <jni.h>
#include <string>
#include "decoder.h"

decoder *mDecoder;

extern "C"
JNIEXPORT jstring JNICALL
Java_coding_yu_helloffmpeg_FfmpegUtils_stringFromJNI(JNIEnv *env, jobject instance) {
    std::string hello = "Hello from C++";
    return env->NewStringUTF(hello.c_str());
}

//初始化
extern "C"
JNIEXPORT void JNICALL
Java_coding_yu_helloffmpeg_FfmpegUtils_initialize(JNIEnv *env, jobject instance) {
    mDecoder = new decoder();
    mDecoder->initialize(AV_PIX_FMT_VDPAU_H264);
}

//关闭解码器释放资源
extern "C"
JNIEXPORT void JNICALL
Java_coding_yu_helloffmpeg_FfmpegUtils_close(JNIEnv *env, jobject instance) {
    mDecoder->close();
    delete (mDecoder);
}

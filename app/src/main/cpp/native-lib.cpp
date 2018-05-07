#include <jni.h>
#include <string>
#include "decoder.h"

extern "C"
JNIEXPORT jstring JNICALL
Java_coding_yu_helloffmpeg_FfmpegUtils_string2FromJNI(JNIEnv *env, jobject instance) {

    char info[10000] = {0};
    sprintf(info, "%s\n", avcodec_configuration());

    return env->NewStringUTF(info);
}

extern "C"
JNIEXPORT jstring JNICALL
Java_coding_yu_helloffmpeg_FfmpegUtils_stringFromJNI(
        JNIEnv *env,
        jobject /* this */) {
    std::string hello = "Hello from C++";
    return env->NewStringUTF(hello.c_str());
}

extern "C"
JNIEXPORT void JNICALL
Java_coding_yu_helloffmpeg_FfmpegUtils_initialize() {
    decoder().initialize(AV_PIX_FMT_VDPAU_H264);
}

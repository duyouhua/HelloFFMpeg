#include <jni.h>
#include <string>

extern "C" {
#include "libavcodec/avcodec.h"
}

extern "C"
JNIEXPORT jstring JNICALL
Java_coding_yu_helloffmpeg_MainActivity_string2FromJNI(JNIEnv *env, jobject instance) {

    char info[10000] = {0};
    sprintf(info, "%s\n", avcodec_configuration());

    return env->NewStringUTF(info);
}

extern "C"
JNIEXPORT jstring JNICALL
Java_coding_yu_helloffmpeg_MainActivity_stringFromJNI(
        JNIEnv *env,
        jobject /* this */) {
    std::string hello = "Hello from C++";
    return env->NewStringUTF(hello.c_str());
}

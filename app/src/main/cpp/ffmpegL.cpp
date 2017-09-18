#include <jni.h>
#include <string>
#include <android/log.h>
extern "C" {
//编码
#include "libavcodec/avcodec.h"
//封装格式处理
#include "libavformat/avformat.h"
//像素处理
#include "libswscale/swscale.h"

#define TAG "LOUIS_LOG"
#define LOGE(...) __android_log_print(ANDROID_LOG_ERROR, TAG, __VA_ARGS__)

JNIEXPORT jstring JNICALL
Java_ilouis_me_louisffmpeg_MainActivity_stringFromJNI(
        JNIEnv *env,
        jobject /* this */) {
    std::string hello = "Hello from C++";
    av_register_all();
    LOGE("初始化完成");
    return env->NewStringUTF(hello.c_str());
}
}
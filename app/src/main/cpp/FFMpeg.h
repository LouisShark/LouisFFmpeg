//
// Created by user on 2017/9/26.
//

#ifndef LOUISFFMPEG_FFMPEG_H
#define LOUISFFMPEG_FFMPEG_H


extern "C" {
#include <android/log.h>
#include <assert.h>
#include <android/native_window_jni.h>
#include <unistd.h>
#include <libswresample/swresample.h>
//编码
#include "libavcodec/avcodec.h"
//封装格式处理
#include "libavformat/avformat.h"
//像素处理
#include "libswscale/swscale.h"

#define TAG "LOUIS_LOG"
#define LOGE(...) __android_log_print(ANDROID_LOG_ERROR, TAG, __VA_ARGS__)

int createFFmpeg(int *rate, int *channel);

void getPCM(void **outBuffer, size_t *size);

void realase();

#endif //LOUISFFMPEG_FFMPEG_H
}
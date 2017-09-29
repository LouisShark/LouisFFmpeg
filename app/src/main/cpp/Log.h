//
// Created by user on 2017/9/28.
//

#ifndef LOUISFFMPEG_LOG_H
#define LOUISFFMPEG_LOG_H

#include <android/log.h>
#define TAG "LOUIS_LOG"
#define LOGE(...) __android_log_print(ANDROID_LOG_ERROR, TAG, __VA_ARGS__)
#endif //LOUISFFMPEG_LOG_H

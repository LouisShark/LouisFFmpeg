//
// Created by user on 2017/9/28.
//

#ifndef LOUISFFMPEG_FFMPEGAUDIO_H
#define LOUISFFMPEG_FFMPEGAUDIO_H
#include <queue>
#include <SLES/OpenSLES_Android.h>
#include <jni.h>
extern "C"
{
#include <unistd.h>
#include <libavcodec/avcodec.h>
#include <pthread.h>
#include <libswresample/swresample.h>
#include <libavformat/avformat.h>
#include "Log.h"

class FFmpegAudio{
public:
    void setAvCodecContext(AVCodecContext *codecContext);

    int get(AVPacket *packet);

    int put(AVPacket *packet);

    void play();

    void stop();

    int createPlayer();

    FFmpegAudio();

    ~FFmpegAudio();
//成员变量
public:
//    是否正在播放
    int isPlay;
//    流索引
    int index;
//音频队列
    std::queue<AVPacket *> queue;
//    处理线程
    pthread_t p_playid;
    AVCodecContext *codec;

//    同步锁
    pthread_mutex_t mutex;
//    条件变量
    pthread_cond_t cond;
    /**
     * 新增
     */
    SwrContext *swrContext;
    uint8_t *out_buffer;
    int out_channer_nb;
//    相对于第一帧时间
    double clock;

    AVRational time_base;

    SLObjectItf engineObject;
    SLEngineItf engineEngine;
    SLEnvironmentalReverbItf outputMixEnvironmentalReverb;
    SLObjectItf outputMixObject;
    SLObjectItf bqPlayerObject;
    SLEffectSendItf bqPlayerEffectSend;
    SLVolumeItf bqPlayerVolume;
    SLPlayItf bqPlayerPlay;
    SLAndroidSimpleBufferQueueItf bqPlayerBufferQueue;


};

};
#endif //LOUISFFMPEG_FFMPEGAUDIO_H

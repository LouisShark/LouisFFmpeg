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
JNIEXPORT void JNICALL
Java_ilouis_me_louisffmpeg_MainActivity_open(JNIEnv *env, jobject instance, jstring inputPath_,
                                             jstring outPath_) {
    const char *inputPath = env->GetStringUTFChars(inputPath_, 0);
    const char *outPath = env->GetStringUTFChars(outPath_, 0);

    LOGE("初始化组件");
    av_register_all();
    AVFormatContext *pContext = avformat_alloc_context();
    if (avformat_open_input(&pContext, inputPath, NULL, NULL) < 0) {
        LOGE("打开失败");
        return;
    }
    if (avformat_find_stream_info(pContext, NULL) < 0) {
        LOGE("获取信息失败");
        return;
    }
    int video_stream_index = -1;
    //找到视频流
    for (int i = 0; i < pContext->nb_streams; ++i) {
        LOGE("循环 %d", i);
        //codec每一个流对于的解码上下文 codec_type流的类型
        if (pContext->streams[i]->codec->codec_type == AVMEDIA_TYPE_VIDEO) {
            video_stream_index = i;
        }
    }
    //获取解码器上下文
    AVCodecContext *pCodeContext = pContext->streams[video_stream_index]->codec;

    //获取解码器
    AVCodec *avCodec = avcodec_find_decoder(pCodeContext->codec_id);

    if (avcodec_open2(pCodeContext, avCodec, NULL) < 0) {
        LOGE("解码失败");
        return;
    }
    //分配内存
    AVPacket *packet = (AVPacket *) av_malloc(sizeof(AVPacket));
    //初始化结构体
    av_init_packet(packet);
    AVFrame *avFrame = av_frame_alloc();

    //在声明一个YUVframe
    AVFrame *yuvFrame = av_frame_alloc();
    //初始化缓冲区
    uint8_t *out_bufftr = (uint8_t *) av_malloc(
            avpicture_get_size(AV_PIX_FMT_YUV420P, pCodeContext->width, pCodeContext->height));
    //填充缓冲区
    avpicture_fill((AVPicture *) yuvFrame, out_bufftr, AV_PIX_FMT_YUV420P, pCodeContext->width,
                   pCodeContext->height);
    LOGE("width: %d, height: %d", pCodeContext->width, pCodeContext->height);
    SwsContext *swsContext = sws_getContext(pCodeContext->width, pCodeContext->height,
                                            pCodeContext->pix_fmt, pCodeContext->width,
                                            pCodeContext->height, AV_PIX_FMT_YUV420P, SWS_BILINEAR,
                                            NULL, NULL, NULL);
    int frame_count = 0;
    FILE *fp_yuv = fopen(outPath, "wb");

    int get_frame;
    while (av_read_frame(pContext, packet) >= 0) {
        //解封装 根据frame 进行原生绘制
        avcodec_decode_video2(pCodeContext, avFrame, &get_frame, packet);
        //拿到frame的数据 视频像素数据 yuv
        LOGE("解码%d", frame_count++);
        if (get_frame > 0) {
            sws_scale(swsContext, (const uint8_t *const *) avFrame->data, avFrame->linesize, 0, avFrame->height, yuvFrame->data, yuvFrame->linesize);

            int y_size = pCodeContext->width * pCodeContext->height;
            //y亮度信息写完
            fwrite(yuvFrame->data[0], 1, y_size, fp_yuv);
            fwrite(yuvFrame->data[1], 1, y_size / 4, fp_yuv);
            fwrite(yuvFrame->data[2], 1, y_size / 4, fp_yuv);
        }
        av_free_packet(packet);
    }
    fclose(fp_yuv);
    av_frame_free(&avFrame);
    av_frame_free(&yuvFrame);
    avcodec_close(pCodeContext);
    avformat_free_context(pContext);
    env->ReleaseStringUTFChars(inputPath_, inputPath);
    env->ReleaseStringUTFChars(outPath_, outPath);
}
}
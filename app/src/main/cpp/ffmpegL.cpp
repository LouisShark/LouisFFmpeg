#include <jni.h>
#include <string>
#include <android/log.h>
#include <assert.h>
#include <android/native_window_jni.h>
#include <unistd.h>
extern "C" {
#include <libswresample/swresample.h>
//编码
#include "libavcodec/avcodec.h"
//封装格式处理
#include "libavformat/avformat.h"
//像素处理
#include "libswscale/swscale.h"

#define TAG "LOUIS_LOG"
#define LOGE(...) __android_log_print(ANDROID_LOG_ERROR, TAG, __VA_ARGS__)
#define NELEM(x) ((int) (sizeof(x) / sizeof((x)[0])))

//JNIEXPORT void JNICALL
//Java_ilouis_me_louisffmpeg_MainActivity_open(JNIEnv *env, jobject instance, jstring inputPath_,
//                                             jstring outPath_) {
//    const char *inputPath = env->GetStringUTFChars(inputPath_, 0);
//    const char *outPath = env->GetStringUTFChars(outPath_, 0);
//
//    LOGE("初始化组件");
//    av_register_all();
//    AVFormatContext *pContext = avformat_alloc_context();
//    if (avformat_open_input(&pContext, inputPath, NULL, NULL) < 0) {
//        LOGE("打开失败");
//        return;
//    }
//    if (avformat_find_stream_info(pContext, NULL) < 0) {
//        LOGE("获取信息失败");
//        return;
//    }
//    int video_stream_index = -1;
//    //找到视频流
//    for (int i = 0; i < pContext->nb_streams; ++i) {
//        LOGE("循环 %d", i);
//        //codec每一个流对于的解码上下文 codec_type流的类型
//        if (pContext->streams[i]->codec->codec_type == AVMEDIA_TYPE_VIDEO) {
//            video_stream_index = i;
//        }
//    }
//    //获取解码器上下文
//    AVCodecContext *pCodeContext = pContext->streams[video_stream_index]->codec;
//
//    //获取解码器
//    AVCodec *avCodec = avcodec_find_decoder(pCodeContext->codec_id);
//
//    if (avcodec_open2(pCodeContext, avCodec, NULL) < 0) {
//        LOGE("解码失败");
//        return;
//    }
//    //分配内存
//    AVPacket *packet = (AVPacket *) av_malloc(sizeof(AVPacket));
//    //初始化结构体
//    av_init_packet(packet);
//    AVFrame *avFrame = av_frame_alloc();
//
//    //在声明一个YUVframe
//    AVFrame *yuvFrame = av_frame_alloc();
//    //初始化缓冲区
//    uint8_t *out_bufftr = (uint8_t *) av_malloc(
//            avpicture_get_size(AV_PIX_FMT_YUV420P, pCodeContext->width, pCodeContext->height));
//    //填充缓冲区
//    avpicture_fill((AVPicture *) yuvFrame, out_bufftr, AV_PIX_FMT_YUV420P, pCodeContext->width,
//                   pCodeContext->height);
//    LOGE("width: %d, height: %d", pCodeContext->width, pCodeContext->height);
//    SwsContext *swsContext = sws_getContext(pCodeContext->width, pCodeContext->height,
//                                            pCodeContext->pix_fmt, pCodeContext->width,
//                                            pCodeContext->height, AV_PIX_FMT_YUV420P, SWS_BILINEAR,
//                                            NULL, NULL, NULL);
//    int frame_count = 0;
//    FILE *fp_yuv = fopen(outPath, "wb");
//
//    int get_frame;
//    while (av_read_frame(pContext, packet) >= 0) {
//        //解封装 根据frame 进行原生绘制
//        avcodec_decode_video2(pCodeContext, avFrame, &get_frame, packet);
//        //拿到frame的数据 视频像素数据 yuv
//        LOGE("解码%d", frame_count++);
//        if (get_frame > 0) {
//            sws_scale(swsContext, (const uint8_t *const *) avFrame->data, avFrame->linesize, 0, avFrame->height, yuvFrame->data, yuvFrame->linesize);
//
//            int y_size = pCodeContext->width * pCodeContext->height;
//            //y亮度信息写完
//            fwrite(yuvFrame->data[0], 1, y_size, fp_yuv);
//            fwrite(yuvFrame->data[1], 1, y_size / 4, fp_yuv);
//            fwrite(yuvFrame->data[2], 1, y_size / 4, fp_yuv);
//        }
//        av_free_packet(packet);
//    }
//    fclose(fp_yuv);
//    av_frame_free(&avFrame);
//    av_frame_free(&yuvFrame);
//    avcodec_close(pCodeContext);
//    avformat_free_context(pContext);
//    env->ReleaseStringUTFChars(inputPath_, inputPath);
//    env->ReleaseStringUTFChars(outPath_, outPath);
//}

JNIEXPORT void JNICALL startPlaying(JNIEnv *env, jobject jobj, jstring input_str, jobject surface) {
    LOGE("--------------------");
    const char *input_path = env->GetStringUTFChars(input_str,NULL);

    if (input_path == NULL) {
        LOGE("path？？？");
        return;
    }
    av_register_all();
    AVFormatContext * pFormatContext = avformat_alloc_context();
    if (avformat_open_input(&pFormatContext, input_path, NULL, NULL) != 0) {
        LOGE("打开文件失败！！！");
        return;
    }
    //获取视频信息
    if (avformat_find_stream_info(pFormatContext, NULL) < 0){
        LOGE("获取视频信息失败");
        return;
    }
    int video_stream_id = -1;
    for (int i = 0; i < pFormatContext->nb_streams; ++i) {
        if (pFormatContext->streams[i]->codec->codec_type == AVMEDIA_TYPE_VIDEO) {
            video_stream_id = i;
            LOGE("找到视频流 id: %d", pFormatContext->streams[i]->codec->codec_type);
            break;
        }
    }
    //获取视频编解码器
    AVCodecContext *pCodeCtx = pFormatContext->streams[video_stream_id]->codec;
    LOGE("获取视频编解码器上下文%p ", pCodeCtx);
    //加密的无法使用
    AVCodec *pCodec = avcodec_find_decoder(pCodeCtx->codec_id);
    LOGE("获取视频编码 %p", pCodec);
    if (avcodec_open2(pCodeCtx, pCodec, NULL) < 0){

    }
//        return;

    AVPacket *packet = (AVPacket *) av_malloc(sizeof(AVPacket));
    av_init_packet(packet);
    //像素数据
    AVFrame *frame;
    frame = av_frame_alloc();

    //RGB
    AVFrame *rgb_frame = av_frame_alloc();

    //给缓冲区分配内存
    //只有指定了AVFrame的像素格式丶画面大小才能真正分配内存
    //缓冲区分配内存
    uint8_t *out_buffer = (uint8_t *) av_malloc(avpicture_get_size(AV_PIX_FMT_RGBA, pCodeCtx->width, pCodeCtx->height));
    LOGE("width : %d, height: %d ", pCodeCtx->width, pCodeCtx->height);

    //设置RGBAframe的缓冲区，像素格式
    int re = avpicture_fill((AVPicture *) rgb_frame, out_buffer, AV_PIX_FMT_RGBA, pCodeCtx->width, pCodeCtx->height);
    LOGE("申请内存大小%d ", re);

    int length = 0;
    int got_frame;
    int frame_count = 0;
    SwsContext *swsContext = sws_getContext(pCodeCtx->width, pCodeCtx->height, pCodeCtx->pix_fmt, pCodeCtx->width, pCodeCtx->height, AV_PIX_FMT_RGBA, SWS_BICUBIC, NULL, NULL, NULL);
    ANativeWindow *nativeWindow = ANativeWindow_fromSurface(env, surface);

    //视频缓冲区
    ANativeWindow_Buffer window_buffer;
    while (av_read_frame(pFormatContext, packet) >= 0) {
        if (packet->stream_index == video_stream_id) {
            length = avcodec_decode_video2(pCodeCtx, frame, &got_frame, packet);
                LOGE("获得长度 %d", length);


                //开始解码
                if (got_frame) {
                    //绘制前配置信息 比如宽高 格式
                    ANativeWindow_setBuffersGeometry(nativeWindow, pCodeCtx->width, pCodeCtx->height, WINDOW_FORMAT_RGBA_8888);

                    //绘制
                    ANativeWindow_lock(nativeWindow, &window_buffer, NULL);
                    LOGE("解码 %d 帧", frame_count++);
                    //转为指定的编码
                    sws_scale(swsContext, (const uint8_t *const *) frame->data, frame->linesize, 0, pCodeCtx->height, rgb_frame->data, rgb_frame->linesize);
                    //rgba_frame 有画面数据
                    uint8_t *dst = (uint8_t *) window_buffer.bits;
                    //拿到一行有多少个字节 RGBA
                    int dest_stride = window_buffer.stride * 4;
                    //像素数据的首地址
                    uint8_t *src = rgb_frame->data[0];
                    //实际内存一行数量
                    int src_stride = rgb_frame->linesize[0];
                    for (int i = 0; i < pCodeCtx->height; ++i) {
                        memcpy(dst + i *dest_stride, src + i * src_stride, src_stride);
                    }
                    ANativeWindow_unlockAndPost(nativeWindow);
                    usleep(1000 * 16);

                }
            }
            av_free_packet(packet);
    }
    ANativeWindow_release(nativeWindow);
    av_frame_free(&frame);
    av_frame_free(&rgb_frame);
    avcodec_close(pCodeCtx);
    avformat_free_context(pFormatContext);


    env->ReleaseStringUTFChars(input_str, input_path);
}

static const JNINativeMethod gMethods[] = {
        {
                "render","(Ljava/lang/String;Landroid/view/Surface;)V",(void*)startPlaying
        },

};

static int registerNatives(JNIEnv* engv)
{
    LOGE("registerNatives begin");
    jclass  clazz;
    clazz = engv -> FindClass("ilouis/me/louisffmpeg/VideoView");

    if (clazz == NULL) {
        LOGE("clazz is null");
        return JNI_FALSE;
    }

    if (engv ->RegisterNatives(clazz, gMethods, NELEM(gMethods)) < 0) {
        LOGE("RegisterNatives error");
        return JNI_FALSE;
    }

    return JNI_TRUE;
}

JNIEXPORT jint JNI_OnLoad(JavaVM* vm, void* reserved)
{

    LOGE("jni_OnLoad begin");

    JNIEnv* env = NULL;
    jint result = -1;

    if (vm->GetEnv((void**) &env, JNI_VERSION_1_4) != JNI_OK) {
        LOGE("ERROR: GetEnv failed\n");
        return -1;
    }
    assert(env != NULL);

    registerNatives(env);

    return JNI_VERSION_1_4;
}





}
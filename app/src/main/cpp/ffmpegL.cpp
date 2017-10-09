#include <jni.h>
#include <string>
#include <SLES/OpenSLES.h>
#include <SLES/OpenSLES_Android.h>
#include <android/log.h>
#include <assert.h>
#include <android/native_window_jni.h>
#include <unistd.h>
#include "FFmpegAudio.h"
#include "FFmpegVideo.h"

extern "C" {
#include <libswresample/swresample.h>
//编码
#include "libavcodec/avcodec.h"
//封装格式处理
#include "libavformat/avformat.h"
//像素处理
#include "libswscale/swscale.h"
#include "FFMpeg.h"
#include "Log.h"
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

JNIEXPORT void JNICALL startMusic(JNIEnv *env, jobject jobj, jstring input_str, jstring output_str) {
    const char *input_path = env->GetStringUTFChars(input_str,NULL);
    const char *output_path = env->GetStringUTFChars(output_str,NULL);
    av_register_all();

    AVFormatContext *pFormatCtx = avformat_alloc_context();
    if (avformat_open_input(&pFormatCtx, input_path, NULL, NULL) != 0) {
        LOGE("打开文件失败");
    }
    if (avformat_find_stream_info(pFormatCtx, NULL) < 0) {
        LOGE("获取视频信息失败");
        return;
    }
    int audio_stream_index = -1;
    for (int i = 0; i < pFormatCtx->nb_streams; ++i) {
        if (pFormatCtx->streams[i]->codec->codec_type == AVMEDIA_TYPE_AUDIO) {
            LOGE("找到音频 id %d", pFormatCtx->streams[i]->codec->codec_type);
            audio_stream_index = i;
            break;
        }
    }
    //mp3解码器
    AVCodecContext *pCodeCtx = pFormatCtx->streams[audio_stream_index]->codec;
    LOGE("获取音频解码器上下文 %p", pCodeCtx);
    AVCodec *pCodec = avcodec_find_decoder(pCodeCtx->codec_id);

    if (avcodec_open2(pCodeCtx, pCodec, NULL) <0) {
        return;
    }
    AVPacket *packet = (AVPacket *) av_malloc(sizeof(AVPacket));
    AVFrame *frame;
    frame = av_frame_alloc();

    SwrContext *swrContext = swr_alloc();

    int length = 0;
    int got_frame;
    uint8_t *out_buffer = (uint8_t *) av_malloc(44100 * 2);

    uint64_t out_ch_layout = AV_CH_LAYOUT_STEREO;

    //输出采样位数 16位
    enum AVSampleFormat out_format = AV_SAMPLE_FMT_S16;
    int out_sample_rate = pCodeCtx->sample_rate;


    swr_alloc_set_opts(swrContext, out_ch_layout, out_format, out_sample_rate, pCodeCtx->channel_layout, pCodeCtx->sample_fmt, pCodeCtx->sample_rate, 0, NULL);
    swr_init(swrContext);
    //获取通道数 2
    int out_channel_nb = av_get_channel_layout_nb_channels(AV_CH_LAYOUT_STEREO);
    //反射得到createAudio方法
    jclass player = env->GetObjectClass(jobj);
    jmethodID  createAudioID = env->GetMethodID(player, "createAudio", "(II)V");
    env->CallVoidMethod(jobj, createAudioID, 44100, out_channel_nb);
    jmethodID audio_write = env->GetMethodID(player, "playTrack", "([BI)V");

    int frame_count;
    while (av_read_frame(pFormatCtx, packet) >= 0) {
        if (packet->stream_index == audio_stream_index) {
            avcodec_decode_audio4(pCodeCtx, frame, &got_frame, packet);

            if (got_frame) {
                LOGE("解码");

                swr_convert(swrContext, &out_buffer, 44100 *2 , (const uint8_t **) frame->data, frame->nb_samples);
                //缓冲区大小
                int size = av_samples_get_buffer_size(NULL, out_channel_nb, frame->nb_samples, AV_SAMPLE_FMT_S16, 1);

                jbyteArray audio_sample_array = env->NewByteArray(size);
                env->SetByteArrayRegion(audio_sample_array, 0, size, (const jbyte *) out_buffer);
                env->CallVoidMethod(jobj, audio_write, audio_sample_array, size);
                env->DeleteLocalRef(audio_sample_array);
            }
        }
    }
    av_frame_free(&frame);
    swr_free(&swrContext);
    avcodec_close(pCodeCtx);
    avformat_close_input(&pFormatCtx);

    env->ReleaseStringUTFChars(input_str, input_path);
    env->ReleaseStringUTFChars(output_str, output_path);
}

SLObjectItf engineObject;
SLEngineItf  engineItf;
SLObjectItf outputMix;
SLEnvironmentalReverbItf outputMixEnvironmentalReverb = NULL;
const SLEnvironmentalReverbSettings settings = SL_I3DL2_ENVIRONMENT_PRESET_DEFAULT;
SLObjectItf player;
SLPlayItf playItf;
//缓冲器队列接口
SLAndroidSimpleBufferQueueItf bqPlayerBufferQueue;
SLVolumeItf bg_play_volume;
size_t buffer_size;
void *buffer;

// 当喇叭播放完声音时回调此方法
void bg_player_callback(SLAndroidSimpleBufferQueueItf bq, void *context)
{
    buffer_size = 0;
    //assert(NULL == context);
    getPCM(&buffer, &buffer_size);
    // for streaming playback, replace this test by logic to find and fill the next buffer
    if (NULL != buffer && 0 != buffer_size) {
        SLresult result;
        // enqueue another buffer
        result = (*bqPlayerBufferQueue)->Enqueue(bqPlayerBufferQueue, buffer,
                                                 buffer_size);
        // the most likely other result is SL_RESULT_BUFFER_INSUFFICIENT,
        // which for this code example would indicate a programming error
        assert(SL_RESULT_SUCCESS == result);
        LOGE("louis  bqPlayerCallback :%d", result);
    }

}


JNIEXPORT void JNICALL startMusic1(JNIEnv *env, jobject jobj) {
    SLresult sLresult;
    //初始化一个引擎
    slCreateEngine(&engineObject, 0, NULL, 0, NULL, NULL);
    (*engineObject)->Realize(engineObject, SL_BOOLEAN_FALSE);

    //获取到引擎接口
    (*engineObject)->GetInterface(engineObject, SL_IID_ENGINE, &engineItf);
    LOGE("引擎地址%p ", engineItf);

    //创建混音器
    (*engineItf)->CreateOutputMix(engineItf, &outputMix, 0, 0, 0);
    //实现混音器
    (*outputMix)->Realize(outputMix, SL_BOOLEAN_FALSE);
    //实现混音器接口,设置环境混响
    sLresult = (*outputMix)->GetInterface(outputMix, SL_IID_ENVIRONMENTALREVERB, &outputMixEnvironmentalReverb);
    if (SL_RESULT_SUCCESS == sLresult) {
        (*outputMixEnvironmentalReverb)->SetEnvironmentalReverbProperties(outputMixEnvironmentalReverb, &settings);
    }
    int rate;
    int channels;
    createFFmpeg(&rate, &channels);
    LOGE("b比特率%d ， channels %d", rate, channels);
    //配置信息设置
    SLDataLocator_AndroidBufferQueue android_queue = {SL_DATALOCATOR_ANDROIDSIMPLEBUFFERQUEUE, 2};

    SLDataFormat_PCM pcm = {SL_DATAFORMAT_PCM, 2, SL_SAMPLINGRATE_44_1, SL_PCMSAMPLEFORMAT_FIXED_16, SL_PCMSAMPLEFORMAT_FIXED_16, SL_SPEAKER_FRONT_LEFT | SL_SPEAKER_FRONT_RIGHT, SL_BYTEORDER_LITTLEENDIAN};

    //新建一个数据源 将上述配置信息放到这个数据源中
    SLDataSource slDataSource = {&android_queue, &pcm};
    //设置混音器
    SLDataLocator_OutputMix slDataLocator_outputMix = {SL_DATALOCATOR_OUTPUTMIX, outputMix};

    SLDataSink audioSink = {&slDataLocator_outputMix, NULL};

    SLInterfaceID ids[3] = {SL_IID_BUFFERQUEUE, SL_IID_EFFECTSEND, SL_IID_VOLUME};
    SLboolean req[3] = {SL_BOOLEAN_TRUE, SL_BOOLEAN_TRUE, SL_BOOLEAN_TRUE};


    //将混音器关联起来 创建播放器
    (*engineItf)->CreateAudioPlayer(engineItf, &player, &slDataSource, &audioSink, 3, ids, req);

    (*player)->Realize(player, SL_BOOLEAN_FALSE);
    //创建一个播放器接口
    (*player)->GetInterface(player, SL_IID_PLAY, &playItf);

    //注册回调缓冲区，获取缓存队列接口
    (*player)->GetInterface(player, SL_IID_BUFFERQUEUE, &bqPlayerBufferQueue);
    LOGE("获取缓冲区数据");

    //缓冲接口回调
    (*bqPlayerBufferQueue)->RegisterCallback(bqPlayerBufferQueue, bg_player_callback, NULL);
    //获取音量接口
    (*player)->GetInterface(player, SL_IID_VOLUME, &bg_play_volume);
    //获取播放状态接口
    (*playItf)->SetPlayState(playItf, SL_PLAYSTATE_PLAYING);
    bg_player_callback(bqPlayerBufferQueue, NULL);
}


// shut down the native audio system
void shutdown()
{
    // destroy buffer queue audio player object, and invalidate all associated interfaces
    if (player != NULL) {
        (*player)->Destroy(player);
        player = NULL;
        playItf = NULL;
        bqPlayerBufferQueue = NULL;
        bg_play_volume = NULL;
    }

    // destroy output mix object, and invalidate all associated interfaces
    if (outputMix != NULL) {
        (*outputMix)->Destroy(outputMix);
        outputMix = NULL;
        outputMixEnvironmentalReverb = NULL;
    }
    // destroy engine object, and invalidate all associated interfaces
    if (engineObject != NULL) {
        (*engineObject)->Destroy(engineObject);
        engineObject = NULL;
        engineItf = NULL;
    }
    // 释放FFmpeg解码器相关资源
    realase();
}
/////////////////////////////////////////////////////////////////音视频同步代码//////////////////////////////////////////////////
const char *path;
//缓冲区
int isPlay = 0;
ANativeWindow* window = 0;

FFmpegVedio *video;
FFmpegAudio *audio;

void *process(void* args) {
    LOGE("开启解码线程");
    //1.注册组件
    av_register_all();
    avformat_network_init();
    //封装格式上下文
    AVFormatContext *pFormatCtx = avformat_alloc_context();

    //2.打开输入视频文件
    if(avformat_open_input(&pFormatCtx,path,NULL,NULL) != 0){
        LOGE("%s","打开输入视频文件失败");
    }
    //3.获取视频信息
    if(avformat_find_stream_info(pFormatCtx,NULL) < 0){
        LOGE("%s","获取视频信息失败");
    }

    //视频解码，需要找到视频对应的AVStream所在pFormatCtx->streams的索引位置
    int i = 0;
    for(; i < pFormatCtx->nb_streams;i++){
        //4.获取视频解码器
        AVCodecContext *pCodeCtx = pFormatCtx->streams[i]->codec;
        AVCodec *pCodec = avcodec_find_decoder(pCodeCtx->codec_id);

        AVCodecContext *codec = avcodec_alloc_context3(pCodec);
        avcodec_copy_context(codec, pCodeCtx);
        if(avcodec_open2(codec,pCodec,NULL) < 0){
            LOGE("%s","解码器无法打开");
            continue;
        }
        //根据类型判断，是否是视频流
        if(pFormatCtx->streams[i]->codec->codec_type == AVMEDIA_TYPE_VIDEO){
            /*找到视频流*/
            video->setAvCodecContext(codec);
            video->index = i;
            video->time_base = pFormatCtx->streams[i]->time_base;
            if (window)
                ANativeWindow_setBuffersGeometry(window, video->codec->width,
                                                 video->codec->height,
                                                 WINDOW_FORMAT_RGBA_8888);

        } else if (pFormatCtx->streams[i]->codec->codec_type == AVMEDIA_TYPE_AUDIO) {
            audio->setAvCodecContext(codec);
            audio->index = i;
            audio->time_base=pFormatCtx->streams[i]->time_base;
        }

    }

//    开启 音频 视频  播放的死循环
    video->setAudio(audio);
    video->play();
    audio->play();
    isPlay = 1;
//    解码packet

    //编码数据
    AVPacket *packet = (AVPacket *)av_malloc(sizeof(AVPacket));
//    解码完整个视频 子线程
    int ret;
    while (isPlay ) {
//        如果这个packet  流索引 等于 视频流索引 添加到视频队列
        ret = av_read_frame(pFormatCtx, packet);
        if (ret == 0) {
            if (video && video->isPlay && packet->stream_index == video->index) {
                video->put(packet);
            } else  if (audio&& audio->isPlay && packet->stream_index == audio->index) {
                audio->put(packet);
            }
            av_packet_unref(packet);
        } else if(ret == AVERROR_EOF) {
//            读完了
            //读取完毕 但是不一定播放完毕
            while (isPlay) {
                if (video->queue.empty() && audio->queue.empty()) {
                    break;
                }
//                LOGI("等待播放完成");
                av_usleep(10000);
            }
        }

    }
//    视频解码完     可能视频播放完了   也可能视频没播放完成
    isPlay = 0;
    if (video && video->isPlay) {
        video->stop();
    }
    if (audio && audio->isPlay) {
        audio->stop();
    }
    av_free_packet(packet);
    avformat_free_context(pFormatCtx);
    pthread_exit(0);
}
void call_video_play(AVFrame *frame){
    if (!window) {
        return;
    }
    ANativeWindow_Buffer window_buffer;
    if (ANativeWindow_lock(window, &window_buffer, 0)) {
        return;
    }

    LOGE("绘制 宽%d,高%d",frame->width,frame->height);
    LOGE("绘制 宽%d,高%d  行字节 %d ",window_buffer.width,window_buffer.height, frame->linesize[0]);
    uint8_t *dst = (uint8_t *) window_buffer.bits;
    int dstStride = window_buffer.stride * 4;
    uint8_t *src = frame->data[0];
    int srcStride = frame->linesize[0];
    for (int i = 0; i < window_buffer.height; ++i) {
        memcpy(dst + i * dstStride, src + i * srcStride, srcStride);
    }
    ANativeWindow_unlockAndPost(window);
}
pthread_t  p_tid;
JNIEXPORT void JNICALL play_sync_net(JNIEnv *env, jobject instance, jstring path_) {
    path = env->GetStringUTFChars(path_, 0);
    video = new FFmpegVedio;
    audio = new FFmpegAudio;
    video->setPlayCall(call_video_play);
    pthread_create(&p_tid, NULL, process, NULL);
    env->ReleaseStringUTFChars(path_, path);
}


JNIEXPORT void JNICALL display_sync_net(JNIEnv *env, jobject instance, jobject surface) {
    if (window) {
        ANativeWindow_release(window);
        window = 0;
    }
    window = ANativeWindow_fromSurface(env, surface);
    if (video && video->codec) {
        ANativeWindow_setBuffersGeometry(window, video->codec->width, video->codec->height,
                                         WINDOW_FORMAT_RGBA_8888);
    }
}


JNIEXPORT void JNICALL release_sync_net(JNIEnv *env, jobject instance) {
    if (isPlay) {
        isPlay = 0;
        pthread_join(p_tid, 0);
    }
    if (video) {
        if (video->isPlay) {
            video->stop();
        }
        delete (video);
        video = 0;
    }
    if (audio) {
        if (audio->isPlay) {
            audio->stop();
        }
        delete (audio);
        audio = 0;

    }
}



/////////////////////////////////////////////////////////////////音视频同步代码//////////////////////////////////////////////////



static const JNINativeMethod gMethods[] = {
        {
                "render","(Ljava/lang/String;Landroid/view/Surface;)V",(void*)startPlaying
        },
        {
                "sound","(Ljava/lang/String;Ljava/lang/String;)V",(void*)startMusic
        },
        {
                "sound1","()V",(void*)startMusic1
        },
        {
                "stop","()V",(void*)shutdown
        },
        {
                "playSyncNet","(Ljava/lang/String;)V",(void*)play_sync_net
        },
        {
                "dispalySync","(Landroid/view/Surface;)V",(void*)display_sync_net
        },
        {
                "releaseSync","()V",(void*)release_sync_net
        }

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

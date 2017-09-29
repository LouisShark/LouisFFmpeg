//
// Created by user on 2017/9/28.
//

#include "FFmpegVideo.h"

FFmpegVedio::FFmpegVedio() {
    pthread_mutex_init(&mutex, NULL);
    pthread_cond_init(&cond, NULL);
}

FFmpegVedio::~FFmpegVedio() {

}

void* play_video(void* args) {
    LOGE("开始播放视频线程");
    FFmpegVedio *video = (FFmpegVedio *) args;
    AVFrame *rgb_frame = av_frame_alloc();
    //只有指定了AVFrame的像素格式丶画面大小才能真正分配内存
    //缓冲区分配内存
    uint8_t * out_buffer = (uint8_t *) av_malloc(avpicture_get_size(AV_PIX_FMT_RGBA, video->codec->width, video->codec->height));
    LOGE("width %d, height %d", video->codec->width, video->codec->height);
    //设置缓冲区丶像素格式
    int re = avpicture_fill((AVPicture *) rgb_frame, out_buffer, AV_PIX_FMT_RGBA, video->codec->width, video->codec->height);
    int frame_count = 0;
    SwsContext *swsContext = sws_getContext(video->codec->width, video->codec->height, video->codec->pix_fmt, video->codec->width, video->codec->height, AV_PIX_FMT_RGBA, SWS_BILINEAR, NULL, NULL, NULL);
    AVPacket * packet = (AVPacket *) av_malloc(sizeof(AVPacket));
    AVFrame * avFrame = av_frame_alloc();
    int got_frame;
    int length;
    while (video->isPlay) {
        //解码一帧视频数据
        LOGE("视频解码一帧数据");
        video->deQueue(packet);
        LOGE("视频队列的长度%d", video->queue.size());
        length = avcodec_decode_video2(video->codec, avFrame, &got_frame, packet);
        if (got_frame) {
            LOGE("视频解码%d帧", frame_count++);
            //转为指定的rgba编码
            sws_scale(swsContext, (const uint8_t *const *) avFrame->data, avFrame->linesize, 0, avFrame->height, rgb_frame->data, rgb_frame->linesize);
            sleep(1);
        }
        av_packet_unref(packet);
        av_frame_unref(avFrame);
    }



}


int FFmpegVedio::enQueue(AVPacket *packet) {
    AVPacket *avPacket = (AVPacket *) av_malloc(sizeof(AVPacket));
    if (av_packet_ref(avPacket, packet)) {
        return 0;
    }
    LOGE("压入一帧视频数据");
    pthread_mutex_lock(&mutex);
    queue.push(avPacket);
    //通知消费者 消费帧 不在阻塞
    pthread_cond_signal(&cond);
    pthread_mutex_unlock(&mutex);
    return 1;
}

int FFmpegVedio::deQueue(AVPacket *avPacket) {
    int ret;
    pthread_mutex_lock(&mutex);
    while (isPlay) {
        LOGE("视频取一帧");
        if (!queue.empty()) {
            if (av_packet_ref(avPacket, queue.front()) < 0) {
                ret = false;
                break;
            }
            AVPacket *apkt = queue.front();
            queue.pop();

            av_free(apkt);
            ret = 1;
            break;
        } else{
            pthread_cond_wait(&cond, &mutex);
        }
    }
    pthread_mutex_unlock(&mutex);
    return 0;
}

void FFmpegVedio::play(FFmpegAudio *audio) {
    isPlay = 1;
    pthread_create(&p_id, 0, play_video, this);
}

void FFmpegVedio::setAvCodecContext(AVCodecContext *codec) {
    this->codec = codec;
}

void FFmpegVedio::setTimeBase(AVRational time_base) {

}

void FFmpegVedio::stop() {

}

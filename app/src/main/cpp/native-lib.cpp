#include <jni.h>
#include <string>
#include "FFmpegMusic.h"
#include "FFmpegVideo.h"
#include <android/native_window_jni.h>


extern "C" {
//编码
#include "libavcodec/avcodec.h"
#include "ffplay/ffplay2.h"
//封装格式处理
#include "libavformat/avformat.h"
#include "libswresample/swresample.h"
//像素处理
#include "libswscale/swscale.h"

#include <unistd.h>
#include "Log.h"
}
const char *inputPath;
int64_t *totalTime;
FFmpegVideo *ffmpegVideo;
FFmpegMusic *ffmpegMusic;
pthread_t p_tid;
int isPlay;
ANativeWindow *window = 0;
int64_t duration;
AVFormatContext *pFormatCtx;
AVPacket *packet;
int step = 0;
jboolean isSeek = false;





AVPacket avPacket;
int size;
AVFrame *avFrame;
AVStream *video_st;
AVCodecContext *avCodecContext;
int fameCount = 0;
AVFormatContext *ofmt_ctx;
int64_t start_time;

#define FPS 10


void call_video_play(AVFrame *frame) {
    if (!window) {
        return;
    }
    ANativeWindow_Buffer window_buffer;
    if (ANativeWindow_lock(window, &window_buffer, 0)) {
        return;
    }

//    LOGE("绘制 宽%d,高%d", frame->width, frame->height);
//    LOGE("绘制 宽%d,高%d  行字节 %d ", window_buffer.width, window_buffer.height, frame->linesize[0]);
    uint8_t *dst = (uint8_t *) window_buffer.bits;
    int dstStride = window_buffer.stride * 4;
    uint8_t *src = frame->data[0];
    int srcStride = frame->linesize[0];
    for (int i = 0; i < window_buffer.height; ++i) {
        memcpy(dst + i * dstStride, src + i * srcStride, srcStride);
    }

    ANativeWindow_unlockAndPost(window);
}

void initFFmpeg() {
    LOGE("开启解码线程")
    //1.注册组件
    avformat_network_init();
    //封装格式上下文
    pFormatCtx = avformat_alloc_context();

    //2.打开输入视频文件
    if (avformat_open_input(&pFormatCtx, inputPath, NULL, NULL) != 0) {
        LOGE("%s", "打开输入视频文件失败");
    }
    //3.获取视频信息
    if (avformat_find_stream_info(pFormatCtx, NULL) < 0) {
        LOGE("%s", "获取视频信息失败");
    }

    //得到播放总时间
    if (pFormatCtx->duration != AV_NOPTS_VALUE) {
        duration = pFormatCtx->duration;//微秒
    }
}

void initPlayer() {
    ffmpegVideo = new FFmpegVideo;
    ffmpegMusic = new FFmpegMusic;
    ffmpegVideo->setPlayCall(call_video_play);


    AVCodec* avCodec = avcodec_find_encoder(AV_CODEC_ID_H264);
    AVCodecContext *avCodecContext =avcodec_alloc_context3(avCodec);

    int ret;

    if ((ret = avcodec_open2(avCodecContext, avCodec, NULL)) < 0) {
        LOGE("dsfdsfsfsf   %d", ret);
    }


    //找到视频流和音频流
    for (int i = 0; i < pFormatCtx->nb_streams; ++i) {
        //获取解码器
        AVCodecContext *avCodecContext = pFormatCtx->streams[i]->codec;
        AVCodec *avCodec = avcodec_find_decoder(avCodecContext->codec_id);

        //copy一个解码器，
        AVCodecContext *codecContext = avcodec_alloc_context3(avCodec);
        avcodec_copy_context(codecContext, avCodecContext);
        if (avcodec_open2(codecContext, avCodec, NULL) < 0) {
            LOGE("打开失败")
            continue;
        }
        //如果是视频流
        if (pFormatCtx->streams[i]->codec->codec_type == AVMEDIA_TYPE_VIDEO) {
            ffmpegVideo->index = i;
            ffmpegVideo->setAvCodecContext(codecContext);
            ffmpegVideo->time_base = pFormatCtx->streams[i]->time_base;
            if (window) {
                ANativeWindow_setBuffersGeometry(window, ffmpegVideo->codec->width,
                                                 ffmpegVideo->codec->height,
                                                 WINDOW_FORMAT_RGBA_8888);
            }
        }//如果是音频流
        else if (pFormatCtx->streams[i]->codec->codec_type == AVMEDIA_TYPE_AUDIO) {
            ffmpegMusic->index = i;
            ffmpegMusic->setAvCodecContext(codecContext);
            ffmpegMusic->time_base = pFormatCtx->streams[i]->time_base;
        }
    }
}

/*void swap(int *a,int *b)
{
    int t=*a;*a=*b;*b=t;
}*/

void seekTo(int mesc) {
    if (mesc <= 0) {
        mesc = 0;
    }
    //清空vector
    ffmpegMusic->queue.clear();
    ffmpegVideo->queue.clear();
    //跳帧
    /* if (av_seek_frame(pFormatCtx, -1,  mesc * AV_TIME_BASE, AVSEEK_FLAG_BACKWARD) < 0) {
         LOGE("failed")
     } else {
         LOGE("success")
     }*/

    av_seek_frame(pFormatCtx, ffmpegVideo->index, (int64_t) (mesc / av_q2d(ffmpegVideo->time_base)),
                  AVSEEK_FLAG_BACKWARD);
    av_seek_frame(pFormatCtx, ffmpegMusic->index, (int64_t) (mesc / av_q2d(ffmpegMusic->time_base)),
                  AVSEEK_FLAG_BACKWARD);

}

void *write_packet_to_queue(void *args) {
    packet = (AVPacket *) av_mallocz(sizeof(AVPacket));
    int ret;
    while (isPlay) {
        ret = av_read_frame(pFormatCtx, packet);
        if (ret < 0) {
            ffmpegVideo->isReadResourceDone = true;
            ffmpegMusic->isReadResourceDone = true;
            break;
        }

        if (ret == 0) {
            if (ffmpegVideo && ffmpegVideo->isPlay && packet->stream_index == ffmpegVideo->index) {
                ffmpegVideo->isReadResourceDone = false;
                ffmpegVideo->put(packet);
            } else if (ffmpegMusic && ffmpegMusic->isPlay &&
                       packet->stream_index == ffmpegMusic->index) {
                ffmpegMusic->isReadResourceDone = false;
                ffmpegMusic->put(packet);
            }

            av_packet_unref(packet);
        }
    }

    LOGE("加载数据完毕");
    pthread_exit(0);
}

void startPlay() {

    //开启播放
    ffmpegVideo->setFFmepegMusic(ffmpegMusic);
    pthread_t musicID = ffmpegMusic->play();
    pthread_t videoID = ffmpegVideo->play();
    isPlay = 1;

    pthread_create(&p_tid, NULL, write_packet_to_queue, NULL);

    pthread_join(p_tid, NULL);
    pthread_join(musicID, NULL);
    pthread_join(videoID, NULL);

    LOGE("释放内存");

    delete ffmpegMusic;
    delete ffmpegVideo;
}

extern "C"
JNIEXPORT void JNICALL
Java_com_test_ffmpegvideoplay_Play_play(JNIEnv *env, jobject instance, jstring inputPath_) {
    inputPath = env->GetStringUTFChars(inputPath_, 0);

    initFFmpeg();
    initPlayer();
    startPlay();

    LOGE("打开文件 %s", inputPath);
//    startVideo(inputPath);
    LOGE("打开文件2222222 %s", inputPath);



    env->ReleaseStringUTFChars(inputPath_, inputPath);
}
extern "C"
JNIEXPORT void JNICALL
Java_com_test_ffmpegvideoplay_Play_display(JNIEnv *env, jobject instance, jobject surface) {
    //得到界面
    if (window) {
        ANativeWindow_release(window);
        window = 0;
    }
    window = ANativeWindow_fromSurface(env, surface);
    if (ffmpegVideo && ffmpegVideo->codec) {
        ANativeWindow_setBuffersGeometry(window, ffmpegVideo->codec->width,
                                         ffmpegVideo->codec->height,
                                         WINDOW_FORMAT_RGBA_8888);
    }
}
extern "C"
JNIEXPORT void JNICALL
Java_com_test_ffmpegvideoplay_Play_release(JNIEnv *env, jobject instance) {
    //释放资源
    if (isPlay) {
        isPlay = 0;
        pthread_join(p_tid, 0);
    }
    if (ffmpegVideo) {
        if (ffmpegVideo->isPlay) {
            ffmpegVideo->stop();
        }
        delete (ffmpegVideo);
        ffmpegVideo = 0;
    }
    if (ffmpegMusic) {
        if (ffmpegMusic->isPlay) {
            ffmpegMusic->stop();
        }
        delete (ffmpegMusic);
        ffmpegMusic = 0;
    }
}extern "C"
JNIEXPORT void JNICALL
Java_com_test_ffmpegvideoplay_Play_stop(JNIEnv *env, jobject instance) {
    //点击暂停按钮
    ffmpegMusic->pause();
    ffmpegVideo->pause();

}
extern "C"
JNIEXPORT jint JNICALL
Java_com_test_ffmpegvideoplay_Play_getTotalTime(JNIEnv *env, jobject instance) {

//获取视频总时间
    return (jint) duration;
}
extern "C"
JNIEXPORT double JNICALL
Java_com_test_ffmpegvideoplay_Play_getCurrentPosition(JNIEnv *env, jobject instance) {
    //获取音频播放时间
    return ffmpegMusic->clock;
}
extern "C"
JNIEXPORT void JNICALL
Java_com_test_ffmpegvideoplay_Play_seekTo(JNIEnv *env, jobject instance, jint msec) {
    seekTo(msec / 1000);
}
extern "C"
JNIEXPORT void JNICALL
Java_com_test_ffmpegvideoplay_Play_stepBack(JNIEnv *env, jobject instance) {

}
extern "C"
JNIEXPORT void JNICALL
Java_com_test_ffmpegvideoplay_Play_stepUp(JNIEnv *env, jobject instance) {
    //点击快进按钮
    seekTo(5);

}



static int push(uint8_t *bytes) {
    start_time = av_gettime();

    int got_picture = 0;
    static int i = 0;

    int j = 0;

    avFrame = av_frame_alloc();
    int picture_size = av_image_get_buffer_size(avCodecContext->pix_fmt, avCodecContext->width,
                                                avCodecContext->height, 1);
    uint8_t buffers[picture_size];

    av_image_fill_arrays(avFrame->data, avFrame->linesize, buffers, avCodecContext->pix_fmt,
                         avCodecContext->width, avCodecContext->height, 1);

    av_new_packet(&avPacket, picture_size);
    size = avCodecContext->width * avCodecContext->height;

    //安卓摄像头数据为NV21格式，此处将其转换为YUV420P格式
    memcpy(avFrame->data[0], bytes, size); //Y
    for (j = 0; j < size / 4; j++) {
        *(avFrame->data[2] + j) = *(bytes + size + j * 2); // V
        *(avFrame->data[1] + j) = *(bytes + size + j * 2 + 1); //U
    }

    int ret = avcodec_encode_video2(avCodecContext, &avPacket, avFrame, &got_picture);
    LOGE("avcodec_encode_video2 spend time %ld", (int) ((av_gettime() - start_time) / 1000));
    if (ret < 0) {
        LOGE("Fail to avcodec_encode ! code:%d", ret);
        return -1;
    }
    if (got_picture == 1) {

        avPacket.pts = i++ * (video_st->time_base.den) / ((video_st->time_base.num) * FPS);
        LOGE("Succeed to encode frame: %5d\tsize:%5d\n", fameCount, avPacket.size);
        avPacket.stream_index = video_st->index;
        avPacket.dts = avPacket.pts;
        avPacket.duration = 1;
        int64_t pts_time = AV_TIME_BASE * av_q2d(video_st->time_base);

//        int64_t now_time = av_gettime() - start_time;
//        if (pts_time > now_time) {
//            LOGE("等待");
//            av_usleep(pts_time - now_time);
//        }

        av_write_frame(ofmt_ctx, &avPacket);
        LOGE("av_write_frame spend time %ld", (int) (av_gettime() - start_time) / 1000);
        av_free_packet(&avPacket);
        fameCount++;
    } else {
        LOGE("唉~");
    }
    av_frame_free(&avFrame);
}








extern "C"
JNIEXPORT void JNICALL
Java_com_test_ffmpegvideoplay_Play_convertImage(JNIEnv *env, jobject thiz, jbyteArray data) {
    // TODO: implement convertImage()
    jsize   in_size    = env->GetArrayLength(data);

    jbyte* bBuffer = env->GetByteArrayElements(data, 0);

    uint8_t* buf=(uint8_t*)bBuffer;

//    push(buf);


//    LOGE("预览的数据   %d", in_size);

}



static int init(const char *destUrl, int w, int h) {
    av_register_all();
    AVOutputFormat *fmt;
    int ret;

    ofmt_ctx = avformat_alloc_context();

//    avformat_alloc_output_context2(&ofmt_ctx, NULL, NULL, destUrl);


    fmt = av_guess_format(NULL, destUrl, NULL);


    ofmt_ctx->oformat = fmt;

    if ((ret = avio_open(&ofmt_ctx->pb, destUrl, AVIO_FLAG_READ_WRITE)) < 0) {
        LOGE("avio_open error");
        return -1;
    }



    video_st = avformat_new_stream(ofmt_ctx, NULL);
    if (video_st == NULL) {
        ret = -1;
        return -1;
    }

    AVCodec *avCodec;
    avCodec = avcodec_find_encoder(fmt->video_codec);
    if (NULL == avCodec) {
        LOGE("寻找编码器失败..");
        return -1;
    }

    avCodecContext =avcodec_alloc_context3(avCodec);

//    avCodecContext->codec_id = fmt->video_codec;
    avCodecContext->codec_id = AV_CODEC_ID_H264;
    avCodecContext->codec_type = AVMEDIA_TYPE_VIDEO;
    avCodecContext->pix_fmt = AV_PIX_FMT_YUV420P;
    avCodecContext->width = w;
    avCodecContext->height = h;
    // 目标的码率，即采样码率；显然，采码率越大，视频大小越大
    avCodecContext->bit_rate = 400000; //400,000
    //每250帧插入一个I帧，I帧越少，视频越小
    avCodecContext->gop_size = 250;
    // 帧率的基本单位用分数表示
    avCodecContext->time_base.num = 1;
    avCodecContext->time_base.den = FPS;

    // 最大和最小量化系数
    avCodecContext->qmin = 10;
    avCodecContext->qmax = 51;

    avCodecContext->max_b_frames = 3;

    // Set Option
    AVDictionary *param = 0;

    //H.264
    if (avCodecContext->codec_id == AV_CODEC_ID_H264) {
        av_dict_set(&param, "preset", "slow", 0);
        av_dict_set(&param, "tune", "zerolatency", 0);
    }
    //H.265
    if (avCodecContext->codec_id == AV_CODEC_ID_H265) {
        av_dict_set(&param, "preset", "ultrafast", 0);
        av_dict_set(&param, "tune", "zero-latency", 0);
    }

    AVDictionaryEntry *entry = av_dict_get(param, "preset", NULL, 0);


    LOGE("成功00    %s", entry->value);

    if ((ret = avcodec_open2(avCodecContext, avCodec, &param)) < 0) {
        char buf[1024];
        av_strerror(ret, buf, 1024);

        LOGE("avcodec_open2 fail    %d  %s!", ret, buf);
        return -1;
    }


    av_dict_get(param, "preset", NULL, 0);

    LOGE("成功");

    // Write File Header
    avformat_write_header(ofmt_ctx, NULL);

    return ret;
}





extern "C"
JNIEXPORT void JNICALL
Java_com_test_ffmpegvideoplay_Play_initConvertImage(JNIEnv *env, jobject thiz) {
    // TODO: implement initConvertImage()

    init("/sdcard/Download/wu/testDI.h264", 400, 400);

}

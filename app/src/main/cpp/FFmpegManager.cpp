//
// Created by Di on 2019-06-20.
//


#include <jni.h>
#include <android/log.h>
#include <android/native_window.h>
#include <android/native_window_jni.h>
#include <string>
#include <unistd.h>

#define LOG_TAG "FFNative"
#define ALOGV(...) ((void)__android_log_print(ANDROID_LOG_VERBOSE, LOG_TAG, __VA_ARGS__))
#define ALOGD(...) ((void)__android_log_print(ANDROID_LOG_DEBUG, LOG_TAG, __VA_ARGS__))
#define ALOGI(...) ((void)__android_log_print(ANDROID_LOG_INFO, LOG_TAG, __VA_ARGS__))
#define ALOGW(...) ((void)__android_log_print(ANDROID_LOG_WARN, LOG_TAG, __VA_ARGS__))
#define ALOGE(...) ((void)__android_log_print(ANDROID_LOG_ERROR, LOG_TAG, __VA_ARGS__))


extern "C" {

    #include <libavcodec/avcodec.h>
    #include <libavformat/avformat.h>
    #include <libavfilter/avfilter.h>
    #include <libswscale/swscale.h>
    #include "libswresample/swresample.h"
    #include "libavutil/opt.h"
    #include <libavutil/imgutils.h>

    #include "VideoStatus.h"
    #include "FFmpegManager.h"

    SwrContext *swrAudio(AVPacket *packet, AVCodecContext *codecContext);
    void intAndroidAudioMethodID(JNIEnv *env, jclass jclass1, VideoStatus *status);
    void playAudio(JNIEnv *env, VideoStatus *status);
    void playVideo(JNIEnv *env, VideoStatus *status, ANativeWindow *nativeWindow, ANativeWindow_Buffer windowBuffer);

    void init(JNIEnv *env, jclass jclass1, VideoStatus *status, const char *path, jobject surface) {
        AVFormatContext *formatContext = avformat_alloc_context();

        status->pFormatCtx = formatContext;

        //打开文件
        if (avformat_open_input(&formatContext, path, NULL, NULL) != 0) {
            ALOGE("Cannot open video file: %s\n", path);
            return;
        }

        // 获取文件流信息
        if (avformat_find_stream_info(formatContext, NULL) < 0) {
            ALOGE("Cannot find stream information.");
            return;
        }

        // 获取文件流的标识
        int video_stream_index = -1;
        int audio_stream_index = -1;
        for (int i = 0; i < formatContext->nb_streams; i++) {
            if (formatContext->streams[i]->codecpar->codec_type == AVMEDIA_TYPE_VIDEO) {
                video_stream_index = i;
            }

            if (formatContext->streams[i]->codecpar->codec_type == AVMEDIA_TYPE_AUDIO) {
                audio_stream_index = i;
            }
        }

        if (video_stream_index == -1) {
            ALOGE("No video stream found.");
            return;
        }

        if (audio_stream_index == -1) {
            ALOGE("No audio stream found.");
            return;
        }

        // TODO 参数错误 获取视频的codec参数信息
        AVCodecParameters *codecParameters = formatContext->streams[audio_stream_index]->codecpar;
        AVCodecParameters *codecVideoParameters = formatContext->streams[video_stream_index]->codecpar;

        //获取音频解码器
        AVCodec *codec = avcodec_find_decoder(codecParameters->codec_id);
        if (codec == NULL) {
            ALOGE("Codec not found.");
            return;
        }

        //获取视频解码器
        AVCodec *videoCodec = avcodec_find_decoder(codecVideoParameters->codec_id);
        if (videoCodec == NULL) {
            ALOGE("Codec not found.");
            return;
        }

        AVCodecContext *codecContext = avcodec_alloc_context3(codec);
        AVCodecContext *codecVideoContext = avcodec_alloc_context3(videoCodec);

        if (codecContext == NULL) {
            ALOGE("CodecContext not found.");
            return;
        }

        // 根据codecParameters填充codecContext
        if (avcodec_parameters_to_context(codecContext, codecParameters) < 0) {
            ALOGD("Fill CodecContext failed.");
            return;
        }

        if (avcodec_parameters_to_context(codecVideoContext, codecVideoParameters) < 0) {
            ALOGD("Fill CodecContext failed.");
            return;
        }

        // Initialize the AVCodecContext to use the given AVCodec
        if (avcodec_open2(codecContext, codec, NULL)) {
            ALOGE("Init CodecContext failed.");
            return;
        }

        // Initialize the AVCodecContext to use the given AVCodec
        if (avcodec_open2(codecVideoContext, videoCodec, NULL)) {
            ALOGE("Init CodecContext failed.");
            return;
        }

        AVPacket *packet = (AVPacket *) av_malloc(sizeof(AVPacket));

        status->audio_swr_ctx = swrAudio(packet, codecContext);

        AVFrame *frame = av_frame_alloc();
        status->audio_frame = frame;
        status->audio_ctx = codecContext;
        status->audio_pkt = packet;

        status->video_pkt = packet;
        status->video_ctx = codecVideoContext;

        intAndroidAudioMethodID(env, jclass1, status);


        ANativeWindow *nativeWindow = ANativeWindow_fromSurface(env, surface);
        ANativeWindow_Buffer windowBuffer;

        // get video width , height
        ALOGI("get video width , height");
        int videoWidth = codecVideoContext->width;
        int videoHeight = codecVideoContext->height;
        ALOGI("VideoSize: [%d,%d]", videoWidth, videoHeight);

        // 设置native window的buffer大小,可自动拉伸
        ALOGI("set native window");
        ANativeWindow_setBuffersGeometry(nativeWindow, videoWidth, videoHeight,
                                         WINDOW_FORMAT_RGBA_8888);


        AVPixelFormat dstFormat = AV_PIX_FMT_RGBA;
        AVFrame *VideoFrame = av_frame_alloc();
        AVFrame *renderVideoFrame = av_frame_alloc();
        status->render_video_frame = renderVideoFrame;
        status->video_frame = VideoFrame;
        // Determine required buffer size and allocate buffer
        ALOGI("Determine required buffer size and allocate buffer");
        int size = av_image_get_buffer_size(dstFormat, codecVideoContext->width, codecVideoContext->height, 1);
        uint8_t *buffer = (uint8_t *) av_malloc(size * sizeof(uint8_t));
        av_image_fill_arrays(renderVideoFrame->data, renderVideoFrame->linesize, buffer, dstFormat,
                             codecVideoContext->width, codecVideoContext->height, 1);

        // init SwsContext
        ALOGI("init SwsContext");
        struct SwsContext *swsContext = sws_getContext(codecVideoContext->width,
                                                       codecVideoContext->height,
                                                       codecVideoContext->pix_fmt,
                                                       1000,
                                                       500,
                                                       dstFormat,
                                                       SWS_BILINEAR,
                                                       NULL,
                                                       NULL,
                                                       NULL);
        if (swsContext == NULL) {
            ALOGE("Init SwsContext failed.");
            return;
        }

        status->video_sws_ctx = swsContext;

        while (av_read_frame(formatContext, packet) >= 0) {
            if (packet->stream_index == audio_stream_index) {
                playAudio(env, status);
            }

            if (packet->stream_index == video_stream_index) {
                playVideo(env, status, nativeWindow, windowBuffer);
            }

            av_packet_unref(packet);
        }

//        swr_free(&status->audio_swr_ctx);
//        avcodec_close(status->audio_ctx);
//        avformat_close_input(&status->pFormatCtx);
//        ANativeWindow_release(nativeWindow);
//        av_frame_free(&frame);
//        av_frame_free(&renderFrame);
//        av_packet_free(&packet);
//        avcodec_close(status->video_ctx);
//        avcodec_free_context(&codecContext);
//        avformat_free_context(formatContext);
    }

    void playVideo(JNIEnv *env, VideoStatus *status, ANativeWindow *nativeWindow, ANativeWindow_Buffer windowBuffer) {
        AVCodecContext* codecContext = status->video_ctx;
        AVPacket* packet = status->video_pkt;
        AVFrame* renderFrame = status->render_video_frame;
        AVFrame* frame = status->video_frame;

        int sendPacketState = avcodec_send_packet(codecContext, packet);
        if (sendPacketState == 0) {
            int receiveFrameState = avcodec_receive_frame(codecContext, frame);
            if (receiveFrameState == 0) {
                ANativeWindow_lock(nativeWindow, &windowBuffer, NULL);

                // 格式转换
                sws_scale(status->video_sws_ctx, (uint8_t const *const *) frame->data,
                          frame->linesize, 0, 500,
                          renderFrame->data, renderFrame->linesize);

                // 获取stride
                uint8_t *dst = (uint8_t *) windowBuffer.bits;
                uint8_t *src = (renderFrame->data[0]);
                int dstStride = windowBuffer.stride * 4;
                int srcStride = renderFrame->linesize[0];

                // 由于window的stride和帧的stride不同,因此需要逐行复制
                for (int i = 0; i < status->video_ctx->height; i++) {
                    memcpy(dst + i * dstStride, src + i * srcStride, srcStride);
                }

                ANativeWindow_unlockAndPost(nativeWindow);
            }
        }
    }

    void intAndroidAudioMethodID(JNIEnv *env, jclass jclass1, VideoStatus *status) {
        jobject instance = env->AllocObject(jclass1);
        //    获取通道数  2
        int out_channer_nb = av_get_channel_layout_nb_channels(AV_CH_LAYOUT_STEREO);
        //    反射得到Class类型
        jclass david_player = env->GetObjectClass(instance);
        //    反射得到createAudio方法
        jmethodID createAudio = env->GetMethodID(david_player, "createTrack", "(II)V");
        //    反射调用createAudio
        env->CallVoidMethod(instance, createAudio, 44100, out_channer_nb);
        jmethodID audio_write = env->GetMethodID(david_player, "playTrack", "([BI)V");

        status->out_channer_nb = out_channer_nb;
        status->audio_play_instance = instance;
        status -> audio_play_method_id = audio_write;
    }

    void playAudio(JNIEnv *env, VideoStatus *status) {
        int got_frame;
        AVFrame* frame = status->audio_frame;
        uint8_t* buf = status->audio_buf;

        avcodec_decode_audio4(status->audio_ctx, frame, &got_frame, status->audio_pkt);
        if (got_frame) {
            swr_convert(status->audio_swr_ctx, &buf, 44100 * 2, (const uint8_t **) frame->data, frame->nb_samples);
            //                缓冲区的大小
            int size = av_samples_get_buffer_size(NULL, status->out_channer_nb, frame->nb_samples,
                                                  AV_SAMPLE_FMT_S16, 1);
            jbyteArray audio_sample_array = env->NewByteArray(size);
            env->SetByteArrayRegion(audio_sample_array, 0, size, (const jbyte *) buf);
            env->CallVoidMethod(status->audio_play_instance, status->audio_play_method_id, audio_sample_array, size);
            env->DeleteLocalRef(audio_sample_array);
        }
    }

    SwrContext *swrAudio(AVPacket *packet, AVCodecContext *codecContext) {
        //得到SwrContext ，进行重采样，具体参考http://blog.csdn.net/jammg/article/details/52688506
        SwrContext *swrContext = swr_alloc();
        //输出的声道布局（立体声）
        uint64_t out_ch_layout = AV_CH_LAYOUT_STEREO;
        //输出采样位数  16位
        enum AVSampleFormat out_formart = AV_SAMPLE_FMT_S16;
        //输出的采样率必须与输入相同
        int out_sample_rate = codecContext->sample_rate;

        //swr_alloc_set_opts将PCM源文件的采样格式转换为自己希望的采样格式
        swr_alloc_set_opts(swrContext, out_ch_layout, out_formart, out_sample_rate,
                           codecContext->channel_layout, codecContext->sample_fmt,
                           codecContext->sample_rate, 0,
                           NULL);

        swr_init(swrContext);

        return swrContext;
    }

}

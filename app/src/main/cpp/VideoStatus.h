//
// Created by Di on 2019-06-20.
//

#include "PacketQueue.h"
#include "VideoPicture.h"

#ifndef FFMPEGDEMO_VIDEOSTATUS_H
#define FFMPEGDEMO_VIDEOSTATUS_H

#endif //FFMPEGDEMO_VIDEOSTATUS_H


//
// Created by Di on 2019-06-20.
//

#define SDL_AUDIO_BUFFER_SIZE 1024
#define MAX_AUDIO_FRAME_SIZE 192000

#define MAX_AUDIOQ_SIZE (5 * 16 * 1024)
#define MAX_VIDEOQ_SIZE (5 * 256 * 1024)

#define AV_SYNC_THRESHOLD 0.01
#define AV_NOSYNC_THRESHOLD 10.0

#define FF_REFRESH_EVENT (SDL_USEREVENT)
#define FF_QUIT_EVENT (SDL_USEREVENT + 1)

#define VIDEO_PICTURE_QUEUE_SIZE 1



class VideoStatus {

public:
    AVFormatContext *pFormatCtx;
    int             videoStream, audioStream;

    AVStream        *audio_st;
    AVCodecContext  *audio_ctx;
    PacketQueue     audioq;
    uint8_t         audio_buf[(MAX_AUDIO_FRAME_SIZE * 3) / 2];
    unsigned int    audio_buf_size;
    unsigned int    audio_buf_index;
    AVFrame         *audio_frame;
    AVPacket        *audio_pkt;
    uint8_t         *audio_pkt_data;
    int             audio_pkt_size;
    int             audio_hw_buf_size;
    struct SwrContext *audio_swr_ctx;

    double          audio_clock;
    double          video_clock; ///<pts of last decoded frame / predicted pts of next decoded frame

    double          frame_timer;
    double          frame_last_pts;
    double          frame_last_delay;

    AVStream        *video_st;
    AVCodecContext  *video_ctx;
    PacketQueue     videoq;
    struct SwsContext *video_sws_ctx;

    VideoPicture    pictq[VIDEO_PICTURE_QUEUE_SIZE];
    int             pictq_size, pictq_rindex, pictq_windex;

    char            filename[1024];
    int             quit;


    jmethodID       audio_play_method_id;
    jobject         audio_play_instance;
    int             out_channer_nb;

};


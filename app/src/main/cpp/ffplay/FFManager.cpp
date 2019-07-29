//
// Created by Di on 2019-07-19.
//

#include "FFManager.h"


typedef struct PacketQueue {
    std::vector<AVPacket *> queue;//队列
    int nb_packets;
    int size;
    int64_t duration;
    int abort_request;
//        SDL_mutex *mutex;
//        SDL_cond *cond;
} PacketQueue;

typedef struct Frame {
    AVFrame *frame;
    AVSubtitle sub;
    int serial;
    double pts;           /* presentation timestamp for the frame */
    double duration;      /* estimated duration of the frame */
    int64_t pos;          /* byte position of the frame in the input file */
    int width;
    int height;
    int format;
    AVRational sar;
    int uploaded;
    int flip_v;
} Frame;

typedef struct FrameQueue {
    Frame queue[16];
    int rindex;
    int windex;
    int size;
    int max_size;
    int keep_last;
    int rindex_shown;
//        SDL_mutex *mutex;
//        SDL_cond *cond;
    PacketQueue *pktq;
} FrameQueue;

typedef struct Decoder {
    AVPacket pkt;
    PacketQueue *queue;
    AVCodecContext *avctx;
    int pkt_serial;
    int finished;
    int packet_pending;
//        SDL_cond *empty_queue_cond;
    int64_t start_pts;
    AVRational start_pts_tb;
    int64_t next_pts;
    AVRational next_pts_tb;

    pthread_t decoder_tid;//处理线程
//        SDL_Thread *decoder_tid;
} Decoder;

typedef struct VideoState {
    AVInputFormat *iformat;
    AVFormatContext *ic;
    FrameQueue pictq;
    Decoder viddec;
    PacketQueue videoq;
    char *filePath;
    int queue_attachments_req;
    AVStream *video_st;
    int video_stream;

} VideoState;

#define VIDEO_PICTURE_QUEUE_SIZE 3
#define SUBPICTURE_QUEUE_SIZE 16
#define SAMPLE_QUEUE_SIZE 9
#define FRAME_QUEUE_SIZE FFMAX(SAMPLE_QUEUE_SIZE, FFMAX(VIDEO_PICTURE_QUEUE_SIZE, SUBPICTURE_QUEUE_SIZE))


int frame_queue_init(FrameQueue *f, PacketQueue *pktq, int max_size, int keep_last) {
    int i;
    memset(f, 0, sizeof(FrameQueue));
    f->pktq = pktq;
    f->max_size = FFMIN(max_size, FRAME_QUEUE_SIZE);
    f->keep_last = keep_last != 0;
    for (i = 0; i < f->max_size; i++)
        if (!(f->queue[i].frame = av_frame_alloc()))
            return AVERROR(ENOMEM);
    return 0;
}

static int packet_queue_init(PacketQueue *q) {
    memset(q, 0, sizeof(PacketQueue));
    q->abort_request = 1;
    return 0;
}

static void decoder_init(Decoder *d, AVCodecContext *avctx, PacketQueue *queue) {
    memset(d, 0, sizeof(Decoder));
    d->avctx = avctx;
    d->queue = queue;
    d->start_pts = AV_NOPTS_VALUE;
    d->pkt_serial = -1;
}


int decoder_start(Decoder *d, void* (*fn)(void *), void *arg) {
    pthread_create(&d->decoder_tid, NULL, fn, arg);

    return 0;
}

int decoder_decode_frame(Decoder *d, AVFrame *frame, AVSubtitle *sub) {
//    int ret = AVERROR(EAGAIN);
//
//    for (;;) {
//        AVPacket pkt;
//
//        do {
//            switch (d->avctx->codec_type) {
//                case AVMEDIA_TYPE_VIDEO:
//                    ret = avcodec_receive_frame(d->avctx, frame);
//                    if (ret >= 0) {
//                        if (decoder_reorder_pts == -1) {
//                            frame->pts = frame->best_effort_timestamp;
//                        } else if (!decoder_reorder_pts) {
//                            frame->pts = frame->pkt_dts;
//                        }
//                    }
//                    break;
//            }
//
//            if (ret == AVERROR_EOF) {
//                d->finished = d->pkt_serial;
//                avcodec_flush_buffers(d->avctx);
//                return 0;
//            }
//            if (ret >= 0)
//                return 1;
//        } while (ret != AVERROR(EAGAIN));
//
//    }
}

static int get_video_frame(VideoState *is, AVFrame *frame)
{
    int got_picture;

    if ((got_picture = decoder_decode_frame(&is->viddec, frame, NULL)) < 0)
        return -1;

    if (got_picture) {
        double dpts = NAN;

        if (frame->pts != AV_NOPTS_VALUE)
            dpts = av_q2d(is->video_st->time_base) * frame->pts;

        frame->sample_aspect_ratio = av_guess_sample_aspect_ratio(is->ic, is->video_st, frame);

//        if (framedrop>0 || (framedrop && get_master_sync_type(is) != AV_SYNC_VIDEO_MASTER)) {
//            if (frame->pts != AV_NOPTS_VALUE) {
//                double diff = dpts - get_master_clock(is);
//                if (!isnan(diff) && fabs(diff) < AV_NOSYNC_THRESHOLD &&
//                    diff - is->frame_last_filter_delay < 0 &&
//                    is->viddec.pkt_serial == is->vidclk.serial &&
//                    is->videoq.nb_packets) {
//                    is->frame_drops_early++;
//                    av_frame_unref(frame);
//                    got_picture = 0;
//                }
//            }
//        }
    }

    return got_picture;
}

static void* video_thread(void *arg) {
    VideoState *is = static_cast<VideoState *>(arg);
    AVFrame *frame = av_frame_alloc();
    double pts;
    double duration;
    int ret;
    AVRational tb = is->video_st->time_base;
    AVRational frame_rate = av_guess_frame_rate(is->ic, is->video_st, NULL);

    for (;;) {
        ret = get_video_frame(is, frame);
        if (ret < 0)
            goto the_end;
        if (!ret)
            continue;
    }

    the_end:
    av_frame_free(&frame);
}

/* open a given stream. Return 0 if OK */
int stream_component_open(VideoState *is, int stream_index) {
    AVFormatContext *ic = is->ic;
    AVCodecContext *avctx;
    AVCodec *codec;
    const char *forced_codec_name = NULL;
    int sample_rate, nb_channels;
    int64_t channel_layout;
    int ret = 0;

    if (stream_index < 0 || stream_index >= ic->nb_streams)
        return -1;

    avctx = avcodec_alloc_context3(NULL);
    if (!avctx)
        return AVERROR(ENOMEM);

    ret = avcodec_parameters_to_context(avctx, ic->streams[stream_index]->codecpar);
    if (ret < 0)
        goto fail;
    avctx->pkt_timebase = ic->streams[stream_index]->time_base;

    codec = avcodec_find_decoder(avctx->codec_id);

    avctx->codec_id = codec->id;


    ic->streams[stream_index]->discard = AVDISCARD_DEFAULT;
    switch (avctx->codec_type) {
        case AVMEDIA_TYPE_VIDEO:
            is->video_stream = stream_index;
            is->video_st = ic->streams[stream_index];

            decoder_init(&is->viddec, avctx, &is->videoq);
            if ((ret = decoder_start(&is->viddec, video_thread, is)) < 0)
                goto out;
            is->queue_attachments_req = 1;
            break;
        default:
            break;
    }
    goto out;

    fail:
    avcodec_free_context(&avctx);

    out:

    return ret;
}

int read_thread(void *arg) {
    VideoState *is = static_cast<VideoState *>(arg);
    AVFormatContext *ic = NULL;
    int st_index[AVMEDIA_TYPE_NB];
    int err, i, ret;

    memset(st_index, -1, sizeof(st_index));

    ic = avformat_alloc_context();
    if (!ic) {
        //fail
    }

    err = avformat_open_input(&ic, is->filePath, NULL, NULL);
    if (err < 0) {
        ret = -1;
        //fail
    }

    is->ic = ic;
    av_format_inject_global_side_data(ic);

    if (avformat_find_stream_info(ic, NULL) < 0) {
        LOGE("%s", "获取视频信息失败");
    }

    if (ic->pb)
        ic->pb->eof_reached = 0;

    st_index[AVMEDIA_TYPE_VIDEO] =
            av_find_best_stream(ic, AVMEDIA_TYPE_VIDEO,
                                st_index[AVMEDIA_TYPE_VIDEO], -1, NULL, 0);

    if (st_index[AVMEDIA_TYPE_VIDEO] >= 0) {
        AVStream *st = ic->streams[st_index[AVMEDIA_TYPE_VIDEO]];
        AVCodecParameters *codecpar = st->codecpar;
        AVRational sar = av_guess_sample_aspect_ratio(ic, st, NULL);
//        if (codecpar->width)
//            set_default_window_size(codecpar->width, codecpar->height, sar);
    }

    ret = -1;
    if (st_index[AVMEDIA_TYPE_VIDEO] >= 0) {
        ret = stream_component_open(is, st_index[AVMEDIA_TYPE_VIDEO]);
    }

}

void stream_open(const char *path) {
    VideoState *is;

    is = static_cast<VideoState *>(av_mallocz(sizeof(VideoState)));

    is->filePath = av_strdup(path);

    if (frame_queue_init(&is->pictq, &is->videoq, VIDEO_PICTURE_QUEUE_SIZE, 1) < 0) {
        // fail
    }

    if (packet_queue_init(&is->videoq) < 0) {
        // fail
    }

}

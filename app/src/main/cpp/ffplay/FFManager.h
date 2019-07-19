//
// Created by Di on 2019-07-19.
//

#ifndef FFMPEGDEMO_FFMANAGER_H
#define FFMPEGDEMO_FFMANAGER_H

#include <queue>
#include<vector>
#include <SLES/OpenSLES_Android.h>
#include "../FFmpegMusic.h"

extern "C" {
#include <unistd.h>
#include <libavcodec/avcodec.h>
#include <pthread.h>
#include <libswresample/swresample.h>
#include <libswscale/swscale.h>
#include <libavformat/avformat.h>
#include "../Log.h"
#include <libavutil/imgutils.h>
#include <libavutil/time.h>

class FFManager {

public:

    void test();
};

};
#endif //FFMPEGDEMO_FFMANAGER_H

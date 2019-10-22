#include <jni.h>
#include <string>
#include <android/native_window_jni.h>


extern "C" {

#include <unistd.h>
#include "Log.h"
}
const char *inputPath;
pthread_t p_tid;
int isPlay;
ANativeWindow *window = 0;
int64_t duration;
int step = 0;
jboolean isSeek = false;



/*void swap(int *a,int *b)
{
    int t=*a;*a=*b;*b=t;
}*/



extern "C"
JNIEXPORT void JNICALL
Java_com_test_ffmpegvideoplay_Play_play(JNIEnv *env, jobject instance, jstring inputPath_) {
    inputPath = env->GetStringUTFChars(inputPath_, 0);

//    initFFmpeg();
//    initPlayer();
//    startPlay();

    LOGE("打开文件 %s", inputPath);
//    startVideo(inputPath);
    LOGE("打开文件2222222 %s", inputPath);



    env->ReleaseStringUTFChars(inputPath_, inputPath);
}
extern "C"
JNIEXPORT void JNICALL
Java_com_test_ffmpegvideoplay_Play_display(JNIEnv *env, jobject instance, jobject surface) {
    //得到界面
}
extern "C"
JNIEXPORT void JNICALL
Java_com_test_ffmpegvideoplay_Play_release(JNIEnv *env, jobject instance) {
}extern "C"
JNIEXPORT void JNICALL
Java_com_test_ffmpegvideoplay_Play_stop(JNIEnv *env, jobject instance) {

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
}
extern "C"
JNIEXPORT void JNICALL
Java_com_test_ffmpegvideoplay_Play_seekTo(JNIEnv *env, jobject instance, jint msec) {
}
extern "C"
JNIEXPORT void JNICALL
Java_com_test_ffmpegvideoplay_Play_stepBack(JNIEnv *env, jobject instance) {

}
extern "C"
JNIEXPORT void JNICALL
Java_com_test_ffmpegvideoplay_Play_stepUp(JNIEnv *env, jobject instance) {
    //点击快进按钮
}

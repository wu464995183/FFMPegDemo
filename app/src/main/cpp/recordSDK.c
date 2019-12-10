//
// Created by Di on 2019-12-10.
//

#include <unistd.h>
#include <jni.h>
#include <android/log.h>

#define loge(content)   __android_log_write(ANDROID_LOG_ERROR,"eric",content)

JNIEXPORT void JNICALL
Java_com_wangheart_rtmpfile_camera_1test_Play_play(JNIEnv *env, jobject thiz) {
    loge("我在测试");
}

cmake_minimum_required(VERSION 3.5)

if (NOT DEFINED FIJKROOT_PATH)
    message(FATAL_ERROR, "ijkmedi/cmake编译错误")
    set(FIJKROOT_PATH ${CMAKE_CURRENT_LIST_DIR}/../)
endif ()

add_subdirectory(ijksdl)
add_subdirectory(ijkplayer) #可疑

if (ANDROID)
    add_subdirectory(ijksoundtouch)
    add_subdirectory(ijkyuv)
    add_subdirectory(ijkj4a)
    # just a dummy android ndk prifiler
    option(ANDROID_NDK_PROFILER OFF)
    if (NDK_PROFILER)
        # not implemented yet
        message(FATAL_ERROR "not implemented yet, android-ndk-profiler")
        add_subdirectory(../ijkprof/android-ndk-profiler/jni ijkprof)
    else ()
        add_subdirectory(../ijkprof/android-ndk-profiler-dummy/jni ijkprof)
    endif (NDK_PROFILER)

    # add_dependencies(ijkplayer android-ndk-profiler)
endif ()

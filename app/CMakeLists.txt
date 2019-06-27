cmake_minimum_required(VERSION 3.4.1)

file(GLOB my_source src/main/cpp/*.cpp src/main/cpp/*.c)
set(JNI_LIBS_DIR ${CMAKE_SOURCE_DIR}/src/main/jniLibs)

add_library( native-lib
             SHARED
             ${my_source}
              )
add_library( avcodec-56
             SHARED
             IMPORTED)
set_target_properties( avcodec-56
                       PROPERTIES IMPORTED_LOCATION
        ${JNI_LIBS_DIR}/${ANDROID_ABI}/libavcodec.so )
add_library( avfilter-5
             SHARED
             IMPORTED)
set_target_properties( avfilter-5
                       PROPERTIES IMPORTED_LOCATION
        ${JNI_LIBS_DIR}/${ANDROID_ABI}/libavfilter.so)

add_library( avformat-56
             SHARED
             IMPORTED)
set_target_properties( avformat-56
                       PROPERTIES IMPORTED_LOCATION
        ${JNI_LIBS_DIR}/${ANDROID_ABI}/libavformat.so)

add_library( avutil-54
             SHARED
             IMPORTED)
set_target_properties( avutil-54
                       PROPERTIES IMPORTED_LOCATION
        ${JNI_LIBS_DIR}/${ANDROID_ABI}/libavutil.so)

add_library( swresample-1
             SHARED
             IMPORTED)
set_target_properties( swresample-1
                       PROPERTIES IMPORTED_LOCATION
        ${JNI_LIBS_DIR}/${ANDROID_ABI}/libswresample.so)

add_library( swscale-3
             SHARED
             IMPORTED)
set_target_properties( swscale-3
                       PROPERTIES IMPORTED_LOCATION
        ${JNI_LIBS_DIR}/${ANDROID_ABI}/libswscale.so)
include_directories(${JNI_LIBS_DIR}/includes)
target_link_libraries( native-lib
                       avcodec-56
                       avfilter-5
                       avformat-56
                       avutil-54
                       swresample-1
                       swscale-3
                       log
                       android
                       OpenSLES
                        )

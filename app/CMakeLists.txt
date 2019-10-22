cmake_minimum_required(VERSION 3.4.1)

file(GLOB my_source src/main/cpp/*.cpp src/main/cpp/ffplay/*.cpp src/main/cpp/ffplay/*.c src/main/cpp/ffplay/*.h src/main/cpp/*.c)
set(JNI_LIBS_DIR ${CMAKE_SOURCE_DIR}/src/main/jniLibs)





set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 11)
set(ANDROID_PLATFORM_LEVEL 16)


add_definitions(-DANDROID)

message(STATUS "-------------开始打印------------")
message(STATUS "CMAKE_BUILD_TYPE: ${CMAKE_BUILD_TYPE}")
message(STATUS "ANDROID_PLATFORM_LEVEL:${ANDROID_PLATFORM_LEVEL}")
message(STATUS "ANDROID_ABI:${ANDROID_ABI}")
message(STATUS "PROJECT_SOURCE_DIR ${PROJECT_SOURCE_DIR}")

set(FIJKROOT_PATH "${PROJECT_SOURCE_DIR}/src/main/cpp")
get_filename_component(FIJKROOT_PATH ${FIJKROOT_PATH} ABSOLUTE)
message(STATUS "FIJKROOT_PATH ${FIJKROOT_PATH}")


if (${CMAKE_BUILD_TYPE} STREQUAL "Release")
    add_definitions(-DNDEBUG)
elseif (${CMAKE_BUILD_TYPE} STREQUAL "Debug")
    add_definitions(-DDEBUG)
endif ()


set(CMAKE_POSITION_INDEPENDENT_CODE ON)


add_library( native-lib
             SHARED
             ${my_source}
              )


message(STATUE "${CMAKE_LIBRARY_OUTPUT_DIRECTORY} ------- ${PROJECT_BINARY_DIR}")
#add_subdirectory(${FIJKROOT_PATH}/ijkmedia ijkmedia)

#add_library( avcodec-56
#             SHARED
#             IMPORTED)
#set_target_properties( avcodec-56
#                       PROPERTIES IMPORTED_LOCATION
#        ${JNI_LIBS_DIR}/${ANDROID_ABI}/libavcodec.so )
#add_library( avfilter-5
#             SHARED
#             IMPORTED)
#set_target_properties( avfilter-5
#                       PROPERTIES IMPORTED_LOCATION
#        ${JNI_LIBS_DIR}/${ANDROID_ABI}/libavfilter.so)
#
#add_library( avformat-56
#             SHARED
#             IMPORTED)
#set_target_properties( avformat-56
#                       PROPERTIES IMPORTED_LOCATION
#        ${JNI_LIBS_DIR}/${ANDROID_ABI}/libavformat.so)
#
#add_library( avutil-54
#             SHARED
#             IMPORTED)
#set_target_properties( avutil-54
#                       PROPERTIES IMPORTED_LOCATION
#        ${JNI_LIBS_DIR}/${ANDROID_ABI}/libavutil.so)
#
#add_library( swresample-1
#             SHARED
#             IMPORTED)
#set_target_properties( swresample-1
#                       PROPERTIES IMPORTED_LOCATION
#        ${JNI_LIBS_DIR}/${ANDROID_ABI}/libswresample.so)
#
#add_library( swscale-3
#             SHARED
#             IMPORTED)
#set_target_properties( swscale-3
#                       PROPERTIES IMPORTED_LOCATION
#        ${JNI_LIBS_DIR}/${ANDROID_ABI}/libswscale.so)
#include_directories(${JNI_LIBS_DIR}/includes)
target_link_libraries( native-lib
#                       avcodec-56
#                       avfilter-5
#                       avformat-56
#                       avutil-54
#                       swresample-1
#                       swscale-3
                       log
                       android
                       OpenSLES
                        )
message(STATUS "-------------结束打印------------")

cmake_minimum_required(VERSION 3.5)

file(GLOB my_source src/main/cpp/*.cpp src/main/cpp/ffplay/*.cpp src/main/cpp/ffplay/*.c src/main/cpp/ffplay/*.h src/main/cpp/*.c)
set(JNI_LIBS_DIR ${PROJECT_SOURCE_DIR}/src/main/jniLibs)





set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 11)
set(ANDROID_PLATFORM_LEVEL 16)


add_definitions(-DANDROID)

message(STATUS "-------------开始打印------------")
message(STATUS "CMAKE_BUILD_TYPE: ${CMAKE_BUILD_TYPE}")
message(STATUS "ANDROID_PLATFORM_LEVEL:${ANDROID_PLATFORM_LEVEL}")
message(STATUS "ANDROID_ABI:${ANDROID_ABI}")
message(STATUS "PROJECT_SOURCE_DIR ${PROJECT_SOURCE_DIR}")
message(STATUS " ffmpeg的位置    ${JNI_LIBS_DIR}/${ANDROID_ABI}/libijkffmpeg.so")

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

add_library(ijkffmpeg SHARED IMPORTED)
set_target_properties( # Specifies the target library.
        ijkffmpeg
        PROPERTIES
        IMPORTED_LOCATION ${JNI_LIBS_DIR}/${ANDROID_ABI}/libijkffmpeg.so
        )


include_directories(${JNI_LIBS_DIR}/includes)
target_link_libraries( native-lib
#                       ijkffmpeg
                       log
#                       android
#                       OpenSLES
                        )



message(STATUE "${CMAKE_LIBRARY_OUTPUT_DIRECTORY} ------- ${PROJECT_BINARY_DIR}")
add_subdirectory(${FIJKROOT_PATH}/ijkmedia ijkmedia)

message(STATUS "-------------结束打印------------")

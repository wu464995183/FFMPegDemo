# Install script for directory: /Users/di/FFMPegDemo/app/src/main/cpp/ijkmedia/ijkyuv

# Set the install prefix
if(NOT DEFINED CMAKE_INSTALL_PREFIX)
  set(CMAKE_INSTALL_PREFIX "/usr/local")
endif()
string(REGEX REPLACE "/$" "" CMAKE_INSTALL_PREFIX "${CMAKE_INSTALL_PREFIX}")

# Set the install configuration name.
if(NOT DEFINED CMAKE_INSTALL_CONFIG_NAME)
  if(BUILD_TYPE)
    string(REGEX REPLACE "^[^A-Za-z0-9_]+" ""
           CMAKE_INSTALL_CONFIG_NAME "${BUILD_TYPE}")
  else()
    set(CMAKE_INSTALL_CONFIG_NAME "Debug")
  endif()
  message(STATUS "Install configuration: \"${CMAKE_INSTALL_CONFIG_NAME}\"")
endif()

# Set the component getting installed.
if(NOT CMAKE_INSTALL_COMPONENT)
  if(COMPONENT)
    message(STATUS "Install component: \"${COMPONENT}\"")
    set(CMAKE_INSTALL_COMPONENT "${COMPONENT}")
  else()
    set(CMAKE_INSTALL_COMPONENT)
  endif()
endif()

# Install shared libraries without execute permission?
if(NOT DEFINED CMAKE_INSTALL_SO_NO_EXE)
  set(CMAKE_INSTALL_SO_NO_EXE "0")
endif()

if("${CMAKE_INSTALL_COMPONENT}" STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib" TYPE STATIC_LIBRARY FILES "/Users/di/FFMPegDemo/app/.cxx/cmake/debug/arm64-v8a/ijkmedia/ijkyuv/libyuv_static.a")
endif()

if("${CMAKE_INSTALL_COMPONENT}" STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include/libyuv" TYPE FILE FILES
    "/Users/di/FFMPegDemo/app/src/main/cpp/ijkmedia/ijkyuv/include/libyuv/basic_types.h"
    "/Users/di/FFMPegDemo/app/src/main/cpp/ijkmedia/ijkyuv/include/libyuv/compare.h"
    "/Users/di/FFMPegDemo/app/src/main/cpp/ijkmedia/ijkyuv/include/libyuv/convert.h"
    "/Users/di/FFMPegDemo/app/src/main/cpp/ijkmedia/ijkyuv/include/libyuv/convert_argb.h"
    "/Users/di/FFMPegDemo/app/src/main/cpp/ijkmedia/ijkyuv/include/libyuv/convert_from.h"
    "/Users/di/FFMPegDemo/app/src/main/cpp/ijkmedia/ijkyuv/include/libyuv/convert_from_argb.h"
    "/Users/di/FFMPegDemo/app/src/main/cpp/ijkmedia/ijkyuv/include/libyuv/cpu_id.h"
    "/Users/di/FFMPegDemo/app/src/main/cpp/ijkmedia/ijkyuv/include/libyuv/format_conversion.h"
    "/Users/di/FFMPegDemo/app/src/main/cpp/ijkmedia/ijkyuv/include/libyuv/planar_functions.h"
    "/Users/di/FFMPegDemo/app/src/main/cpp/ijkmedia/ijkyuv/include/libyuv/rotate.h"
    "/Users/di/FFMPegDemo/app/src/main/cpp/ijkmedia/ijkyuv/include/libyuv/rotate_argb.h"
    "/Users/di/FFMPegDemo/app/src/main/cpp/ijkmedia/ijkyuv/include/libyuv/row.h"
    "/Users/di/FFMPegDemo/app/src/main/cpp/ijkmedia/ijkyuv/include/libyuv/scale.h"
    "/Users/di/FFMPegDemo/app/src/main/cpp/ijkmedia/ijkyuv/include/libyuv/scale_argb.h"
    "/Users/di/FFMPegDemo/app/src/main/cpp/ijkmedia/ijkyuv/include/libyuv/scale_row.h"
    "/Users/di/FFMPegDemo/app/src/main/cpp/ijkmedia/ijkyuv/include/libyuv/version.h"
    "/Users/di/FFMPegDemo/app/src/main/cpp/ijkmedia/ijkyuv/include/libyuv/video_common.h"
    "/Users/di/FFMPegDemo/app/src/main/cpp/ijkmedia/ijkyuv/include/libyuv/mjpeg_decoder.h"
    )
endif()

if("${CMAKE_INSTALL_COMPONENT}" STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include" TYPE FILE FILES "/Users/di/FFMPegDemo/app/src/main/cpp/ijkmedia/ijkyuv/include/libyuv.h")
endif()


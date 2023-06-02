# For more information about using CMake with Android Studio, read the
# documentation: https://d.android.com/studio/projects/add-native-code.html

# Sets the minimum version of CMake required to build the native library.
cmake_minimum_required(VERSION 3.10.2)
project("anyLive")
enable_language(ASM)
set(CMAKE_ASM_COMPILER ${CMAKE_C_COMPILER})
set(STATIC_LIB_DIR ${CMAKE_SOURCE_DIR}/lib/)
set(ROOT_DIR ./../../../../../)
set(ARLIVE_DIR ./../../../../../ArLiveLite)#publish
#set(ARLIVE_DIR ./ArLiveLite)#debug
set(CMAKE_SHARED_LINKER_FLAGS "${CMAKE_SHARED_LINKER_FLAGS} -Wl,--gc-sections -Wl,--exclude-libs,
${STATIC_LIB_DIR}/ffmpeg/${ANDROID_ABI}/libswscale.so,
${STATIC_LIB_DIR}/ffmpeg/${ANDROID_ABI}/libavformat.so,
${STATIC_LIB_DIR}/ffmpeg/${ANDROID_ABI}/libavcodec.so,
${STATIC_LIB_DIR}/ffmpeg/${ANDROID_ABI}/libavfilter.so,
${STATIC_LIB_DIR}/ffmpeg/${ANDROID_ABI}/libavutil.so,
${STATIC_LIB_DIR}/ffmpeg/${ANDROID_ABI}/libswresample.so,
${STATIC_LIB_DIR}/ffmpeg/${ANDROID_ABI}/libpostproc.so,
${STATIC_LIB_DIR}/webrtc/${ANDROID_ABI}/libwebrtc.a")

set(NATIVE_LIB "anyLive")

add_library(avutil SHARED IMPORTED)
set_target_properties(avutil PROPERTIES IMPORTED_LOCATION ${STATIC_LIB_DIR}/ffmpeg/${ANDROID_ABI}/libavutil.so)

add_library(avformat SHARED IMPORTED)
set_target_properties(avformat PROPERTIES IMPORTED_LOCATION ${STATIC_LIB_DIR}/ffmpeg/${ANDROID_ABI}/libavformat.so)

add_library(avcodec SHARED IMPORTED)
set_target_properties(avcodec PROPERTIES IMPORTED_LOCATION ${STATIC_LIB_DIR}/ffmpeg/${ANDROID_ABI}/libavcodec.so)

add_library(avfilter SHARED IMPORTED)
set_target_properties(avfilter PROPERTIES IMPORTED_LOCATION ${STATIC_LIB_DIR}/ffmpeg/${ANDROID_ABI}/libavfilter.so)

add_library(swresample SHARED IMPORTED)
set_target_properties(swresample PROPERTIES IMPORTED_LOCATION ${STATIC_LIB_DIR}/ffmpeg/${ANDROID_ABI}/libswresample.so)

add_library(swscale SHARED IMPORTED)
set_target_properties(swscale PROPERTIES IMPORTED_LOCATION ${STATIC_LIB_DIR}/ffmpeg/${ANDROID_ABI}/libswscale.so)

add_library(postproc SHARED IMPORTED)
set_target_properties(postproc PROPERTIES IMPORTED_LOCATION ${STATIC_LIB_DIR}/ffmpeg/${ANDROID_ABI}/libpostproc.so)

add_library(webrtc STATIC IMPORTED)
set_target_properties(webrtc PROPERTIES IMPORTED_LOCATION ${STATIC_LIB_DIR}/webrtc/${ANDROID_ABI}/libwebrtc.a)

include(${ROOT_DIR}/third_party/faac-1.28/CMakeLists.txt)
include(${ARLIVE_DIR}/rtmp/CMakeLists.txt)
add_library(
        ${NATIVE_LIB}
        SHARED
        ${CMAKE_SOURCE_DIR}/jni/LiveEngine.cpp
        ${CMAKE_SOURCE_DIR}/jni/StaticThreads.cpp
        ${CMAKE_SOURCE_DIR}/jni/android/AndroidContext.cpp
        ${CMAKE_SOURCE_DIR}/jni/android/VideoCameraCapturer.cpp
        ${CMAKE_SOURCE_DIR}/jni/liveEngine/ArLivePlayEvent.cpp
        ${CMAKE_SOURCE_DIR}/jni/liveEngine/ArLivePushEvent.cpp
        ${CMAKE_SOURCE_DIR}/jni/liveEngine/AndroidDeviceManager.cpp

       ${ARLIVE_DIR}/AndroidRenderer.cpp
       ${ARLIVE_DIR}/ArLive2Engine.cpp
       ${ARLIVE_DIR}/PlatformImpl.cpp
       ${ARLIVE_DIR}/ArLive2Player.cpp
       ${ARLIVE_DIR}/ArLive2Pusher.cpp
       ${ARLIVE_DIR}/codec/aacencode.cc
       ${ARLIVE_DIR}/codec/AvCodec.cc
       ${ARLIVE_DIR}/MgrRender.cpp
       ${ARLIVE_DIR}/PlayBuffer.cpp
       ${ARLIVE_DIR}/player/ARFFPlayer.cpp
       ${ARLIVE_DIR}/player/FFBuffer.cpp
       ${ARLIVE_DIR}/player/sonic.c
       ${ARLIVE_DIR}/pusher/ARRtmpPusher.cpp
       ${ARLIVE_DIR}/ArNetClient.cpp
       ${ARLIVE_DIR}/ArNetTcpClient.cpp
       ${ARLIVE_DIR}/RtcTick.cpp
       ${ARLIVE_DIR}/H264SeiPack.cpp
       ${CMAKE_SOURCE_DIR}/jni/util/ClassreferenceHolder.cc

)

find_library( # Sets the name of the path variable.
        log-lib

        # Specifies the name of the NDK library that
        # you want CMake to locate.
        log)

target_link_libraries(
        ${NATIVE_LIB}
        faac
        rtmp
        -Wl,--start-group
        swscale
        avformat
        avcodec
        avfilter
        swresample
        postproc
        avutil
        -Wl,--end-group
        -Wl,--whole-archive
        webrtc
        -Wl,--no-whole-archive
        OpenSLES
        ${log-lib}
)



include_directories(${CMAKE_SOURCE_DIR})
include_directories(${CMAKE_SOURCE_DIR}/jni/)
include_directories(${CMAKE_SOURCE_DIR}/jni/util/)
include_directories(${CMAKE_SOURCE_DIR}/jni/android/)
include_directories(${CMAKE_SOURCE_DIR}/webrtc)
include_directories(${CMAKE_SOURCE_DIR}/webrtc/sdk/)
include_directories(${CMAKE_SOURCE_DIR}/webrtc/sdk/android/src/jni/)
include_directories(${CMAKE_SOURCE_DIR}/webrtc/third_party/abseil-cpp/)
include_directories(${CMAKE_SOURCE_DIR}/lib/ffmpeg/include/)
include_directories(${ARLIVE_DIR})
include_directories(${ARLIVE_DIR}/include/)
include_directories(${ARLIVE_DIR}/codec/)
include_directories(${ARLIVE_DIR}/player/)
include_directories(${ARLIVE_DIR}/pusher/)
include_directories(${ROOT_DIR}/VideoRender/)
include_directories(${ROOT_DIR}/third_party/)
include_directories(${ROOT_DIR}/third_party/libyuv/include/)
include_directories(${ROOT_DIR}/third_party/rapidjson/)




add_definitions("-DWEBRTC_POSIX")
add_definitions("-DWEBRTC_ANDROID")
add_definitions("-DWEBRTC_USE_H264")
add_definitions("-DNDEBUG")
add_definitions("-DBUILDFLAG_INTERNAL_EXPENSIVE_DCHECKS_ARE_ON")
add_definitions("-DRW_LOCK")
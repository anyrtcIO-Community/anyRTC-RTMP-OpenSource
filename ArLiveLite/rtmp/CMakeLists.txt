cmake_minimum_required(VERSION 3.10.2)

add_library(rtmp STATIC
        ${CMAKE_SOURCE_DIR}/ArLive/rtmp/libflv/source/amf0.c
        ${CMAKE_SOURCE_DIR}/ArLive/rtmp/libflv/source/amf3.c
        ${CMAKE_SOURCE_DIR}/ArLive/rtmp/libflv/source/aom-av1.c
        ${CMAKE_SOURCE_DIR}/ArLive/rtmp/libflv/source/flv-demuxer-script.c
        ${CMAKE_SOURCE_DIR}/ArLive/rtmp/libflv/source/flv-demuxer.c
        ${CMAKE_SOURCE_DIR}/ArLive/rtmp/libflv/source/flv-header.c
        ${CMAKE_SOURCE_DIR}/ArLive/rtmp/libflv/source/flv-muxer.c
        ${CMAKE_SOURCE_DIR}/ArLive/rtmp/libflv/source/flv-parser.c
        ${CMAKE_SOURCE_DIR}/ArLive/rtmp/libflv/source/flv-reader.c
        ${CMAKE_SOURCE_DIR}/ArLive/rtmp/libflv/source/flv-writer.c
        ${CMAKE_SOURCE_DIR}/ArLive/rtmp/libflv/source/hevc-annexbtomp4.c
        ${CMAKE_SOURCE_DIR}/ArLive/rtmp/libflv/source/hevc-mp4toannexb.c
        ${CMAKE_SOURCE_DIR}/ArLive/rtmp/libflv/source/mp3-header.c
        ${CMAKE_SOURCE_DIR}/ArLive/rtmp/libflv/source/mpeg4-aac-asc.c
        ${CMAKE_SOURCE_DIR}/ArLive/rtmp/libflv/source/mpeg4-aac.c
        ${CMAKE_SOURCE_DIR}/ArLive/rtmp/libflv/source/mpeg4-annexbtomp4.c
        ${CMAKE_SOURCE_DIR}/ArLive/rtmp/libflv/source/mpeg4-avc.c
        ${CMAKE_SOURCE_DIR}/ArLive/rtmp/libflv/source/mpeg4-hevc.c
        ${CMAKE_SOURCE_DIR}/ArLive/rtmp/libflv/source/mpeg4-mp4toannexb.c
        ${CMAKE_SOURCE_DIR}/ArLive/rtmp/libflv/source/opus-head.c
        ${CMAKE_SOURCE_DIR}/ArLive/rtmp/libflv/source/webm-vpx.c
        ${CMAKE_SOURCE_DIR}/ArLive/rtmp/librtmp/source/rtmp-chunk-header.c
        ${CMAKE_SOURCE_DIR}/ArLive/rtmp/librtmp/source/rtmp-chunk-read.c
        ${CMAKE_SOURCE_DIR}/ArLive/rtmp/librtmp/source/rtmp-chunk-write.c
        ${CMAKE_SOURCE_DIR}/ArLive/rtmp/librtmp/source/rtmp-client.c
        ${CMAKE_SOURCE_DIR}/ArLive/rtmp/librtmp/source/rtmp-control-handler.c
        ${CMAKE_SOURCE_DIR}/ArLive/rtmp/librtmp/source/rtmp-control-message.c
        ${CMAKE_SOURCE_DIR}/ArLive/rtmp/librtmp/source/rtmp-event.c
        ${CMAKE_SOURCE_DIR}/ArLive/rtmp/librtmp/source/rtmp-handler.c
        ${CMAKE_SOURCE_DIR}/ArLive/rtmp/librtmp/source/rtmp-handshake.c
        ${CMAKE_SOURCE_DIR}/ArLive/rtmp/librtmp/source/rtmp-invoke-handler.c
        ${CMAKE_SOURCE_DIR}/ArLive/rtmp/librtmp/source/rtmp-netconnection.c
        ${CMAKE_SOURCE_DIR}/ArLive/rtmp/librtmp/source/rtmp-netstream.c
        )

target_include_directories(rtmp PUBLIC
        ${CMAKE_SOURCE_DIR}/ArLive/rtmp/
        ${CMAKE_SOURCE_DIR}/ArLive/rtmp/librtmp/include/
        ${CMAKE_SOURCE_DIR}/ArLive/rtmp/librtmp/source/
        ${CMAKE_SOURCE_DIR}/ArLive/rtmp/librtmp/aio/
        ${CMAKE_SOURCE_DIR}/ArLive/rtmp/libflv/include/
      )

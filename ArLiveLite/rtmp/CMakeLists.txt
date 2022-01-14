cmake_minimum_required(VERSION 3.10.2)
set(RTMP_DIR ./../../../../../ArLiveLite/rtmp)

add_library(rtmp STATIC
        ${RTMP_DIR}/libflv/source/amf0.c
         ${RTMP_DIR}/libflv/source/amf3.c
         ${RTMP_DIR}/libflv/source/aom-av1.c
         ${RTMP_DIR}/libflv/source/flv-demuxer-script.c
         ${RTMP_DIR}/libflv/source/flv-demuxer.c
         ${RTMP_DIR}/libflv/source/flv-header.c
         ${RTMP_DIR}/libflv/source/flv-muxer.c
         ${RTMP_DIR}/libflv/source/flv-parser.c
         ${RTMP_DIR}/libflv/source/flv-reader.c
         ${RTMP_DIR}/libflv/source/flv-writer.c
         ${RTMP_DIR}/libflv/source/hevc-annexbtomp4.c
         ${RTMP_DIR}/libflv/source/hevc-mp4toannexb.c
         ${RTMP_DIR}/libflv/source/mp3-header.c
         ${RTMP_DIR}/libflv/source/mpeg4-aac-asc.c
         ${RTMP_DIR}/libflv/source/mpeg4-aac.c
         ${RTMP_DIR}/libflv/source/mpeg4-annexbtomp4.c
         ${RTMP_DIR}/libflv/source/mpeg4-avc.c
         ${RTMP_DIR}/libflv/source/mpeg4-hevc.c
         ${RTMP_DIR}/libflv/source/mpeg4-mp4toannexb.c
         ${RTMP_DIR}/libflv/source/opus-head.c
         ${RTMP_DIR}/libflv/source/webm-vpx.c
         ${RTMP_DIR}/librtmp/source/rtmp-chunk-header.c
         ${RTMP_DIR}/librtmp/source/rtmp-chunk-read.c
         ${RTMP_DIR}/librtmp/source/rtmp-chunk-write.c
         ${RTMP_DIR}/librtmp/source/rtmp-client.c
         ${RTMP_DIR}/librtmp/source/rtmp-control-handler.c
         ${RTMP_DIR}/librtmp/source/rtmp-control-message.c
         ${RTMP_DIR}/librtmp/source/rtmp-event.c
         ${RTMP_DIR}/librtmp/source/rtmp-handler.c
         ${RTMP_DIR}/librtmp/source/rtmp-handshake.c
         ${RTMP_DIR}/librtmp/source/rtmp-invoke-handler.c
         ${RTMP_DIR}/librtmp/source/rtmp-netconnection.c
         ${RTMP_DIR}/librtmp/source/rtmp-netstream.c
        )

target_include_directories(rtmp PUBLIC
         ${RTMP_DIR}/
         ${RTMP_DIR}/librtmp/include/
         ${RTMP_DIR}/librtmp/source/
         ${RTMP_DIR}/librtmp/aio/
         ${RTMP_DIR}/libflv/include/
      )

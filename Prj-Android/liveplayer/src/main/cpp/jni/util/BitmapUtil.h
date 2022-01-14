//
// Created by liu on 2022/1/14.
//

#ifndef LIVEPLAYER_BITMAPUTIL_H
#define LIVEPLAYER_BITMAPUTIL_H

#include <jni.h>
#include <libyuv.h>
#include <android/bitmap.h>

namespace anyrtc {

    typedef enum ImageFormat {
        NONE = 0,
        NV21 = 1,
        I420 = 2,
        RGB565 = 3,
        RGB24 = 4,    // ie. BGR888
        ARGB_8888 = 5 // in libyuv is ABGR format
    } ImageFormat;

    ImageFormat getFormat(int formatValue);
}

JNIEXPORT jbyteArray bitmapToRgba(JNIEnv *env, jobject jbitmap);

int RGB565ToRGBA(const uint8_t *src_rgb_data, int width, int height, uint8_t *dst_rgba_data);

template<typename Func>
jbyteArray bitmapToByteArray(JNIEnv *env, jobject jbitmap, Func callback);


#endif //LIVEPLAYER_BITMAPUTIL_H

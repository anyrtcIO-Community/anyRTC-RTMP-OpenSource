//
// Created by liu on 2022/1/14.
//


#include <common.h>
#include "BitmapUtil.h"
#include "cppToJavaHelper.hpp"


jbyteArray bitmapToRgba(JNIEnv *env,jobject jbitmap) {
    if (jbitmap == NULL) {
        return NULL;
    }
    auto callback = [](uint8_t **target_data, int width, int height, int *dataSize, uint8_t *pixel,
                       BitmapFormat format) -> int {
        *dataSize = width * height * 4;
        *target_data = new uint8_t[*dataSize];
        if (format == ARGB_8888) {
            memcpy(*target_data, pixel, sizeof(uint8_t) * width * height * 4);
            return 0;
        } else if (format == RGB_565) {
            return RGB565ToRGBA(pixel, width, height, *target_data);
        } else {
            return -1;
        }
    };

    return bitmapToByteArray(env, jbitmap, callback);
}

int RGB565ToRGBA(const uint8_t *src_rgb_data, int width, int height, uint8_t *dst_rgba_data) {
    if (libyuv::RGB565ToARGB(src_rgb_data, width * 2, dst_rgba_data, width * 4, width, height) !=0) {
        return -1;
    }
    return libyuv::ARGBToABGR(dst_rgba_data, width * 4, dst_rgba_data, width * 4, width, height);
}

template<typename Func>
jbyteArray bitmapToByteArray(JNIEnv *env, jobject jbitmap, Func callback) {
    if (jbitmap == NULL) {
        return NULL;
    }
    AndroidBitmapInfo info;
    uint8_t *pixels;
    AndroidBitmap_getInfo(env, jbitmap, &info);
    AndroidBitmap_lockPixels(env, jbitmap, reinterpret_cast<void **>(&pixels));

    uint8_t *target_data = NULL;
    int dataSize = 0;
    int code = -1;
    if (info.format == ANDROID_BITMAP_FORMAT_RGBA_8888) {
        code = callback(&target_data, info.width, info.height, &dataSize, pixels, ARGB_8888);
    } else if (info.format == ANDROID_BITMAP_FORMAT_RGB_565) {
        code = callback(&target_data, info.width, info.height, &dataSize, pixels, RGB_565);
    }

    if (code != 0) {
        AndroidBitmap_unlockPixels(env, jbitmap);
        if (target_data != NULL) {
            delete[] target_data;
        }
        return NULL;
    }
    jbyteArray res = env->NewByteArray(dataSize);
    env->SetByteArrayRegion(res, 0, dataSize, reinterpret_cast<const jbyte *>(target_data));
    AndroidBitmap_unlockPixels(env, jbitmap);

    if (target_data != NULL) {
        delete[] target_data;
    }

    return res;
}




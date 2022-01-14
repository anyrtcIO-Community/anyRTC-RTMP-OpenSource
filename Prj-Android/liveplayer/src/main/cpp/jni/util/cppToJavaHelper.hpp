//
//

#ifndef IMAGEUTILS_CPPTOJAVAHELPER_HPP
#define IMAGEUTILS_CPPTOJAVAHELPER_HPP

#include <jni.h>
#include <android/bitmap.h>

enum BitmapFormat {
    ARGB_8888,
    RGB_565
};

const char *formatToStr(BitmapFormat format) {
    switch (format) {
        case RGB_565:
            return "RGB_565";
        case ARGB_8888:
        default:
            return "ARGB_8888";
    }
}

template<typename Func>
jbyteArray bitmapToByteArray(JNIEnv *env, jobject jbitmap, Func callback);

#endif //IMAGEUTILS_CPPTOJAVAHELPER_HPP

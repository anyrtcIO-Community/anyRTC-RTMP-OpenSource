//
// Created by liu on 2021/9/18.
//

#include "AndroidContext.h"
#include "sdk/android/native_api/jni/jvm.h"
#include <modules/utility/include/jvm_android.h>
#include "webrtc/sdk/android/src/jni/jvm.h"
namespace arlive{

    AndroidContext::AndroidContext(JNIEnv *env, jobject instance,bool isScreen) {
        VideoCapturerDeviceClass = (jclass) env->NewGlobalRef(env->FindClass("io/anyrtc/live/internal/VideoCapturerDevice"));
        jmethodID initMethodId = env->GetMethodID(VideoCapturerDeviceClass, "<init>", "(Z)V");
        javaCapturer = env->NewGlobalRef(env->NewObject(VideoCapturerDeviceClass, initMethodId,isScreen));
    }



    AndroidContext::~AndroidContext() {
            JNIEnv *env = webrtc::AttachCurrentThreadIfNeeded();
            jmethodID onDestroyMethodId = env->GetMethodID(VideoCapturerDeviceClass, "onDestroy", "()V");
            env->CallVoidMethod(javaCapturer, onDestroyMethodId);
            env->DeleteGlobalRef(javaCapturer);
            javaCapturer = nullptr;
            env->DeleteGlobalRef(VideoCapturerDeviceClass);
    }


    jobject AndroidContext::getJavaCapturer() {
        return javaCapturer;
    }

    jclass AndroidContext::getJavaCapturerClass() {
        return VideoCapturerDeviceClass;
    }


}

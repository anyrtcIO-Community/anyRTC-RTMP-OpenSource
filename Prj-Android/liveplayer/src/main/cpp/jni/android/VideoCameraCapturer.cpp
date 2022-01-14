#include "VideoCameraCapturer.h"

#include <cstdint>
#include <memory>
#include <algorithm>

#include "AndroidContext.h"
#include "sdk/android/native_api/jni/jvm.h"

namespace arlive {

VideoCameraCapturer::VideoCameraCapturer(rtc::scoped_refptr<webrtc::JavaVideoTrackSourceInterface> source, std::string deviceId,int width,int height,int fps, std::shared_ptr<PlatformContext> platformContext) : _platformContext(platformContext) {
    AndroidContext *context = (AndroidContext *) platformContext.get();
    JNIEnv *env = webrtc::AttachCurrentThreadIfNeeded();
    jmethodID methodId = env->GetMethodID(context->getJavaCapturerClass(), "init", "(JLjava/lang/String;III)V");
    env->CallVoidMethod(context->getJavaCapturer(), methodId, (jlong) (intptr_t) this, env->NewStringUTF(deviceId.c_str()),width,height,fps);
}


void VideoCameraCapturer::setState(VideoState state) {
    JNIEnv *env = webrtc::AttachCurrentThreadIfNeeded();
    auto context = (AndroidContext *) _platformContext.get();
    jmethodID methodId = env->GetMethodID(context->getJavaCapturerClass(), "onStateChanged", "(JI)V");
    env->CallVoidMethod(context->getJavaCapturer(), methodId, (jlong) (intptr_t) this, (jint) state);
}

bool VideoCameraCapturer::isAutoFocusEnabled() {
    JNIEnv *env = webrtc::AttachCurrentThreadIfNeeded();
    auto context = (AndroidContext *) _platformContext.get();
    jmethodID methodId = env->GetMethodID(context->getJavaCapturerClass(), "isAutoFocusEnabled", "()Z");
    jboolean result = env->CallBooleanMethod(context->getJavaCapturer(), methodId);
    return result;
}

bool VideoCameraCapturer::isFrontCamera() {

}

int VideoCameraCapturer::switchCamera(bool frontCamera) {

}

void VideoCameraCapturer::changeCaptureFormat(int width, int height, int fps) {
    JNIEnv *env = webrtc::AttachCurrentThreadIfNeeded();
    auto context = (AndroidContext *) _platformContext.get();
    jmethodID methodId = env->GetMethodID(context->getJavaCapturerClass(), "setCameraCapturerParam", "(III)V");
    env->CallVoidMethod(context->getJavaCapturer(), methodId,(jint)width,(jint)height,(jint)fps);
}

float VideoCameraCapturer::getCameraZoomMaxRatio() {
    JNIEnv *env = webrtc::AttachCurrentThreadIfNeeded();
    auto context = (AndroidContext *) _platformContext.get();
    jmethodID methodId = env->GetMethodID(context->getJavaCapturerClass(), "getCameraZoomMaxRatio", "()F");
    jfloat result = env->CallFloatMethod(context->getJavaCapturer(), methodId);
    return result;
}

int VideoCameraCapturer::setCameraZoomRatio(float zoomRatio) {
    JNIEnv *env = webrtc::AttachCurrentThreadIfNeeded();
    auto context = (AndroidContext *) _platformContext.get();
    jmethodID methodId = env->GetMethodID(context->getJavaCapturerClass(), "setCameraZoomRatio", "(F)I");
    jint result = env->CallIntMethod(context->getJavaCapturer(), methodId,(jfloat)zoomRatio);
    return result;
}

int VideoCameraCapturer::enableCameraAutoFocus(bool enabled) {
    JNIEnv *env = webrtc::AttachCurrentThreadIfNeeded();
    auto context = (AndroidContext *) _platformContext.get();
    jmethodID methodId = env->GetMethodID(context->getJavaCapturerClass(), "enableCameraAutoFocus", "(Z)I");
    return env->CallIntMethod(context->getJavaCapturer(), methodId,(jboolean)enabled);

}

int VideoCameraCapturer::setCameraFocusPosition(float x, float y) {
    JNIEnv *env = webrtc::AttachCurrentThreadIfNeeded();
    auto context = (AndroidContext *) _platformContext.get();
    jmethodID methodId = env->GetMethodID(context->getJavaCapturerClass(), "setCameraFocusPosition", "(FF)I");
    return env->CallIntMethod(context->getJavaCapturer(), methodId,(jfloat)x,(jfloat)y);
}
int VideoCameraCapturer::enableCameraTorch(bool enabled) {
    JNIEnv *env = webrtc::AttachCurrentThreadIfNeeded();
    auto context = (AndroidContext *) _platformContext.get();
    jmethodID methodId = env->GetMethodID(context->getJavaCapturerClass(), "enableCameraTorch", "(Z)Z");
    jboolean result = env->CallBooleanMethod(context->getJavaCapturer(), methodId, (jboolean) enabled);
    return result?0:-1;
}

int VideoCameraCapturer::setAudioRoute(ArAudioRoute route) {

}

int VideoCameraCapturer::setSystemVolumeType(ArSystemVolumeType type) {

}







}  // namespace ARLIVE

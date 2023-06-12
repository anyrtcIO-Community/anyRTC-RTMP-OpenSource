#include <jni.h>
#include <string>
#include <sdk/android/src/jni/jni_helpers.h>
#include <rtc_base/ssl_adapter.h>
#include <sdk/android/native_api/video/wrapper.h>
#include <modules/utility/include/jvm_android.h>
#include <sdk/android/native_api/base/init.h>
#include <memory>
#include <utility>
#include <map>
#include "ArLive2Engine.h"
#include "IArLivePusher.hpp"
#include <android/AndroidContext.h>
#include <android/native_api/jni/class_loader.h>
#include "liveEngine/ArLivePushEvent.h"
#include "liveEngine/ArLivePlayEvent.h"
#include "media/engine/webrtc_media_engine.h"
#include "libyuv.h"
#include "util/ClassreferenceHolder.h"
#include "liveEngine/AndroidDeviceManager.h"
extern "C" {
using namespace arlive;
using namespace anyrtc;
bool webrtcLoaded = false;
jclass NativeInstance;

void initWebRTC(JNIEnv *env) {
    if (webrtcLoaded) {
        return;
    }
    JavaVM *vm;
    env->GetJavaVM(&vm);
    webrtc::InitAndroid(vm);
    webrtc::JVM::Initialize(vm);
    webrtcLoaded = true;
    NativeInstance = static_cast<jclass>(env->NewGlobalRef(
            env->FindClass("io/anyrtc/live/internal/NativeInstance")));
    webrtc_loader::LoadGlobalClassReferenceHolder();
}


struct InstanceHolder {
    std::shared_ptr<PlatformContext> _platformContext;
    IArLive2Engine *arLiveEngine;
};

jlong getInstanceHolderId(JNIEnv *env, jobject obj) {
    return env->GetLongField(obj, env->GetFieldID(NativeInstance, "nativePtr", "J"));
}

InstanceHolder *getInstanceHolder(JNIEnv *env, jobject obj) {
    return reinterpret_cast<InstanceHolder *>(getInstanceHolderId(env, obj));
}


}

extern "C" {
JNIEXPORT jlong JNICALL
Java_io_anyrtc_live_internal_NativeInstance_makeNativeInstance(JNIEnv *env, jobject obj, jobject instance) {
    initWebRTC(env);
    auto *holder = new InstanceHolder();
    holder->arLiveEngine = V2_CALL::createArLive2Engine();
    std::shared_ptr<PlatformContext> platformContext;
    platformContext = std::make_shared<AndroidContext>(env,obj, false);
    holder->_platformContext = platformContext;
    AndroidDeviceManager::Inst().setPlatformContext(platformContext);
    holder->arLiveEngine->initialize(NULL);
    return reinterpret_cast<jlong>(holder);
}

JNIEXPORT jint JNICALL
Java_io_anyrtc_live_internal_NativeInstance_nativeSetAppInBackground(JNIEnv *env, jobject obj,
                                                                     jboolean background) {
    InstanceHolder *instance = getInstanceHolder(env, obj);
    if (instance->arLiveEngine == nullptr) {
        return (jint) -1;
    }
    instance->arLiveEngine->setAppInBackground(background);
    return (jint) 0;
}




JNIEXPORT jlong JNICALL
Java_io_anyrtc_live_internal_NativeInstance_nativeCreatePushKit(JNIEnv *env, jobject obj) {
    InstanceHolder *instance = getInstanceHolder(env, obj);
    if (instance->arLiveEngine == nullptr){
        return (jint)-1;
    }
    IArLivePusher *arLivePushKit = instance->arLiveEngine->createArLivePusher(NULL);
    std::unique_ptr<webrtc::VideoEncoderFactory> video_encoder_factory = AndroidDeviceManager::Inst().makeVideoEncoderFactory();
    arLivePushKit->setExVideoEncoderFactory(video_encoder_factory.release());
    return reinterpret_cast<jlong>(arLivePushKit);
}

JNIEXPORT void JNICALL
Java_io_anyrtc_live_internal_NativeInstance_nativeSetPushObserver(JNIEnv *env, jobject obj,jobject observer,jlong nativePtr) {
    IArLivePusher* arLivePushKit = reinterpret_cast<IArLivePusher *>(nativePtr);
    if (arLivePushKit != NULL){
        LivePushEvent *pushEvent = new LivePushEvent(observer);
        arLivePushKit->setObserver(pushEvent);
    }
}


JNIEXPORT jint JNICALL
Java_io_anyrtc_live_internal_NativeInstance_nativeStartPush(JNIEnv *env, jobject obj,jlong nativePtr,jstring pushUrl) {
    IArLivePusher* arLivePushKit = reinterpret_cast<IArLivePusher *>(nativePtr);
    if (arLivePushKit != NULL){
        std::string strPushUrl = webrtc::JavaToStdString(env, pushUrl);
        int result =arLivePushKit->startPush(strPushUrl.c_str());
        return (jint)result;
    }
    return (jint)-1;
}

JNIEXPORT jint JNICALL
Java_io_anyrtc_live_internal_NativeInstance_nativeStopPush(JNIEnv *env, jobject obj,jlong nativePtr) {
    InstanceHolder *instance = getInstanceHolder(env, obj);
    IArLivePusher* arLivePushKit = reinterpret_cast<IArLivePusher *>(nativePtr);
    
    if (arLivePushKit != NULL&&instance->arLiveEngine!=NULL){
        int result =arLivePushKit->stopPush();
        instance->arLiveEngine->releaseArLivePusher(arLivePushKit);
        return (jint)result;
    }
    return (jint)-1;
}


JNIEXPORT jint JNICALL
Java_io_anyrtc_live_internal_NativeInstance_nativeIsPushing(JNIEnv *env, jobject obj,jlong nativePtr) {
    IArLivePusher* arLivePushKit = reinterpret_cast<IArLivePusher *>(nativePtr);
    if (arLivePushKit!=NULL){
        bool result = arLivePushKit->isPushing();
        return (jint)result;
    }
    return (jint)-1;
  
}





JNIEXPORT void JNICALL
Java_io_anyrtc_live_internal_NativeInstance_nativeRelease(JNIEnv *env, jobject obj) {
    InstanceHolder *instance = getInstanceHolder(env, obj);
    if (instance == NULL){
        return;
    }
    if (instance->arLiveEngine != NULL){
        instance->arLiveEngine->release();
    }
    delete instance;
}


JNIEXPORT jint JNICALL
Java_io_anyrtc_live_internal_NativeInstance_nativeSetRenderView(JNIEnv *env, jobject obj, jlong nativePtr,jobject localSink) {
    jint result = -1;
    IArLivePusher* arLivePushKit = reinterpret_cast<IArLivePusher *>(nativePtr);
    if (arLivePushKit!= NULL){
        result = arLivePushKit->setRenderView(reinterpret_cast<view_t *>(localSink));
    }
    return (jint)result;
}

JNIEXPORT jint JNICALL
Java_io_anyrtc_live_internal_NativeInstance_nativeSetPushRenderRotation(JNIEnv *env, jobject thiz,jlong nativeHandle,jint rotation
) {
    jint result = -1;
    IArLivePusher* arLivePushKit = reinterpret_cast<IArLivePusher *>(nativeHandle);
    if (arLivePushKit != NULL){
        result = arLivePushKit->setRenderRotation(static_cast<ArLiveRotation>(rotation));
    }
    return (jint)result;
}


JNIEXPORT jint JNICALL
Java_io_anyrtc_live_internal_NativeInstance_nativeStartCamera(JNIEnv *env, jobject obj,jlong nativePtr,jboolean isFront) {
    jint result = -1;
    IArLivePusher* arLivePushKit = reinterpret_cast<IArLivePusher *>(nativePtr);
    if (arLivePushKit!= NULL){
        result = arLivePushKit->startCamera(isFront);
    }
    return (jint)result;
}

JNIEXPORT jint JNICALL
Java_io_anyrtc_live_internal_NativeInstance_nativeStopCamera(JNIEnv *env, jobject obj,jlong nativePtr) {
    jint result = -1;
    IArLivePusher* arLivePushKit = reinterpret_cast<IArLivePusher *>(nativePtr);
    if (arLivePushKit!= NULL){
        result = arLivePushKit->stopCamera();
    }
    return (jint)result;
}

JNIEXPORT jint JNICALL
Java_io_anyrtc_live_internal_NativeInstance_nativeSetEncoderMirror(JNIEnv *env, jobject obj,jlong nativePtr,jboolean mirror) {
    jint result = -1;
    IArLivePusher* arLivePushKit = reinterpret_cast<IArLivePusher *>(nativePtr);
    if (arLivePushKit!= NULL){
        result = arLivePushKit->setEncoderMirror(mirror);
    }
    return (jint)result;
}

JNIEXPORT jint JNICALL
Java_io_anyrtc_live_internal_NativeInstance_nativeStartMicrophone(JNIEnv *env, jobject obj,jlong nativePtr) {
    jint result = -1;
    IArLivePusher* arLivePushKit = reinterpret_cast<IArLivePusher *>(nativePtr);
    if (arLivePushKit!= NULL){
        result = arLivePushKit->startMicrophone();
    }
    return (jint)result;
}

JNIEXPORT jint JNICALL
Java_io_anyrtc_live_internal_NativeInstance_nativeStopMicrophone(JNIEnv *env, jobject obj,jlong nativePtr) {
    jint result = -1;
    IArLivePusher* arLivePushKit = reinterpret_cast<IArLivePusher *>(nativePtr);
    if (arLivePushKit!= NULL){
        result = arLivePushKit->stopMicrophone();
    }
    return (jint)result;
}

JNIEXPORT jint JNICALL
Java_io_anyrtc_live_internal_NativeInstance_nativeSetBeautyEffect(JNIEnv *env, jobject obj,jboolean enable) {
    return (jint)AndroidDeviceManager::Inst().setBeautyEffect(enable);
}

JNIEXPORT jint JNICALL
Java_io_anyrtc_live_internal_NativeInstance_nativeSetWhitenessLevel(JNIEnv *env, jobject obj,
                                                                    jfloat level) {
    return (jint)AndroidDeviceManager::Inst().setWhitenessLevel(level);
}
JNIEXPORT jint JNICALL
Java_io_anyrtc_live_internal_NativeInstance_nativeSetBeautyLevel(JNIEnv *env, jobject obj,
                                                                 jfloat level) {
    return (jint)AndroidDeviceManager::Inst().setBeautyLevel(level);
}
JNIEXPORT jint JNICALL
Java_io_anyrtc_live_internal_NativeInstance_nativeSetToneLevel(JNIEnv *env, jobject obj,
                                                               jfloat level) {
    return (jint)AndroidDeviceManager::Inst().setToneLevel(level);
}


JNIEXPORT jint JNICALL
Java_io_anyrtc_live_internal_NativeInstance_nativeStartVirtualCamera(JNIEnv *env, jobject thiz,
                                                                     jlong nativePtr,jint type,
                                                                     jbyteArray bitmap, jint width,
                                                                     jint height) {
    jint result = -1;
    IArLivePusher* arLivePushKit = reinterpret_cast<IArLivePusher *>(nativePtr);
    if (arLivePushKit!= NULL){
        jbyte* bufferPtr=(env)->GetByteArrayElements(bitmap, NULL);
        ArLiveImage *image = new ArLiveImage();
        image->imageType = static_cast<ArLiveImageType>(type);
        image->imageWidth = width;
        image->imageHeight = height;
        image->imageSrc = reinterpret_cast<const char *>(bufferPtr);
        result = arLivePushKit->startVirtualCamera(image);
        env->ReleaseByteArrayElements(bitmap, bufferPtr, 0);
    }
    return (jint)result;
}
JNIEXPORT jint JNICALL
Java_io_anyrtc_live_internal_NativeInstance_nativeStopVirtualCamera(JNIEnv *env, jobject thiz, jlong nativePtr) {
    jint result = -1;
    IArLivePusher* arLivePushKit = reinterpret_cast<IArLivePusher *>(nativePtr);
    if (arLivePushKit!= NULL){
        result = arLivePushKit->stopVirtualCamera();
    }
    return (jint)result;
}

JNIEXPORT jint JNICALL
Java_io_anyrtc_live_internal_NativeInstance_nativePausePusherAudio(JNIEnv *env, jobject obj,jlong nativePtr) {
    jint result = -1;
    IArLivePusher* arLivePushKit = reinterpret_cast<IArLivePusher *>(nativePtr);
    if (arLivePushKit!= NULL){
        result = arLivePushKit->pauseAudio();
    }
    return (jint)result;
}
JNIEXPORT jint JNICALL
Java_io_anyrtc_live_internal_NativeInstance_nativeResumePusherAudio(JNIEnv *env, jobject obj,jlong nativePtr) {
    jint result = -1;
    IArLivePusher* arLivePushKit = reinterpret_cast<IArLivePusher *>(nativePtr);
    if (arLivePushKit!= NULL){
        result = arLivePushKit->resumeAudio();
    }
    return (jint)result;
}
JNIEXPORT jint JNICALL
Java_io_anyrtc_live_internal_NativeInstance_nativePausePusherVideo(JNIEnv *env, jobject obj,jlong nativePtr) {
    jint result = -1;
    IArLivePusher* arLivePushKit = reinterpret_cast<IArLivePusher *>(nativePtr);
    if (arLivePushKit!= NULL){
        result = arLivePushKit->pauseVideo();
    }
    return (jint)result;
}
JNIEXPORT jint JNICALL
Java_io_anyrtc_live_internal_NativeInstance_nativeResumePusherVideo(JNIEnv *env, jobject obj,jlong nativePtr) {
    jint result = -1;
    IArLivePusher* arLivePushKit = reinterpret_cast<IArLivePusher *>(nativePtr);
    if (arLivePushKit!= NULL){
        result = arLivePushKit->resumeVideo();
    }
    return (jint)result;
}


JNIEXPORT void JNICALL
Java_io_anyrtc_live_internal_NativeInstance_switchCamera(JNIEnv *env, jobject obj,jboolean isFront) {
        AndroidDeviceManager::Inst().switchCamera(isFront);
}



JNIEXPORT jint JNICALL
Java_io_anyrtc_live_internal_NativeInstance_nativeSetVideoQuality(JNIEnv *env, jobject obj,jlong nativePtr,jint videoResolution,jint videoResolutionMode,jint videoFps,jint videoBitrate,jint minVideoBitrate,jint scaleMode) {
    jint result = -1;
    IArLivePusher* arLivePushKit = reinterpret_cast<IArLivePusher *>(nativePtr);
    if (arLivePushKit!= NULL){
        AR::ArLiveVideoEncoderParam param = anyrtc::ArLiveVideoEncoderParam(ArLiveVideoResolution640x480);
        param.videoResolution = static_cast<ArLiveVideoResolution>(videoResolution);
        param.videoResolutionMode = static_cast<ArLiveVideoResolutionMode>(videoResolutionMode);
        param.videoBitrate = videoBitrate;
        param.videoFps  = videoFps;
        param.minVideoBitrate = minVideoBitrate;
        param.videoScaleMode= static_cast<ArLiveVideoScaleMode>(scaleMode);
        result = arLivePushKit->setVideoQuality(param);
    }
    return (jint)result;
}

JNIEXPORT jint JNICALL
Java_io_anyrtc_live_internal_NativeInstance_nativeSetAudioQuality(JNIEnv *env, jobject obj,jlong nativePtr,jint mode) {
    jint result = -1;
    IArLivePusher* arLivePushKit = reinterpret_cast<IArLivePusher *>(nativePtr);
    if (arLivePushKit!= NULL){
        result = arLivePushKit->setAudioQuality(static_cast<ArLiveAudioQuality>(mode));
    }
    return (jint)result;
}

JNIEXPORT jint JNICALL
Java_io_anyrtc_live_internal_NativeInstance_nativePusherEnableVolumeEvaluation(JNIEnv *env,
                                                                               jobject thiz,jlong nativePtr,
                                                                               jint interval_ms) {
    IArLivePusher* arLivePushKit = reinterpret_cast<IArLivePusher *>(nativePtr);
    jint result = -1;
    if (arLivePushKit!= NULL){
        result = arLivePushKit->enableVolumeEvaluation(interval_ms);
    }
    return (jint)result;
}

JNIEXPORT jint JNICALL
Java_io_anyrtc_live_internal_NativeInstance_nativeSendSeiMessage(JNIEnv *env, jobject thiz,jlong nativePtr,
                                                                 jint var1, jbyteArray var2) {
    IArLivePusher* arLivePushKit = reinterpret_cast<IArLivePusher *>(nativePtr);
    jint result = -1;
    if (arLivePushKit!= NULL){
        jbyte* bufferPtr=(env)->GetByteArrayElements(var2, NULL);
        jint size = env->GetArrayLength(var2);
        result = arLivePushKit->sendSeiMessage(var1, reinterpret_cast<const uint8_t *>(bufferPtr), size);
        env->ReleaseByteArrayElements(var2, bufferPtr, 0);
    }
    return (jint)result;

}

JNIEXPORT jint JNICALL
Java_io_anyrtc_live_internal_NativeInstance_nativeEnableCustomVideoCapture(JNIEnv *env,
                                                                           jobject thiz,
                                                                           jlong nativePtr,
                                                                           jboolean var1) {
    IArLivePusher* arLivePushKit = reinterpret_cast<IArLivePusher *>(nativePtr);
    jint result = -1;
    if (arLivePushKit!= NULL){
        result = arLivePushKit->enableCustomVideoCapture(var1);
    }
    return (jint)result;
}

JNIEXPORT jint JNICALL
Java_io_anyrtc_live_internal_NativeInstance_nativeSendCustomVideoFrame(JNIEnv *env, jobject thiz,
                                                                       jlong nativePtr,
                                                                       jint pixel_format,
                                                                       jint buffer_type,
                                                                       jbyteArray data,
                                                                       jobject buffer, jint width,
                                                                       jint height, jint rotation,jint stride) {
    IArLivePusher* arLivePushKit = reinterpret_cast<IArLivePusher *>(nativePtr);
    jint result = -1;
    if (arLivePushKit!= NULL){
        ArLiveVideoFrame *frame = new ArLiveVideoFrame();
        frame->pixelFormat = static_cast<ArLivePixelFormat>(pixel_format);
        frame->bufferType = static_cast<ArLiveBufferType>(buffer_type);
        frame->width = width;
        frame->height = height;
        frame->rotation = static_cast<ArLiveRotation>(rotation);
        jbyte* arrayData=(env)->GetByteArrayElements(data, NULL);
        frame->data = reinterpret_cast<char *>(arrayData);
        frame->stride = stride;
        result = arLivePushKit->sendCustomVideoFrame(frame);
        env->ReleaseByteArrayElements(data, arrayData, 0);
    }
    return (jint)result;
}


JNIEXPORT jint JNICALL
Java_io_anyrtc_live_internal_NativeInstance_nativeEnableCustomAudioCapture(JNIEnv *env,
                                                                           jobject thiz,jlong nativePtr,
                                                                           jboolean enable) {
    IArLivePusher* arLivePushKit = reinterpret_cast<IArLivePusher *>(nativePtr);
    jint result  = -1;
    if (arLivePushKit!= NULL){
        result = arLivePushKit->enableCustomAudioCapture(enable);
    }
    return (jint)result;
}

JNIEXPORT jint JNICALL
Java_io_anyrtc_live_internal_NativeInstance_nativeSendCustomAudioFrame(JNIEnv *env, jobject thiz,
                                                                       jlong nativePtr,
                                                                       jint channel,
                                                                       jint sample_rate,
                                                                       jbyteArray data) {
    IArLivePusher* arLivePushKit = reinterpret_cast<IArLivePusher *>(nativePtr);
    jint result  = -1;
    if (arLivePushKit!= NULL){
        jbyte* bufferPtr=(env)->GetByteArrayElements(data, NULL);
        jint size = env->GetArrayLength(data);
        ArLiveAudioFrame *audioFrame = new ArLiveAudioFrame();
        audioFrame->data = reinterpret_cast<char *>(bufferPtr);
        audioFrame->channel = channel;
        audioFrame->sampleRate = sample_rate;
        audioFrame->length = size;
        result = arLivePushKit->sendCustomAudioFrame(audioFrame);
        env->ReleaseByteArrayElements(data, bufferPtr, 0);
    }
    return (jint)result;
}




JNIEXPORT void JNICALL
Java_io_anyrtc_live_internal_NativeInstance_startScreenCapture(JNIEnv *env, jobject obj,jlong nativePtr) {
    IArLivePusher* arLivePushKit = reinterpret_cast<IArLivePusher *>(nativePtr);
    if (arLivePushKit!=NULL){
        arLivePushKit->startScreenCapture();
    }
}

JNIEXPORT void JNICALL
Java_io_anyrtc_live_internal_NativeInstance_stopScreenCapture(JNIEnv *env, jobject obj,jlong nativePtr) {
    IArLivePusher* arLivePushKit = reinterpret_cast<IArLivePusher *>(nativePtr);
    if (arLivePushKit!=NULL){
        arLivePushKit->stopScreenCapture();
    }
}






}


extern "C" {

JNIEXPORT jint JNICALL
Java_io_anyrtc_live_internal_NativeInstance_nativeStartPlay(JNIEnv *env, jobject thiz,
                                                            jlong native_ptr, jstring url) {

    int result = -1;
    IArLivePlayer* arLivePlayKit = reinterpret_cast<IArLivePlayer *>(native_ptr);
    if (arLivePlayKit !=NULL){
        std::string strUrl = webrtc::JavaToStdString(env, url);
        result = arLivePlayKit->startPlay(strUrl.c_str());
    }
    return (jint)result;
}


JNIEXPORT jint JNICALL
Java_io_anyrtc_live_internal_NativeInstance_nativeStopPlay(JNIEnv *env, jobject thiz,
                                                            jlong nativeHandle) {
    jint result = -1;
    IArLivePlayer* arLivePlayKit = reinterpret_cast<IArLivePlayer *>(nativeHandle);
    if (arLivePlayKit != NULL){
        result = arLivePlayKit->stopPlay();
    }
    return (jint)result;
}

JNIEXPORT jint JNICALL
Java_io_anyrtc_live_internal_NativeInstance_nativeIsPlaying(JNIEnv *env, jobject thiz,jlong nativeHandle
                                                          ) {
    jint result = -1;
    IArLivePlayer* arLivePlayKit = reinterpret_cast<IArLivePlayer *>(nativeHandle);
    if (arLivePlayKit != NULL){
        result = arLivePlayKit->isPlaying();
    }
    return (jint)result;
}
JNIEXPORT jint JNICALL
Java_io_anyrtc_live_internal_NativeInstance_nativeSetRenderRotation(JNIEnv *env, jobject thiz,jlong nativeHandle,jint rotation
) {
    jint result = -1;
    IArLivePlayer* arLivePlayKit = reinterpret_cast<IArLivePlayer *>(nativeHandle);
    if (arLivePlayKit != NULL){
        result = arLivePlayKit->setRenderRotation(static_cast<ArLiveRotation>(rotation));
    }
    return (jint)result;
}


JNIEXPORT jint JNICALL
Java_io_anyrtc_live_internal_NativeInstance_nativePauseAudio(JNIEnv *env, jobject thiz,jlong nativeHandle
) {
    jint result = -1;
    IArLivePlayer* arLivePlayKit = reinterpret_cast<IArLivePlayer *>(nativeHandle);
    if (arLivePlayKit != NULL){
        result = arLivePlayKit->pauseAudio();
    }
    return (jint)result;
}

JNIEXPORT jint JNICALL
Java_io_anyrtc_live_internal_NativeInstance_nativeResumeAudio(JNIEnv *env, jobject thiz,jlong nativeHandle
) {
    jint result = -1;
    IArLivePlayer* arLivePlayKit = reinterpret_cast<IArLivePlayer *>(nativeHandle);
    if (arLivePlayKit != NULL){
        result = arLivePlayKit->resumeAudio();
    }
    return (jint)result;
}

JNIEXPORT jint JNICALL
Java_io_anyrtc_live_internal_NativeInstance_nativeResumeVideo(JNIEnv *env, jobject thiz,jlong nativeHandle
) {
    jint result = -1;
    IArLivePlayer* arLivePlayKit = reinterpret_cast<IArLivePlayer *>(nativeHandle);
    if (arLivePlayKit != NULL){
        result = arLivePlayKit->resumeVideo();
    }
    return (jint)result;
}



JNIEXPORT jint JNICALL
Java_io_anyrtc_live_internal_NativeInstance_nativePauseVideo(JNIEnv *env, jobject thiz,jlong nativeHandle
) {
    jint result = -1;
    IArLivePlayer* arLivePlayKit = reinterpret_cast<IArLivePlayer *>(nativeHandle);
    if (arLivePlayKit != NULL){
        result = arLivePlayKit->pauseVideo();
    }
    return (jint)result;
}


JNIEXPORT jint JNICALL
Java_io_anyrtc_live_internal_NativeInstance_nativeSetPlayoutVolume(JNIEnv *env, jobject thiz,jlong nativeHandle,jint vom
) {
    jint result = -1;
    IArLivePlayer* arLivePlayKit = reinterpret_cast<IArLivePlayer *>(nativeHandle);
    if (arLivePlayKit != NULL){
        result = arLivePlayKit->setPlayoutVolume(vom);
    }
    return (jint)result;
}

JNIEXPORT jint JNICALL
Java_io_anyrtc_live_internal_NativeInstance_nativeSetCacheParams(JNIEnv *env, jobject thiz,jlong nativeHandle,jfloat minTime,jfloat maxTime
) {
    jint result = -1;
    IArLivePlayer* arLivePlayKit = reinterpret_cast<IArLivePlayer *>(nativeHandle);
    if (arLivePlayKit != NULL){
        result = arLivePlayKit->setCacheParams(minTime,maxTime);
    }
    return (jint)result;
}

JNIEXPORT jint JNICALL
Java_io_anyrtc_live_internal_NativeInstance_nativeEnableVolumeEvaluation(JNIEnv *env, jobject thiz,jlong nativeHandle,jint intervalMs
) {
    jint result = -1;
    IArLivePlayer* arLivePlayKit = reinterpret_cast<IArLivePlayer *>(nativeHandle);
    if (arLivePlayKit != NULL){
        result = arLivePlayKit->enableVolumeEvaluation(intervalMs);
    }
    return (jint)result;
}


JNIEXPORT jint JNICALL
Java_io_anyrtc_live_internal_NativeInstance_nativeEnableCustomRendering(JNIEnv *env, jobject thiz,jlong nativeHandle,jboolean enable,jint format,jint type
) {
    jint result = -1;
    IArLivePlayer* arLivePlayKit = reinterpret_cast<IArLivePlayer *>(nativeHandle);
    if (arLivePlayKit != NULL){
        result = arLivePlayKit->enableCustomRendering(enable,(ArLivePixelFormat)format,(ArLiveBufferType)type);
    }
    return (jint)result;
}

JNIEXPORT jint JNICALL
Java_io_anyrtc_live_internal_NativeInstance_nativeEnableReceiveSeiMessage(JNIEnv *env, jobject thiz,jlong nativeHandle,jboolean enable,jint payloadType
) {
    jint result = -1;
    IArLivePlayer* arLivePlayKit = reinterpret_cast<IArLivePlayer *>(nativeHandle);
    if (arLivePlayKit != NULL){
        result = arLivePlayKit->enableReceiveSeiMessage(enable,payloadType);
    }
    return (jint)result;
}

JNIEXPORT void JNICALL
Java_io_anyrtc_live_internal_NativeInstance_nativeShowDebugView(JNIEnv *env, jobject thiz,jlong nativeHandle,jboolean isShow
) {
    IArLivePlayer* arLivePlayKit = reinterpret_cast<IArLivePlayer *>(nativeHandle);
    if (arLivePlayKit != NULL){
        arLivePlayKit->showDebugView(isShow);
    }
}

JNIEXPORT void JNICALL
Java_io_anyrtc_live_internal_NativeInstance_nativePlayKitRelease(JNIEnv *env, jobject thiz,jlong nativeHandle
) {
    InstanceHolder *instance = getInstanceHolder(env, thiz);
    IArLivePlayer* arLivePlayKit = reinterpret_cast<IArLivePlayer *>(nativeHandle);
    if (instance->arLiveEngine != NULL){
       instance->arLiveEngine->releaseArLivePlayer(arLivePlayKit);
    }
}








JNIEXPORT jlong JNICALL
Java_io_anyrtc_live_internal_NativeInstance_nativeCreatePlayKit(JNIEnv *env, jobject obj) {
    InstanceHolder *instance = getInstanceHolder(env, obj);
    if (instance->arLiveEngine == nullptr){
        return (jint)-1;
    }
    IArLivePlayer* player = instance->arLiveEngine->createArLivePlayer(NULL);
   return reinterpret_cast<jlong>(player);
}

JNIEXPORT void JNICALL
Java_io_anyrtc_live_internal_NativeInstance_nativeSetPlayerobserver(JNIEnv *env, jobject thiz,jlong nativeHandle,jobject observer
) {
    IArLivePlayer* arLivePlayKit = reinterpret_cast<IArLivePlayer *>(nativeHandle);
    if (arLivePlayKit != NULL){
        LivePlayEvent *event = new LivePlayEvent(observer);
        arLivePlayKit->setObserver(event);
    }
}




JNIEXPORT jint JNICALL
Java_io_anyrtc_live_internal_NativeInstance_nativeSetPlayerView(JNIEnv *env, jobject obj,jlong nativeHandle,jobject videoSink) {

    IArLivePlayer* arLivePlayKit = reinterpret_cast<IArLivePlayer *>(nativeHandle);
    jint result = -1;
    if (arLivePlayKit!= NULL){
        result = arLivePlayKit->setRenderView(reinterpret_cast<view_t *>(videoSink));
    }
    return (jint)result;
}

}


extern "C" {
JNIEXPORT void JNICALL
Java_org_webrtc_effector_format_LibYuvBridge_i420ToAbgrInternal(
        JNIEnv *env,
        jobject obj,
        jobject dataYBuffer, jint strideY,
        jobject dataUBuffer, jint strideU,
        jobject dataVBuffer, jint strideV,
        jint width, jint height,
        jobject outRgbaBuffer)
{
    uint8_t *data_y = (uint8_t*) env->GetDirectBufferAddress(dataYBuffer);
    uint8_t *data_u = (uint8_t*) env->GetDirectBufferAddress(dataUBuffer);
    uint8_t *data_v = (uint8_t*) env->GetDirectBufferAddress(dataVBuffer);
    uint8_t *out_rgba = (uint8_t *)(env->GetDirectBufferAddress(outRgbaBuffer));

    int stride_y = strideY;
    int stride_u = strideU;
    int stride_v = strideV;

    int dst_stride_rgba = width * 4;
    int src_width = width;
    int src_height = height;

    /*
    LIBYUV_API
    int I420ToABGR(const uint8_t* src_y,
                   int src_stride_y,
                   const uint8_t* src_u,
                   int src_stride_u,
                   const uint8_t* src_v,
                   int src_stride_v,
                   uint8_t* dst_abgr,
                   int dst_stride_abgr,
                   int width,
                   int height);
    */
    libyuv::I420ToABGR(data_y, stride_y,
               data_u, stride_u,
               data_v, stride_v,
               out_rgba, dst_stride_rgba,
               src_width, src_height);
}

JNIEXPORT void JNICALL
Java_org_webrtc_effector_format_LibYuvBridge_abgrToI420Internal(
        JNIEnv *env,
        jobject obj,
        jobject rgbaBuffer,
        jint width,
        jint height,
        jobject outDataYBuffer,
        jint strideY,
        jobject outDataUBuffer,
        jint strideU,
        jobject outDataVBuffer,
        jint strideV)
{
    uint8_t *rgba = (uint8_t*) env->GetDirectBufferAddress(rgbaBuffer);
    uint8_t *out_data_y = (uint8_t*) env->GetDirectBufferAddress(outDataYBuffer);
    uint8_t *out_data_u = (uint8_t*) env->GetDirectBufferAddress(outDataUBuffer);
    uint8_t *out_data_v = (uint8_t*) env->GetDirectBufferAddress(outDataVBuffer);

    /*
    // ABGR little endian (rgba in memory) to I420.
    LIBYUV_API
    int ABGRToI420(const uint8* src_frame, int src_stride_frame,
                   uint8* dst_y, int dst_stride_y,
                   uint8* dst_u, int dst_stride_u,
                   uint8* dst_v, int dst_stride_v,
                   int width, int height);
    */
    libyuv::ABGRToI420(rgba, width * 4,
               out_data_y, strideY,
               out_data_u, strideU,
               out_data_v, strideV,
               width, height);
}



}


//device manager
extern "C"{
JNIEXPORT jboolean JNICALL
Java_io_anyrtc_live_internal_NativeInstance_enableCameraTorch(JNIEnv *env, jobject thiz,
                                                              jboolean enable) {
    return AndroidDeviceManager::Inst().enableCameraTorch(enable);
}

JNIEXPORT jfloat JNICALL
Java_io_anyrtc_live_internal_NativeInstance_getCameraZoomMaxRatio(JNIEnv *env, jobject thiz) {
    return  AndroidDeviceManager::Inst().getCameraZoomMaxRatio();
}

JNIEXPORT jint JNICALL
Java_io_anyrtc_live_internal_NativeInstance_setCameraZoomRatio(JNIEnv *env, jobject thiz,
                                                               jfloat var1) {
    return  AndroidDeviceManager::Inst().setCameraZoomRatio(var1);
}

JNIEXPORT jboolean JNICALL
Java_io_anyrtc_live_internal_NativeInstance_isAutoFocusEnabled(JNIEnv *env, jobject thiz) {
    return AndroidDeviceManager::Inst().isAutoFocusEnabled();
}

JNIEXPORT jint JNICALL
Java_io_anyrtc_live_internal_NativeInstance_enableCameraAutoFocus(JNIEnv *env, jobject thiz,
                                                                  jboolean var1) {
    return AndroidDeviceManager::Inst().enableCameraAutoFocus(var1);
}

JNIEXPORT void JNICALL
Java_io_anyrtc_live_internal_NativeInstance_setCameraCapturerParam(JNIEnv *env, jobject thiz,
                                                                   jint mode, jint width,
                                                                   jint height) {
    AndroidDeviceManager::Inst().setCameraCapturerParam(mode,width,height);
}
JNIEXPORT jint JNICALL
Java_io_anyrtc_live_internal_NativeInstance_setCameraFocusPosition(JNIEnv *env, jobject thiz,
                                                                   jfloat var1, jfloat var2) {
    return AndroidDeviceManager::Inst().setCameraFocusPosition(var1,var2);
}
}














extern "C"
JNIEXPORT void JNICALL
Java_org_webrtc_Camera1Session_recoverCamera(JNIEnv *env, jobject thiz) {
    AndroidDeviceManager::Inst().recoverCamera();
}
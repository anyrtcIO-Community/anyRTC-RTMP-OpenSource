//
// Created by liu on 2021/11/4.
//

#include <StaticThreads.h>
#include <pc/video_track_source_proxy.h>
#include <AndroidContext.h>
#include <android/native_api/codecs/wrapper.h>
#include "AndroidDeviceManager.h"
#include "ArLiveCode.hpp"


using namespace arlive;
using namespace anyrtc;
static AndroidDeviceManager *gInstance = NULL;

AndroidDeviceManager&  AndroidDeviceManager::Inst(){
    if (gInstance == NULL){
        gInstance = new AndroidDeviceManager();
    }
    return *gInstance;
}

AndroidDeviceManager::AndroidDeviceManager(void):
        videoSource(NULL),
        isScreenCapture(false),
        _platformContext(NULL)
{

}

AndroidDeviceManager::~AndroidDeviceManager(void){
   // gInstance = NULL;
}

rtc::scoped_refptr<webrtc::VideoTrackSourceInterface> AndroidDeviceManager::createVideoSource(){
    JNIEnv *env = webrtc::AttachCurrentThreadIfNeeded();
    videoSource = webrtc::CreateJavaVideoSource(env, arlive::StaticThreads::getThreads()->getMediaThread(), false,
                                                    false);
    return webrtc::VideoTrackSourceProxy::Create(arlive::StaticThreads::getThreads()->getMediaThread(), arlive::StaticThreads::getThreads()->getWorkerThread(),
                                                 videoSource);
}

void* AndroidDeviceManager::createVideoCapture(size_t width, size_t height, size_t fps, size_t capture_device_index){
    if (capture_device_index==8){
        _capturer = std::make_unique<VideoCameraCapturer>(videoSource, "screen",width,height,fps, _platformContext);
        isScreenCapture = true;
    } else{
        _capturer = std::make_unique<VideoCameraCapturer>(videoSource, capture_device_index==0 ? "front" : "back",width,height,fps, _platformContext);
        isScreenCapture = false;
    }
    captureFps = fps;
    captureWidth=width;
    captureheight = height;
    return this;
}


std::unique_ptr<webrtc::VideoEncoderFactory> AndroidDeviceManager::makeVideoEncoderFactory() {
JNIEnv *env = webrtc::AttachCurrentThreadIfNeeded();
AndroidContext *context = (AndroidContext *) _platformContext.get();
jmethodID methodId = env->GetMethodID(context->getJavaCapturerClass(),
                                      "getSharedEGLContext",
                                      "()Lorg/webrtc/EglBase$Context;");
jobject eglContext = env->CallObjectMethod(context->getJavaCapturer(), methodId);

webrtc::ScopedJavaLocalRef<jclass> factory_class = webrtc::GetClass(env,
                                                                    "org/webrtc/DefaultVideoEncoderFactory");
jmethodID factory_constructor = env->GetMethodID(factory_class.obj(), "<init>",
                                                 "(Lorg/webrtc/EglBase$Context;ZZ)V");
webrtc::ScopedJavaLocalRef<jobject> factory_object(env, env->NewObject(factory_class.obj(),
                                                                       factory_constructor,
                                                                       eglContext, false,
                                                                       true));
return webrtc::JavaToNativeVideoEncoderFactory(env, factory_object.obj());
}

std::unique_ptr<webrtc::VideoDecoderFactory> AndroidDeviceManager::makeVideoDecoderFactory() {
    JNIEnv *env = webrtc::AttachCurrentThreadIfNeeded();
    AndroidContext *context = (AndroidContext *) _platformContext.get();
    jmethodID methodId = env->GetMethodID(context->getJavaCapturerClass(),
                                          "getSharedEGLContext",
                                          "()Lorg/webrtc/EglBase$Context;");
    jobject eglContext = env->CallObjectMethod(context->getJavaCapturer(), methodId);

    webrtc::ScopedJavaLocalRef<jclass> factory_class = webrtc::GetClass(env,
                                                                        "org/webrtc/DefaultVideoDecoderFactory");
    jmethodID factory_constructor = env->GetMethodID(factory_class.obj(), "<init>",
                                                     "(Lorg/webrtc/EglBase$Context;)V");
    webrtc::ScopedJavaLocalRef<jobject> factory_object(env, env->NewObject(factory_class.obj(),
                                                                           factory_constructor,
                                                                           eglContext));
    return webrtc::JavaToNativeVideoDecoderFactory(env, factory_object.obj());
}


void  AndroidDeviceManager::setPlatformContext(std::shared_ptr<arlive::PlatformContext> context){
    _platformContext = context;
}

webrtc::ScopedJavaLocalRef<jobject> AndroidDeviceManager::GetJavaVideoCapturerObserver(JNIEnv *env) {
    return videoSource->GetJavaVideoCapturerObserver(env);
}


jint AndroidDeviceManager::switchCamera(bool isFront){
    if (isScreenCapture)
        return ArLIVE_ERROR_REFUSED;
    if (videoSource) {
        //this should outlive the capturer
        _capturer = nullptr;
        _capturer=std::make_unique<VideoCameraCapturer>(videoSource, isFront?"front":"back",captureWidth,captureheight,captureFps, _platformContext);
    }
    if (_capturer!= nullptr){
        _capturer->setState(_state);
        return ArLiveCode::ArLIVE_OK;
    }
    return ArLiveCode::ArLIVE_ERROR_REFUSED;
}

jboolean AndroidDeviceManager::isFrontCamera(){
    return JNI_FALSE;
}
jfloat AndroidDeviceManager::getCameraZoomMaxRatio(){
    if (_capturer!=NULL){
        return _capturer->getCameraZoomMaxRatio();
    }
    return (jfloat)0;
}
jint AndroidDeviceManager::setCameraZoomRatio(float zoomRatio){
    if (_capturer!=NULL){
        return _capturer->setCameraZoomRatio(zoomRatio);
    }
    return (jint)-1;
}

void AndroidDeviceManager::setCameraCapturerParam(int mode,int width,int height){
    if (_capturer!=NULL){
        captureWidth=width;
        captureheight = height;
        _capturer->changeCaptureFormat(captureWidth,captureheight,captureFps);
    }
}

jboolean AndroidDeviceManager::isAutoFocusEnabled(){
    if (_capturer!=NULL){
        return _capturer->isAutoFocusEnabled();
    }
    return JNI_FALSE;
}
jint AndroidDeviceManager::enableCameraAutoFocus(bool enabled){
    if (_capturer!=NULL){
        int result = _capturer->enableCameraAutoFocus(enabled);
        return (jint)result;
    }
    return (jint)-1;
}

jint AndroidDeviceManager::setCameraFocusPosition(float x, float y){
    if (_capturer!=NULL){
        int result = _capturer->setCameraFocusPosition(x,y);
        return (jint)result;
    }
    return (jint)-1;
}
jboolean AndroidDeviceManager::enableCameraTorch(bool enabled){
    if (_capturer!=NULL){
        int result = _capturer->enableCameraTorch(enabled);
        return result==0;
    }
    return JNI_FALSE;
}





bool AndroidDeviceManager::startCapture(){
    if (_capturer == NULL){
        return false;
    }
    _capturer->setState(arlive::VideoState::Active);
    _state = arlive::VideoState::Active;
    return true;
}

bool AndroidDeviceManager::stopCapture(){
    if (_capturer == NULL){
        return false;
    }
    _capturer->setState(arlive::VideoState::Inactive);
    _state = arlive::VideoState::Inactive;
    return true;
}
void AndroidDeviceManager::destoryCapture(){
    if (_capturer!=nullptr){
        _capturer.reset();
        _capturer = nullptr;
        _platformContext.reset();
        _platformContext= nullptr;
    }
}
int AndroidDeviceManager::setBeautyEffect(bool enable){

    JNIEnv *env = webrtc::AttachCurrentThreadIfNeeded();
    auto context = (AndroidContext *) _platformContext.get();
    jmethodID methodId = env->GetMethodID(context->getJavaCapturerClass(), "needBeautyEffect", "(Z)V");
    env->CallVoidMethod(context->getJavaCapturer(), methodId, (jboolean) enable);
    return anyrtc::ArLiveCode::ArLIVE_OK;
}
int AndroidDeviceManager::setWhitenessLevel(float level){
    JNIEnv *env = webrtc::AttachCurrentThreadIfNeeded();
    auto context = (AndroidContext *) _platformContext.get();
    jmethodID methodId = env->GetMethodID(context->getJavaCapturerClass(), "setWhitenessLevel", "(F)V");
    env->CallVoidMethod(context->getJavaCapturer(), methodId, (jfloat) level);
    return anyrtc::ArLiveCode::ArLIVE_ERROR_REFUSED;
}
int AndroidDeviceManager::setBeautyLevel(float level){
    JNIEnv *env = webrtc::AttachCurrentThreadIfNeeded();
    auto context = (AndroidContext *) _platformContext.get();
    jmethodID methodId = env->GetMethodID(context->getJavaCapturerClass(), "setBeautyLevel", "(F)V");
    env->CallVoidMethod(context->getJavaCapturer(), methodId, (jfloat) level);
    return anyrtc::ArLiveCode::ArLIVE_OK;
}

int AndroidDeviceManager::setToneLevel(float level){
    JNIEnv *env = webrtc::AttachCurrentThreadIfNeeded();
    auto context = (AndroidContext *) _platformContext.get();
    jmethodID methodId = env->GetMethodID(context->getJavaCapturerClass(), "setToneLevel", "(F)V");
    env->CallVoidMethod(context->getJavaCapturer(), methodId, (jfloat) level);
    return anyrtc::ArLiveCode::ArLIVE_OK;
}

void AndroidDeviceManager::recoverCamera(){
    JNIEnv *env = webrtc::AttachCurrentThreadIfNeeded();
    auto context = (AndroidContext *) _platformContext.get();
    jmethodID methodId = env->GetMethodID(context->getJavaCapturerClass(), "recoverCamera", "()V");
    env->CallVoidMethod(context->getJavaCapturer(), methodId);
}




extern "C" {
JNIEXPORT jobject Java_io_anyrtc_live_internal_VideoCapturerDevice_nativeGetJavaVideoCapturerObserver(JNIEnv *env, jclass clazz, jlong ptr) {
    return AndroidDeviceManager().Inst().GetJavaVideoCapturerObserver(env).Release();
}
}
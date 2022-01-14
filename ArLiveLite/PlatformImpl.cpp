#include "PlatformImpl.h"
#ifdef WEBRTC_WIN
#include "WinVideoTrackSource.h"
#elif defined(WEBRTC_ANDROID)
#include "jni.h"
#include <modules/utility/include/jvm_android.h>
#include "webrtc/sdk/android/src/jni/jvm.h"
#include "sdk/android/native_api/video/video_source.h"
#include "StaticThreads.h"
#include <android/AndroidContext.h>
#include <pc/video_track_source_proxy.h>
#include "VideoCaptureInterface.h"
#include "VideoCapturerInterface.h"
#include "StaticThreads.h"
rtc::scoped_refptr<webrtc::JavaVideoTrackSourceInterface> javaVideoSource;
#elif defined(WEBRTC_IOS)
#import "ObjcVCMCapturer.h"
#include "sdk/objc/native/src/objc_video_track_source.h"
#endif
#include "VcmCapturer.h"
#include "rtc_base/ref_counted_object.h"

rtc::scoped_refptr< webrtc::VideoTrackSourceInterface> createPlatformVideoSouce() {
#ifdef WEBRTC_WIN
	return rtc::make_ref_counted<WinVideoTrackSource>();
#elif defined(WEBRTC_ANDROID)
	JNIEnv *env = webrtc::AttachCurrentThreadIfNeeded();
	rtc::scoped_refptr<webrtc::VideoTrackSourceInterface> ;
	javaVideoSource = webrtc::CreateJavaVideoSource(env, arlive::StaticThreads::getThreads()->getMediaThread(), false,
																										   false);
	return webrtc::VideoTrackSourceProxy::Create(arlive::StaticThreads::getThreads()->getMediaThread(), arlive::StaticThreads::getThreads()->getWorkerThread(),
												 javaVideoSource);
#elif defined(WEBRTC_IOS)
    return rtc::make_ref_counted<webrtc::ObjCVideoTrackSource>();
#endif
	return NULL;
}

#ifdef WEBRTC_ANDROID
extern "C" {

JNIEXPORT jobject Java_io_anyrtc_live_internal_VideoCapturerDevice_nativeGetJavaVideoCapturerObserver(JNIEnv *env, jclass clazz, jlong ptr) {
	JNIEnv *env1 = webrtc::AttachCurrentThreadIfNeeded();
	return javaVideoSource->GetJavaVideoCapturerObserver(env1).Release();
}
}
#endif


void* createPlatformVideoCapture(const rtc::scoped_refptr<webrtc::VideoTrackSourceInterface>vidSource, size_t width, size_t height, size_t fps, size_t capture_device_index)
{
#if (defined(WEBRTC_WIN) || defined(WEBRTC_MAC)) && !defined(WEBRTC_IOS)
	VcmCapturer* vCap = VcmCapturer::Create(width, height, fps, capture_device_index);
	vCap->SetVideoSource(vidSource);
	return vCap;
#elif (defined(WEBRTC_ANDROID))
	using namespace arlive;
	JNIEnv *env = webrtc::AttachCurrentThreadIfNeeded();
	std::unique_ptr<VideoCaptureInterface> capture;
	capture = VideoCaptureInterface::Create(StaticThreads::getThreads(),
													std::make_shared<AndroidContext>(env, nullptr,
																					 capture_device_index==8));
	capture->setCaptureOptions(width,height,fps);
	if (capture_device_index==8){
		capture->setVideoSource(vidSource,"screen", true);
	} else{
		capture->setVideoSource(vidSource,capture_device_index==0 ? "front" : "back", false);
	}
	return capture.release();
#elif defined(WEBRTC_IOS)
    ObjcVCMCapturer *objcCapturer = new ObjcVCMCapturer(width, height, fps, capture_device_index);
    objcCapturer->SetVideoSource(vidSource);
    return objcCapturer;
#endif
}
bool startPlatformVideoCapture(void*ptrCap)
{
	if (ptrCap == NULL)
		return false;
#if (defined(WEBRTC_WIN) || defined(WEBRTC_MAC)) && !defined(WEBRTC_IOS)
	VcmCapturer* vCap = (VcmCapturer*)ptrCap;
	return vCap->Start();
#elif (defined(WEBRTC_ANDROID))
	auto capturer = reinterpret_cast<arlive::VideoCaptureInterface *>(ptrCap);
	capturer->setState(arlive::VideoState::Active);
	return true;
#elif defined(WEBRTC_IOS)
    ObjcVCMCapturer *capturer = (ObjcVCMCapturer *)ptrCap;
    capturer->StartCapture();
    return true;
#endif

}
bool stopPlatformVideoCapture(void*ptrCap)
{
	if (ptrCap == NULL)
		return false;
#if (defined(WEBRTC_WIN) || defined(WEBRTC_MAC)) && !defined(WEBRTC_IOS)
	VcmCapturer* vCap = (VcmCapturer*)ptrCap;
	return vCap->Stop();
#elif (defined(WEBRTC_ANDROID))
	auto capturer = reinterpret_cast<arlive::VideoCaptureInterface *>(ptrCap);
	capturer->setState(arlive::VideoState::Inactive);
	return true;
#elif defined(WEBRTC_IOS)
    ObjcVCMCapturer *capturer = (ObjcVCMCapturer *)ptrCap;
    capturer->StopCapture();
#endif
    return true;
}
void destoryPlatformVideoCapture(void*ptrCap)
{
	if (ptrCap == NULL)
		return;
#if (defined(WEBRTC_WIN) || defined(WEBRTC_MAC)) && !defined(WEBRTC_IOS)
	VcmCapturer* vCap = (VcmCapturer*)ptrCap;
	vCap->Stop();
	vCap->SetVideoSource(nullptr);
	delete vCap;
	vCap = NULL;
#elif (defined(WEBRTC_ANDROID))
	auto capturer = reinterpret_cast<arlive::VideoCaptureInterface *>(ptrCap);
	delete capturer;
#elif defined(WEBRTC_IOS)
    ObjcVCMCapturer *capturer = (ObjcVCMCapturer *)ptrCap;
    delete capturer;
#endif
}

#if TARGET_PLATFORM_PHONE
void platformVideoCaptureSetBeautyEffect(void*ptrCap, bool enable)
{
    if (ptrCap == NULL)
        return;
#if (defined(WEBRTC_ANDROID))
	auto capturer = reinterpret_cast<arlive::VideoCaptureInterface *>(ptrCap);
	capturer->setBeautyEffect(enable);
#else
    ObjcVCMCapturer *capturer = (ObjcVCMCapturer *)ptrCap;
    capturer->SetBeautyEffect(enable);
#endif
}

void switchPlatformVideoCapture(void*ptrCap, bool isFront)
{
	if (ptrCap == NULL)
		return;
#if (defined(WEBRTC_ANDROID))
	auto capturer = reinterpret_cast<arlive::VideoCaptureInterface *>(ptrCap);
	capturer->switchToDevice(isFront ? "front" : "back", false);
#else
    ObjcVCMCapturer *capturer = (ObjcVCMCapturer *)ptrCap;
    capturer->SwitchCapture();
#endif
}
#endif

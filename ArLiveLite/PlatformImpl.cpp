#include "PlatformImpl.h"
#ifdef WEBRTC_WIN
#include "WinVideoTrackSource.h"
#elif defined(WEBRTC_ANDROID)
#include "jni/liveEngine/AndroidDeviceManager.h"
#elif defined(WEBRTC_IOS)
#import "ObjcVCMCapturer.h"
#include "sdk/objc/native/src/objc_video_track_source.h"
#endif
#include "VcmCapturer.h"
#include "rtc_base/ref_counted_object.h"

rtc::scoped_refptr<webrtc::VideoTrackSourceInterface> createPlatformVideoSouce() {
#ifdef WEBRTC_WIN
	return rtc::make_ref_counted<WinVideoTrackSource>();
#elif defined(WEBRTC_ANDROID)
    return AndroidDeviceManager::Inst().createVideoSource();
#elif defined(WEBRTC_IOS)
    return rtc::make_ref_counted<webrtc::ObjCVideoTrackSource>();
#endif
	return NULL;
}


void* createPlatformVideoCapture(const rtc::scoped_refptr<webrtc::VideoTrackSourceInterface>vidSource, size_t width, size_t height, size_t fps, size_t capture_device_index)
{
#if (defined(WEBRTC_WIN) || defined(WEBRTC_MAC)) && !defined(WEBRTC_IOS)
	VcmCapturer* vCap = VcmCapturer::Create(width, height, fps, capture_device_index);
	vCap->SetVideoSource(vidSource);
	return vCap;
#elif (defined(WEBRTC_ANDROID))
    return AndroidDeviceManager::Inst().createVideoCapture(width, height, fps, capture_device_index);
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
	return AndroidDeviceManager::Inst().startCapture();
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
	return AndroidDeviceManager::Inst().stopCapture();
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
	AndroidDeviceManager::Inst().destoryCapture();
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
	AndroidDeviceManager::Inst().setBeautyEffect(enable);
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
	AndroidDeviceManager::Inst().switchCamera(isFront);
#else
    ObjcVCMCapturer *capturer = (ObjcVCMCapturer *)ptrCap;
    capturer->SwitchCapture();
#endif
}
#endif

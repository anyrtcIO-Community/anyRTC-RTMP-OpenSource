#ifndef ARLIVE_VIDEO_CAMERA_CAPTURER_H
#define ARLIVE_VIDEO_CAMERA_CAPTURER_H

#include "api/scoped_refptr.h"
#include "api/media_stream_interface.h"
#include "modules/video_capture/video_capture.h"
#include "sdk/android/native_api/jni/scoped_java_ref.h"
#include "sdk/android/native_api/video/video_source.h"

#include <memory>
#include <vector>
#include <stddef.h>
#include <jni.h>
#include <PlatformContext.h>
#include "IArDeviceManager.h"
namespace arlive {

enum class VideoState {
		Inactive,
		Paused,
		Active,
};
using namespace anyrtc;
class VideoCameraCapturer;

class VideoCameraCapturer :ArDeviceManager{

public:
	VideoCameraCapturer(rtc::scoped_refptr<webrtc::JavaVideoTrackSourceInterface> source, std::string deviceId, int width,int height,int fps, std::shared_ptr<PlatformContext> platformContext);
	void setState(arlive::VideoState state);
	virtual bool isFrontCamera();
	virtual int switchCamera(bool frontCamera);
	virtual void changeCaptureFormat(int width,int height,int fps);
	virtual float getCameraZoomMaxRatio();
	virtual int setCameraZoomRatio(float zoomRatio);
	virtual bool isAutoFocusEnabled();
	virtual int enableCameraAutoFocus(bool enabled);
	virtual int setCameraFocusPosition(float x, float y);
	virtual int enableCameraTorch(bool enabled);
	virtual int setAudioRoute(ArAudioRoute route);
	virtual int setSystemVolumeType(ArSystemVolumeType type);

private:
	std::shared_ptr<arlive::PlatformContext> _platformContext;
};

}  // namespace ARLIVE

#endif

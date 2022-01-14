#include "VideoRendererWin32.h"
#include <math.h>
#include "pluginds.h"

//
// VideoRendererWin32
//
// A little helper class to make sure we always to proper locking and
// unlocking when working with VideoRendererWin32 buffers.
template <typename T>
class AutoLock {
public:
	explicit AutoLock(T* obj) : obj_(obj) { obj_->Lock(); }
	~AutoLock() { obj_->Unlock(); }
protected:
	T* obj_;
};


/*rtc::VideoSinkInterface<cricket::VideoFrame>* webrtc::AVRtmplayerImpl::GetPlatformRender(void* handle)
{
	return new VideoRendererWin32((HWND)handle, 640, 480);
}*/

VideoRendererWin32* VideoRendererWin32::Create(HWND wnd, int width, int height)
{
	return new VideoRendererWin32((HWND)wnd, 640, 480);
}

VideoRendererWin32::VideoRendererWin32(
	HWND wnd, int width, int height)
	: ds_(NULL) {
	//VideoRendererWin32::Lock();
	ds_ = ds_display_open(wnd, 0);
}

VideoRendererWin32::~VideoRendererWin32() {
	//VideoRendererWin32::Unlock();

	if (ds_) {
		ds_display_close(ds_);
		ds_ = NULL;
	}
}

void VideoRendererWin32::OnYuv(uint8_t*pYUV420Data[3], int nStride[3], int nWidth, int nHeight)
{
	if (ds_) {
		ds_display_display_frame(ds_, pYUV420Data, nStride, nWidth, nHeight);
	}
}

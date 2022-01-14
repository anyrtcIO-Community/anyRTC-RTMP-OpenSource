#ifndef __ANDROID_RENDERER_H__
#define __ANDROID_RENDERER_H__
#include "../VideoRender/video_renderer.h"

class AndroidRenderer : public webrtc::VideoRenderer {
public:
	static AndroidRenderer* Create(const void* javaSink, size_t width,
		size_t height);

public:
	AndroidRenderer(const void* javaSink);
	virtual void OnFrame(const webrtc::VideoFrame& frame) override;
	virtual ~AndroidRenderer() { }

private:
	std::shared_ptr<rtc::VideoSinkInterface<webrtc::VideoFrame>> video_sink_;
	size_t width_;
	size_t height_;
};

#endif	// __ANDROID_RENDERER_H__
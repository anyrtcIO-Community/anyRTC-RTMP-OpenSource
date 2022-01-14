#ifndef __IOS_RENDERER_H__
#define __IOS_RENDERER_H__
#include "../VideoRender/video_renderer.h"
class IosRenderer : public webrtc::VideoRenderer {
public:
	static IosRenderer* Create(const void* iosSink, size_t width,
		size_t height);

public:
	IosRenderer(const void* javaSink);
	virtual void OnFrame(const webrtc::VideoFrame& frame) override;
	virtual ~IosRenderer() { }

private:
	std::shared_ptr<rtc::VideoSinkInterface<webrtc::VideoFrame>> video_sink_;
	size_t width_;
	size_t height_;
};

#endif	/// __IOS_RENDERER_H__


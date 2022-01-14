#include "IosRenderer.h"

namespace webrtc {
	VideoRenderer* VideoRenderer::CreatePlatformRenderer(const void* hwnd,
		size_t width,
		size_t height) {
		return IosRenderer::Create(hwnd, width, height);
	}
}//namespace webrtc
IosRenderer* IosRenderer::Create(const void* javaSink, size_t width,
	size_t height)
{
	return new IosRenderer(javaSink);
}
IosRenderer::IosRenderer(const void* javaSink)
	: width_(0)
	, height_(0)
{

}
void IosRenderer::OnFrame(const webrtc::VideoFrame& frame)
{
	if (video_sink_ != NULL) {
		video_sink_->OnFrame(frame);
	}
}

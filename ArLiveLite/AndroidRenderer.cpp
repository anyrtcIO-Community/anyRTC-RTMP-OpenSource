#include "AndroidRenderer.h"
#include <sdk/android/native_api/video/wrapper.h>
namespace webrtc {
	VideoRenderer* VideoRenderer::CreatePlatformRenderer(const void* hwnd,
		size_t width,
		size_t height) {
		return AndroidRenderer::Create(hwnd, width, height);
	}
}//namespace webrtc
AndroidRenderer* AndroidRenderer::Create(const void* javaSink, size_t width,
	size_t height)
{
	return new AndroidRenderer(javaSink);
}
AndroidRenderer::AndroidRenderer(const void* javaSink)
	: width_(0),
	height_(0)
{
	JNIEnv *env = webrtc::AttachCurrentThreadIfNeeded();
	video_sink_=webrtc::JavaToNativeVideoSink(env,(jobject) javaSink);

}
void AndroidRenderer::OnFrame(const webrtc::VideoFrame& frame)
{
	if (video_sink_ != NULL) {
		video_sink_->OnFrame(frame);
	}
}

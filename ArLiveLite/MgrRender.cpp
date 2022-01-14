#include "MgrRender.h"

MgrRender::MgrRender(void)
{

}
MgrRender::~MgrRender(void)
{

}

void MgrRender::SetRender(const char*pId, webrtc::VideoRenderer*render)
{
	rtc::CritScope l(&cs_renders_);
	if (map_renders_.find(pId) != map_renders_.end()) {
		webrtc::VideoRenderer*findRender = map_renders_[pId];
		map_renders_.erase(pId);

		delete findRender;
	}

	if (render != NULL) {
		map_renders_[pId] = render;
	}
}

void MgrRender::SetRotation(const char* pId, int nRotation)
{

}

void MgrRender::SetFillMode(const char* pId, int nMode)
{

}

void MgrRender::DoRenderFrame(const char*pId, const webrtc::VideoFrame& frame)
{
	rtc::CritScope l(&cs_renders_);
	if (map_renders_.find(pId) != map_renders_.end()) {
		webrtc::VideoRenderer*findRender = map_renders_[pId];

		findRender->OnFrame(frame);
	}
}
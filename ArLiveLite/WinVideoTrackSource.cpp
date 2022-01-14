#include "WinVideoTrackSource.h"

WinVideoTrackSource::WinVideoTrackSource(void)
{

}
WinVideoTrackSource::~WinVideoTrackSource(void)
{

}

void WinVideoTrackSource::FrameCaptured(const webrtc::VideoFrame& frame)
{
	OnFrame(frame);
}
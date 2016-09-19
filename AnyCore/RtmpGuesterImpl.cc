/*
*  Copyright (c) 2016 The AnyRTC project authors. All Rights Reserved.
*
*  Please visit https://www.anyrtc.io for detail.
*
* The GNU General Public License is a free, copyleft license for
* software and other kinds of works.
*
* The licenses for most software and other practical works are designed
* to take away your freedom to share and change the works.  By contrast,
* the GNU General Public License is intended to guarantee your freedom to
* share and change all versions of a program--to make sure it remains free
* software for all its users.  We, the Free Software Foundation, use the
* GNU General Public License for most of our software; it applies also to
* any other work released this way by its authors.  You can apply it to
* your programs, too.
* See the GNU LICENSE file for more info.
*/
#include "RtmpGuesterImpl.h"

RTMPGuester* RTMPGuester::Create(RTMPGuesterEvent&callback)
{
	return new RtmpGuesterImpl(callback);
}
void RTMPGuester::Destory(RTMPGuester*guester)
{
	RtmpGuesterImpl* impl = (RtmpGuesterImpl*)guester->GotSelfPtr();
	delete impl;
	guester = NULL;
}

RtmpGuesterImpl::RtmpGuesterImpl(RTMPGuesterEvent&callback)
	: callback_(callback)
	, worker_thread_(&webrtc::AnyRtmpCore::Inst())
	, av_rtmp_started_(NULL)
	, av_rtmp_player_(NULL)
	, video_render_(NULL)
{
	av_rtmp_player_ = AnyRtmplayer::Create(*this);
}


RtmpGuesterImpl::~RtmpGuesterImpl()
{
	StopRtmpPlay();
	if (av_rtmp_player_ != NULL) {
		av_rtmp_player_->StopPlay();
		delete av_rtmp_player_;
		av_rtmp_player_ = NULL;
	}

	if (video_render_ != NULL) {
		delete video_render_;
		video_render_ = NULL;
	}
}

//* Rtmp function for pull rtmp stream 
void RtmpGuesterImpl::StartRtmpPlay(const char* url, void* render)
{
	if (!av_rtmp_started_) {
		rtmp_url_ = url;
		av_rtmp_started_ = true;
		video_render_ = webrtc::VideoRenderer::Create(render, 640, 480);
		av_rtmp_player_->SetVideoRender(video_render_);
		av_rtmp_player_->StartPlay(url);
		webrtc::AnyRtmpCore::Inst().StartAudioTrack(this);
	}
}

void RtmpGuesterImpl::StopRtmpPlay()
{
	if (av_rtmp_started_) {
		av_rtmp_started_ = false;
		rtmp_url_ = "";
		webrtc::AnyRtmpCore::Inst().StopAudioTrack();
		av_rtmp_player_->StopPlay();
		if (video_render_ != NULL) {
			delete video_render_;
			video_render_ = NULL;
		}
	}
}

//* For AnyRtmplayerEvent
void RtmpGuesterImpl::OnRtmplayerOK()
{
	callback_.OnRtmplayerOK();
}
void RtmpGuesterImpl::OnRtmplayerStatus(int cacheTime, int curBitrate)
{
	callback_.OnRtmplayerStatus(cacheTime, curBitrate);
}
void RtmpGuesterImpl::OnRtmplayerCache(int time)
{
	callback_.OnRtmplayerCache(time);
}
void RtmpGuesterImpl::OnRtmplayerClose(int errcode)
{
	callback_.OnRtmplayerClosed(errcode);
}

//* For webrtc::AVAudioTrackCallback
int RtmpGuesterImpl::OnNeedPlayAudio(void* audioSamples, uint32_t& samplesPerSec, size_t& nChannels)
{
	return ((webrtc::AnyRtmplayerImpl*)av_rtmp_player_)->GetNeedPlayAudio(audioSamples, samplesPerSec, nChannels);
}
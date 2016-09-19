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
#ifndef __RTMP_GUSTER_IMPL_H__
#define __RTMP_GUSTER_IMPL_H__
#include "RtmpGuester.h"
#include "anyrtmpcore.h"
#include "anyrtmplayer.h"
#include "video_renderer.h"

class RtmpGuesterImpl : public RTMPGuester, public AnyRtmplayerEvent, public webrtc::AVAudioTrackCallback
{
public:
	RtmpGuesterImpl(RTMPGuesterEvent&callback);
	virtual ~RtmpGuesterImpl();

public:
	//* Rtmp function for pull rtmp stream 
	virtual void StartRtmpPlay(const char* url, void* render);
	virtual void StopRtmpPlay();

	virtual void* GotSelfPtr() { return this; };

protected:
	//* For AnyRtmplayerEvent
	virtual void OnRtmplayerOK();
	virtual void OnRtmplayerStatus(int cacheTime, int curBitrate);
	virtual void OnRtmplayerCache(int time);
	virtual void OnRtmplayerClose(int errcode);

	//* For webrtc::AVAudioTrackCallback
	virtual int OnNeedPlayAudio(void* audioSamples, uint32_t& samplesPerSec, size_t& nChannels);

private:
	RTMPGuesterEvent	&callback_;
	rtc::Thread         *worker_thread_;
	std::string			rtmp_url_;

	bool				av_rtmp_started_;
	AnyRtmplayer			*av_rtmp_player_;

	webrtc::VideoRenderer	*video_render_;
};

#endif	// __RTMP_GUSTER_IMPL_H__
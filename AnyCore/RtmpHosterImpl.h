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

#ifndef __RTMP_HOSTER_IMPL_H__
#define __RTMP_HOSTER_IMPL_H__

#include "RtmpHoster.h"
#include "anyrtmpcore.h"
#include "anyrtmpstreamer.h"
#include "videofilter.h"
#include "video_renderer.h"
#include "webrtc/media/base/videocapturer.h"

class RtmpHosterImpl : public RTMPHoster, public AnyRtmpstreamerEvent, rtc::MessageHandler, public webrtc::AVAudioRecordCallback
{
public:
	RtmpHosterImpl(RTMPHosterEvent&callback);
	virtual ~RtmpHosterImpl();


public:
	//* For	RTMPHoster
	//* Common function
	virtual void SetAudioEnable(bool enabled);
	virtual void SetVideoEnable(bool enabled);
	virtual void SetVideoRender(void* render);
	virtual void SetVideoCapturer(void* handle);
	virtual void SetVideoMode(RTMPVideoMode videoMode);

	//* Rtmp function for push rtmp stream 
	virtual void StartRtmpStream(const char*url);
	virtual void StopRtmpStream();

	void* GotSelfPtr() { return this; };

protected:
	void AddVideoCapturer_w(void* handle);
	void RemoveAVideoCapturer_w();

public:
	//* For AnyRtmpstreamerEvent
	virtual bool ExternalVideoEncoder();
	virtual void OnStreamOk();
	virtual void OnStreamReconnecting(int times);
	virtual void OnStreamFailed(int code);
	virtual void OnStreamClosed();
	virtual void OnStreamStatus(int delayMs, int netBand);

public:
	//* For rtc::MessageHandler
	virtual void OnMessage(rtc::Message* msg);

	//* For webrtc::AVAudioRecordCallback
	virtual void OnRecordAudio(const void* audioSamples, const size_t nSamples,
		const size_t nBytesPerSample, const size_t nChannels, const uint32_t samplesPerSec, const uint32_t totalDelayMS);

private:
	RTMPHosterEvent		&callback_;
	rtc::Thread         *worker_thread_;
	bool				av_rtmp_started_;
	std::string         rtmp_url_;
	AnyRtmpstreamer		*av_rtmp_streamer_;

	int					req_w_;
	int					req_h_;
	int					v_width_;
	int					v_height_;
	std::unique_ptr<cricket::VideoCapturer>	video_capturer_;
	VideoFilter			video_filter_;

	webrtc::VideoRenderer*	video_render_;
};

#endif	// __RTMP_HOSTER_IMPL_H__
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
#ifndef __ANY_RTMP_STREAMER_H__
#define __ANY_RTMP_STREAMER_H__
#include "avcodec.h"
#include "anyrtmpcore.h"
#include "anyrtmpush.h"
#include "anyrtmpstream_interface.h"
#include "webrtc/audio_sink.h"
#include "webrtc/api/mediastreaminterface.h"
#include "webrtc/base/thread.h"
#include "webrtc/media/base/videocapturer.h"
#include "webrtc/media/base/videobroadcaster.h"
#include "webrtc/modules/audio_device/include/audio_device_defines.h"

namespace webrtc {

class AnyRtmpStreamerImpl : public AnyRtmpstreamer, public AVCodecCallback, public AnyRtmpushCallback
{
public:
	AnyRtmpStreamerImpl(AnyRtmpstreamerEvent&callback);
	virtual ~AnyRtmpStreamerImpl(void);

	webrtc::AudioSinkInterface* GetAudioSink(){ return a_aac_encoder_; };
	rtc::VideoSinkInterface<cricket::VideoFrame>* GetVideoSink() {return v_h264_encoder_;};
	virtual void SetAudioEnable(bool enabled);
	virtual void SetVideoEnable(bool enabled);
    virtual void SetAutoAdjustBit(bool enabled);
	virtual void SetVideoParameter(int w, int h, int bitrate);
	virtual void SetBitrate(int bitrate);

	void StartStream(const std::string&url);
	void StopStream();

public:
	//* For AVCodecCallback
	virtual void OnEncodeDataCallback(bool audio, uint8_t *p, uint32_t length, uint32_t ts);

	//* For AnyRtmpushCallback
	virtual void OnRtmpConnected();
	virtual void OnRtmpReconnecting(int times);
	virtual void OnRtmpDisconnect();
	virtual void OnRtmpStatusEvent(int delayMs, int netBand);

protected:
	virtual void StartEncoder();
	virtual void StopEncoder();
	void OnAACData(uint8_t* pdata, int len, uint32_t ts);
	void OnH264Data(uint8_t* pdata, int len, uint32_t ts);

private:
	bool					rtmp_connected_;
    bool                    auto_adjust_bit_;
	AnyRtmpstreamerEvent		&callback_;

	// Audio
	A_AACEncoder*			a_aac_encoder_;
	bool					a_muted_;
	bool					a_enabled_;
	int						a_sample_hz_;
	int						a_channels_;
	// Video
	V_H264Encoder*			v_h264_encoder_;
	int						v_width;
	int						v_height;
	int						v_framerate_;
	int						v_bitrate_;

    rtc::CriticalSection	cs_av_rtmp_;
	AnyRtmpPush*				av_rtmp_;
};

}	// namespace webrtc

#endif	// __ANY_RTMP_STREAMER_H__
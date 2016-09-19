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
#ifndef __ANY_RTMP_CORE_H__
#define __ANY_RTMP_CORE_H__
#include "webrtc/base/criticalsection.h"
#include "webrtc/base/scoped_ptr.h"
#include "webrtc/base/thread.h"
#include "webrtc/common_audio/ring_buffer.h"
#include "webrtc/media/engine/webrtcvideodecoderfactory.h"
#include "webrtc/media/engine/webrtcvideoencoderfactory.h"
#include "webrtc/modules/audio_coding/acm2/acm_resampler.h"
#include "webrtc/modules/audio_device/include/audio_device.h"
#include "webrtc/modules/audio_device/include/audio_device_defines.h"


namespace webrtc {
class AVAudioRecordCallback {
public:
	AVAudioRecordCallback(void){};
	virtual ~AVAudioRecordCallback(void){};

	virtual void OnRecordAudio(const void* audioSamples, const size_t nSamples,
		const size_t nBytesPerSample, const size_t nChannels, const uint32_t samplesPerSec, const uint32_t totalDelayMS) = 0;
};

class AVAudioTrackCallback {
public:
	AVAudioTrackCallback(void){};
	virtual ~AVAudioTrackCallback(void){};

	virtual int OnNeedPlayAudio(void* audioSamples, uint32_t& samplesPerSec, size_t& nChannels) = 0;
};

class AnyRtmpCore : public rtc::Thread, webrtc::AudioTransport
{
public:
	static AnyRtmpCore& Inst() {
		static AnyRtmpCore avcore;
		return avcore;
	}

	void StartAudioRecord(AVAudioRecordCallback* callback, int sampleHz, int channel);
	void StopAudioRecord();

	void StartAudioTrack(AVAudioTrackCallback* callback);
	void StopAudioTrack();

public:
	void SetExternalVideoEncoderFactory(cricket::WebRtcVideoEncoderFactory* factory);
	cricket::WebRtcVideoEncoderFactory* ExternalVideoEncoderFactory();

protected:
	//* For rtc::Thread
	virtual void Run();

	//* For webrtc::AudioTransport
	virtual int32_t RecordedDataIsAvailable(const void* audioSamples, const size_t nSamples, 
		const size_t nBytesPerSample, const size_t nChannels, const uint32_t samplesPerSec, const uint32_t totalDelayMS, 
		const int32_t clockDrift, const uint32_t currentMicLevel, const bool keyPressed,uint32_t& newMicLevel);

	virtual int32_t NeedMorePlayData(const size_t nSamples, const size_t nBytesPerSample, const size_t nChannels,
		const uint32_t samplesPerSec, void* audioSamples, size_t& nSamplesOut, int64_t* elapsed_time_ms, int64_t* ntp_time_ms);

protected:
	AnyRtmpCore();
	virtual ~AnyRtmpCore();
	bool					running_;
	rtc::scoped_refptr<webrtc::AudioDeviceModule>		audio_device_ptr_;
	rtc::scoped_ptr<cricket::WebRtcVideoEncoderFactory> video_encoder_factory_;

	//* For audio record
	rtc::CriticalSection	cs_audio_record_;
	webrtc::acm2::ACMResampler resampler_record_;
	AVAudioRecordCallback	*audio_record_callback_;
	int						audio_record_sample_hz_;
	int						audio_record_channels_;

	//* For audio player
	rtc::CriticalSection	cs_audio_track_;
	webrtc::acm2::ACMResampler resampler_track_;
	AVAudioTrackCallback	*audio_track_callback_;
	int						audio_track_sample_hz_;
	int						audio_track_channels_;
};

}	// namespace webrtc

#endif	// __ANY_RTMP_CORE_H__
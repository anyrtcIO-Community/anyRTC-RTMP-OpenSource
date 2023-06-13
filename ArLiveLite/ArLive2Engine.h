/*
 *  Copyright (c) 2013 The WebRTC@AnyRTC project authors. All Rights Reserved.
 *
 *  Use of this source code is governed by a BSD-style license
 *  that can be found in the LICENSE file in the root of the source
 *  tree. An additional intellectual property rights grant can be found
 *  in the file PATENTS.  All contributing project authors may
 *  be found in the AUTHORS file in the root of the source tree.
 *
 *  This is a part of the AR RTC Service SDK.
 *  Copyright (C) 2021 anRTC IO
 *  All rights reserved.
 *  Author - Eric.Mao
 *  Email - maozongwu@dync.cc
 *  Website - https://www.anyrtc.io
 *
 */
#ifndef __AR_LIVE2_ENGINE_H__
#define __AR_LIVE2_ENGINE_H__
#include "IArLive2Engine.h"
#include "AudDeviceEvent.h"
#include "RtcTick.h"
#include "MgrRender.h"
#include "PlatformImpl.h"
#include "rtc_base/deprecated/recursive_critical_section.h"
#include "rtc_base/thread.h"
#include "api/peer_connection_interface.h"
#include "api/video_codecs/video_encoder_factory.h"
#include "api/video_codecs/video_decoder_factory.h"
#include "modules/audio_coding/acm2/acm_resampler.h"
#include "modules/audio_device/include/audio_device.h"
#include "modules/audio_device/include/audio_device_defines.h"
#include "modules/video_capture/video_capture.h"

class ArLive2Engine : public AR::IArLive2Engine, public rtc::Thread, public MThreadTick, webrtc::AudioTransport
{
public:
	ArLive2Engine(void);
	virtual ~ArLive2Engine(void);

	//* For IArLive2Engine
	virtual int32_t initialize(AR::IArLive2EngineObserver* observer);

	virtual void release();

#ifdef __ANDROID__
	virtual AR::IArLivePusher* createArLivePusher(void* context);

	virtual AR::IArLivePlayer* createArLivePlayer(void* context);
#else
	virtual AR::IArLivePusher* createArLivePusher();

	virtual AR::IArLivePlayer* createArLivePlayer();
#endif
	virtual void releaseArLivePusher(AR::IArLivePusher* pusher);

	virtual void releaseArLivePlayer(AR::IArLivePlayer* player);

	virtual void setAppInBackground(bool bBackground);

public:
	int32_t setVideoRenderView(AR::uid_t renderId, const AR::VideoCanvas& canvas);

	int32_t setVideoRenderMirror(AR::uid_t renderId, AR::ArLiveMirrorType mirrorType);

	int32_t setVideoRenderRotation(AR::uid_t renderId, AR::ArLiveRotation rotation);

	int32_t setVideoRenderFillMode(AR::uid_t renderId, AR::ArLiveFillMode mode);


	//* For rtc::Thread
	virtual void Run();

	//* For webrtc::AudioTransport
	virtual int32_t RecordedDataIsAvailable(const void* audioSamples, const size_t nSamples,
		const size_t nBytesPerSample, const size_t nChannels, const uint32_t samplesPerSec, const uint32_t totalDelayMS,
		const int32_t clockDrift, const uint32_t currentMicLevel, const bool keyPressed, uint32_t& newMicLevel);

	virtual int32_t NeedMorePlayData(const size_t nSamples, const size_t nBytesPerSample, const size_t nChannels,
		const uint32_t samplesPerSec, void* audioSamples, size_t& nSamplesOut, int64_t* elapsed_time_ms, int64_t* ntp_time_ms);

	// Method to pull mixed render audio data from all active VoE channels.
	// The data will not be passed as reference for audio processing internally.
	virtual void PullRenderData(int bits_per_sample,
		int sample_rate,
		size_t number_of_channels,
		size_t number_of_frames,
		void* audio_data,
		int64_t* elapsed_time_ms,
		int64_t* ntp_time_ms) {};


public:
	rtc::Thread* GetThread() {
		return this;
	}
	MgrRender& GetMgrRender() {
		return mgr_render_;
	}

	rtc::scoped_refptr<webrtc::PeerConnectionFactoryInterface> PeerConnectionFactory() {
		return peer_connection_factory_;
	}



protected:
	void InitAudDevice();
	void DeInitAudDevice();
	void InitPeerConnection();
	void DeInitPeerConnection();

public:
	void AttachAudCapture(AudDevCaptureEvent*pEvent);
	void DetachAudCapture(AudDevCaptureEvent*pEvent);

	void AttachAudSpeaker(AudDevSpeakerEvent*pEvent);
	void DetachAudSpeaker(AudDevSpeakerEvent*pEvent);

private:
    AR::IArLive2EngineObserver* observer_;

	// Control or logic
	bool	b_running_;
	bool	b_app_background_;
	bool	b_aud_cap_exception_;
	bool	b_aud_ply_exception_;
	bool	b_video_preview_;
	bool	b_video_muted_;
	int64_t n_timer_10ms_;
	int64_t n_timer_100ms_;

private:
	// Video render
	MgrRender				mgr_render_;

	// Audio devices
	typedef std::map<AudDevCaptureEvent*, void*> MapAudDevCapture;
	typedef std::map<AudDevSpeakerEvent*, void*> MapAudDevSpeaker;
	rtc::RecursiveCriticalSection	cs_aud_capture_;
	MapAudDevCapture		map_aud_dev_capture_;
	rtc::RecursiveCriticalSection	cs_aud_speaker_;
	MapAudDevSpeaker		map_aud_dev_speaker_;

private:
	// PeerConnection
	rtc::scoped_refptr<webrtc::AudioDeviceModule>		audio_device_ptr_;
	std::unique_ptr<webrtc::TaskQueueFactory>			task_queue_factory_;
	rtc::scoped_refptr<webrtc::AudioDeviceModule>		rtc_adm_;
	rtc::scoped_refptr<webrtc::PeerConnectionFactoryInterface> peer_connection_factory_;

private:
	// Pusher
	typedef std::map<std::string, AR::IArLivePusher*>MapArLive2Pusher;
	rtc::RecursiveCriticalSection	cs_arlive2_pusher_;
	MapArLive2Pusher		map_arlive2_pusher_;

	// Player
	typedef std::map<std::string, AR::IArLivePlayer*>MapArLive2Player;
	rtc::RecursiveCriticalSection	cs_arlive2_player_;
	MapArLive2Player		map_arlive2_player_;
};

#endif	// __AR_LIVE2_ENGINE_H__


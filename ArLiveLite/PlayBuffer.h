/*
*  Copyright (c) 2021 The AnyRTC project authors. All Rights Reserved.
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
#ifndef __PLAY_BUFFER_H__
#define __PLAY_BUFFER_H__
#include <list>
#include "rtc_base/deprecated/recursive_critical_section.h"
#include "modules/audio_coding/acm2/acm_resampler.h"
#include "api/video/video_frame.h"

struct PcmData {
	PcmData(char* pdata, int len, int sample_hz, int channels) :len_(len), sample_hz_(sample_hz), channels_(channels), pts_(0) {
		pdata_ = new char[len];
		memcpy(pdata_, pdata, len);
	}
	virtual ~PcmData(void) {
		delete[] pdata_;
	}
	char* pdata_;
	int len_;
	int sample_hz_;
	int channels_;
	int64_t pts_;
};

struct VideoData {
	VideoData(): pdata_(NULL), len_(0) { };
	virtual ~VideoData(void) {
		if (pdata_ != NULL) {
			delete[] pdata_;
			pdata_ = NULL;
		}
		
	};

	void SetData(char* ptr, int len) {
		if (pdata_ != NULL) {
			delete[] pdata_;
			pdata_ = NULL;
		}
		pdata_ = ptr; len_ = len;
	}

	rtc::scoped_refptr<webrtc::VideoFrameBuffer> video_frame_;
	char* pdata_;
	int len_;
	int64_t pts_;
};


class PlayBuffer
{
public:
	PlayBuffer(void);
	virtual ~PlayBuffer(void);

	int DoVidRender(bool bVideoPaused);
	int DoAudRender(bool mix, void* audioSamples, uint32_t samplesPerSec, int nChannels, bool bAudioPaused);
	int DoRender(bool mix, void* audioSamples, uint32_t samplesPerSec, int nChannels, bool bAudioPaused, bool bVideoPaused);
	void DoClear();
	void SetAppInBackground(bool bBackground);

	bool NeedMoreAudioPlyData();
	bool NeedMoreVideoPlyData();
	bool AppIsBackground();
	void PlayVideoData(VideoData *videoData);
	void PlayAudioData(PcmData*pcmData);

public:
	virtual void OnBufferVideoRender(VideoData *videoData, int64_t pts) {};
	virtual void OnBufferVideoDropped() {};
	virtual void OnBufferAudioDropped() {};
	virtual void OnFirstVideoDecoded() {};
	virtual void OnFirstAudioDecoded() {};

private:
	bool	b_video_decoded_;
	bool	b_audio_decoded_;
	bool	b_app_in_background_;		// app 是否进入后台
	int64_t n_last_render_video_time_;
	int64_t n_last_render_video_pts_;

	//* 显示缓存
	rtc::RecursiveCriticalSection	cs_audio_play_;
	std::list<PcmData*>		lst_audio_play_;
	rtc::RecursiveCriticalSection	cs_video_play_;
	std::list<VideoData*>	lst_video_play_;

private:
	webrtc::acm2::ACMResampler resampler_;
	char* aud_data_resamp_;             // The actual 16bit audio data.
	char* aud_data_mix_;
};

#endif	// __PLAY_BUFFER_H__
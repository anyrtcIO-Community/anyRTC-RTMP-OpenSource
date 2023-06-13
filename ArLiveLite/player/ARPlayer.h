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
#ifndef __ARP_PLAYER_H__
#define __ARP_PLAYER_H__
#include "ARDef.h"

typedef enum AUDIO_DUAL_MONO_MODE {
	/**
	 * 0: Original mode.
	 */
	AUDIO_DUAL_MONO_AUTO = 0,
	/**
	 * 1: Left channel mode. This mode replaces the audio of the right channel
	 * with the audio of the left channel, which means the user can only hear
	 * the audio of the left channel.
	 */
	 AUDIO_DUAL_MONO_L = 1,
	 /**
	  * 2: Right channel mode. This mode replaces the audio of the left channel with
	  * the audio of the right channel, which means the user can only hear the audio
	  * of the right channel.
	  */
	  AUDIO_DUAL_MONO_R = 2,
	  /**
	   * 3: Mixed channel mode. This mode mixes the audio of the left channel and
	   * the right channel, which means the user can hear the audio of the left
	   * channel and the right channel at the same time.
	   */
	   AUDIO_DUAL_MONO_MIX = 3
}AUDIO_DUAL_MONO_MODE;

class ARPlayerEvent
{
public:
	ARPlayerEvent(void) {};
	virtual ~ARPlayerEvent(void) {};

	//* 播放成功，只有播放的第一次才会回到
	virtual void OnArPlyOK(void*player) {};
	virtual void OnArPlyPlaying(void* player, bool hasAudio, bool hasVideo) {};
	virtual void OnArPlyCacheing(void* player, bool hasAudio, bool hasVideo) {};
	virtual void OnArPlyClose(void*player,int nErrCode) {};
	virtual void OnArPlyVolumeUpdate(void* player, int32_t volume) {};
	virtual void OnArPlyVodProcess(void* player, int nAllTimeMs, int nCurTimeMs, int nCacheTimeMs) {};
	virtual void OnArPlyStatistics(void* player, int nWidth, int nHeight, int nFps, int nAudBitrate, int nVidBitrate) {};

	virtual bool OnArPlyNeedMoreAudioData(void*player) { return true; };
	virtual bool OnArPlyNeedMoreVideoData(void*player) { return true; };
	virtual bool OnArPlyAppIsBackground(void* player) { return false; };
	virtual void OnArPlyAudio(void*player, const char*pData, int nSampleHz, int nChannels, int64_t pts) {};
	virtual void OnArPlyVideo(void*player, int fmt, int ww, int hh, uint8_t**pData, int*linesize, int64_t pts) {};
	virtual void OnArPlyRawVideo(void* player, const char* pData, int nLen, bool bKeyframe, int64_t pts) {};
	virtual void OnArPlySeiData(void* player, const char* pData, int nLen, int64_t pts) {};
};

struct PlayerUserSet
{
	PlayerUserSet(void)
		: b_repeat_(false)
		, n_repeat_count_(0)
		, b_audio_enabled_(true)
		, b_video_enabled_(true)
		, f_aud_volume_(1.0)
		, n_volume_evaluation_interval_ms_(0)
		, n_volume_evaluation_used_ms_(0){};
	bool b_repeat_;
	int n_repeat_count_;
	bool b_audio_enabled_;
	bool b_video_enabled_;
	float f_aud_volume_;
	int n_volume_evaluation_interval_ms_;
	int n_volume_evaluation_used_ms_;
};

class ARPlayer
{
public:
	ARPlayer(ARPlayerEvent&callback) :callback_(callback), e_audio_dual_mono_mode_(AUDIO_DUAL_MONO_AUTO){};
	virtual ~ARPlayer(void) {};

	virtual int StartTask(const char*strUrl) = 0;
	virtual void StopTask() = 0;

	virtual void RunOnce() = 0;
	virtual void SetAudioEnable(bool bAudioEnable) = 0;
	virtual void SetVideoEnable(bool bVideoEnable) = 0;
	// true: repeat
	virtual void SetRepeat(bool bEnable) = 0;
	virtual void SetUseTcp(bool bUseTcp) = 0;
	virtual void SetNoBuffer(bool bNoBuffer) = 0;
	// -1: loop forever >=0 repeat count
	virtual void SetRepeatCount(int loopCount) = 0;
	virtual void SeekTo(int nSeconds) = 0;
	virtual void SetSpeed(float fSpeed) = 0;
	virtual void SetVolume(float fVolume) = 0;
	virtual void EnableVolumeEvaluation(int32_t intervalMs) = 0;
	virtual int GetTotalDuration() = 0;

	virtual void SetRtcFactory(void* ptr) {};
	virtual void Play() {};
	virtual void Pause() {};
	virtual void RePlay() {};
	virtual void SetAudioDualMonoMode(AUDIO_DUAL_MONO_MODE eMode) { e_audio_dual_mono_mode_ = eMode; };
	virtual void selectAudioTrack(int index) {};
	virtual int getAudioTrackCount() { return 0; };

	virtual void Config(bool bAuto, int nCacheTime, int nMinCacheTime, int nMaxCacheTime, int nVideoBlockThreshold) = 0;

protected:
	ARPlayerEvent	&callback_;

	PlayerUserSet	user_set_;

	AUDIO_DUAL_MONO_MODE	e_audio_dual_mono_mode_;
};


ARP_API ARPlayer* ARP_CALL createARPlayer(ARPlayerEvent&callback);

ARP_API ARPlayer* ARP_CALL createRtmpPlayer(ARPlayerEvent& callback);

#endif	// __ARP_PLAYER_H__

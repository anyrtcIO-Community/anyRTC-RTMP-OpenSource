#ifndef __AR_LIVE2_PLAYER_H__
#define __AR_LIVE2_PLAYER_H__
#include "ArLive2Engine.h"
#include "IArLivePlayer.hpp"
#include "AudDeviceEvent.h"
#include <list>
#include "rtc_base/deprecated/recursive_critical_section.h"
#include "rtc_base/thread.h"
#include "api/video/i420_buffer.h"
#include "codec/pluginaac.h"
#include "PlayBuffer.h"
#include "ARPlayer.h"

struct CustemRender
{
	CustemRender(void):bEnable(false), ePixelFormat(AR::ArLivePixelFormatUnknown), eBufferType(AR::ArLiveBufferTypeUnknown){}
	bool bEnable;
	AR::ArLivePixelFormat ePixelFormat;
	AR::ArLiveBufferType eBufferType;
};

class ArLive2Player : public AR::IArLivePlayer, public RtcTick, public PlayBuffer, public AudDevSpeakerEvent, public ARPlayerEvent
{
public:
	ArLive2Player(ArLive2Engine*pEngine, const std::string&strPlayId);
	virtual ~ArLive2Player(void);

	const std::string& PlayId() { return str_local_play_id_; };

	//* For AR::IArLivePlayer
	virtual void setObserver(AR::ArLivePlayerObserver* observer);
	virtual int32_t setRenderView(void* view);
	virtual int32_t setRenderRotation(AR::ArLiveRotation rotation);
	virtual int32_t setRenderFillMode(AR::ArLiveFillMode mode);
	virtual int32_t setPlayMode(AR::ArLivePlayMode mode);
	virtual int32_t startPlay(const char* url);
	virtual int32_t stopPlay();
	virtual int32_t isPlaying();
	virtual int32_t seekTo(int seekTimeS);
	virtual int32_t setSpeed(float speed);
	virtual int32_t rePlay();
	virtual int32_t pauseAudio();
	virtual int32_t resumeAudio();
	virtual int32_t pauseVideo();
	virtual int32_t resumeVideo();
	virtual int32_t setPlayoutVolume(int32_t volume);
	virtual int32_t setCacheParams(float minTime, float maxTime);
	virtual int32_t enableVolumeEvaluation(int32_t intervalMs);
	virtual int32_t snapshot();
	virtual int32_t enableCustomRendering(bool enable, AR::ArLivePixelFormat pixelFormat, AR::ArLiveBufferType bufferType);
	virtual int32_t enableReceiveSeiMessage(bool enable, int payloadType);
	virtual void showDebugView(bool isShow);

	//* For RtcTick
	virtual void OnTick();
	virtual void OnTickUnAttach();

	//* For PlayBuffer
	virtual void OnBufferVideoRender(VideoData *videoData, int64_t pts);
	virtual void OnFirstVideoDecoded();
	virtual void OnFirstAudioDecoded();

	//* For AudDevSpeakerEvent
	virtual int MixAudioData(bool mix, void* audioSamples, uint32_t samplesPerSec, int nChannels);

	//* For ARPlayerEvent
	virtual void OnArPlyOK(void*player);
	virtual void OnArPlyPlaying(void* player, bool hasAudio, bool hasVideo);
	virtual void OnArPlyCacheing(void* player, bool hasAudio, bool hasVideo);
	virtual void OnArPlyClose(void*player, int errcode);
	virtual void OnArPlyVolumeUpdate(void* player, int32_t volume);
	virtual void OnArPlyVodProcess(void* player, int nAllTimeMs, int nCurTimeMs, int nCacheTimeMs);
	virtual void OnArPlyStatistics(void* player, int nWidth, int nHeight, int nFps, int nAudBitrate, int nVidBitrate);

	virtual bool OnArPlyNeedMoreAudioData(void*player);
	virtual bool OnArPlyNeedMoreVideoData(void*player);
	virtual bool OnArPlyAppIsBackground(void* player);
	virtual bool OnArPlyIsLiveMode(void* player);
	virtual void OnArPlyAudio(void*player, const char*pData, int nSampleHz, int nChannels, int64_t pts);
	virtual void OnArPlyVideo(void*player, int fmt, int ww, int hh, uint8_t**pData, int*linesize, int64_t pts);
	virtual void OnArPlySeiData(void* player, const char* pData, int nLen, int64_t pts);

private:
	AR::ArLivePlayerObserver* observer_;

private:
	ArLive2Engine*				ar_engine_;
	rtc::Thread*				main_thread_;
	std::string					str_local_play_id_;
	ARPlayer*					ar_player_;
	bool						b_play_ok_;
	bool						b_shutdown_;
	bool						b_audio_paused_;
	bool						b_video_paused_;
	AR::ArLivePlayMode			e_play_mode_;
	int							n_sei_payload_type_;
	std::string					str_play_url_;

	AR::ArLivePlayConfig		ar_play_conf_;

	AR::ArLiveRotation			frameRotation;

private:
	CustemRender	custom_render_;
	int				n_rgba_width_;
	int				n_rgba_height_;
	char			*p_rgba_data_;
	bool			b_snap_shot_;

	rtc::RecursiveCriticalSection cs_render_frame_;
	rtc::scoped_refptr<webrtc::I420Buffer> i420_render_frame_;
};

#endif	// __AR_LIVE2_PLAYER_H__

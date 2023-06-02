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
#ifndef __ARP_RTMP_PLAYER_H__
#define __ARP_RTMP_PLAYER_H__
#include "ARPlayer.h"
#include "ArNetClient.h"
#include "rtmp-client.h"
#include "flv-demuxer.h"
#include "flv-proto.h"
#include "rtc_base/thread.h"
#include "JitBuffer.h"
#include "codec/pluginaac.h"
#include "codec/AvCodec.h"

class ARRtmpPlayer : public ARPlayer, public JitBuffer, public INetClientEvent, public webrtc::RtcVidDecoderEvent
{
public:
	ARRtmpPlayer(ARPlayerEvent& callback);
	virtual ~ARRtmpPlayer(void);

	//* For ARPlayer
	virtual int StartTask(const char* strUrl);
	virtual void StopTask();
	virtual void RunOnce();
	virtual void Play();
	virtual void Pause();
	virtual void SetAudioEnable(bool bAudioEnable);
	virtual void SetVideoEnable(bool bVideoEnable);
	virtual void SetRepeat(bool bEnable);
	virtual void SetUseTcp(bool bUseTcp);
	virtual void SetNoBuffer(bool bNoBuffer);
	virtual void SetRepeatCount(int loopCount);
	virtual void SeekTo(int nSeconds);
	virtual void SetSpeed(float fSpeed);
	virtual void SetVolume(float fVolume);
	virtual void EnableVolumeEvaluation(int32_t intervalMs);
	virtual int GetTotalDuration();
	virtual void RePlay();
	virtual void Config(bool bAuto, int nCacheTime, int nMinCacheTime, int nMaxCacheTime, int nVideoBlockThreshold);
	virtual void selectAudioTrack(int index);
	virtual int getAudioTrackCount();

	//* For PlayerBuffer
	virtual void OnBufferDecodeVideoData(const uint8_t* pData, int len, bool bKeyrame, uint32_t ts);
	virtual void OnBufferDecodeAudioData(const uint8_t* pData, int len, int seqn, uint32_t ts);
	virtual void OnBufferStatusChanged(PlayStatus playStatus);
	virtual bool OnBufferGetPuased();

	//* For ArNetClientEvent
	virtual void OnArClientConnected();
	virtual void OnArClientConnectFailure();
	virtual void OnArClientDisconnect();
	virtual void OnArClientSent(int err);
	virtual void OnArClientRecv(const char* pData, int nLen);

	//* For webrtc::RtcVidDecoderEvent
	virtual void OnDecodeFrame(const char* strIdd, const char* yData, const char* uData, const char* vData, int strideY, int strideU, int strideV, int w, int h, int rotate, unsigned int timeStamp);

public:
	//* For Rtmp
	void on_flv_demuxer_data(int type, const void* data, size_t bytes, uint32_t timestamp);
	int do_rtmp_client_send(const void* header, size_t len, const void* payload, size_t bytes);
	int do_rtmp_client_onaudio(const void* audio, size_t bytes, uint32_t timestamp);
	int do_rtmp_client_onvideo(const void* video, size_t bytes, uint32_t timestamp);
	int do_rtmp_client_onscript(const void* script, size_t bytes, uint32_t timestamp);

protected:
	bool NaluIsKeyFrame(const unsigned char* pData, int nLen);
private:
	rtc::Thread* main_thread_;

	std::string str_rtmp_url_;

	INetClient* ar_net_client_;
	rtmp_client_t* rtmp_client_;
	bool			b_rtmp_connected_;
	flv_demuxer_t* flv_demuxer_;

private:
	//* For audio
	aac_dec_t		aac_decoder_;
	uint8_t* audio_cache_;
	int				a_cache_len_;
	uint32_t		aac_sample_hz_;
	uint8_t			aac_channels_;
	uint32_t		aac_frame_per10ms_size_;

	//* For video
	webrtc::V_H264Decoder	*h264_decoder_;
};

#endif	//__ARP_RTMP_PLAYER_H__
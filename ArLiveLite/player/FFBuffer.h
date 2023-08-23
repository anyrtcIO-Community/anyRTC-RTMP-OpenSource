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
#ifndef __FF_BUFFER_H__
#define __FF_BUFFER_H__
#include <list>
#include "rtc_base/deprecated/recursive_critical_section.h"
extern "C" {
#include "libavformat/avformat.h"
}

struct RecvPacket
{
	RecvPacket(void): pkt_(NULL), dts_(0), pts_(0), duration_(0), recv_time_(0){
	};
	virtual ~RecvPacket(void) {
		if (pkt_ != NULL) {
			av_packet_unref(pkt_);
			delete pkt_;
			pkt_ = NULL;
		}
	};

	AVPacket* pkt_;
	int64_t dts_;
	int64_t pts_;
	int64_t duration_;
	int64_t recv_time_;
};


enum PlayMode
{
	PM_Fluent = 0,	// 流畅，会有缓冲，适合直播
	PM_RealTime,	// 实时，没有缓冲，适合短视频
};

enum PlayStatus
{
	PS_Init = 0,	// 初始化状态
	PS_Caching,		// 缓冲中
	PS_Playing,		// 播放中
};

struct PlaySetting
{
	bool bAuto;					//设置是否自动调整缓存时间。
	int nCacheTime;				//设置播放器缓存时间。
	int nMinCacheTime;			//设置最小的缓存时间。
	int nMaxCacheTime;			//设置最大的缓存时间。
	int nVideoBlockThreshold;	//设置播放器视频卡顿报警阈值。
};

class FFBuffer
{
public:
	FFBuffer();
	virtual ~FFBuffer(void);

	void DoClear();
	void DoTick();
	bool DoDecodeAudio();
	bool DoDecodeVideo(bool bBackground);
	
	void SetPlaySetting(bool bAuto, int nCacheTime, int nMinCacheTime, int nMaxCacheTime, int nVideoBlockThreshold);
	void ResetTime();
	bool NeedMoreData();
	bool IsPlaying();
	bool BufferIsEmpty();
	int AudioCacheTime();
	int VideoCacheTime();
	void RecvVideoData(AVPacket* pkt, int64_t dts, int64_t pts, int64_t duration);
	void RecvAudioData(AVPacket* pkt, int64_t dts, int64_t pts, int64_t duration);

public:
	virtual void OnBufferDecodeAudioData(AVPacket* pkt) {};
	virtual void OnBufferDecodeVideoData(AVPacket* pkt) {};
	virtual void OnBufferStatusChanged(PlayStatus playStatus) {};
	virtual bool OnBufferIsKeyFrame(AVPacket* pkt) { return true; };
	virtual bool OnBufferGetPuased() { return false; }
	virtual float OnBufferGetSpeed() { return 1.0; };
	
private:
	PlayStatus	play_status_;			// 当前状态
	PlayMode	play_mode_;				// 播放的模式
	bool		b_reset_time_;
	bool		b_need_keyframe_;		// 视频解码需要关键帧
	int32_t		n_cacheing_time_;		// 缓冲区时间，如果低于这个时间，就需要从网络库中读取
	int32_t     n_cache_to_play_time_;	// 缓冲播放时间，开始或卡顿后，必须达到这个时间才能重新播放
	int32_t		n_cache_to_play_max_;	// 最大的缓冲播放时间
	uint32_t	last_sys_time_;			// 上次调用的系统时间
	uint32_t	n_sys_played_time_;		// 已经播放的时间
	uint32_t	n_played_time_;			// 已经播放的时间
	int64_t		decode_data_time_;		// 解码的数据开始时间(DTS)
	int64_t		last_aud_dts_time_;		// 记录上一次解码的音频时间戳
	int64_t		last_vid_dts_time_;		// 记录上一次解码的视频时间戳

	PlaySetting	play_setting_;

private:
	//* 数据缓存
	rtc::RecursiveCriticalSection	cs_audio_recv_;
	std::list<RecvPacket*>	lst_audio_recv_;
	rtc::RecursiveCriticalSection	cs_video_recv_;
	std::list<RecvPacket*>	lst_video_recv_;

private:
	//* 准备解码数据
	rtc::RecursiveCriticalSection	cs_audio_decode_;
	std::list<RecvPacket*>	lst_audio_decode_;
	rtc::RecursiveCriticalSection	cs_video_decode_;
	std::list<RecvPacket*>	lst_video_decode_;
};

#endif	// __FF_BUFFER_H__

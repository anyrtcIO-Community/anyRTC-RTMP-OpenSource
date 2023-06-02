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
#ifndef __JIT_BUFFER_H__
#define __JIT_BUFFER_H__
#include <stdint.h>
#include <string>
#include <list>
#include "rtc_base/deprecated/recursive_critical_section.h"

class JitBuffer
{
public:
	JitBuffer(void);
	virtual ~JitBuffer(void);

	void SetPlaySetting(bool bAuto, int nCacheTime, int nMinCacheTime, int nMaxCacheTime, int nVideoBlockThreshold);
	void ResetTime();

	void GetData();
	void DoClear();

	void CacheAudData(const uint8_t* pdata, int len, uint32_t ts, int seqn);
	void CacheVidData(const uint8_t* pdata, int len, uint32_t ts, bool bKeyframe);

	void ClearPcmCache(uint32_t ts);

public:
	enum PlayStatus
	{
		PS_Init = 0,	// 初始化状态
		PS_Caching,		// 缓冲中
		PS_Playing,		// 播放中
	};

	virtual void OnBufferDecodeVideoData(const uint8_t* pData, int len, bool bKeyframe, uint32_t ts) = 0;
	virtual void OnBufferDecodeAudioData(const uint8_t* pData, int len, int seqn, uint32_t ts) = 0;
	virtual void OnBufferStatusChanged(PlayStatus playStatus) = 0;
	virtual bool OnBufferGetPuased() = 0;

private:
	struct PlaySetting
	{
		bool bAuto;					//设置是否自动调整缓存时间。
		int nCacheTime;				//设置播放器缓存时间。
		int nMinCacheTime;			//设置最小的缓存时间。
		int nMaxCacheTime;			//设置最大的缓存时间。
		int nVideoBlockThreshold;	//设置播放器视频卡顿报警阈值。
	};

	PlayStatus	play_status_;			// 当前状态
	PlaySetting	play_setting_;

	bool		b_reset_time_;
	int32_t		n_cacheing_time_;		// 缓冲区时间，如果低于这个时间，就需要从网络库中读取
	int32_t     n_cache_to_play_time_;	// 缓冲播放时间，开始或卡顿后，必须达到这个时间才能重新播放
	int32_t		n_cache_to_play_max_;	// 最大的缓冲播放时间
	uint32_t	last_sys_time_;			// 上次调用的系统时间
	uint32_t	n_sys_played_time_;		// 已经播放的时间
	uint32_t	n_played_time_;			// 已经播放的时间
	int64_t		decode_data_time_;		// 解码的数据开始时间(DTS)
	int64_t		last_aud_dts_time_;		// 记录上一次解码的音频时间戳
	int64_t		last_vid_dts_time_;		// 记录上一次解码的视频时间戳

private:
	typedef struct PlyPacket
	{
		PlyPacket(bool isvideo) :_data(NULL), _data_len(0),
			_b_video(isvideo), _dts(0), _aud_seqn_(0), _b_vid_keyrame_(false), _duration(0){}

		virtual ~PlyPacket(void) {
			if (_data)
				delete[] _data;
		}
		void SetData(const uint8_t* pdata, int len, uint32_t ts) {
			_dts = ts;
			if (len > 0 && pdata != NULL) {
				if (_data)
					delete[] _data;
				if (_b_video)
					_data = new uint8_t[len + 8];
				else
					_data = new uint8_t[len];
				memcpy(_data, pdata, len);
				_data_len = len;
			}
		}
		uint8_t* _data;
		int _data_len;
		bool _b_video;
		uint32_t _dts;
		int _aud_seqn_;
		bool _b_vid_keyrame_;
		int _duration;
	}PlyPacket;

	rtc::RecursiveCriticalSection	cs_audio_recv_;
	std::list<PlyPacket*>	lst_audio_buffer_;
	rtc::RecursiveCriticalSection	cs_video_recv_;
	std::list<PlyPacket*>	lst_video_buffer_;
};

#endif	// __JIT_BUFFER_H__
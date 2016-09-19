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
#ifndef __PLAYER_BUFER_H__
#define __PLAYER_BUFER_H__
#include <list>
#include <stdint.h>
#include "webrtc/base/criticalsection.h"
#include "webrtc/base/messagehandler.h"
#include "webrtc/base/thread.h"

typedef struct PlyPacket
{
	PlyPacket(bool isvideo) :_data(NULL), _data_len(0),
		_b_video(isvideo), _dts(0) {}

	virtual ~PlyPacket(void){
		if (_data)
			delete[] _data;
	}
	void SetData(const uint8_t*pdata, int len, uint32_t ts) {
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
	uint8_t*_data;
	int _data_len;
	bool _b_video;
	uint32_t _dts;
}PlyPacket;

enum PlyStuts {
	PS_Fast = 0,	//	Fast video decode
	PS_Normal,
	PS_Cache,
};

class PlyBufferCallback {
public:
	PlyBufferCallback(void){};
	virtual ~PlyBufferCallback(void){};

	virtual void OnPlay() = 0;
	virtual void OnPause() = 0;
	virtual bool OnNeedDecodeData(PlyPacket* pkt) = 0;
};

class PlyBuffer : public rtc::MessageHandler
{
public:
	PlyBuffer(PlyBufferCallback&callback, rtc::Thread*worker);
	virtual ~PlyBuffer();

	void SetCacheSize(int miliseconds/*ms*/);
	int GetPlayAudio(void* audioSamples);
    PlyStuts PlayerStatus(){return ply_status_;};
    int GetPlayCacheTime(){return buf_cache_time_;};
	void CacheH264Data(const uint8_t*pdata, int len, uint32_t ts);
	void CachePcmData(const uint8_t*pdata, int len, uint32_t ts);

protected:
	//* For MessageHandler
	virtual void OnMessage(rtc::Message* msg);

	int	GetCacheTime();
	void DoDecode();

private:
	PlyBufferCallback		&callback_;
	bool					got_audio_;
	int						cache_time_;
	int						cache_delta_;
    int                     buf_cache_time_;
	PlyStuts				ply_status_;
	rtc::Thread				*worker_thread_;
	uint32_t				sys_fast_video_time_;	// Ãë¿ªÊ±¼äÖá
	uint32_t				rtmp_fast_video_time_;
	uint32_t				rtmp_cache_time_;
	uint32_t				play_cur_time_;
	rtc::CriticalSection	cs_list_audio_;
	std::list<PlyPacket*>	lst_audio_buffer_;
	rtc::CriticalSection	cs_list_video_;
	std::list<PlyPacket*>	lst_video_buffer_;
};

#endif	// __PLAYER_BUFER_H__
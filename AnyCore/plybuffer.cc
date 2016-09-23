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
#include "plybuffer.h"
#include "webrtc/base/logging.h"

#define PLY_MIN_TIME	500		// 0.5s
#define PLY_MAX_TIME	600000	// 10minute
#define PLY_RED_TIME	250		// redundancy time
#define PLY_MAX_DELAY	1000	// 1 second
#define PLY_MAX_CACHE   16      // 16s

#define PB_TICK	1011

PlyBuffer::PlyBuffer(PlyBufferCallback&callback, rtc::Thread*worker)
	: callback_(callback)
	, worker_thread_(NULL)
	, got_audio_(false)
	, cache_time_(1000)	// default 1000ms(1s)
	, cache_delta_(1)
    , buf_cache_time_(0)
	, ply_status_(PS_Fast)
	, sys_fast_video_time_(0)
	, rtmp_fast_video_time_(0)
	, rtmp_cache_time_(0)
	, play_cur_time_(0)
{
	ASSERT(worker != NULL);
	worker_thread_ = worker;
	worker_thread_->PostDelayed(RTC_FROM_HERE, 1, this, PB_TICK);
}


PlyBuffer::~PlyBuffer()
{
	std::list<PlyPacket*>::iterator iter = lst_audio_buffer_.begin();
	while (iter != lst_audio_buffer_.end()) {
		PlyPacket* pkt = *iter;
		lst_audio_buffer_.erase(iter++);
		delete pkt;
	}
	iter = lst_video_buffer_.begin();
	while (iter != lst_video_buffer_.end()) {
		PlyPacket* pkt = *iter;
		lst_video_buffer_.erase(iter++);
		delete pkt;
	}
}

void PlyBuffer::SetCacheSize(int miliseconds/*ms*/)
{
	if (miliseconds > 500 && miliseconds <= 600000) {	//* 0.5s ~ 10 minute
		cache_time_ = miliseconds;
	}
}
int PlyBuffer::GetPlayAudio(void* audioSamples)
{
	int ret = 0;
	rtc::CritScope cs(&cs_list_audio_);
	if (lst_audio_buffer_.size() > 0) {
		PlyPacket* pkt_front = lst_audio_buffer_.front();
		ret = pkt_front->_data_len;
		play_cur_time_ = pkt_front->_dts;
		memcpy(audioSamples, pkt_front->_data, pkt_front->_data_len);
		lst_audio_buffer_.pop_front();
		delete pkt_front;
	}

	return ret;
}
void PlyBuffer::CacheH264Data(const uint8_t*pdata, int len, uint32_t ts)
{
	PlyPacket* pkt = new PlyPacket(true);
	pkt->SetData(pdata, len, ts);
	if (sys_fast_video_time_ == 0)
	{
		sys_fast_video_time_ = rtc::Time();
		rtmp_fast_video_time_ = ts;
	}
	rtc::CritScope cs(&cs_list_video_);
	lst_video_buffer_.push_back(pkt);
}

void PlyBuffer::CachePcmData(const uint8_t*pdata, int len, uint32_t ts)
{
	PlyPacket* pkt = new PlyPacket(false);
	pkt->SetData(pdata, len, ts);
	rtc::CritScope cs(&cs_list_audio_);
	got_audio_ = true;
	lst_audio_buffer_.push_back(pkt);
	if (sys_fast_video_time_ == 0) {
		PlyPacket* pkt_front = lst_audio_buffer_.front();
		PlyPacket* pkt_back = lst_audio_buffer_.back();
		if ((pkt_back->_dts - pkt_front->_dts) >= PLY_MAX_DELAY) {
			sys_fast_video_time_ = rtc::Time();
			rtmp_fast_video_time_ = ts;
		}
	}
}

void PlyBuffer::OnMessage(rtc::Message* msg)
{
	if (msg->message_id == PB_TICK) {
		DoDecode();
		worker_thread_->PostDelayed(RTC_FROM_HERE, 5, this, PB_TICK);
	}
}

int	PlyBuffer::GetCacheTime()
{
	return cache_time_;
}

void PlyBuffer::DoDecode()
{
	uint32_t curTime = rtc::Time();
	if (sys_fast_video_time_ == 0)
		return;
	if (ply_status_ == PS_Fast) {
		PlyPacket* pkt = NULL;
		uint32_t videoSysGap = curTime - sys_fast_video_time_;
		uint32_t videoPlyTime = rtmp_fast_video_time_ + videoSysGap;
		if (videoSysGap >= PLY_RED_TIME) {
			//* Start play a/v
			rtc::CritScope cs(&cs_list_audio_);
			if (lst_audio_buffer_.size() > 0) {
				PlyPacket* pkt_front = lst_audio_buffer_.front();
				PlyPacket* pkt_back = lst_audio_buffer_.back();
				if ((pkt_back->_dts - pkt_front->_dts) > PLY_RED_TIME) {
					ply_status_ = PS_Normal;
					play_cur_time_ = pkt_front->_dts;
					callback_.OnPlay();
				}
			}
			else {
				if (videoSysGap >= PLY_RED_TIME * 4)
				{
					rtc::CritScope cs(&cs_list_video_);
					if (lst_video_buffer_.size() > 0) {
						PlyPacket* pkt_front = lst_video_buffer_.front();
						ply_status_ = PS_Normal;
						play_cur_time_ = pkt_front->_dts;
						callback_.OnPlay();
					}
				}
			}
		}
	}
	else if (ply_status_ == PS_Normal) {
		PlyPacket* pkt_video = NULL;
		uint32_t media_buf_time = 0;
		uint32_t play_video_time = play_cur_time_;
		{//* Get audio 
			rtc::CritScope cs(&cs_list_audio_);
			if (lst_audio_buffer_.size() > 0) {
				media_buf_time = lst_audio_buffer_.back()->_dts - lst_audio_buffer_.front()->_dts;
			}
		}
		if (media_buf_time == 0 && !got_audio_) {
			rtc::CritScope cs(&cs_list_video_);
			if (lst_video_buffer_.size() > 0) {
				media_buf_time = lst_video_buffer_.back()->_dts - lst_video_buffer_.front()->_dts;
				uint32_t videoSysGap = curTime - sys_fast_video_time_;
				play_video_time = rtmp_fast_video_time_ + videoSysGap;
			}
		}
	
		{//* Get video 
			rtc::CritScope cs(&cs_list_video_);
			if (lst_video_buffer_.size() > 0) {
				pkt_video = lst_video_buffer_.front();
				if (pkt_video->_dts <= play_video_time) {
					lst_video_buffer_.pop_front();
				}
				else {
					pkt_video = NULL;
				}
			}
		}

		if (pkt_video) {
			if (!callback_.OnNeedDecodeData(pkt_video)) {
				delete pkt_video;
			}
		}

		if (media_buf_time <= PLY_RED_TIME) {
			// Play buffer is so small, then we need buffer it?
			callback_.OnPause();
			ply_status_ = PS_Cache;
            cache_time_ = cache_delta_ * 1000;
            if(cache_delta_ < PLY_MAX_CACHE)
                cache_delta_ *= 2;
			rtmp_cache_time_ = rtc::Time() + cache_time_;
		}
        buf_cache_time_ = media_buf_time;
	}
	else if (ply_status_ == PS_Cache) {
		if (rtmp_cache_time_ <= rtc::Time()) {
			uint32_t media_buf_time = 0;
			{
				rtc::CritScope cs(&cs_list_audio_);
				if (lst_audio_buffer_.size() > 0) {
					media_buf_time = lst_audio_buffer_.back()->_dts - lst_audio_buffer_.front()->_dts;
				}
			}
			if (media_buf_time == 0 && !got_audio_) {
				rtc::CritScope cs(&cs_list_video_);
				if (lst_video_buffer_.size() > 0) {
					media_buf_time = lst_video_buffer_.back()->_dts - lst_video_buffer_.front()->_dts;
				}
			}

			if (media_buf_time >= cache_time_ - PLY_RED_TIME) {
				ply_status_ = PS_Normal;
				if (cache_delta_ == PLY_MAX_CACHE)
					cache_delta_ /= 2;
				callback_.OnPlay();
			}
			else {
				rtmp_cache_time_ = rtc::Time() + cache_time_;
			}
			if (!got_audio_) {
				sys_fast_video_time_ += cache_time_;
			}
			buf_cache_time_ = media_buf_time;
		}
	}
}
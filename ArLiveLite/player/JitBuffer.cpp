#include "JitBuffer.h"
#include "rtc_base/logging.h"
#include "rtc_base/time_utils.h"

#define DFT_CACHEING_TIME	10000	// 10senconds
#define MAX_CACHEING_TIME	60000	// 1mins

JitBuffer::JitBuffer(void)
	: play_status_(PS_Init)
	, b_reset_time_(false)
	, n_cacheing_time_(DFT_CACHEING_TIME)
	, n_cache_to_play_max_(1000)
	, n_cache_to_play_time_(1000)
	, last_sys_time_(0)
	, n_sys_played_time_(0)
	, n_played_time_(0)
	, decode_data_time_(0)
	, last_aud_dts_time_(0)
	, last_vid_dts_time_(0)
{

}
JitBuffer::~JitBuffer(void)
{
	DoClear();
}

void JitBuffer::SetPlaySetting(bool bAuto, int nCacheTime, int nMinCacheTime, int nMaxCacheTime, int nVideoBlockThreshold)
{
	play_setting_.bAuto = bAuto;
	play_setting_.nCacheTime = nCacheTime;
	play_setting_.nMinCacheTime = nMinCacheTime;
	play_setting_.nMaxCacheTime = nMaxCacheTime;
	play_setting_.nVideoBlockThreshold = nVideoBlockThreshold;

	if (bAuto) {
		n_cache_to_play_max_ = nCacheTime;
		n_cache_to_play_time_ = nMinCacheTime;
	}
	else {
		n_cache_to_play_max_ = nMaxCacheTime;
		n_cache_to_play_time_ = 1000;
	}
	if (n_cache_to_play_max_ > MAX_CACHEING_TIME) {
		n_cacheing_time_ = n_cache_to_play_max_;
	}
}
void JitBuffer::ResetTime()
{
	b_reset_time_ = true;
}

void JitBuffer::GetData()
{
	int vidCacheTime = 0;
	int audCacheTime = 0;
	int32_t sysTimeGap = 0;
	{// 计算每次处理的时间间隔
		if (last_sys_time_ == 0) {
			last_sys_time_ = rtc::Time32();
		}
		sysTimeGap = rtc::Time32() - last_sys_time_;
		last_sys_time_ = rtc::Time32();
	}

	if (play_status_ == PS_Init || play_status_ == PS_Caching || b_reset_time_) {
		int64_t vidFirstDtsTime = 0;
		int64_t audFirstDtsTime = 0;
		{
			rtc::CritScope cs(&cs_video_recv_);
			if (lst_video_buffer_.size() > 0) {
				vidCacheTime = lst_video_buffer_.back()->_dts - lst_video_buffer_.front()->_dts;
				vidFirstDtsTime = lst_video_buffer_.front()->_dts;
			}
		}
		{
			rtc::CritScope cs(&cs_audio_recv_);
			if (lst_audio_buffer_.size() > 0) {
				audCacheTime = lst_audio_buffer_.back()->_dts - lst_audio_buffer_.front()->_dts;
				audFirstDtsTime = lst_audio_buffer_.front()->_dts;
			}
		}
		//RTC_LOG(LS_INFO) << "DoTick audCacheTime: " << audCacheTime << " vidCacheTime: " << vidCacheTime;
		if (b_reset_time_) {
			b_reset_time_ = false;

			n_sys_played_time_ = 0;
			n_played_time_ = 0;
			if (audFirstDtsTime != 0) {
				decode_data_time_ = audFirstDtsTime;
			}
			else {
				decode_data_time_ = vidFirstDtsTime;
			}
		}
		else {
			int dataCacheTime = audCacheTime > vidCacheTime ? audCacheTime : vidCacheTime;
			if (play_setting_.bAuto) {
				if (dataCacheTime >= n_cache_to_play_time_) {
					n_sys_played_time_ = 0;
					n_played_time_ = 0;
					if (audFirstDtsTime != 0) {
						decode_data_time_ = audFirstDtsTime;
					}
					else {
						decode_data_time_ = vidFirstDtsTime;
					}
					if (play_status_ != PS_Init) {//* 第一次不需要回调
						OnBufferStatusChanged(PS_Playing);
					}
					play_status_ = PS_Playing;
					RTC_LOG(LS_INFO) << "FFBuffer playing...";

				}
			}
			else {
				if (dataCacheTime >= n_cache_to_play_time_) {
					n_sys_played_time_ = 0;
					n_played_time_ = 0;
					if (audFirstDtsTime != 0) {
						decode_data_time_ = audFirstDtsTime;
					}
					else {
						decode_data_time_ = vidFirstDtsTime;
					}
					if (play_status_ != PS_Init) {//* 第一次不需要回调
						OnBufferStatusChanged(PS_Playing);
					}
					play_status_ = PS_Playing;
				}
			}
		}
	}
	else {
		if (OnBufferGetPuased()) {
			// 如果播放暂停了，不需继续走播放流程
			return;
		}

		int vidListSize = 0;
		int audListSize = 0;
		uint32_t curTime = rtc::Time32();
		n_sys_played_time_ += sysTimeGap;
		n_played_time_ += sysTimeGap;
		int64_t decodeTime = decode_data_time_ + n_played_time_;
		std::list< PlyPacket*> vidList;
		std::list< PlyPacket*> audList;
		{
			rtc::CritScope cs(&cs_video_recv_);
			if (lst_video_buffer_.size() > 0) {
				vidListSize = lst_video_buffer_.size();
				if (lst_video_buffer_.size() > 0) {
					if (lst_video_buffer_.front()->_dts <= decodeTime) {
						vidList.push_back(lst_video_buffer_.front());
						lst_video_buffer_.pop_front();
						while (lst_video_buffer_.size() > 0) {
							if (lst_video_buffer_.front()->_dts <= decodeTime) {
								vidList.push_back(lst_video_buffer_.front());
								lst_video_buffer_.pop_front();
							}
							else {
								break;
							}
						}
					}
					else {
						if (last_vid_dts_time_ != 0 && lst_video_buffer_.front()->_duration > 0) {
							int64_t timeGap = lst_video_buffer_.front()->_dts - last_vid_dts_time_;
							if (timeGap > (lst_video_buffer_.front()->_duration * 2)) {
								b_reset_time_ = true;

								RTC_LOG(LS_INFO) << "FFBuffer restart lastVidDts: " << last_vid_dts_time_ << " frontDts: " << lst_video_buffer_.front()->_dts;
							}
						}
					}
				}
			}
		}
		{
			rtc::CritScope cs(&cs_audio_recv_);
			if (lst_audio_buffer_.size() > 0) {
				audListSize = lst_audio_buffer_.size();
				if (lst_audio_buffer_.size() > 0) {
					//RTC_LOG(LS_INFO) << "Frong pts: " << lst_audio_buffer_.front()->_dts << " decodeTime: " << decodeTime;
					if (lst_audio_buffer_.front()->_dts <= decodeTime) {
						audList.push_back(lst_audio_buffer_.front());
						lst_audio_buffer_.pop_front();
						while (lst_audio_buffer_.size() > 0) {
							if (lst_audio_buffer_.front()->_dts <= decodeTime) {
								audList.push_back(lst_audio_buffer_.front());
								lst_audio_buffer_.pop_front();
							}
							else {
								break;
							}
						}
					}
					else {
						if (last_aud_dts_time_ != 0 && lst_audio_buffer_.front()->_duration > 0) {
							int64_t timeGap = lst_audio_buffer_.front()->_dts - last_aud_dts_time_;
							if (timeGap > (lst_audio_buffer_.front()->_duration * 2)) {
								b_reset_time_ = true;

								RTC_LOG(LS_INFO) << "FFBuffer restart lastAudDts: " << last_aud_dts_time_ << " frontDts: " << lst_audio_buffer_.front()->_dts;
							}
						}
					}
				}
			}
		}

		while (audList.size() > 0) {
			PlyPacket* audPkt = audList.front();
			audList.pop_front();
			last_aud_dts_time_ = audPkt->_dts;
			OnBufferDecodeAudioData(audPkt->_data, audPkt->_data_len, audPkt->_aud_seqn_, audPkt->_dts);
			delete audPkt;
			audPkt = NULL;
		}

		while (vidList.size() > 0) {
			PlyPacket* vidPkt = vidList.front();
			vidList.pop_front();
			last_vid_dts_time_ = vidPkt->_dts;
			OnBufferDecodeVideoData(vidPkt->_data, vidPkt->_data_len, vidPkt->_b_vid_keyrame_, vidPkt->_dts);
			delete vidPkt;
			vidPkt = NULL;
		}


		if (vidListSize == 0 && audListSize == 0) {
			play_status_ = PS_Caching;
			OnBufferStatusChanged(play_status_);
			RTC_LOG(LS_INFO) << "FFBuffer caching...";
			if (play_setting_.bAuto) {
				if (n_cache_to_play_time_ < n_cache_to_play_max_) {
					n_cache_to_play_time_ += 1000;
				}
			}
			else {
				if (n_cache_to_play_time_ < n_cache_to_play_max_) {
					n_cache_to_play_time_ += 1000;
				}
			}
			if (n_cache_to_play_time_ > n_cache_to_play_max_) {
				n_cache_to_play_time_ = n_cache_to_play_max_;
			}
		}
		else {
			//RTC_LOG(LS_INFO) << "FFBuffer audSize: " << audListSize << " vidSize: " << vidListSize;
		}
	}
}

void JitBuffer::DoClear()
{
	{
		rtc::CritScope cs(&cs_audio_recv_);
		std::list<PlyPacket*>::iterator iter = lst_audio_buffer_.begin();
		while (iter != lst_audio_buffer_.end()) {
			PlyPacket* pkt = *iter;
			lst_audio_buffer_.erase(iter++);
			delete pkt;
		}
	}
	{
		rtc::CritScope cs(&cs_video_recv_);
		std::list<PlyPacket*>::iterator iter = lst_video_buffer_.begin();
		while (iter != lst_video_buffer_.end()) {
			PlyPacket* pkt = *iter;
			lst_video_buffer_.erase(iter++);
			delete pkt;
		}
	}
	play_status_ == PS_Init;
	n_sys_played_time_ = 0;
	n_played_time_ = 0;
	decode_data_time_ = 0;
	last_aud_dts_time_ = 0;
	last_vid_dts_time_ = 0;
	b_reset_time_ = true;		// 缓存清除之后，需要重置时间
}
void JitBuffer::CacheAudData(const uint8_t* pdata, int len, uint32_t ts, int seqn)
{
	PlyPacket* pkt = new PlyPacket(false);
	pkt->SetData(pdata, len, ts);
	pkt->_aud_seqn_ = seqn;
	rtc::CritScope cs(&cs_audio_recv_);
	lst_audio_buffer_.push_back(pkt);
}
void JitBuffer::CacheVidData(const uint8_t* pdata, int len, uint32_t ts, bool bKeyframe)
{
	uint32_t vts = 0;
	PlyPacket* pkt = new PlyPacket(true);
	pkt->SetData(pdata, len, ts);
	pkt->_b_vid_keyrame_ = bKeyframe;
	{
		rtc::CritScope cs(&cs_video_recv_);
		if (bKeyframe && lst_video_buffer_.size() > 5) {
			while (lst_video_buffer_.size() > 0) {
				vts = lst_video_buffer_.front()->_dts;
				delete lst_video_buffer_.front();
				lst_video_buffer_.pop_front();
			}
		}
		lst_video_buffer_.push_back(pkt);
	}

	if (vts != 0) {
		//ClearPcmCache(vts);	//音频会掉帧??
	}
}

void JitBuffer::ClearPcmCache(uint32_t ts)
{
	while (lst_audio_buffer_.size() > 0) {
		if (lst_audio_buffer_.front()->_dts <= ts) {
			delete lst_audio_buffer_.front();
			lst_audio_buffer_.pop_front();
		}
		else {
			break;
		}
	}
}
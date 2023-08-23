#include "FFBuffer.h"
#include "rtc_base/logging.h"
#include "rtc_base/time_utils.h"

#define DFT_CACHEING_TIME	10000	// 10senconds
#define MAX_CACHEING_TIME	60000	// 1mins

FFBuffer::FFBuffer()
	: play_status_(PS_Init)
	, play_mode_(PM_Fluent)
	, b_reset_time_(false)
	, b_need_keyframe_(true)
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
FFBuffer::~FFBuffer(void)
{
	DoClear();
}

void FFBuffer::DoClear()
{
	{
		rtc::CritScope cs(&cs_audio_recv_);
		std::list<RecvPacket*>::iterator iter = lst_audio_recv_.begin();
		while (iter != lst_audio_recv_.end()) {
			RecvPacket* pkt = *iter;
			lst_audio_recv_.erase(iter++);
			delete pkt;
		}
	}
	{
		rtc::CritScope cs(&cs_video_recv_);
		std::list<RecvPacket*>::iterator iter = lst_video_recv_.begin();
		while (iter != lst_video_recv_.end()) {
			RecvPacket* pkt = *iter;
			lst_video_recv_.erase(iter++);
			delete pkt;
		}
	}

	{
		rtc::CritScope cs(&cs_audio_decode_);
		std::list<RecvPacket*>::iterator iter = lst_audio_decode_.begin();
		while (iter != lst_audio_decode_.end()) {
			RecvPacket* pkt = *iter;
			lst_audio_decode_.erase(iter++);
			delete pkt;
		}
	}
	{
		rtc::CritScope cs(&cs_video_decode_);
		std::list<RecvPacket*>::iterator iter = lst_video_decode_.begin();
		while (iter != lst_video_decode_.end()) {
			RecvPacket* pkt = *iter;
			lst_video_decode_.erase(iter++);
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

void FFBuffer::DoTick()
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
			if (lst_video_recv_.size() > 0) {
				vidCacheTime = lst_video_recv_.back()->dts_ - lst_video_recv_.front()->dts_;
				vidFirstDtsTime = lst_video_recv_.front()->dts_;

				if (rtc::TimeUTCMillis() - lst_video_recv_.front()->recv_time_ >= n_cacheing_time_) {
					RTC_LOG(LS_NONE) << "RenderVideo cache pts: " << lst_video_recv_.front()->pts_ << " curTime: " << rtc::TimeMillis();
					vidCacheTime = n_cacheing_time_;
					rtc::CritScope l(&cs_video_decode_);
					lst_video_decode_.push_back(lst_video_recv_.front());
					lst_video_recv_.pop_front();
				}
			}
		}
		{
			rtc::CritScope cs(&cs_audio_recv_);
			if (lst_audio_recv_.size() > 0) {
				audCacheTime = lst_audio_recv_.back()->dts_ - lst_audio_recv_.front()->dts_;
				audFirstDtsTime = lst_audio_recv_.front()->dts_;

				if (rtc::TimeUTCMillis() - lst_audio_recv_.front()->recv_time_ >= n_cacheing_time_) {
					audCacheTime = n_cacheing_time_;
					rtc::CritScope l(&cs_audio_decode_);
					lst_audio_decode_.push_back(lst_audio_recv_.front());
					lst_audio_recv_.pop_front();
				}
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
		float plySpeed = OnBufferGetSpeed();
		uint32_t curTime = rtc::Time32();
		n_sys_played_time_ += sysTimeGap;
		n_played_time_ += sysTimeGap * plySpeed;
		int64_t decodeTime = decode_data_time_ + n_played_time_;
		std::list< RecvPacket*> vidList;
		std::list< RecvPacket*> audList;
		{
			rtc::CritScope cs(&cs_video_recv_);
			if (lst_video_recv_.size() > 0) {
				vidListSize = lst_video_recv_.size();
				if (lst_video_recv_.size() > 0) {
					if (lst_video_recv_.front()->dts_ <= decodeTime) {
						vidList.push_back(lst_video_recv_.front());
						lst_video_recv_.pop_front();
						while (lst_video_recv_.size() > 0) {
							if (lst_video_recv_.front()->dts_ <= decodeTime) {
								vidList.push_back(lst_video_recv_.front());
								lst_video_recv_.pop_front();
							}
							else {
								break;
							}
						}
					}
					else {
						if (last_vid_dts_time_ != 0 && lst_video_recv_.front()->duration_ > 0) {
							int64_t timeGap = lst_video_recv_.front()->dts_ - last_vid_dts_time_;
							if (timeGap > (lst_video_recv_.front()->duration_*2)) {
								b_reset_time_ = true;

								RTC_LOG(LS_INFO) << "FFBuffer restart lastVidDts: " << last_vid_dts_time_ << " frontDts: " << lst_video_recv_.front()->dts_;
							}
						}
					}
				}
			}
		}
		{
			rtc::CritScope cs(&cs_audio_recv_);
			if (lst_audio_recv_.size() > 0) {
				audListSize = lst_audio_recv_.size();
				if (lst_audio_recv_.size() > 0) {
					//RTC_LOG(LS_INFO) << "Frong pts: " << lst_audio_recv_.front()->dts_ << " decodeTime: " << decodeTime;
					if (lst_audio_recv_.front()->dts_ <= decodeTime) {
						audList.push_back(lst_audio_recv_.front());
						lst_audio_recv_.pop_front();
						while (lst_audio_recv_.size() > 0) {
							if (lst_audio_recv_.front()->dts_ <= decodeTime) {
								audList.push_back(lst_audio_recv_.front());
								lst_audio_recv_.pop_front();
							}
							else {
								break;
							}
						}
					}
					else {
						if (last_aud_dts_time_ != 0 && lst_audio_recv_.front()->duration_ > 0) {
							int64_t timeGap = lst_audio_recv_.front()->dts_ - last_aud_dts_time_;
							if (timeGap > (lst_audio_recv_.front()->duration_ * 2)) {
								b_reset_time_ = true;

								RTC_LOG(LS_INFO) << "FFBuffer restart lastAudDts: " << last_aud_dts_time_ << " frontDts: " << lst_audio_recv_.front()->dts_;
							}
						}
					}
				}
			}
		}

		while (audList.size() > 0) {
			RecvPacket* audPkt = audList.front();
			audList.pop_front();
			last_aud_dts_time_ = audPkt->dts_;
			{
				rtc::CritScope l(&cs_audio_decode_);
				lst_audio_decode_.push_back(audPkt);
			}
			audPkt = NULL;
		}

		while (vidList.size() > 0) {
			RecvPacket* vidPkt = vidList.front();
			vidList.pop_front();
			last_vid_dts_time_ = vidPkt->dts_;
			{
				rtc::CritScope l(&cs_video_decode_);
				lst_video_decode_.push_back(vidPkt);
			}
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

bool  FFBuffer::DoDecodeAudio()
{
	bool bRet = false;
	rtc::CritScope l(&cs_audio_decode_);
	if (lst_audio_decode_.size() > 0) {
		RecvPacket* audPkt = lst_audio_decode_.front();
		lst_audio_decode_.pop_front();

		OnBufferDecodeAudioData(audPkt->pkt_);
		delete audPkt;
		audPkt = NULL;
		bRet = true;
	}

	return bRet;
}
bool  FFBuffer::DoDecodeVideo(bool bBackground)
{
	bool bRet = false;
	rtc::CritScope l(&cs_video_decode_);
	if (lst_video_decode_.size() > 0) {
		RecvPacket* vidPkt = lst_video_decode_.front();
		lst_video_decode_.pop_front();
		
		if (bBackground) {
			b_need_keyframe_ = true;
		}
		else {
			if (b_need_keyframe_ && OnBufferIsKeyFrame(vidPkt->pkt_)) {
				b_need_keyframe_ = false;
			}
		}
	
		if (!b_need_keyframe_ && !bBackground) {
			OnBufferDecodeVideoData(vidPkt->pkt_);
		}
		delete vidPkt;
		vidPkt = NULL;
		bRet = true;
	}

	return bRet;
}

void FFBuffer::SetPlaySetting(bool bAuto, int nCacheTime, int nMinCacheTime, int nMaxCacheTime, int nVideoBlockThreshold)
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
void FFBuffer::ResetTime()
{
	b_reset_time_ = true;
}
bool FFBuffer::NeedMoreData()
{
	int vidCacheTime = 0;
	int audCacheTime = 0;
	{
		rtc::CritScope cs(&cs_video_recv_);
		if (lst_video_recv_.size() > 0) {
			vidCacheTime = lst_video_recv_.back()->dts_ - lst_video_recv_.front()->dts_;
		}
	}
	{
		rtc::CritScope cs(&cs_audio_recv_);
		if (lst_audio_recv_.size() > 0) {
			audCacheTime = lst_audio_recv_.back()->dts_ - lst_audio_recv_.front()->dts_;
		}
	}
	int dataCacheTime = audCacheTime > vidCacheTime ? audCacheTime : vidCacheTime;
	if (dataCacheTime <= n_cacheing_time_) {
		return true;
	}
	return false;
}
bool FFBuffer::IsPlaying()
{
	return play_status_ == PS_Playing;
}
bool FFBuffer::BufferIsEmpty()
{
	int vidListSize = 0;
	int audListSize = 0;
	{
		rtc::CritScope cs(&cs_video_recv_);
		vidListSize = lst_video_recv_.size();
	}
	{
		rtc::CritScope cs(&cs_audio_recv_);
		audListSize = lst_audio_recv_.size();
	}
	if (vidListSize == 0) {
		rtc::CritScope cs(&cs_video_decode_);
		vidListSize = lst_video_decode_.size();
	}
	if (audListSize == 0) {
		rtc::CritScope cs(&cs_audio_decode_);
		audListSize = lst_audio_decode_.size();
	}

	if (vidListSize == 0 && audListSize == 0) {
		return true;
	}

	return false;
}
int FFBuffer::AudioCacheTime()
{
	int audCacheTime = 0;	
	{
		rtc::CritScope cs(&cs_audio_recv_);
		if (lst_audio_recv_.size() > 0) {
			audCacheTime = lst_audio_recv_.back()->dts_ - lst_audio_recv_.front()->dts_;
		}
	}
	return audCacheTime;
}
int FFBuffer::VideoCacheTime()
{
	int vidCacheTime = 0;
	{
		rtc::CritScope cs(&cs_video_recv_);
		if (lst_video_recv_.size() > 0) {
			vidCacheTime = lst_video_recv_.back()->dts_ - lst_video_recv_.front()->dts_;
		}
	}
	return vidCacheTime;
}
void FFBuffer::RecvVideoData(AVPacket* pkt, int64_t dts, int64_t pts, int64_t duration)
{
	//RTC_LOG(LS_INFO) << "RecvVideoData: len " << pkt->size << " dts: " << dts << " pts: " << pts << " duration: " << duration;
	//RTC_LOG(LS_INFO) << "RecvVideoData duration: " << duration;
	RecvPacket* recvPkt = new RecvPacket();
	recvPkt->pkt_ = pkt;
	recvPkt->dts_ = dts;
	recvPkt->pts_ = pts;
	recvPkt->duration_ = duration;
	recvPkt->recv_time_ = rtc::TimeUTCMillis();

	rtc::CritScope cs(&cs_video_recv_);
	lst_video_recv_.push_back(recvPkt);
}
void FFBuffer::RecvAudioData(AVPacket* pkt, int64_t dts, int64_t pts, int64_t duration)
{
#if 0
	static int64_t gDts = 0;
	if (dts - gDts > 200) {
		RTC_LOG(LS_INFO) << "Last dts: " << gDts << " dts: " << dts;
	}
	gDts = dts;
#endif
	//RTC_LOG(LS_INFO) << "RecvAudioData duration: " << duration;
	//RTC_LOG(LS_INFO) << "RecvAudioData dts: " << dts << " pts: " << pts << " duration: " << duration << " curTime: " << rtc::Time32();
	RecvPacket* recvPkt = new RecvPacket();
	recvPkt->pkt_ = pkt;
	recvPkt->dts_ = dts;
	recvPkt->pts_ = pts;
	recvPkt->duration_ = duration;
	recvPkt->recv_time_ = rtc::TimeUTCMillis();

	rtc::CritScope cs(&cs_audio_recv_);
	lst_audio_recv_.push_back(recvPkt);
}


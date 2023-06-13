#include "PlayBuffer.h"
#include "rtc_base/logging.h"
#include "rtc_base/time_utils.h"

static const size_t kMaxDataSizeSamples = 3840;
#define MAX_INT 32767
#define MIN_INT -32767
#define CHECK_MAX_VALUE(value) ((value > 32767) ? 32767 : value)
#define CHECK_MIN_VALUE(value) ((value < -32767) ? -32767 : value)
static int MixAudio(int iChannelNum, short* sourceData1, short* sourceData2, float fBackgroundGain, float fWordsGain, short *outputData)
{
	double f = 1.0;
	if (iChannelNum <= 0) {
		return -1;
	}
	if (iChannelNum > 2) {
		return -2;
	}

	if (iChannelNum == 2) {
		float fLeftValue1 = 0;
		float fRightValue1 = 0;
		float fLeftValue2 = 0;
		float fRightValue2 = 0;
		float fLeftValue = 0;
		float fRightValue = 0;
		int output = 0;
		int iIndex = 0;

		fLeftValue1 = (float)(sourceData1[0]);
		fRightValue1 = (float)(sourceData1[1]);
		fLeftValue2 = (float)(sourceData2[0]);
		fRightValue2 = (float)(sourceData2[1]);
		fLeftValue1 = fLeftValue1 * fBackgroundGain;
		fRightValue1 = fRightValue1 * fBackgroundGain;
		fLeftValue2 = fLeftValue2 * fWordsGain;
		fRightValue2 = fRightValue2 * fWordsGain;
		fLeftValue1 = CHECK_MAX_VALUE(fLeftValue1);
		fLeftValue1 = CHECK_MIN_VALUE(fLeftValue1);
		fRightValue1 = CHECK_MAX_VALUE(fRightValue1);
		fRightValue1 = CHECK_MIN_VALUE(fRightValue1);
		fLeftValue2 = CHECK_MAX_VALUE(fLeftValue2);
		fLeftValue2 = CHECK_MIN_VALUE(fLeftValue2);
		fRightValue2 = CHECK_MAX_VALUE(fRightValue2);
		fRightValue2 = CHECK_MIN_VALUE(fRightValue2);
		fLeftValue = fLeftValue1 + fLeftValue2;
		fRightValue = fRightValue1 + fRightValue2;

		for (iIndex = 0; iIndex < 2; iIndex++) {

			if (iIndex == 0) {
				output = (int)(fLeftValue*f);
			}
			else {
				output = (int)(fRightValue*f);
			}
			if (output > MAX_INT)
			{
				f = (double)MAX_INT / (double)(output);
				output = MAX_INT;
			}
			if (output < MIN_INT)
			{
				f = (double)MIN_INT / (double)(output);
				output = MIN_INT;
			}
			if (f < 1)
			{
				f += ((double)1 - f) / (double)32;
			}
			outputData[iIndex] = (short)output;
		}
	}
	else {
		float fValue1 = 0;
		float fValue2 = 0;
		float fValue = 0;

		fValue1 = (float)(*(short*)(sourceData1));
		fValue2 = (float)(*(short*)(sourceData2));
		fValue1 = fValue1 * fBackgroundGain;
		fValue2 = fValue2 * fWordsGain;
		fValue = fValue1 + fValue2;

		fValue = CHECK_MAX_VALUE(fValue);
		fValue = CHECK_MIN_VALUE(fValue);
		*outputData = (short)fValue;
	}
	return 1;
}

#ifdef WEBRTC_ANDROID
//android 的某些机型，音频播放的线程实时性不高，所以一次性需要更多的数据
const int kMaxAudioPlaySize = 30;
#else
const int kMaxAudioPlaySize = 20;
#endif
const int kMaxVedeoPlaySize = kMaxAudioPlaySize / 2;


PlayBuffer::PlayBuffer(void)
	: aud_data_resamp_(NULL)
	, aud_data_mix_(NULL)
	, b_video_decoded_(false)
	, b_audio_decoded_(false)
	, b_app_in_background_(false)
	, n_last_render_video_time_(0)
	, n_last_render_video_pts_(0)
{
	aud_data_resamp_ = new char[kMaxDataSizeSamples];
	memset(aud_data_resamp_, 0, kMaxDataSizeSamples);
	aud_data_mix_ = new char[kMaxDataSizeSamples];
	memset(aud_data_mix_, 0, kMaxDataSizeSamples);
}
PlayBuffer::~PlayBuffer(void)
{
	delete[] aud_data_resamp_;
	delete[] aud_data_mix_;

	DoClear();
}

int PlayBuffer::DoVidRender(bool bVideoPaused)
{
	bool bRender = false;
	while (1) {
		VideoData* vidPkt = NULL;
		{
			rtc::CritScope cs(&cs_video_play_);
			if (lst_video_play_.size() > 0) {
				vidPkt = lst_video_play_.front();
				if (n_last_render_video_pts_ == 0 || n_last_render_video_pts_ > vidPkt->pts_) {
					n_last_render_video_time_ = rtc::TimeUTCMillis();
					n_last_render_video_pts_ = vidPkt->pts_;
				}
				if (vidPkt->pts_ <= (rtc::TimeUTCMillis() - n_last_render_video_time_) + n_last_render_video_pts_) {
					lst_video_play_.pop_front();
				}
				else {
					vidPkt = NULL;
				}
			}
		}

		if (vidPkt != NULL) {
			//RTC_LOG(LS_INFO) << "DoRender video pts: " << vidPkt->pts_ << " plytime: " << play_pts_time_;
			if (!bVideoPaused && !b_app_in_background_) {
				if (!bRender) {//@Eric - 跳帧处理，防止一次性输出过多
					OnBufferVideoRender(vidPkt, vidPkt->pts_);
				}
				bRender = true;
			}
			delete vidPkt;
			vidPkt = NULL;
		}
		else {
			break;
		}
	}

	return 0;
}
int PlayBuffer::DoAudRender(bool mix, void* audioSamples, uint32_t samplesPerSec, int nChannels, bool bAudioPaused)
{
	int ret = 0;
	PcmData* audPkt = NULL;
	
	{//*
		rtc::CritScope cs(&cs_audio_play_);
		//RTC_LOG(LS_INFO) << "Audio list size: " << lst_audio_play_.size();

		if (lst_audio_play_.size() > 0) {
			audPkt = lst_audio_play_.front();
			lst_audio_play_.pop_front();

		}

	}
	
	if (audPkt != NULL) {
		if (!bAudioPaused) {
			ret = 1;
			int a_frame_size = samplesPerSec * nChannels * sizeof(int16_t) / 100;

			if (samplesPerSec != audPkt->sample_hz_ || audPkt->channels_ != nChannels) {
				resampler_.Resample10Msec((int16_t*)audPkt->pdata_, audPkt->sample_hz_ * audPkt->channels_,
					samplesPerSec * nChannels, 1, kMaxDataSizeSamples, (int16_t*)aud_data_resamp_);
			}
			else {
				memcpy(aud_data_resamp_, audPkt->pdata_, a_frame_size);
			}
			if (!mix) {
				memcpy(audioSamples, aud_data_resamp_, a_frame_size);
			}
			else {
				float voice_gain = 1.0;
				float musice_gain = 1.0;
				short* pMusicUnit = (short*)aud_data_resamp_;
				short* pMicUnit = (short*)audioSamples;
				short* pOutputPcm = (short*)aud_data_mix_;
				for (int iIndex = 0; iIndex < audPkt->len_; iIndex = iIndex + nChannels) {
					MixAudio(nChannels, &pMusicUnit[iIndex], &pMicUnit[iIndex], musice_gain, voice_gain, &pOutputPcm[iIndex]);
				}
				memcpy(audioSamples, pOutputPcm, a_frame_size);
			}
		}
		{
			n_last_render_video_time_ = rtc::TimeUTCMillis();
			n_last_render_video_pts_ = audPkt->pts_;
		}
		delete audPkt;
		audPkt = NULL;
	}
	else {
		RTC_LOG(LS_INFO) << "* No audio data time: " << rtc::Time32(); 
	}

	return ret;
}

int PlayBuffer::DoRender(bool mix, void* audioSamples, uint32_t samplesPerSec, int nChannels, bool bAudioPaused, bool bVideoPaused)
{
	int ret = 0;
	PcmData* audPkt = NULL;
	VideoData* vidPkt = NULL;
	{//*
		rtc::CritScope cs(&cs_audio_play_);
		//RTC_LOG(LS_INFO) << "Audio list size: " << lst_audio_play_.size();
		if (lst_audio_play_.size() > 0) {
			audPkt = lst_audio_play_.front();
			lst_audio_play_.pop_front();
		}

	}
	{
		rtc::CritScope cs(&cs_video_play_);
		if (lst_video_play_.size() > 0) {
			vidPkt = lst_video_play_.front();
			lst_video_play_.pop_front();
		}
	}

	if (audPkt != NULL) {
		if (!bAudioPaused) {
			ret = 1;
			int a_frame_size = samplesPerSec * nChannels * sizeof(int16_t) / 100;

			if (samplesPerSec != audPkt->sample_hz_ || audPkt->channels_ != nChannels) {
				resampler_.Resample10Msec((int16_t*)audPkt->pdata_, audPkt->sample_hz_ * audPkt->channels_,
					samplesPerSec * nChannels, 1, kMaxDataSizeSamples, (int16_t*)aud_data_resamp_);
			}
			else {
				memcpy(aud_data_resamp_, audPkt->pdata_, a_frame_size);
			}
			if (!mix) {
				memcpy(audioSamples, aud_data_resamp_, a_frame_size);
			}
			else {
				float voice_gain = 1.0;
				float musice_gain = 1.0;
				short* pMusicUnit = (short*)aud_data_resamp_;
				short* pMicUnit = (short*)audioSamples;
				short* pOutputPcm = (short*)aud_data_mix_;
				for (int iIndex = 0; iIndex < audPkt->len_; iIndex = iIndex + nChannels) {
					MixAudio(nChannels, &pMusicUnit[iIndex], &pMicUnit[iIndex], musice_gain, voice_gain, &pOutputPcm[iIndex]);
				}
				memcpy(audioSamples, pOutputPcm, a_frame_size);
			}
		}
		delete audPkt;
		audPkt = NULL;
	}
	else {
		//RTC_LOG(LS_INFO) << "* No audio data time: " << rtc::Time32();
	}

	if (vidPkt != NULL) {
		//RTC_LOG(LS_INFO) << "DoRender video pts: " << vidPkt->pts_ << " plytime: " << play_pts_time_;
		if (!bVideoPaused && !b_app_in_background_) {
			OnBufferVideoRender(vidPkt, vidPkt->pts_);
		}
		delete vidPkt;
		vidPkt = NULL;
	}

	return ret;
}

void PlayBuffer::DoClear()
{
	{
		rtc::CritScope cs(&cs_audio_play_);
		std::list<PcmData*>::iterator iter = lst_audio_play_.begin();
		while (iter != lst_audio_play_.end()) {
			PcmData* pkt = *iter;
			lst_audio_play_.erase(iter++);
			delete pkt;
		}
	}
	{
		rtc::CritScope cs(&cs_video_play_);
		std::list<VideoData*>::iterator iter = lst_video_play_.begin();
		while (iter != lst_video_play_.end()) {
			VideoData* pkt = *iter;
			lst_video_play_.erase(iter++);
			delete pkt;
		}
	}
}

void PlayBuffer::SetAppInBackground(bool bBackground)
{
	b_app_in_background_ = bBackground;
}

bool PlayBuffer::NeedMoreAudioPlyData()
{
	rtc::CritScope cs(&cs_audio_play_);
	return lst_audio_play_.size() <= kMaxAudioPlaySize;
}

bool PlayBuffer::NeedMoreVideoPlyData()
{
	rtc::CritScope cs(&cs_video_play_);
	return lst_video_play_.size() <= kMaxVedeoPlaySize;
}
bool PlayBuffer::AppIsBackground()
{
	return b_app_in_background_;
}
void PlayBuffer::PlayVideoData(VideoData* videoData)
{
	//RTC_LOG(LS_INFO) << "PlayVideoData pts: " << videoData->pts_;
	if (!b_video_decoded_) {
		b_video_decoded_ = true;
		OnFirstVideoDecoded();
	}
	rtc::CritScope cs(&cs_video_play_);
	if (lst_video_play_.size() > 0) {
		if (lst_video_play_.front()->pts_ >= videoData->pts_) {
			lst_video_play_.push_front(videoData);
		}
		else if (lst_video_play_.back()->pts_ <= videoData->pts_) {
			lst_video_play_.push_back(videoData);
		}
		else {
			std::list<VideoData*>::iterator iter = lst_video_play_.begin();
			while (iter != lst_video_play_.end()) {
				if ((*iter)->pts_ >= videoData->pts_) {
					lst_video_play_.insert(iter, videoData);
					break;
				}
				iter++;
			}
		}
	}
	else {
#if 0
		while (lst_video_play_.size() > (kMaxVedeoPlaySize << 1)) {
			VideoData* pkt = lst_video_play_.front();
			lst_video_play_.pop_front();
			delete pkt;

			OnBufferVideoDropped();
		}
#endif
		lst_video_play_.push_back(videoData);
	}

#if 0
	//@Eric - 跳帧处理 - 防止缓存太多：内存报警，渲染延时增大
	while (lst_video_play_.size() > 1) {
		int64_t timeGap = lst_video_play_.back()->pts_ - lst_video_play_.front()->pts_;
		if (timeGap < (kMaxVideoPlaySize << 1) * 10) {
			break;
		}
		VideoData* pkt = lst_video_play_.front();
		lst_video_play_.pop_front();
		delete pkt;

		OnBufferVideoDropped();
	}
#endif
}
void PlayBuffer::PlayAudioData(PcmData*pcmData)
{
	//RTC_LOG(LS_INFO) << "PlayAudioData pts: " << pcmData->pts_;
	if (!b_audio_decoded_) {
		b_audio_decoded_ = true;
		OnFirstAudioDecoded();
	}

	int64_t dropPts = -1;
	{
		rtc::CritScope cs(&cs_audio_play_);
		lst_audio_play_.push_back(pcmData);

		//@Eric - 跳帧处理 - 防止缓存太多：延时增大
		if (lst_audio_play_.size() >= kMaxAudioPlaySize) {//清一半缓存
			while (lst_audio_play_.size() > kMaxAudioPlaySize/2) {
				PcmData* pkt = lst_audio_play_.front();
				dropPts = pkt->pts_;
				lst_audio_play_.pop_front();
				delete pkt;

				OnBufferAudioDropped();
			}
		}
	}

#if 0
	if (b_app_in_background_) {
		if (lst_audio_play_.size() > 0) {
			PcmData* pkt = lst_audio_play_.front();
			dropPts = pkt->pts_;
		}
	}
#endif

	if (dropPts != -1) {
		//@Eric - 跳帧处理 - 防止缓存太多：内存报警，渲染延时增大
		while (lst_video_play_.size() > 1) {
			if (lst_video_play_.front()->pts_ > dropPts) {
				break;
			}
			VideoData* vidPkt = lst_video_play_.front();
			lst_video_play_.pop_front();
			delete vidPkt;

			OnBufferVideoDropped();
		}
	}

	//RTC_LOG(LS_INFO) << "PlayAudioData list size: " << lst_audio_play_.size();
}




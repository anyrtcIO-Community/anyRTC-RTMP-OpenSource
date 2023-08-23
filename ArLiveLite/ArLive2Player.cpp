#include "ArLive2Player.h"
#include "libyuv/include/libyuv.h"
#include "api/video/video_frame.h"
#include "rtc_base/bind.h"
#include "rtc_base/time_utils.h"

static const size_t kMaxDataSizeSamples = 3840;


//=========================================================
ArLive2Player::ArLive2Player(ArLive2Engine*pEngine, const std::string& strPlayId)
	: ar_engine_(pEngine)
	, observer_(NULL)
	, main_thread_(pEngine->GetThread())
	, ar_player_(NULL)
	, b_play_ok_(false)
	, b_shutdown_(false)
	, b_audio_paused_(false)
	, b_video_paused_(false)
	, e_play_mode_(AR::ArLivePlayModeLive)
	, n_sei_payload_type_(0)
	, n_rgba_width_(0)
	, n_rgba_height_(0)
	, p_rgba_data_(NULL)
	, b_snap_shot_(false)
	, frameRotation(AR::ArLiveRotation0)
{
	str_local_play_id_ = strPlayId;
	ar_engine_->RegisteRtcTick(this, this);
}
ArLive2Player::~ArLive2Player(void)
{
	ar_engine_->UnRegisteRtcTick(this);
	if (ar_player_ != NULL) {
		ar_engine_->DetachAudSpeaker(this);
		ar_player_->StopTask();
		delete ar_player_;
		ar_player_ = NULL;
	}
	{
		AR::VideoCanvas vidCanvas;
		vidCanvas.view = NULL;
		vidCanvas.uid = str_local_play_id_.c_str();
		ar_engine_->setVideoRenderView(str_local_play_id_.c_str(), vidCanvas);
	}

	delete[] p_rgba_data_;
}

//* For AR::IArLivePlayer
void ArLive2Player::setObserver(AR::ArLivePlayerObserver* observer)
{
	observer_ = observer;
}
int32_t ArLive2Player::setRenderView(void* view)
{
    AR::VideoCanvas vidCanvas;
	vidCanvas.view = view;
	vidCanvas.uid = str_local_play_id_.c_str();
	ar_engine_->setVideoRenderView(str_local_play_id_.c_str(), vidCanvas);
	return AR::ArLIVE_OK;
}
int32_t ArLive2Player::setRenderRotation(AR::ArLiveRotation rotation)
{
	frameRotation = rotation;
//	ar_engine_->setVideoRenderRotation(str_local_play_id_.c_str(), rotation);
	return AR::ArLIVE_OK;
}
int32_t ArLive2Player::setRenderFillMode(AR::ArLiveFillMode mode)
{
	ar_engine_->setVideoRenderFillMode(str_local_play_id_.c_str(), mode);
	return AR::ArLIVE_OK;
}
int32_t ArLive2Player::setPlayMode(AR::ArLivePlayMode mode)
{
	if (ar_player_ != NULL) {
		return AR::ArLIVE_ERROR_REFUSED;
	}
	e_play_mode_ = mode;
	return AR::ArLIVE_OK;
}
int32_t ArLive2Player::startPlay(const char* strPlayUrl)
{
	if (!main_thread_->IsCurrent()) {
		return main_thread_->Invoke<int>(RTC_FROM_HERE, rtc::Bind(&ArLive2Player::startPlay, this, strPlayUrl));
	}
	if (strPlayUrl == NULL || strlen(strPlayUrl) == 0) {
		return -1;
	}

	if (ar_player_ == NULL) {
        ar_player_ = createARPlayer(*this);
		ar_player_->Config(ar_play_conf_.bAuto, ar_play_conf_.nCacheTime * 1000, ar_play_conf_.nMinCacheTime * 1000, ar_play_conf_.nMaxCacheTime * 1000, ar_play_conf_.nVideoBlockThreshold);
		ar_player_->SetUseTcp(1);
		ar_player_->SetAudioEnable(!b_audio_paused_);
		ar_player_->SetVideoEnable(!b_video_paused_);
		ar_player_->StartTask(strPlayUrl);

		ar_engine_->RegisteRtcTick(this, this);
		ar_engine_->AttachAudSpeaker(this);
	}
	return 0;
}
int32_t ArLive2Player::stopPlay()
{
	bool isNeedClearLastImg = true;
	if (!main_thread_->IsCurrent()) {
		return main_thread_->Invoke<int>(RTC_FROM_HERE, rtc::Bind(&ArLive2Player::stopPlay, this));
	}

	if (ar_player_ != NULL) {
		ar_engine_->UnRegisteRtcTick(this);
		ar_engine_->DetachAudSpeaker(this);
		ar_player_->StopTask();
		delete ar_player_;
		ar_player_ = NULL;
	}

	b_snap_shot_ = false;
	PlayBuffer::DoClear();

	return AR::ArLIVE_OK;
}
int32_t ArLive2Player::isPlaying()
{
	return !b_audio_paused_ || !b_video_paused_;
}
int32_t ArLive2Player::seekTo(int seekTimeS)
{
	if (!main_thread_->IsCurrent()) {
		return main_thread_->Invoke<int>(RTC_FROM_HERE, rtc::Bind(&ArLive2Player::seekTo, this, seekTimeS));
	}
	if (ar_player_ == NULL) {
		return AR::ArLIVE_ERROR_REFUSED;
	}
	ar_player_->SeekTo(seekTimeS);
	return AR::ArLIVE_OK;
}

int32_t ArLive2Player::setSpeed(float speed)
{
	if (!main_thread_->IsCurrent()) {
		return main_thread_->Invoke<int>(RTC_FROM_HERE, rtc::Bind(&ArLive2Player::setSpeed, this, speed));
	}
	if (ar_player_ == NULL) {
		return AR::ArLIVE_ERROR_REFUSED;
	}
	if (speed != 0.5 && speed != 0.75 && speed != 1.0 && speed != 1.25 && speed != 1.5 && speed != 1.75 && speed != 2.0 && speed != 3.0) {
		return AR::ArLIVE_ERROR_INVALID_PARAMETER;
	}
	ar_player_->SetSpeed(speed);
	return AR::ArLIVE_OK;
}
int32_t ArLive2Player::rePlay()
{
	if (!main_thread_->IsCurrent()) {
		return main_thread_->Invoke<int>(RTC_FROM_HERE, rtc::Bind(&ArLive2Player::rePlay, this));
	}
	if (ar_player_ == NULL) {
		return AR::ArLIVE_ERROR_REFUSED;
	}
	ar_player_->RePlay();
	return AR::ArLIVE_OK;
}
int32_t ArLive2Player::pauseAudio()
{
	if (!main_thread_->IsCurrent()) {
		return main_thread_->Invoke<int>(RTC_FROM_HERE, rtc::Bind(&ArLive2Player::pauseAudio, this));
	}
	b_audio_paused_ = true;
	if (ar_player_ != NULL) {
		ar_player_->SetAudioEnable(!b_audio_paused_);
	}
	return 0;
}
int32_t ArLive2Player::resumeAudio()
{
	if (!main_thread_->IsCurrent()) {
		return main_thread_->Invoke<int>(RTC_FROM_HERE, rtc::Bind(&ArLive2Player::resumeAudio, this));
	}
	b_audio_paused_ = false;
	if (ar_player_ != NULL) {
		ar_player_->SetAudioEnable(!b_audio_paused_);
	}
	return 0;
}
int32_t ArLive2Player::pauseVideo()
{
	if (!main_thread_->IsCurrent()) {
		return main_thread_->Invoke<int>(RTC_FROM_HERE, rtc::Bind(&ArLive2Player::pauseVideo, this));
	}
	b_video_paused_ = true;
	if (ar_player_ != NULL) {
		ar_player_->SetVideoEnable(!b_video_paused_);
	}
	return 0;
}
int32_t ArLive2Player::resumeVideo()
{
	if (!main_thread_->IsCurrent()) {
		return main_thread_->Invoke<int>(RTC_FROM_HERE, rtc::Bind(&ArLive2Player::resumeVideo, this));
	}
	b_video_paused_ = false;
	if (ar_player_ != NULL) {
		ar_player_->SetVideoEnable(!b_video_paused_);
	}
	return 0;
}
int32_t ArLive2Player::setPlayoutVolume(int32_t volume)
{
	if (!main_thread_->IsCurrent()) {
		return main_thread_->Invoke<int>(RTC_FROM_HERE, rtc::Bind(&ArLive2Player::setPlayoutVolume, this, volume));
	}
	float fVolume = (float)volume / 100.0;
	if (ar_player_ != NULL) {
		ar_player_->SetVolume(fVolume);
	}
	return 0;
}
int32_t ArLive2Player::setCacheParams(float minTime, float maxTime)
{
	if (!main_thread_->IsCurrent()) {
		return main_thread_->Invoke<int>(RTC_FROM_HERE, rtc::Bind(&ArLive2Player::setCacheParams, this, minTime, maxTime));
	}
	if ((minTime < 0.0 || maxTime < 0.0) || minTime > maxTime)
	{
		return AR::ArLIVE_ERROR_INVALID_PARAMETER;
	}
	if (ar_player_ != NULL) {
		return AR::ArLIVE_ERROR_REFUSED;
	}
	ar_play_conf_.nMinCacheTime = minTime;
	ar_play_conf_.nMaxCacheTime = maxTime;
	ar_play_conf_.nCacheTime = minTime;
	
	return AR::ArLIVE_OK;
}
int32_t ArLive2Player::enableVolumeEvaluation(int32_t intervalMs)
{
	if (!main_thread_->IsCurrent()) {
		return main_thread_->Invoke<int>(RTC_FROM_HERE, rtc::Bind(&ArLive2Player::enableVolumeEvaluation, this, intervalMs));
	}
	if (intervalMs % 100 != 0) {
		return AR::ArLIVE_ERROR_INVALID_PARAMETER;
	}
	if (ar_player_ == NULL) {
		return AR::ArLIVE_ERROR_REFUSED;
	}

	if (intervalMs <= 0) {
		ar_player_->EnableVolumeEvaluation(0);
	}
	else if (intervalMs >= 100) {
		ar_player_->EnableVolumeEvaluation(intervalMs);
	}

	return 0;
}
int32_t ArLive2Player::snapshot()
{
	if (!main_thread_->IsCurrent()) {
		return main_thread_->Invoke<int>(RTC_FROM_HERE, rtc::Bind(&ArLive2Player::snapshot, this));
	}
	if (ar_player_ == NULL) {
		return AR::ArLIVE_ERROR_REFUSED;
	}

	b_snap_shot_ = true;

	return AR::ArLIVE_OK;
}
int32_t ArLive2Player::enableCustomRendering(bool enable, AR::ArLivePixelFormat pixelFormat, AR::ArLiveBufferType bufferType)
{
	if (!main_thread_->IsCurrent()) {
		return main_thread_->Invoke<int>(RTC_FROM_HERE, rtc::Bind(&ArLive2Player::enableCustomRendering, this, enable, pixelFormat, bufferType));
	}
	if (enable && (pixelFormat == AR::ArLivePixelFormatUnknown || bufferType == AR::ArLiveBufferTypeUnknown)) {
		return AR::ArLIVE_ERROR_INVALID_PARAMETER;
	}
	custom_render_.bEnable = enable;
	custom_render_.ePixelFormat = pixelFormat;
	custom_render_.eBufferType = bufferType;
	return 0;
}
int32_t ArLive2Player::enableReceiveSeiMessage(bool enable, int payloadType)
{
	if (!main_thread_->IsCurrent()) {
		return main_thread_->Invoke<int>(RTC_FROM_HERE, rtc::Bind(&ArLive2Player::enableReceiveSeiMessage, this, enable, payloadType));
	}

	if (enable) {
		n_sei_payload_type_ = payloadType;
	}
	else {
		n_sei_payload_type_ = 0;
	}
	return 0;
}
void ArLive2Player::showDebugView(bool isShow)
{
}

//* For RtcTick
void ArLive2Player::OnTick()
{
	//play_buffer_.DoTick();
	if (ar_player_ != NULL) {
		ar_player_->RunOnce();
	}

	PlayBuffer::DoVidRender(b_video_paused_);
}
void ArLive2Player::OnTickUnAttach()
{

}
//* For PlayBuffer
void ArLive2Player::OnBufferVideoRender(VideoData *videoData, int64_t pts)
{
	if (videoData->video_frame_ != nullptr) {
		if (custom_render_.bEnable) {
            AR::ArLiveVideoFrame videFrame;
			videFrame.pixelFormat = custom_render_.ePixelFormat;
			videFrame.bufferType = custom_render_.eBufferType;
			videFrame.width = videoData->video_frame_->width();
			videFrame.height = videoData->video_frame_->height();

			if (custom_render_.ePixelFormat == AR::ArLivePixelFormatI420) {
				if (observer_ != NULL) {
					videFrame.data = (char*)videoData->video_frame_->GetI420()->DataY();
					videFrame.length = videFrame.width * videFrame.height * 3 / 2;
					observer_->onRenderVideoFrame(this, &videFrame);
				}
			}
			else if (custom_render_.ePixelFormat == AR::ArLivePixelFormatBGRA32) {
				if (p_rgba_data_ == NULL || n_rgba_width_ != videoData->video_frame_->width() || n_rgba_height_ != videoData->video_frame_->height()) {
					n_rgba_width_ = videoData->video_frame_->width();
					n_rgba_height_ = videoData->video_frame_->height();
					if (p_rgba_data_ != NULL) {
						delete[] p_rgba_data_;
						p_rgba_data_ = NULL;
					}
					p_rgba_data_ = new char[n_rgba_width_* n_rgba_height_ * 4];
				}

				const webrtc::I420BufferInterface* srcYuv = videoData->video_frame_->GetI420();
				libyuv::I420ToABGR(srcYuv->DataY(), srcYuv->StrideY(), srcYuv->DataU(), srcYuv->StrideU(), srcYuv->DataV(), srcYuv->StrideV(), (uint8_t*)p_rgba_data_, n_rgba_width_ * 4, n_rgba_width_, n_rgba_height_);

				videFrame.data = (char*)p_rgba_data_;
				videFrame.length = videFrame.width * videFrame.height * 4;
				observer_->onRenderVideoFrame(this, &videFrame);

				if (b_snap_shot_) {
					b_snap_shot_ = false;
					observer_->onSnapshotComplete(this, videFrame.data, videFrame.length, videFrame.width, videFrame.height, AR::ArLivePixelFormatBGRA32);
				}
			}
		}
		else {
			webrtc::VideoRotation videoRotation;
			switch (frameRotation) {
                case AR::ArLiveRotation0:
					videoRotation = webrtc::VideoRotation::kVideoRotation_0;
					break;
                case AR::ArLiveRotation90:
					videoRotation = webrtc::VideoRotation::kVideoRotation_90;
					break;
                case AR::ArLiveRotation180:
					videoRotation = webrtc::VideoRotation::kVideoRotation_180;
					break;
                case AR::ArLiveRotation270:
					videoRotation = webrtc::VideoRotation::kVideoRotation_270;
					break;
				default:
					videoRotation = webrtc::VideoRotation::kVideoRotation_0;
			}
			const webrtc::VideoFrame render_frame(videoData->video_frame_, videoRotation, rtc::TimeMicros());
			ar_engine_->GetMgrRender().DoRenderFrame(str_local_play_id_.c_str(), render_frame);
		}
	}

	if (b_snap_shot_) {
		b_snap_shot_ = false;
		const webrtc::VideoFrame frame(videoData->video_frame_, webrtc::VideoRotation::kVideoRotation_0, rtc::TimeMicros());
		char* dst_frame = new char[frame.width() * frame.height() * 4];
		const webrtc::I420BufferInterface* i420_buffer =
			frame.video_frame_buffer()->GetI420();
		libyuv::I420ToABGR(i420_buffer->DataY(), i420_buffer->StrideY(), i420_buffer->DataU(), i420_buffer->StrideU(), i420_buffer->DataV(), i420_buffer->StrideV(), (uint8_t*)dst_frame, frame.width() * 4, frame.width(), frame.height());
		observer_->onSnapshotComplete(this, dst_frame, frame.width() * frame.height() * 4, frame.width(), frame.height(), AR::ArLivePixelFormatBGRA32);
		delete[] dst_frame;
	}
}
void ArLive2Player::OnFirstVideoDecoded()
{
#if 0
	if (listener_live_play_ != NULL) {
		listener_live_play_->onPlayFirstVideoFrameDecoded();
	}
#endif
}
void ArLive2Player::OnFirstAudioDecoded()
{
#if 0
	if (listener_live_play_ != NULL) {
		listener_live_play_->onPlayFirstAudioFrameDecoded();
	}
#endif
}
//* For AudDevSpeakerEvent
int ArLive2Player::MixAudioData(bool mix, void* audioSamples, uint32_t samplesPerSec, int nChannels)
{
	if (b_audio_paused_ && b_video_paused_) {
		if (e_play_mode_ == AR::ArLivePlayModeLive) {//@Eric - 如果是直播，保持播放进度
			char pMixData[1920];
			PlayBuffer::DoAudRender(false, pMixData, samplesPerSec, nChannels, true);
		}
		return 0;
	}
	return PlayBuffer::DoAudRender(mix, audioSamples, samplesPerSec, nChannels, b_audio_paused_);
}

//* For ARPlayerEvent
void ArLive2Player::OnArPlyOK(void*player) 
{
	if (observer_ != NULL) {
		observer_->onAudioPlayStatusUpdate(this, AR::ArLivePlayStatusPlaying, AR::ArLiveStatusChangeReasonLocalStarted, NULL);
		observer_->onVideoPlayStatusUpdate(this, AR::ArLivePlayStatusPlaying, AR::ArLiveStatusChangeReasonLocalStarted, NULL);
	}
};
void ArLive2Player::OnArPlyPlaying(void* player, bool hasAudio, bool hasVideo)
{
	if (observer_ != NULL) {
		if (hasAudio) {
			observer_->onAudioPlayStatusUpdate(this, AR::ArLivePlayStatusPlaying, AR::ArLiveStatusChangeReasonBufferingEnd, NULL);
		}
		if (hasVideo) {
			observer_->onVideoPlayStatusUpdate(this, AR::ArLivePlayStatusPlaying, AR::ArLiveStatusChangeReasonBufferingEnd, NULL);
		}
	}
}
void ArLive2Player::OnArPlyCacheing(void* player, bool hasAudio, bool hasVideo)
{
	if (observer_ != NULL) {
		if (hasAudio) {
			observer_->onAudioPlayStatusUpdate(this, AR::ArLivePlayStatusLoading, AR::ArLiveStatusChangeReasonBufferingBegin, NULL);
		}
		if (hasVideo) {
			observer_->onVideoPlayStatusUpdate(this, AR::ArLivePlayStatusLoading, AR::ArLiveStatusChangeReasonBufferingBegin, NULL);
		}
	}
}
void ArLive2Player::OnArPlyClose(void*player, int errcode) 
{
	if (errcode == 0) {
		if (observer_ != NULL) {
			observer_->onAudioPlayStatusUpdate(this, AR::ArLivePlayStatusStopped, AR::ArLiveStatusChangeReasonLocalStopped, NULL);
			observer_->onVideoPlayStatusUpdate(this, AR::ArLivePlayStatusStopped, AR::ArLiveStatusChangeReasonLocalStopped, NULL);
		}
	}
	else {
		if (observer_ != NULL) {
			observer_->onAudioPlayStatusUpdate(this, AR::ArLivePlayStatusStopped, AR::ArLiveStatusChangeReasonInternal, NULL);
			observer_->onVideoPlayStatusUpdate(this, AR::ArLivePlayStatusStopped, AR::ArLiveStatusChangeReasonInternal, NULL);
		}
	}
}
void ArLive2Player::OnArPlyVolumeUpdate(void* player, int32_t volume)
{
	if (observer_ != NULL) {
		observer_->onPlayoutVolumeUpdate(this, volume);
	}
}
void ArLive2Player::OnArPlyVodProcess(void* player, int nAllTimeMs, int nCurTimeMs, int nCacheTimeMs)
{
	if (observer_ != NULL) {
		observer_->onVodPlaybackProcess(this, nAllTimeMs, nCurTimeMs, nCacheTimeMs);
	}
}
void ArLive2Player::OnArPlyStatistics(void* player, int nWidth, int nHeight, int nFps, int nAudBitrate, int nVidBitrate)
{
	if (observer_ != NULL) {
        AR::ArLivePlayerStatistics statistics;
		statistics.width = nWidth;
		statistics.height = nHeight;
		statistics.fps = nFps;
		statistics.audioBitrate = nAudBitrate;
		statistics.videoBitrate = nVidBitrate;
		observer_->onStatisticsUpdate(this, statistics);
	}
}

bool ArLive2Player::OnArPlyNeedMoreAudioData(void*player)
{
	return PlayBuffer::NeedMoreAudioPlyData();
}
bool ArLive2Player::OnArPlyNeedMoreVideoData(void*player)
{
	return PlayBuffer::NeedMoreVideoPlyData();
}
bool ArLive2Player::OnArPlyAppIsBackground(void* player)
{
	return PlayBuffer::AppIsBackground();
}

bool ArLive2Player::OnArPlyIsLiveMode(void* player)
{
	return e_play_mode_ == AR::ArLivePlayModeLive;
}
void ArLive2Player::OnArPlyAudio(void*player, const char*pData, int nSampleHz, int nChannels, int64_t pts)
{
	PcmData *audioData = new PcmData((char*)pData, (nSampleHz / 100)*nChannels * sizeof(short), nSampleHz, nChannels);
	audioData->pts_ = pts;
	PlayBuffer::PlayAudioData(audioData);
};
void ArLive2Player::OnArPlyVideo(void*player, int fmt, int ww, int hh, uint8_t**pData, int*linesize, int64_t pts)
{
	rtc::CritScope l(&cs_render_frame_);
	if (i420_render_frame_ == NULL || (i420_render_frame_->width() != ww || i420_render_frame_->height() != hh)) {
		i420_render_frame_ = webrtc::I420Buffer::Create(ww, hh);
	}

	uint8_t* dataY = (uint8_t*)pData[0];
	uint8_t* dataU = (uint8_t*)pData[1];
	uint8_t* dataV = (uint8_t*)pData[2];
	libyuv::I420Copy(dataY, linesize[0], dataU, linesize[1], dataV, linesize[2],
		(uint8_t*)i420_render_frame_->DataY(), i420_render_frame_->StrideY(), (uint8_t*)i420_render_frame_->DataU(), i420_render_frame_->StrideU(),
		(uint8_t*)i420_render_frame_->DataV(), i420_render_frame_->StrideV(), i420_render_frame_->width(), i420_render_frame_->height());

	VideoData *videoData = new VideoData();
	videoData->video_frame_ = i420_render_frame_;
	videoData->pts_ = pts;
	PlayBuffer::PlayVideoData(videoData);
};

void ArLive2Player::OnArPlySeiData(void* player, const char* pData, int nLen, int64_t pts)
{
	int semiLen = 4;
	int nType = pData[4] & 0x1f;
	if (pData[2] == 1) {
		nType = pData[3] & 0x1f;
		semiLen = 3;
	}
	if (nType == 6) {
		int nPayloadType = pData[semiLen + 1] & 0xff;
		if (nPayloadType == 5 || nPayloadType == 242) {// self defined
			int nPayloadLen = 0;
			const char* sei = pData + semiLen + 2;
			//数据长度
			int sl = 0;
			do {
				sl = (*sei) & 0xff;
				sei++;
				nPayloadLen += sl;
			} while (sl == 255);

			if (observer_ != NULL) {
				observer_->onReceiveSeiMessage(this, nPayloadType, (uint8_t*)sei, nPayloadLen);
			}
		}
		//return;	//@Eric - bug 加密包解码花屏
	}
}


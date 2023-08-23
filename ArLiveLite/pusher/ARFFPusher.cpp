#include "ARFFPusher.h"
#include "webrtc/rtc_base/bind.h"
#include "webrtc/rtc_base/time_utils.h"

ARPusher* V2_CALL createARPusher()
{
	return new ARFFPusher();
}

ARFFPusher::ARFFPusher()
	: b_pushed_(false)
	, b_connected_(false)
	, b_need_keyframe_(true)
	, n_retry_times_(0)
	, ar_writer_(NULL)
	, n_push_time_(0)
{
	main_thread_ = rtc::Thread::Current();
}
ARFFPusher::~ARFFPusher(void)
{
	if (ar_writer_ != NULL) {
		ar_writer_->StopTask();
		delete ar_writer_;
		ar_writer_ = NULL;
	}
}

//* For ARPusher
int ARFFPusher::startTask(const char* strUrl)
{
	if (!main_thread_->IsCurrent()) {
		return main_thread_->Invoke<int>(RTC_FROM_HERE, rtc::Bind(&ARFFPusher::startTask, this, strUrl));
	}
	RTC_CHECK(main_thread_->IsCurrent());
	RTC_CHECK(strUrl != NULL && strlen(strUrl) > 0);
	str_rtsp_url_ = strUrl;
	if (!b_pushed_) {
		b_pushed_ = true;
		b_need_keyframe_ = true;
		n_retry_times_ = 0;
		webrtc::MutexLock l(&cs_ar_writer_);
		if (ar_writer_ == NULL) {
			ar_writer_ = createArWriter();
			ar_writer_->SetCallback(this);
			ar_writer_->SetAutoRetry(true);
			if (strstr(strUrl, "rtmp://") != NULL) {
				ar_writer_->StartTask("flv", strUrl);
			}
			else if (strstr(strUrl, "rtp://") != NULL) {
				ar_writer_->StartTask("rtp_mpegts", strUrl);
			}
			else if (strstr(strUrl, "rtsp://") != NULL) {
				ar_writer_->StartTask("rtsp", strUrl);
				ar_writer_->SetEncType(CT_H264, CT_G711A);
			}
		}
	}
	return 0;
};
int ARFFPusher::stopTask()
{
	if (!main_thread_->IsCurrent()) {
		return main_thread_->Invoke<int>(RTC_FROM_HERE, rtc::Bind(&ARFFPusher::stopTask, this));
	}
	if (b_pushed_) {
		b_pushed_ = false;
		b_connected_ = false;

		webrtc::MutexLock l(&cs_ar_writer_);
		if (ar_writer_ != NULL) {
			ar_writer_->StopTask();
			delete ar_writer_;
			ar_writer_ = NULL;
		}
	}
	return 0;
};

int ARFFPusher::runOnce()
{
	RTC_CHECK(main_thread_->IsCurrent());

	webrtc::MutexLock l(&cs_ar_writer_);
	if (ar_writer_ != NULL) {
		ar_writer_->RunOnce();
	}
	return 0;
};
int ARFFPusher::setAudioEnable(bool bEnable)
{
	if (!main_thread_->IsCurrent()) {
		return main_thread_->Invoke<int>(RTC_FROM_HERE, rtc::Bind(&ARFFPusher::setAudioEnable, this, bEnable));
	}
	return 0; 
};
int ARFFPusher::setVideoEnable(bool bEnable)
{
	if (!main_thread_->IsCurrent()) {
		return main_thread_->Invoke<int>(RTC_FROM_HERE, rtc::Bind(&ARFFPusher::setVideoEnable, this, bEnable));
	}
	return 0;
};
int ARFFPusher::setRepeat(bool bEnable)
{
	if (!main_thread_->IsCurrent()) {
		return main_thread_->Invoke<int>(RTC_FROM_HERE, rtc::Bind(&ARFFPusher::setRepeat, this, bEnable));
	}
	return 0;
};
int ARFFPusher::setRetryCountDelay(int nCount, int nDelay)
{
	if (!main_thread_->IsCurrent()) {
		return main_thread_->Invoke<int>(RTC_FROM_HERE, rtc::Bind(&ARFFPusher::setRetryCountDelay, this, nCount, nDelay));
	}
	return 0;
};
int ARFFPusher::setAudioData(const char* pData, int nLen, uint32_t ts)
{
	//RTC_CHECK(main_thread_->IsCurrent());
	if (!b_need_keyframe_) {
		webrtc::MutexLock l(&cs_ar_writer_);
		if (ar_writer_ != NULL) {
			int64_t pts = rtc::TimeUTCMillis() - n_push_time_;
			ar_writer_->SetAudioEncData(pData, nLen, pts, pts);
		}
	}
	return 0;
};
int ARFFPusher::setVideoData(const char* pData, int nLen, bool bKeyFrame, uint32_t ts)
{
	//RTC_CHECK(main_thread_->IsCurrent());
	if (b_need_keyframe_) {
		if (bKeyFrame) {
			b_need_keyframe_ = false;
		}
	}
	if (!b_need_keyframe_) {
		webrtc::MutexLock l(&cs_ar_writer_);
		if (ar_writer_ != NULL) {
			int64_t pts = rtc::TimeUTCMillis() - n_push_time_;
			ar_writer_->SetVideoEncData(pData, nLen, bKeyFrame, pts, pts);
		}
	}
	return 0;
};
int ARFFPusher::setSeiData(const char* pData, int nLen, uint32_t ts)
{
	//RTC_CHECK(main_thread_->IsCurrent());
	return 0;
};

//* For ArWriterEvent
void ARFFPusher::OnArWriterStateChange(ArWriterState oldState, ArWriterState newState)
{
	if (newState == WS_Connecting) {
		if (n_retry_times_ == 0) {
			if (callback_ != NULL) {
				callback_->onPushStatusUpdate(AR::ArLivePushStatusConnecting, NULL, NULL);
			}
		}
		else {
			if (callback_ != NULL) {
				callback_->onPushStatusUpdate(AR::ArLivePushStatusReconnecting, NULL, NULL);
			}
		}
		n_retry_times_++;
	}
	else if (newState == WS_Connected) {
		n_push_time_ = rtc::TimeUTCMillis();
		b_need_keyframe_ = true;
		if (callback_ != NULL) {
			callback_->onPushStatusUpdate(AR::ArLivePushStatusConnectSuccess, NULL, NULL);
		}
	}
	else if (newState == WS_Failed) {
		
	}
	else if (newState == WS_Ended) {
		if (callback_ != NULL) {
			callback_->onPushStatusUpdate(AR::ArLivePushStatusDisconnected, NULL, NULL);
		}
	}
}
void ARFFPusher::OnArWriterStats(const char* strJSonDetail)
{

}

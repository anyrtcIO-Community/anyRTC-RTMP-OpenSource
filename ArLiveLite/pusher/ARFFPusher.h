#ifndef __AR_FF_PUSHER_H__
#define __AR_FF_PUSHER_H__
#include "ARPusher.h"
#include "rtc_base/synchronization/mutex.h"
#include "rtc_base/thread.h"
#include "ArWriter.h"

class ARFFPusher : public ARPusher, public ArWriterEvent
{
public:
	ARFFPusher();
	virtual ~ARFFPusher(void);

	//* For ARPusher
	virtual int startTask(const char* strUrl);
	virtual int stopTask();

	virtual int runOnce();
	virtual int setAudioEnable(bool bAudioEnable);
	virtual int setVideoEnable(bool bVideoEnable);
	virtual int setRepeat(bool bEnable);
	virtual int setRetryCountDelay(int nCount, int nDelay);
	virtual int setAudioData(const char* pData, int nLen, uint32_t ts);
	virtual int setVideoData(const char* pData, int nLen, bool bKeyFrame, uint32_t ts);
	virtual int setSeiData(const char* pData, int nLen, uint32_t ts);

	//* For ArWriterEvent
	virtual void OnArWriterStateChange(ArWriterState oldState, ArWriterState newState);
	virtual void OnArWriterStats(const char* strJSonDetail);

private:
	rtc::Thread* main_thread_;
	bool b_pushed_;
	bool b_connected_;
	bool b_need_keyframe_;
	int n_retry_times_;	

	std::string str_rtsp_url_;

private:
	webrtc::Mutex cs_ar_writer_;
	ArWriter* ar_writer_;
	int64_t		n_push_time_;
};

#endif	//__AR_FF_PUSHER_H__
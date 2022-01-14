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
#ifndef __ARP_RTMP_PUSHER_H__
#define __ARP_RTMP_PUSHER_H__
#include "ARPusher.h"
#include "ArNetClient.h"
#include "rtmp-client.h"
#include "flv-muxer.h"
#include "flv-proto.h"
#include "rtc_base/thread.h"

class ARRtmpPusher : public ARPusher, public INetClientEvent
{
public:
	ARRtmpPusher();
	virtual ~ARRtmpPusher(void);

	//* For ARPusher
	virtual int startTask(const char* strUrl);
	virtual int stopTask();

	virtual int runOnce();
	virtual int setAudioEnable(bool bAudioEnable) { return 0; };
	virtual int setVideoEnable(bool bVideoEnable) { return 0; };
	virtual int setRepeat(bool bEnable);
	virtual int setRetryCountDelay(int nCount, int nDelay);
	virtual int setAudioData(const char* pData, int nLen, uint32_t ts);
	virtual int setVideoData(const char* pData, int nLen, bool bKeyFrame, uint32_t ts);
	virtual int setSeiData(const char* pData, int nLen, uint32_t ts);

	//* For ArNetClientEvent
	virtual void OnArClientConnected();
	virtual void OnArClientConnectFailure();
	virtual void OnArClientDisconnect();
	virtual void OnArClientSent(int err);
	virtual void OnArClientRecv(const char* pData, int nLen);

public:
	void on_flv_muxer_data(int type, const void* data, size_t bytes, uint32_t timestamp);
	int do_rtmp_client_send(const void* header, size_t len, const void* payload, size_t bytes);
	int do_rtmp_client_onaudio(const void* audio, size_t bytes, uint32_t timestamp) { return 0; };
	int do_rtmp_client_onvideo(const void* video, size_t bytes, uint32_t timestamp) { return 0; };
	int do_rtmp_client_onscript(const void* script, size_t bytes, uint32_t timestamp) { return 0; };

private:
	rtc::Thread* main_thread_;

	std::string str_rtmp_url_;

	INetClient*		ar_net_client_;
	rtmp_client_t*	rtmp_client_;
	bool			b_rtmp_connected_;
	bool			b_vid_need_keyframe_;
	uint32_t		n_flv_start_time_;
	flv_muxer_t*	flv_muxer_;

	int				n_set_retry_count_;
	int				n_set_retry_delay_;
	int				n_retry_count_;
	int64_t			n_retry_time_;

private:
	struct FlvData
	{
		FlvData(void) : pData(NULL), nType(0), nLen(0), nTimestamp(0) {

		}
		virtual ~FlvData(void) {
			if (pData != NULL) {
				delete[] pData;
				pData = NULL;
			}
		}
		void SetData(const char* data, int len) {
			if (pData == NULL) {
				nLen = len;
				pData = new char[len];
				memcpy(pData, data, len);
			}
		}
		char* pData;
		int nType;
		int nLen;
		uint32_t nTimestamp;
	};

	webrtc::Mutex cs_send_data_;
	std::list<std::unique_ptr<FlvData>> lst_send_data_;
};

#endif  // __ARP_RTMP_PUSHER_H__
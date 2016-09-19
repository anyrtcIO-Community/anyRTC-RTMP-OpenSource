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
#ifndef __ANY_RTMP_PLAYER_H__
#define __ANY_RTMP_PLAYER_H__
#include "anyrtmplayer_interface.h"
#include "anyrtmpull.h"
#include "plydecoder.h"
#include "webrtc/base/messagehandler.h"
#include "webrtc/api/mediastreaminterface.h"

namespace webrtc {
class AnyRtmplayerImpl : public AnyRtmplayer, public rtc::Thread, public rtc::MessageHandler
		, public AnyRtmpPullCallback
{
public:
	AnyRtmplayerImpl(AnyRtmplayerEvent&callback);
	virtual ~AnyRtmplayerImpl(void);

	virtual void StartPlay(const char* url);
	virtual void SetVideoRender(void* handle);
	virtual void StopPlay();

	int GetNeedPlayAudio(void* audioSamples, uint32_t& samplesPerSec, size_t& nChannels);

protected:
	//* For MessageHandler
	virtual void OnMessage(rtc::Message* msg);

	//* For AnyRtmpPullCallback
	virtual void OnRtmpullConnected();
	virtual void OnRtmpullFailed();
	virtual void OnRtmpullDisconnect();
	virtual void OnRtmpullH264Data(const uint8_t*pdata, int len, uint32_t ts);
	virtual void OnRtmpullAACData(const uint8_t*pdata, int len, uint32_t ts);

private:
	AnyRtmpPull			*rtmp_pull_;
	PlyDecoder			*ply_decoder_;
    int                 cur_bitrate_;
	std::string			str_url_;

	rtc::VideoSinkInterface < cricket::VideoFrame > *video_renderer_;
};

}	// namespace webrtc

#endif	// __ANY_RTMP_PLAYER_H__
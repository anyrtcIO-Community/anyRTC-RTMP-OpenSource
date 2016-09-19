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
#include "anyrtmplayer.h"
#include "anyrtmpcore.h"
#include "srs_librtmp.h"
#include "webrtc/base/logging.h"
#include "webrtc/media/base/videoframe.h"
#include "webrtc/modules/audio_device/audio_device_impl.h"
#include "webrtc/voice_engine/voice_engine_defines.h"

#define PLY_START	1001
#define PLY_STOP	1002
#define PLY_TICK    1003

AnyRtmplayer* AnyRtmplayer::Create(AnyRtmplayerEvent&callback)
{
	return new webrtc::AnyRtmplayerImpl(callback);
}
namespace webrtc {

AnyRtmplayerImpl::AnyRtmplayerImpl(AnyRtmplayerEvent&callback)
	: AnyRtmplayer(callback)
	, rtmp_pull_(NULL)
	, ply_decoder_(NULL)
    , cur_bitrate_(0)
	, video_renderer_(NULL)
{
	rtc::Thread::Start();

	AnyRtmpCore::Inst();
}

AnyRtmplayerImpl::~AnyRtmplayerImpl(void)
{
	rtc::Thread::Stop();

	if (rtmp_pull_) {
		delete rtmp_pull_;
		rtmp_pull_ = NULL;
	}

	if (ply_decoder_) {
		delete ply_decoder_;
		ply_decoder_ = NULL;
	}
}

void AnyRtmplayerImpl::StartPlay(const char* url)
{
	str_url_ = url;
	rtc::Thread::Post(RTC_FROM_HERE, this, PLY_START);

    rtc::Thread::PostDelayed(RTC_FROM_HERE, 1000, this, PLY_TICK);
}

void AnyRtmplayerImpl::SetVideoRender(void* handle)
{
	video_renderer_ = (rtc::VideoSinkInterface < cricket::VideoFrame >	*) handle;
}

void AnyRtmplayerImpl::StopPlay()
{
    rtc::Thread::Clear(this, PLY_TICK);
	rtc::Thread::Post(RTC_FROM_HERE, this, PLY_STOP);
    callback_.OnRtmplayerClose(0);
}

void AnyRtmplayerImpl::OnMessage(rtc::Message* msg)
{
	switch (msg->message_id) {
	case PLY_START: {
		if (ply_decoder_ == NULL) {
			ply_decoder_ = new PlyDecoder();
			if (video_renderer_)
				ply_decoder_->SetVideoRender(video_renderer_);
		}
		if (rtmp_pull_ == NULL) {
			rtmp_pull_ = new AnyRtmpPull(*this, str_url_);
		}
		
	}
		break;
	case PLY_STOP: {
		if (rtmp_pull_) {
			delete rtmp_pull_;
			rtmp_pull_ = NULL;
		}
		if (ply_decoder_) {
			delete ply_decoder_;
			ply_decoder_ = NULL;
		}
	}
		break;
    case PLY_TICK: {
        if (ply_decoder_) {
            if (ply_decoder_->IsPlaying()) {
                callback_.OnRtmplayerStatus(ply_decoder_->CacheTime(), cur_bitrate_);
                cur_bitrate_ = 0;
            } else {
                callback_.OnRtmplayerCache(ply_decoder_->CacheTime());
            }
        }
        rtc::Thread::PostDelayed(RTC_FROM_HERE, 1000, this, PLY_TICK);
    }
        break;
	}
}

void AnyRtmplayerImpl::OnRtmpullConnected()
{
	callback_.OnRtmplayerOK();
}

void AnyRtmplayerImpl::OnRtmpullFailed()
{
	StopPlay();
	callback_.OnRtmplayerClose(-1);
}

void AnyRtmplayerImpl::OnRtmpullDisconnect()
{
	StopPlay();
	callback_.OnRtmplayerClose(-2);
}

void AnyRtmplayerImpl::OnRtmpullH264Data(const uint8_t*pdata, int len, uint32_t ts)
{
	if (ply_decoder_) {
		ply_decoder_->AddH264Data(pdata, len, ts);
	}
    cur_bitrate_ += len;
}

void AnyRtmplayerImpl::OnRtmpullAACData(const uint8_t*pdata, int len, uint32_t ts)
{
	if (ply_decoder_) {
		ply_decoder_->AddAACData(pdata, len, ts);
	}
    cur_bitrate_ += len;
}

int AnyRtmplayerImpl::GetNeedPlayAudio(void* audioSamples, uint32_t& samplesPerSec, size_t& nChannels)
{
	if (ply_decoder_) {
		return ply_decoder_->GetPcmData(audioSamples, samplesPerSec, nChannels);
	}
	return 0;
}

}	// namespace webrtc
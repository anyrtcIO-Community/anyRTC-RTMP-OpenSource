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
#include "anyrtmpstreamer.h"
#include "webrtc/media/base/videocommon.h"
#include "webrtc/media/engine/webrtcvideocapturerfactory.h"
#include "webrtc/modules/audio_device/audio_device_impl.h"

AnyRtmpstreamer* AnyRtmpstreamer::Create(AnyRtmpstreamerEvent&callback)
{
	return new webrtc::AnyRtmpStreamerImpl(callback);
}
namespace webrtc {
AnyRtmpStreamerImpl::AnyRtmpStreamerImpl(AnyRtmpstreamerEvent&callback)
: callback_(callback)
, rtmp_connected_(false)
, auto_adjust_bit_(false)
, a_aac_encoder_(NULL)
, a_muted_(false)
, a_enabled_(false)
, a_sample_hz_(44100)
, a_channels_(1)
, v_h264_encoder_(NULL)
, v_width(640)
, v_height(480)
, v_framerate_(20)
, v_bitrate_(768)
, av_rtmp_(NULL)
{
	AnyRtmpCore::Inst();

	{
		int bitpersample = 16;
		a_aac_encoder_ = new A_AACEncoder(*this);
		a_aac_encoder_->Init(a_channels_, a_sample_hz_, bitpersample);
		a_aac_encoder_->Muted(a_muted_);
	}
	{
		v_h264_encoder_ = new V_H264Encoder(*this);
		v_h264_encoder_->Init(AnyRtmpCore::Inst().ExternalVideoEncoderFactory());
		v_h264_encoder_->SetParameter(v_width, v_height, v_framerate_, v_bitrate_);
	}
}
AnyRtmpStreamerImpl::~AnyRtmpStreamerImpl(void)
{
	if(a_aac_encoder_)
	{
		delete a_aac_encoder_;
		a_aac_encoder_ = NULL;
	}
	if(v_h264_encoder_)
	{
		delete v_h264_encoder_;
		v_h264_encoder_ = NULL;
	}
	if(av_rtmp_)
	{
		delete av_rtmp_;
		av_rtmp_ = NULL;
	}
}

void AnyRtmpStreamerImpl::SetAudioEnable(bool enabled)
{
	a_muted_ = !enabled;
	if (a_aac_encoder_) {
		a_aac_encoder_->Muted(a_muted_);
	}
}

void AnyRtmpStreamerImpl::SetVideoEnable(bool enabled)
{
	if (!enabled && av_rtmp_) {
		av_rtmp_->EnableOnlyAudioMode();
	}
}

void AnyRtmpStreamerImpl::SetAutoAdjustBit(bool enabled)
{
    auto_adjust_bit_ = enabled;
	if (v_h264_encoder_) {
		v_h264_encoder_->SetAutoAdjustBit(enabled);
	}
   
}
    
void AnyRtmpStreamerImpl::SetVideoParameter(int w, int h, int bitrate)
{
    v_width = w;
    v_height = h;
	if (v_h264_encoder_) {
		v_h264_encoder_->SetRates(bitrate);
	}
	v_bitrate_ = bitrate;
}
    
void AnyRtmpStreamerImpl::SetBitrate(int bitrate)
{
	if (v_h264_encoder_) {
		v_h264_encoder_->SetRates(bitrate);
	}
	v_bitrate_ = bitrate;
}

void AnyRtmpStreamerImpl::StartStream(const std::string&url)
{
   	int bitpersample = 16;
    rtc::CritScope l(&cs_av_rtmp_);
	if(av_rtmp_ == NULL)
		av_rtmp_ = new AnyRtmpPush(*this, url);
	av_rtmp_->SetAudioParameter(a_sample_hz_, bitpersample, a_channels_);
	av_rtmp_->SetVideoParameter(v_width, v_height, v_bitrate_, v_framerate_);
}

void AnyRtmpStreamerImpl::StopStream()
{
    {
        rtc::CritScope l(&cs_av_rtmp_);
        if(av_rtmp_)
        {
            delete av_rtmp_;
            av_rtmp_ = NULL;
        }
    }

	StopEncoder();
}

void AnyRtmpStreamerImpl::OnEncodeDataCallback(bool audio, uint8_t *p, uint32_t length, uint32_t ts)
{
	if(audio)
	{
		OnAACData(p, length, ts);
	}
	else
	{
		OnH264Data(p, length, ts);
	}
}

void AnyRtmpStreamerImpl::OnRtmpConnected()
{
	StartEncoder();
	callback_.OnStreamOk();
}

void AnyRtmpStreamerImpl::OnRtmpReconnecting(int times)
{
	StopEncoder();
	callback_.OnStreamReconnecting(times);
}

void AnyRtmpStreamerImpl::OnRtmpDisconnect()
{
	StopEncoder();
	if(rtmp_connected_)
	{
		callback_.OnStreamClosed();
	}
	else
	{
		callback_.OnStreamFailed(1);
	}
	rtmp_connected_ = false;
}

void AnyRtmpStreamerImpl::OnRtmpStatusEvent(int delayMs, int netBand)
{
    if (auto_adjust_bit_ && v_h264_encoder_) {
        v_h264_encoder_->SetNetDelay(delayMs);
    }
	callback_.OnStreamStatus(delayMs, netBand);
}

void AnyRtmpStreamerImpl::StartEncoder()
{
	if(v_h264_encoder_) {
		v_h264_encoder_->StartEncoder();
	}
	if (a_aac_encoder_) {
		a_aac_encoder_->StartEncoder();
	}
}

void AnyRtmpStreamerImpl::StopEncoder()
{
	if (v_h264_encoder_) {
		v_h264_encoder_->StopEncoder();
	}
	if (a_aac_encoder_) {
		a_aac_encoder_->StopEncoder();
	}
}

void AnyRtmpStreamerImpl::OnAACData(uint8_t* pData, int len, uint32_t ts)
{
    rtc::CritScope l(&cs_av_rtmp_);
	if(av_rtmp_)
	{
		av_rtmp_->SetAacData(pData, len, ts);
	}
}

void AnyRtmpStreamerImpl::OnH264Data(uint8_t* pData, int len, uint32_t ts)
{
    rtc::CritScope l(&cs_av_rtmp_);
	if(av_rtmp_)
	{
		av_rtmp_->SetH264Data(pData, len, ts);
	}
}

}	// namespace webrtc

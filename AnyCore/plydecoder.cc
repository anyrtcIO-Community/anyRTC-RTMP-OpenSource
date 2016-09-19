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
#include "plydecoder.h"
#include "anyrtmpcore.h"
#include "webrtc/base/logging.h"
#include "webrtc/media/engine/webrtcvideoframe.h"

PlyDecoder::PlyDecoder()
	: running_(false)
	, playing_(false)
	, h264_decoder_(NULL)
	, video_render_(NULL)
	, aac_decoder_(NULL)
	, a_cache_len_(0)
	, aac_sample_hz_(44100)
	, aac_channels_(2)
	, aac_frame_per10ms_size_(0)
{
	{
		h264_decoder_ = webrtc::H264Decoder::Create();
		webrtc::VideoCodec codecSetting;
		codecSetting.codecType = webrtc::kVideoCodecH264;
		codecSetting.width = 640;
		codecSetting.height = 480;
		h264_decoder_->InitDecode(&codecSetting, 1);
		h264_decoder_->RegisterDecodeCompleteCallback(this);
		webrtc::VideoCodec setting;
		setting.width = 640;
		setting.height = 480;
		setting.codecType = webrtc::kVideoCodecH264;
		setting.maxFramerate = 30;
		if (h264_decoder_->InitDecode(&setting, 1) != 0) {
			//@AnyRTC - Error
		}
	}

	aac_frame_per10ms_size_ = (aac_sample_hz_ / 100) * sizeof(int16_t) * aac_channels_;
	running_ = true;
	rtc::Thread::Start();

	ply_buffer_ = new PlyBuffer(*this, this);
}


PlyDecoder::~PlyDecoder()
{
	running_ = false;
	rtc::Thread::Stop();

	if (ply_buffer_) {
		delete ply_buffer_;
		ply_buffer_ = NULL;
	}
	if (aac_decoder_) {
		aac_decoder_close(aac_decoder_);
		aac_decoder_ = NULL;
	}
	if (h264_decoder_) {
		delete h264_decoder_;
		h264_decoder_ = NULL;
	}
}

bool PlyDecoder::IsPlaying()
{
    if (ply_buffer_ == NULL) {
        return false;
    }
    if (ply_buffer_->PlayerStatus() == PS_Cache) {
        return false;
    }
    return true;
}

int  PlyDecoder::CacheTime()
{
    if (ply_buffer_ != NULL) {
        return ply_buffer_->GetPlayCacheTime();
    }
    return 0;
}

void PlyDecoder::AddH264Data(const uint8_t*pdata, int len, uint32_t ts)
{
	if (ply_buffer_) {
		ply_buffer_->CacheH264Data(pdata, len, ts);
	}
}
void PlyDecoder::AddAACData(const uint8_t*pdata, int len, uint32_t ts)
{
	if (ply_buffer_) {
		if (aac_decoder_ == NULL) {
			aac_decoder_ = aac_decoder_open((unsigned char*)pdata, len, &aac_channels_, &aac_sample_hz_);
			if (aac_channels_ == 0)
				aac_channels_ = 1;
			aac_frame_per10ms_size_ = (aac_sample_hz_ / 100) * sizeof(int16_t) * aac_channels_;
		}
		else {
			unsigned int outlen = 0;
			if (aac_decoder_decode_frame(aac_decoder_, (unsigned char*)pdata, len, audio_cache_ + a_cache_len_, &outlen) > 0) {
				//printf("");
				a_cache_len_ += outlen;
				int ct = 0;
				int fsize = aac_frame_per10ms_size_;
				while (a_cache_len_ > fsize) {
					ply_buffer_->CachePcmData(audio_cache_ + ct * fsize, fsize, ts);
					a_cache_len_ -= fsize;
					ct++;
				}

				memmove(audio_cache_, audio_cache_ + ct * fsize, a_cache_len_);
			}
		}
	}
}

int PlyDecoder::GetPcmData(void* audioSamples, uint32_t& samplesPerSec, size_t& nChannels)
{
	if (!playing_) {
		return 0;
	}
	samplesPerSec = aac_sample_hz_;
	nChannels = aac_channels_;
	return ply_buffer_->GetPlayAudio(audioSamples);
}

void PlyDecoder::Run()
{
	while (running_)
	{
		{// ProcessMessages
			this->ProcessMessages(1);
		}
		PlyPacket* pkt = NULL;
		{
			rtc::CritScope cs(&cs_list_h264_);
			if (lst_h264_buffer_.size() > 0)
			{
				pkt = lst_h264_buffer_.front();
				lst_h264_buffer_.pop_front();
			}
		}
		if (pkt != NULL) {
			if (h264_decoder_)
			{
				int frameType = pkt->_data[4] & 0x1f;
				webrtc::EncodedImage encoded_image;
				encoded_image._buffer = (uint8_t*)pkt->_data;
				encoded_image._length = pkt->_data_len;
				encoded_image._size = pkt->_data_len + 8;
				if (frameType == 7) {
					encoded_image._frameType = webrtc::kVideoFrameKey;
				}
				else {
					encoded_image._frameType = webrtc::kVideoFrameDelta;
				}
				encoded_image._completeFrame = true;
                webrtc::RTPFragmentationHeader frag_info;
                int ret = h264_decoder_->Decode(encoded_image, false, &frag_info);
				if (ret != 0)
				{
				}
			}
			delete pkt;
		}
	}
}

void PlyDecoder::OnPlay()
{
	playing_ = true;
}
void PlyDecoder::OnPause()
{
	playing_ = false;
}
bool PlyDecoder::OnNeedDecodeData(PlyPacket* pkt)
{
	const uint8_t*pdata = pkt->_data;
	int len = pkt->_data_len;
	uint32_t ts = pkt->_dts;
	if (pkt->_b_video) {
		int type = pdata[4] & 0x1f;
		rtc::CritScope cs(&cs_list_h264_);
		if (type == 7) {
			//* Skip all buffer data, beacause decode is so slow!!!
			std::list<PlyPacket*>::iterator iter = lst_h264_buffer_.begin();
			while (iter != lst_h264_buffer_.end()) {
				PlyPacket* pkt = *iter;
				lst_h264_buffer_.erase(iter++);
				delete pkt;
			}
		}
		lst_h264_buffer_.push_back(pkt);
	}

	return true;
}

int32_t PlyDecoder::Decoded(webrtc::VideoFrame& decodedImage)
{
	const cricket::WebRtcVideoFrame render_frame(
		decodedImage.video_frame_buffer(),
		decodedImage.render_time_ms() * rtc::kNumNanosecsPerMillisec, decodedImage.rotation());

	if (video_render_ != NULL) {
		video_render_->OnFrame(render_frame);
	}
	return 0;
}
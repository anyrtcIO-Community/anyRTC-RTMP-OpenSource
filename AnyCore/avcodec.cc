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
#include "avcodec.h"
#include "webrtc/media/base/videoframe.h"
#include "webrtc/modules/video_coding/codecs/h264/include/h264.h"

static const size_t kEventMaxWaitTimeMs = 100;
static const size_t kMaxDataSizeSamples = 3840;

namespace webrtc {
	class AudioPcm{
	public:
		AudioPcm(const void* audioSamples, size_t nSamples,
			size_t nChannels, uint32_t samplesPerSec) :audio_samples_(NULL){
			audio_samples_ = new char[nSamples*sizeof(int16_t)*nChannels];
			memcpy(audio_samples_, audioSamples, nSamples*sizeof(int16_t)*nChannels);
			sample_per_channel = nSamples;
			channels_ = nChannels;
			samples_hz_ = samplesPerSec;
		}
		virtual ~AudioPcm(void){
			if (audio_samples_) {
				delete (char *)audio_samples_;
				audio_samples_ = NULL;
			}
		}
		void* audio_samples_;
		int sample_per_channel; 
		int channels_;
		uint32_t samples_hz_;
	};
A_AACEncoder::A_AACEncoder(AVCodecCallback&callback)
: callback_(callback)
, encoder_(nullptr)
, muted_(false)
, encoded_(false)
, audio_record_sample_hz_(44100)
, audio_record_channels_(2)
{
    if (!running_) {
        running_ = true;
    }
}

A_AACEncoder::~A_AACEncoder(void)
{
    if (running_) {
        running_ = false;
    }
    
    if (NULL != encoder_)
	{
		/*Close FAAC engine*/
		aac_encoder_close(encoder_);
		encoder_ = NULL;
	}

	rtc::CritScope cs(&buffer_critsect_);
	while (audio_buffer_.size() > 0) {
		AudioPcm* pcm = (AudioPcm*)audio_buffer_.front();
		audio_buffer_.pop_front();

		delete pcm;
	}
}

bool A_AACEncoder::Init(int num_channels, int sample_rate, int pcm_bit_size)
{
	if(encoder_)
		return false;
	audio_record_sample_hz_ = sample_rate;
	audio_record_channels_ = num_channels;
	encoder_ = aac_encoder_open(num_channels, sample_rate, pcm_bit_size, false);
	return true;
}

void A_AACEncoder::StartEncoder()
{
    rtc::CritScope cs(&buffer_critsect_);
    while (audio_buffer_.size() > 0) {
        AudioPcm* pcm = (AudioPcm*)audio_buffer_.front();
        audio_buffer_.pop_front();
        
        delete pcm;
    }
    encoded_ = true;
}
    void A_AACEncoder::StopEncoder()
{
	rtc::CritScope cs(&buffer_critsect_);
    encoded_ = false;
	while (audio_buffer_.size() > 0) {
		AudioPcm* pcm = (AudioPcm*)audio_buffer_.front();
		audio_buffer_.pop_front();

		delete pcm;
	}
}
int A_AACEncoder::Encode(const void* audioSamples, const size_t nSamples, const size_t nBytesPerSample, 
		const size_t nChannels, const uint32_t samplesPerSec, const uint32_t totalDelayMS)
{
	int status = 0;
	if(encoder_)
	{
		unsigned int outlen = 0;
		uint32_t curtime = rtc::Time();
		uint8_t encoded[1024];
		if(muted_)
		{// mute audio
			memset((uint8_t*)audioSamples, 0, nSamples*nBytesPerSample*nChannels);
		}
		status = aac_encoder_encode_frame(encoder_, (uint8_t*)audioSamples, nSamples*nBytesPerSample*nChannels, encoded, &outlen);
		if(outlen > 0)
		{
			//ALOGE("Encode aac len:%d", outlen);
			callback_.OnEncodeDataCallback(true, encoded, outlen, curtime);
		}
	}

	return status;
}

void A_AACEncoder::OnData(const Data& audio)
{
	rtc::CritScope cs(&buffer_critsect_);
    if(encoded_) {
		if (audio_record_sample_hz_ != audio.sample_rate || audio.channels != audio_record_channels_) {
			int16_t temp_output[kMaxDataSizeSamples];
			int samples_per_channel_int = resampler_record_.Resample10Msec((int16_t*)audio.data, audio.sample_rate * audio.channels,
				audio_record_sample_hz_ * audio_record_channels_, 1, kMaxDataSizeSamples, temp_output);
			Encode(temp_output, audio_record_sample_hz_ / 100, 2, audio_record_channels_, audio_record_sample_hz_, 0);
		}
		else {
			Encode((int16_t*)audio.data, audio.samples_per_channel, 2, audio_record_channels_, audio_record_sample_hz_, 0);
		}
    }
}

//===================================================
//* V_H264Encoder
V_H264Encoder::V_H264Encoder(AVCodecCallback&callback)
: callback_(callback)
, need_keyframe_(true)
, running_(false)
, encoded_(false)
, video_bitrate_(768)
, delay_ms_(0)
, adjust_v_bitrate_(0)
, render_buffers_(new VideoRenderFrames(0))
, video_encoder_factory_(NULL)
, encoder_(NULL)
{
	h264_.codecType = kVideoCodecH264;
	h264_.mode = kRealtimeVideo;
	h264_.startBitrate = video_bitrate_;
	h264_.targetBitrate = video_bitrate_;
	h264_.maxBitrate = 1024;
	h264_.maxFramerate = 20;
	h264_.width = 640;
	h264_.height = 480;
	h264_.codecSpecific.H264.profile = kProfileBase;
	h264_.codecSpecific.H264.frameDroppingOn = true;
	h264_.codecSpecific.H264.keyFrameInterval = h264_.maxFramerate * 3;	//	3s
	h264_.codecSpecific.H264.spsData = nullptr;	
	h264_.codecSpecific.H264.spsLen = 0;
	h264_.codecSpecific.H264.ppsData = nullptr;
	h264_.codecSpecific.H264.ppsLen = 0;
    
    adjust_v_bitrate_ = video_bitrate_;
    if (!running_) {
        running_ = true;
        rtc::Thread::Start();
    }
}

V_H264Encoder::~V_H264Encoder(void)
{
	if (running_) {
		running_ = false;
		rtc::Thread::Stop();
	}
	
	{
		rtc::CritScope cs_buffer(&buffer_critsect_);
		render_buffers_.reset();
	}

	if(encoder_)
	{
		encoder_->Release();
		delete encoder_;
		encoder_ = NULL;
	}
}

void V_H264Encoder::Init(cricket::WebRtcVideoEncoderFactory* video_encoder_factory)
{
	video_encoder_factory_ = video_encoder_factory;
}

void V_H264Encoder::SetParameter(int width, int height, int fps, int bitrate)
{
	h264_.startBitrate = bitrate;
	h264_.targetBitrate = bitrate;
	h264_.maxBitrate = bitrate;
	h264_.maxFramerate = fps;
	h264_.width = width;
	h264_.height = height;
}

void V_H264Encoder::SetRates(int bitrate)
{
    video_bitrate_ = bitrate;
	h264_.startBitrate = bitrate;
	h264_.targetBitrate = bitrate;
	h264_.maxBitrate = bitrate;
	if(encoder_ != NULL)
	{
		encoder_->SetRates(bitrate, h264_.maxFramerate);
	}
}
    
void V_H264Encoder::SetAutoAdjustBit(bool enabled)
{
    if (enabled) {
        adjust_v_bitrate_ = video_bitrate_;
    }
}
    
void V_H264Encoder::SetNetDelay(int delayMs)
{
    if(delayMs >= 0 && delayMs < 3000)
    {
        if(delay_ms_ != 0 && delayMs <=1000){
            delay_ms_ = 0;
            SetRates(adjust_v_bitrate_);
        }
    }
    else if(delayMs >= 3000 && delayMs < 5000)
    {
        if(delay_ms_ == 0)
        {
            delay_ms_ = 3000;
            SetRates(adjust_v_bitrate_*3/4);
        }
        else if(delay_ms_ >= 5000) {
            if(delayMs <= 4000)
            {
                delay_ms_ = 3000;
                SetRates(adjust_v_bitrate_*3/4);
            }
        }
    }
    else if(delayMs >= 5000 && delayMs < 10000)
    {
        if(delay_ms_ <= 3000)
        {
            delay_ms_ = 5000;
            SetRates(adjust_v_bitrate_/2);
        }
        else if(delay_ms_ >= 10000) {
            if(delayMs <= 7000)
            {
                delay_ms_ = 5000;
                SetRates(adjust_v_bitrate_/2);
                // Fps Up
            }
        }
    }
    else if(delayMs >= 10000)
    {
        if(delay_ms_ <= 5000)
        {
            delay_ms_ = 10000;
            SetRates(adjust_v_bitrate_/3);
            // Fps down
        }
    }
}

void V_H264Encoder::CreateVideoEncoder()
{
	VideoEncoder* extern_encoder = NULL;
	if(video_encoder_factory_ != NULL)
	{
		extern_encoder = video_encoder_factory_->CreateVideoEncoder(kVideoCodecH264);
	}
	if(extern_encoder != NULL)
	{// Try to use encoder use H/W
		if(extern_encoder->InitEncode(&h264_, 1, 0) == WEBRTC_VIDEO_CODEC_OK)
		{
			encoder_ = extern_encoder;
		} 
		else
		{
			delete extern_encoder;
		}
	}

	if(encoder_ == NULL)
	{// Use software codec
		encoder_ = webrtc::H264Encoder::Create();
		if(encoder_->InitEncode(&h264_, 1, 0) != WEBRTC_VIDEO_CODEC_OK)
		{
			assert(false);
		}
	}

	encoder_->RegisterEncodeCompleteCallback(this);
}

void V_H264Encoder::StartEncoder()
{
    rtc::CritScope cs_buffer(&buffer_critsect_);
    //render_buffers_->ReleaseAllFrames();
    need_keyframe_ = true;
    encoded_ = true;
	
}
void V_H264Encoder::StopEncoder()
{
    rtc::CritScope cs_buffer(&buffer_critsect_);
    //render_buffers_->ReleaseAllFrames();
    encoded_ = false;
}

void V_H264Encoder::RequestKeyFrame()
{
	need_keyframe_ = true;
}

//* For Thread
void V_H264Encoder::Run()
{
	while(running_)
	{
		int64_t cur_time = rtc::TimeMillis();
		// Get a new frame to render and the time for the frame after this one.
		rtc::Optional<VideoFrame> frame_to_render;
		uint32_t wait_time = 0;
		{
		  rtc::CritScope cs(&buffer_critsect_);
		  frame_to_render = render_buffers_->FrameToRender();
		  wait_time = render_buffers_->TimeToNextFrameRelease();
		}

		if (frame_to_render) {
			if(h264_.width != frame_to_render->width() || h264_.height != frame_to_render->height())
			{
				h264_.width = frame_to_render->width();
				h264_.height = frame_to_render->height();
				if(encoder_)
				{
					encoder_->Release();
					encoder_ = NULL;
				}
				
			}
			if(encoder_ == NULL)
			{
				CreateVideoEncoder();
			}
			CodecSpecificInfo codec_info;
			std::vector<FrameType> next_frame_types(1, kVideoFrameDelta);
			if(need_keyframe_) {
				need_keyframe_ = false;
				next_frame_types[0] = kVideoFrameKey;
			}
			
			if(encoder_)
			{
				int ret = encoder_->Encode(*frame_to_render, &codec_info, &next_frame_types);
				if(ret != 0)
				{
					//printf("Encode ret :%d", ret);
				}
			}
		}

		// Set timer for next frame to render.
		if (wait_time > kEventMaxWaitTimeMs) {
		  wait_time = kEventMaxWaitTimeMs;
		}
		// Reduce encode time
		uint64_t slapTime = rtc::TimeMillis() - cur_time;
		if (wait_time >= slapTime)
			wait_time -= slapTime;
		else
			wait_time = 1;
		if(wait_time > 0)
		{
			//ALOGE("WaitTime: %d", wait_time);
			rtc::Thread::SleepMs(wait_time);
		}
	}
}

void V_H264Encoder::OnFrame(const cricket::VideoFrame& frame)
{
    rtc::CritScope csB(&buffer_critsect_);
    if (encoded_) {
        webrtc::VideoFrame video_frame(frame.video_frame_buffer(), 0, rtc::TimeMillis() + 150, frame.rotation());
        if (!video_frame.IsZeroSize()) {
            if (render_buffers_->AddFrame(video_frame) == 1) {
            // OK
            }
        }
    }
}

int32_t V_H264Encoder::Encoded(const EncodedImage& encoded_image,
                          const CodecSpecificInfo* codec_specific_info,
                          const RTPFragmentationHeader* fragmentation)
{
	callback_.OnEncodeDataCallback(false, encoded_image._buffer, encoded_image._length, rtc::Time());
	return 0;
}


}	// namespace webrtc
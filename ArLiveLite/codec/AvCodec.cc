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
#include "Avcodec.h"
#include "libyuv/include/libyuv.h"
#include "rtc_base/bind.h"
#include "rtc_base/time_utils.h"
#include "modules/video_coding/codecs/h264/include/h264.h"
#include "rtc_base/internal/default_socket_server.h"

static const size_t kEventMaxWaitTimeMs = 15;
static const size_t kMaxDataSizeSamples = 3840;

namespace webrtc {
A_AACEncoder::A_AACEncoder(AVCodecCallback&callback)
: callback_(callback)
, encoder_(nullptr)
, audio_record_sample_hz_(44100)
, audio_record_channels_(2)
{

}

A_AACEncoder::~A_AACEncoder(void)
{

    if (NULL != encoder_)
	{
		/*Close FAAC engine*/
		aac_encoder_close(encoder_);
		encoder_ = NULL;
	}
}

bool A_AACEncoder::Init(int sample_rate, int num_channels, int bitrate)
{
	if(encoder_)
		return false;
	audio_record_sample_hz_ = sample_rate;
	audio_record_channels_ = num_channels;
	encoder_ = aac_encoder_open(num_channels, sample_rate, sizeof(short)*8, bitrate, true);
	return true;
}

void A_AACEncoder::DeInit()
{
	if (encoder_ != NULL) {
		/*Close FAAC engine*/
		aac_encoder_close(encoder_);
		encoder_ = NULL;
	}
}

int A_AACEncoder::Encode(const void* audioSamples, const size_t nSamples, const size_t nBytesPerSample,
		const size_t nChannels, const uint32_t samplesPerSec, const uint32_t totalDelayMS)
{
	int status = 0;
	if(encoder_)
	{
		int16_t temp_output[kMaxDataSizeSamples];
		if (audio_record_sample_hz_ != samplesPerSec || audio_record_channels_ != nChannels) {

			int samples_per_channel_int = resampler_record_.Resample10Msec((int16_t*)audioSamples, samplesPerSec * nChannels,
				audio_record_sample_hz_ * audio_record_channels_, 1, kMaxDataSizeSamples, temp_output);
		}
		else {
			memcpy(temp_output, audioSamples, (audio_record_sample_hz_*audio_record_channels_*sizeof(short))/100);
		}

		unsigned int outlen = 0;
		uint32_t curtime = rtc::Time32();
		uint8_t encoded[1024];
		status = aac_encoder_encode_frame(encoder_, (uint8_t*)temp_output, (audio_record_sample_hz_*audio_record_channels_ * sizeof(short)) / 100, encoded, &outlen);
		if(outlen > 0)
		{
			//ALOGE("Encode aac len:%d", outlen);
			callback_.OnEncodeDataCallback(true, false, encoded, outlen, curtime);
		}
	}

	return status;
}

//===================================================
//* V_H264Encoder
#define YUV_B_SIZE 4
void ScaleToReqYuvCrop(const webrtc::I420BufferInterface* i420_src, const uint8_t* yPtr, const uint8_t* uPtr, const uint8_t* vPtr, int ww, int hh, int stride)
{
	int nSrcWidth = i420_src->width();
	int nSrcHeight = i420_src->height();
	int nNeedWidth = ww;
	int nNeedHeight = hh;

	uint8_t* y = (uint8_t*)i420_src->DataY();
	uint8_t* u = (uint8_t*)i420_src->DataU();
	uint8_t* v = (uint8_t*)i420_src->DataV();

	float yuvF = (float)i420_src->width() / (float)i420_src->height();
	float realF = (float)nNeedWidth / (float)nNeedHeight;
	int nScaleWidth = nSrcWidth;
	int nScaleHeight = (int)((float)nScaleWidth / realF);
	if (nScaleHeight > nSrcHeight) {
		nScaleHeight = nSrcHeight;
		nScaleWidth = (int)((float)nSrcHeight * realF);
	}
	if (nScaleWidth % YUV_B_SIZE != 0) {
		nScaleWidth += (YUV_B_SIZE - nScaleWidth % YUV_B_SIZE);
		if (nScaleWidth > nSrcWidth) {
			nScaleWidth = nSrcWidth;
		}
	}
	if (nScaleHeight % YUV_B_SIZE != 0) {
		nScaleHeight += (YUV_B_SIZE - nScaleHeight % YUV_B_SIZE);
		if (nScaleHeight > nSrcHeight) {
			nScaleHeight = nSrcHeight;
		}
	}

	int offsetW = (nSrcWidth - nScaleWidth) / 2;
	int offsetH = (nSrcHeight - nScaleHeight) / 2;

	uint8_t* buf0_y = y + (i420_src->StrideY() * offsetH);
	uint8_t* buf0_u = u + (i420_src->StrideY() * offsetH) / 4;
	uint8_t* buf0_v = v + (i420_src->StrideY() * offsetH) / 4;

	libyuv::I420Scale(buf0_y + offsetW, i420_src->StrideY(), buf0_u + offsetW / 2, i420_src->StrideU(),
		buf0_v + offsetW / 2, i420_src->StrideV(), nScaleWidth, nScaleHeight, (uint8_t*)yPtr, stride, (uint8_t*)uPtr, stride / 2, (uint8_t*)vPtr, stride / 2, ww, hh, libyuv::kFilterBilinear);
}
void ScaleToReqYuvFit(const webrtc::I420BufferInterface* i420_src, const uint8_t* yPtr, const uint8_t* uPtr, const uint8_t* vPtr, int ww, int hh, int stride)
{
	int nSrcWidth = i420_src->width();
	int nSrcHeight = i420_src->height();
	int nNeedWidth = ww;
	int nNeedHeight = hh;

	uint8_t* y = (uint8_t*)yPtr;
	uint8_t* u = (uint8_t*)uPtr;
	uint8_t* v = (uint8_t*)vPtr;

	float yuvF = (float)i420_src->width() / (float)i420_src->height();
	float realF = (float)nNeedWidth / (float)nNeedHeight;
	int nScaleWidth = nNeedWidth;
	int nScaleHeight = (int)((float)nScaleWidth / yuvF);
	if (nScaleHeight > nNeedHeight) {
		nScaleHeight = nNeedHeight;
		nScaleWidth = (int)((float)nNeedHeight * yuvF);
	}
	if (nScaleWidth % YUV_B_SIZE != 0) {
		nScaleWidth += (YUV_B_SIZE - nScaleWidth % YUV_B_SIZE);
		if (nScaleWidth > nNeedWidth) {
			nScaleWidth = nNeedWidth;
		}
	}
	if (nScaleHeight % YUV_B_SIZE != 0) {
		nScaleHeight += (YUV_B_SIZE - nScaleHeight % YUV_B_SIZE);
		if (nScaleHeight > nNeedHeight) {
			nScaleHeight = nNeedHeight;
		}
	}

	int offsetW = (nNeedWidth - nScaleWidth) / 2;
	int offsetH = (nNeedHeight - nScaleHeight) / 2;

	uint8_t* buf0_y = y + (stride * offsetH);
	uint8_t* buf0_u = u + (stride * offsetH) / 4;
	uint8_t* buf0_v = v + (stride * offsetH) / 4;

	libyuv::I420Scale(i420_src->DataY(), i420_src->StrideY(), i420_src->DataU(), i420_src->StrideU(),
		i420_src->DataV(), i420_src->StrideV(), i420_src->width(), i420_src->height(),
		buf0_y + offsetW, stride, buf0_u + offsetW / 2, stride / 2,
		buf0_v + offsetW / 2, stride / 2, nScaleWidth, nScaleHeight,
		libyuv::kFilterBilinear);
}
void ScaleToReqYuv(const webrtc::I420BufferInterface* i420_src, const uint8_t* yPtr, const uint8_t* uPtr, const uint8_t* vPtr, int ww, int hh, int stride)
{
	uint8_t* y = (uint8_t*)yPtr;
	uint8_t* u = (uint8_t*)uPtr;
	uint8_t* v = (uint8_t*)vPtr;

	int offsetW = 0;
	int offsetH = 0;
	uint8_t* buf0_y = y + (stride * offsetH);
	uint8_t* buf0_u = u + (stride * offsetH) / 4;
	uint8_t* buf0_v = v + (stride * offsetH) / 4;

	libyuv::I420Scale(i420_src->DataY(), i420_src->StrideY(), i420_src->DataU(), i420_src->StrideU(),
		i420_src->DataV(), i420_src->StrideV(), i420_src->width(), i420_src->height(),
		buf0_y + offsetW, stride, buf0_u + offsetW / 2, stride / 2,
		buf0_v + offsetW / 2, stride / 2, ww, hh,
		libyuv::kFilterBilinear);
}

V_H264Encoder::V_H264Encoder(AVCodecCallback&callback)
: callback_(callback)
, rtc::Thread(rtc::CreateDefaultSocketServer())
, need_keyframe_(true)
, running_(false)
, b_mirror_(false)
, e_scale_mode_(VideoScaleModeAuto)
, n_next_keyframe_time_(0)
, render_buffers_(new VideoRenderFrames(0))
, video_encoder_factory_(NULL)
{
	h264_.codecType = kVideoCodecH264;
	h264_.mode = webrtc::VideoCodecMode::kRealtimeVideo;
	h264_.startBitrate = 768;
	h264_.maxBitrate = 768;
	h264_.maxFramerate = 25;
	h264_.width = 640;
	h264_.height = 480;
}

V_H264Encoder::~V_H264Encoder(void)
{
	DestoryVideoEncoder();
}

void V_H264Encoder::SetExVideoEncoderFactory(webrtc::VideoEncoderFactory* video_encoder_factory)
{
	video_encoder_factory_ = video_encoder_factory;
}

void V_H264Encoder::SetParameter(int width, int height, int fps, int bitrate)
{
	h264_.startBitrate = bitrate;
	h264_.maxBitrate = bitrate;
	h264_.maxFramerate = fps;
	h264_.width = width;
	h264_.height = height;
}

void V_H264Encoder::SetMirror(bool bMirror)
{
	b_mirror_ = bMirror;
}
void V_H264Encoder::SetVideoScaleMode(VideoScaleMode eMode)
{
	e_scale_mode_ = eMode;
}

void V_H264Encoder::UpdateBitrate(int bitrate)
{
	h264_.startBitrate = bitrate;
	h264_.maxBitrate = bitrate;
	if(encoder_ != NULL)
	{
		webrtc::VideoBitrateAllocation allocation;
		allocation.SetBitrate(0, 0, (bitrate * 1000));
		webrtc::VideoEncoder::RateControlParameters rateCtrlParams(allocation, h264_.maxFramerate);
		encoder_->SetRates(rateCtrlParams);
	}
}

void V_H264Encoder::CreateVideoEncoder()
{
	if (!running_) {
		running_ = true;
		rtc::Thread::Start();

#ifdef WIN32
		NewVideoEncoder();
#else
		if (!rtc::Thread::IsCurrent()) {
			rtc::Thread::Invoke<void>(RTC_FROM_HERE, rtc::Bind(&V_H264Encoder::NewVideoEncoder, this));
		}
		else {
			NewVideoEncoder();
		}
#endif
	}
}

void V_H264Encoder::DestoryVideoEncoder()
{
	if (running_) {
#ifdef WIN32
		FreeVideoEncoder();
#else
		if (!rtc::Thread::IsCurrent()) {
			rtc::Thread::Invoke<void>(RTC_FROM_HERE, rtc::Bind(&V_H264Encoder::FreeVideoEncoder, this));
		}
		else {
			FreeVideoEncoder();
		}
#endif
		running_ = false;
		rtc::Thread::Stop();
	}
}

void V_H264Encoder::RequestKeyFrame()
{
	need_keyframe_ = true;
}

void V_H264Encoder::Encode(const webrtc::VideoFrame& frame)
{
	if (!running_)
		return;

	if (frame.video_frame_buffer()->GetI420() != NULL && frame.rotation() == webrtc::kVideoRotation_0) {
		webrtc::VideoFrame copy_frame(frame);
		copy_frame.set_timestamp_us(rtc::TimeMicros());
		AddToFrameList(copy_frame);
	}
	else {
		/* Apply pending rotation. */
		rtc::scoped_refptr<webrtc::VideoFrameBuffer> buffer(
			frame.video_frame_buffer());

		webrtc::VideoFrame rotated_frame(frame);
		if (buffer->GetI420() != NULL) {
			rotated_frame.set_video_frame_buffer(
				webrtc::I420Buffer::Rotate(*buffer->GetI420(), frame.rotation()));
		}
		else {
			rotated_frame.set_video_frame_buffer(
				webrtc::I420Buffer::Rotate(*buffer->ToI420(), frame.rotation()));
		}

		rotated_frame.set_rotation(webrtc::kVideoRotation_0);
		rotated_frame.set_timestamp_us(rtc::TimeMicros());
		AddToFrameList(rotated_frame);
	}
}
void V_H264Encoder::AddToFrameList(webrtc::VideoFrame& frame)
{
	int reqWidth = h264_.width;
	int reqHeight = h264_.height;
	if (e_scale_mode_ == VideoScaleModeAuto) {
		int nSrcWidth = h264_.width;
		int nSrcHeight = h264_.height;
		int nNeedWidth = frame.width();
		int nNeedHeight = frame.height();
		float yuvF = (float)nSrcWidth / (float)nSrcHeight;
		float realF = (float)nNeedWidth / (float)nNeedHeight;
		int nScaleWidth = nSrcWidth;
		int nScaleHeight = (int)((float)nScaleWidth / realF);
		if (nScaleHeight > nSrcHeight) {
			nScaleHeight = nSrcHeight;
			nScaleWidth = (int)((float)nSrcHeight * realF);
		}
		if (nScaleWidth % 4 != 0) {
			nScaleWidth += (4 - nScaleWidth % 4);
			if (nScaleWidth > nSrcWidth) {
				nScaleWidth = nSrcWidth;
			}
		}
		if (nScaleHeight % 4 != 0) {
			nScaleHeight += (4 - nScaleHeight % 4);
			if (nScaleHeight > nSrcHeight) {
				nScaleHeight = nSrcHeight;
			}
		}

		reqWidth = nScaleWidth;
		reqHeight = nScaleHeight;
	}

	if (b_mirror_ || reqWidth != frame.width() || reqHeight != frame.height()) {
		rtc::scoped_refptr<webrtc::I420Buffer> video_buffer_ = I420Buffer::Create(
			reqWidth, reqHeight, reqWidth, reqWidth / 2, reqWidth / 2);
		webrtc::I420Buffer::SetBlack(video_buffer_);
		if (b_mirror_) {
			int mirror_w_ = frame.width();
			int mirror_h_ = frame.height();
			int stride_y = frame.width();
			int stride_uv = (frame.width() + 1) / 2;
			if (video_mirror_buffer_ == NULL || (video_mirror_buffer_->width() != frame.width() || video_mirror_buffer_->height() != frame.height())) {
				video_mirror_buffer_ = I420Buffer::Create(
					mirror_w_, abs(mirror_h_), stride_y, stride_uv, stride_uv);
			}
			const webrtc::I420BufferInterface* i420_buffer =
				frame.video_frame_buffer()->GetI420();
			libyuv::I420Mirror(i420_buffer->DataY(), i420_buffer->StrideY(), i420_buffer->DataU(), i420_buffer->StrideU(),
				i420_buffer->DataV(), i420_buffer->StrideV(),
				(uint8_t*)video_mirror_buffer_->DataY(), video_mirror_buffer_->StrideY(), (uint8_t*)video_mirror_buffer_->DataU(), video_mirror_buffer_->StrideU(),
				(uint8_t*)video_mirror_buffer_->DataV(), video_mirror_buffer_->StrideV(), video_mirror_buffer_->width(), video_mirror_buffer_->height());

			if (e_scale_mode_ == VideoScaleModeFill) {
				ScaleToReqYuvCrop(video_mirror_buffer_, video_buffer_->DataY(), video_buffer_->DataU(), video_buffer_->DataV(), video_buffer_->width(), video_buffer_->height(), video_buffer_->StrideY());
			}
			else if (e_scale_mode_ == VideoScaleModeFit) {
				ScaleToReqYuvFit(video_mirror_buffer_, video_buffer_->DataY(), video_buffer_->DataU(), video_buffer_->DataV(), video_buffer_->width(), video_buffer_->height(), video_buffer_->StrideY());
			}
			else if (e_scale_mode_ == VideoScaleModeAuto) {
				ScaleToReqYuv(video_mirror_buffer_, video_buffer_->DataY(), video_buffer_->DataU(), video_buffer_->DataV(), video_buffer_->width(), video_buffer_->height(), video_buffer_->StrideY());
			}
		}
		else {
			if (e_scale_mode_ == VideoScaleModeFill) {
				ScaleToReqYuvCrop(frame.video_frame_buffer()->GetI420(), video_buffer_->DataY(), video_buffer_->DataU(), video_buffer_->DataV(), video_buffer_->width(), video_buffer_->height(), video_buffer_->StrideY());
			}
			else if (e_scale_mode_ == VideoScaleModeFit) {
				ScaleToReqYuvFit(frame.video_frame_buffer()->GetI420(), video_buffer_->DataY(), video_buffer_->DataU(), video_buffer_->DataV(), video_buffer_->width(), video_buffer_->height(), video_buffer_->StrideY());
			}
			else if (e_scale_mode_ == VideoScaleModeAuto) {
				ScaleToReqYuv(frame.video_frame_buffer()->GetI420(), video_buffer_->DataY(), video_buffer_->DataU(), video_buffer_->DataV(), video_buffer_->width(), video_buffer_->height(), video_buffer_->StrideY());
			}
		}

		rtc::CritScope cs(&buffer_critsect_);
		webrtc::VideoFrame video_frame(video_buffer_, 0, rtc::TimeMillis(), frame.rotation());
		render_buffers_->AddFrame(std::move(video_frame));
	}
	else {
		rtc::CritScope cs(&buffer_critsect_);
		render_buffers_->AddFrame(std::move(frame));
	}
}

//* For Thread
void V_H264Encoder::Run()
{
	while(running_)
	{
		int64_t cur_time = rtc::TimeMillis();
		// Get a new frame to render and the time for the frame after this one.
		absl::optional<webrtc::VideoFrame> frame_to_render;
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
				video_encode_buffer_ = NULL;
			}
			if(encoder_ == NULL)
			{
				NewVideoEncoder();
			}
			if (video_encode_buffer_ == NULL) {
				video_encode_buffer_ = I420Buffer::Create(
					h264_.width, abs(h264_.height), h264_.width, h264_.width/2, h264_.width/2);
			}
			if (n_next_keyframe_time_ <= rtc::TimeUTCMillis()) {
				n_next_keyframe_time_ = rtc::TimeUTCMillis() + 3000;
				need_keyframe_ = true;
			}
			std::vector<webrtc::VideoFrameType> next_frame_types(1, webrtc::VideoFrameType::kVideoFrameDelta);
			if (need_keyframe_) {
				need_keyframe_ = false;
				next_frame_types[0] = webrtc::VideoFrameType::kVideoFrameKey;
			}
			
			if(encoder_)
			{
#ifdef WEBRTC_IOS
				rtc::scoped_refptr<webrtc::I420BufferInterface> yuv420 = frame_to_render->video_frame_buffer()->ToI420();
				libyuv::I420Copy(yuv420->DataY(), yuv420->StrideY(), yuv420->DataU(), yuv420->StrideU(), yuv420->DataV(), yuv420->StrideV(),
					(uint8_t*)video_encode_buffer_->DataY(), video_encode_buffer_->StrideY(), (uint8_t*)video_encode_buffer_->DataU(), video_encode_buffer_->StrideU(),
					(uint8_t*)video_encode_buffer_->DataV(), video_encode_buffer_->StrideV(), video_encode_buffer_->width(), video_encode_buffer_->height());
				webrtc::VideoFrame videoFrame(video_encode_buffer_, webrtc::VideoRotation::kVideoRotation_0, frame_to_render->timestamp_us());
				videoFrame.set_ntp_time_ms(frame_to_render->ntp_time_ms());
				videoFrame.set_timestamp(frame_to_render->timestamp());
				int ret = encoder_->Encode(videoFrame, &next_frame_types);
#else 
				int ret = encoder_->Encode(*frame_to_render, &next_frame_types);
#endif
				if (ret != 0)
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

		rtc::Thread::ProcessMessages(1);
	}
}

webrtc::EncodedImageCallback::Result V_H264Encoder::OnEncodedImage(
	const EncodedImage& encoded_image,
	const CodecSpecificInfo* codec_specific_info)
{
	if (encoded_image._frameType == webrtc::VideoFrameType::kVideoFrameKey) {
		n_next_keyframe_time_ = rtc::TimeUTCMillis() + 3000;
	}
	callback_.OnEncodeDataCallback(false, encoded_image._frameType == webrtc::VideoFrameType::kVideoFrameKey, encoded_image.data(), encoded_image.size(), rtc::Time32());
	EncodedImageCallback::Result result(EncodedImageCallback::Result::OK);
	return result;
}

void V_H264Encoder::NewVideoEncoder()
{
	std::string strCodecName = "H264";
	webrtc::SdpVideoFormat sdpParam(strCodecName);
	sdpParam.parameters[cricket::kH264FmtpPacketizationMode] = "1";	//@Eric - 导致单帧太大?

	std::unique_ptr<VideoEncoder> extern_encoder = NULL;
	if (video_encoder_factory_ != NULL)
	{
		extern_encoder = video_encoder_factory_->CreateVideoEncoder(sdpParam);
	}
	if (extern_encoder != NULL)
	{// Try to use encoder use H/W
		if (extern_encoder->InitEncode(&h264_, 1, 0) == WEBRTC_VIDEO_CODEC_OK)
		{
			encoder_.reset(extern_encoder.release());
		}
		else
		{
			extern_encoder = NULL;
		}
	}

	if (encoder_ == NULL)
	{// Use software codec
		encoder_ = webrtc::H264Encoder::Create(cricket::VideoCodec(sdpParam));
		if (encoder_->InitEncode(&h264_, 1, 0) != WEBRTC_VIDEO_CODEC_OK)
		{
			assert(false);
		}
	}

	encoder_->RegisterEncodeCompleteCallback(this);
}

void V_H264Encoder::FreeVideoEncoder()
{
	if (encoder_ != NULL)
	{
		encoder_->Release();
		encoder_ = NULL;
	}
	{
		rtc::CritScope cs_buffer(&buffer_critsect_);
		while (1) {
			absl::optional<webrtc::VideoFrame> frame_to_render = render_buffers_->FrameToRender();
			if (!frame_to_render) {
				break;
			}
		}
	}
}


//======================================================================
//
V_H264Decoder::V_H264Decoder(RtcVidDecoderEvent& callback)
	: rtc::Thread(rtc::CreateDefaultSocketServer())
	, callback_(callback)
	, status_event_(NULL)
	, running_(false)
	, has_decoded_(false)
	, has_dec_frame_(false)
	, has_video_(false)
	, need_keyframe_(false)
	, v_width_(0)
	, v_height_(0)
	, v_report_w_(0)
	, v_report_h_(0)
	, v_report_time_(0)
	, n_decoded_time_(0)
	, n_decoded_used_(0)
{
	video_decoder_factory_ = NULL;
	running_ = true;
	rtc::Thread::SetName("RtcVidDecoderThread", this);
	rtc::Thread::Start();
}
V_H264Decoder::~V_H264Decoder(void)
{
	if (running_) {
		running_ = false;
		rtc::Thread::Stop();
	}

	if (vid_decoder_ != NULL) {
		vid_decoder_->Release();
		vid_decoder_ = NULL;
	}
	video_decoder_factory_ = NULL;

	{
		rtc::CritScope l(&cs_lst_vid_data_);
		while (lst_vid_data_cache_.size() > 0) {
			VidData* vidData = lst_vid_data_cache_.front();
			lst_vid_data_cache_.pop_front();

			delete vidData;
		}

		while (lst_vid_data_.size() > 0) {
			VidData* vidData = lst_vid_data_.front();
			lst_vid_data_.pop_front();

			delete vidData;
		}
	}
}

void V_H264Decoder::SetIdd(const char* strIdd)
{
	str_idd_ = strIdd;
}
void V_H264Decoder::SetNeedFirstDecodeFrame(bool bNeed)
{
	if (bNeed)
	{
		has_decoded_ = false;
		has_dec_frame_ = false;
	}
	else {
		has_decoded_ = true;
		has_dec_frame_ = true;
	}
}
void V_H264Decoder::SetStatusEvent(RtcVidDeocoderStatusEvent* pEvent)
{
	has_video_ = false;
	status_event_ = pEvent;
}
void V_H264Decoder::SetVideoData(bool bKeyFrame, const char* pData, int nLen)
{
	has_video_ = true;
	VidData* vidData = NULL;
	rtc::CritScope l(&cs_lst_vid_data_);
	if (bKeyFrame) {//关键帧则清空队列
		while (lst_vid_data_.size() > 0) {
			VidData* tpData = lst_vid_data_.front();
			lst_vid_data_.pop_front();

			lst_vid_data_cache_.push_back(tpData);
		}
	}
	if (lst_vid_data_cache_.size() > 0) {
		vidData = lst_vid_data_cache_.front();
		lst_vid_data_cache_.pop_front();
	}
	if (vidData == NULL) {
		vidData = new VidData();
	}
	if (pData[0] == '*') {
		// 5bytes: * 00(rotate) 00 00 00
		vidData->SetData(bKeyFrame, pData + 5, nLen - 5);
		vidData->nRotate = pData[1];
	}
	else {
		vidData->SetData(bKeyFrame, pData, nLen);
	}

	lst_vid_data_.push_back(vidData);
}

void V_H264Decoder::SetNeedKeyFrame()
{
	need_keyframe_ = true;
}

//* For Thread
void V_H264Decoder::Run()
{
	bool bGotData = false;
	while (running_) {
		bGotData = false;
		VidData* vidData = GetVidData();
		if (vidData != NULL) {
			bGotData = true;
			if (vid_decoder_ == NULL)
			{
				webrtc::SdpVideoFormat sdpFormat(cricket::kH264CodecName);
				if (video_decoder_factory_ != NULL) {
					vid_decoder_ = video_decoder_factory_->CreateVideoDecoder(sdpFormat);
				}
				if (vid_decoder_ == NULL) {
					vid_decoder_ = webrtc::H264Decoder::Create();
				}
				webrtc::VideoCodec codecSet;
				codecSet.codecType = webrtc::kVideoCodecH264;
				codecSet.width = 640;
				codecSet.height = 480;
				vid_decoder_->InitDecode(&codecSet, 2);
				vid_decoder_->RegisterDecodeCompleteCallback(this);
				need_keyframe_ = true;
			}
			if (vid_decoder_ != NULL)
			{
				if (need_keyframe_)
				{
					if (vidData->bKeyFrame) {
						need_keyframe_ = false;
					}
				}
				if (!need_keyframe_)
				{
					if (!has_decoded_ && vidData->bKeyFrame)
					{
						has_decoded_ = true;
						int w = 640;
						int h = 480;
						int nStart = 0;
						int nFind = nStart;
						int nFind7 = -1;
						while ((nFind + 4) < vidData->nLen)
						{
							char* ptr = vidData->pData + nFind;
							if (ptr[0] == 0x0 && ptr[1] == 0x0 && ptr[2] == 0x0 && ptr[3] == 0x1)
							{
								int nType = ptr[4] & 0x1f;
								if (nType == 7) {
									nFind7 = nFind;
								}
								if (nType == 8)
								{
									if (nFind7 >= 0)
									{
										int size7 = nFind - nFind7 - 4;
										char* ptr7 = vidData->pData + nFind7 + 4;

										//h264_decode_seq_parameter_set((uint8_t*)ptr7, size7, w, h);
									}
									break;
								}
							}
							nFind++;
						}

						v_width_ = w;
						v_height_ = h;
						if (status_event_ != NULL) {
							status_event_->OnFirstRemoteVideoDecoded(str_idd_.c_str(), w, h);
						}
					}

					webrtc::EncodedImage encodeData;
					encodeData.SetTimestamp(rtc::Time32());
					encodeData.ntp_time_ms_ = rtc::TimeMillis();
					encodeData.SetEncodedData(webrtc::EncodedImageBuffer::Create((uint8_t*)vidData->pData, vidData->nLen));
					//encodeData.set_size(vidData->nLen);
					//encodeData.set_buffer((uint8_t*)vidData->pData, vidData->nLen + 8);
					encodeData.rotation_ = (webrtc::VideoRotation)vidData->nRotate;

					n_decoded_time_ = rtc::Time32();
					if (vid_decoder_->Decode(encodeData, false, rtc::TimeMillis()) != 0)
					{
						vid_decoder_->RegisterDecodeCompleteCallback(NULL);
						vid_decoder_->Release();
						vid_decoder_ = NULL;
					}
					n_decoded_used_ = rtc::Time32() - n_decoded_time_;
				}
			}

			//RTC_LOG(LS_NONE) << "* Dec use time: " << n_decoded_used_;
			CacheVidData(vidData);
		}
		if (v_report_time_ <= rtc::Time32()) {
			v_report_time_ = rtc::Time32() + 1000;
			if (status_event_ != NULL) {
				status_event_->OnRemoteVideoFrameSizeReport(str_idd_.c_str(), v_report_w_, v_report_h_);
			}
			v_report_w_ = 0;
			v_report_h_ = 0;
		}
		if (bGotData) {
			rtc::Thread::SleepMs(1);
		}
		else {
			rtc::Thread::SleepMs(5);
		}
	}
}

//* For webrtc::DecodedImageCallback
int32_t V_H264Decoder::Decoded(webrtc::VideoFrame& decodedImage)
{
	if (!has_dec_frame_) {
		has_dec_frame_ = true;
		if (status_event_ != NULL) {
			status_event_->OnFirstRemoteVideoFrame(str_idd_.c_str(), decodedImage.width(), decodedImage.height());
		}
	}

	if (v_width_ != decodedImage.width() || v_height_ != decodedImage.height())
	{
		v_width_ = decodedImage.width();
		v_height_ = decodedImage.height();
		if (status_event_ != NULL) {
			status_event_->OnRemoteVideoFrameSizeChange(str_idd_.c_str(), v_width_, v_height_);
		}
	}
	v_report_w_ = v_width_;
	v_report_h_ = v_height_;

	//callback_.OnDecodeFrame(str_chan_id_, str_idd_, decodedImage);
	const webrtc::I420BufferInterface* yuv420 = decodedImage.video_frame_buffer()->GetI420();
	callback_.OnDecodeFrame(str_idd_.c_str(), (char*)yuv420->DataY(), (char*)yuv420->DataU(), (char*)yuv420->DataV(), yuv420->StrideY(), yuv420->StrideU(), yuv420->StrideV(), yuv420->width(), yuv420->height(), decodedImage.rotation(), decodedImage.timestamp());
	return 0;
}

void V_H264Decoder::CacheVidData(VidData* vidData)
{
	rtc::CritScope l(&cs_lst_vid_data_);
	lst_vid_data_cache_.push_back(vidData);
}
VidData* V_H264Decoder::GetVidData()
{
	VidData* vidData = NULL;
	{
		rtc::CritScope l(&cs_lst_vid_data_);
		if (lst_vid_data_.size() > 0) {
			vidData = lst_vid_data_.front();
			lst_vid_data_.pop_front();

			//RTC_LOG(LS_NONE) << "lst_vid_data_ size: " << lst_vid_data_.size();
		}
	}
	return vidData;
}


}	// namespace webrtc

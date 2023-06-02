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
#ifndef __AV_CODEC_H__
#define __AV_CODEC_H__
#include "api/video_codecs/video_decoder.h"
#include "api/video_codecs/video_encoder.h"
#include "api/video/video_frame.h"
#include "api/video/i420_buffer.h"
#include "rtc_base/deprecated/recursive_critical_section.h"
#include "rtc_base/thread.h"
#include "rtc_base/thread_annotations.h"
#include "common_video/video_render_frames.h"
#include "modules/audio_coding/acm2/acm_resampler.h"
#include "modules/audio_device/include/audio_device.h"
#include "api/video_codecs/video_encoder_factory.h"
#include "api/video_codecs/video_decoder_factory.h"
#include "pluginaac.h"

namespace webrtc {
	enum VideoScaleMode {

		/// 图像铺满屏幕，超出显示视窗的视频部分将被裁剪，画面显示可能不完整
		VideoScaleModeFill,

		/// 图像长边填满屏幕，短边区域会被填充黑色，画面的内容完整
		VideoScaleModeFit,

		/// 图像长边填满屏幕，根据设置的比例进行缩放，画面的内容完整
		VideoScaleModeAuto,

	};

class AVCodecCallback
{
public:
	AVCodecCallback(void){};
	virtual ~AVCodecCallback(void){};

	virtual void OnEncodeDataCallback(bool audio, bool bKeyFrame, const uint8_t *p, uint32_t length, uint32_t ts) = 0;
};

class A_AACEncoder 
{
public:
	A_AACEncoder(AVCodecCallback&callback);
	virtual ~A_AACEncoder(void);

	bool Init(int sample_rate, int num_channels, int bitrate);
	void DeInit();

	int Encode(const void* audioSamples, const size_t nSamples, const size_t nBytesPerSample, 
		const size_t nChannels, const uint32_t samplesPerSec, const uint32_t totalDelayMS);


private:
	AVCodecCallback& callback_;

	aac_enc_t	encoder_;

	webrtc::acm2::ACMResampler resampler_record_;
	int						audio_record_sample_hz_;
	int						audio_record_channels_;
};

class V_H264Encoder : public rtc::Thread, public EncodedImageCallback
{
public:
	V_H264Encoder(AVCodecCallback&callback);
	virtual ~V_H264Encoder(void);

	void SetExVideoEncoderFactory(webrtc::VideoEncoderFactory* video_encoder_factory = NULL);
	void SetParameter(int width, int height, int fps, int bitrate);
	void SetMirror(bool bMirror);
	void SetVideoScaleMode(VideoScaleMode eMode);
	void UpdateBitrate(int bitrate);
	void CreateVideoEncoder();
	void DestoryVideoEncoder();
	void RequestKeyFrame();
	void Encode(const webrtc::VideoFrame& frame);

	//* For Thread
	virtual void Run();

	virtual Result OnEncodedImage(
		const EncodedImage& encoded_image,
		const CodecSpecificInfo* codec_specific_info);

protected:
	void NewVideoEncoder();
	void FreeVideoEncoder();

	void AddToFrameList(webrtc::VideoFrame& frame);

private:
	bool		running_;
	bool		need_keyframe_;
	bool		b_mirror_;
	VideoScaleMode	e_scale_mode_;
	int64_t		n_next_keyframe_time_;
	AVCodecCallback& callback_;
	VideoCodec		h264_;
	webrtc::VideoEncoderFactory*	video_encoder_factory_;
	std::unique_ptr<VideoEncoder>	encoder_;
	rtc::RecursiveCriticalSection buffer_critsect_;
	std::unique_ptr<VideoRenderFrames> render_buffers_;

private:
	rtc::scoped_refptr<webrtc::I420Buffer> video_mirror_buffer_;
    
    rtc::scoped_refptr<webrtc::I420Buffer>  video_encode_buffer_;
};


//==================================================================================
//
struct VidData
{
	VidData(void) :pData(NULL), nLen(0), nSize(0), bKeyFrame(false), nRotate(0) {};
	virtual ~VidData() {
		if (pData != NULL) {
			delete[] pData;
			pData = NULL;
		}
	}

	void SetData(bool keyFrame, const char* data, int len)
	{
		if (nSize < len || pData == NULL) {
			if (pData != NULL) {
				delete[] pData;
				pData = NULL;
			}
			nSize = len;
			pData = new char[nSize + 8];
		}

		nLen = len;
		bKeyFrame = keyFrame;
		memcpy(pData, data, len);
	}

	char* pData;
	int nLen;
	int nSize;
	bool bKeyFrame;
	int	nRotate;
};
class RtcVidDecoderEvent
{
public:
	RtcVidDecoderEvent(void) {};
	virtual ~RtcVidDecoderEvent(void) {};

	virtual void OnDecodeFrame(const char* strIdd, const char* yData, const char* uData, const char* vData, int strideY, int strideU, int strideV, int w, int h, int rotate, unsigned int timeStamp) = 0;
};
class RtcVidDeocoderStatusEvent
{
public:
	RtcVidDeocoderStatusEvent(void) {};
	virtual ~RtcVidDeocoderStatusEvent(void) {};

	virtual void OnFirstRemoteVideoDecoded(const char* strIdd, int w, int h) = 0;
	virtual void OnFirstRemoteVideoFrame(const char* strIdd, int w, int h) = 0;
	virtual void OnRemoteVideoFrameSizeChange(const char* strIdd, int w, int h) = 0;
	virtual void OnRemoteVideoFrameSizeReport(const char* strIdd, int w, int h) = 0;
};

class V_H264Decoder : public rtc::Thread, public webrtc::DecodedImageCallback
{
public:
	V_H264Decoder(RtcVidDecoderEvent& callback);
	virtual ~V_H264Decoder(void);

	//* For RtcVidDecoder
	virtual void SetIdd(const char* strIdd);
	virtual void SetNeedFirstDecodeFrame(bool bNeed);
	virtual void SetStatusEvent(RtcVidDeocoderStatusEvent* pEvent);
	virtual void SetVideoData(bool bKeyFrame, const char* pData, int nLen);
	virtual void SetNeedKeyFrame();

	uint32_t GetDecodedTimeUsed() { return n_decoded_used_; };
	bool HasVideo() { return has_video_; };

	//* For Thread
	virtual void Run();

	//* For webrtc::DecodedImageCallback
	virtual int32_t Decoded(webrtc::VideoFrame& decodedImage);

private:
	void CacheVidData(VidData* vidData);
	VidData* GetVidData();

private:
	RtcVidDecoderEvent& callback_;
	RtcVidDeocoderStatusEvent* status_event_;
	bool				running_;
	bool				has_decoded_;
	bool				has_dec_frame_;
	bool				has_video_;
	bool                need_keyframe_;
	int					v_width_;
	int					v_height_;
	int					v_report_w_;
	int					v_report_h_;
	uint32_t			v_report_time_;
	std::string			str_idd_;

	uint32_t			n_decoded_time_;
	uint32_t			n_decoded_used_;

	std::unique_ptr<webrtc::VideoDecoderFactory> video_decoder_factory_;

	std::unique_ptr<webrtc::VideoDecoder>		vid_decoder_;

	rtc::RecursiveCriticalSection cs_lst_vid_data_;
	std::list<VidData*> lst_vid_data_;
	std::list<VidData*> lst_vid_data_cache_;
};

}	// namespace webrtc

#endif	// __AV_CODEC_H__

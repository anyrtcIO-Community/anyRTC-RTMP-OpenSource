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
#include "webrtc/audio_sink.h"
#include "webrtc/video_decoder.h"
#include "webrtc/video_encoder.h"
#include "webrtc/video_frame.h"
#include "webrtc/api/mediastreaminterface.h"
#include "webrtc/base/criticalsection.h"
#include "webrtc/base/thread.h"
#include "webrtc/base/scoped_ptr.h"
#include "webrtc/base/thread_annotations.h"
#include "webrtc/common_video/video_render_frames.h"
#include "webrtc/modules/audio_coding/acm2/acm_resampler.h"
#include "webrtc/modules/audio_device/include/audio_device.h"
#include "webrtc/media/engine/webrtcvideodecoderfactory.h"
#include "webrtc/media/engine/webrtcvideoencoderfactory.h"
#include "pluginaac.h"

namespace webrtc {

class AVCodecCallback
{
public:
	AVCodecCallback(void){};
	virtual ~AVCodecCallback(void){};

	virtual void OnEncodeDataCallback(bool audio, uint8_t *p, uint32_t length, uint32_t ts) = 0;
};

class A_AACEncoder : public webrtc::AudioSinkInterface
{
public:
	A_AACEncoder(AVCodecCallback&callback);
	virtual ~A_AACEncoder(void);

	bool Init(int num_channels, int sample_rate, int pcm_bit_size);
	void Muted(bool enable){muted_ = enable;};
	void StartEncoder();
	void StopEncoder();

	int Encode(const void* audioSamples, const size_t nSamples, const size_t nBytesPerSample, 
		const size_t nChannels, const uint32_t samplesPerSec, const uint32_t totalDelayMS);

protected:

	//* For webrtc::AudioSinkInterface
	virtual void OnData(const Data& audio);

private:
	AVCodecCallback& callback_;
	bool        running_;
	bool        muted_;
    bool        encoded_;

	aac_enc_t	encoder_;

	webrtc::acm2::ACMResampler resampler_record_;
	int						audio_record_sample_hz_;
	int						audio_record_channels_;

	rtc::CriticalSection buffer_critsect_;
	std::list<void*>	audio_buffer_;
};

class V_H264Encoder : public rtc::Thread, public rtc::VideoSinkInterface<cricket::VideoFrame> , public EncodedImageCallback
{
public:
	V_H264Encoder(AVCodecCallback&callback);
	virtual ~V_H264Encoder(void);

	void Init(cricket::WebRtcVideoEncoderFactory* video_encoder_factory = NULL);
	void SetParameter(int width, int height, int fps, int bitrate);
	void SetRates(int bitrate);
    void SetAutoAdjustBit(bool enabled);
    void SetNetDelay(int delayMs);
	void CreateVideoEncoder();
	void StartEncoder();
	void StopEncoder();
	void RequestKeyFrame();

	//* For Thread
	virtual void Run();

	//* For VideoSinkInterface
	virtual void OnFrame(const cricket::VideoFrame& frame);

	virtual int32_t Encoded(const EncodedImage& encoded_image,
                          const CodecSpecificInfo* codec_specific_info,
                          const RTPFragmentationHeader* fragmentation);

private:
	bool		running_;
	bool		need_keyframe_;
    bool        encoded_;
    int         video_bitrate_;
    int         delay_ms_;
    int         adjust_v_bitrate_;
	AVCodecCallback& callback_;
	VideoCodec		h264_;
	cricket::WebRtcVideoEncoderFactory*	video_encoder_factory_;
	VideoEncoder*	encoder_;
	rtc::CriticalSection buffer_critsect_;
	rtc::scoped_ptr<VideoRenderFrames> render_buffers_
      GUARDED_BY(buffer_critsect_);
};

}	// namespace webrtc

#endif	// __AV_CODEC_H__
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
#ifndef __PLAYER_DECODER_H__
#define __PLAYER_DECODER_H__
#include "plybuffer.h"
#include "pluginaac.h"
#include "anyrtmpcore.h"
#include "webrtc/base/thread.h"
#include "webrtc/common_audio/ring_buffer.h"
#include "webrtc/modules/audio_coding/acm2/acm_resampler.h"
#include "webrtc/modules/audio_device/include/audio_device.h"
#include "webrtc/modules/audio_device/include/audio_device_defines.h"
#include "webrtc/modules/video_coding/codecs/h264/include/h264.h"
#include "webrtc/api/mediastreaminterface.h"

class PlyDecoder : public rtc::Thread, PlyBufferCallback, public webrtc::DecodedImageCallback
{
public:
	PlyDecoder();
	virtual ~PlyDecoder();

	void SetVideoRender(rtc::VideoSinkInterface<cricket::VideoFrame> *render){ video_render_ = render; };
    bool IsPlaying();
    int  CacheTime();

	void AddH264Data(const uint8_t*pdata, int len, uint32_t ts);
	void AddAACData(const uint8_t*pdata, int len, uint32_t ts);
	int GetPcmData(void* audioSamples, uint32_t& samplesPerSec, size_t& nChannels);

protected:
	//* For Thread
	virtual void Run();

	//* For PlyBufferCallback
	virtual void OnPlay();
	virtual void OnPause();
	virtual bool OnNeedDecodeData(PlyPacket* pkt);

	//* For webrtc::DecodedImageCallback
	virtual int32_t Decoded(webrtc::VideoFrame& decodedImage);

private:
	bool			running_;
	bool			playing_;
	PlyBuffer*		ply_buffer_;
	
	//* For video
	webrtc::VideoDecoder	*h264_decoder_;
	rtc::CriticalSection	cs_list_h264_;
	std::list<PlyPacket*>	lst_h264_buffer_;
	rtc::VideoSinkInterface<cricket::VideoFrame>	*video_render_;

	//* For audio
	aac_dec_t		aac_decoder_;
	uint8_t			audio_cache_[8192];
	int				a_cache_len_;
	uint32_t		aac_sample_hz_;
	uint8_t			aac_channels_;
	uint32_t		aac_frame_per10ms_size_;
};

#endif	// __PLAYER_DECODER_H__


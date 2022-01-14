#include "flv-demuxer.h"
#include "flv-header.h"
#include "flv-proto.h"
#include "mpeg4-aac.h"
#include "mpeg4-avc.h"
#include "mpeg4-hevc.h"
#include "opus-head.h"
#include "aom-av1.h"
#include "amf0.h"
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <errno.h>

struct flv_demuxer_t
{
	union
	{
		struct mpeg4_aac_t aac;
		struct opus_head_t opus;
	} a;

	union
	{
		struct aom_av1_t av1;
		struct mpeg4_avc_t avc;
		struct mpeg4_hevc_t hevc;
	} v;

	flv_demuxer_handler handler;
	void* param;

	uint8_t* ptr;
	int capacity;
};

struct flv_demuxer_t* flv_demuxer_create(flv_demuxer_handler handler, void* param)
{
	struct flv_demuxer_t* flv;
	flv = (struct flv_demuxer_t*)malloc(sizeof(struct flv_demuxer_t));
	if (NULL == flv)
		return NULL;

	memset(flv, 0, sizeof(struct flv_demuxer_t));
	flv->handler = handler;
	flv->param = param;
	return flv;
}

void flv_demuxer_destroy(struct flv_demuxer_t* flv)
{
	if (flv->ptr)
	{
		assert(flv->capacity > 0);
		free(flv->ptr);
	}

	free(flv);
}

static int flv_demuxer_check_and_alloc(struct flv_demuxer_t* flv, int bytes)
{
	if (bytes > flv->capacity)
	{
		void* p = realloc(flv->ptr, bytes);
		if (NULL == p)
			return -1;
		flv->ptr = (uint8_t*)p;
		flv->capacity = bytes;
	}
	return 0;
}

static int flv_demuxer_audio(struct flv_demuxer_t* flv, const uint8_t* data, int bytes, uint32_t timestamp)
{
	int r, n;
	struct flv_audio_tag_header_t audio;
	n = flv_audio_tag_header_read(&audio, data, bytes);
	if (n < 0)
		return n;

	if (FLV_AUDIO_AAC == audio.codecid)
	{
		// Adobe Flash Video File Format Specification Version 10.1 >> E.4.2.1 AUDIODATA (p77)
		// If the SoundFormat indicates AAC, the SoundType should be 1 (stereo) and the SoundRate should be 3 (44 kHz).
		// However, this does not mean that AAC audio in FLV is always stereo, 44 kHz data.Instead, the Flash Player ignores
		// these values and extracts the channel and sample rate data is encoded in the AAC bit stream.
		//assert(3 == audio.bitrate && 1 == audio.channel);
		if (FLV_SEQUENCE_HEADER == audio.avpacket)
		{
			mpeg4_aac_audio_specific_config_load(data + n, bytes - n, &flv->a.aac);
			return flv->handler(flv->param, FLV_AUDIO_ASC, data + n, bytes - n, timestamp, timestamp, 0);
		}
		else
		{
			if (0 != flv_demuxer_check_and_alloc(flv, bytes + 7 + 1 + flv->a.aac.npce))
				return -ENOMEM;

			// AAC ES stream with ADTS header
			assert(bytes <= 0x1FFF);
			assert(bytes > 2 && 0xFFF0 != (((data[2] << 8) | data[3]) & 0xFFF0)); // don't have ADTS
			r = mpeg4_aac_adts_save(&flv->a.aac, (uint16_t)bytes - n, flv->ptr, 7 + 1 + flv->a.aac.npce); // 13-bits
			if (r < 7) return -EINVAL; // invalid pce
			flv->a.aac.npce = 0; // pce write only once
			memmove(flv->ptr + r, data + n, bytes - n);
			return flv->handler(flv->param, FLV_AUDIO_AAC, flv->ptr, bytes - n + r, timestamp, timestamp, 0);
		}
	}
	else if (FLV_AUDIO_OPUS == audio.codecid)
	{
		if (FLV_SEQUENCE_HEADER == audio.avpacket)
		{
			opus_head_load(data + n, bytes - n, &flv->a.opus);
			return flv->handler(flv->param, FLV_AUDIO_OPUS_HEAD, data + n, bytes - n, timestamp, timestamp, 0);
		}
		else
		{
			return flv->handler(flv->param, audio.codecid, data + n, bytes - n, timestamp, timestamp, 0);
		}
	}
	else if (FLV_AUDIO_MP3 == audio.codecid || FLV_AUDIO_MP3_8K == audio.codecid)
	{
		return flv->handler(flv->param, audio.codecid, data + n, bytes - n, timestamp, timestamp, 0);
	}
	else
	{
		// Audio frame data
		return flv->handler(flv->param, audio.codecid, data + n, bytes - n, timestamp, timestamp, 0);
	}
}

static int flv_demuxer_video(struct flv_demuxer_t* flv, const uint8_t* data, int bytes, uint32_t timestamp)
{
	int n;
	struct flv_video_tag_header_t video;
	n = flv_video_tag_header_read(&video, data, bytes);
	if (n < 0)
		return n;

	if (FLV_VIDEO_H264 == video.codecid)
	{
		if (FLV_SEQUENCE_HEADER == video.avpacket)
		{
			// AVCDecoderConfigurationRecord
			assert(bytes > n + 7);
			mpeg4_avc_decoder_configuration_record_load(data + n, bytes - n, &flv->v.avc);
			return flv->handler(flv->param, FLV_VIDEO_AVCC, data + n, bytes - n, timestamp + video.cts, timestamp, 0);
		}
		else if(FLV_AVPACKET == video.avpacket)
		{
			assert(flv->v.avc.nalu > 0); // parse AVCDecoderConfigurationRecord failed
			if (flv->v.avc.nalu > 0 && bytes > n) // 5 ==  bytes flv eof
			{
				// H.264
				if (0 != flv_demuxer_check_and_alloc(flv, bytes + 4 * 1024))
					return -ENOMEM;

				assert(flv->v.avc.nalu <= 4);
				n = h264_mp4toannexb(&flv->v.avc, data + n, bytes - n, flv->ptr, flv->capacity);
				if (n <= 0 || n > flv->capacity)
				{
					assert(0);
					return -ENOMEM;
				}
				return flv->handler(flv->param, FLV_VIDEO_H264, flv->ptr, n, timestamp + video.cts, timestamp, (FLV_VIDEO_KEY_FRAME == video.keyframe) ? 1 : 0);
			}
			return -EINVAL;
		}
		else if (FLV_END_OF_SEQUENCE == video.avpacket)
		{
			return 0; // AVC end of sequence (lower level NALU sequence ender is not required or supported)
		}
		else
		{
			assert(0);
			return -EINVAL;
		}
	}
	else if (FLV_VIDEO_H265 == video.codecid)
	{
		if (FLV_SEQUENCE_HEADER == video.avpacket)
		{
			// HEVCDecoderConfigurationRecord
			assert(bytes > n + 7);
			mpeg4_hevc_decoder_configuration_record_load(data + n, bytes - n, &flv->v.hevc);
			return flv->handler(flv->param, FLV_VIDEO_HVCC, data + n, bytes - n, timestamp + video.cts, timestamp, 0);
		}
		else if (FLV_AVPACKET == video.avpacket)
		{
			assert(flv->v.hevc.numOfArrays > 0); // parse HEVCDecoderConfigurationRecord failed
			if (flv->v.hevc.numOfArrays > 0 && bytes > n) // 5 ==  bytes flv eof
			{
				// H.265
				if (0 != flv_demuxer_check_and_alloc(flv, bytes + 4 * 1024))
					return -ENOMEM;

				n = h265_mp4toannexb(&flv->v.hevc, data + n, bytes - n, flv->ptr, flv->capacity);
				if (n <= 0 || n > flv->capacity)
				{
					assert(0);
					return -ENOMEM;
				}
				return flv->handler(flv->param, FLV_VIDEO_H265, flv->ptr, n, timestamp + video.cts, timestamp, (FLV_VIDEO_KEY_FRAME == video.keyframe) ? 1 : 0);
			}
			return -EINVAL;
		}
		else if (FLV_END_OF_SEQUENCE == video.avpacket)
		{
			return 0; // AVC end of sequence (lower level NALU sequence ender is not required or supported)
		}
		else
		{
			assert(0);
			return -EINVAL;
		}
	}
	else if (FLV_VIDEO_AV1 == video.codecid)
	{
		if (FLV_SEQUENCE_HEADER == video.avpacket)
		{
			// AV1CodecConfigurationRecord
			assert(bytes > n + 5);
			aom_av1_codec_configuration_record_load(data + n, bytes - n, &flv->v.av1);
			return flv->handler(flv->param, FLV_VIDEO_AV1C, data + n, bytes - n, timestamp + video.cts, timestamp, 0);
		}
		else if (FLV_AVPACKET == video.avpacket)
		{
			return flv->handler(flv->param, FLV_VIDEO_AV1, data + n, bytes - n, timestamp + video.cts, timestamp, (FLV_VIDEO_KEY_FRAME == video.keyframe) ? 1 : 0);
		}
		else if (FLV_END_OF_SEQUENCE == video.avpacket)
		{
			return 0; // AV1 end of sequence (lower level NALU sequence ender is not required or supported)
		}
		else
		{
			assert(0);
			return -EINVAL;
		}
	}
	else
	{
		// Video frame data
		return flv->handler(flv->param, video.codecid, data + n, bytes - n, timestamp + video.cts, timestamp, (FLV_VIDEO_KEY_FRAME==video.keyframe) ? 1 : 0);
	}
}

int flv_demuxer_script(struct flv_demuxer_t* flv, const uint8_t* data, size_t bytes);
int flv_demuxer_input(struct flv_demuxer_t* flv, int type, const void* data, size_t bytes, uint32_t timestamp)
{
	int n;
	if (bytes < 1)
		return 0;

	switch (type)
	{
	case FLV_TYPE_AUDIO:
		return flv_demuxer_audio(flv, data, (int)bytes, timestamp);

	case FLV_TYPE_VIDEO:
		return flv_demuxer_video(flv, data, (int)bytes, timestamp);

	case FLV_TYPE_SCRIPT:
		n = flv_demuxer_script(flv, data, bytes);
		if (n < 12)
			return 0; // ignore
		n -= 12; // 2-LEN + 10-onMetaData
		return flv->handler(flv->param, FLV_SCRIPT_METADATA, (const uint8_t*)data + n, bytes - n, timestamp, timestamp, 0);
		
	default:
		assert(0);
		return -1;
	}
}

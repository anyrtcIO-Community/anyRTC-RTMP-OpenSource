#include "flv-muxer.h"
#include "flv-proto.h"
#include "flv-header.h"
#include "amf0.h"
#include <stdio.h>
#include <errno.h>
#include <stdlib.h>
#include <assert.h>
#include <stdint.h>
#include <string.h>
#include "aom-av1.h"
#include "mpeg4-aac.h"
#include "mpeg4-avc.h"
#include "mpeg4-hevc.h"
#include "mp3-header.h"
#include "opus-head.h"

#define FLV_MUXER "ireader/media-server"

struct flv_muxer_t
{
	flv_muxer_handler handler;
	void* param;

	uint8_t audio_sequence_header;
	uint8_t video_sequence_header;

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
	int vcl; // 0-non vcl, 1-idr, 2-p/b
	int update; // avc/hevc sequence header update

	uint8_t* ptr;
	int bytes;
	int capacity;
};

struct flv_muxer_t* flv_muxer_create(flv_muxer_handler handler, void* param)
{
	struct flv_muxer_t* flv;
	flv = (struct flv_muxer_t*)calloc(1, sizeof(struct flv_muxer_t));
	if (NULL == flv)
		return NULL;

	flv_muxer_reset(flv);
	flv->handler = handler;
	flv->param = param;
	return flv;
}

void flv_muxer_destroy(struct flv_muxer_t* flv)
{
	if (flv->ptr)
	{
		assert(flv->capacity > 0);
		free(flv->ptr);
		flv->ptr = NULL;
	}

	free(flv);
}

int flv_muxer_reset(struct flv_muxer_t* flv)
{
	memset(&flv->v, 0, sizeof(flv->v));
	flv->audio_sequence_header = 0;
	flv->video_sequence_header = 0;
	return 0;
}

static int flv_muxer_alloc(struct flv_muxer_t* flv, int bytes)
{
	void* p;
	p = realloc(flv->ptr, bytes);
	if (!p)
		return ENOMEM;

	flv->ptr = (uint8_t*)p;
	flv->capacity = bytes;
	return 0;
}

int flv_muxer_g711a(struct flv_muxer_t* flv, const void* data, size_t bytes, uint32_t pts, uint32_t dts)
{
	struct flv_audio_tag_header_t audio;
	(void)pts;

	if (flv->capacity < bytes + 1)
	{
		if (0 != flv_muxer_alloc(flv, bytes + 4))
			return ENOMEM;
	}

	audio.bits = 1; // 16-bit samples
	audio.channels = 0;
	audio.rate = 0;
	audio.codecid = FLV_AUDIO_G711A;
	audio.avpacket = FLV_AVPACKET;
	flv_audio_tag_header_write(&audio, flv->ptr, 1);
	memcpy(flv->ptr + 1, data, bytes);
	return flv->handler(flv->param, FLV_TYPE_AUDIO, flv->ptr, bytes + 1, dts);
}

int flv_muxer_g711u(struct flv_muxer_t* flv, const void* data, size_t bytes, uint32_t pts, uint32_t dts)
{
	struct flv_audio_tag_header_t audio;
	(void)pts;

	if (flv->capacity < bytes + 1)
	{
		if (0 != flv_muxer_alloc(flv, bytes + 4))
			return ENOMEM;
	}

	audio.bits = 1; // 16-bit samples
	audio.channels = 0;
	audio.rate = 0;
	audio.codecid = FLV_AUDIO_G711U;
	audio.avpacket = FLV_AVPACKET;
	flv_audio_tag_header_write(&audio, flv->ptr, 1);
	memcpy(flv->ptr + 1, data, bytes);
	return flv->handler(flv->param, FLV_TYPE_AUDIO, flv->ptr, bytes + 1, dts);
}

int flv_muxer_mp3(struct flv_muxer_t* flv, const void* data, size_t sz, uint32_t pts, uint32_t dts)
{
    int bytes;
	struct mp3_header_t mp3;
	struct flv_audio_tag_header_t audio;
	(void)pts;

    bytes = (int)sz;
	if (0 == mp3_header_load(&mp3, data, bytes))
	{
		return EINVAL;
	}
	else
	{
		audio.channels = 3 == mp3.mode ? 0 : 1;
		switch (mp3_get_frequency(&mp3))
		{
		case 5500: audio.rate = 0; break;
		case 11025: audio.rate = 1; break;
		case 22050: audio.rate = 2; break;
		case 44100: audio.rate = 3; break;
		default: audio.rate = 3;
		}
	}

	if (flv->capacity < bytes + 1)
	{
		if (0 != flv_muxer_alloc(flv, bytes + 4))
			return ENOMEM;
	}

	audio.bits = 1; // 16-bit samples
	audio.codecid = FLV_AUDIO_MP3;
	audio.avpacket = FLV_AVPACKET;
	flv_audio_tag_header_write(&audio, flv->ptr, 1);
	memcpy(flv->ptr + 1, data, bytes); // MP3
	return flv->handler(flv->param, FLV_TYPE_AUDIO, flv->ptr, bytes + 1, dts);
}

int flv_muxer_aac(struct flv_muxer_t* flv, const void* data, size_t sz, uint32_t pts, uint32_t dts)
{
    int r, n, m, bytes;
	struct flv_audio_tag_header_t audio;
	(void)pts;

    bytes = (int)sz;
	if (flv->capacity < bytes + 2/*AudioTagHeader*/ + 2/*AudioSpecificConfig*/)
	{
		if (0 != flv_muxer_alloc(flv, bytes + 4))
			return ENOMEM;
	}

	/* ADTS */
	n = mpeg4_aac_adts_load(data, bytes, &flv->a.aac);
	if (n <= 0)
		return -1; // invalid data

	audio.codecid = FLV_AUDIO_AAC;
	audio.rate = 3; // 44k-SoundRate
	audio.bits = 1; // 16-bit samples
	audio.channels = 1; // Stereo sound
	if (0 == flv->audio_sequence_header)
	{
		flv->audio_sequence_header = 1; // once only
		audio.avpacket = FLV_SEQUENCE_HEADER;

		// AudioSpecificConfig(AAC sequence header)
		flv_audio_tag_header_write(&audio, flv->ptr, flv->capacity);
		m = mpeg4_aac_audio_specific_config_save(&flv->a.aac, flv->ptr + 2, flv->capacity - 2);
		assert(m + 2 <= (int)flv->capacity);
		r = flv->handler(flv->param, FLV_TYPE_AUDIO, flv->ptr, m + 2, dts);
		if (0 != r) return r;
	}

	audio.avpacket = FLV_AVPACKET;
	flv_audio_tag_header_write(&audio, flv->ptr, flv->capacity);
	memcpy(flv->ptr + 2, (uint8_t*)data + n, bytes - n); // AAC exclude ADTS
	assert(bytes - n + 2 <= (int)flv->capacity);
	return flv->handler(flv->param, FLV_TYPE_AUDIO, flv->ptr, bytes - n + 2, dts);
}

int flv_muxer_opus(flv_muxer_t* flv, const void* data, size_t sz, uint32_t pts, uint32_t dts)
{
	int r, m, bytes;
	struct flv_audio_tag_header_t audio;
	(void)pts;

	bytes = (int)sz;
	if (flv->capacity < bytes + 2/*AudioTagHeader*/ + 29/*OpusHead*/)
	{
		if (0 != flv_muxer_alloc(flv, bytes + 4))
			return ENOMEM;
	}

	audio.codecid = FLV_AUDIO_OPUS;
	audio.rate = 3; // 44k-SoundRate
	audio.bits = 1; // 16-bit samples
	audio.channels = 1; // Stereo sound

	if (0 == flv->audio_sequence_header)
	{
		if (opus_head_load(data, bytes, &flv->a.opus) < 0)
			return -1;

		flv->audio_sequence_header = 1; // once only
		audio.avpacket = FLV_SEQUENCE_HEADER;
		
		// Opus Head
		m = flv_audio_tag_header_write(&audio, flv->ptr, flv->capacity);
        m += opus_head_save(&flv->a.opus, flv->ptr+m, flv->capacity-m);
		assert(m <= (int)flv->capacity);
		r = flv->handler(flv->param, FLV_TYPE_AUDIO, flv->ptr, m, dts);
		if (0 != r) return r;
	}

	audio.avpacket = FLV_AVPACKET;
	m = flv_audio_tag_header_write(&audio, flv->ptr, flv->capacity);
	memcpy(flv->ptr + m, (uint8_t*)data, bytes);
	assert(bytes - m <= (int)flv->capacity);
	return flv->handler(flv->param, FLV_TYPE_AUDIO, flv->ptr, bytes + m, dts);
}

static int flv_muxer_h264(struct flv_muxer_t* flv, uint32_t pts, uint32_t dts)
{
	int r;
	int m;
	struct flv_video_tag_header_t video;

	video.codecid = FLV_VIDEO_H264;
	if ( /*0 == flv->video_sequence_header &&*/ flv->update && flv->v.avc.nb_sps > 0 && flv->v.avc.nb_pps > 0)
	{
		video.cts = 0;
		video.keyframe = 1; // keyframe
		video.avpacket = FLV_SEQUENCE_HEADER;
		flv_video_tag_header_write(&video, flv->ptr + flv->bytes, flv->capacity - flv->bytes);
		m = mpeg4_avc_decoder_configuration_record_save(&flv->v.avc, flv->ptr + flv->bytes + 5, flv->capacity - flv->bytes - 5);
		if (m <= 0)
			return -1; // invalid data

		flv->video_sequence_header = 1; // once only
		assert(flv->bytes + m + 5 <= (int)flv->capacity);
		r = flv->handler(flv->param, FLV_TYPE_VIDEO, flv->ptr + flv->bytes, m + 5, dts);
		if (0 != r) return r;
	}

	// has video frame
	if (flv->vcl && flv->video_sequence_header)
	{
		video.cts = pts - dts;
		video.keyframe = 1 == flv->vcl ? FLV_VIDEO_KEY_FRAME : FLV_VIDEO_INTER_FRAME;
		video.avpacket = FLV_AVPACKET;
		flv_video_tag_header_write(&video, flv->ptr, flv->capacity);
		assert(flv->bytes <= (int)flv->capacity);
		return flv->handler(flv->param, FLV_TYPE_VIDEO, flv->ptr, flv->bytes, dts);
	}
	return 0;
}

int flv_muxer_avc(struct flv_muxer_t* flv, const void* data, size_t bytes, uint32_t pts, uint32_t dts)
{
	if ((size_t)flv->capacity < bytes + sizeof(flv->v.avc) /*AVCDecoderConfigurationRecord*/)
	{
		if (0 != flv_muxer_alloc(flv, (int)bytes + sizeof(flv->v.avc)))
			return ENOMEM;
	}

	flv->bytes = 5;
	flv->bytes += h264_annexbtomp4(&flv->v.avc, data, (int)bytes, flv->ptr + flv->bytes, flv->capacity - flv->bytes, &flv->vcl, &flv->update);
	if (flv->bytes <= 5)
		return ENOMEM;

	return flv_muxer_h264(flv, pts, dts);
}

static int flv_muxer_h265(struct flv_muxer_t* flv, uint32_t pts, uint32_t dts)
{
	int r;
	int m;
	struct flv_video_tag_header_t video;

	video.codecid = FLV_VIDEO_H265;
	if ( /*0 == flv->avc_sequence_header &&*/ flv->update && flv->v.hevc.numOfArrays >= 3) // vps + sps + pps
	{
		video.cts = 0;
		video.keyframe = 1; // keyframe
		video.avpacket = FLV_SEQUENCE_HEADER;
		flv_video_tag_header_write(&video, flv->ptr + flv->bytes, flv->capacity - flv->bytes);
		m = mpeg4_hevc_decoder_configuration_record_save(&flv->v.hevc, flv->ptr + flv->bytes + 5, flv->capacity - flv->bytes - 5);
		if (m <= 0)
			return -1; // invalid data

		flv->video_sequence_header = 1; // once only
		assert(flv->bytes + m + 5 <= (int)flv->capacity);
		r = flv->handler(flv->param, FLV_TYPE_VIDEO, flv->ptr + flv->bytes, m + 5, dts);
		if (0 != r) return r;
	}

	// has video frame
	if (flv->vcl && flv->video_sequence_header)
	{
		video.cts = pts - dts;
		video.keyframe = 1 == flv->vcl ? FLV_VIDEO_KEY_FRAME : FLV_VIDEO_INTER_FRAME;
		video.avpacket = FLV_AVPACKET;
		flv_video_tag_header_write(&video, flv->ptr, flv->capacity);
		assert(flv->bytes <= (int)flv->capacity);
		return flv->handler(flv->param, FLV_TYPE_VIDEO, flv->ptr, flv->bytes, dts);
	}
	return 0;
}

int flv_muxer_hevc(struct flv_muxer_t* flv, const void* data, size_t bytes, uint32_t pts, uint32_t dts)
{
	if ((size_t)flv->capacity < bytes + sizeof(flv->v.hevc) /*HEVCDecoderConfigurationRecord*/)
	{
		if (0 != flv_muxer_alloc(flv, (int)bytes + sizeof(flv->v.hevc)))
			return ENOMEM;
	}

	flv->bytes = 5;
	flv->bytes += h265_annexbtomp4(&flv->v.hevc, data, (int)bytes, flv->ptr + flv->bytes, flv->capacity - flv->bytes, &flv->vcl, &flv->update);
	if (flv->bytes <= 5)
		return ENOMEM;

	return flv_muxer_h265(flv, pts, dts);
}

int flv_muxer_av1(flv_muxer_t* flv, const void* data, size_t bytes, uint32_t pts, uint32_t dts)
{
	int r;
	int m;
	struct flv_video_tag_header_t video;

	if ((size_t)flv->capacity < bytes + 5 + sizeof(flv->v.av1) /*HEVCDecoderConfigurationRecord*/)
	{
		if (0 != flv_muxer_alloc(flv, (int)bytes + sizeof(flv->v.av1)))
			return ENOMEM;
	}

	video.codecid = FLV_VIDEO_AV1;
	if (0 == flv->video_sequence_header)
	{
		// load av1 information
		r = aom_av1_codec_configuration_record_init(&flv->v.av1, data, bytes);
		if (0 != r || flv->v.av1.width < 1 || flv->v.av1.height < 1)
			return 0 == r ? -1 : r;

		video.cts = 0;
		video.keyframe = 1; // keyframe
		video.avpacket = FLV_SEQUENCE_HEADER;
		flv_video_tag_header_write(&video, flv->ptr + flv->bytes, flv->capacity - flv->bytes);
		m = aom_av1_codec_configuration_record_save(&flv->v.av1, flv->ptr + flv->bytes + 5, flv->capacity - flv->bytes - 5);
		if (m <= 0)
			return -1; // invalid data

		flv->video_sequence_header = 1; // once only
		assert(flv->bytes + m + 5 <= (int)flv->capacity);
		r = flv->handler(flv->param, FLV_TYPE_VIDEO, flv->ptr + flv->bytes, m + 5, dts);
		if (0 != r) return r;
	}

	// has video frame
	if (flv->video_sequence_header)
	{
		video.cts = pts - dts;
		video.keyframe = 1 == flv->vcl ? FLV_VIDEO_KEY_FRAME : FLV_VIDEO_INTER_FRAME;
		video.avpacket = FLV_AVPACKET;
		flv_video_tag_header_write(&video, flv->ptr, flv->capacity);
		memcpy(flv->ptr + 5, data, bytes);
		return flv->handler(flv->param, FLV_TYPE_VIDEO, flv->ptr, bytes + 5, dts);
	}
	return 0;
}

int flv_muxer_metadata(flv_muxer_t* flv, const struct flv_metadata_t* metadata)
{
	uint8_t* ptr, *end;
	uint32_t count;

	if (!metadata) return -1;

	count = (metadata->audiocodecid ? 5 : 0) + (metadata->videocodecid ? 5 : 0) + 1;
	if (flv->capacity < 1024)
	{
		if (0 != flv_muxer_alloc(flv, 1024))
			return ENOMEM;
	}

	ptr = flv->ptr;
	end = flv->ptr + flv->capacity;
	count = (metadata->audiocodecid ? 5 : 0) + (metadata->videocodecid ? 5 : 0) + 1;

	// ScriptTagBody

	// name
	ptr = AMFWriteString(ptr, end, "onMetaData", 10);

	// value: SCRIPTDATAECMAARRAY
	ptr[0] = AMF_ECMA_ARRAY;
	ptr[1] = (uint8_t)((count >> 24) & 0xFF);;
	ptr[2] = (uint8_t)((count >> 16) & 0xFF);;
	ptr[3] = (uint8_t)((count >> 8) & 0xFF);
	ptr[4] = (uint8_t)(count & 0xFF);
	ptr += 5;

	if (metadata->audiocodecid)
	{
		ptr = AMFWriteNamedDouble(ptr, end, "audiocodecid", 12, metadata->audiocodecid);
		ptr = AMFWriteNamedDouble(ptr, end, "audiodatarate", 13, metadata->audiodatarate /* / 1024.0*/);
		ptr = AMFWriteNamedDouble(ptr, end, "audiosamplerate", 15, metadata->audiosamplerate);
		ptr = AMFWriteNamedDouble(ptr, end, "audiosamplesize", 15, metadata->audiosamplesize);
		ptr = AMFWriteNamedBoolean(ptr, end, "stereo", 6, (uint8_t)metadata->stereo);
	}

	if (metadata->videocodecid)
	{
		ptr = AMFWriteNamedDouble(ptr, end, "duration", 8, metadata->duration);
		ptr = AMFWriteNamedDouble(ptr, end, "interval", 8, metadata->interval);
		ptr = AMFWriteNamedDouble(ptr, end, "videocodecid", 12, metadata->videocodecid);
		ptr = AMFWriteNamedDouble(ptr, end, "videodatarate", 13, metadata->videodatarate /* / 1024.0*/);
		ptr = AMFWriteNamedDouble(ptr, end, "framerate", 9, metadata->framerate);
		ptr = AMFWriteNamedDouble(ptr, end, "height", 6, metadata->height);
		ptr = AMFWriteNamedDouble(ptr, end, "width", 5, metadata->width);
	}

	ptr = AMFWriteNamedString(ptr, end, "encoder", 7, FLV_MUXER, strlen(FLV_MUXER));
	ptr = AMFWriteObjectEnd(ptr, end);

	return flv->handler(flv->param, FLV_TYPE_SCRIPT, flv->ptr, ptr - flv->ptr, 0);
}

#include "flv-header.h"
#include "flv-proto.h"
#include <assert.h>
#include <errno.h>

#define N_TAG_SIZE			4	// previous tag size
#define FLV_HEADER_SIZE		9	// DataOffset included
#define FLV_TAG_HEADER_SIZE	11	// StreamID included

static inline uint32_t be_read_uint32(const uint8_t* ptr)
{
	return (ptr[0] << 24) | (ptr[1] << 16) | (ptr[2] << 8) | ptr[3];
}

static inline void be_write_uint32(uint8_t* ptr, uint32_t val)
{
	ptr[0] = (uint8_t)((val >> 24) & 0xFF);
	ptr[1] = (uint8_t)((val >> 16) & 0xFF);
	ptr[2] = (uint8_t)((val >> 8) & 0xFF);
	ptr[3] = (uint8_t)(val & 0xFF);
}

int flv_header_read(struct flv_header_t* flv, const uint8_t* buf, size_t len)
{
	if (len < FLV_HEADER_SIZE || 'F' != buf[0] || 'L' != buf[1] || 'V' != buf[2])
	{
		assert(0);
		return -1;
	}

	flv->FLV[0] = buf[0];
	flv->FLV[1] = buf[1];
	flv->FLV[2] = buf[2];
	flv->version = buf[3];

	assert(0x00 == (buf[4] & 0xF8) && 0x00 == (buf[4] & 0x20));
	flv->audio = (buf[4] >> 2) & 0x01;
	flv->video = buf[4] & 0x01;
	flv->offset = be_read_uint32(buf + 5);

	return FLV_HEADER_SIZE;
}

int flv_tag_header_read(struct flv_tag_header_t* tag, const uint8_t* buf, size_t len)
{
	if (len < FLV_TAG_HEADER_SIZE)
	{
		assert(0);
		return -1;
	}

	// TagType
	tag->type = buf[0] & 0x1F;
	tag->filter = (buf[0] >> 5) & 0x01;
	assert(FLV_TYPE_VIDEO == tag->type || FLV_TYPE_AUDIO == tag->type || FLV_TYPE_SCRIPT == tag->type);

	// DataSize
	tag->size = (buf[1] << 16) | (buf[2] << 8) | buf[3];

	// TimestampExtended | Timestamp
	tag->timestamp = (buf[4] << 16) | (buf[5] << 8) | buf[6] | (buf[7] << 24);

	// StreamID Always 0
	tag->streamId = (buf[8] << 16) | (buf[9] << 8) | buf[10];
	//assert(0 == tag->streamId);

	return FLV_TAG_HEADER_SIZE;
}

int flv_audio_tag_header_read(struct flv_audio_tag_header_t* audio, const uint8_t* buf, size_t len)
{
	assert(len > 0);
	audio->codecid = (buf[0] & 0xF0) /*>> 4*/;
	audio->rate = (buf[0] & 0x0C) >> 2;
	audio->bits = (buf[0] & 0x02) >> 1;
	audio->channels = buf[0] & 0x01;

	if (FLV_AUDIO_AAC == audio->codecid || FLV_AUDIO_OPUS == audio->codecid)
	{
		if (len < 2)
		{
			assert(0);
			return -1;
		}
		audio->avpacket = buf[1];
		assert(FLV_SEQUENCE_HEADER == audio->avpacket || FLV_AVPACKET == audio->avpacket);
		return 2;
	}
	else
	{
		return 1;
	}
}

int flv_video_tag_header_read(struct flv_video_tag_header_t* video, const uint8_t* buf, size_t len)
{
	assert(len > 0);
	video->keyframe = (buf[0] & 0xF0) >> 4;
	video->codecid = (buf[0] & 0x0F);

	if (FLV_VIDEO_H264 == video->codecid || FLV_VIDEO_H265 == video->codecid || FLV_VIDEO_AV1 == video->codecid)
	{
		if (len < 5)
			return -1;

		video->avpacket = buf[1]; // AVCPacketType
		video->cts = (buf[2] << 16) | (buf[3] << 8) | buf[4];
		//if (video->cts >= (1 << 23)) video->cts -= (1 << 24);
		video->cts = (video->cts + 0xFF800000) ^ 0xFF800000; // signed 24-integer
		assert(FLV_SEQUENCE_HEADER == video->avpacket || FLV_AVPACKET == video->avpacket || FLV_END_OF_SEQUENCE == video->avpacket);
		return 5;
	}
	else
	{
		return 1;
	}
}

int flv_data_tag_header_read(const uint8_t* buf, size_t len)
{
	(void)buf;
	return (int)len;
}

int flv_header_write(int audio, int video, uint8_t* buf, size_t len)
{
	if (len < FLV_HEADER_SIZE)
	{
		assert(0);
		return -1;
	}

	buf[0] = 'F'; // FLV signature
	buf[1] = 'L';
	buf[2] = 'V';
	buf[3] = 0x01; // File version
	buf[4] = ((audio ? 1 : 0) << 2) | (video ? 1 : 0); // Type flags (audio & video)
	be_write_uint32(buf + 5, FLV_HEADER_SIZE); // Data offset
	return FLV_HEADER_SIZE;
}

int flv_tag_header_write(const struct flv_tag_header_t* tag, uint8_t* buf, size_t len)
{
	if (len < FLV_TAG_HEADER_SIZE)
	{
		assert(0);
		return -1;
	}

	// TagType
	assert(FLV_TYPE_VIDEO == tag->type || FLV_TYPE_AUDIO == tag->type || FLV_TYPE_SCRIPT == tag->type);
	buf[0] = (tag->type & 0x1F) | ((tag->filter & 0x01) << 5);

	// DataSize
	buf[1] = (tag->size >> 16) & 0xFF;
	buf[2] = (tag->size >> 8) & 0xFF;
	buf[3] = tag->size & 0xFF;

	// Timestamp
	buf[4] = (tag->timestamp >> 16) & 0xFF;
	buf[5] = (tag->timestamp >> 8) & 0xFF;
	buf[6] = (tag->timestamp >> 0) & 0xFF;
	buf[7] = (tag->timestamp >> 24) & 0xFF; // Timestamp Extended

	// StreamID(Always 0)
	buf[8] = (tag->streamId >> 16) & 0xFF;
	buf[9] = (tag->streamId >> 8) & 0xFF;
	buf[10] = (tag->streamId) & 0xFF;

	return FLV_TAG_HEADER_SIZE;
}

int flv_audio_tag_header_write(const struct flv_audio_tag_header_t* audio, uint8_t* buf, size_t len)
{
	if ((int)len < 1 + ((FLV_AUDIO_AAC == audio->codecid || FLV_AUDIO_OPUS == audio->codecid)? 1 : 0))
		return -1;

	if (FLV_AUDIO_AAC == audio->codecid || FLV_AUDIO_OPUS == audio->codecid)
	{
		assert(FLV_SEQUENCE_HEADER == audio->avpacket || FLV_AVPACKET == audio->avpacket);
		buf[0] = (audio->codecid /* <<4 */) /* SoundFormat */ | (3 << 2) /* 44k-SoundRate */ | (1 << 1) /* 16-bit samples */ | 1 /* Stereo sound */;
		buf[1] = audio->avpacket; // AACPacketType
		return 2;
	}
	else
	{
		buf[0] = (audio->codecid /* <<4 */) | ((audio->rate & 0x03) << 2) | ((audio->bits & 0x01) << 1) | (audio->channels & 0x01);
		return 1;
	}
}

int flv_video_tag_header_write(const struct flv_video_tag_header_t* video, uint8_t* buf, size_t len)
{
	if (len < 1)
		return -1;

	buf[0] = (video->keyframe << 4) /*FrameType*/ | (video->codecid & 0x0F) /*CodecID*/;

	if (FLV_VIDEO_H264 == video->codecid || FLV_VIDEO_H265 == video->codecid || FLV_VIDEO_AV1 == video->codecid)
	{
		assert(FLV_SEQUENCE_HEADER == video->avpacket || FLV_AVPACKET == video->avpacket || FLV_END_OF_SEQUENCE == video->avpacket);
		if (len < 5)
			return -1;

		buf[1] = video->avpacket; // AVCPacketType
		buf[2] = (video->cts >> 16) & 0xFF;
		buf[3] = (video->cts >> 8) & 0xFF;
		buf[4] = video->cts & 0xFF;
		return 5;
	}

	return 1;
}

int flv_data_tag_header_write(uint8_t* buf, size_t len)
{
    (void)buf;
    (void)len;
	return 0;
}

int flv_tag_size_read(const uint8_t* buf, size_t len, uint32_t* size)
{
    if(len < 4)
        return -1;
    *size = be_read_uint32(buf);
    return 4;
}

int flv_tag_size_write(uint8_t* buf, size_t len, uint32_t size)
{
    if(len < 4)
        return -1;
    be_write_uint32(buf, size);
    return 4;
}

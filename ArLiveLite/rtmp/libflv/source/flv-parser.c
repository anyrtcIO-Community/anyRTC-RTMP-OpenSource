#include "flv-parser.h"
#include "flv-header.h"
#include "flv-proto.h"
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <errno.h>

#define N_TAG_SIZE			4	// previous tag size
#define FLV_HEADER_SIZE		9	// DataOffset included
#define FLV_TAG_HEADER_SIZE	11	// StreamID included

#define FLV_VIDEO_CODEC_NAME(codecid) (FLV_VIDEO_H264==(codecid) ? FLV_VIDEO_AVCC : (FLV_VIDEO_H265==(codecid) ? FLV_VIDEO_HVCC : FLV_VIDEO_AV1C))

static int flv_parser_audio(struct flv_audio_tag_header_t* audio, const uint8_t* data, size_t bytes, uint32_t timestamp, flv_parser_handler handler, void* param)
{
	if (FLV_SEQUENCE_HEADER == audio->avpacket)
		return handler(param, FLV_AUDIO_AAC == audio->codecid ? FLV_AUDIO_ASC : FLV_AUDIO_OPUS_HEAD, data, bytes, timestamp, timestamp, 0);
	else
		return handler(param, audio->codecid, data, bytes, timestamp, timestamp, 0);
}

static int flv_parser_video(struct flv_video_tag_header_t* video, const uint8_t* data, size_t bytes, uint32_t timestamp, flv_parser_handler handler, void* param)
{
	if (FLV_VIDEO_H264 == video->codecid || FLV_VIDEO_H265 == video->codecid || FLV_VIDEO_AV1 == video->codecid)
	{
		if (FLV_SEQUENCE_HEADER == video->avpacket)
		{
			return handler(param, FLV_VIDEO_CODEC_NAME(video->codecid), data, bytes, timestamp, timestamp, 0);
		}
		else if (FLV_AVPACKET == video->avpacket)
		{
			return handler(param, video->codecid, data, bytes, timestamp + video->cts, timestamp, (FLV_VIDEO_KEY_FRAME == video->keyframe) ? 1 : 0);
		}
		else if (FLV_END_OF_SEQUENCE == video->avpacket)
		{
			return 0; // AVC end of sequence (lower level NALU sequence ender is not required or supported)
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
		return handler(param, video->codecid, data, bytes, timestamp, timestamp, (FLV_VIDEO_KEY_FRAME == video->keyframe) ? 1 : 0);
	}
}

// http://www.cnblogs.com/musicfans/archive/2012/11/07/2819291.html
// metadata keyframes/filepositions
static int flv_parser_script(const uint8_t* data, size_t bytes, uint32_t timestamp, flv_parser_handler handler, void* param)
{
	return handler(param, FLV_SCRIPT_METADATA, data, bytes, timestamp, timestamp, 0);
}

int flv_parser_tag(int type, const void* data, size_t bytes, uint32_t timestamp, flv_parser_handler handler, void* param)
{
	int n;
	struct flv_audio_tag_header_t audio;
	struct flv_video_tag_header_t video;

	if (bytes < 1) return -EINVAL;

	switch (type)
	{
	case FLV_TYPE_AUDIO:
		n = flv_audio_tag_header_read(&audio, data, bytes);
		if (n < 0)
			return n;
		return flv_parser_audio(&audio, (const uint8_t*)data + n, (int)bytes - n, timestamp, handler, param);

	case FLV_TYPE_VIDEO:
		n = flv_video_tag_header_read(&video, data, bytes);
		if (n < 0)
			return n;
		return flv_parser_video(&video, (const uint8_t*)data + n, (int)bytes - n, timestamp, handler, param);

	case FLV_TYPE_SCRIPT:
		n = flv_data_tag_header_read(data, bytes);
		if (n < 0)
			return n;
		return flv_parser_script((const uint8_t*)data + n, (int)bytes - n, timestamp, handler, param);

	default:
		assert(0);
		return -1;
	}
}

static size_t flv_parser_append(struct flv_parser_t* parser, const uint8_t* data, size_t bytes, size_t expect)
{
	size_t n;
	assert(parser->bytes <= expect && expect <= sizeof(parser->ptr));
	n = parser->bytes + bytes >= expect ? expect - parser->bytes : bytes;
	if (n > 0)
	{
		memcpy(parser->ptr + parser->bytes, data, n);
		parser->bytes += n;
	}
	return n;
}

int flv_parser_input(struct flv_parser_t* parser, const uint8_t* data, size_t bytes, flv_parser_handler handler, void* param)
{
    int r;
	size_t n;
	uint8_t codec;
	uint32_t size;
	enum {FLV_HEADER=0, FLV_HEADER_OFFSET, FLV_PREVIOUS_SIZE, FLV_TAG_HEADER, FLV_AVHEADER_CODEC, FLV_AVHEADER_EXTRA, FLV_TAG_BODY};

	for (n = r = 0; bytes > 0 && n >= 0 && 0 == r; data += n, bytes -= n)
	{
		switch (parser->state)
		{
		case FLV_HEADER:
			n = flv_parser_append(parser, data, bytes, FLV_HEADER_SIZE);
			if (FLV_HEADER_SIZE == parser->bytes)
			{
				flv_header_read(&parser->header, parser->ptr, parser->bytes);
				if (parser->header.offset < 9 || parser->header.offset > sizeof(parser->ptr))
					return -1;
				parser->header.offset -= 9;
				parser->state = parser->header.offset > 0 ? FLV_HEADER_OFFSET : FLV_PREVIOUS_SIZE;
				parser->bytes = 0;
			}
			break;

		case FLV_HEADER_OFFSET:
			n = flv_parser_append(parser, data, bytes, parser->header.offset);
			if (parser->header.offset == (uint32_t)parser->bytes)
			{
				parser->bytes = 0;
				parser->state = FLV_PREVIOUS_SIZE;
			}
			break;

		case FLV_PREVIOUS_SIZE:
			n = flv_parser_append(parser, data, bytes, N_TAG_SIZE);
			if (N_TAG_SIZE == parser->bytes)
			{
				flv_tag_size_read(parser->ptr, parser->bytes, &size);
				assert(size == 0 || size == parser->tag.size + FLV_TAG_HEADER_SIZE);
				parser->bytes = 0;
				parser->state = FLV_TAG_HEADER;
			}
			break;

		case FLV_TAG_HEADER:
			n = flv_parser_append(parser, data, bytes, FLV_TAG_HEADER_SIZE);
			if (FLV_TAG_HEADER_SIZE == parser->bytes)
			{
				flv_tag_header_read(&parser->tag, parser->ptr, parser->bytes);
				parser->bytes = 0;
				parser->state = FLV_AVHEADER_CODEC;
			}
			break;
			
		case FLV_AVHEADER_CODEC:
			switch (parser->tag.type)
			{
			case FLV_TYPE_AUDIO:
				parser->expect = 1;
				n = flv_parser_append(parser, data, bytes, 1);
				codec = (parser->ptr[0] & 0xF0) /*>> 4*/;
				if (FLV_AUDIO_AAC == codec || FLV_AUDIO_OPUS == codec)
					parser->expect = 2;
				break;

			case FLV_TYPE_VIDEO:
				parser->expect = 1;
				n = flv_parser_append(parser, data, bytes, 1);
				codec = (parser->ptr[0] & 0x0F);
				if (FLV_VIDEO_H264 == codec || FLV_VIDEO_H265 == codec || FLV_VIDEO_AV1 == codec)
					parser->expect = 5;
				break;

			case FLV_TYPE_SCRIPT:
				parser->expect = 0;
				n = 0; // noops
			}
			parser->state = FLV_AVHEADER_EXTRA;
			break;

		case FLV_AVHEADER_EXTRA:
			n = flv_parser_append(parser, data, bytes, parser->expect);
			if (parser->expect == parser->bytes)
			{
				if(FLV_TYPE_AUDIO == parser->tag.type)
					flv_audio_tag_header_read(&parser->audio, parser->ptr, parser->bytes);
				else if(FLV_TYPE_VIDEO == parser->tag.type)
					flv_video_tag_header_read(&parser->video, parser->ptr, parser->bytes);
				parser->bytes = 0;
				parser->state = FLV_TAG_BODY;

				parser->expect = parser->tag.size - parser->expect;
				parser->body = parser->alloc ? parser->alloc(param, parser->expect) : malloc(parser->expect);
				if (!parser->body)
					return -1;
			}
			break;

		case FLV_TAG_BODY:
			assert(parser->body && parser->bytes <= parser->expect);
			n = parser->bytes + bytes >= parser->expect ? parser->expect - parser->bytes : bytes;
			if(n > 0) {
				memmove(parser->body + parser->bytes, data, n);
				parser->bytes += n;
			}

			if (parser->expect == parser->bytes)
			{
				parser->bytes = 0;
				parser->state = FLV_PREVIOUS_SIZE;
				switch (parser->tag.type)
				{
				case FLV_TYPE_AUDIO:
					r = flv_parser_audio(&parser->audio, parser->body, parser->expect, parser->tag.timestamp, handler, param);
					break;

				case FLV_TYPE_VIDEO:
					r = flv_parser_video(&parser->video, parser->body, parser->expect, parser->tag.timestamp, handler, param);
					break;

				case FLV_TYPE_SCRIPT:
					r = flv_parser_script(parser->body, parser->expect, parser->tag.timestamp, handler, param);
					break;

				default:
					assert(0);
					r = -1;
					break;
				}
				
				parser->free ? parser->free(param, parser->body) : free(parser->body);
			}
			break;

		default:
			assert(0);
			return -1;
		}
	}

	return r;
}

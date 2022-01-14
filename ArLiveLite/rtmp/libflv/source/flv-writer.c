#include "flv-writer.h"
#include "flv-header.h"
#include "flv-proto.h"
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <stdio.h>

#define FLV_HEADER_SIZE		9 // DataOffset included
#define FLV_TAG_HEADER_SIZE	11 // StreamID included

struct flv_writer_t
{
	FILE* fp;
	flv_writer_onwrite write;
	void* param;
};

static int flv_write_header(int audio, int video, struct flv_writer_t* flv)
{
	struct flv_vec_t vec[1];
	uint8_t header[FLV_HEADER_SIZE + 4];
	flv_header_write(audio, video, header, FLV_HEADER_SIZE);
    flv_tag_size_write(header + FLV_HEADER_SIZE, 4, 0); // PreviousTagSize0(Always 0)
	vec[0].ptr = header;
	vec[0].len = sizeof(header);
	return flv->write(flv->param, vec, 1);
}

static int flv_write_eos(struct flv_writer_t* flv)
{
	int n;
	uint8_t header[16];
	struct flv_video_tag_header_t video;
	memset(&video, 0, sizeof(video));
	video.codecid = FLV_VIDEO_H264;
	video.keyframe = 1;
	video.avpacket = FLV_END_OF_SEQUENCE;
	video.cts = 0;

	n = flv_video_tag_header_write(&video, header, sizeof(header));
	return n > 0 ? flv_writer_input(flv, FLV_TYPE_VIDEO, header, n, 0) : -1;
}

static int file_write(void* param, const struct flv_vec_t* vec, int n)
{
	int i;
	for(i = 0; i < n; i++)
	{
		if (vec[i].len != (int)fwrite(vec[i].ptr, 1, vec[i].len, (FILE*)param))
			return ferror((FILE*)param);
	}
	return 0;
}

void* flv_writer_create(const char* file)
{
	FILE* fp;
	struct flv_writer_t* flv;
	fp = fopen(file, "wb");
	if (!fp)
		return NULL;

	flv = flv_writer_create2(1, 1, file_write, fp);
	if (!flv)
	{
		fclose(fp);
		return NULL;
	}

	flv->fp = fp;
	return flv;
}

void* flv_writer_create2(int audio, int video, flv_writer_onwrite write, void* param)
{
	struct flv_writer_t* flv;
	flv = (struct flv_writer_t*)calloc(1, sizeof(*flv));
	if (!flv)
		return NULL;

	flv->write = write;
	flv->param = param;
	if (0 != flv_write_header(audio, video, flv))
	{
		flv_writer_destroy(flv);
		return NULL;
	}

	return flv;
}

void flv_writer_destroy(void* p)
{
	struct flv_writer_t* flv;
	flv = (struct flv_writer_t*)p;

	if (NULL != flv)
	{
		flv_write_eos(flv);
		if (flv->fp)
			fclose(flv->fp);
		free(flv);
	}
}

int flv_writer_input(void* p, int type, const void* data, size_t bytes, uint32_t timestamp)
{
	uint8_t buf[FLV_TAG_HEADER_SIZE + 4];
	struct flv_vec_t vec[3];
	struct flv_writer_t* flv;
	struct flv_tag_header_t tag;
	flv = (struct flv_writer_t*)p;

	memset(&tag, 0, sizeof(tag));
	tag.size = (int)bytes;
	tag.type = (uint8_t)type;
	tag.timestamp = timestamp;
	flv_tag_header_write(&tag, buf, FLV_TAG_HEADER_SIZE);
	flv_tag_size_write(buf + FLV_TAG_HEADER_SIZE, 4, (uint32_t)bytes + FLV_TAG_HEADER_SIZE);

	vec[0].ptr = buf;  // FLV Tag Header
	vec[0].len = FLV_TAG_HEADER_SIZE;
	vec[1].ptr = (void*)data;
	vec[1].len = (int)bytes;
	vec[2].ptr = buf + FLV_TAG_HEADER_SIZE; // TAG size
	vec[2].len = 4;
	return flv->write(flv->param, vec, 3);
}

int flv_writer_input_v(void* p, int type, const struct flv_vec_t* v, int n, uint32_t timestamp)
{
	int i;
	uint8_t buf[FLV_TAG_HEADER_SIZE + 4];
	struct flv_vec_t vec[8];
	struct flv_writer_t* flv;
	struct flv_tag_header_t tag;
	flv = (struct flv_writer_t*)p;

	memset(&tag, 0, sizeof(tag));
	tag.size = 0;
	tag.type = (uint8_t)type;
	tag.timestamp = timestamp;

	assert(n + 2 <= sizeof(vec) / sizeof(vec[0]));
	for (i = 0; i < n && i + 2 < sizeof(vec)/sizeof(vec[0]); i++)
	{
		tag.size += v[i].len;
		vec[i+1].ptr = v[i].ptr;
		vec[i+1].len = v[i].len;
	}

	vec[0].ptr = buf;  // FLV Tag Header
	vec[0].len = FLV_TAG_HEADER_SIZE;
	vec[n + 1].ptr = buf + FLV_TAG_HEADER_SIZE; // TAG size
	vec[n + 1].len = 4;

	flv_tag_header_write(&tag, buf, FLV_TAG_HEADER_SIZE);
	flv_tag_size_write(buf + FLV_TAG_HEADER_SIZE, 4, (uint32_t)tag.size + FLV_TAG_HEADER_SIZE);

	return flv->write(flv->param, vec, n+2);
}

#include "flv-reader.h"
#include "flv-header.h"
#include "flv-proto.h"
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <stdio.h>

#define FLV_HEADER_SIZE		9 // DataOffset included
#define FLV_TAG_HEADER_SIZE	11 // StreamID included

struct flv_reader_t
{
	FILE* fp;
	int (*read)(void* param, void* buf, int len);
	void* param;
};

static int flv_read_header(struct flv_reader_t* flv)
{
    uint32_t sz;
	uint8_t data[FLV_HEADER_SIZE];
	struct flv_header_t h;
	int n;

	if (FLV_HEADER_SIZE != flv->read(flv->param, data, FLV_HEADER_SIZE))
		return -1;

	if(FLV_HEADER_SIZE != flv_header_read(&h, data, FLV_HEADER_SIZE))
		return -1;

	assert(h.offset >= FLV_HEADER_SIZE && h.offset < FLV_HEADER_SIZE + 4096);
	for(n = (int)(h.offset - FLV_HEADER_SIZE); n > 0 && n < 4096; n -= sizeof(data))
		flv->read(flv->param, data, n >= sizeof(data) ? sizeof(data) : n); // skip

	// PreviousTagSize0
	if (4 != flv->read(flv->param, data, 4))
		return -1;

	flv_tag_size_read(data, 4, &sz);
    assert(0 == sz);
	return 0 == sz ? 0 : -1;
}

static int file_read(void* param, void* buf, int len)
{
	return (int)fread(buf, 1, len, (FILE*)param);
}

void* flv_reader_create(const char* file)
{
	FILE* fp;
	struct flv_reader_t* flv;
	fp = fopen(file, "rb");
	if (!fp)
		return NULL;

	flv = flv_reader_create2(file_read, fp);
	if (!flv)
	{
		fclose(fp);
		return NULL;
	}

	flv->fp = fp;
	return flv;
}

void* flv_reader_create2(int (*read)(void* param, void* buf, int len), void* param)
{
	struct flv_reader_t* flv;
	flv = (struct flv_reader_t*)calloc(1, sizeof(*flv));
	if (!flv)
		return NULL;
	
	flv->read = read;
	flv->param = param;
	if (0 != flv_read_header(flv))
	{
		flv_reader_destroy(flv);
		return NULL;
	}

	return flv;
}

void flv_reader_destroy(void* p)
{
	struct flv_reader_t* flv;
	flv = (struct flv_reader_t*)p;
	if (NULL != flv)
	{
		if (flv->fp)
			fclose(flv->fp);
		free(flv);
	}
}

int flv_reader_read(void* p, int* tagtype, uint32_t* timestamp, size_t* taglen, void* buffer, size_t bytes)
{
	int r;
    uint32_t sz;
	uint8_t header[FLV_TAG_HEADER_SIZE];
	struct flv_tag_header_t tag;
	struct flv_reader_t* flv;
	flv = (struct flv_reader_t*)p;

	r = flv->read(flv->param, &header, FLV_TAG_HEADER_SIZE);
	if (r != FLV_TAG_HEADER_SIZE)
		return r < 0 ? r : 0; // 0-EOF

	if (FLV_TAG_HEADER_SIZE != flv_tag_header_read(&tag, header, FLV_TAG_HEADER_SIZE))
		return -1;

	if (bytes < tag.size)
		return -1;

	// FLV stream
	r = flv->read(flv->param, buffer, tag.size);
	if(tag.size != (uint32_t)r)
		return r < 0 ? r : 0; // 0-EOF

	// PreviousTagSizeN
	r = flv->read(flv->param, header, 4);
	if (4 != r)
		return r < 0 ? r : 0; // 0-EOF

	*taglen = tag.size;
	*tagtype = tag.type;
	*timestamp = tag.timestamp;
	flv_tag_size_read(header, 4, &sz);
    assert(0 == tag.streamId); // StreamID Always 0
    assert(sz == tag.size + FLV_TAG_HEADER_SIZE);
	return (sz == tag.size + FLV_TAG_HEADER_SIZE) ? 1 : -1;
}

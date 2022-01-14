#include "rtmp-chunk-header.h"
#include "rtmp-util.h"

// 5.3.1.1. Chunk Basic Header (p12)
/*
 0 1 2 3 4 5 6 7
+-+-+-+-+-+-+-+-+
|fmt|   cs id   |
+-+-+-+-+-+-+-+-+

 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5
+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
|fmt|     0     |   cs id - 64  |
+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+

 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1 2 3
+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
|fmt|     1     |          cs id - 64           |
+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
*/
int rtmp_chunk_basic_header_read(const uint8_t* data, uint8_t* fmt, uint32_t* cid)
{
	*fmt = data[0] >> 6;
	*cid = data[0] & 0x3F;

	if (0 == *cid)
	{
		*cid = 64 + (uint32_t)data[1];
		return 2;
	}
	else if (1 == *cid)
	{
		*cid = 64 + (uint32_t)data[1] + ((uint32_t)data[2] << 8) /* 256 */;
		return 3;
	}
	else
	{
		return 1;
	}
}

int rtmp_chunk_basic_header_write(uint8_t* out, uint8_t fmt, uint32_t id)
{
	if (id >= 64 + 256)
	{
		*out++ = (fmt << 6) | 1;
		*out++ = (uint8_t)((id - 64) & 0xFF);
		*out++ = (uint8_t)(((id - 64) >> 8) & 0xFF);
		return 3;
	}
	else if (id >= 64)
	{
		*out++ = (fmt << 6) | 0;
		*out++ = (uint8_t)(id - 64);
		return 2;
	}
	else
	{
		*out++ = (fmt << 6) | (uint8_t)id;
		return 1;
	}
}

// 5.3.1.2. Chunk Message Header (p13)
/*
 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1
+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
|                   timestamp                   |message length |
+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
| message length (cont)         |message type id| msg stream id |
+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
|           message stream id (cont)            |
+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
*/
int rtmp_chunk_message_header_read(const uint8_t* data, struct rtmp_chunk_header_t* header)
{
	int offset = 0;

	// timestamp / delta
	if (header->fmt <= RTMP_CHUNK_TYPE_2)
	{
		be_read_uint24(data + offset, &header->timestamp);
		offset += 3;
	}

	// message length + type
	if (header->fmt <= RTMP_CHUNK_TYPE_1)
	{
		be_read_uint24(data + offset, &header->length);
		header->type = data[offset + 3];
		offset += 4;
	}

	// message stream id
	if (header->fmt == RTMP_CHUNK_TYPE_0)
	{
		le_read_uint32(data + offset, &header->stream_id);
		offset += 4;
	}

	return offset;
}

int rtmp_chunk_message_header_write(uint8_t* out, const struct rtmp_chunk_header_t* header)
{
	const static int s_header_size[] = { 11, 7, 3, 0 };

	// timestamp / delta
	if (header->fmt <= RTMP_CHUNK_TYPE_2)
	{
		be_write_uint24(out, header->timestamp >= 0xFFFFFF ? 0xFFFFFF : header->timestamp);
		out += 3;
	}

	// message length + type
	if (header->fmt <= RTMP_CHUNK_TYPE_1)
	{
		be_write_uint24(out, header->length);
		out[3] = header->type;
		out += 4;
	}

	// message stream id
	if (header->fmt == RTMP_CHUNK_TYPE_0)
	{
		le_write_uint32(out, header->stream_id);
		out += 4;
	}

	return s_header_size[header->fmt % 4];
}

// 5.3.1.3. Extended Timestamp (p16)
int rtmp_chunk_extended_timestamp_read(const uint8_t* out, uint32_t* timestamp)
{
	// extended timestamp
	be_read_uint32(out, timestamp);
	return 4;
}

int rtmp_chunk_extended_timestamp_write(uint8_t* out, uint32_t timestamp)
{
	// extended timestamp
	be_write_uint32(out, timestamp);
	return 4;
}

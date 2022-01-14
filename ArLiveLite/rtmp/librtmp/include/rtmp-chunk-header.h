#ifndef _rtmp_chunk_header_h_
#define _rtmp_chunk_header_h_

#include <stdint.h>
#include <stddef.h>

enum rtmp_chunk_type_t
{
	RTMP_CHUNK_TYPE_0 = 0, // 11-bytes: timestamp(3) + length(3) + stream type(1) + stream id(4)
	RTMP_CHUNK_TYPE_1 = 1, // 7-bytes: delta(3) + length(3) + stream type(1)
	RTMP_CHUNK_TYPE_2 = 2, // 3-bytes: delta(3)
	RTMP_CHUNK_TYPE_3 = 3, // 0-byte
};

struct rtmp_chunk_header_t
{
	uint8_t fmt; // RTMP_CHUNK_TYPE_XXX
	uint32_t cid; // chunk stream id(22-bits)

	uint32_t timestamp; // delta(24-bits) / extended timestamp(32-bits)

	uint32_t length; // message length (24-bits)
	uint8_t type; // message type id

	uint32_t stream_id; // message stream id
};

/// @return read bytes
int rtmp_chunk_basic_header_read(const uint8_t* data, uint8_t* fmt, uint32_t* cid);
int rtmp_chunk_message_header_read(const uint8_t* data, struct rtmp_chunk_header_t* header);
int rtmp_chunk_extended_timestamp_read(const uint8_t* out, uint32_t* timestamp);

/// @return write bytes
int rtmp_chunk_basic_header_write(uint8_t* out, uint8_t fmt, uint32_t id);
int rtmp_chunk_message_header_write(uint8_t* out, const struct rtmp_chunk_header_t* header);
int rtmp_chunk_extended_timestamp_write(uint8_t* out, uint32_t timestamp);

#endif /* !_rtmp_chunk_header_h_ */

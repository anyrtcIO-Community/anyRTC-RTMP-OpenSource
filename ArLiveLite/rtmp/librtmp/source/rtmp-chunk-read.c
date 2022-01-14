#include "rtmp-internal.h"
#include "rtmp-msgtypeid.h"
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <errno.h>

#define MIN(x, y) ((x) < (y) ? (x) : (y))

static struct rtmp_packet_t* rtmp_packet_find(struct rtmp_t* rtmp, uint32_t cid)
{
	uint32_t i;
	struct rtmp_packet_t* pkt;

	// The protocol supports up to 65597 streams with IDs 3-65599
	assert(cid <= 65535 + 64 && cid >= 2 /* Protocol Control Messages */);
	for (i = 0; i < N_CHUNK_STREAM; i++)
	{
		pkt = rtmp->in_packets + ((i + cid) % N_CHUNK_STREAM);
		if (pkt->header.cid == cid)
			return pkt;
	}
	return NULL;
}

static struct rtmp_packet_t* rtmp_packet_create(struct rtmp_t* rtmp, uint32_t cid)
{
	uint32_t i;
	struct rtmp_packet_t* pkt;

	// The protocol supports up to 65597 streams with IDs 3-65599
	assert(cid <= 65535 + 64 && cid >= 2 /* Protocol Control Messages */);
	assert(NULL == rtmp_packet_find(rtmp, cid));
	for (i = 0; i < N_CHUNK_STREAM; i++)
	{
		pkt = rtmp->in_packets + ((i + cid) % N_CHUNK_STREAM);
		if (0 == pkt->header.cid)
			return pkt;
	}
	return NULL;
}

static struct rtmp_packet_t* rtmp_packet_parse(struct rtmp_t* rtmp, const uint8_t* buffer)
{
	uint8_t fmt = 0;
	uint32_t cid = 0;
	struct rtmp_packet_t* packet;
	
	// chunk base header
	buffer += rtmp_chunk_basic_header_read(buffer, &fmt, &cid);

	// load previous header
	packet = rtmp_packet_find(rtmp, cid);
	if (NULL == packet)
	{
		if (RTMP_CHUNK_TYPE_0 != fmt && RTMP_CHUNK_TYPE_1 != fmt)
			return NULL; // don't know stream length

		packet = rtmp_packet_create(rtmp, cid);
		if (NULL == packet)
			return NULL;
	}

	// chunk message header
	packet->header.cid = cid;
	packet->header.fmt = fmt;
	rtmp_chunk_message_header_read(buffer, &packet->header);

	return packet;
}

static int rtmp_packet_alloc(struct rtmp_t* rtmp, struct rtmp_packet_t* packet)
{
	void* p;
	(void)rtmp;

	// 24-bytes length
	assert(0 == packet->bytes);
	assert(packet->header.length < (1 << 24));
	// fixed SMS (Chinacache Smart Media Server) packet->header.length = 0
	if (0 == packet->capacity || packet->capacity < packet->header.length)
	{
		p = realloc(packet->payload, packet->header.length + 1024);
		if (NULL == p)
			return ENOMEM;
		packet->payload = p;
		packet->capacity = packet->header.length + 1024;
	}

	return 0;
}

int rtmp_chunk_read(struct rtmp_t* rtmp, const uint8_t* data, size_t bytes)
{
	const static uint32_t s_header_size[] = { 11, 7, 3, 0 };

	int r, invalid_extended_timestamp;
	size_t size, offset = 0;
	uint8_t extended_timestamp_buffer[4];
	uint32_t extended_timestamp = 0;
	struct rtmp_parser_t* parser = &rtmp->parser;
	struct rtmp_chunk_header_t header;

	while (offset < bytes)
	{
		switch (parser->state)
		{
		case RTMP_PARSE_INIT:
			parser->pkt = NULL;
			parser->bytes = 1;
			parser->buffer[0] = data[offset++];

			if (0 == (parser->buffer[0] & 0x3F))
				parser->basic_bytes = 2;
			else if (1 == (parser->buffer[0] & 0x3F))
				parser->basic_bytes = 3;
			else
				parser->basic_bytes = 1;

			parser->state = RTMP_PARSE_BASIC_HEADER;
			break;

		case RTMP_PARSE_BASIC_HEADER:
			assert(parser->bytes <= parser->basic_bytes);
			while (parser->bytes < parser->basic_bytes && offset < bytes)
			{
				parser->buffer[parser->bytes++] = data[offset++];
			}

			assert(parser->bytes <= parser->basic_bytes);
			if (parser->bytes >= parser->basic_bytes)
			{
				parser->state = RTMP_PARSE_MESSAGE_HEADER;
			}
			break;

		case RTMP_PARSE_MESSAGE_HEADER:
			size = s_header_size[parser->buffer[0] >> 6] + parser->basic_bytes;
			assert(parser->bytes <= size);
			while (parser->bytes < size && offset < bytes)
			{
				parser->buffer[parser->bytes++] = data[offset++];
			}

			assert(parser->bytes <= size);
			if (parser->bytes >= size)
			{
				parser->pkt = rtmp_packet_parse(rtmp, parser->buffer);
				parser->state = RTMP_PARSE_EXTENDED_TIMESTAMP;
			}
			break;

		case RTMP_PARSE_EXTENDED_TIMESTAMP:
			if (NULL == parser->pkt) return ENOMEM;

			assert(parser->pkt->header.timestamp <= 0xFFFFFF);
			size = s_header_size[parser->pkt->header.fmt] + parser->basic_bytes;
			if (parser->pkt->header.timestamp == 0xFFFFFF) size += 4; // extended timestamp

			assert(parser->bytes <= size);
			while (parser->bytes < size && offset < bytes)
			{
				parser->buffer[parser->bytes++] = data[offset++];
			}

			assert(parser->bytes <= size);
			if (parser->bytes >= size)
			{
				invalid_extended_timestamp = 0;
				extended_timestamp = parser->pkt->header.timestamp;
				if (parser->pkt->header.timestamp == 0xFFFFFF)
				{
					// parse extended timestamp
					rtmp_chunk_extended_timestamp_read(parser->buffer + s_header_size[parser->buffer[0] >> 6] + parser->basic_bytes, &extended_timestamp);
					if (RTMP_CHUNK_TYPE_3 == parser->pkt->header.fmt && extended_timestamp != parser->pkt->delta) 
					{
						// fix code offset -= 4 on offset < 4;
						invalid_extended_timestamp = 1;
						memcpy(extended_timestamp_buffer, parser->buffer + s_header_size[parser->buffer[0] >> 6] + parser->basic_bytes, 4);
					}
				}

				// first chunk
				if (0 == parser->pkt->bytes)
				{
					parser->pkt->delta = extended_timestamp;

					// handle timestamp/delta
					if (RTMP_CHUNK_TYPE_0 == parser->pkt->header.fmt)
						parser->pkt->clock = parser->pkt->delta;
					else
						parser->pkt->clock += parser->pkt->delta;

					if (0 != rtmp_packet_alloc(rtmp, parser->pkt))
						return ENOMEM;
				}
				parser->state = RTMP_PARSE_PAYLOAD;

				// rewind extended_timestamp_buffer
				if (invalid_extended_timestamp)
				{
					r = rtmp_chunk_read(rtmp, extended_timestamp_buffer, 4);
					if (0 != r) return r;
				}	
			}
			break;

		case RTMP_PARSE_PAYLOAD:
			if (NULL == parser->pkt || NULL == parser->pkt->payload 
				|| parser->pkt->bytes > parser->pkt->capacity 
				|| parser->pkt->bytes > parser->pkt->header.length 
				|| parser->pkt->header.length > parser->pkt->capacity)
			{
				assert(0);
				return ENOMEM;
			}
			//assert(parser->pkt->bytes <= parser->pkt->header.length);
			//assert(parser->pkt->capacity >= parser->pkt->header.length);
			size = MIN(rtmp->in_chunk_size - (parser->pkt->bytes % rtmp->in_chunk_size), parser->pkt->header.length - parser->pkt->bytes);
			size = MIN(size, bytes - offset);
			if(size > 0) memcpy(parser->pkt->payload + parser->pkt->bytes, data + offset, size);
			parser->pkt->bytes += size;
			offset += size;

			if (parser->pkt->bytes >= parser->pkt->header.length)
			{
				assert(parser->pkt->bytes == parser->pkt->header.length);
				parser->state = RTMP_PARSE_INIT; // reset parser state
				parser->pkt->bytes = 0; // clear bytes

				memcpy(&header, &parser->pkt->header, sizeof(header));
				header.timestamp = parser->pkt->clock;
				r = rtmp_handler(rtmp, &header, parser->pkt->payload);
				if(0 != r) return r;
			}
			else if (0 == (parser->pkt->bytes % rtmp->in_chunk_size))
			{
				// next chunk
				parser->state = RTMP_PARSE_INIT;
			}
			else
			{
				// need more data
				assert(offset == bytes);
			}
			break;

		default:
			assert(0);
			break;
		}
	}

	return 0;
}

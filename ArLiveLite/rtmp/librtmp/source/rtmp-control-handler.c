#include "rtmp-internal.h"
#include "rtmp-msgtypeid.h"
#include "rtmp-util.h"
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

#define RTMP_CHUNK_SIZE_MINIMUM 64
#define RTMP_CHUNK_SIZE_MAXIMUM 0x800000

/// 5.4.1. Set Chunk Size (1)
/// @return 0-error, >0-ok
static int rtmp_read_chunk_size(const uint8_t* out, size_t size, uint32_t *chunkSize)
{
	if (size >= 4)
	{
		be_read_uint32(out, chunkSize);
        if (*chunkSize < RTMP_CHUNK_SIZE_MINIMUM || *chunkSize > RTMP_CHUNK_SIZE_MAXIMUM)
            return 0;
		return 4;
	}
	return 0;
}

/// 5.4.2. Abort Message (2)
/// @return 0-error, >0-ok
static int rtmp_read_abort_message(const uint8_t* out, size_t size, uint32_t* chunkStreamId)
{
	if (size >= 4)
	{
		be_read_uint32(out, chunkStreamId);
		return 4;
	}
	return 0;
}

/// 5.4.3. Acknowledgement (3)
/// @return 0-error, >0-ok
static int rtmp_read_acknowledgement(const uint8_t* out, size_t size, uint32_t* sequenceNumber)
{
	if (size >= 4)
	{
		be_read_uint32(out, sequenceNumber);
		return 4;
	}
	return 0;
}

/// 5.4.4. Window Acknowledgement Size (5)
/// @return 0-error, >0-ok
static int rtmp_read_window_acknowledgement_size(const uint8_t* out, size_t size, uint32_t* windowSize)
{
	if (size >= 4)
	{
		be_read_uint32(out, windowSize);
		return 4;
	}
	return 0;
}

/// 5.4.5. Set Peer Bandwidth (6)
/// @return 0-error, >0-ok
static int rtmp_read_set_peer_bandwidth(const uint8_t* out, size_t size, uint32_t *windowSize, uint8_t *limitType)
{
	if (size >= 5)
	{
		be_read_uint32(out, windowSize);
		*limitType = out[4];
		return 5;
	}
	return 0;
}

int rtmp_control_handler(struct rtmp_t* rtmp, const struct rtmp_chunk_header_t* header, const uint8_t* data)
{
	uint32_t chunk_stream_id = 0;
	assert(2 == header->cid);

	switch (header->type)
	{
	case RTMP_TYPE_SET_CHUNK_SIZE:
		assert(4 == header->length);
		return rtmp_read_chunk_size(data, header->length, &rtmp->in_chunk_size);

	case RTMP_TYPE_ABORT:
		assert(4 == header->length);
		if (4 == rtmp_read_abort_message(data, header->length, &chunk_stream_id))
		{
			rtmp->onabort(rtmp->param, chunk_stream_id);
			return 4;
		}
		return 0;

	case RTMP_TYPE_ACKNOWLEDGEMENT:
		assert(4 == header->length);
		return rtmp_read_acknowledgement(data, header->length, &rtmp->sequence_number);

	case RTMP_TYPE_WINDOW_ACKNOWLEDGEMENT_SIZE:
		assert(4 == header->length);
		return rtmp_read_window_acknowledgement_size(data, header->length, &rtmp->window_size);

	case RTMP_TYPE_SET_PEER_BANDWIDTH:
		assert(5 == header->length);
		if (5 == rtmp_read_set_peer_bandwidth(data, header->length, &rtmp->peer_bandwidth, &rtmp->limit_type))
		{
			rtmp->u.client.onbandwidth(rtmp->param);
			return 5;
		}
		return 0;

	default:
		printf("unknown rtmp protocol control message: %d\n", (int)header->type);
		assert(0);
		return 0;
	}
}

#include "rtmp-internal.h"
#include "rtmp-msgtypeid.h"
#include "rtmp-event.h"
#include "rtmp-util.h"
#include <assert.h>

#define N_CHUNK_HEADER 12

static void rtmp_user_control_message_header(uint8_t* out, size_t payload)
{
	// 6.2. User Control Messages (p23)
	// RTMP uses message type ID 4 for User Control messages
	// User Control messages SHOULD use message stream ID 0 (known 
	// as the control stream) and be sent in chunk stream ID 2.
	out[0] = (0x00 << 6) /*fmt*/ | 0x02 /*cs id*/;

	/* timestamp */
	out[1] = 0x00;
	out[2] = 0x00;
	out[3] = 0x00;

	/* message length */
	out[4] = (uint8_t)(payload >> 16);
	out[5] = (uint8_t)(payload >> 8);
	out[6] = (uint8_t)payload;

	/* message type id */
	out[7] = RTMP_TYPE_EVENT;

	/* message stream id */
	out[8] = 0x00;
	out[9] = 0x00;
	out[10] = 0x00;
	out[11] = 0x00;
}

// s -> c
int rtmp_event_stream_begin(uint8_t* data, size_t bytes, uint32_t streamId)
{
	if (bytes < N_CHUNK_HEADER + 6) return 0;
	rtmp_user_control_message_header(data, 6);
	be_write_uint16(N_CHUNK_HEADER + data, RTMP_EVENT_STREAM_BEGIN);
	be_write_uint32(N_CHUNK_HEADER + data + 2, streamId);
	return N_CHUNK_HEADER + 6;
}

// s -> c
int rtmp_event_stream_eof(uint8_t* data, size_t bytes, uint32_t streamId)
{
	if (bytes < N_CHUNK_HEADER + 6) return 0;
	rtmp_user_control_message_header(data, 6);
	be_write_uint16(N_CHUNK_HEADER + data, RTMP_EVENT_STREAM_EOF);
	be_write_uint32(N_CHUNK_HEADER + data + 2, streamId);
	return N_CHUNK_HEADER + 6;
}

// s -> c
int rtmp_event_stream_dry(uint8_t* data, size_t bytes, uint32_t streamId)
{
	if (bytes < N_CHUNK_HEADER + 6) return 0;
	rtmp_user_control_message_header(data, 6);
	be_write_uint16(N_CHUNK_HEADER + data, RTMP_EVENT_STREAM_DRY);
	be_write_uint32(N_CHUNK_HEADER + data + 2, streamId);
	return N_CHUNK_HEADER + 6;
}

// c -> s
int rtmp_event_set_buffer_length(uint8_t* data, size_t bytes, uint32_t streamId, uint32_t ms)
{
	if (bytes < N_CHUNK_HEADER + 10) return 0;
	rtmp_user_control_message_header(data, 10);
	be_write_uint16(N_CHUNK_HEADER + data, RTMP_EVENT_SET_BUFFER_LENGTH);
	be_write_uint32(N_CHUNK_HEADER + data + 2, streamId);
	be_write_uint32(N_CHUNK_HEADER + data + 6, ms);
	return N_CHUNK_HEADER + 10;
}

// s -> c
int rtmp_event_stream_is_record(uint8_t* data, size_t bytes, uint32_t streamId)
{
	if (bytes < N_CHUNK_HEADER + 6) return 0;
	rtmp_user_control_message_header(data, 6);
	be_write_uint16(N_CHUNK_HEADER + data, RTMP_EVENT_STREAM_IS_RECORD);
	be_write_uint32(N_CHUNK_HEADER + data + 2, streamId);
	return N_CHUNK_HEADER + 6;
}

// s -> c
int rtmp_event_ping(uint8_t* data, size_t bytes, uint32_t timstamp)
{
	if (bytes < N_CHUNK_HEADER + 6) return 0;
	rtmp_user_control_message_header(data, 6);
	be_write_uint16(N_CHUNK_HEADER + data, RTMP_EVENT_PING);
	be_write_uint32(N_CHUNK_HEADER + data + 2, timstamp);
	return N_CHUNK_HEADER + 6;
}

// c -> s
int rtmp_event_pong(uint8_t* data, size_t bytes, uint32_t timstamp)
{
	if (bytes < N_CHUNK_HEADER + 6) return 0;
	rtmp_user_control_message_header(data, 6);
	be_write_uint16(N_CHUNK_HEADER + data, RTMP_EVENT_PONG);
	be_write_uint32(N_CHUNK_HEADER + data + 2, timstamp);
	return N_CHUNK_HEADER + 6;
}

int rtmp_event_handler(struct rtmp_t* rtmp, const struct rtmp_chunk_header_t* header, const uint8_t* data)
{
	uint16_t event = 0;
	uint32_t streamId = 0;

	if (header->length < 6) return 0;
	be_read_uint16(data, &event);
	be_read_uint32(data + 2, &streamId);

	switch (event)
	{
	case RTMP_EVENT_STREAM_BEGIN:
	case RTMP_EVENT_STREAM_DRY:
	case RTMP_EVENT_STREAM_IS_RECORD:
		return 6;

    case RTMP_EVENT_STREAM_EOF:
        rtmp->u.client.oneof(rtmp->param, streamId);
		return 6;

	case RTMP_EVENT_SET_BUFFER_LENGTH:
		if (header->length < 10) return 0;
		be_read_uint32(data + 6, &rtmp->buffer_length_ms);
		return 10;

	case RTMP_EVENT_PING:
		rtmp->u.client.onping(rtmp->param, streamId);
		return 6;

	case RTMP_EVENT_PONG:
		return 6;

	default:
		return header->length;
	}
}

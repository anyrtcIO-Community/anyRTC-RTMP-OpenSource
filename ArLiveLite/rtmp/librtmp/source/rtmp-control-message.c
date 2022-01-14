#include "rtmp-control-message.h"
#include "rtmp-msgtypeid.h"
#include "rtmp-util.h"

#define N_CHUNK_HEADER 12

static void rtmp_protocol_control_message_header(uint8_t* out, uint8_t id, size_t payload)
{
	// 5.4. Protocol Control Messages (p18)
	// These protocol control messages MUST have message stream ID 0 (known
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
	out[7] = id;

	/* message stream id */
	out[8] = 0x00;
	out[9] = 0x00;
	out[10] = 0x00;
	out[11] = 0x00;
}

// 5.4.1. Set Chunk Size (1) (p19)
int rtmp_set_chunk_size(uint8_t* out, size_t size, uint32_t chunkSize)
{	
	if (size < N_CHUNK_HEADER + 4) return 0;

	rtmp_protocol_control_message_header(out, RTMP_TYPE_SET_CHUNK_SIZE, 4);
	be_write_uint32(out + N_CHUNK_HEADER, chunkSize & 0x7FFFFFFF); // first bit MUST be zero
	return N_CHUNK_HEADER + 4;
}

// 5.4.2. Abort Message (2)  (p19)
int rtmp_abort_message(uint8_t* out, size_t size, uint32_t chunkStreamId)
{
	if (size < N_CHUNK_HEADER + 4) return 0;
	rtmp_protocol_control_message_header(out, RTMP_TYPE_ABORT, 4);
	be_write_uint32(out + N_CHUNK_HEADER, chunkStreamId);
	return N_CHUNK_HEADER + 4;
}

// 5.4.3. Acknowledgement (3) (p20)
int rtmp_acknowledgement(uint8_t* out, size_t size, uint32_t sequenceNumber)
{
	if (size < N_CHUNK_HEADER + 4) return 0;
	rtmp_protocol_control_message_header(out, RTMP_TYPE_ACKNOWLEDGEMENT, 4);
	be_write_uint32(out + N_CHUNK_HEADER, sequenceNumber);
	return N_CHUNK_HEADER + 4;
}

// 5.4.4. Window Acknowledgement Size (5) (p20)
int rtmp_window_acknowledgement_size(uint8_t* out, size_t size, uint32_t windowSize)
{
	if (size < N_CHUNK_HEADER + 4) return 0;
	rtmp_protocol_control_message_header(out, RTMP_TYPE_WINDOW_ACKNOWLEDGEMENT_SIZE, 4);
	be_write_uint32(out + N_CHUNK_HEADER, windowSize);
	return N_CHUNK_HEADER + 4;
}

// 5.4.5. Set Peer Bandwidth (6) (p21)
int rtmp_set_peer_bandwidth(uint8_t* out, size_t size, uint32_t windowSize, uint8_t limitType)
{
	if (size < N_CHUNK_HEADER + 5) return 0;
	rtmp_protocol_control_message_header(out, RTMP_TYPE_SET_PEER_BANDWIDTH, 5);
	be_write_uint32(out + N_CHUNK_HEADER, windowSize);
	out[N_CHUNK_HEADER + 4] = limitType;
	return N_CHUNK_HEADER + 5;
}

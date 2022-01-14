#ifndef _rtmp_internal_h_
#define _rtmp_internal_h_

#include "rtmp-chunk-header.h"
#include "rtmp-netconnection.h"
#include "rtmp-netstream.h"

#define N_CHUNK_STREAM	8 // maximum chunk stream count
#define N_STREAM_NAME	256

#define RTMP_STREAM_LIVE	"live"
#define RTMP_STREAM_RECORD	"record"
#define RTMP_STREAM_APPEND	"append"

#define RTMP_LEVEL_WARNING	"warning"
#define RTMP_LEVEL_STATUS	"status"
#define RTMP_LEVEL_ERROR	"error"
#define RTMP_LEVEL_FINISH	"finish" // ksyun cdn

// Chunk Stram Id
enum rtmp_channel_t
{
	RTMP_CHANNEL_PROTOCOL = 2,	// Protocol Control Messages (1,2,3,5,6) & User Control Messages Event (4)
	RTMP_CHANNEL_INVOKE,		// RTMP_TYPE_INVOKE (20) & RTMP_TYPE_FLEX_MESSAGE (17)
	RTMP_CHANNEL_AUDIO,			// RTMP_TYPE_AUDIO (8)
	RTMP_CHANNEL_VIDEO,			// RTMP_TYPE_VIDEO (9)
	RTMP_CHANNEL_DATA,			// RTMP_TYPE_DATA (18) & RTMP_TYPE_FLEX_STREAM (15)

	RTMP_CHANNEL_MAX = 65599,	// The protocol supports up to 65597 streams with IDs 3-65599(65535 + 64)
};

enum rtmp_state_t
{
	RTMP_STATE_UNINIT = 0,
	RTMP_STATE_HANDSHAKE,
	RTMP_STATE_CONNECTED,
	RTMP_STATE_CREATE_STREAM,
	RTMP_STATE_START,
	RTMP_STATE_STOP,
	RTMP_STATE_DELETE_STREAM,
};

enum rtmp_transaction_id_t
{
	RTMP_TRANSACTION_CONNECT = 1,
	RTMP_TRANSACTION_CREATE_STREAM,
	RTMP_TRANSACTION_GET_STREAM_LENGTH,
};

enum rtmp_notify_t
{
	RTMP_NOTIFY_START = 1,
	RTMP_NOTIFY_STOP,
	RTMP_NOTIFY_PAUSE,
	RTMP_NOTIFY_SEEK,
};

struct rtmp_packet_t
{
	struct rtmp_chunk_header_t header;
	uint32_t delta; // delta / timestamp
	uint32_t clock; // timestamp

	uint8_t* payload;
	size_t capacity; // only for network read
	size_t bytes; // only for network read
};

// 5.3.1. Chunk Format (p11)
/* 3-bytes basic header + 11-bytes message header + 4-bytes extended timestamp */
#define MAX_CHUNK_HEADER 18

enum rtmp_parser_state_t
{
	RTMP_PARSE_INIT = 0,
	RTMP_PARSE_BASIC_HEADER,
	RTMP_PARSE_MESSAGE_HEADER,
	RTMP_PARSE_EXTENDED_TIMESTAMP,
	RTMP_PARSE_PAYLOAD,
};

struct rtmp_parser_t
{
	uint8_t buffer[MAX_CHUNK_HEADER];
	uint32_t basic_bytes; // basic header length
	uint32_t bytes;

	enum rtmp_parser_state_t state;

	struct rtmp_packet_t* pkt;
};

struct rtmp_t
{
	uint32_t in_chunk_size; // read from network
	uint32_t out_chunk_size; // write to network

	uint32_t sequence_number; // bytes read report
	uint32_t window_size; // server bandwidth (2500000)
	uint32_t peer_bandwidth; // client bandwidth
	
	uint32_t buffer_length_ms; // s -> c

	uint8_t limit_type; // client bandwidth limit
	
	// chunk header
	struct rtmp_packet_t in_packets[N_CHUNK_STREAM]; // receive from network
	struct rtmp_packet_t out_packets[N_CHUNK_STREAM]; // send to network
	struct rtmp_parser_t parser;

	void* param;

	/// @return 0-ok, other-error
	int (*send)(void* param, const uint8_t* header, uint32_t headerBytes, const uint8_t* payload, uint32_t payloadBytes);
	
	int (*onaudio)(void* param, const uint8_t* data, size_t bytes, uint32_t timestamp);
	int (*onvideo)(void* param, const uint8_t* data, size_t bytes, uint32_t timestamp);
	int (*onscript)(void* param, const uint8_t* data, size_t bytes, uint32_t timestamp);

	void (*onabort)(void* param, uint32_t chunk_stream_id);

	union
	{
		struct
		{
			// server side
			int (*onconnect)(void* param, int r, double transaction, const struct rtmp_connect_t* connect);
			int (*oncreate_stream)(void* param, int r, double transaction);
			int (*onplay)(void* param, int r, double transaction, const char* stream_name, double start, double duration, uint8_t reset);
			int (*ondelete_stream)(void* param, int r, double transaction, double stream_id);
			int (*onreceive_audio)(void* param, int r, double transaction, uint8_t audio);
			int (*onreceive_video)(void* param, int r, double transaction, uint8_t video);
			int (*onpublish)(void* param, int r, double transaction, const char* stream_name, const char* stream_type);
			int (*onseek)(void* param, int r, double transaction, double milliSeconds);
			int (*onpause)(void* param, int r, double transaction, uint8_t pause, double milliSeconds);
			int (*onget_stream_length)(void* param, int r, double transaction, const char* stream_name);
		} server;

		struct
		{
			// client side
			int (*onconnect)(void* param);
			int (*oncreate_stream)(void* param, double stream_id);
			int (*onnotify)(void* param, enum rtmp_notify_t notify);
            int (*oneof)(void* param, uint32_t stream_id); // EOF event
			int (*onping)(void* param, uint32_t stream_id); // send pong
			int (*onbandwidth)(void* param); // send window acknowledgement size
		} client;
	} u;
};

/// @return 0-ok, other-error
int rtmp_chunk_read(struct rtmp_t* rtmp, const uint8_t* data, size_t bytes);
/// @return 0-ok, other-error
int rtmp_chunk_write(struct rtmp_t* rtmp, const struct rtmp_chunk_header_t* header, const uint8_t* payload);

int rtmp_handler(struct rtmp_t* rtmp, struct rtmp_chunk_header_t* header, const uint8_t* payload);
int rtmp_event_handler(struct rtmp_t* rtmp, const struct rtmp_chunk_header_t* header, const uint8_t* data);
int rtmp_invoke_handler(struct rtmp_t* rtmp, const struct rtmp_chunk_header_t* header, const uint8_t* data);
/// @return >0-ok, 0-error
int rtmp_control_handler(struct rtmp_t* rtmp, const struct rtmp_chunk_header_t* header, const uint8_t* data);

#endif /* !_rtmp_internal_h_ */

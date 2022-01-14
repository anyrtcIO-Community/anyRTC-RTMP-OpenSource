#ifndef _rtmp_event_h_
#define _rtmp_event_h_

// 7.1.3. Shared Object Message (19, 16) (p24)
// AMF Shared Object Message Event
enum
{
	RTMP_AMF_EVENT_USE				= 1,
	RTMP_AMF_EVENT_RELEASE			= 2,
	RTMP_AMF_EVENT_REQUEST_CHANGE	= 3,
	RTMP_AMF_EVENT_CHANGE			= 4,
	RTMP_AMF_EVENT_SUCCESS			= 5,
	RTMP_AMF_EVENT_SEND_MESSAGE		= 6,
	RTMP_AMF_EVENT_STATUS			= 7,
	RTMP_AMF_EVENT_CLEAR			= 8,
	RTMP_AMF_EVENT_REMOVE			= 9,
	RTMP_AMF_EVENT_REQUEST_REMOVE	= 10,
	RTMP_AMF_EVENT_USE_SUCCESS		= 11,
};

// 7.1.7. User Control Message Events (p27)
enum
{
	RTMP_EVENT_STREAM_BEGIN			= 0,
	RTMP_EVENT_STREAM_EOF			= 1,
	RTMP_EVENT_STREAM_DRY			= 2,
	RTMP_EVENT_SET_BUFFER_LENGTH	= 3,
	RTMP_EVENT_STREAM_IS_RECORD		= 4,

	RTMP_EVENT_PING					= 6, // RTMP_EVENT_PING_REQUEST
	RTMP_EVENT_PONG					= 7, // RTMP_EVENT_PING_RESPONSE

	// https://www.gnu.org/software/gnash/manual/doxygen/namespacegnash_1_1rtmp.html
	RTMP_EVENT_REQUEST_VERIFY		= 0x1a,
	RTMP_EVENT_RESPOND_VERIFY		= 0x1b,
	RTMP_EVENT_BUFFER_EMPTY			= 0x1f,
	RTMP_EVENT_BUFFER_READY			= 0x20,
};

int rtmp_event_stream_begin(uint8_t* data, size_t bytes, uint32_t streamId);
int rtmp_event_stream_eof(uint8_t* data, size_t bytes, uint32_t streamId);
int rtmp_event_stream_dry(uint8_t* data, size_t bytes, uint32_t streamId);
int rtmp_event_set_buffer_length(uint8_t* data, size_t bytes, uint32_t streamId, uint32_t ms);
int rtmp_event_stream_is_record(uint8_t* data, size_t bytes, uint32_t streamId);
int rtmp_event_ping(uint8_t* data, size_t bytes, uint32_t timstamp);
int rtmp_event_pong(uint8_t* data, size_t bytes, uint32_t timstamp);

#endif /* !_rtmp_event_h_ */

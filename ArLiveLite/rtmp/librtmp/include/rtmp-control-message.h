#ifndef _rtmp_control_message_h_
#define _rtmp_control_message_h_

#include <stdint.h>
#include <stddef.h>

enum
{
	// 5.4.1. Set Chunk Size (1) (p19)
	// The maximum chunk size defaults to 128 bytes
	RTMP_CHUNK_SIZE = 128,
};

/// 5.4.1. Set Chunk Size (1)
/// @return 0-error, >0-ok
int rtmp_set_chunk_size(uint8_t* out, size_t size, uint32_t chunkSize);

/// 5.4.2. Abort Message (2)
/// @return 0-error, >0-ok
int rtmp_abort_message(uint8_t* out, size_t size, uint32_t chunkStreamId);

/// 5.4.3. Acknowledgement (3)
/// @return 0-error, >0-ok
int rtmp_acknowledgement(uint8_t* out, size_t size, uint32_t sequenceNumber);

/// 5.4.4. Window Acknowledgement Size (5)
/// @return 0-error, >0-ok
int rtmp_window_acknowledgement_size(uint8_t* out, size_t size, uint32_t windowSize);

enum
{
	RTMP_BANDWIDTH_LIMIT_HARD = 0,
	RTMP_BANDWIDTH_LIMIT_SOFT = 1,
	RTMP_BANDWIDTH_LIMIT_DYNAMIC = 2,
};

/// 5.4.5. Set Peer Bandwidth (6)
/// @return 0-error, >0-ok
int rtmp_set_peer_bandwidth(uint8_t* out, size_t size, uint32_t windowSize, uint8_t limitType);

#endif /* !_rtmp_control_message_h_ */

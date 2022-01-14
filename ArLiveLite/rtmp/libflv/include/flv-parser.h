#ifndef _flv_parser_h_
#define _flv_parser_h_

#include <stdint.h>
#include <stddef.h>
#include "flv-header.h"

#if defined(__cplusplus)
extern "C" {
#endif

/// Audio/Video Elementary Stream
/// @param[in] param user-defined parameter
/// @param[in] codec audio/video format (see more flv-proto.h)
/// @param[in] data audio/video element data, AAC: AAC-Frame, H.264: MP4 Stream, MP3-Raw data
/// @param[in] bytes data length in byte
/// @param[in] pts audio/video presentation timestamp
/// @param[in] dts audio/video decoding timestamp
/// @param[in] flags 1-video keyframe, other-undefined
/// @return 0-ok, other-error
typedef int (*flv_parser_handler)(void* param, int codec, const void* data, size_t bytes, uint32_t pts, uint32_t dts, int flags);

/// Input FLV Audio/Video Stream
/// @param[in] type 8-audio, 9-video, 18-script (see more flv-proto.h)
/// @param[in] data flv audio/video Stream, AudioTagHeader/VideoTagHeader + A/V Data
/// @param[in] bytes data length in byte
/// @param[in] timestamp milliseconds relative to the first tag(DTS)
/// @return 0-ok, other-error
int flv_parser_tag(int type, const void* data, size_t bytes, uint32_t timestamp, flv_parser_handler handler, void* param);

struct flv_parser_t
{
	int state;

	size_t bytes;
	size_t expect;
	uint8_t ptr[32];
	struct flv_header_t header;
	struct flv_tag_header_t tag;
	struct flv_audio_tag_header_t audio;
	struct flv_video_tag_header_t video;

	uint8_t* body;
	void* (*alloc)(void* param, size_t bytes);
	void (*free)(void* param, void* ptr);
};

int flv_parser_input(struct flv_parser_t* parser, const uint8_t* data, size_t bytes, flv_parser_handler handler, void* param);

#if defined(__cplusplus)
}
#endif
#endif /* !_flv_parser_h_ */

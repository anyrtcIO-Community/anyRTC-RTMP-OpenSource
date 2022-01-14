#ifndef _flv_demuxer_h_
#define _flv_demuxer_h_

#include <stdint.h>
#include <stddef.h>

#if defined(__cplusplus)
extern "C" {
#endif

typedef struct flv_demuxer_t flv_demuxer_t;

/// Audio/Video Elementary Stream
/// @param[in] param user-defined parameter
/// @param[in] codec audio/video format (see more flv-proto.h)
/// @param[in] data audio/video element data, AAC: ADTS + AAC-Frame, H.264: startcode + NALU, MP3-Raw data
/// @param[in] bytes data length in byte
/// @param[in] pts audio/video presentation timestamp
/// @param[in] dts audio/video decoding timestamp
/// @param[in] flags 1-video keyframe, other-undefined
/// @return 0-ok, other-error
typedef int (*flv_demuxer_handler)(void* param, int codec, const void* data, size_t bytes, uint32_t pts, uint32_t dts, int flags);

flv_demuxer_t* flv_demuxer_create(flv_demuxer_handler handler, void* param);
void flv_demuxer_destroy(flv_demuxer_t* demuxer);

/// Input FLV Audio/Video Stream
/// @param[in] type 8-audio, 9-video, 18-script (see more flv-proto.h)
/// @param[in] data flv audio/video Stream, AudioTagHeader/VideoTagHeader + A/V Data
/// @param[in] bytes data length in byte
/// @param[in] timestamp milliseconds relative to the first tag(DTS)
/// @return 0-ok, other-error
int flv_demuxer_input(flv_demuxer_t* demuxer, int type, const void* data, size_t bytes, uint32_t timestamp);

#if defined(__cplusplus)
}
#endif
#endif /* !_flv_demuxer_h_ */

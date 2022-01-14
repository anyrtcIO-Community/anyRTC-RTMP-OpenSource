#ifndef _flv_header_h_
#define _flv_header_h_

#include <stdint.h>
#include <stddef.h>

#if defined(__cplusplus)
extern "C" {
#endif

struct flv_header_t
{
	uint8_t FLV[3];
	uint8_t version;
	uint8_t audio;
	uint8_t video;
	uint32_t offset; // data offset
};

struct flv_tag_header_t
{
	uint8_t filter; // 0-No pre-processing required
	uint8_t type; // 8-audio, 9-video, 18-script data
	uint32_t size; // data size
	uint32_t timestamp;
	uint32_t streamId;
};

struct flv_audio_tag_header_t
{
	uint8_t codecid;	/// audio codec id: FLV_AUDIO_AAC
	uint8_t rate;		/// audio sample frequence: 0-5.5 kHz, 1-11 kHz, 2-22 kHz, 3-44 kHz
	uint8_t bits;		/// audio sample bits: 0-8 bit samples, 1-16-bit samples
	uint8_t channels;	/// audio channel count: 0-Mono sound, 1-Stereo sound
	uint8_t avpacket;	/// AAC only:FLV_SEQUENCE_HEADER/FLV_AVPACKET
};

struct flv_video_tag_header_t
{
	uint8_t codecid;	/// video codec id: FLV_VIDEO_H264
	uint8_t keyframe;	/// video frame type: 1-key frame, 2-inter frame
	uint8_t avpacket;	/// H.264/H.265/AV1 only:FLV_SEQUENCE_HEADER/FLV_AVPACKET/FLV_END_OF_SEQUENCE
	int32_t cts;		/// video composition time(PTS - DTS), AVC/HEVC/AV1 only
};

/// Read FLV File Header
/// @return >=0-header length in byte, <0-error
int flv_header_read(struct flv_header_t* flv, const uint8_t* buf, size_t len);

/// Write FLV File Header
/// @param[in] audio 1-has audio, 0-don't have
/// @param[in] video 1-has video, 0-don't have
/// @param[out] buf flv header buffer
/// @param[out] len flv header length
/// @return >=0-header length in byte, <0-error
int flv_header_write(int audio, int video, uint8_t* buf, size_t len);


/// Read FLV Tag Header
/// @return >=0-header length in byte, <0-error
int flv_tag_header_read(struct flv_tag_header_t* tag, const uint8_t* buf, size_t len);

/// Write FLV Tag Header
/// @param[out] buf flv tag header buffer
/// @param[out] len flv tag header length
/// @return >=0-header length in byte, <0-error
int flv_tag_header_write(const struct flv_tag_header_t* tag, uint8_t* buf, size_t len);


/// Read FLV Audio Tag Header
/// @param[out] audio flv audio parameter
/// @param[in] buf flv audio tag header buffer
/// @param[in] len flv audio tag header length
/// @return >=0-header length in byte, <0-error
int flv_audio_tag_header_read(struct flv_audio_tag_header_t* audio, const uint8_t* buf, size_t len);

/// Write FLV Audio Tag Header
/// @param[in] audio flv audio parameter
/// @param[out] buf flv audio tag header buffer
/// @param[out] len flv audio tag header length
/// @return >=0-header length in byte, <0-error
int flv_audio_tag_header_write(const struct flv_audio_tag_header_t* audio, uint8_t* buf, size_t len);


/// Read FLV Video Tag Header
/// @param[out] video flv video parameter
/// @param[in] buf flv video tag header buffer
/// @param[in] len flv video tag header length
/// @return >=0-header length in byte, <0-error
int flv_video_tag_header_read(struct flv_video_tag_header_t* video, const uint8_t* buf, size_t len);

/// Write FLV Video Tag Header
/// @param[in] video flv video parameter
/// @param[out] buf flv video tag header buffer
/// @param[out] len flv video tag header length
/// @return >=0-header length in byte, <0-error
int flv_video_tag_header_write(const struct flv_video_tag_header_t* video, uint8_t* buf, size_t len);


/// Read FLV Data Tag Header
/// @return >=0-header length in byte, <0-error
int flv_data_tag_header_read(const uint8_t* buf, size_t len);

/// Write FLV Data Tag Header
/// @param[out] buf flv data tag header buffer
/// @param[out] len flv data tag header length
/// @return >=0-header length in byte, <0-error
int flv_data_tag_header_write(uint8_t* buf, size_t len);


/// Read/Write FLV previous tag size
int flv_tag_size_read(const uint8_t* buf, size_t len, uint32_t* size);
int flv_tag_size_write(uint8_t* buf, size_t len, uint32_t size);

#if defined(__cplusplus)
}
#endif
#endif /* !_flv_header_h_ */

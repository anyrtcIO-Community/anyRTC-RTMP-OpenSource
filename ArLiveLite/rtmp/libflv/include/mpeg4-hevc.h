#ifndef _mpeg4_hevc_h_
#define _mpeg4_hevc_h_

#include <stdint.h>
#include <stddef.h>

#if defined(__cplusplus)
extern "C" {
#endif

struct mpeg4_hevc_t
{
	uint8_t  configurationVersion;	// 1-only
	uint8_t  general_profile_space;	// 2bit,[0,3]
	uint8_t  general_tier_flag;		// 1bit,[0,1]
	uint8_t  general_profile_idc;	// 5bit,[0,31]
	uint32_t general_profile_compatibility_flags;
	uint64_t general_constraint_indicator_flags;
	uint8_t  general_level_idc;
	uint16_t min_spatial_segmentation_idc;
	uint8_t  parallelismType;		// 2bit,[0,3]
	uint8_t  chromaFormat;			// 2bit,[0,3]
	uint8_t  bitDepthLumaMinus8;	// 3bit,[0,7]
	uint8_t  bitDepthChromaMinus8;	// 3bit,[0,7]
	uint16_t avgFrameRate;
	uint8_t  constantFrameRate;		// 2bit,[0,3]
	uint8_t  numTemporalLayers;		// 3bit,[0,7]
	uint8_t  temporalIdNested;		// 1bit,[0,1]
	uint8_t  lengthSizeMinusOne;	// 2bit,[0,3]

	uint8_t  numOfArrays;
	struct
	{
		uint8_t array_completeness;
		uint8_t type; // nalu type
		uint16_t bytes;
		uint8_t* data;
	} nalu[64];

	uint8_t array_completeness;
	uint8_t data[4 * 1024];
	size_t off;
};

int mpeg4_hevc_decoder_configuration_record_load(const uint8_t* data, size_t bytes, struct mpeg4_hevc_t* hevc);

int mpeg4_hevc_decoder_configuration_record_save(const struct mpeg4_hevc_t* hevc, uint8_t* data, size_t bytes);

int mpeg4_hevc_to_nalu(const struct mpeg4_hevc_t* hevc, uint8_t* data, size_t bytes);

int mpeg4_hevc_codecs(const struct mpeg4_hevc_t* hevc, char* codecs, size_t bytes);

int h265_annexbtomp4(struct mpeg4_hevc_t* hevc, const void* data, size_t bytes, void* out, size_t size, int *vcl, int* update);

int h265_mp4toannexb(const struct mpeg4_hevc_t* hevc, const void* data, size_t bytes, void* out, size_t size);

/// h265_is_new_access_unit H.265 new access unit(frame)
/// @return 1-new access, 0-not a new access
int h265_is_new_access_unit(const uint8_t* nalu, size_t bytes);

#if defined(__cplusplus)
}
#endif
#endif /* !_mpeg4_hevc_h_ */

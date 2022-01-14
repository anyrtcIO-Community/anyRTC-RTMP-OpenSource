#ifndef _aom_av1_h_
#define _aom_av1_h_

#include <stdint.h>
#include <stddef.h>

#if defined(__cplusplus)
extern "C" {
#endif

struct aom_av1_t
{
	uint32_t marker : 1;
	uint32_t version : 7;
	uint32_t seq_profile : 3;
	uint32_t seq_level_idx_0 : 5;
	uint32_t seq_tier_0 : 1;
	uint32_t high_bitdepth : 1;
	uint32_t twelve_bit : 1;
	uint32_t monochrome : 1;
	uint32_t chroma_subsampling_x : 1;
	uint32_t chroma_subsampling_y : 1;
	uint32_t chroma_sample_position : 2;

	uint32_t reserved : 3;
	uint32_t initial_presentation_delay_present : 1;
	uint32_t initial_presentation_delay_minus_one : 4;

	uint8_t buffer_delay_length_minus_1; // decoder_model_info
	uint32_t width; // max_frame_width_minus_1
	uint32_t height; // max_frame_height_minus_1

	uint16_t bytes;
	uint8_t data[2 * 1024];
};

/// Create av1 codec configuration record from Sequence Header OBU
/// @param[in] data av1 low overhead bitstream format
/// @return 0-ok, other-error
int aom_av1_codec_configuration_record_init(struct aom_av1_t* av1, const void* data, size_t bytes);

int aom_av1_codec_configuration_record_load(const uint8_t* data, size_t bytes, struct aom_av1_t* av1);
int aom_av1_codec_configuration_record_save(const struct aom_av1_t* av1, uint8_t* data, size_t bytes);

/// @param[in] data av1 split low overhead/annexb bitstream format to obu
int aom_av1_obu_split(const uint8_t* data, size_t bytes, int (*handler)(void* param, const uint8_t* obu, size_t bytes), void* param);
int aom_av1_annexb_split(const uint8_t* data, size_t bytes, int (*handler)(void* param, const uint8_t* obu, size_t bytes), void* param);

int aom_av1_codecs(const struct aom_av1_t* av1, char* codecs, size_t bytes);

#if defined(__cplusplus)
}
#endif
#endif /* !_aom_av1_h_ */

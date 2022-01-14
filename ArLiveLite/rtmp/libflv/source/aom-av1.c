#include "aom-av1.h"
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <stdio.h>
#include "mpeg4-bits.h"

// https://aomediacodec.github.io/av1-isobmff
// https://aomediacodec.github.io/av1-avif/

enum
{
	OBU_SEQUENCE_HEADER = 1,
	OBU_TEMPORAL_DELIMITER = 2,
	OBU_FRAME_HEADER = 3,
	OBU_TILE_GROUP = 4,
	OBU_METADATA = 5,
	OBU_FRAME = 6,
	OBU_REDUNDANT_FRAME_HEADER = 7,
	OBU_TILE_LIST = 8,
	// 9-14	Reserved
	OBU_PADDING = 15,
};

/*
aligned (8) class AV1CodecConfigurationRecord {
  unsigned int (1) marker = 1;
  unsigned int (7) version = 1;
  unsigned int (3) seq_profile;
  unsigned int (5) seq_level_idx_0;
  unsigned int (1) seq_tier_0;
  unsigned int (1) high_bitdepth;
  unsigned int (1) twelve_bit;
  unsigned int (1) monochrome;
  unsigned int (1) chroma_subsampling_x;
  unsigned int (1) chroma_subsampling_y;
  unsigned int (2) chroma_sample_position;
  unsigned int (3) reserved = 0;

  unsigned int (1) initial_presentation_delay_present;
  if (initial_presentation_delay_present) {
	unsigned int (4) initial_presentation_delay_minus_one;
  } else {
	unsigned int (4) reserved = 0;
  }

  unsigned int (8)[] configOBUs;
}
*/

int aom_av1_codec_configuration_record_load(const uint8_t* data, size_t bytes, struct aom_av1_t* av1)
{
	if (bytes < 4)
		return -1;

	av1->marker = data[0] >> 7;
	av1->version = data[0] & 0x7F;
	av1->seq_profile = data[1] >> 5;
	av1->seq_level_idx_0 = data[1] & 0x1F;

	av1->seq_tier_0 = data[2] >> 7;
	av1->high_bitdepth = (data[2] >> 6) & 0x01;
	av1->twelve_bit = (data[2] >> 5) & 0x01;
	av1->monochrome = (data[2] >> 4) & 0x01;
	av1->chroma_subsampling_x = (data[2] >> 3) & 0x01;
	av1->chroma_subsampling_y = (data[2] >> 2) & 0x01;
	av1->chroma_sample_position = data[2] & 0x03;

	av1->reserved = data[3] >> 5;
	av1->initial_presentation_delay_present = (data[3] >> 4) & 0x01;
	av1->initial_presentation_delay_minus_one = data[3] & 0x0F;

	if (bytes - 4 > sizeof(av1->data))
		return -1;

	av1->bytes = (uint16_t)(bytes - 4);
	memcpy(av1->data, data + 4, av1->bytes);
	return (int)bytes;
}

int aom_av1_codec_configuration_record_save(const struct aom_av1_t* av1, uint8_t* data, size_t bytes)
{
	if (bytes < (size_t)av1->bytes + 4)
		return 0; // don't have enough memory

	data[0] = (uint8_t)((av1->marker << 7) | av1->version);
	data[1] = (uint8_t)((av1->seq_profile << 5) | av1->seq_level_idx_0);
	data[2] = (uint8_t)((av1->seq_tier_0 << 7) | (av1->high_bitdepth << 6) | (av1->twelve_bit << 5) | (av1->monochrome << 4) | (av1->chroma_subsampling_x << 3) | (av1->chroma_subsampling_y << 2) | av1->chroma_sample_position);
	data[3] = (uint8_t)((av1->initial_presentation_delay_present << 4) | av1->initial_presentation_delay_minus_one);

	memcpy(data + 4, av1->data, av1->bytes);
	return av1->bytes + 4;
}

static inline const uint8_t* leb128(const uint8_t* data, int bytes, uint64_t* v)
{
	int i;
	uint64_t b;

	b = 0x80;
	for (*v = i = 0; i * 7 < 64 && i < bytes && 0 != (b & 0x80); i++)
	{
		b = data[i];
		*v |= (b & 0x7F) << (i * 7);
	}
	return data + i;
}

int aom_av1_annexb_split(const uint8_t* data, size_t bytes, int (*handler)(void* param, const uint8_t* obu, size_t bytes), void* param)
{
	int r;
	uint64_t n[3];
	const uint8_t* temporal, * frame, * obu;

	r = 0;
	for (temporal = data; temporal < data + bytes && 0 == r; temporal += n[0])
	{
		// temporal_unit_size
		temporal = leb128(temporal, (int)(data + bytes - temporal), &n[0]);
		if (temporal + n[0] > data + bytes)
			return -1;

		for (frame = temporal; frame < temporal + n[0] && 0 == r; frame += n[1])
		{
			// frame_unit_size
			frame = leb128(frame, (int)(temporal + n[0] - frame), &n[1]);
			if (frame + n[1] > temporal + n[0])
				return -1;

			for (obu = frame; obu < frame + n[1] && 0 == r; obu += n[2])
			{
				obu = leb128(obu, (int)(frame + n[1] - obu), &n[2]);
				if (obu + n[2] > frame + n[1])
					return -1;

				r = handler(param, obu, (size_t)n[2]);
			}
		}
	}

	return r;
}

int aom_av1_obu_split(const uint8_t* data, size_t bytes, int (*handler)(void* param, const uint8_t* obu, size_t bytes), void* param)
{
	int r;
	size_t i;
	size_t offset;
	uint64_t len;
	uint8_t obu_type;
	const uint8_t* ptr;

	for (i = r = 0; i < bytes && 0 == r; i += (size_t)len)
	{
		// http://aomedia.org/av1/specification/syntax/#obu-header-syntax
		obu_type = (data[i] >> 3) & 0x0F;
		if (data[i] & 0x04) // obu_extension_flag
		{
			// http://aomedia.org/av1/specification/syntax/#obu-extension-header-syntax
			// temporal_id = (obu[1] >> 5) & 0x07;
			// spatial_id = (obu[1] >> 3) & 0x03;
			offset = 2;
		}
		else
		{
			offset = 1;
		}

		if (data[i] & 0x02) // obu_has_size_field
		{
			ptr = leb128(data + i + offset, (int)(bytes - i - offset), &len);
			if (ptr + len > data + bytes)
				return -1;
			len += ptr - data - i;
		}
		else
		{
			len = bytes - i;
		}

		r = handler(param, data + i, (size_t)len);
	}

	return r;
}

// http://aomedia.org/av1/specification/syntax/#color-config-syntax
static int aom_av1_color_config(struct mpeg4_bits_t* bits, struct aom_av1_t* av1)
{
	uint8_t BitDepth;
	uint8_t color_primaries;
	uint8_t transfer_characteristics;
	uint8_t matrix_coefficients;

	av1->high_bitdepth = mpeg4_bits_read(bits);
	if (av1->seq_profile == 2 && av1->high_bitdepth)
	{
		av1->twelve_bit = mpeg4_bits_read(bits);
		BitDepth = av1->twelve_bit ? 12 : 10;
	}
	else if (av1->seq_profile <= 2)
	{
		BitDepth = av1->high_bitdepth ? 10 : 8;
	}
	else
	{
		assert(0);
		BitDepth = 8;
	}

	if (av1->seq_profile == 1)
	{
		av1->monochrome = 0;
	}
	else
	{
		av1->monochrome = mpeg4_bits_read(bits);
	}

	if (mpeg4_bits_read(bits)) // color_description_present_flag
	{
		color_primaries = mpeg4_bits_read_uint8(bits, 8); // color_primaries
		transfer_characteristics = mpeg4_bits_read_uint8(bits, 8); // transfer_characteristics
		matrix_coefficients = mpeg4_bits_read_uint8(bits, 8); // matrix_coefficients
	}
	else
	{
		// http://aomedia.org/av1/specification/semantics/#color-config-semantics
		color_primaries = 2; // CP_UNSPECIFIED;
		transfer_characteristics = 2; // TC_UNSPECIFIED;
		matrix_coefficients = 2; // MC_UNSPECIFIED;
	}

	if (av1->monochrome)
	{
		mpeg4_bits_read(bits); // color_range
		av1->chroma_subsampling_x = 1;
		av1->chroma_subsampling_y = 1;
	}
	else if (color_primaries == 1 /*CP_BT_709*/ && transfer_characteristics == 13 /*TC_SRGB*/ && matrix_coefficients == 0 /*MC_IDENTITY*/)
	{
		av1->chroma_subsampling_x = 0;
		av1->chroma_subsampling_y = 0;
	}
	else
	{
		mpeg4_bits_read(bits); // color_range
		if (av1->seq_profile == 0)
		{
			av1->chroma_subsampling_x = 1;
			av1->chroma_subsampling_y = 1;
		}
		else if (av1->seq_profile == 1)
		{
			av1->chroma_subsampling_x = 0;
			av1->chroma_subsampling_y = 0;
		}
		else
		{
			if (BitDepth == 12)
			{
				av1->chroma_subsampling_x = mpeg4_bits_read(bits);
				if (av1->chroma_subsampling_x)
					av1->chroma_subsampling_y = mpeg4_bits_read(bits);
				else
					av1->chroma_subsampling_y = 0;
			}
			else
			{
				av1->chroma_subsampling_x = 1;
				av1->chroma_subsampling_y = 0;
			}
		}

		if (av1->chroma_subsampling_x && av1->chroma_subsampling_y)
			av1->chroma_sample_position = mpeg4_bits_read_uint32(bits, 2);
	}

	mpeg4_bits_read(bits); // separate_uv_delta_q
	return 0;
}

// http://aomedia.org/av1/specification/syntax/#timing-info-syntax
static int aom_av1_timing_info(struct mpeg4_bits_t* bits, struct aom_av1_t* av1)
{
	(void)av1;
	mpeg4_bits_read_n(bits, 32); // num_units_in_display_tick
	mpeg4_bits_read_n(bits, 32); // time_scale
	if(mpeg4_bits_read(bits)) // equal_picture_interval
		mpeg4_bits_read_uvlc(bits); // num_ticks_per_picture_minus_1
	return 0;
}

// http://aomedia.org/av1/specification/syntax/#decoder-model-info-syntax
static int aom_av1_decoder_model_info(struct mpeg4_bits_t* bits, struct aom_av1_t* av1)
{
	av1->buffer_delay_length_minus_1 = mpeg4_bits_read_uint8(bits, 5); // buffer_delay_length_minus_1
	mpeg4_bits_read_n(bits, 32); // num_units_in_decoding_tick
	mpeg4_bits_read_n(bits, 5); // buffer_removal_time_length_minus_1
	mpeg4_bits_read_n(bits, 5); // frame_presentation_time_length_minus_1
	return 0;
}

// http://aomedia.org/av1/specification/syntax/#operating-parameters-info-syntax
static int aom_av1_operating_parameters_info(struct mpeg4_bits_t* bits, struct aom_av1_t* av1, int op)
{
	uint8_t n;
	n = av1->buffer_delay_length_minus_1 + 1;
	mpeg4_bits_read_n(bits, n); // decoder_buffer_delay[ op ]
	mpeg4_bits_read_n(bits, n); // encoder_buffer_delay[ op ]
	mpeg4_bits_read(bits); // low_delay_mode_flag[ op ]
	(void)op;
	return 0;
}

// http://aomedia.org/av1/specification/syntax/#sequence-header-obu-syntax
static int aom_av1_obu_sequence_header(struct aom_av1_t* av1, const void* data, size_t bytes)
{
	uint8_t i;
	uint8_t reduced_still_picture_header;
	uint8_t decoder_model_info_present_flag;
	uint8_t operating_points_cnt_minus_1;
	uint8_t frame_width_bits_minus_1;
	uint8_t frame_height_bits_minus_1;
	uint8_t enable_order_hint;
	uint8_t seq_force_screen_content_tools;
	struct mpeg4_bits_t bits;

	mpeg4_bits_init(&bits, (void*)data, bytes);
	av1->seq_profile = mpeg4_bits_read_uint32(&bits, 3);
	mpeg4_bits_read(&bits); // still_picture
	reduced_still_picture_header = mpeg4_bits_read_uint8(&bits, 1);
	if (reduced_still_picture_header)
	{
		av1->initial_presentation_delay_present = 0; // initial_display_delay_present_flag
		av1->seq_level_idx_0 = mpeg4_bits_read_uint32(&bits, 5);
		av1->seq_tier_0 = 0;
		decoder_model_info_present_flag = 0;
	}
	else
	{
		if (mpeg4_bits_read(&bits)) // timing_info_present_flag
		{
			// timing_info( )
			aom_av1_timing_info(&bits, av1);

			decoder_model_info_present_flag = mpeg4_bits_read_uint8(&bits, 1); // decoder_model_info_present_flag
			if (decoder_model_info_present_flag)
			{
				// decoder_model_info( )
				aom_av1_decoder_model_info(&bits, av1);
			}
		}
		else
		{
			decoder_model_info_present_flag = 0;
		}

		av1->initial_presentation_delay_present = mpeg4_bits_read(&bits); //  initial_display_delay_present_flag =
		operating_points_cnt_minus_1 = mpeg4_bits_read_uint8(&bits, 5);
		for (i = 0; i <= operating_points_cnt_minus_1; i++)
		{
			uint8_t seq_level_idx;
			uint8_t seq_tier;
			uint8_t initial_display_delay_minus_1;

			mpeg4_bits_read_n(&bits, 12); // operating_point_idc[ i ]
			seq_level_idx = mpeg4_bits_read_uint8(&bits, 5); // seq_level_idx[ i ]
			if (seq_level_idx > 7)
			{
				seq_tier = mpeg4_bits_read_uint8(&bits, 1); // seq_tier[ i ]
			}
			else
			{
				seq_tier = 0;
			}

			if (decoder_model_info_present_flag)
			{
				if (mpeg4_bits_read(&bits)) // decoder_model_present_for_this_op[i]
				{
					aom_av1_operating_parameters_info(&bits, av1, i);
				}
			}

			if (av1->initial_presentation_delay_present && mpeg4_bits_read(&bits)) // initial_display_delay_present_for_this_op[ i ]
				initial_display_delay_minus_1 = mpeg4_bits_read_uint8(&bits, 4); // initial_display_delay_minus_1[ i ]
			else
				initial_display_delay_minus_1 = 0;

			if (0 == i)
			{
				av1->seq_level_idx_0 = seq_level_idx;
				av1->seq_tier_0 = seq_tier;
				av1->initial_presentation_delay_minus_one = initial_display_delay_minus_1;
			}
		}
	}

	// choose_operating_point( )
	frame_width_bits_minus_1 = mpeg4_bits_read_uint8(&bits, 4);
	frame_height_bits_minus_1 = mpeg4_bits_read_uint8(&bits, 4);
	av1->width = 1 + mpeg4_bits_read_uint32(&bits, frame_width_bits_minus_1 + 1); // max_frame_width_minus_1
	av1->height = 1 + mpeg4_bits_read_uint32(&bits, frame_height_bits_minus_1 + 1); // max_frame_height_minus_1

	if (!reduced_still_picture_header && mpeg4_bits_read(&bits)) // frame_id_numbers_present_flag
	{
		mpeg4_bits_read_n(&bits, 4); // delta_frame_id_length_minus_2
		mpeg4_bits_read_n(&bits, 3); // additional_frame_id_length_minus_1
	}

	mpeg4_bits_read(&bits); // use_128x128_superblock
	mpeg4_bits_read(&bits); // enable_filter_intra
	mpeg4_bits_read(&bits); // enable_intra_edge_filter

	if (!reduced_still_picture_header)
	{
		mpeg4_bits_read(&bits); // enable_interintra_compound
		mpeg4_bits_read(&bits); // enable_masked_compound
		mpeg4_bits_read(&bits); // enable_warped_motion
		mpeg4_bits_read(&bits); // enable_dual_filter
		enable_order_hint = mpeg4_bits_read_uint8(&bits, 1);
		if (enable_order_hint)
		{
			mpeg4_bits_read(&bits); // enable_jnt_comp
			mpeg4_bits_read(&bits); // enable_ref_frame_mvs
		}
		if (mpeg4_bits_read(&bits)) // seq_choose_screen_content_tools
		{
			seq_force_screen_content_tools = 2; // SELECT_SCREEN_CONTENT_TOOLS;
		}
		else
		{
			seq_force_screen_content_tools = mpeg4_bits_read_uint8(&bits, 1); // seq_force_screen_content_tools
		}

		if (seq_force_screen_content_tools > 0)
		{
			if (!mpeg4_bits_read(&bits)) // seq_choose_integer_mv
				mpeg4_bits_read(&bits); // seq_force_integer_mv
			//else
			// seq_force_integer_mv = SELECT_INTEGER_MV
		}
		else
		{
			//seq_force_integer_mv = SELECT_INTEGER_MV;
		}

		if (enable_order_hint)
		{
			mpeg4_bits_read_n(&bits, 3); // order_hint_bits_minus_1
		}
	}

	mpeg4_bits_read(&bits); // enable_superres
	mpeg4_bits_read(&bits); // enable_cdef
	mpeg4_bits_read(&bits); // enable_restoration

	// color_config( )
	aom_av1_color_config(&bits, av1);

	mpeg4_bits_read(&bits); // film_grain_params_present

	return mpeg4_bits_error(&bits) ? -1 : 0;
}

// http://aomedia.org/av1/specification/syntax/#general-obu-syntax
static int aom_av1_extra_handler(void* param, const uint8_t* obu, size_t bytes)
{
	uint64_t i;
	uint64_t len;
	size_t offset;
	uint8_t obu_type;
	const uint8_t* ptr;
	struct aom_av1_t* av1;
	
	av1 = (struct aom_av1_t*)param;
	if (bytes < 2)
		return -1;

	// http://aomedia.org/av1/specification/syntax/#obu-header-syntax
	obu_type = (obu[0] >> 3) & 0x0F;
	if (obu[0] & 0x04) // obu_extension_flag
	{
		// http://aomedia.org/av1/specification/syntax/#obu-extension-header-syntax
		// temporal_id = (obu[1] >> 5) & 0x07;
		// spatial_id = (obu[1] >> 3) & 0x03;
		offset = 2;
	}
	else
	{
		offset = 1;
	}

	if (obu[0] & 0x02) // obu_has_size_field
	{
		ptr = leb128(obu + offset, (int)(bytes - offset), &len);
		if (ptr + len > obu + bytes)
			return -1;
	}
	else
	{
		ptr = obu + offset;
		len = bytes - offset;
	}

	if (OBU_SEQUENCE_HEADER == obu_type || OBU_METADATA == obu_type)
	{
		if (av1->bytes + bytes + 8 /*leb128*/ >= sizeof(av1->data))
			return -1;

		av1->data[av1->bytes++] = obu[0] | 0x02 /*obu_has_size_field*/;
		if (obu[0] & 0x04) // obu_extension_flag
			av1->data[av1->bytes++] = obu[1];

		//if (0 == (obu[0] & 0x02))
		{
			// fill obu size, leb128
			for(i = len; i >= 0x80; av1->bytes++)
			{
				av1->data[av1->bytes] = (uint8_t)(i & 0x7F);
				av1->data[av1->bytes] |= 0x80;
				i >>= 7;
			}
			av1->data[av1->bytes++] = (uint8_t)(i & 0x7F);
		}
		memcpy(av1->data + av1->bytes, ptr, (size_t)len);
		av1->bytes += (uint16_t)len;
	}

	// http://aomedia.org/av1/specification/semantics/#obu-header-semantics
	if (obu_type == OBU_SEQUENCE_HEADER)
	{
		return aom_av1_obu_sequence_header(av1, ptr, (size_t)len);
	}

	return 0;
}

// https://aomediacodec.github.io/av1-isobmff/#av1codecconfigurationbox-section
int aom_av1_codec_configuration_record_init(struct aom_av1_t* av1, const void* data, size_t bytes)
{
	av1->version = 1;
	av1->marker = 1;
	return aom_av1_obu_split((const uint8_t*)data, bytes, aom_av1_extra_handler, av1);
}

int aom_av1_codecs(const struct aom_av1_t* av1, char* codecs, size_t bytes)
{
	unsigned int bitdepth;

	// AV1 5.5.2.Color config syntax
	if (2 == av1->seq_profile && av1->high_bitdepth)
		bitdepth = av1->twelve_bit ? 12 : 10;
	else
		bitdepth = av1->high_bitdepth ? 10 : 8;

	// https://aomediacodec.github.io/av1-isobmff/#codecsparam
	// https://developer.mozilla.org/en-US/docs/Web/Media/Formats/codecs_parameter
	// <sample entry 4CC>.<profile>.<level><tier>.<bitDepth>.<monochrome>.<chromaSubsampling>.<colorPrimaries>.<transferCharacteristics>.<matrixCoefficients>.<videoFullRangeFlag>
	return snprintf(codecs, bytes, "av01.%u.%02u%c.%02u", (unsigned int)av1->seq_profile, (unsigned int)av1->seq_level_idx_0, av1->seq_tier_0 ? 'H' : 'M', (unsigned int)bitdepth);
}

#if defined(_DEBUG) || defined(DEBUG)
void aom_av1_test(void)
{
	const unsigned char src[] = {
		0x81, 0x04, 0x0c, 0x00, 0x0a, 0x0b, 0x00, 0x00, 0x00, 0x24, 0xcf, 0x7f, 0x0d, 0xbf, 0xff, 0x30, 0x08
	};
	unsigned char data[sizeof(src)];

	struct aom_av1_t av1;
	assert(sizeof(src) == aom_av1_codec_configuration_record_load(src, sizeof(src), &av1));
	assert(1 == av1.version && 0 == av1.seq_profile && 4 == av1.seq_level_idx_0);
	assert(0 == av1.seq_tier_0 && 0 == av1.high_bitdepth && 0 == av1.twelve_bit && 0 == av1.monochrome && 1 == av1.chroma_subsampling_x && 1 == av1.chroma_subsampling_y && 0 == av1.chroma_sample_position);
	assert(0 == av1.initial_presentation_delay_present && 0 == av1.initial_presentation_delay_minus_one);
	assert(13 == av1.bytes);
	assert(sizeof(src) == aom_av1_codec_configuration_record_save(&av1, data, sizeof(data)));
	assert(0 == memcmp(src, data, sizeof(src)));

	aom_av1_codecs(&av1, (char*)data, sizeof(data));
	assert(0 == memcmp("av01.0.04M.08", data, 13));
}

void aom_av1_sequence_header_obu_test(void)
{
	const uint8_t obu[] = { /*0x0A, 0x0B,*/ 0x00, 0x00, 0x00, 0x2C, 0xCF, 0x7F, 0x0D, 0xBF, 0xFF, 0x38, 0x18 };
	
	struct aom_av1_t av1;
	memset(&av1, 0, sizeof(av1));
	assert(0 == aom_av1_obu_sequence_header(&av1, obu, sizeof(obu)));
}

void aom_av1_obu_test(const char* file)
{
	size_t n;
	FILE* fp;
	struct aom_av1_t av1;
	static uint8_t buffer[24 * 1024 * 1024];
	aom_av1_sequence_header_obu_test();
	memset(&av1, 0, sizeof(av1));
	fp = fopen(file, "rb");
	n = fread(buffer, 1, sizeof(buffer), fp);
	aom_av1_codec_configuration_record_init(&av1, buffer, n);
	fclose(fp);
}
#endif

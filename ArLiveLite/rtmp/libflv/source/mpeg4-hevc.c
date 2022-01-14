#include "mpeg4-hevc.h"
#include <stdio.h>
#include <string.h>
#include <assert.h>

#define H265_VPS		32
#define H265_SPS		33
#define H265_PPS		34
#define H265_PREFIX_SEI 39
#define H265_SUFFIX_SEI 40

static uint8_t* w32(uint8_t* p, uint32_t v)
{
	*p++ = (uint8_t)(v >> 24);
	*p++ = (uint8_t)(v >> 16);
	*p++ = (uint8_t)(v >> 8);
	*p++ = (uint8_t)v;
	return p;
}

static uint8_t* w16(uint8_t* p, uint16_t v)
{
	*p++ = (uint8_t)(v >> 8);
	*p++ = (uint8_t)v;
	return p;
}

/*
ISO/IEC 14496-15:2017(E) 8.3.3.1.2 Syntax (p71)

aligned(8) class HEVCDecoderConfigurationRecord {
	unsigned int(8) configurationVersion = 1;
	unsigned int(2) general_profile_space;
	unsigned int(1) general_tier_flag;
	unsigned int(5) general_profile_idc;
	unsigned int(32) general_profile_compatibility_flags;
	unsigned int(48) general_constraint_indicator_flags;
	unsigned int(8) general_level_idc;
	bit(4) reserved = '1111'b;
	unsigned int(12) min_spatial_segmentation_idc;
	bit(6) reserved = '111111'b;
	unsigned int(2) parallelismType;
	bit(6) reserved = '111111'b;
	unsigned int(2) chromaFormat;
	bit(5) reserved = '11111'b;
	unsigned int(3) bitDepthLumaMinus8;
	bit(5) reserved = '11111'b;
	unsigned int(3) bitDepthChromaMinus8;
	bit(16) avgFrameRate;
	bit(2) constantFrameRate;
	bit(3) numTemporalLayers;
	bit(1) temporalIdNested;
	unsigned int(2) lengthSizeMinusOne;
	unsigned int(8) numOfArrays;
	for (j=0; j < numOfArrays; j++) {
		bit(1) array_completeness;
		unsigned int(1) reserved = 0;
		unsigned int(6) NAL_unit_type;
		unsigned int(16) numNalus;
		for (i=0; i< numNalus; i++) {
			unsigned int(16) nalUnitLength;
			bit(8*nalUnitLength) nalUnit;
		}
	}
}
*/
int mpeg4_hevc_decoder_configuration_record_load(const uint8_t* data, size_t bytes, struct mpeg4_hevc_t* hevc)
{
	uint8_t nalutype;
	uint16_t i, j, k, n, numOfArrays;
	const uint8_t* p;
	uint8_t* dst;

	if (bytes < 23)
		return -1;

	hevc->configurationVersion = data[0];
	if (1 != hevc->configurationVersion)
		return -1;

	hevc->general_profile_space = (data[1] >> 6) & 0x03;
	hevc->general_tier_flag = (data[1] >> 5) & 0x01;
	hevc->general_profile_idc = data[1] & 0x1F;
	hevc->general_profile_compatibility_flags = (data[2] << 24) | (data[3] << 16) | (data[4] << 8) | data[5];
    hevc->general_constraint_indicator_flags = ((uint32_t)data[6] << 24) | ((uint32_t)data[7] << 16) | ((uint32_t)data[8] << 8) | (uint32_t)data[9];
	hevc->general_constraint_indicator_flags = (hevc->general_constraint_indicator_flags << 16) | (((uint64_t)data[10]) << 8) | data[11];
	hevc->general_level_idc = data[12];
	hevc->min_spatial_segmentation_idc = ((data[13] & 0x0F) << 8) | data[14];
	hevc->parallelismType = data[15] & 0x03;
	hevc->chromaFormat = data[16] & 0x03;
	hevc->bitDepthLumaMinus8 = data[17] & 0x07;
	hevc->bitDepthChromaMinus8 = data[18] & 0x07;
	hevc->avgFrameRate = (data[19] << 8) | data[20];
	hevc->constantFrameRate = (data[21] >> 6) & 0x03;
	hevc->numTemporalLayers = (data[21] >> 3) & 0x07;
	hevc->temporalIdNested = (data[21] >> 2) & 0x01;
	hevc->lengthSizeMinusOne = data[21] & 0x03;
	numOfArrays = data[22];
	
	p = data + 23;
	dst = hevc->data;
	hevc->numOfArrays = 0;
	for (i = 0; i < numOfArrays; i++)
	{
		if (p + 3 > data + bytes)
			return -1;

		nalutype = p[0];
		n = (p[1] << 8) | p[2];
		p += 3;

		for (j = 0; j < n; j++)
		{
			if (hevc->numOfArrays >= sizeof(hevc->nalu) / sizeof(hevc->nalu[0]))
			{
				assert(0);
				return -1; // too many nalu(s)
			}

			if (p + 2 > data + bytes)
				return -1;

			k = (p[0] << 8) | p[1];
			if (p + 2 + k > data + bytes || dst + k > hevc->data + sizeof(hevc->data))
			{
				assert(0);
				return -1;
			}

			assert((nalutype & 0x3F) == ((p[2] >> 1) & 0x3F));
			hevc->nalu[hevc->numOfArrays].array_completeness = (nalutype >> 7) & 0x01;
			hevc->nalu[hevc->numOfArrays].type = nalutype & 0x3F;
			hevc->nalu[hevc->numOfArrays].bytes = k;
			hevc->nalu[hevc->numOfArrays].data = dst;
			memcpy(hevc->nalu[hevc->numOfArrays].data, p + 2, k);
			hevc->numOfArrays++;

			p += 2 + k;
			dst += k;
		}
	}

	hevc->off = (int)(dst - hevc->data);
	return (int)(p - data);
}

int mpeg4_hevc_decoder_configuration_record_save(const struct mpeg4_hevc_t* hevc, uint8_t* data, size_t bytes)
{
	uint16_t n;
	uint8_t i, j, k;
	uint8_t *ptr, *end;
	uint8_t *p = data;
	uint8_t array_completeness = 1;
	const uint8_t nalu[] = {H265_VPS, H265_SPS, H265_PPS, H265_PREFIX_SEI, H265_SUFFIX_SEI};

	assert(hevc->lengthSizeMinusOne <= 3);
	end = data + bytes;
	if (bytes < 23)
		return 0; // don't have enough memory

	// HEVCDecoderConfigurationRecord
	// ISO/IEC 14496-15:2017
	// 8.3.3.1.2 Syntax
	assert(1 == hevc->configurationVersion);
	data[0] = hevc->configurationVersion;

	// general_profile_space + general_tier_flag + general_profile_idc
	data[1] = ((hevc->general_profile_space & 0x03) << 6) | ((hevc->general_tier_flag & 0x01) << 5) | (hevc->general_profile_idc & 0x1F);

	// general_profile_compatibility_flags
	w32(data + 2, hevc->general_profile_compatibility_flags);

	// general_constraint_indicator_flags
	w32(data + 6, (uint32_t)(hevc->general_constraint_indicator_flags >> 16));
	w16(data + 10, (uint16_t)hevc->general_constraint_indicator_flags);
	
	// general_level_idc
	data[12] = hevc->general_level_idc;
	
	// min_spatial_segmentation_idc
	w16(data + 13, 0xF000 | hevc->min_spatial_segmentation_idc);

	data[15] = 0xFC | hevc->parallelismType;
	data[16] = 0xFC | hevc->chromaFormat;
	data[17] = 0xF8 | hevc->bitDepthLumaMinus8;
	data[18] = 0xF8 | hevc->bitDepthChromaMinus8;
	w16(data + 19, hevc->avgFrameRate);
	data[21] = (hevc->constantFrameRate << 6) | ((hevc->numTemporalLayers & 0x07) << 3) | ((hevc->temporalIdNested & 0x01) << 2) | (hevc->lengthSizeMinusOne & 0x03);
//	data[22] = hevc->numOfArrays;

	p = data + 23;
	for (k = i = 0; i < sizeof(nalu)/sizeof(nalu[0]); i++)
	{
		ptr = p + 3;
		for (n = j = 0; j < hevc->numOfArrays; j++)
		{
			assert(hevc->nalu[j].type == ((hevc->nalu[j].data[0] >> 1) & 0x3F));
			if(nalu[i] != hevc->nalu[j].type)
				continue;

			if (ptr + 2 + hevc->nalu[j].bytes > end)
				return 0; // don't have enough memory

			array_completeness = hevc->nalu[j].array_completeness;
			assert(hevc->nalu[i].data + hevc->nalu[j].bytes <= hevc->data + sizeof(hevc->data));
			w16(ptr, hevc->nalu[j].bytes);
			memcpy(ptr + 2, hevc->nalu[j].data, hevc->nalu[j].bytes);
			ptr += 2 + hevc->nalu[j].bytes;
			n++;
		}

		if (n > 0)
		{
			// array_completeness + NAL_unit_type
			p[0] = (array_completeness << 7) | (nalu[i] & 0x3F);
			w16(p + 1, n);
			p = ptr;
			k++;
		}
	}

	data[22] = k;

	return (int)(p - data);
}

int mpeg4_hevc_to_nalu(const struct mpeg4_hevc_t* hevc, uint8_t* data, size_t bytes)
{
	uint8_t i;
	uint8_t* p, *end;
	const uint8_t startcode[] = { 0, 0, 0, 1 };

	p = data;
	end = p + bytes;
	
	for (i = 0; i < hevc->numOfArrays; i++)
	{
		if (p + hevc->nalu[i].bytes + 4 > end)
			return -1;

		memcpy(p, startcode, 4);
		memcpy(p + 4, hevc->nalu[i].data, hevc->nalu[i].bytes);
		assert(hevc->nalu[i].type == ((hevc->nalu[i].data[0] >> 1) & 0x3F));
		p += 4 + hevc->nalu[i].bytes;
	}

	return (int)(p - data);
}

int mpeg4_hevc_codecs(const struct mpeg4_hevc_t* hevc, char* codecs, size_t bytes)
{
    // ISO/IEC 14496-15:2017(E) 
    // Annex E Sub-parameters of the MIME type "codecs" parameter (p154)
    // 'hev1.' or 'hvc1.' prefix (5 chars)
    // profile, e.g. '.A12' (max 4 chars)
    // profile_compatibility reserve bit order, dot + 32-bit hex number (max 9 chars)
    // tier and level, e.g. '.H120' (max 5 chars)
    // up to 6 constraint bytes, bytes are dot-separated and hex-encoded.
    const char* tier = "LH";
    const char* space[] = { "", "A", "B", "C" };
    uint32_t x;
    x = hevc->general_profile_compatibility_flags;
    x = ((x >> 1) & 0x55555555) | ((x & 0x55555555) << 1);
    x = ((x >> 2) & 0x33333333) | ((x & 0x33333333) << 2);
    x = ((x >> 4) & 0x0f0f0f0f) | ((x & 0x0f0f0f0f) << 4);
    x = ((x >> 8) & 0x00ff00ff) | ((x & 0x00ff00ff) << 8);
    x = (x >> 16) | (x << 16);
    return snprintf(codecs, bytes, "hvc1.%s%u.%x.%c%u", space[hevc->general_profile_space%4], (unsigned int)hevc->general_profile_idc, (unsigned int)x, tier[hevc->general_tier_flag%2], (unsigned int)hevc->general_level_idc);
}

#if defined(_DEBUG) || defined(DEBUG)
void hevc_annexbtomp4_test(void);
void mpeg4_hevc_test(void)
{
	const unsigned char src[] = {
		0x01,0x01,0x60,0x00,0x00,0x00,0x80,0x00,0x00,0x00,0x00,0x00,0xb4,0xf0,0x00,
		0xfc,0xfd,0xf8,0xf8,0x00,0x00,0x0f,0x03,0xa0,0x00,0x01,0x00,0x18,0x40,0x01,
		0x0c,0x01,0xff,0xff,0x01,0x60,0x00,0x00,0x03,0x00,0x80,0x00,0x00,0x03,0x00,
		0x00,0x03,0x00,0xb4,0x9d,0xc0,0x90,0xa1,0x00,0x01,0x00,0x29,0x42,0x01,0x01,
		0x01,0x60,0x00,0x00,0x03,0x00,0x80,0x00,0x00,0x03,0x00,0x00,0x03,0x00,0xb4,
		0xa0,0x01,0xe0,0x20,0x02,0x1c,0x59,0x67,0x79,0x24,0x6d,0xae,0x01,0x00,0x00,
		0x03,0x03,0xe8,0x00,0x00,0x5d,0xc0,0x08,0xa2,0x00,0x01,0x00,0x06,0x44,0x01,
		0xc1,0x73,0xd1,0x89
	};
	const unsigned char nalu[] = {
		0x00,0x00,0x00,0x01,0x40,0x01,0x0c,0x01,0xff,0xff,0x01,0x60,0x00,0x00,0x03,
		0x00,0x80,0x00,0x00,0x03,0x00,0x00,0x03,0x00,0xb4,0x9d,0xc0,0x90,0x00,0x00,
		0x00,0x01,0x42,0x01,0x01,0x01,0x60,0x00,0x00,0x03,0x00,0x80,0x00,0x00,0x03,
		0x00,0x00,0x03,0x00,0xb4,0xa0,0x01,0xe0,0x20,0x02,0x1c,0x59,0x67,0x79,0x24,
		0x6d,0xae,0x01,0x00,0x00,0x03,0x03,0xe8,0x00,0x00,0x5d,0xc0,0x08,0x00,0x00,
		0x00,0x01,0x44,0x01,0xc1,0x73,0xd1,0x89
	};
	unsigned char data[sizeof(src)];

	struct mpeg4_hevc_t hevc;
	assert(sizeof(src) == mpeg4_hevc_decoder_configuration_record_load(src, sizeof(src), &hevc));
	assert(0 == hevc.general_profile_space && 0 == hevc.general_tier_flag);
	assert(1 == hevc.general_profile_idc && 0xb4 == hevc.general_level_idc);
	assert(1 == hevc.numTemporalLayers && 1 == hevc.temporalIdNested);
	assert(3 == hevc.numOfArrays);
	assert(sizeof(src) == mpeg4_hevc_decoder_configuration_record_save(&hevc, data, sizeof(data)));
	assert(0 == memcmp(src, data, sizeof(src)));
    mpeg4_hevc_codecs(&hevc, (char*)data, sizeof(data));
    assert(0 == memcmp("hvc1.1.6.L180", data, 13));

	assert(sizeof(nalu) == mpeg4_hevc_to_nalu(&hevc, data, sizeof(data)));
	assert(0 == memcmp(nalu, data, sizeof(nalu)));

	hevc_annexbtomp4_test();
}
#endif

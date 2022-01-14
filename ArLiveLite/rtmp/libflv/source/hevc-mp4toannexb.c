#include "mpeg4-hevc.h"
#include <string.h>
#include <assert.h>

#define H265_NAL_VPS 32
#define H265_NAL_SPS 33
#define H265_NAL_PPS 34
#define H265_NAL_AUD 35 // Access unit delimiter

int h265_mp4toannexb(const struct mpeg4_hevc_t* hevc, const void* data, size_t bytes, void* out, size_t size)
{
	int r;
	uint8_t i;
	uint8_t* dst, *dstend;
	const uint8_t* src, *srcend;
	const uint8_t startcode[] = { 0, 0, 0, 1 };
	uint8_t irap, nalu_type;
	uint32_t nalu_size;
	uint8_t vps_sps_pps_flag;

	srcend = (uint8_t*)data + bytes;
	dst = (uint8_t*)out;
	dstend = dst + size;
	vps_sps_pps_flag = 0;

	for(src = (uint8_t*)data; src + hevc->lengthSizeMinusOne + 1 < srcend; src += nalu_size)
	{
		nalu_size = 0;
		for (i = 0; i < hevc->lengthSizeMinusOne + 1; i++)
			nalu_size = (nalu_size << 8) | src[i];
		src += hevc->lengthSizeMinusOne + 1;
		if (nalu_size < 1 || src + nalu_size > srcend)
		{
			assert(0);
			return 0; // invalid
		}

		nalu_type = (src[0] >> 1) & 0x3F;
#if defined(H2645_FILTER_AUD)
		if (H265_NAL_AUD == nalu_type)
			continue; // ignore AUD
#endif

		if (H265_NAL_VPS == nalu_type || H265_NAL_SPS == nalu_type || H265_NAL_PPS == nalu_type)
			vps_sps_pps_flag = 1;

		irap = 16 <= nalu_type && nalu_type <= 23;
		if (irap && 0 == vps_sps_pps_flag)
		{
			r = mpeg4_hevc_to_nalu(hevc, dst, dstend - dst);
			if (r <= 0)
				return 0; // don't have enough memory

			dst += r;
			vps_sps_pps_flag = 1;
		}

		if (dst + nalu_size + 4 > dstend)
			return 0; // don't have enough memory
		memcpy(dst, startcode, 4);
		memcpy(dst + 4, src, nalu_size);
		dst += 4 + nalu_size;
	}

	return (int)(dst - (uint8_t*)out);
}

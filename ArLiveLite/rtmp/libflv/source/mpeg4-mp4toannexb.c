// ISO/IEC 14496-1:2010(E)
// Annex I: Usage of ITU-T Recommendation H.264 | ISO/IEC 14496-10 AVC (p150)

#include "mpeg4-avc.h"
#include <string.h>
#include <assert.h>
#include <stdint.h>
#include <stddef.h>

#define H264_NAL_IDR		5 // Coded slice of an IDR picture
#define H264_NAL_SPS		7 // Sequence parameter set
#define H264_NAL_PPS		8 // Picture parameter set
#define H264_NAL_AUD		9 // Access unit delimiter

static int h264_sps_pps_size(const struct mpeg4_avc_t* avc)
{
	int i, n = 0;
	for (i = 0; i < avc->nb_sps; i++)
		n += avc->sps[i].bytes + 4;
	for (i = 0; i < avc->nb_pps; i++)
		n += avc->pps[i].bytes + 4;
	return n;
}

int h264_mp4toannexb(const struct mpeg4_avc_t* avc, const void* data, size_t bytes, void* out, size_t size)
{
	int i, n;
	uint8_t sps_pps_flag;
	uint8_t* dst;
	const uint8_t* src, *end;
	const uint8_t h264_start_code[] = { 0x00, 0x00, 0x00, 0x01 };

	sps_pps_flag = 0;
	dst = (uint8_t*)out;
	end = (const uint8_t*)data + bytes;
	for (src = (const uint8_t*)data; src + avc->nalu + 1 < end; src += n + avc->nalu)
	{
		for (n = 0, i = 0; i < avc->nalu; i++)
			n = (n << 8) + ((uint8_t*)src)[i];
		
#if defined(DEBUG) || defined(_DEBUG)
		// fix 0x00 00 00 01 => flv nalu size
		if (1 == n && (3 == avc->nalu || 4 == avc->nalu))
			n = (int)(end - src) - avc->nalu;
#endif

		if (n <= 0 || src + n + avc->nalu > end)
		{
			assert(0);
			return 0;
		}

		// insert SPS/PPS before IDR frame
		switch (src[avc->nalu] & 0x1f)
		{
		case H264_NAL_SPS:
		case H264_NAL_PPS:
			//flv->data[k++] = 0; // SPS/PPS add zero_byte(ITU H.264 B.1.2 Byte stream NAL unit semantics)
			sps_pps_flag = 1;
			break;

		case H264_NAL_IDR:
			if (0 == sps_pps_flag)
			{
				sps_pps_flag = 1; // don't insert more than one-times
				if (dst != out)
				{
					// write sps/pps at first
					i = h264_sps_pps_size(avc);
					memmove((uint8_t*)out + i, out, dst - (uint8_t*)out);
				}
				i = mpeg4_avc_to_nalu(avc, out, size);
				if (i <= 0)
					return 0;
				dst += i;
			}
			break;
#if defined(H2645_FILTER_AUD)
		case H264_NAL_AUD:
			continue; // ignore AUD
#endif
		}

		if (dst + n + sizeof(h264_start_code) > (uint8_t*)out + size)
			return 0;

		memcpy(dst, h264_start_code, sizeof(h264_start_code));
		memcpy(dst + sizeof(h264_start_code), src + avc->nalu, n);
		dst += sizeof(h264_start_code) + n;
	}

	assert(src == end);
	return (int)(dst - (uint8_t*)out);
}

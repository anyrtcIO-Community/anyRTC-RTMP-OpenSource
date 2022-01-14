// ISO/IEC 14496-1:2010(E)
// Annex I: Usage of ITU-T Recommendation H.264 | ISO/IEC 14496-10 AVC (p150)
//
// 1. Start Codes shall not be present in the stream. The field indicating the size of each following NAL unit
//    shall be added before NAL unit.The size of this field is defined in DecoderSpecificInfo.
// 2. It is recommended encapsulating one NAL unit in one SL packet when it is delivered over lossy environment.

#include "mpeg4-avc.h"
#include <string.h>
#include <assert.h>
#include <stdint.h>
#include <stddef.h>

#define H264_NAL_IDR		5 // Coded slice of an IDR picture
#define H264_NAL_SPS		7 // Sequence parameter set
#define H264_NAL_PPS		8 // Picture parameter set
#define H264_NAL_AUD		9 // Access unit delimiter

#define H2645_BITSTREAM_FORMAT_DETECT

struct h264_annexbtomp4_handle_t
{
	struct mpeg4_avc_t* avc;
	int errcode;
	int* update; // avc sps/pps update flags
	int* vcl;

	uint8_t* out;
	size_t bytes;
	size_t capacity;
};

static const uint8_t* h264_startcode(const uint8_t *data, size_t bytes)
{
	size_t i;
	for (i = 2; i + 1 < bytes; i++)
	{
		if (0x01 == data[i] && 0x00 == data[i - 1] && 0x00 == data[i - 2])
			return data + i + 1;
	}

	return NULL;
}

/// @return >0-ok, <=0-error
static inline int h264_avcc_length(const uint8_t* h264, size_t bytes, size_t avcc)
{
	size_t i;
	uint32_t n;

	n = 0;
	assert(3 <= avcc && avcc <= 4);
	for (i = 0; i < avcc && i < bytes; i++)
		n = (n << 8) | h264[i];
	return avcc >= bytes ? -1 : (int)n;
}

/// @return 1-true, 0-false
static int mpeg4_h264_avcc_bitstream_valid(const uint8_t* h264, size_t bytes, size_t avcc)
{
	size_t n;

	while(avcc + 1 < bytes)
	{
		n = h264_avcc_length(h264, bytes, avcc);
		if (n < 0 || n + avcc > bytes)
			return 0; // invalid

		h264 += n + avcc;
		bytes -= n + avcc;
	}

	return 0 == bytes ? 1 : 0;
}

/// @return 0-annexb, >0-avcc, <0-error
int mpeg4_h264_bitstream_format(const uint8_t* h264, size_t bytes)
{
	uint32_t n;
	if (bytes < 4)
		return -1;

	n = ((uint32_t)h264[0]) << 16 | ((uint32_t)h264[1]) << 8 | ((uint32_t)h264[2]);
	if (0 == n && h264[3] <= 1)
	{
		return 0; // annexb
	}
	else if(1 == n)
	{
		// try avcc & annexb
		return mpeg4_h264_avcc_bitstream_valid(h264, bytes, 4) ? 4 : 0;
	}
	else
	{
		// try avcc 4/3 bytes
		return mpeg4_h264_avcc_bitstream_valid(h264, bytes, 4) ? 4 : (mpeg4_h264_avcc_bitstream_valid(h264, bytes, 3) ? 3 : -1);
	}
}

static int mpeg4_h264_avcc_nalu(const void* h264, size_t bytes, int avcc, void (*handler)(void* param, const uint8_t* nalu, size_t bytes), void* param)
{
	uint32_t n;
	const uint8_t* p, * end;

	p = (const uint8_t*)h264;
	end = (const uint8_t*)h264 + bytes;
	for(n = h264_avcc_length(p, (int)(end - p), avcc); p + n + avcc <= end; n = h264_avcc_length(p, (int)(end - p), avcc))
	{
		assert(n > 0);
		if (n > 0)
		{
			handler(param, p + avcc, (int)n);
		}

		p += n + avcc;
	}

	return 0;
}

///@param[in] h264 H.264 byte stream format data(A set of NAL units)
int mpeg4_h264_annexb_nalu(const void* h264, size_t bytes, void (*handler)(void* param, const uint8_t* nalu, size_t bytes), void* param)
{
	ptrdiff_t n;
	const uint8_t* p, *next, *end;

#if defined(H2645_BITSTREAM_FORMAT_DETECT)
	int avcc;
	avcc = mpeg4_h264_bitstream_format(h264, bytes);
	if (avcc > 0)
		return mpeg4_h264_avcc_nalu(h264, bytes, avcc, handler, param);
#endif

	end = (const uint8_t*)h264 + bytes;
	p = h264_startcode((const uint8_t*)h264, bytes);

	while (p)
	{
		next = h264_startcode(p, (int)(end - p));
		if (next)
		{
			n = next - p - 3;
		}
		else
		{
			n = end - p;
		}

		while (n > 0 && 0 == p[n - 1]) n--; // filter tailing zero

		assert(n > 0);
		if (n > 0)
		{
			handler(param, p, (int)n);
		}

		p = next;
	}

	return 0;
}

uint8_t mpeg4_h264_read_ue(const uint8_t* data, size_t bytes, size_t* offset)
{
	int bit, i;
	int leadingZeroBits = -1;

	for (bit = 0; !bit && *offset / 8 < bytes; ++leadingZeroBits)
	{
		bit = (data[*offset / 8] >> (7 - (*offset % 8))) & 0x01;
		++*offset;
	}

	bit = 0;
	assert(leadingZeroBits < 32);
	for (i = 0; i < leadingZeroBits && *offset / 8 < bytes; i++)
	{
		bit = (bit << 1) | ((data[*offset / 8] >> (7 - (*offset % 8))) & 0x01);
		++*offset;
	}

	return (uint8_t)((1 << leadingZeroBits) - 1 + bit);
}

static void mpeg4_avc_remove(struct mpeg4_avc_t* avc, uint8_t* ptr, size_t bytes, const uint8_t* end)
{
	uint8_t i;
	assert(ptr >= avc->data && ptr + bytes <= end && end <= avc->data + sizeof(avc->data));
	memmove(ptr, ptr + bytes, end - ptr - bytes);

	for (i = 0; i < avc->nb_sps; i++)
	{
		if (avc->sps[i].data > ptr)
			avc->sps[i].data -= bytes;
	}

	for (i = 0; i < avc->nb_pps; i++)
	{
		if (avc->pps[i].data > ptr)
			avc->pps[i].data -= bytes;
	}
}

static int h264_sps_copy(struct mpeg4_avc_t* avc, const uint8_t* nalu, size_t bytes)
{
	size_t i;
	size_t offset;
    uint8_t spsid;

	if (bytes < 4 + 1)
	{
		assert(0);
		return -1; // invalid length
	}

	offset = 4 * 8; // 1-NALU + 3-profile+flags+level
	spsid = mpeg4_h264_read_ue(nalu, bytes, &offset);

	for (i = 0; i < avc->nb_sps; i++)
	{
		offset = 4 * 8; // reset offset
		if (spsid == mpeg4_h264_read_ue(avc->sps[i].data, avc->sps[i].bytes, &offset))
		{
			if (bytes == avc->sps[i].bytes && 0 == memcmp(nalu, avc->sps[i].data, bytes))
				return 0; // do nothing

			if (bytes > avc->sps[i].bytes && avc->off + (bytes - avc->sps[i].bytes) > sizeof(avc->data))
			{
				assert(0);
				return -1; // too big
			}

			mpeg4_avc_remove(avc, avc->sps[i].data, avc->sps[i].bytes, avc->data + avc->off);
			avc->off -= avc->sps[i].bytes;

			avc->sps[i].data = avc->data + avc->off;
			avc->sps[i].bytes = (uint16_t)bytes;
			memcpy(avc->sps[i].data, nalu, bytes);
			avc->off += bytes;
			return 1; // set update flag
		}
	}

	// copy new
	assert(avc->nb_sps < sizeof(avc->sps) / sizeof(avc->sps[0]));
	if (avc->nb_sps >= sizeof(avc->sps) / sizeof(avc->sps[0])
		|| avc->off + bytes > sizeof(avc->data))
	{
		assert(0);
		return -1;
	}

	avc->sps[avc->nb_sps].data = avc->data + avc->off;
	avc->sps[avc->nb_sps].bytes = (uint16_t)bytes;
	memcpy(avc->sps[avc->nb_sps].data, nalu, bytes);
	avc->off += bytes;
	++avc->nb_sps;
	return 1; // set update flag
}

static int h264_pps_copy(struct mpeg4_avc_t* avc, const uint8_t* nalu, size_t bytes)
{
	size_t i;
    size_t offset;
	uint8_t spsid;
	uint8_t ppsid;

	if (bytes < 1 + 1)
	{
		assert(0);
		return -1; // invalid length
	}

	offset = 1 * 8; // 1-NALU
	ppsid = mpeg4_h264_read_ue(nalu, bytes, &offset);
	spsid = mpeg4_h264_read_ue(nalu, bytes, &offset);

	for (i = 0; i < avc->nb_pps; i++)
	{
		offset = 1 * 8; // reset offset
		if (ppsid == mpeg4_h264_read_ue(avc->pps[i].data, avc->pps[i].bytes, &offset) && spsid == mpeg4_h264_read_ue(avc->pps[i].data, avc->pps[i].bytes, &offset))
		{
			if (bytes == avc->pps[i].bytes && 0 == memcmp(nalu, avc->pps[i].data, bytes))
				return 0; // do nothing

			if (bytes > avc->pps[i].bytes && avc->off + (bytes - avc->pps[i].bytes) > sizeof(avc->data))
			{
				assert(0);
				return -1; // too big
			}

			mpeg4_avc_remove(avc, avc->pps[i].data, avc->pps[i].bytes, avc->data + avc->off);
			avc->off -= avc->pps[i].bytes;

			avc->pps[i].data = avc->data + avc->off;
			avc->pps[i].bytes = (uint16_t)bytes;
			memcpy(avc->pps[i].data, nalu, bytes);
			avc->off += bytes;
			return 1; // set update flag
		}
	}

	// copy new
	assert((unsigned int)avc->nb_pps < sizeof(avc->pps) / sizeof(avc->pps[0]));
	if ((unsigned int)avc->nb_pps >= sizeof(avc->pps) / sizeof(avc->pps[0])
		|| avc->off + bytes > sizeof(avc->data))
	{
		assert(0);
		return -1;
	}

	avc->pps[avc->nb_pps].data = avc->data + avc->off;
	avc->pps[avc->nb_pps].bytes = (uint16_t)bytes;
	memcpy(avc->pps[avc->nb_pps].data, nalu, bytes);
	avc->off += bytes;
	++avc->nb_pps;
	return 1; // set update flag
}

int mpeg4_avc_update(struct mpeg4_avc_t* avc, const uint8_t* nalu, size_t bytes)
{
	int r;
	
	switch (nalu[0] & 0x1f)
	{
	case H264_NAL_SPS:
		r = h264_sps_copy(avc, nalu, bytes);
		if (1 == avc->nb_sps)
		{
			// update profile/level once only
			avc->profile = nalu[1];
			avc->compatibility = nalu[2];
			avc->level = nalu[3];
		}
		break;

	case H264_NAL_PPS:
		r = h264_pps_copy(avc, nalu, bytes);
		break;

	default:
		r = 0;
	}

	return r;
}

static void h264_handler(void* param, const uint8_t* nalu, size_t bytes)
{
	int r;
	uint8_t nalutype;
	struct h264_annexbtomp4_handle_t* mp4;
	mp4 = (struct h264_annexbtomp4_handle_t*)param;

	if (bytes < 1)
	{
		assert(0);
		return;
	}

	nalutype = (nalu[0]) & 0x1f;
#if defined(H2645_FILTER_AUD)
	if (H264_NAL_AUD == nalutype)
		return; // ignore AUD
#endif

	r = mpeg4_avc_update(mp4->avc, nalu, bytes);
	if (1 == r && mp4->update)
		*mp4->update = 1;
	else if (r < 0)
		mp4->errcode = r;
	
	// IDR-1, B/P-2, other-0
	if (mp4->vcl && 1 <= nalutype && nalutype <= H264_NAL_IDR)
		*mp4->vcl = nalutype == H264_NAL_IDR ? 1 : 2;

	if (mp4->capacity >= mp4->bytes + bytes + 4)
	{
		mp4->out[mp4->bytes + 0] = (uint8_t)((bytes >> 24) & 0xFF);
		mp4->out[mp4->bytes + 1] = (uint8_t)((bytes >> 16) & 0xFF);
		mp4->out[mp4->bytes + 2] = (uint8_t)((bytes >> 8) & 0xFF);
		mp4->out[mp4->bytes + 3] = (uint8_t)((bytes >> 0) & 0xFF);
		memmove(mp4->out + mp4->bytes + 4, nalu, bytes);
		mp4->bytes += bytes + 4;
	}
	else
	{
		mp4->errcode = -1;
	}
}

int h264_annexbtomp4(struct mpeg4_avc_t* avc, const void* data, size_t bytes, void* out, size_t size, int* vcl, int* update)
{
	struct h264_annexbtomp4_handle_t h;
	memset(&h, 0, sizeof(h));
	h.avc = avc;
	h.vcl = vcl;
	h.update = update;
	h.out = (uint8_t*)out;
	h.capacity = size;
	if (vcl) *vcl = 0;
	if (update) *update = 0;
	
	mpeg4_h264_annexb_nalu(data, bytes, h264_handler, &h);
	avc->nalu = 4;
	return 0 == h.errcode ? (int)h.bytes : 0;
}

/// h264_is_new_access_unit H.264 new access unit(frame)
/// @return 1-new access, 0-not a new access
int h264_is_new_access_unit(const uint8_t* nalu, size_t bytes)
{
    enum { NAL_NIDR = 1, NAL_PARTITION_A = 2, NAL_IDR = 5, NAL_SEI = 6, NAL_SPS = 7, NAL_PPS = 8, NAL_AUD = 9, };
    
    uint8_t nal_type;
    
    if(bytes < 2)
        return 0;
    
    nal_type = nalu[0] & 0x1f;
    
    // 7.4.1.2.3 Order of NAL units and coded pictures and association to access units
    if(NAL_AUD == nal_type || NAL_SPS == nal_type || NAL_PPS == nal_type || NAL_SEI == nal_type || (14 <= nal_type && nal_type <= 18))
        return 1;
    
    // 7.4.1.2.4 Detection of the first VCL NAL unit of a primary coded picture
    if(NAL_NIDR == nal_type || NAL_PARTITION_A == nal_type || NAL_IDR == nal_type)
    {
        // Live555 H264or5VideoStreamParser::parse
        // The high-order bit of the byte after the "nal_unit_header" tells us whether it's
        // the start of a new 'access unit' (and thus the current NAL unit ends an 'access unit'):
        return (nalu[1] & 0x80) ? 1 : 0; // first_mb_in_slice
    }
    
    return 0;
}

#if defined(_DEBUG) || defined(DEBUG)
static void mpeg4_h264_bitstream_format_test(void)
{
	const uint8_t bs3[] = { 0x00,0x00,0x01,0x67,0x42,0xe0,0x1e,0xab,0xcd, };
	const uint8_t bs4[] = { 0x00,0x00,0x00,0x01,0x67,0x42,0xe0,0x1e,0xab,0xcd, };
	const uint8_t bs5[] = { 0x00,0x00,0x00,0x00,0x01,0x67,0x42,0xe0,0x1e,0xab,0xcd, };
	const uint8_t avcc3[] = { 0x00,0x00,0x06,0x67,0x42,0xe0,0x1e,0xab,0xcd, };
	const uint8_t avcc4[] = { 0x00,0x00,0x00,0x06,0x67,0x42,0xe0,0x1e,0xab,0xcd, };
	assert(0 == mpeg4_h264_bitstream_format(bs3, sizeof(bs3)));
	assert(0 == mpeg4_h264_bitstream_format(bs4, sizeof(bs4)));
	assert(0 == mpeg4_h264_bitstream_format(bs5, sizeof(bs5)));
	assert(3 == mpeg4_h264_bitstream_format(avcc3, sizeof(avcc3)));
	assert(4 == mpeg4_h264_bitstream_format(avcc4, sizeof(avcc4)));	
}

static void mpeg4_annexbtomp4_test2(void)
{
	const uint8_t sps[] = { 0x00,0x00,0x00,0x01,0x67,0x42,0xe0,0x1e,0xab,0xcd, };
	const uint8_t pps[] = { 0x00,0x00,0x00,0x01,0x28,0xce,0x3c,0x80 };
	const uint8_t sps1[] = { 0x00,0x00,0x00,0x01,0x67,0x42,0xe0,0x1e,0x4b,0xcd, 0x01 };
	const uint8_t pps1[] = { 0x00,0x00,0x00,0x01,0x28,0xce,0x3c,0x80, 0x01 };
	const uint8_t sps2[] = { 0x00,0x00,0x00,0x01,0x67,0x42,0xe0,0x1e,0xab };
	const uint8_t pps2[] = { 0x00,0x00,0x00,0x01,0x28,0xce,0x3c };

	int vcl, update;
	uint8_t buffer[128];
	struct mpeg4_avc_t avc;
	memset(&avc, 0, sizeof(avc));

	h264_annexbtomp4(&avc, sps, sizeof(sps), buffer, sizeof(buffer), &vcl, &update);
	assert(0 == vcl && 1 == update);
	h264_annexbtomp4(&avc, pps, sizeof(pps), buffer, sizeof(buffer), &vcl, &update);
	assert(0 == vcl && 1 == update && 1 == avc.nb_sps && avc.sps[0].bytes == sizeof(sps)-4 && 0 == memcmp(avc.sps[0].data, sps+4, sizeof(sps) - 4) && 1 == avc.nb_pps && avc.pps[0].bytes == sizeof(pps) - 4 && 0 == memcmp(avc.pps[0].data, pps+4, sizeof(pps) - 4));

	h264_annexbtomp4(&avc, sps1, sizeof(sps1), buffer, sizeof(buffer), &vcl, &update);
	assert(0 == vcl && 1 == update && 2 == avc.nb_sps && avc.sps[0].bytes == sizeof(sps) - 4 && avc.sps[1].bytes == sizeof(sps1) - 4 && 0 == memcmp(avc.sps[0].data, sps+4, sizeof(sps) - 4) && 0 == memcmp(avc.sps[1].data, sps1 + 4, sizeof(sps1) - 4) && 1 == avc.nb_pps && avc.pps[0].bytes == sizeof(pps) - 4 && 0 == memcmp(avc.pps[0].data, pps + 4, sizeof(pps) - 4));
	
	h264_annexbtomp4(&avc, pps1, sizeof(pps1), buffer, sizeof(buffer), &vcl, &update);
	assert(0 == vcl && 1 == update && 2 == avc.nb_sps && avc.sps[0].bytes == sizeof(sps) - 4 && avc.sps[1].bytes == sizeof(sps1) - 4 && 0 == memcmp(avc.sps[0].data, sps + 4, sizeof(sps) - 4) && 0 == memcmp(avc.sps[1].data, sps1 + 4, sizeof(sps1) - 4) && 1 == avc.nb_pps && avc.pps[0].bytes == sizeof(pps1) - 4 && 0 == memcmp(avc.pps[0].data, pps1 + 4, sizeof(pps1) - 4));
	
	h264_annexbtomp4(&avc, sps2, sizeof(sps2), buffer, sizeof(buffer), &vcl, &update);
	assert(0 == vcl && 1 == update && 2 == avc.nb_sps && avc.sps[0].bytes == sizeof(sps2) - 4 && avc.sps[1].bytes == sizeof(sps1) - 4 && 0 == memcmp(avc.sps[0].data, sps2 + 4, sizeof(sps2) - 4) && 0 == memcmp(avc.sps[1].data, sps1 + 4, sizeof(sps1) - 4) && 1 == avc.nb_pps && avc.pps[0].bytes == sizeof(pps1) - 4 && 0 == memcmp(avc.pps[0].data, pps1 + 4, sizeof(pps1) - 4));
	
	h264_annexbtomp4(&avc, pps2, sizeof(pps2), buffer, sizeof(buffer), &vcl, &update);
	assert(0 == vcl && 1 == update && 2 == avc.nb_sps && avc.sps[0].bytes == sizeof(sps2) - 4 && avc.sps[1].bytes == sizeof(sps1) - 4 && 0 == memcmp(avc.sps[0].data, sps2 + 4, sizeof(sps2) - 4) && 0 == memcmp(avc.sps[1].data, sps1 + 4, sizeof(sps1) - 4) && 1 == avc.nb_pps && avc.pps[0].bytes == sizeof(pps2) - 4 && 0 == memcmp(avc.pps[0].data, pps2 + 4, sizeof(pps2) - 4));
}

void mpeg4_annexbtomp4_test(void)
{
	const uint8_t sps[] = { 0x67,0x42,0xe0,0x1e,0xab };
	const uint8_t pps[] = { 0x28,0xce,0x3c,0x80 };
	const uint8_t annexb[] = { 0x00,0x00,0x00,0x01,0x67,0x42,0xe0,0x1e,0xab, 0x00,0x00,0x00,0x01,0x28,0xce,0x3c,0x80,0x00,0x00,0x00,0x01,0x65,0x11 };
	uint8_t output[256];
	int vcl, update;

	struct mpeg4_avc_t avc;
	memset(&avc, 0, sizeof(avc));
	assert(h264_annexbtomp4(&avc, annexb, sizeof(annexb), output, sizeof(output), &vcl, &update) > 0);
	assert(1 == avc.nb_sps && avc.sps[0].bytes == sizeof(sps) && 0 == memcmp(avc.sps[0].data, sps, sizeof(sps)));
	assert(1 == avc.nb_pps && avc.pps[0].bytes == sizeof(pps) && 0 == memcmp(avc.pps[0].data, pps, sizeof(pps)));
	assert(vcl == 1);

	mpeg4_annexbtomp4_test2();
	mpeg4_h264_bitstream_format_test();
}
#endif

#include "mp3-header.h"
#include <stdint.h>
#include <string.h>
#include <assert.h>

// layer-1, layer-2, layer-3
static int s_bitrate_mpeg1[3][16] = {
	{ 0/*free*/, 32000, 64000, 96000, 128000, 160000, 192000, 224000, 256000, 288000, 320000, 352000, 384000, 416000, 448000, -1 },
	{ 0/*free*/, 32000, 48000, 56000, 64000, 80000, 96000, 112000, 128000, 160000, 192000, 224000, 256000, 320000, 384000, -1 },
	{ 0/*free*/, 32000, 40000, 48000, 56000, 64000, 80000, 96000, 112000, 128000, 160000, 192000, 224000, 256000, 320000, -1 },
};

// layer-1, layer-2, layer-3
static int s_bitrate_mpeg2[3][16] = {
	{ 0/*free*/, 32000, 48000, 56000, 64000, 80000, 96000, 112000, 128000, 144000, 160000, 176000, 192000, 224000, 256000, -1 },
	{ 0/*free*/, 8000, 16000, 24000, 32000, 40000, 48000, 56000, 64000, 80000, 96000, 112000, 128000, 144000, 160000, -1 },
	{ 0/*free*/, 8000, 16000, 24000, 32000, 40000, 48000, 56000, 64000, 80000, 96000, 112000, 128000, 144000, 160000, -1 },
};

// layer-1, layer-2, layer-3
static int s_frequency_mpeg1[4] = { 44100, 48000, 32000, -1 };
static int s_frequency_mpeg2[4] = { 22050, 24000, 16000, -1 };
static int s_frequency_mpeg25[4] = { 11025, 12000, 8000, -1 };

// layer-1, layer-2, layer-3
//static int s_frames_mpeg1[3] = { 384, 1152, 1152 };
//static int s_frames_mpeg2[3] = { 384, 1152, 576 };
//static int s_frames_mpeg25[3] = { 384, 1152, 576 };

// layer-1 bytes = ((frames / 8 * bitrate) / frequency + padding * 4
// layer-2/3 bytes = ((frames / 8 * bitrate) / frequency + padding

int mp3_header_load(struct mp3_header_t* mp3, const void* data, int bytes)
{
	const uint8_t* p;
	if (bytes < 4)
		return 0;

	p = data;
	if (0 == memcmp("TAG", p, 3))
	{
		if (bytes < 128/*ID3v1*/ + 4)
			return 0;
		p += 128;
	}
	else if (0 == memcmp("ID3", p, 3))
	{
		uint32_t n;
		if (3 != p[3]/*version*/ || bytes < 10)
			return 0;
		n = (((uint32_t)p[6] & 0x7F) << 21) | (((uint32_t)p[7] & 0x7F) << 14) | (((uint32_t)p[8] & 0x7F) << 7) | (p[9] & 0x7F);
		if (bytes < (int)n + 10)
			return 0;
		p += n + 10;
	}

	//sync: 1111 1111 111
	if (0xFF != p[0] || 0xE0 != (p[1] & 0xE0))
	{
		assert(0);
		return 0;
	}

	mp3->version = (p[1] >> 3) & 0x03;
	mp3->layer = (p[1] >> 1) & 0x03;
	mp3->protection = p[1] & 0x01;
	mp3->bitrate_index = (p[2] >> 4) & 0x0F;
	mp3->sampling_frequency = (p[2] >> 2) & 0x03;
	mp3->priviate = p[2] & 0x01;
	mp3->mode = (p[3] >> 6) & 0x03;
	mp3->mode_extension = (p[3] >> 4) & 0x03;
	mp3->copyright = (p[3] >> 3) & 0x01;
	mp3->original = (p[3] >> 2) & 0x01;
	mp3->emphasis = p[3] & 0x03;

	return (int)(p - (uint8_t*)data) + 4;
}

int mp3_header_save(const struct mp3_header_t* mp3, void* data, int bytes)
{
	uint8_t* p;
	if (bytes < 4)
		return 0;

	p = data;
	p[0] = 0xFF;
	p[1] = (uint8_t)(0xE0 | (mp3->version << 3) | (mp3->layer << 1) | mp3->protection);
	p[2] = (uint8_t)((mp3->bitrate_index << 4) | (mp3->sampling_frequency << 2) | 0x00 /*padding*/ | mp3->priviate);
	p[3] = (uint8_t)((mp3->mode << 6) | (mp3->mode_extension << 4) | (mp3->copyright << 3) | (mp3->original << 2) | mp3->emphasis);
	return 4;
}

int mp3_get_channel(const struct mp3_header_t* mp3)
{
    return 0x03 == mp3->mode ? 1 : 2;
}

int mp3_get_bitrate(const struct mp3_header_t* mp3)
{
	if (mp3->layer < 1 || mp3->layer > 3)
	{
		assert(0);
		return -1;
	}

	switch (mp3->version)
	{
	case MP3_MPEG1:
		return s_bitrate_mpeg1[3 - mp3->layer][mp3->bitrate_index];

	case MP3_MPEG2:
	case MP3_MPEG2_5:
		return s_bitrate_mpeg2[3 - mp3->layer][mp3->bitrate_index];

	default:
		assert(0);
		return -1;
	}
}

static int mp3_find_bitrate(const int* arr, int bitrate)
{
	int i;
	for (i = 0; i < 16; i++)
	{
		if (bitrate == arr[i])
			return i;
	}
	return -1;
}

int mp3_set_bitrate(struct mp3_header_t* mp3, int bitrate)
{
	int r;
	if (mp3->layer < 1 || mp3->layer > 3)
	{
		assert(0);
		return -1;
	}

	switch (mp3->version)
	{
	case MP3_MPEG1:
		r = mp3_find_bitrate(s_bitrate_mpeg1[3 - mp3->layer], bitrate);
		break;

	case MP3_MPEG2:
	case MP3_MPEG2_5:
		r = mp3_find_bitrate(s_bitrate_mpeg2[3 - mp3->layer], bitrate);
		break;

	default:
		assert(0);
		r = -1;
	}

	if (-1 == r)
		return -1;

	mp3->bitrate_index = (unsigned int)r;
	return 0;
}

int mp3_get_frequency(const struct mp3_header_t* mp3)
{
	if (mp3->sampling_frequency < 0 || mp3->sampling_frequency > 3)
		return -1;

	switch (mp3->version)
	{
	case MP3_MPEG1: return s_frequency_mpeg1[mp3->sampling_frequency];
	case MP3_MPEG2: return s_frequency_mpeg2[mp3->sampling_frequency];
	case MP3_MPEG2_5: return s_frequency_mpeg25[mp3->sampling_frequency];
	default: assert(0); return -1;
	}
}

static int mp3_find_frequency(const int* arr, int frequency)
{
	int i;
	for (i = 0; i < 4; i++)
	{
		if (frequency == arr[i])
			return i;
	}
	return -1;
}

int mp3_set_frequency(struct mp3_header_t* mp3, int frequency)
{
	int r;
	switch (mp3->version)
	{
	case MP3_MPEG1: 
		r = mp3_find_frequency(s_frequency_mpeg1, frequency);
		break;

	case MP3_MPEG2: 
		r = mp3_find_frequency(s_frequency_mpeg2, frequency);
		break;

	case MP3_MPEG2_5:
		r = mp3_find_frequency(s_frequency_mpeg25, frequency);
		break;

	default: 
		assert(0);
		r = -1;
	}

	if (-1 == r)
		return -1;

	mp3->sampling_frequency = (unsigned int)r;
	return 0;
}

#if defined(DEBUG) || defined(_DEBUG)
void mp3_header_test(void)
{
	uint8_t v[4] = { 0xff, 0xfb, 0xe0, 0x64 };
	uint8_t v2[4];
	struct mp3_header_t mp3;

	assert(4 == mp3_header_load(&mp3, v, 4));
	assert(MP3_MPEG1 == mp3.version && MP3_LAYER3 == mp3.layer);
	assert(14 == mp3.bitrate_index && 320000 == mp3_get_bitrate(&mp3));
	assert(0 == mp3.sampling_frequency && 44100 == mp3_get_frequency(&mp3));
	assert(1 == mp3.mode && 1 == mp3.protection);
	assert(4 == mp3_header_save(&mp3, v2, 4));
	assert(0 ==  memcmp(v, v2, 4));
}
#endif

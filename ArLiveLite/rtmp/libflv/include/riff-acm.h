#ifndef _riff_acm_h_
#define _riff_acm_h_

#include <stdint.h>
#include <stddef.h>

#ifndef WAVE_FORMAT_PCM
#define WAVE_FORMAT_PCM		1
#define WAVE_FORMAT_ADPCM	2
#define WAVE_FORMAT_ALAW	6
#define WAVE_FORMAT_MULAW	7
#endif

#if defined(__cplusplus)
extern "C" {
#endif

#pragma pack(push)
#pragma pack(1)
struct wave_format_t
{
	uint16_t wFormatTag;
	uint16_t nChannels;
	uint32_t nSamplesPerSec;
	uint32_t nAvgBytesPerSec;
	uint16_t nBlockAlign;
	uint16_t wBitsPerSample;
	uint16_t cbSize;

	// WAVEFORMATEXTENSIBLE(only cbSize > 0)
	uint16_t Samples;
	uint32_t dwChannelMask;
	uint8_t  SubFormat[16];
};
#pragma pack(pop)

int wave_format_load(const uint8_t* data, int bytes, struct wave_format_t* wav);
int wave_format_save(const struct wave_format_t* wav, uint8_t* data, int bytes);

#if defined(__cplusplus)
}
#endif
#endif /* !_riff_acm_h_ */

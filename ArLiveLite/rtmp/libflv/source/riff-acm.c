#include "riff-acm.h"

int wave_format_load(const uint8_t* data, int bytes,  struct wave_format_t* wav)
{
	if (bytes < 18)
		return -1;

	// little endian
	wav->wFormatTag = data[0] | ((uint16_t)data[1] << 8);
	wav->nChannels = data[2] | ((uint16_t)data[3] << 8);
	wav->nSamplesPerSec = ((uint32_t)data[4] << 0) | ((uint32_t)data[5] << 8) | ((uint32_t)data[6] << 16) | ((uint32_t)data[7] << 24);
	wav->nAvgBytesPerSec = ((uint32_t)data[8] << 0) | ((uint32_t)data[9] << 8) | ((uint32_t)data[10] << 16) | ((uint32_t)data[11] << 24);
	wav->nBlockAlign = data[12] | ((uint16_t)data[13] << 8);
	wav->wBitsPerSample = data[14] | ((uint16_t)data[15] << 8);
	wav->cbSize = data[16] | ((uint16_t)data[17] << 8);

	if (18 + wav->cbSize > bytes)
		return -1;

	return 18 + wav->cbSize;
}

int wave_format_save(const struct wave_format_t* wav, uint8_t* data, int bytes)
{
	if (bytes < wav->cbSize)
		return -1;

	// little endian
	data[0] = (uint8_t)wav->wFormatTag;
	data[1] = (uint8_t)(wav->wFormatTag >> 8);
	data[2] = (uint8_t)wav->nChannels;
	data[3] = (uint8_t)(wav->nChannels >> 8);
	data[4] = (uint8_t)wav->nSamplesPerSec; 
	data[5] = (uint8_t)(wav->nSamplesPerSec >> 8);
	data[6] = (uint8_t)(wav->nSamplesPerSec >> 16);
	data[7] = (uint8_t)(wav->nSamplesPerSec >> 24);
	data[8] = (uint8_t)wav->nAvgBytesPerSec;
	data[9] = (uint8_t)(wav->nAvgBytesPerSec >> 8);
	data[10] = (uint8_t)(wav->nAvgBytesPerSec >> 16);
	data[11] = (uint8_t)(wav->nAvgBytesPerSec >> 24);
	data[12] = (uint8_t)wav->nBlockAlign;
	data[13] = (uint8_t)(wav->nBlockAlign >> 8);
	data[14] = (uint8_t)wav->wBitsPerSample;
	data[15] = (uint8_t)(wav->wBitsPerSample >> 8);
	data[16] = (uint8_t)wav->cbSize;
	data[17] = (uint8_t)(wav->cbSize >> 8);

	//if(wav->cbSize > 0)
	//	memcpy(data + 18, wav->extra, wav->cbSize);
	return wav->cbSize + 18;
}

/*
*  Copyright (c) 2016 The AnyRTC project authors. All Rights Reserved.
*
*  Please visit https://www.anyrtc.io for detail.
*
* The GNU General Public License is a free, copyleft license for
* software and other kinds of works.
*
* The licenses for most software and other practical works are designed
* to take away your freedom to share and change the works.  By contrast,
* the GNU General Public License is intended to guarantee your freedom to
* share and change all versions of a program--to make sure it remains free
* software for all its users.  We, the Free Software Foundation, use the
* GNU General Public License for most of our software; it applies also to
* any other work released this way by its authors.  You can apply it to
* your programs, too.
* See the GNU LICENSE file for more info.
*/
#ifndef __PLUGIN_AAC_H__
#define __PLUGIN_AAC_H__

#include "pluginaac_export.h"

// the AAC Encoder handler.
typedef void* aac_enc_t;
// @AnyRTC Interface
PLUGIN_AAC_API aac_enc_t aac_encoder_open(unsigned char ucAudioChannel, unsigned int u32AudioSamplerate, unsigned int u32PCMBitSize, bool mp4);
PLUGIN_AAC_API void aac_encoder_close(void*pHandle);
PLUGIN_AAC_API int aac_encoder_encode_frame(void*pHandle, unsigned char* inbuf, unsigned int inlen, unsigned char* outbuf, unsigned int* outlen);

// the AAC Decoder handler.
typedef void* aac_dec_t;
// @AnyRTC Interface
PLUGIN_AAC_API aac_dec_t aac_decoder_open(unsigned char* adts, unsigned int len, unsigned char* outChannels, unsigned int* outSampleHz);
PLUGIN_AAC_API void aac_decoder_close(void*pHandle);
PLUGIN_AAC_API int aac_decoder_decode_frame(void*pHandle, unsigned char* inbuf, unsigned int inlen, unsigned char* outbuf, unsigned int* outlen);

#endif	// __PLUGIN_AAC_H__
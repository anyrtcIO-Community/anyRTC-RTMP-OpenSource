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
#include "pluginaac.h"
#include <stdio.h>
#include <string.h>
#include "third_party/faad2-2.7/include/faad.h"

aac_dec_t aac_decoder_open(unsigned char* adts, unsigned int len, unsigned char* outChannels, unsigned int* outSampleHz)
{
	unsigned long samplerate;
	unsigned char channels;
	NeAACDecHandle decoder = NULL;
	
	//open decoder  
	decoder = NeAACDecOpen();
	//initialize decoder  
	if (NeAACDecInit(decoder, adts, len, &samplerate, &channels) < 0)
	{
		NeAACDecClose(decoder);
		decoder = NULL;
	}
	*outChannels = channels;
	*outSampleHz = samplerate;
	return decoder;
}
void aac_decoder_close(void*pHandle)
{
	if (pHandle != NULL) {
		NeAACDecClose(pHandle);
		pHandle = NULL;
	}
}
int aac_decoder_decode_frame(void*pHandle, unsigned char* inbuf, unsigned int inlen, unsigned char* outbuf, unsigned int* outlen)
{
	NeAACDecFrameInfo frame_info;
	unsigned char* pcm_data = NULL;
	if (pHandle != NULL) {
		//decode ADTS frame  
		pcm_data = (unsigned char*)NeAACDecDecode(pHandle, &frame_info, inbuf, inlen);

		if (frame_info.error > 0)
		{
			printf("%s\n", NeAACDecGetErrorMessage(frame_info.error));
			return 0;
		}
		else if (pcm_data && frame_info.samples > 0)
		{
			*outlen = frame_info.samples * frame_info.channels;
			memcpy(outbuf, pcm_data, frame_info.samples * frame_info.channels);
		}
	}
	return *outlen;
}
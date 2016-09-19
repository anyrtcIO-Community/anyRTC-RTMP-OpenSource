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
extern "C"
{
#include <faac.h>
}
#define DEFAULT_TNS     0
typedef struct tagAacENC
{
	tagAacENC(void)
		: hEncoder(NULL)
		, pOutput(NULL)
		, pPCM(NULL)
		, nInputSamples(0)
		, nMaxOutputBytes(0)
		, nPcmSize(0)
		, nPcmALen(0)
	{
	}

	faacEncHandle	hEncoder;

	int				nInputSamples;
	unsigned char*	pOutput;
	int				nMaxOutputBytes;
	unsigned char*	pPCM;
	int				nPcmSize;
	int				nPcmALen;

}AacENC, *PAacENC;

void*aac_encoder_open(unsigned char ucAudioChannel, unsigned int u32AudioSamplerate, unsigned int u32PCMBitSize, bool mp4)
{
	unsigned int objectType = LOW;
	unsigned int mpegVersion = MPEG4;// mp4 ? MPEG4 : MPEG2;
	unsigned long nInputSamples = 0;
	unsigned long nMaxOutputBytes = 0;

	AacENC* pEnc = new AacENC();
	pEnc->hEncoder = faacEncOpen(u32AudioSamplerate, ucAudioChannel, &nInputSamples, &nMaxOutputBytes);

	pEnc->nInputSamples = nInputSamples;
	pEnc->nPcmSize = nInputSamples * u32PCMBitSize / 8;
	if (pEnc->nPcmSize > 0) {
		pEnc->pPCM = new unsigned char[pEnc->nPcmSize];
	}
	pEnc->nMaxOutputBytes = nMaxOutputBytes;
	if (pEnc->nMaxOutputBytes > 0) {
		pEnc->pOutput = new unsigned char[pEnc->nMaxOutputBytes];
	}
	/*get current encoding configuration*/
	faacEncConfigurationPtr pConfiguration = faacEncGetCurrentConfiguration(pEnc->hEncoder);
	pConfiguration->inputFormat = FAAC_INPUT_16BIT;

	/*0 - raw; 1 - ADTS*/
	pConfiguration->outputFormat = mp4?0:1;
	pConfiguration->useTns = DEFAULT_TNS;
	pConfiguration->aacObjectType = objectType;
	pConfiguration->mpegVersion = mpegVersion;
	pConfiguration->bitRate = 64;

	/*set encoding configuretion*/
	faacEncSetConfiguration(pEnc->hEncoder, pConfiguration);
	return pEnc;
}

void aac_encoder_close(void*pHandle)
{
	if (pHandle) {
		/*Close FAAC engine*/
		AacENC* pEnc = (AacENC*)pHandle;
		faacEncClose(pEnc->hEncoder);
		if (pEnc->pPCM) {
			delete[] pEnc->pPCM;
			pEnc->pPCM = NULL;
		}
		if (pEnc->pOutput) {
			delete[] pEnc->pOutput;
			pEnc->pOutput = NULL;
		}
		delete pEnc;
		pHandle = NULL;
	}
}

int aac_encoder_encode_frame(void*pHandle, unsigned char* inbuf, unsigned int inlen, unsigned char* outbuf, unsigned int* outlen)
{
	int ret = 0;
	if (pHandle != NULL) {
		AacENC* pEnc = (AacENC*)pHandle;
		if (pEnc->nPcmALen + inlen < pEnc->nPcmSize){
			memcpy(pEnc->pPCM + pEnc->nPcmALen, inbuf, inlen);
			pEnc->nPcmALen += inlen;
			return 0;
		}
		else{
			ret = pEnc->nPcmALen;
			memcpy(pEnc->pPCM + pEnc->nPcmALen, inbuf, pEnc->nPcmSize - pEnc->nPcmALen);
			int nRet = faacEncEncode(pEnc->hEncoder, (int*)pEnc->pPCM, pEnc->nInputSamples, pEnc->pOutput, pEnc->nMaxOutputBytes);
			if (nRet > 0) {
				memcpy(outbuf , pEnc->pOutput, nRet);
				*outlen = (unsigned int)nRet;
			}
			memcpy(pEnc->pPCM, inbuf + (pEnc->nPcmSize - pEnc->nPcmALen), inlen - (pEnc->nPcmSize - pEnc->nPcmALen));
			pEnc->nPcmALen = inlen - (pEnc->nPcmSize - pEnc->nPcmALen);
		}
	}
	return ret;
}
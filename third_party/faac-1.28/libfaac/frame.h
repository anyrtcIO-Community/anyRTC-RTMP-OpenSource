/*
 * FAAC - Freeware Advanced Audio Coder
 * Copyright (C) 2001 Menno Bakker
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.

 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 *
 * $Id: frame.h,v 1.30 2009/01/25 18:50:32 menno Exp $
 */

#ifndef FRAME_H
#define FRAME_H

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#ifdef HAVE_SYS_TYPES_H
# include <sys/types.h>
#endif
#ifdef HAVE_INTTYPES_H
# include <inttypes.h>
#endif
#ifdef HAVE_STDINT_H
# include <stdint.h>
#endif

#ifndef HAVE_INT32_T
typedef signed int int32_t;
#endif

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#include "coder.h"
#include "channels.h"
#include "psych.h"
#include "aacquant.h"
#include "fft.h"

#if defined(_WIN32) && !defined(__MINGW32__)
  #ifndef FAACAPI
    #define FAACAPI __stdcall
  #endif
#else
  #ifndef FAACAPI
    #define FAACAPI
  #endif
#endif

#pragma pack(push, 1)

typedef struct {
  psymodel_t *model;
  char *name;
} psymodellist_t;

#include <faaccfg.h>

typedef struct {
    /* number of channels in AAC file */
    unsigned int numChannels;

    /* samplerate of AAC file */
    unsigned long sampleRate;
    unsigned int sampleRateIdx;

    unsigned int usedBytes;

    /* frame number */
    unsigned int frameNum;
    unsigned int flushFrame;

    /* Scalefactorband data */
    SR_INFO *srInfo;

    /* sample buffers of current next and next next frame*/
    double *sampleBuff[MAX_CHANNELS];
    double *nextSampleBuff[MAX_CHANNELS];
    double *next2SampleBuff[MAX_CHANNELS];
    double *next3SampleBuff[MAX_CHANNELS];
    double *ltpTimeBuff[MAX_CHANNELS];

    /* Filterbank buffers */
    double *sin_window_long;
    double *sin_window_short;
    double *kbd_window_long;
    double *kbd_window_short;
    double *freqBuff[MAX_CHANNELS];
    double *overlapBuff[MAX_CHANNELS];

    double *msSpectrum[MAX_CHANNELS];

    /* Channel and Coder data for all channels */
    CoderInfo coderInfo[MAX_CHANNELS];
    ChannelInfo channelInfo[MAX_CHANNELS];

    /* Psychoacoustics data */
    PsyInfo psyInfo[MAX_CHANNELS];
    GlobalPsyInfo gpsyInfo;

    /* Configuration data */
    faacEncConfiguration config;

    psymodel_t *psymodel;

    /* quantizer specific config */
    AACQuantCfg aacquantCfg;

	/* FFT Tables */
	FFT_Tables	fft_tables;

    /* output bits difference in average bitrate mode */
    int bitDiff;
} faacEncStruct, *faacEncHandle;

int FAACAPI faacEncGetVersion(char **faac_id_string,
			      char **faac_copyright_string);

int FAACAPI faacEncGetDecoderSpecificInfo(faacEncHandle hEncoder,
                                          unsigned char** ppBuffer,
                                          unsigned long* pSizeOfDecoderSpecificInfo);

faacEncConfigurationPtr FAACAPI faacEncGetCurrentConfiguration(faacEncHandle hEncoder);
int FAACAPI faacEncSetConfiguration (faacEncHandle hEncoder, faacEncConfigurationPtr config);

faacEncHandle FAACAPI faacEncOpen(unsigned long sampleRate,
                                  unsigned int numChannels,
                                  unsigned long *inputSamples,
                                  unsigned long *maxOutputBytes);

int FAACAPI faacEncEncode(faacEncHandle hEncoder,
                          int32_t *inputBuffer,
                          unsigned int samplesInput,
                          unsigned char *outputBuffer,
                          unsigned int bufferSize
                          );

int FAACAPI faacEncClose(faacEncHandle hEncoder);


#pragma pack(pop)

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* FRAME_H */

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
 * $Id: frame.c,v 1.67 2004/11/17 14:26:06 menno Exp $
 */

/*
 * CHANGES:
 *  2001/01/17: menno: Added frequency cut off filter.
 *  2001/02/28: menno: Added Temporal Noise Shaping.
 *  2001/03/05: menno: Added Long Term Prediction.
 *  2001/05/01: menno: Added backward prediction.
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <math.h>

#include "frame.h"
#include "coder.h"
#include "midside.h"
#include "channels.h"
#include "bitstream.h"
#include "filtbank.h"
#include "aacquant.h"
#include "util.h"
#include "huffman.h"
#include "psych.h"
#include "tns.h"
#include "ltp.h"
#include "backpred.h"
#include "version.h"

#if FAAC_RELEASE
static char *libfaacName = FAAC_VERSION;
#else
static char *libfaacName = FAAC_VERSION ".1 (" __DATE__ ") UNSTABLE";
#endif
static char *libCopyright =
  "FAAC - Freeware Advanced Audio Coder (http://www.audiocoding.com/)\n"
  " Copyright (C) 1999,2000,2001  Menno Bakker\n"
  " Copyright (C) 2002,2003  Krzysztof Nikiel\n"
  "This software is based on the ISO MPEG-4 reference source code.\n";

static const psymodellist_t psymodellist[] = {
  {&psymodel2, "knipsycho psychoacoustic"},
  {NULL}
};

static SR_INFO srInfo[12+1];

// base bandwidth for q=100
static const int bwbase = 16000;
// bandwidth multiplier (for quantiser)
static const int bwmult = 120;
// max bandwidth/samplerate ratio
static const double bwfac = 0.45;


int FAACAPI faacEncGetVersion( char **faac_id_string,
			      				char **faac_copyright_string)
{
  if (faac_id_string)
    *faac_id_string = libfaacName;

  if (faac_copyright_string)
    *faac_copyright_string = libCopyright;

  return FAAC_CFG_VERSION;
}


int FAACAPI faacEncGetDecoderSpecificInfo(faacEncHandle hEncoder,unsigned char** ppBuffer,unsigned long* pSizeOfDecoderSpecificInfo)
{
    BitStream* pBitStream = NULL;

    if((hEncoder == NULL) || (ppBuffer == NULL) || (pSizeOfDecoderSpecificInfo == NULL)) {
        return -1;
    }

    if(hEncoder->config.mpegVersion == MPEG2){
        return -2; /* not supported */
    }

    *pSizeOfDecoderSpecificInfo = 2;
    *ppBuffer = malloc(2);

    if(*ppBuffer != NULL){

        memset(*ppBuffer,0,*pSizeOfDecoderSpecificInfo);
        pBitStream = OpenBitStream(*pSizeOfDecoderSpecificInfo, *ppBuffer);
        PutBit(pBitStream, hEncoder->config.aacObjectType, 5);
        PutBit(pBitStream, hEncoder->sampleRateIdx, 4);
        PutBit(pBitStream, hEncoder->numChannels, 4);
        CloseBitStream(pBitStream);

        return 0;
    } else {
        return -3;
    }
}


faacEncConfigurationPtr FAACAPI faacEncGetCurrentConfiguration(faacEncHandle hEncoder)
{
    faacEncConfigurationPtr config = &(hEncoder->config);

    return config;
}

int FAACAPI faacEncSetConfiguration(faacEncHandle hEncoder,
                                    faacEncConfigurationPtr config)
{
	int i;

    hEncoder->config.allowMidside = config->allowMidside;
    hEncoder->config.useLfe = config->useLfe;
    hEncoder->config.useTns = config->useTns;
    hEncoder->config.aacObjectType = config->aacObjectType;
    hEncoder->config.mpegVersion = config->mpegVersion;
    hEncoder->config.outputFormat = config->outputFormat;
    hEncoder->config.inputFormat = config->inputFormat;
    hEncoder->config.shortctl = config->shortctl;

    assert((hEncoder->config.outputFormat == 0) || (hEncoder->config.outputFormat == 1));

    switch( hEncoder->config.inputFormat )
    {
        case FAAC_INPUT_16BIT:
        //case FAAC_INPUT_24BIT:
        case FAAC_INPUT_32BIT:
        case FAAC_INPUT_FLOAT:
            break;

        default:
            return 0;
            break;
    }

    /* No SSR supported for now */
    if (hEncoder->config.aacObjectType == SSR)
        return 0;

    /* LTP only with MPEG4 */
    if ((hEncoder->config.aacObjectType == LTP) && (hEncoder->config.mpegVersion != MPEG4))
        return 0;

    /* Re-init TNS for new profile */
    TnsInit(hEncoder);

    /* Check for correct bitrate */
    if (config->bitRate > MaxBitrate(hEncoder->sampleRate))
		return 0;
#if 0
    if (config->bitRate < MinBitrate())
        return 0;
#endif

    if (config->bitRate && !config->bandWidth)
    {	
		static struct {
			int rate; // per channel at 44100 sampling frequency
			int cutoff;
		}	rates[] = {
#ifdef DRM
            /* DRM uses low bit-rates. We've chosen higher bandwidth values and
               decrease the quantizer quality at the same time to preserve the
               low bit-rate */
            {4500,  1200},
            {9180,  2500},
            {11640, 3000},
            {14500, 4000},
            {17460, 5500},
            {20960, 6250},
            {40000, 12000},
#else
			{29500, 5000},
			{37500, 7000},
			{47000, 10000},
			{64000, 16000},
			{76000, 20000},
#endif
			{0, 0}
		};

		int f0, f1;
		int r0, r1;

#ifdef DRM
        double tmpbitRate = (double)config->bitRate;
#else
        double tmpbitRate = (double)config->bitRate * 44100 / hEncoder->sampleRate;
#endif

        config->quantqual = 100;

		f0 = f1 = rates[0].cutoff;
		r0 = r1 = rates[0].rate;
		
		for (i = 0; rates[i].rate; i++)
		{
			f0 = f1;
			f1 = rates[i].cutoff;
			r0 = r1;
			r1 = rates[i].rate;
			if (rates[i].rate >= tmpbitRate)
				break;
		}

        if (tmpbitRate > r1)
            tmpbitRate = r1;
        if (tmpbitRate < r0)
            tmpbitRate = r0;

		if (f1 > f0)
            config->bandWidth =
                    pow((double)tmpbitRate / r1,
                    log((double)f1 / f0) / log ((double)r1 / r0)) * (double)f1;
		else
			config->bandWidth = f1;

#ifndef DRM
		config->bandWidth =
				(double)config->bandWidth * hEncoder->sampleRate / 44100;
		config->bitRate = tmpbitRate * hEncoder->sampleRate / 44100;
#endif

		if (config->bandWidth > bwbase)
		  config->bandWidth = bwbase;
	}

    hEncoder->config.bitRate = config->bitRate;

    if (!config->bandWidth)
    {
        config->bandWidth = (config->quantqual - 100) * bwmult + bwbase;
    }

    hEncoder->config.bandWidth = config->bandWidth;

    // check bandwidth
    if (hEncoder->config.bandWidth < 100)
		hEncoder->config.bandWidth = 100;
    if (hEncoder->config.bandWidth > (hEncoder->sampleRate / 2))
		hEncoder->config.bandWidth = hEncoder->sampleRate / 2;

    if (config->quantqual > 500)
		config->quantqual = 500;
    if (config->quantqual < 10)
		config->quantqual = 10;

    hEncoder->config.quantqual = config->quantqual;

    /* set quantization quality */
    hEncoder->aacquantCfg.quality = config->quantqual;

    // reset psymodel
    hEncoder->psymodel->PsyEnd(&hEncoder->gpsyInfo, hEncoder->psyInfo, hEncoder->numChannels);
    if (config->psymodelidx >= (sizeof(psymodellist) / sizeof(psymodellist[0]) - 1))
		config->psymodelidx = (sizeof(psymodellist) / sizeof(psymodellist[0])) - 2;

    hEncoder->config.psymodelidx = config->psymodelidx;
    hEncoder->psymodel = psymodellist[hEncoder->config.psymodelidx].model;
    hEncoder->psymodel->PsyInit(&hEncoder->gpsyInfo, hEncoder->psyInfo, hEncoder->numChannels,
			hEncoder->sampleRate, hEncoder->srInfo->cb_width_long,
			hEncoder->srInfo->num_cb_long, hEncoder->srInfo->cb_width_short,
			hEncoder->srInfo->num_cb_short);
	
	/* load channel_map */
	for( i = 0; i < 64; i++ )
		hEncoder->config.channel_map[i] = config->channel_map[i];

    /* OK */
    return 1;
}

faacEncHandle FAACAPI faacEncOpen(unsigned long sampleRate,
                                  unsigned int numChannels,
                                  unsigned long *inputSamples,
                                  unsigned long *maxOutputBytes)
{
    unsigned int channel;
    faacEncHandle hEncoder;

    *inputSamples = FRAME_LEN*numChannels;
    *maxOutputBytes = (6144/8)*numChannels;

#ifdef DRM
    *maxOutputBytes += 1; /* for CRC */
#endif

    hEncoder = (faacEncStruct*)AllocMemory(sizeof(faacEncStruct));
    SetMemory(hEncoder, 0, sizeof(faacEncStruct));

    hEncoder->numChannels = numChannels;
    hEncoder->sampleRate = sampleRate;
    hEncoder->sampleRateIdx = GetSRIndex(sampleRate);

    /* Initialize variables to default values */
    hEncoder->frameNum = 0;
    hEncoder->flushFrame = 0;

    /* Default configuration */
    hEncoder->config.version = FAAC_CFG_VERSION;
    hEncoder->config.name = libfaacName;
    hEncoder->config.copyright = libCopyright;
    hEncoder->config.mpegVersion = MPEG4;
    hEncoder->config.aacObjectType = LTP;
    hEncoder->config.allowMidside = 1;
    hEncoder->config.useLfe = 1;
    hEncoder->config.useTns = 0;
    hEncoder->config.bitRate = 0; /* default bitrate / channel */
    hEncoder->config.bandWidth = bwfac * hEncoder->sampleRate;
    if (hEncoder->config.bandWidth > bwbase)
		hEncoder->config.bandWidth = bwbase;
    hEncoder->config.quantqual = 100;
    hEncoder->config.psymodellist = (psymodellist_t *)psymodellist;
    hEncoder->config.psymodelidx = 0;
    hEncoder->psymodel =
      hEncoder->config.psymodellist[hEncoder->config.psymodelidx].model;
    hEncoder->config.shortctl = SHORTCTL_NORMAL;

	/* default channel map is straight-through */
	for( channel = 0; channel < 64; channel++ )
		hEncoder->config.channel_map[channel] = channel;
	
    /*
        by default we have to be compatible with all previous software
        which assumes that we will generate ADTS
        /AV
    */
    hEncoder->config.outputFormat = 1;

    /*
        be compatible with software which assumes 24bit in 32bit PCM
    */
    hEncoder->config.inputFormat = FAAC_INPUT_32BIT;

    /* find correct sampling rate depending parameters */
    hEncoder->srInfo = &srInfo[hEncoder->sampleRateIdx];

    for (channel = 0; channel < numChannels; channel++) 
	{
        hEncoder->coderInfo[channel].prev_window_shape = SINE_WINDOW;
        hEncoder->coderInfo[channel].window_shape = SINE_WINDOW;
        hEncoder->coderInfo[channel].block_type = ONLY_LONG_WINDOW;
        hEncoder->coderInfo[channel].num_window_groups = 1;
        hEncoder->coderInfo[channel].window_group_length[0] = 1;

        /* FIXME: Use sr_idx here */
        hEncoder->coderInfo[channel].max_pred_sfb = GetMaxPredSfb(hEncoder->sampleRateIdx);

        hEncoder->sampleBuff[channel] = NULL;
        hEncoder->nextSampleBuff[channel] = NULL;
        hEncoder->next2SampleBuff[channel] = NULL;
        hEncoder->ltpTimeBuff[channel] = (double*)AllocMemory(2*BLOCK_LEN_LONG*sizeof(double));
        SetMemory(hEncoder->ltpTimeBuff[channel], 0, 2*BLOCK_LEN_LONG*sizeof(double));
    }

    /* Initialize coder functions */
	fft_initialize( &hEncoder->fft_tables );
    
	hEncoder->psymodel->PsyInit(&hEncoder->gpsyInfo, hEncoder->psyInfo, hEncoder->numChannels,
        hEncoder->sampleRate, hEncoder->srInfo->cb_width_long,
        hEncoder->srInfo->num_cb_long, hEncoder->srInfo->cb_width_short,
        hEncoder->srInfo->num_cb_short);

    FilterBankInit(hEncoder);

    TnsInit(hEncoder);

    LtpInit(hEncoder);

    PredInit(hEncoder);

    AACQuantizeInit(hEncoder->coderInfo, hEncoder->numChannels,
		    &(hEncoder->aacquantCfg));

	

    HuffmanInit(hEncoder->coderInfo, hEncoder->numChannels);

    /* Return handle */
    return hEncoder;
}

int FAACAPI faacEncClose(faacEncHandle hEncoder)
{
    unsigned int channel;

    /* Deinitialize coder functions */
    hEncoder->psymodel->PsyEnd(&hEncoder->gpsyInfo, hEncoder->psyInfo, hEncoder->numChannels);

    FilterBankEnd(hEncoder);

    LtpEnd(hEncoder);

    AACQuantizeEnd(hEncoder->coderInfo, hEncoder->numChannels,
			&(hEncoder->aacquantCfg));

    HuffmanEnd(hEncoder->coderInfo, hEncoder->numChannels);

	fft_terminate( &hEncoder->fft_tables );

    /* Free remaining buffer memory */
    for (channel = 0; channel < hEncoder->numChannels; channel++) 
	{
		if (hEncoder->ltpTimeBuff[channel])
			FreeMemory(hEncoder->ltpTimeBuff[channel]);
		if (hEncoder->sampleBuff[channel])
			FreeMemory(hEncoder->sampleBuff[channel]);
		if (hEncoder->nextSampleBuff[channel])
			FreeMemory(hEncoder->nextSampleBuff[channel]);
		if (hEncoder->next2SampleBuff[channel])
			FreeMemory (hEncoder->next2SampleBuff[channel]);
		if (hEncoder->next3SampleBuff[channel])
			FreeMemory (hEncoder->next3SampleBuff[channel]);
    }

    /* Free handle */
    if (hEncoder) 
		FreeMemory(hEncoder);

    return 0;
}

int FAACAPI faacEncEncode(faacEncHandle hEncoder,
                          int32_t *inputBuffer,
                          unsigned int samplesInput,
                          unsigned char *outputBuffer,
                          unsigned int bufferSize
                          )
{
    unsigned int channel, i;
    int sb, frameBytes;
    unsigned int offset;
    BitStream *bitStream; /* bitstream used for writing the frame to */
    TnsInfo *tnsInfo_for_LTP;
    TnsInfo *tnsDecInfo;
#ifdef DRM
    int desbits, diff;
    double fix;
#endif

    /* local copy's of parameters */
    ChannelInfo *channelInfo = hEncoder->channelInfo;
    CoderInfo *coderInfo = hEncoder->coderInfo;
    unsigned int numChannels = hEncoder->numChannels;
    unsigned int sampleRate = hEncoder->sampleRate;
    unsigned int aacObjectType = hEncoder->config.aacObjectType;
    unsigned int mpegVersion = hEncoder->config.mpegVersion;
    unsigned int useLfe = hEncoder->config.useLfe;
    unsigned int useTns = hEncoder->config.useTns;
    unsigned int allowMidside = hEncoder->config.allowMidside;
    unsigned int bandWidth = hEncoder->config.bandWidth;
    unsigned int shortctl = hEncoder->config.shortctl;

    /* Increase frame number */
    hEncoder->frameNum++;

    if (samplesInput == 0)
        hEncoder->flushFrame++;

    /* After 4 flush frames all samples have been encoded,
       return 0 bytes written */
    if (hEncoder->flushFrame > 4)
        return 0;

    /* Determine the channel configuration */
    GetChannelInfo(channelInfo, numChannels, useLfe);

    /* Update current sample buffers */
    for (channel = 0; channel < numChannels; channel++) 
	{
		double *tmp;

        if (hEncoder->sampleBuff[channel]) {
            for(i = 0; i < FRAME_LEN; i++) {
                hEncoder->ltpTimeBuff[channel][i] = hEncoder->sampleBuff[channel][i];
            }
        }
        if (hEncoder->nextSampleBuff[channel]) {
            for(i = 0; i < FRAME_LEN; i++) {
                hEncoder->ltpTimeBuff[channel][FRAME_LEN + i] =
						hEncoder->nextSampleBuff[channel][i];
            }
        }

		if (!hEncoder->sampleBuff[channel])
			hEncoder->sampleBuff[channel] = (double*)AllocMemory(FRAME_LEN*sizeof(double));
		
		tmp = hEncoder->sampleBuff[channel];

        hEncoder->sampleBuff[channel]		= hEncoder->nextSampleBuff[channel];
        hEncoder->nextSampleBuff[channel]	= hEncoder->next2SampleBuff[channel];
        hEncoder->next2SampleBuff[channel]	= hEncoder->next3SampleBuff[channel];
		hEncoder->next3SampleBuff[channel]	= tmp;

        if (samplesInput == 0)
        {
            /* start flushing*/
            for (i = 0; i < FRAME_LEN; i++)
                hEncoder->next3SampleBuff[channel][i] = 0.0;
        }
        else
        {
			int samples_per_channel = samplesInput/numChannels;

            /* handle the various input formats and channel remapping */
            switch( hEncoder->config.inputFormat )
			{
                case FAAC_INPUT_16BIT:
					{
						short *input_channel = (short*)inputBuffer + hEncoder->config.channel_map[channel];

						for (i = 0; i < samples_per_channel; i++)
						{
							hEncoder->next3SampleBuff[channel][i] = (double)*input_channel;
							input_channel += numChannels;
						}
					}
                    break;

                case FAAC_INPUT_32BIT:
					{
						int32_t *input_channel = (int32_t*)inputBuffer + hEncoder->config.channel_map[channel];
						
						for (i = 0; i < samples_per_channel; i++)
						{
							hEncoder->next3SampleBuff[channel][i] = (1.0/256) * (double)*input_channel;
							input_channel += numChannels;
						}
					}
                    break;

                case FAAC_INPUT_FLOAT:
					{
						float *input_channel = (float*)inputBuffer + hEncoder->config.channel_map[channel];

						for (i = 0; i < samples_per_channel; i++)
						{
							hEncoder->next3SampleBuff[channel][i] = (double)*input_channel;
							input_channel += numChannels;
						}
					}
                    break;

                default:
                    return -1; /* invalid input format */
                    break;
            }

            for (i = (int)(samplesInput/numChannels); i < FRAME_LEN; i++)
                hEncoder->next3SampleBuff[channel][i] = 0.0;
		}

		/* Psychoacoustics */
		/* Update buffers and run FFT on new samples */
		/* LFE psychoacoustic can run without it */
		if (!channelInfo[channel].lfe || channelInfo[channel].cpe)
		{
			hEncoder->psymodel->PsyBufferUpdate( 
					&hEncoder->fft_tables, 
					&hEncoder->gpsyInfo, 
					&hEncoder->psyInfo[channel],
					hEncoder->next3SampleBuff[channel], 
					bandWidth,
					hEncoder->srInfo->cb_width_short,
					hEncoder->srInfo->num_cb_short);
		}
    }

    if (hEncoder->frameNum <= 3) /* Still filling up the buffers */
        return 0;

    /* Psychoacoustics */
    hEncoder->psymodel->PsyCalculate(channelInfo, &hEncoder->gpsyInfo, hEncoder->psyInfo,
        hEncoder->srInfo->cb_width_long, hEncoder->srInfo->num_cb_long,
        hEncoder->srInfo->cb_width_short,
        hEncoder->srInfo->num_cb_short, numChannels);

    hEncoder->psymodel->BlockSwitch(coderInfo, hEncoder->psyInfo, numChannels);

    /* force block type */
    if (shortctl == SHORTCTL_NOSHORT)
    {
		for (channel = 0; channel < numChannels; channel++)
		{
			coderInfo[channel].block_type = ONLY_LONG_WINDOW;
		}
    }
    if (shortctl == SHORTCTL_NOLONG)
    {
		for (channel = 0; channel < numChannels; channel++)
		{
			coderInfo[channel].block_type = ONLY_SHORT_WINDOW;
		}
    }

    /* AAC Filterbank, MDCT with overlap and add */
    for (channel = 0; channel < numChannels; channel++) {
        int k;

        FilterBank(hEncoder,
            &coderInfo[channel],
            hEncoder->sampleBuff[channel],
            hEncoder->freqBuff[channel],
            hEncoder->overlapBuff[channel],
            MOVERLAPPED);

        if (coderInfo[channel].block_type == ONLY_SHORT_WINDOW) {
            for (k = 0; k < 8; k++) {
                specFilter(hEncoder->freqBuff[channel]+k*BLOCK_LEN_SHORT,
						sampleRate, bandWidth, BLOCK_LEN_SHORT);
            }
        } else {
            specFilter(hEncoder->freqBuff[channel], sampleRate,
					bandWidth, BLOCK_LEN_LONG);
        }
    }

    /* TMP: Build sfb offset table and other stuff */
    for (channel = 0; channel < numChannels; channel++) {
        channelInfo[channel].msInfo.is_present = 0;

        if (coderInfo[channel].block_type == ONLY_SHORT_WINDOW) {
			coderInfo[channel].max_sfb = hEncoder->srInfo->num_cb_short;
            coderInfo[channel].nr_of_sfb = hEncoder->srInfo->num_cb_short;

            coderInfo[channel].num_window_groups = 1;
            coderInfo[channel].window_group_length[0] = 8;
            coderInfo[channel].window_group_length[1] = 0;
            coderInfo[channel].window_group_length[2] = 0;
            coderInfo[channel].window_group_length[3] = 0;
            coderInfo[channel].window_group_length[4] = 0;
            coderInfo[channel].window_group_length[5] = 0;
            coderInfo[channel].window_group_length[6] = 0;
            coderInfo[channel].window_group_length[7] = 0;

            offset = 0;
            for (sb = 0; sb < coderInfo[channel].nr_of_sfb; sb++) {
                coderInfo[channel].sfb_offset[sb] = offset;
                offset += hEncoder->srInfo->cb_width_short[sb];
            }
            coderInfo[channel].sfb_offset[coderInfo[channel].nr_of_sfb] = offset;
        } else {
            coderInfo[channel].max_sfb = hEncoder->srInfo->num_cb_long;
            coderInfo[channel].nr_of_sfb = hEncoder->srInfo->num_cb_long;

            coderInfo[channel].num_window_groups = 1;
            coderInfo[channel].window_group_length[0] = 1;

            offset = 0;
            for (sb = 0; sb < coderInfo[channel].nr_of_sfb; sb++) {
                coderInfo[channel].sfb_offset[sb] = offset;
                offset += hEncoder->srInfo->cb_width_long[sb];
            }
            coderInfo[channel].sfb_offset[coderInfo[channel].nr_of_sfb] = offset;
        }
    }

    /* Perform TNS analysis and filtering */
    for (channel = 0; channel < numChannels; channel++) {
        if ((!channelInfo[channel].lfe) && (useTns)) {
            TnsEncode(&(coderInfo[channel].tnsInfo),
					coderInfo[channel].max_sfb,
					coderInfo[channel].max_sfb,
					coderInfo[channel].block_type,
					coderInfo[channel].sfb_offset,
					hEncoder->freqBuff[channel]);
        } else {
            coderInfo[channel].tnsInfo.tnsDataPresent = 0;      /* TNS not used for LFE */
        }
    }

    for(channel = 0; channel < numChannels; channel++)
    {
        if((coderInfo[channel].tnsInfo.tnsDataPresent != 0) && (useTns))
            tnsInfo_for_LTP = &(coderInfo[channel].tnsInfo);
        else
            tnsInfo_for_LTP = NULL;

        if(channelInfo[channel].present && (!channelInfo[channel].lfe) &&
            (coderInfo[channel].block_type != ONLY_SHORT_WINDOW) &&
            (mpegVersion == MPEG4) && (aacObjectType == LTP))
        {
            LtpEncode(hEncoder,
					&coderInfo[channel],
					&(coderInfo[channel].ltpInfo),
					tnsInfo_for_LTP,
					hEncoder->freqBuff[channel],
					hEncoder->ltpTimeBuff[channel]);
        } else {
            coderInfo[channel].ltpInfo.global_pred_flag = 0;
        }
    }

    for(channel = 0; channel < numChannels; channel++)
    {
        if ((aacObjectType == MAIN) && (!channelInfo[channel].lfe)) {
            int numPredBands = min(coderInfo[channel].max_pred_sfb, coderInfo[channel].nr_of_sfb);
            PredCalcPrediction(hEncoder->freqBuff[channel],
					coderInfo[channel].requantFreq,
					coderInfo[channel].block_type,
					numPredBands,
					(coderInfo[channel].block_type==ONLY_SHORT_WINDOW)?
					hEncoder->srInfo->cb_width_short:hEncoder->srInfo->cb_width_long,
					coderInfo,
					channelInfo,
					channel);
        } else {
            coderInfo[channel].pred_global_flag = 0;
        }
    }

    for (channel = 0; channel < numChannels; channel++) {
		if (coderInfo[channel].block_type == ONLY_SHORT_WINDOW) {
			SortForGrouping(&coderInfo[channel],
					&hEncoder->psyInfo[channel],
					&channelInfo[channel],
					hEncoder->srInfo->cb_width_short,
					hEncoder->freqBuff[channel]);
		}
		CalcAvgEnrg(&coderInfo[channel], hEncoder->freqBuff[channel]);

      // reduce LFE bandwidth
		if (!channelInfo[channel].cpe && channelInfo[channel].lfe)
		{
			coderInfo[channel].nr_of_sfb = coderInfo[channel].max_sfb = 3;
		}
	}

    MSEncode(coderInfo, channelInfo, hEncoder->freqBuff, numChannels, allowMidside);

    for (channel = 0; channel < numChannels; channel++)
    {
        CalcAvgEnrg(&coderInfo[channel], hEncoder->freqBuff[channel]);
    }

#ifdef DRM
    /* loop the quantization until the desired bit-rate is reached */
    diff = 1; /* to enter while loop */
    hEncoder->aacquantCfg.quality = 120; /* init quality setting */
    while (diff > 0) { /* if too many bits, do it again */
#endif
    /* Quantize and code the signal */
    for (channel = 0; channel < numChannels; channel++) {
        if (coderInfo[channel].block_type == ONLY_SHORT_WINDOW) {
            AACQuantize(&coderInfo[channel], &hEncoder->psyInfo[channel],
					&channelInfo[channel], hEncoder->srInfo->cb_width_short,
					hEncoder->srInfo->num_cb_short, hEncoder->freqBuff[channel],
					&(hEncoder->aacquantCfg));
        } else {
            AACQuantize(&coderInfo[channel], &hEncoder->psyInfo[channel],
					&channelInfo[channel], hEncoder->srInfo->cb_width_long,
					hEncoder->srInfo->num_cb_long, hEncoder->freqBuff[channel],
					&(hEncoder->aacquantCfg));
        }
    }

#ifdef DRM
    /* Write the AAC bitstream */
    bitStream = OpenBitStream(bufferSize, outputBuffer);
    WriteBitstream(hEncoder, coderInfo, channelInfo, bitStream, numChannels);

    /* Close the bitstream and return the number of bytes written */
    frameBytes = CloseBitStream(bitStream);

    /* now calculate desired bits and compare with actual encoded bits */
    desbits = (int) ((double) numChannels * (hEncoder->config.bitRate * FRAME_LEN)
            / hEncoder->sampleRate);

    diff = ((frameBytes - 1 /* CRC */) * 8) - desbits;

    /* do linear correction according to relative difference */
    fix = (double) desbits / ((frameBytes - 1 /* CRC */) * 8);

    /* speed up convergence. A value of 0.92 gives approx up to 10 iterations */
    if (fix > 0.92)
        fix = 0.92;

    hEncoder->aacquantCfg.quality *= fix;

    /* quality should not go lower than 1, set diff to exit loop */
    if (hEncoder->aacquantCfg.quality <= 1)
        diff = -1;
    }
#endif

    // fix max_sfb in CPE mode
    for (channel = 0; channel < numChannels; channel++)
    {
		if (channelInfo[channel].present
				&& (channelInfo[channel].cpe)
				&& (channelInfo[channel].ch_is_left))
		{
			CoderInfo *cil, *cir;

			cil = &coderInfo[channel];
			cir = &coderInfo[channelInfo[channel].paired_ch];

			cil->max_sfb = cir->max_sfb = max(cil->max_sfb, cir->max_sfb);
			cil->nr_of_sfb = cir->nr_of_sfb = cil->max_sfb;
		}
    }

    MSReconstruct(coderInfo, channelInfo, numChannels);

    for (channel = 0; channel < numChannels; channel++)
    {
        /* If short window, reconstruction not needed for prediction */
        if ((coderInfo[channel].block_type == ONLY_SHORT_WINDOW)) {
            int sind;
            for (sind = 0; sind < BLOCK_LEN_LONG; sind++) {
				coderInfo[channel].requantFreq[sind] = 0.0;
            }
        } else {

            if((coderInfo[channel].tnsInfo.tnsDataPresent != 0) && (useTns))
                tnsDecInfo = &(coderInfo[channel].tnsInfo);
            else
                tnsDecInfo = NULL;

            if ((!channelInfo[channel].lfe) && (aacObjectType == LTP)) {  /* no reconstruction needed for LFE channel*/

                LtpReconstruct(&coderInfo[channel], &(coderInfo[channel].ltpInfo),
						coderInfo[channel].requantFreq);

                if(tnsDecInfo != NULL)
                    TnsDecodeFilterOnly(&(coderInfo[channel].tnsInfo), coderInfo[channel].nr_of_sfb,
							coderInfo[channel].max_sfb, coderInfo[channel].block_type,
							coderInfo[channel].sfb_offset, coderInfo[channel].requantFreq);

                IFilterBank(hEncoder, &coderInfo[channel],
						coderInfo[channel].requantFreq,
						coderInfo[channel].ltpInfo.time_buffer,
						coderInfo[channel].ltpInfo.ltp_overlap_buffer,
						MOVERLAPPED);

                LtpUpdate(&(coderInfo[channel].ltpInfo),
						coderInfo[channel].ltpInfo.time_buffer,
						coderInfo[channel].ltpInfo.ltp_overlap_buffer,
						BLOCK_LEN_LONG);
            }
        }
    }

#ifndef DRM
    /* Write the AAC bitstream */
    bitStream = OpenBitStream(bufferSize, outputBuffer);

    WriteBitstream(hEncoder, coderInfo, channelInfo, bitStream, numChannels);

    /* Close the bitstream and return the number of bytes written */
    frameBytes = CloseBitStream(bitStream);

    /* Adjust quality to get correct average bitrate */
    if (hEncoder->config.bitRate)
	{
		double fix;
		int desbits = numChannels * (hEncoder->config.bitRate * FRAME_LEN)
				/ hEncoder->sampleRate;
		int diff = (frameBytes * 8) - desbits;

		hEncoder->bitDiff += diff;
		fix = (double)hEncoder->bitDiff / desbits;
		fix *= 0.01;
		fix = max(fix, -0.2);
		fix = min(fix, 0.2);

		if (((diff > 0) && (fix > 0.0)) || ((diff < 0) && (fix < 0.0)))
		{
			hEncoder->aacquantCfg.quality *= (1.0 - fix);
			if (hEncoder->aacquantCfg.quality > 300)
				hEncoder->aacquantCfg.quality = 300;
            if (hEncoder->aacquantCfg.quality < 50)
                hEncoder->aacquantCfg.quality = 50;
		}
    }
#endif

    return frameBytes;
}


#ifdef DRM
/* Scalefactorband data table for 960 transform length */
/* all parameters which are different from the 1024 transform length table are
   marked with an "x" */
static SR_INFO srInfo[12+1] =
{
    { 96000, 40/*x*/, 12,
        {
            4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4,
            8, 8, 8, 8, 8, 12, 12, 12, 12, 12, 16, 16, 24, 28,
            36, 44, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 0/*x*/
        },{
            4, 4, 4, 4, 4, 4, 8, 8, 8, 16, 28, 28/*x*/
        }
    }, { 88200, 40/*x*/, 12,
        {
            4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4,
            8, 8, 8, 8, 8, 12, 12, 12, 12, 12, 16, 16, 24, 28,
            36, 44, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 0/*x*/
        },{
            4, 4, 4, 4, 4, 4, 8, 8, 8, 16, 28, 28/*x*/
        }
    }, { 64000, 45/*x*/, 12,
        {
            4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4,
            8, 8, 8, 8, 12, 12, 12, 16, 16, 16, 20, 24, 24, 28,
            36, 40, 40, 40, 40, 40, 40, 40, 40, 40, 40, 40, 40, 40,
            40, 40, 40, 16/*x*/, 0/*x*/
        },{
            4, 4, 4, 4, 4, 4, 8, 8, 8, 16, 28, 28/*x*/
        }
    }, { 48000, 49, 14,
        {
            4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  8,  8,  8,  8,  8,  8,  8,
            12, 12, 12, 12, 16, 16, 20, 20, 24, 24, 28, 28, 32, 32, 32, 32, 32, 32,
            32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32/*x*/
        }, {
            4,  4,  4,  4,  4,  8,  8,  8, 12, 12, 12, 16, 16, 8/*x*/
        }
    }, { 44100, 49, 14,
        {
            4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  8,  8,  8,  8,  8,  8,  8,
            12, 12, 12, 12, 16, 16, 20, 20, 24, 24, 28, 28, 32, 32, 32, 32, 32, 32,
            32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32/*x*/
        }, {
            4,  4,  4,  4,  4,  8,  8,  8, 12, 12, 12, 16, 16, 8/*x*/
        }
    }, { 32000, 49/*x*/, 14,
        {
            4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  8,  8,  8,  8,
            8,  8,  8,  12, 12, 12, 12, 16, 16, 20, 20, 24, 24, 28,
            28, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32,
            32, 32, 32, 32, 32, 32, 32, 0/*x*/, 0/*x*/
        },{
            4,  4,  4,  4,  4,  8,  8,  8,  12, 12, 12, 16, 16, 16
        }
    }, { 24000, 46/*x*/, 15,
        {
            4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  8,  8,  8,  8,  8,  8,  8,
            8,  8,  8,  12, 12, 12, 12, 16, 16, 16, 20, 20, 24, 24, 28, 28, 32,
            36, 36, 40, 44, 48, 52, 52, 64, 64, 64, 64, 0/*x*/
        }, {
            4,  4,  4,  4,  4,  4,  4,  8,  8,  8, 12, 12, 16, 16, 12/*x*/
        }
    }, { 22050, 46/*x*/, 15,
        {
            4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  8,  8,  8,  8,  8,  8,  8,
            8,  8,  8,  12, 12, 12, 12, 16, 16, 16, 20, 20, 24, 24, 28, 28, 32,
            36, 36, 40, 44, 48, 52, 52, 64, 64, 64, 64, 0/*x*/
        }, {
            4,  4,  4,  4,  4,  4,  4,  8,  8,  8, 12, 12, 16, 16, 12/*x*/
        }
    }, { 16000, 42/*x*/, 15,
        {
            8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 12, 12, 12,
            12, 12, 12, 12, 12, 12, 16, 16, 16, 16, 20, 20, 20, 24,
            24, 28, 28, 32, 36, 40, 40, 44, 48, 52, 56, 60, 64, 64, 0/*x*/
        }, {
            4, 4, 4, 4, 4, 4, 4, 4, 8, 8, 12, 12, 16, 20, 12/*x*/
        }
    }, { 12000, 42/*x*/, 15,
        {
            8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 12, 12, 12,
            12, 12, 12, 12, 12, 12, 16, 16, 16, 16, 20, 20, 20, 24,
            24, 28, 28, 32, 36, 40, 40, 44, 48, 52, 56, 60, 64, 64, 0/*x*/
        }, {
            4, 4, 4, 4, 4, 4, 4, 4, 8, 8, 12, 12, 16, 20, 12/*x*/
        }
    }, { 11025, 42/*x*/, 15,
        {
            8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 12, 12, 12,
            12, 12, 12, 12, 12, 12, 16, 16, 16, 16, 20, 20, 20, 24,
            24, 28, 28, 32, 36, 40, 40, 44, 48, 52, 56, 60, 64, 64, 0/*x*/
        }, {
            4, 4, 4, 4, 4, 4, 4, 4, 8, 8, 12, 12, 16, 20, 12/*x*/
        }
    }, { 8000, 40, 15,
        {
            12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 16,
            16, 16, 16, 16, 16, 16, 20, 20, 20, 20, 24, 24, 24, 28,
            28, 32, 36, 36, 40, 44, 48, 52, 56, 60, 64, 16/*x*/
        }, {
            4, 4, 4, 4, 4, 4, 4, 8, 8, 8, 8, 12, 16, 20, 12/*x*/
        }
    },
    { -1 }
};
#else
/* Scalefactorband data table for 1024 transform length */
static SR_INFO srInfo[12+1] =
{
    { 96000, 41, 12,
        {
            4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4,
            8, 8, 8, 8, 8, 12, 12, 12, 12, 12, 16, 16, 24, 28,
            36, 44, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64
        },{
            4, 4, 4, 4, 4, 4, 8, 8, 8, 16, 28, 36
        }
    }, { 88200, 41, 12,
        {
            4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4,
            8, 8, 8, 8, 8, 12, 12, 12, 12, 12, 16, 16, 24, 28,
            36, 44, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64
        },{
            4, 4, 4, 4, 4, 4, 8, 8, 8, 16, 28, 36
        }
    }, { 64000, 47, 12,
        {
            4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4,
            8, 8, 8, 8, 12, 12, 12, 16, 16, 16, 20, 24, 24, 28,
            36, 40, 40, 40, 40, 40, 40, 40, 40, 40, 40, 40, 40, 40,
            40, 40, 40, 40, 40
        },{
            4, 4, 4, 4, 4, 4, 8, 8, 8, 16, 28, 32
        }
    }, { 48000, 49, 14,
        {
            4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  8,  8,  8,  8,  8,  8,  8,
            12, 12, 12, 12, 16, 16, 20, 20, 24, 24, 28, 28, 32, 32, 32, 32, 32, 32,
            32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 96
        }, {
            4,  4,  4,  4,  4,  8,  8,  8, 12, 12, 12, 16, 16, 16
        }
    }, { 44100, 49, 14,
        {
            4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  8,  8,  8,  8,  8,  8,  8,
            12, 12, 12, 12, 16, 16, 20, 20, 24, 24, 28, 28, 32, 32, 32, 32, 32, 32,
            32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 96
        }, {
            4,  4,  4,  4,  4,  8,  8,  8, 12, 12, 12, 16, 16, 16
        }
    }, { 32000, 51, 14,
        {
            4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  8,  8,  8,  8,
            8,  8,  8,  12, 12, 12, 12, 16, 16, 20, 20, 24, 24, 28,
            28, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32,
            32, 32, 32, 32, 32, 32, 32, 32, 32
        },{
            4,  4,  4,  4,  4,  8,  8,  8,  12, 12, 12, 16, 16, 16
        }
    }, { 24000, 47, 15,
        {
            4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  8,  8,  8,  8,  8,  8,  8,
            8,  8,  8,  12, 12, 12, 12, 16, 16, 16, 20, 20, 24, 24, 28, 28, 32,
            36, 36, 40, 44, 48, 52, 52, 64, 64, 64, 64, 64
        }, {
            4,  4,  4,  4,  4,  4,  4,  8,  8,  8, 12, 12, 16, 16, 20
        }
    }, { 22050, 47, 15,
        {
            4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  8,  8,  8,  8,  8,  8,  8,
            8,  8,  8,  12, 12, 12, 12, 16, 16, 16, 20, 20, 24, 24, 28, 28, 32,
            36, 36, 40, 44, 48, 52, 52, 64, 64, 64, 64, 64
        }, {
            4,  4,  4,  4,  4,  4,  4,  8,  8,  8, 12, 12, 16, 16, 20
        }
    }, { 16000, 43, 15,
        {
            8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 12, 12, 12,
            12, 12, 12, 12, 12, 12, 16, 16, 16, 16, 20, 20, 20, 24,
            24, 28, 28, 32, 36, 40, 40, 44, 48, 52, 56, 60, 64, 64, 64
        }, {
            4, 4, 4, 4, 4, 4, 4, 4, 8, 8, 12, 12, 16, 20, 20
        }
    }, { 12000, 43, 15,
        {
            8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 12, 12, 12,
            12, 12, 12, 12, 12, 12, 16, 16, 16, 16, 20, 20, 20, 24,
            24, 28, 28, 32, 36, 40, 40, 44, 48, 52, 56, 60, 64, 64, 64
        }, {
            4, 4, 4, 4, 4, 4, 4, 4, 8, 8, 12, 12, 16, 20, 20
        }
    }, { 11025, 43, 15,
        {
            8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 12, 12, 12,
            12, 12, 12, 12, 12, 12, 16, 16, 16, 16, 20, 20, 20, 24,
            24, 28, 28, 32, 36, 40, 40, 44, 48, 52, 56, 60, 64, 64, 64
        }, {
            4, 4, 4, 4, 4, 4, 4, 4, 8, 8, 12, 12, 16, 20, 20
        }
    }, { 8000, 40, 15,
        {
            12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 16,
            16, 16, 16, 16, 16, 16, 20, 20, 20, 20, 24, 24, 24, 28,
            28, 32, 36, 36, 40, 44, 48, 52, 56, 60, 64, 80
        }, {
            4, 4, 4, 4, 4, 4, 4, 8, 8, 8, 8, 12, 16, 20, 20
        }
    },
    { -1 }
};
#endif

/*
$Log: frame.c,v $
Revision 1.67  2004/11/17 14:26:06  menno
Infinite loop fix
dunno if this is good, encoder might be tuned to use energies from before MS encoding. But since the MS encoded samples are used in quantisation this might actually be better. Please test.

Revision 1.66  2004/11/04 12:51:09  aforanna
version number updated to 1.24.1 due to changes in Winamp and CoolEdit plugins

Revision 1.65  2004/07/18 09:34:24  corrados
New bandwidth settings for DRM, improved quantization quality adaptation (almost constant bit-rate now)

Revision 1.64  2004/07/13 17:56:37  corrados
bug fix with new object type definitions

Revision 1.63  2004/07/08 14:01:25  corrados
New scalefactorband table for 960 transform length, bug fix in HCR

Revision 1.62  2004/07/04 12:10:52  corrados
made faac compliant with Digital Radio Mondiale (DRM) (DRM macro must be set)
implemented HCR tool, VCB11, CRC, scalable bitstream order
note: VCB11 only uses codebook 11! TODO: implement codebooks 16-32
960 transform length is not yet implemented (TODO)! Use 1024 for encoding and 960 for decoding, resulting in a lot of artefacts

Revision 1.61  2004/05/03 11:37:16  danchr
bump version to unstable 1.24+

Revision 1.60  2004/04/13 13:47:33  danchr
clarify release <> unstable status

Revision 1.59  2004/04/02 14:56:17  danchr
fix name clash w/ libavcodec: fft_init -> fft_initialize
bump version number to 1.24 beta

Revision 1.58  2004/03/17 13:34:20  danchr
Automatic, untuned setting of lowpass for VBR.

Revision 1.57  2004/03/15 20:16:42  knik
fixed copyright notice

Revision 1.56  2004/01/23 10:22:26  stux
*** empty log message ***

Revision 1.55  2003/12/17 20:59:55  knik
changed default cutoff to 16k

Revision 1.54  2003/11/24 18:09:12  knik
A safe version of faacEncGetVersion() without string length problem.
Removed Stux from copyright notice. I don't think he contributed something very
substantial to faac and this is not the right place to list all contributors.

Revision 1.53  2003/11/16 05:02:52  stux
moved global tables from fft.c into hEncoder FFT_Tables. Add fft_init and fft_terminate, flowed through all necessary changes. This should remove at least one instance of a memory leak, and fix some thread-safety problems. Version update to 1.23.3

Revision 1.52  2003/11/15 08:13:42  stux
added FaacEncGetVersion(), version 1.23.2, added myself to faacCopyright :-P, does vanity know no bound ;)

Revision 1.51  2003/11/10 17:48:00  knik
Allowed independent bitRate and bandWidth setting.
Small fixes.

Revision 1.50  2003/10/29 10:31:25  stux
Added channel_map to FaacEncHandle, facilitates free generalised channel remapping in the faac core. Default is straight-through, should be *zero* performance hit... and even probably an immeasurable performance gain, updated FAAC_CFG_VERSION to 104 and FAAC_VERSION to 1.22.0

Revision 1.49  2003/10/12 16:43:39  knik
average bitrate control made more stable

Revision 1.48  2003/10/12 14:29:53  knik
more accurate average bitrate control

Revision 1.47  2003/09/24 16:26:54  knik
faacEncStruct: quantizer specific data enclosed in AACQuantCfg structure.
Added config option to enforce block type.

Revision 1.46  2003/09/07 16:48:31  knik
Updated psymodel call. Updated bitrate/cutoff mapping table.

Revision 1.45  2003/08/23 15:02:13  knik
last frame moved back to the library

Revision 1.44  2003/08/15 11:42:08  knik
removed single silent flush frame

Revision 1.43  2003/08/11 09:43:47  menno
thread safety, some tables added to the encoder context

Revision 1.42  2003/08/09 11:39:30  knik
LFE support enabled by default

Revision 1.41  2003/08/08 10:02:09  menno
Small fix

Revision 1.40  2003/08/07 08:17:00  knik
Better LFE support (reduced bandwidth)

Revision 1.39  2003/08/02 11:32:10  stux
added config.inputFormat, and associated defines and code, faac now handles native endian 16bit, 24bit and float input. Added faacEncGetDecoderSpecificInfo to the dll exports, needed for MP4. Updated DLL .dsp to compile without error. Updated CFG_VERSION to 102. Version number might need to be updated as the API has technically changed. Did not update libfaac.pdf

Revision 1.38  2003/07/10 19:17:01  knik
24-bit input

Revision 1.37  2003/06/26 19:20:09  knik
Mid/Side support.
Copyright info moved from frontend.
Fixed memory leak.

Revision 1.36  2003/05/12 17:53:16  knik
updated ABR table

Revision 1.35  2003/05/10 09:39:55  knik
added approximate ABR setting
modified default cutoff

Revision 1.34  2003/05/01 09:31:39  knik
removed ISO psyodel
disabled m/s coding
fixed default bandwidth
reduced max_sfb check

Revision 1.33  2003/04/13 08:37:23  knik
version number moved to version.h

Revision 1.32  2003/03/27 17:08:23  knik
added quantizer quality and bandwidth setting

Revision 1.31  2002/10/11 18:00:15  menno
small bugfix

Revision 1.30  2002/10/08 18:53:01  menno
Fixed some memory leakage

Revision 1.29  2002/08/19 16:34:43  knik
added one additional flush frame
fixed sample buffer memory allocation

*/

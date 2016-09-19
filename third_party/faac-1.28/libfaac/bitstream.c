/**********************************************************************

This software module was originally developed by
and edited by Texas Instruments in the course of
development of the MPEG-2 NBC/MPEG-4 Audio standard
ISO/IEC 13818-7, 14496-1,2 and 3. This software module is an
implementation of a part of one or more MPEG-2 NBC/MPEG-4 Audio tools
as specified by the MPEG-2 NBC/MPEG-4 Audio standard. ISO/IEC gives
users of the MPEG-2 NBC/MPEG-4 Audio standards free license to this
software module or modifications thereof for use in hardware or
software products claiming conformance to the MPEG-2 NBC/ MPEG-4 Audio
standards. Those intending to use this software module in hardware or
software products are advised that this use may infringe existing
patents. The original developer of this software module and his/her
company, the subsequent editors and their companies, and ISO/IEC have
no liability for use of this software module or modifications thereof
in an implementation. Copyright is not released for non MPEG-2
NBC/MPEG-4 Audio conforming products. The original developer retains
full right to use the code for his/her own purpose, assign or donate
the code to a third party and to inhibit third party from using the
code for non MPEG-2 NBC/MPEG-4 Audio conforming products. This
copyright notice must be included in all copies or derivative works.

Copyright (c) 1997.
**********************************************************************/
/*
 * $Id: bitstream.c,v 1.34 2007/06/05 18:59:47 menno Exp $
 */

#include <stdio.h>
#include <stdlib.h>

#include "coder.h"
#include "channels.h"
#include "huffman.h"
#include "bitstream.h"
#include "ltp.h"
#include "util.h"

static int CountBitstream(faacEncHandle hEncoder,
                          CoderInfo *coderInfo,
                          ChannelInfo *channelInfo,
                          BitStream *bitStream,
                          int numChannels);
static int WriteADTSHeader(faacEncHandle hEncoder,
                           BitStream *bitStream,
                           int writeFlag);
static int WriteCPE(CoderInfo *coderInfoL,
                    CoderInfo *coderInfoR,
                    ChannelInfo *channelInfo,
                    BitStream* bitStream,
                    int objectType,
                    int writeFlag);
static int WriteSCE(CoderInfo *coderInfo,
                    ChannelInfo *channelInfo,
                    BitStream *bitStream,
                    int objectType,
                    int writeFlag);
static int WriteLFE(CoderInfo *coderInfo,
                    ChannelInfo *channelInfo,
                    BitStream *bitStream,
                    int objectType,
                    int writeFlag);
static int WriteICSInfo(CoderInfo *coderInfo,
                        BitStream *bitStream,
                        int objectType,
                        int common_window,
                        int writeFlag);
static int WriteICS(CoderInfo *coderInfo,
                    BitStream *bitStream,
                    int commonWindow,
                    int objectType,
                    int writeFlag);
static int WriteLTPPredictorData(CoderInfo *coderInfo,
                                 BitStream *bitStream,
                                 int writeFlag);
static int WritePredictorData(CoderInfo *coderInfo,
                              BitStream *bitStream,
                              int writeFlag);
static int WritePulseData(CoderInfo *coderInfo,
                          BitStream *bitStream,
                          int writeFlag);
static int WriteTNSData(CoderInfo *coderInfo,
                        BitStream *bitStream,
                        int writeFlag);
static int WriteGainControlData(CoderInfo *coderInfo,
                                BitStream *bitStream,
                                int writeFlag);
static int WriteSpectralData(CoderInfo *coderInfo,
                             BitStream *bitStream,
                             int writeFlag);
static int WriteAACFillBits(BitStream* bitStream,
                            int numBits,
                            int writeFlag);
static int FindGroupingBits(CoderInfo *coderInfo);
static long BufferNumBit(BitStream *bitStream);
static int WriteByte(BitStream *bitStream,
                     unsigned long data,
                     int numBit);
static int ByteAlign(BitStream* bitStream,
                     int writeFlag, int bitsSoFar);
#ifdef DRM
static int PutBitHcr(BitStream *bitStream,
                     unsigned long curpos,
                     unsigned long data,
                     int numBit);
static int rewind_word(int W, int len);
static int WriteReorderedSpectralData(CoderInfo *coderInfo,
                                      BitStream *bitStream,
                                      int writeFlag);
static void calc_CRC(BitStream *bitStream, int len);
#endif


static int WriteFAACStr(BitStream *bitStream, char *version, int write)
{
  int i;
  char str[200];
  int len, padbits, count;
  int bitcnt;

  sprintf(str, "libfaac %s", version);

  len = strlen(str) + 1;
  padbits = (8 - ((bitStream->numBit + 7) % 8)) % 8;
  count = len + 3;

  bitcnt = LEN_SE_ID + 4 + ((count < 15) ? 0 : 8) + count * 8;
  if (!write)
    return bitcnt;

  PutBit(bitStream, ID_FIL, LEN_SE_ID);
  if (count < 15)
  {
    PutBit(bitStream, count, 4);
  }
  else
  {
    PutBit(bitStream, 15, 4);
    PutBit(bitStream, count - 14, 8);
  }

  PutBit(bitStream, 0, padbits);
  PutBit(bitStream, 0, 8);
  PutBit(bitStream, 0, 8); // just in case
  for (i = 0; i < len; i++)
    PutBit(bitStream, str[i], 8);

  PutBit(bitStream, 0, 8 - padbits);

  return bitcnt;
}


int WriteBitstream(faacEncHandle hEncoder,
                   CoderInfo *coderInfo,
                   ChannelInfo *channelInfo,
                   BitStream *bitStream,
                   int numChannel)
{
    int channel;
    int bits = 0;
    int bitsLeftAfterFill, numFillBits;

    CountBitstream(hEncoder, coderInfo, channelInfo, bitStream, numChannel);

    if(hEncoder->config.outputFormat == 1){
        bits += WriteADTSHeader(hEncoder, bitStream, 1);
    }else{
        bits = 0; // compilier will remove it, byt anyone will see that current size of bitstream is 0
    }

/* sur: faad2 complains about scalefactor error if we are writing FAAC String */
#ifndef DRM
    if (hEncoder->frameNum == 4)
      WriteFAACStr(bitStream, hEncoder->config.name, 1);
#endif

    for (channel = 0; channel < numChannel; channel++) {

        if (channelInfo[channel].present) {

            /* Write out a single_channel_element */
            if (!channelInfo[channel].cpe) {

                if (channelInfo[channel].lfe) {
                    /* Write out lfe */
                    bits += WriteLFE(&coderInfo[channel],
                        &channelInfo[channel],
                        bitStream,
                        hEncoder->config.aacObjectType,
                        1);
                } else {
                    /* Write out sce */
                    bits += WriteSCE(&coderInfo[channel],
                        &channelInfo[channel],
                        bitStream,
                        hEncoder->config.aacObjectType,
                        1);
                }

            } else {

                if (channelInfo[channel].ch_is_left) {
                    /* Write out cpe */
                    bits += WriteCPE(&coderInfo[channel],
                        &coderInfo[channelInfo[channel].paired_ch],
                        &channelInfo[channel],
                        bitStream,
                        hEncoder->config.aacObjectType,
                        1);
                }
            }
        }
    }

    /* Compute how many fill bits are needed to avoid overflowing bit reservoir */
    /* Save room for ID_END terminator */
    if (bits < (8 - LEN_SE_ID) ) {
        numFillBits = 8 - LEN_SE_ID - bits;
    } else {
        numFillBits = 0;
    }

    /* Write AAC fill_elements, smallest fill element is 7 bits. */
    /* Function may leave up to 6 bits left after fill, so tell it to fill a few extra */
    numFillBits += 6;
    bitsLeftAfterFill = WriteAACFillBits(bitStream, numFillBits, 1);
    bits += (numFillBits - bitsLeftAfterFill);

    /* Write ID_END terminator */
    bits += LEN_SE_ID;
    PutBit(bitStream, ID_END, LEN_SE_ID);

    /* Now byte align the bitstream */
    /*
     * This byte_alignment() is correct for both MPEG2 and MPEG4, although
     * in MPEG4 the byte_alignment() is officially done before the new frame
     * instead of at the end. But this is basically the same.
     */
    bits += ByteAlign(bitStream, 1, bits);

    return bits;
}

static int CountBitstream(faacEncHandle hEncoder,
                          CoderInfo *coderInfo,
                          ChannelInfo *channelInfo,
                          BitStream *bitStream,
                          int numChannel)
{
    int channel;
    int bits = 0;
    int bitsLeftAfterFill, numFillBits;


    if(hEncoder->config.outputFormat == 1){
        bits += WriteADTSHeader(hEncoder, bitStream, 0);
    }else{
        bits = 0; // compilier will remove it, byt anyone will see that current size of bitstream is 0
    }

/* sur: faad2 complains about scalefactor error if we are writing FAAC String */
#ifndef DRM
    if (hEncoder->frameNum == 4)
      bits += WriteFAACStr(bitStream, hEncoder->config.name, 0);
#endif

    for (channel = 0; channel < numChannel; channel++) {

        if (channelInfo[channel].present) {

            /* Write out a single_channel_element */
            if (!channelInfo[channel].cpe) {

                if (channelInfo[channel].lfe) {
                    /* Write out lfe */
                    bits += WriteLFE(&coderInfo[channel],
                        &channelInfo[channel],
                        bitStream,
                        hEncoder->config.aacObjectType,
                        0);
                } else {
                    /* Write out sce */
                    bits += WriteSCE(&coderInfo[channel],
                        &channelInfo[channel],
                        bitStream,
                        hEncoder->config.aacObjectType,
                        0);
                }

            } else {

                if (channelInfo[channel].ch_is_left) {
                    /* Write out cpe */
                    bits += WriteCPE(&coderInfo[channel],
                        &coderInfo[channelInfo[channel].paired_ch],
                        &channelInfo[channel],
                        bitStream,
                        hEncoder->config.aacObjectType,
                        0);
                }
            }
        }
    }

    /* Compute how many fill bits are needed to avoid overflowing bit reservoir */
    /* Save room for ID_END terminator */
    if (bits < (8 - LEN_SE_ID) ) {
        numFillBits = 8 - LEN_SE_ID - bits;
    } else {
        numFillBits = 0;
    }

    /* Write AAC fill_elements, smallest fill element is 7 bits. */
    /* Function may leave up to 6 bits left after fill, so tell it to fill a few extra */
    numFillBits += 6;
    bitsLeftAfterFill = WriteAACFillBits(bitStream, numFillBits, 0);
    bits += (numFillBits - bitsLeftAfterFill);

    /* Write ID_END terminator */
    bits += LEN_SE_ID;

    /* Now byte align the bitstream */
    bits += ByteAlign(bitStream, 0, bits);

    hEncoder->usedBytes = bit2byte(bits);

    return bits;
}

static int WriteADTSHeader(faacEncHandle hEncoder,
                           BitStream *bitStream,
                           int writeFlag)
{
    int bits = 56;

    if (writeFlag) {
        /* Fixed ADTS header */
        PutBit(bitStream, 0xFFFF, 12); /* 12 bit Syncword */
        PutBit(bitStream, hEncoder->config.mpegVersion, 1); /* ID == 0 for MPEG4 AAC, 1 for MPEG2 AAC */
        PutBit(bitStream, 0, 2); /* layer == 0 */
        PutBit(bitStream, 1, 1); /* protection absent */
        PutBit(bitStream, hEncoder->config.aacObjectType - 1, 2); /* profile */
        PutBit(bitStream, hEncoder->sampleRateIdx, 4); /* sampling rate */
        PutBit(bitStream, 0, 1); /* private bit */
        PutBit(bitStream, hEncoder->numChannels, 3); /* ch. config (must be > 0) */
                                                     /* simply using numChannels only works for
                                                        6 channels or less, else a channel
                                                        configuration should be written */
        PutBit(bitStream, 0, 1); /* original/copy */
        PutBit(bitStream, 0, 1); /* home */

#if 0 // Removed in corrigendum 14496-3:2002
        if (hEncoder->config.mpegVersion == 0)
            PutBit(bitStream, 0, 2); /* emphasis */
#endif

        /* Variable ADTS header */
        PutBit(bitStream, 0, 1); /* copyr. id. bit */
        PutBit(bitStream, 0, 1); /* copyr. id. start */
        PutBit(bitStream, hEncoder->usedBytes, 13);
        PutBit(bitStream, 0x7FF, 11); /* buffer fullness (0x7FF for VBR) */
        PutBit(bitStream, 0, 2); /* raw data blocks (0+1=1) */

    }

    /*
     * MPEG2 says byte_aligment() here, but ADTS always is multiple of 8 bits
     * MPEG4 has no byte_alignment() here
     */
    /*
    if (hEncoder->config.mpegVersion == 1)
        bits += ByteAlign(bitStream, writeFlag);
    */

#if 0 // Removed in corrigendum 14496-3:2002
    if (hEncoder->config.mpegVersion == 0)
        bits += 2; /* emphasis */
#endif

    return bits;
}

static int WriteCPE(CoderInfo *coderInfoL,
                    CoderInfo *coderInfoR,
                    ChannelInfo *channelInfo,
                    BitStream* bitStream,
                    int objectType,
                    int writeFlag)
{
    int bits = 0;

#ifndef DRM
    if (writeFlag) {
        /* write ID_CPE, single_element_channel() identifier */
        PutBit(bitStream, ID_CPE, LEN_SE_ID);

        /* write the element_identifier_tag */
        PutBit(bitStream, channelInfo->tag, LEN_TAG);

        /* common_window? */
        PutBit(bitStream, channelInfo->common_window, LEN_COM_WIN);
    }

    bits += LEN_SE_ID;
    bits += LEN_TAG;
    bits += LEN_COM_WIN;
#endif

    /* if common_window, write ics_info */
    if (channelInfo->common_window) {
        int numWindows, maxSfb;

        bits += WriteICSInfo(coderInfoL, bitStream, objectType, channelInfo->common_window, writeFlag);
        numWindows = coderInfoL->num_window_groups;
        maxSfb = coderInfoL->max_sfb;

        if (writeFlag) {
            PutBit(bitStream, channelInfo->msInfo.is_present, LEN_MASK_PRES);
            if (channelInfo->msInfo.is_present == 1) {
                int g;
                int b;
                for (g=0;g<numWindows;g++) {
                    for (b=0;b<maxSfb;b++) {
                        PutBit(bitStream, channelInfo->msInfo.ms_used[g*maxSfb+b], LEN_MASK);
                    }
                }
            }
        }
        bits += LEN_MASK_PRES;
        if (channelInfo->msInfo.is_present == 1)
            bits += (numWindows*maxSfb*LEN_MASK);
    }

    /* Write individual_channel_stream elements */
    bits += WriteICS(coderInfoL, bitStream, channelInfo->common_window, objectType, writeFlag);
    bits += WriteICS(coderInfoR, bitStream, channelInfo->common_window, objectType, writeFlag);

    return bits;
}

static int WriteSCE(CoderInfo *coderInfo,
                    ChannelInfo *channelInfo,
                    BitStream *bitStream,
                    int objectType,
                    int writeFlag)
{
    int bits = 0;

#ifndef DRM
    if (writeFlag) {
        /* write Single Element Channel (SCE) identifier */
        PutBit(bitStream, ID_SCE, LEN_SE_ID);

        /* write the element identifier tag */
        PutBit(bitStream, channelInfo->tag, LEN_TAG);
    }

    bits += LEN_SE_ID;
    bits += LEN_TAG;
#endif

    /* Write an Individual Channel Stream element */
    bits += WriteICS(coderInfo, bitStream, 0, objectType, writeFlag);

    return bits;
}

static int WriteLFE(CoderInfo *coderInfo,
                    ChannelInfo *channelInfo,
                    BitStream *bitStream,
                    int objectType,
                    int writeFlag)
{
    int bits = 0;

    if (writeFlag) {
        /* write ID_LFE, lfe_element_channel() identifier */
        PutBit(bitStream, ID_LFE, LEN_SE_ID);

        /* write the element_identifier_tag */
        PutBit(bitStream, channelInfo->tag, LEN_TAG);
    }

    bits += LEN_SE_ID;
    bits += LEN_TAG;

    /* Write an individual_channel_stream element */
    bits += WriteICS(coderInfo, bitStream, 0, objectType, writeFlag);

    return bits;
}

static int WriteICSInfo(CoderInfo *coderInfo,
                        BitStream *bitStream,
                        int objectType,
                        int common_window,
                        int writeFlag)
{
    int grouping_bits;
    int bits = 0;

    if (writeFlag) {
        /* write out ics_info() information */
        PutBit(bitStream, 0, LEN_ICS_RESERV);  /* reserved Bit*/

        /* Write out window sequence */
        PutBit(bitStream, coderInfo->block_type, LEN_WIN_SEQ);  /* block type */

        /* Write out window shape */
        PutBit(bitStream, coderInfo->window_shape, LEN_WIN_SH);  /* window shape */
    }

    bits += LEN_ICS_RESERV;
    bits += LEN_WIN_SEQ;
    bits += LEN_WIN_SH;

    /* For short windows, write out max_sfb and scale_factor_grouping */
    if (coderInfo->block_type == ONLY_SHORT_WINDOW){
        if (writeFlag) {
            PutBit(bitStream, coderInfo->max_sfb, LEN_MAX_SFBS);
            grouping_bits = FindGroupingBits(coderInfo);
            PutBit(bitStream, grouping_bits, MAX_SHORT_WINDOWS - 1);  /* the grouping bits */
        }
        bits += LEN_MAX_SFBS;
        bits += MAX_SHORT_WINDOWS - 1;
    } else { /* Otherwise, write out max_sfb and predictor data */
        if (writeFlag) {
            PutBit(bitStream, coderInfo->max_sfb, LEN_MAX_SFBL);
        }
        bits += LEN_MAX_SFBL;
#ifdef DRM
    }
    if (writeFlag) {
        PutBit(bitStream,coderInfo->tnsInfo.tnsDataPresent,LEN_TNS_PRES);
    }
    bits += LEN_TNS_PRES;
#endif
        if (objectType == LTP)
        {
            bits++;
            if(writeFlag)
                PutBit(bitStream, coderInfo->ltpInfo.global_pred_flag, 1); /* Prediction Global used */

            bits += WriteLTPPredictorData(coderInfo, bitStream, writeFlag);
            if (common_window)
                bits += WriteLTPPredictorData(coderInfo, bitStream, writeFlag);
        } else {
            bits++;
            if (writeFlag)
                PutBit(bitStream, coderInfo->pred_global_flag, LEN_PRED_PRES);  /* predictor_data_present */

            bits += WritePredictorData(coderInfo, bitStream, writeFlag);
        }
#ifndef DRM
    }
#endif

    return bits;
}

static int WriteICS(CoderInfo *coderInfo,
                    BitStream *bitStream,
                    int commonWindow,
                    int objectType,
                    int writeFlag)
{
    /* this function writes out an individual_channel_stream to the bitstream and */
    /* returns the number of bits written to the bitstream */
    int bits = 0;

#ifndef DRM
    /* Write the 8-bit global_gain */
    if (writeFlag)
        PutBit(bitStream, coderInfo->global_gain, LEN_GLOB_GAIN);
    bits += LEN_GLOB_GAIN;
#endif

    /* Write ics information */
    if (!commonWindow) {
        bits += WriteICSInfo(coderInfo, bitStream, objectType, commonWindow, writeFlag);
    }

#ifdef DRM
    /* Write the 8-bit global_gain */
    if (writeFlag)
        PutBit(bitStream, coderInfo->global_gain, LEN_GLOB_GAIN);
    bits += LEN_GLOB_GAIN;
#endif

    bits += SortBookNumbers(coderInfo, bitStream, writeFlag);
    bits += WriteScalefactors(coderInfo, bitStream, writeFlag);
#ifdef DRM
    if (writeFlag) {
        /* length_of_reordered_spectral_data */
        PutBit(bitStream, coderInfo->iLenReordSpData, LEN_HCR_REORDSD);

        /* length_of_longest_codeword */
        PutBit(bitStream, coderInfo->iLenLongestCW, LEN_HCR_LONGCW);
    }
    bits += LEN_HCR_REORDSD + LEN_HCR_LONGCW;
#else
    bits += WritePulseData(coderInfo, bitStream, writeFlag);
#endif
    bits += WriteTNSData(coderInfo, bitStream, writeFlag);
#ifndef DRM
    bits += WriteGainControlData(coderInfo, bitStream, writeFlag);
#endif

#ifdef DRM
    /* DRM CRC calculation */
    if (writeFlag)
        calc_CRC(bitStream, bits);

    bits += WriteReorderedSpectralData(coderInfo, bitStream, writeFlag);
#else
    bits += WriteSpectralData(coderInfo, bitStream, writeFlag);
#endif

    /* Return number of bits */
    return bits;
}

static int WriteLTPPredictorData(CoderInfo *coderInfo, BitStream *bitStream, int writeFlag)
{
    int i, last_band;
    int bits;
    LtpInfo *ltpInfo = &coderInfo->ltpInfo;

    bits = 0;

    if (ltpInfo->global_pred_flag)
    {

        if(writeFlag)
            PutBit(bitStream, 1, 1); /* LTP used */
        bits++;

        switch(coderInfo->block_type)
        {
        case ONLY_LONG_WINDOW:
        case LONG_SHORT_WINDOW:
        case SHORT_LONG_WINDOW:
            bits += LEN_LTP_LAG;
            bits += LEN_LTP_COEF;
            if(writeFlag)
            {
                PutBit(bitStream, ltpInfo->delay[0], LEN_LTP_LAG);
                PutBit(bitStream, ltpInfo->weight_idx,  LEN_LTP_COEF);
            }

            last_band = ((coderInfo->nr_of_sfb < MAX_LT_PRED_LONG_SFB) ?
                coderInfo->nr_of_sfb : MAX_LT_PRED_LONG_SFB);
//            last_band = coderInfo->nr_of_sfb;

            bits += last_band;
            if(writeFlag)
                for (i = 0; i < last_band; i++)
                    PutBit(bitStream, ltpInfo->sfb_prediction_used[i], LEN_LTP_LONG_USED);
            break;

        default:
            break;
        }
    }

    return (bits);
}

static int WritePredictorData(CoderInfo *coderInfo,
                              BitStream *bitStream,
                              int writeFlag)
{
    int bits = 0;

    /* Write global predictor data present */
    short predictorDataPresent = coderInfo->pred_global_flag;
    int numBands = min(coderInfo->max_pred_sfb, coderInfo->nr_of_sfb);

    if (writeFlag) {
        if (predictorDataPresent) {
            int b;
            if (coderInfo->reset_group_number == -1) {
                PutBit(bitStream, 0, LEN_PRED_RST); /* No prediction reset */
            } else {
                PutBit(bitStream, 1, LEN_PRED_RST);
                PutBit(bitStream, (unsigned long)coderInfo->reset_group_number,
                    LEN_PRED_RSTGRP);
            }

            for (b=0;b<numBands;b++) {
                PutBit(bitStream, coderInfo->pred_sfb_flag[b], LEN_PRED_ENAB);
            }
        }
    }
    bits += (predictorDataPresent) ?
        (LEN_PRED_RST +
        ((coderInfo->reset_group_number)!=-1)*LEN_PRED_RSTGRP +
        numBands*LEN_PRED_ENAB) : 0;

    return bits;
}

static int WritePulseData(CoderInfo *coderInfo,
                          BitStream *bitStream,
                          int writeFlag)
{
    int bits = 0;

    if (writeFlag) {
        PutBit(bitStream, 0, LEN_PULSE_PRES);  /* no pulse_data_present */
    }

    bits += LEN_PULSE_PRES;

    return bits;
}

static int WriteTNSData(CoderInfo *coderInfo,
                        BitStream *bitStream,
                        int writeFlag)
{
    int bits = 0;
    int numWindows;
    int len_tns_nfilt;
    int len_tns_length;
    int len_tns_order;
    int filtNumber;
    int resInBits;
    int bitsToTransmit;
    unsigned long unsignedIndex;
    int w;

    TnsInfo* tnsInfoPtr = &coderInfo->tnsInfo;

#ifndef DRM
    if (writeFlag) {
        PutBit(bitStream,tnsInfoPtr->tnsDataPresent,LEN_TNS_PRES);
    }
    bits += LEN_TNS_PRES;
#endif

    /* If TNS is not present, bail */
    if (!tnsInfoPtr->tnsDataPresent) {
        return bits;
    }

    /* Set window-dependent TNS parameters */
    if (coderInfo->block_type == ONLY_SHORT_WINDOW) {
        numWindows = MAX_SHORT_WINDOWS;
        len_tns_nfilt = LEN_TNS_NFILTS;
        len_tns_length = LEN_TNS_LENGTHS;
        len_tns_order = LEN_TNS_ORDERS;
    }
    else {
        numWindows = 1;
        len_tns_nfilt = LEN_TNS_NFILTL;
        len_tns_length = LEN_TNS_LENGTHL;
        len_tns_order = LEN_TNS_ORDERL;
    }

    /* Write TNS data */
    bits += (numWindows * len_tns_nfilt);
    for (w=0;w<numWindows;w++) {
        TnsWindowData* windowDataPtr = &tnsInfoPtr->windowData[w];
        int numFilters = windowDataPtr->numFilters;
        if (writeFlag) {
            PutBit(bitStream,numFilters,len_tns_nfilt); /* n_filt[] = 0 */
        }
        if (numFilters) {
            bits += LEN_TNS_COEFF_RES;
            resInBits = windowDataPtr->coefResolution;
            if (writeFlag) {
                PutBit(bitStream,resInBits-DEF_TNS_RES_OFFSET,LEN_TNS_COEFF_RES);
            }
            bits += numFilters * (len_tns_length+len_tns_order);
            for (filtNumber=0;filtNumber<numFilters;filtNumber++) {
                TnsFilterData* tnsFilterPtr=&windowDataPtr->tnsFilter[filtNumber];
                int order = tnsFilterPtr->order;
                if (writeFlag) {
                    PutBit(bitStream,tnsFilterPtr->length,len_tns_length);
                    PutBit(bitStream,order,len_tns_order);
                }
                if (order) {
                    bits += (LEN_TNS_DIRECTION + LEN_TNS_COMPRESS);
                    if (writeFlag) {
                        PutBit(bitStream,tnsFilterPtr->direction,LEN_TNS_DIRECTION);
                        PutBit(bitStream,tnsFilterPtr->coefCompress,LEN_TNS_COMPRESS);
                    }
                    bitsToTransmit = resInBits - tnsFilterPtr->coefCompress;
                    bits += order * bitsToTransmit;
                    if (writeFlag) {
                        int i;
                        for (i=1;i<=order;i++) {
                            unsignedIndex = (unsigned long) (tnsFilterPtr->index[i])&(~(~0<<bitsToTransmit));
                            PutBit(bitStream,unsignedIndex,bitsToTransmit);
                        }
                    }
                }
            }
        }
    }
    return bits;
}

static int WriteGainControlData(CoderInfo *coderInfo,
                                BitStream *bitStream,
                                int writeFlag)
{
    int bits = 0;

    if (writeFlag) {
        PutBit(bitStream, 0, LEN_GAIN_PRES);
    }

    bits += LEN_GAIN_PRES;

    return bits;
}

static int WriteSpectralData(CoderInfo *coderInfo,
                             BitStream *bitStream,
                             int writeFlag)
{
    int i, bits = 0;

    /* set up local pointers to data and len */
    /* data array contains data to be written */
    /* len array contains lengths of data words */
    int* data = coderInfo->data;
    int* len  = coderInfo->len;

    if (writeFlag) {
        for(i = 0; i < coderInfo->spectral_count; i++) {
            if (len[i] > 0) {  /* only send out non-zero codebook data */
                PutBit(bitStream, data[i], len[i]); /* write data */
                bits += len[i];
            }
        }
    } else {
        for(i = 0; i < coderInfo->spectral_count; i++) {
            bits += len[i];
        }
    }

    return bits;
}

static int WriteAACFillBits(BitStream* bitStream,
                            int numBits,
                            int writeFlag)
{
    int numberOfBitsLeft = numBits;

    /* Need at least (LEN_SE_ID + LEN_F_CNT) bits for a fill_element */
    int minNumberOfBits = LEN_SE_ID + LEN_F_CNT;

    while (numberOfBitsLeft >= minNumberOfBits)
    {
        int numberOfBytes;
        int maxCount;

        if (writeFlag) {
            PutBit(bitStream, ID_FIL, LEN_SE_ID);   /* Write fill_element ID */
        }
        numberOfBitsLeft -= minNumberOfBits;    /* Subtract for ID,count */

        numberOfBytes = (int)(numberOfBitsLeft/LEN_BYTE);
        maxCount = (1<<LEN_F_CNT) - 1;  /* Max count without escaping */

        /* if we have less than maxCount bytes, write them now */
        if (numberOfBytes < maxCount) {
            int i;
            if (writeFlag) {
                PutBit(bitStream, numberOfBytes, LEN_F_CNT);
                for (i = 0; i < numberOfBytes; i++) {
                    PutBit(bitStream, 0, LEN_BYTE);
                }
            }
            /* otherwise, we need to write an escape count */
        }
        else {
            int maxEscapeCount, maxNumberOfBytes, escCount;
            int i;
            if (writeFlag) {
                PutBit(bitStream, maxCount, LEN_F_CNT);
            }
            maxEscapeCount = (1<<LEN_BYTE) - 1;  /* Max escape count */
            maxNumberOfBytes = maxCount + maxEscapeCount;
            numberOfBytes = (numberOfBytes > maxNumberOfBytes ) ? (maxNumberOfBytes) : (numberOfBytes);
            escCount = numberOfBytes - maxCount;
            if (writeFlag) {
                PutBit(bitStream, escCount, LEN_BYTE);
                for (i = 0; i < numberOfBytes-1; i++) {
                    PutBit(bitStream, 0, LEN_BYTE);
                }
            }
        }
        numberOfBitsLeft -= LEN_BYTE*numberOfBytes;
    }

    return numberOfBitsLeft;
}

static int FindGroupingBits(CoderInfo *coderInfo)
{
    /* This function inputs the grouping information and outputs the seven bit
    'grouping_bits' field that the AAC decoder expects.  */

    int grouping_bits = 0;
    int tmp[8];
    int i, j;
    int index = 0;

    for(i = 0; i < coderInfo->num_window_groups; i++){
        for (j = 0; j < coderInfo->window_group_length[i]; j++){
            tmp[index++] = i;
        }
    }

    for(i = 1; i < 8; i++){
        grouping_bits = grouping_bits << 1;
        if(tmp[i] == tmp[i-1]) {
            grouping_bits++;
        }
    }

    return grouping_bits;
}

/* size in bytes! */
BitStream *OpenBitStream(int size, unsigned char *buffer)
{
    BitStream *bitStream;

    bitStream = AllocMemory(sizeof(BitStream));
    bitStream->size = size;
#ifdef DRM
    /* skip first byte for CRC */
    bitStream->numBit = 8;
    bitStream->currentBit = 8;
#else
    bitStream->numBit = 0;
    bitStream->currentBit = 0;
#endif
    bitStream->data = buffer;
    SetMemory(bitStream->data, 0, size);

    return bitStream;
}

int CloseBitStream(BitStream *bitStream)
{
    int bytes = bit2byte(bitStream->numBit);

    FreeMemory(bitStream);

    return bytes;
}

static long BufferNumBit(BitStream *bitStream)
{
    return bitStream->numBit;
}

static int WriteByte(BitStream *bitStream,
                     unsigned long data,
                     int numBit)
{
    long numUsed,idx;

    idx = (bitStream->currentBit / BYTE_NUMBIT) % bitStream->size;
    numUsed = bitStream->currentBit % BYTE_NUMBIT;
#ifndef DRM
    if (numUsed == 0)
        bitStream->data[idx] = 0;
#endif
    bitStream->data[idx] |= (data & ((1<<numBit)-1)) <<
        (BYTE_NUMBIT-numUsed-numBit);
    bitStream->currentBit += numBit;
    bitStream->numBit = bitStream->currentBit;

    return 0;
}

int PutBit(BitStream *bitStream,
           unsigned long data,
           int numBit)
{
    int num,maxNum,curNum;
    unsigned long bits;

    if (numBit == 0)
        return 0;

    /* write bits in packets according to buffer byte boundaries */
    num = 0;
    maxNum = BYTE_NUMBIT - bitStream->currentBit % BYTE_NUMBIT;
    while (num < numBit) {
        curNum = min(numBit-num,maxNum);
        bits = data>>(numBit-num-curNum);
        if (WriteByte(bitStream, bits, curNum)) {
            return 1;
        }
        num += curNum;
        maxNum = BYTE_NUMBIT;
    }

    return 0;
}

static int ByteAlign(BitStream *bitStream, int writeFlag, int bitsSoFar)
{
    int len, i,j;

    if (writeFlag)
    {
        len = BufferNumBit(bitStream);
    } else {
        len = bitsSoFar;
    }

    j = (8 - (len%8))%8;

    if ((len % 8) == 0) j = 0;
    if (writeFlag) {
        for( i=0; i<j; i++ ) {
            PutBit(bitStream, 0, 1);
        }
    }
    return j;
}

#ifdef DRM
/*
    ****************************************************************************
    The following code was written by Volker Fischer (c) 2004

    The GNU Lesser General Public License as published by the
    Free Software Foundation applies to this code.
    ****************************************************************************
*/
#define LEN_PRESORT_CODEBOOK 22
static const unsigned short PresortedCodebook_VCB11[] = {11, 31, 30, 29, 28, 27, 26, 25, 24, 23, 22, 21, 20, 19, 18, 17, 16, 9, 7, 5, 3, 1};
static const int maxCwLen[32] = {0, 11, 9, 20, 16, 13, 11, 14, 12, 17, 14, 49,
    0, 0, 0, 0, 14, 17, 21, 21, 25, 25, 29, 29, 29, 29, 33, 33, 33, 37, 37, 41}; /* 8.5.3.3.3.1 */

typedef struct { /* segment parameters */
    unsigned int left; /* left start of free space in segment */
    unsigned int right; /* right position of free space in segment */
    unsigned int len; /* length of free space in segment */
} segment_t;

typedef struct { /* codeword parameters */
    unsigned int   cw_offset; /* offset in actual codeword data vector */
    unsigned short window; /* which window belongs to this codeword */
    unsigned short cb; /* codebook */
    unsigned short num_sl_cw; /* number of spectral lines per codeword */
    unsigned int   cw_nr; /* codeword number in the window */
    unsigned short cw_len; /* codeword lenght */
    unsigned short num_data; /* number of data cells for codeword */
} cw_info_t;

static int PutBitHcr(BitStream *bitStream,
                     unsigned long curpos,
                     unsigned long data,
                     int numBit)
{ /* data can be written at an arbitrary position in the bitstream */
    bitStream->currentBit = curpos;
    return PutBit(bitStream, data, numBit);
}

static int rewind_word(int W, int len)
{ /* rewind len (max. 32) bits so that the MSB becomes LSB */
    short i;
    int tmp_W = 0;

    for (i = 0; i < len; i++) {
        tmp_W <<= 1;
        if (W & (1<<i)) tmp_W |= 1;
    }
    return tmp_W;
}

static int WriteReorderedSpectralData(CoderInfo *coderInfo,
                                      BitStream *bitStream,
                                      int writeFlag)
{
    int i, j;
    int cursegmsize, accsegmsize = 0;
    int segmcnt = 0;
    long startbitpos;
    segment_t segment[FRAME_LEN];
    int* window_group_length = coderInfo->window_group_length;
    int* sfb_offset = coderInfo->sfb_offset;

    cw_info_t cw_info[FRAME_LEN];
    cw_info_t cw_info_preso[FRAME_LEN];

    int num_cw = coderInfo->cur_cw;
    int window_cw_cnt[MAX_SHORT_WINDOWS] = {0,0,0,0,0,0,0,0};

    int presort, set, num_sets;

    unsigned short cur_cb, cw_cnt;
    short is_backwards;
    int diff, tmp_data, cw_part_cnt, cur_cw_part;

    int cur_cw_len, cur_data;
    int sfb_cnt, win_cnt, acc_win_cnt, win_grp_cnt;
    int coeff_cnt, last_sfb, cur_sfb_len;

    /* set up local pointers to data and len */
    /* data array contains data to be written */
    /* len array contains lengths of data words */
    int* data = coderInfo->data;
    int* len  = coderInfo->len;
    int* num_data = coderInfo->num_data_cw;

    if (writeFlag) {
        /* build offset table */
        cur_data = 0;
        cw_info[0].cw_offset = 0;
        for (i = 0; i < num_cw; i++) {
            cur_cw_len = 0;
            for (j = 0; j < num_data[i]; j++) {
                cur_cw_len += len[cur_data++];
            }

            cw_info[i].num_data = num_data[i];
            cw_info[i].cw_len = cur_cw_len;
            if (i > 0) /* calculate offset (codeword info parameter) */
                cw_info[i].cw_offset = cw_info[i - 1].cw_offset + num_data[i - 1];
        }

        /* presort codewords ------------------------------------------------ */
        /* classify codewords first */
        sfb_cnt = win_cnt = win_grp_cnt = coeff_cnt = last_sfb = acc_win_cnt = 0;
        cur_sfb_len = sfb_offset[1] / window_group_length[0];
        cur_cb = coderInfo->book_vector[0];
        for (i = 0; i < num_cw; i++) {
            /* Set codeword info parameters */
            cw_info[i].cb = cur_cb;
            cw_info[i].num_sl_cw = (cur_cb < FIRST_PAIR_HCB) ? QUAD_LEN : PAIR_LEN;

            cw_info[i].window = acc_win_cnt + win_cnt;
            cw_info[i].cw_nr = window_cw_cnt[cw_info[i].window];
            window_cw_cnt[cw_info[i].window]++;

            coeff_cnt += cw_info[i].num_sl_cw;
            if (coeff_cnt - last_sfb >= cur_sfb_len) {
                last_sfb += cur_sfb_len;

                win_cnt++; /* next window */
                if (win_cnt == window_group_length[win_grp_cnt]) {
                    win_cnt = 0;

                    sfb_cnt++; /* next sfb */
                    if (sfb_cnt == coderInfo->all_sfb) {
                        sfb_cnt = 0;

                        acc_win_cnt += window_group_length[win_grp_cnt];
                        win_grp_cnt++; /* next window group */
                    }

                    /* new codebook and sfb length */
                    cur_cb = coderInfo->book_vector[sfb_cnt];
                    if (last_sfb < FRAME_LEN) {
                        cur_sfb_len = (sfb_offset[sfb_cnt + 1] - sfb_offset[sfb_cnt])
                            / window_group_length[win_grp_cnt];
                    }
                }
            }
        }

        /* presorting (first presorting step) */
        /* only needed for short windows */

/* Somehow the second presorting step does not give expected results. Disabling the
   following code surprisingly gives good results. TODO: find the bug */
        if (0) {//coderInfo->block_type == ONLY_SHORT_WINDOW) {
            for (i = 0; i < MAX_SHORT_WINDOWS; i++)
                window_cw_cnt[i] = 0; /* reset all counters */

            win_cnt = 0;
            cw_cnt = 0;
            for (i = 0; i < num_cw; i++) {
                for (j = 0; j < num_cw; j++) {
                    if (cw_info[j].window == win_cnt) {
                        if (cw_info[j].cw_nr == window_cw_cnt[win_cnt]) {
                            cw_info_preso[cw_cnt++] = cw_info[j];
                            window_cw_cnt[win_cnt]++;

                            /* check if two one-dimensional codewords */
                            if (cw_info[j].num_sl_cw == PAIR_LEN) {
                                cw_info_preso[cw_cnt++] = cw_info[j + 1];
                                window_cw_cnt[win_cnt]++;
                            }

                            win_cnt++; /* next window */
                            if (win_cnt == MAX_SHORT_WINDOWS)
                                win_cnt = 0;
                        }
                    }
                }
            }
        } else {
            for (i = 0; i < num_cw; i++) {
                cw_info_preso[i] = cw_info[i]; /* just copy */
            }
        }

        /* presorting (second presorting step) */
        cw_cnt = 0;
        for (presort = 0; presort < LEN_PRESORT_CODEBOOK; presort++) {
            /* next codebook that has to be processed according to presorting */
            unsigned short nextCB = PresortedCodebook_VCB11[presort];

            for (i = 0; i < num_cw; i++) {
                /* process only codewords that are due now */
                if ((cw_info_preso[i].cb == nextCB) ||
                    ((nextCB < ESC_HCB) && (cw_info_preso[i].cb == nextCB + 1)))
                {
                    cw_info[cw_cnt++] = cw_info_preso[i];
                }
            }
        }

        /* init segments */
        accsegmsize = 0;
        for (i = 0; i < num_cw; i++) {
            /* 8.5.3.3.3.2 Derivation of segment width */
            cursegmsize = min(maxCwLen[cw_info[i].cb], coderInfo->iLenLongestCW);

            if (accsegmsize + cursegmsize > coderInfo->iLenReordSpData) {
                /* the last segment is extended until iLenReordSpData */
                segment[segmcnt - 1].right = coderInfo->iLenReordSpData - 1;
                segment[segmcnt - 1].len = coderInfo->iLenReordSpData - segment[segmcnt - 1].left;
                break;
            }

            segment[segmcnt].left = accsegmsize;
            segment[segmcnt].right = accsegmsize + cursegmsize - 1;
            segment[segmcnt++].len = cursegmsize;
            accsegmsize += cursegmsize;
        }

        /* store current bit position */
        startbitpos = bitStream->currentBit;

        /* write write priority codewords (PCWs) and nonPCWs ---------------- */
        num_sets = num_cw / segmcnt; /* number of sets */

        for (set = 0; set <= num_sets; set++) {
            int trial;

            /* ever second set the bit order is reversed */
            is_backwards = set % 2;

            for (trial = 0; trial < segmcnt; trial++) {
                int codewordBase;
                int set_encoded = segmcnt;

                if (set == num_sets)
                    set_encoded = num_cw - set * segmcnt; /* last set is shorter than the rest */

                for (codewordBase = 0; codewordBase < segmcnt; codewordBase++) {
                    int segment_index = (trial + codewordBase) % segmcnt;
                    int codeword_index = codewordBase + set * segmcnt;

                    if (codeword_index >= num_cw)
                        break;

                    if ((cw_info[codeword_index].cw_len > 0) && (segment[segment_index].len > 0)) {
                        /* codeword is not yet written (completely) */
                        /* space left in this segment */
                        short tmplen;

                        /* how many bits can be written? */
                        if (segment[segment_index].len >= cw_info[codeword_index].cw_len) {
                            tmplen = cw_info[codeword_index].cw_len;
                            set_encoded--; /* CW fits into segment */
                        } else {
                            tmplen = segment[segment_index].len;
                        }

                        /* Adjust lengths */
                        cw_info[codeword_index].cw_len -= tmplen;
                        segment[segment_index].len -= tmplen;

                        /* write codewords to bitstream */
                        for (cw_part_cnt = 0; cw_part_cnt < cw_info[codeword_index].num_data; cw_part_cnt++) {
                            cur_cw_part = cw_info[codeword_index].cw_offset + cw_part_cnt;

                            if (len[cur_cw_part] <= tmplen) {
                                /* write complete data, no partitioning */
                                if (is_backwards) {
                                    /* write data in reversed bit-order */
                                    PutBitHcr(bitStream, startbitpos + segment[segment_index].right - len[cur_cw_part] + 1,
                                        rewind_word(data[cur_cw_part], len[cur_cw_part]), len[cur_cw_part]);

                                    segment[segment_index].right -= len[cur_cw_part];
                                } else {
                                    PutBitHcr(bitStream, startbitpos + segment[segment_index].left,
                                        data[cur_cw_part], len[cur_cw_part]);

                                    segment[segment_index].left += len[cur_cw_part];
                                }

                                tmplen -= len[cur_cw_part];
                                len[cur_cw_part] = 0;
                            } else {
                                /* codeword part must be partitioned */
                                /* data must be taken from the left side */
                                tmp_data = data[cur_cw_part];

                                diff = len[cur_cw_part] - tmplen;
                                tmp_data >>= diff;

                                /* remove bits which are already used */
                                data[cur_cw_part] &= (1 << diff) - 1 /* diff number of ones */;
                                len[cur_cw_part] = diff;

                                if (is_backwards) {
                                    /* write data in reversed bit-order */
                                    PutBitHcr(bitStream, startbitpos + segment[segment_index].right - tmplen + 1,
                                        rewind_word(tmp_data, tmplen), tmplen);

                                    segment[segment_index].right -= tmplen;
                                } else {
                                    PutBitHcr(bitStream, startbitpos + segment[segment_index].left,
                                        tmp_data, tmplen);

                                    segment[segment_index].left += tmplen;
                                }

                                tmplen = 0;
                            }
                            
                            if (tmplen == 0)
                                break; /* all data written for this segment trial */
                        }
                    }
                } /* of codewordBase */

                if (set_encoded == 0)
                    break; /* no unencoded codewords left in this set */
            } /* of trial */
        }

        /* set parameter for bit stream to current correct position */
        bitStream->currentBit = startbitpos + coderInfo->iLenReordSpData;
        bitStream->numBit = bitStream->currentBit;
    }

    return coderInfo->iLenReordSpData;
}

/*
    CRC8 x^8 + x^4 + x^3 + x^2 + 1
*/
static const unsigned char _crctable[256] =
{
    0x00, 0x1D, 0x3A, 0x27, 0x74, 0x69, 0x4E, 0x53,
    0xE8, 0xF5, 0xD2, 0xCF, 0x9C, 0x81, 0xA6, 0xBB,
    0xCD, 0xD0, 0xF7, 0xEA, 0xB9, 0xA4, 0x83, 0x9E,
    0x25, 0x38, 0x1F, 0x02, 0x51, 0x4C, 0x6B, 0x76,
    0x87, 0x9A, 0xBD, 0xA0, 0xF3, 0xEE, 0xC9, 0xD4,
    0x6F, 0x72, 0x55, 0x48, 0x1B, 0x06, 0x21, 0x3C,
    0x4A, 0x57, 0x70, 0x6D, 0x3E, 0x23, 0x04, 0x19,
    0xA2, 0xBF, 0x98, 0x85, 0xD6, 0xCB, 0xEC, 0xF1,
    0x13, 0x0E, 0x29, 0x34, 0x67, 0x7A, 0x5D, 0x40,
    0xFB, 0xE6, 0xC1, 0xDC, 0x8F, 0x92, 0xB5, 0xA8,
    0xDE, 0xC3, 0xE4, 0xF9, 0xAA, 0xB7, 0x90, 0x8D,
    0x36, 0x2B, 0x0C, 0x11, 0x42, 0x5F, 0x78, 0x65,
    0x94, 0x89, 0xAE, 0xB3, 0xE0, 0xFD, 0xDA, 0xC7,
    0x7C, 0x61, 0x46, 0x5B, 0x08, 0x15, 0x32, 0x2F,
    0x59, 0x44, 0x63, 0x7E, 0x2D, 0x30, 0x17, 0x0A,
    0xB1, 0xAC, 0x8B, 0x96, 0xC5, 0xD8, 0xFF, 0xE2,
    0x26, 0x3B, 0x1C, 0x01, 0x52, 0x4F, 0x68, 0x75,
    0xCE, 0xD3, 0xF4, 0xE9, 0xBA, 0xA7, 0x80, 0x9D,
    0xEB, 0xF6, 0xD1, 0xCC, 0x9F, 0x82, 0xA5, 0xB8,
    0x03, 0x1E, 0x39, 0x24, 0x77, 0x6A, 0x4D, 0x50,
    0xA1, 0xBC, 0x9B, 0x86, 0xD5, 0xC8, 0xEF, 0xF2,
    0x49, 0x54, 0x73, 0x6E, 0x3D, 0x20, 0x07, 0x1A,
    0x6C, 0x71, 0x56, 0x4B, 0x18, 0x05, 0x22, 0x3F,
    0x84, 0x99, 0xBE, 0xA3, 0xF0, 0xED, 0xCA, 0xD7,
    0x35, 0x28, 0x0F, 0x12, 0x41, 0x5C, 0x7B, 0x66,
    0xDD, 0xC0, 0xE7, 0xFA, 0xA9, 0xB4, 0x93, 0x8E,
    0xF8, 0xE5, 0xC2, 0xDF, 0x8C, 0x91, 0xB6, 0xAB,
    0x10, 0x0D, 0x2A, 0x37, 0x64, 0x79, 0x5E, 0x43,
    0xB2, 0xAF, 0x88, 0x95, 0xC6, 0xDB, 0xFC, 0xE1,
    0x5A, 0x47, 0x60, 0x7D, 0x2E, 0x33, 0x14, 0x09,
    0x7F, 0x62, 0x45, 0x58, 0x0B, 0x16, 0x31, 0x2C,
    0x97, 0x8A, 0xAD, 0xB0, 0xE3, 0xFE, 0xD9, 0xC4
};

static void calc_CRC(BitStream *bitStream, int len)
{
    //int i;
    //unsigned char r = ~0;  /* Initialize to all ones */
    unsigned char crc = ~0;  /* Initialize to all ones */
    
    /* CRC polynome used x^8 + x^4 + x^3 + x^2 +1 */

    unsigned int cb         = len / 8;
    unsigned int taillen    = len & 0x7;
    unsigned char* pb       = &bitStream->data[1];
    //compatible, but slower unsigned char b         = ( bitStream->data[cb + 1] ) >> ( 8 - taillen );
    unsigned char b         = bitStream->data[cb + 1];
    
//#define GPOLY 0435
//
//    for (i = 8; i < len + 8; i++) {
//        r = ( (r << 1) ^ (( (
//            ( bitStream->data[i / 8] >> (7 - (i % 8)) )
//            & 1) ^ ((r >> 7) & 1)) * GPOLY )) & 0xFF;
//    }

#define GP 0x1d

//fprintf( stderr, "\nfaac:" );

    while ( cb-- )
    {
//fprintf( stderr, " %02X", *pb );
        crc = _crctable[ crc ^ *pb++ ];
    }
    
    //compatible, but slower switch ( taillen )
    //{
    //case 7:
    //    crc = ( ( crc << 1 ) ^ ( ( ( ( b >> 6 ) & 1 ) ^ ( ( crc >> 7 ) & 1 ) ) * GP ) ) & 0xFF;
    //    // goto next case
    //case 6:
    //    crc = ( ( crc << 1 ) ^ ( ( ( ( b >> 5 ) & 1 ) ^ ( ( crc >> 7 ) & 1 ) ) * GP ) ) & 0xFF;
    //    // goto next case
    //case 5:
    //    crc = ( ( crc << 1 ) ^ ( ( ( ( b >> 4 ) & 1 ) ^ ( ( crc >> 7 ) & 1 ) ) * GP ) ) & 0xFF;
    //    // goto next case
    //case 4:
    //    crc = ( ( crc << 1 ) ^ ( ( ( ( b >> 3 ) & 1 ) ^ ( ( crc >> 7 ) & 1 ) ) * GP ) ) & 0xFF;
    //    // goto next case
    //case 3:
    //    crc = ( ( crc << 1 ) ^ ( ( ( ( b >> 2 ) & 1 ) ^ ( ( crc >> 7 ) & 1 ) ) * GP ) ) & 0xFF;
    //    // goto next case
    //case 2:
    //    crc = ( ( crc << 1 ) ^ ( ( ( ( b >> 1 ) & 1 ) ^ ( ( crc >> 7 ) & 1 ) ) * GP ) ) & 0xFF;
    //    // goto next case
    //case 1:
    //    crc = ( ( crc << 1 ) ^ ( ( ( b & 1 ) ^ ( ( crc >> 7 ) & 1 ) ) * GP ) ) & 0xFF;
    //    break;
    //}
//fprintf( stderr, " %02X", ( b >> ( 8 - taillen ) ) << 7 );
    switch ( taillen )
    {
    case 7:
        crc = ( ( crc << 1 ) ^ ( ( (signed char)( b ^ crc ) >> 7 ) & GP ) ) & 0xFF;
        b <<= 1;
        // goto next case
    case 6:
        crc = ( ( crc << 1 ) ^ ( ( (signed char)( b ^ crc ) >> 7 ) & GP ) ) & 0xFF;
        b <<= 1;
        // goto next case
    case 5:
        crc = ( ( crc << 1 ) ^ ( ( (signed char)( b ^ crc ) >> 7 ) & GP ) ) & 0xFF;
        b <<= 1;
        // goto next case
    case 4:
        crc = ( ( crc << 1 ) ^ ( ( (signed char)( b ^ crc ) >> 7 ) & GP ) ) & 0xFF;
        b <<= 1;
        // goto next case
    case 3:
        crc = ( ( crc << 1 ) ^ ( ( (signed char)( b ^ crc ) >> 7 ) & GP ) ) & 0xFF;
        b <<= 1;
        // goto next case
    case 2:
        crc = ( ( crc << 1 ) ^ ( ( (signed char)( b ^ crc ) >> 7 ) & GP ) ) & 0xFF;
        b <<= 1;
        // goto next case
    case 1:
        crc = ( ( crc << 1 ) ^ ( ( (signed char)( b ^ crc ) >> 7 ) & GP ) ) & 0xFF;
        break;
    }

    //if ( crc != r )
    //{
    //    fprintf( stderr, "%08X != %08X\n", crc, r );
    //}
//fprintf( stderr, " (%5d bits), CRC is %02X\n", len, ~crc & 0xFF );
    
    /* CRC is stored inverted, per definition at first byte in stream */
    bitStream->data[0] = ~crc;
}
#endif

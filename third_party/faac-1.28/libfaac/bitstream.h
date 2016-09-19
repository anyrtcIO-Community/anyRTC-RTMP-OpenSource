/**********************************************************************
MPEG-4 Audio VM
Bit stream module



This software module was originally developed by

Heiko Purnhagen (University of Hannover)

and edited by

in the course of development of the MPEG-2 NBC/MPEG-4 Audio standard
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

Copyright (c) 1996.
**********************************************************************/
/*
 * $Id: bitstream.h,v 1.14 2004/07/04 12:10:52 corrados Exp $
 */

#ifndef BITSTREAM_H
#define BITSTREAM_H

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#include "frame.h"
#include "coder.h"
#include "channels.h"

/*
 * Raw bitstream constants
 */
#define LEN_SE_ID 3
#define LEN_TAG 4
#define LEN_GLOB_GAIN 8
#define LEN_COM_WIN 1
#define LEN_ICS_RESERV 1
#define LEN_WIN_SEQ 2
#define LEN_WIN_SH 1
#define LEN_MAX_SFBL 6
#define LEN_MAX_SFBS 4
#define LEN_CB 4
#define LEN_SCL_PCM 8
#define LEN_PRED_PRES 1
#define LEN_PRED_RST 1
#define LEN_PRED_RSTGRP 5
#define LEN_PRED_ENAB 1
#define LEN_MASK_PRES 2
#define LEN_MASK 1
#define LEN_PULSE_PRES 1

#define LEN_TNS_PRES 1
#define LEN_TNS_NFILTL 2
#define LEN_TNS_NFILTS 1
#define LEN_TNS_COEFF_RES 1
#define LEN_TNS_LENGTHL 6
#define LEN_TNS_LENGTHS 4
#define LEN_TNS_ORDERL 5
#define LEN_TNS_ORDERS 3
#define LEN_TNS_DIRECTION 1
#define LEN_TNS_COMPRESS 1
#define LEN_GAIN_PRES 1

#define LEN_NEC_NPULSE 2
#define LEN_NEC_ST_SFB 6
#define LEN_NEC_POFF 5
#define LEN_NEC_PAMP 4
#define NUM_NEC_LINES 4
#define NEC_OFFSET_AMP 4

#define LEN_NCC 3
#define LEN_IS_CPE 1
#define LEN_CC_LR 1
#define LEN_CC_DOM 1
#define LEN_CC_SGN 1
#define LEN_CCH_GES 2
#define LEN_CCH_CGP 1
#define LEN_D_CNT 4
#define LEN_D_ESC 12
#define LEN_F_CNT 4
#define LEN_F_ESC 8
#define LEN_BYTE 8
#define LEN_PAD_DATA 8

#define LEN_PC_COMM 8

#ifdef DRM
# define LEN_HCR_REORDSD 14
# define LEN_HCR_LONGCW 6
# define FIRST_PAIR_HCB 5
# define QUAD_LEN 4
# define PAIR_LEN 2
# define ESC_HCB 11
#endif

#define ID_SCE 0
#define ID_CPE 1
#define ID_CCE 2
#define ID_LFE 3
#define ID_DSE 4
#define ID_PCE 5
#define ID_FIL 6
#define ID_END 7


/* MPEG ID's */
#define MPEG2 1
#define MPEG4 0

/* AAC object types */
#define MAIN 1
#define LOW  2
#define SSR  3
#define LTP  4


#define BYTE_NUMBIT 8       /* bits in byte (char) */
#define LONG_NUMBIT 32      /* bits in unsigned long */
#define bit2byte(a) (((a)+BYTE_NUMBIT-1)/BYTE_NUMBIT)


typedef struct
{
  unsigned char *data;      /* data bits */
  long numBit;          /* number of bits in buffer */
  long size;            /* buffer size in bytes */
  long currentBit;      /* current bit position in bit stream */
  long numByte;         /* number of bytes read/written (only file) */
} BitStream;



int WriteBitstream(faacEncHandle hEncoder,
                   CoderInfo *coderInfo,
                   ChannelInfo *channelInfo,
                   BitStream *bitStream,
                   int numChannels);


BitStream *OpenBitStream(int size, unsigned char *buffer);

int CloseBitStream(BitStream *bitStream);

int PutBit(BitStream *bitStream,
           unsigned long data,
           int numBit);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* BITSTREAM_H */


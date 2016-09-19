/***********

This software module was originally developed by Dolby
Laboratories in the course of development of the MPEG-2 AAC/MPEG-4
Audio standard ISO/IEC13818-7, 14496-1, 2 and 3. This software module is an implementation of a part
of one or more MPEG-2 AAC/MPEG-4 Audio tools as specified by the
MPEG-2 aac/MPEG-4 Audio standard. ISO/IEC  gives users of the
MPEG-2aac/MPEG-4 Audio standards free license to this software module
or modifications thereof for use in hardware or software products
claiming conformance to the MPEG-2 aac/MPEG-4 Audio  standards. Those
intending to use this software module in hardware or software products
are advised that this use may infringe existing patents. The original
developer of this software module, the subsequent
editors and their companies, and ISO/IEC have no liability for use of
this software module or modifications thereof in an
implementation. Copyright is not released for non MPEG-2 aac/MPEG-4
Audio conforming products. The original developer retains full right to
use the code for the developer's own purpose, assign or donate the code to a
third party and to inhibit third party from using the code for non
MPEG-2 aac/MPEG-4 Audio conforming products. This copyright notice
must be included in all copies or derivative works. Copyright 1996.

***********/
/*
 * $Id: huffman.h,v 1.6 2004/07/12 08:46:43 corrados Exp $
 */

#ifndef HUFFMAN_H
#define HUFFMAN_H

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#include "bitstream.h"
#include "coder.h"

/* Huffman tables */
#define MAXINDEX 289
#define NUMINTAB 2
#define FIRSTINTAB 0
#define LASTINTAB 1

#define INTENSITY_HCB 15
#define INTENSITY_HCB2 14


#define ABS(A) ((A) < 0 ? (-A) : (A))

#include "frame.h"

void HuffmanInit(CoderInfo *coderInfo, unsigned int numChannels);
void HuffmanEnd(CoderInfo *coderInfo, unsigned int numChannels);

int BitSearch(CoderInfo *coderInfo,
              int *quant);

int NoiselessBitCount(CoderInfo *coderInfo,
                      int *quant,
                      int hop,
                      int min_book_choice[112][3]);

static int CalculateEscSequence(int input, int *len_esc_sequence);

int CalcBits(CoderInfo *coderInfo,
             int book,
             int *quant,
             int offset,
             int length);

int OutputBits(CoderInfo *coderInfo,
#ifdef DRM
               int *book, /* we need to change book for VCB11 */
#else
               int book,
#endif
               int *quant,
               int offset,
               int length);

int SortBookNumbers(CoderInfo *coderInfo,
                    BitStream *bitStream,
                    int writeFlag);

int WriteScalefactors(CoderInfo *coderInfo,
                      BitStream *bitStream,
                      int writeFlag);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* HUFFMAN_H */

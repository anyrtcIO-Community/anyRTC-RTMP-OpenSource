/***********

This software module was originally developed by Dolby
Laboratories and edited by Sony Corporation
in the course of development of the MPEG-2 NBC/MPEG-4
Audio standard ISO/IEC13818-7, 14496-1, 2 and 3. This software module is an
implementation of a part of one or more MPEG-2 NBC/MPEG-4 Audio tools as
specified by the MPEG-2 NBC/MPEG-4 Audio standard. ISO/IEC  gives users of the
MPEG-2NBC/MPEG-4 Audio standards free license to this software module
or modifications thereof for use in hardware or software products
claiming conformance to the MPEG-2 NBC/MPEG-4 Audio  standards. Those
intending to use this software module in hardware or software products
are advised that this use may infringe existing patents. The original
developer of this software module, the subsequent
editors and their companies, and ISO/IEC have no liability for use of
this software module or modifications thereof in an
implementation. Copyright is not released for non MPEG-2 NBC/MPEG-4
Audio conforming products. The original developer retains full right to
use the code for the developer's own purpose, assign or donate the code to a
third party and to inhibit third party from using the code for non
MPEG-2 NBC/MPEG-4 Audio conforming products. This copyright notice
must be included in all copies or derivative works. Copyright 1996.

***********/
/*
 * $Id: huffman.c,v 1.11 2005/02/02 07:53:20 sur Exp $
 */

#include <math.h>
#include <stdlib.h>

#include "huffman.h"
#include "coder.h"
#include "bitstream.h"
#include "util.h"

#include "hufftab.h"

void HuffmanInit(CoderInfo *coderInfo, unsigned int numChannels)
{
    unsigned int channel;

    for (channel = 0; channel < numChannels; channel++) {
        coderInfo[channel].data = (int*)AllocMemory(5*FRAME_LEN*sizeof(int));
        coderInfo[channel].len = (int*)AllocMemory(5*FRAME_LEN*sizeof(int));

#ifdef DRM
        coderInfo[channel].num_data_cw = (int*)AllocMemory(FRAME_LEN*sizeof(int));
#endif
    }
}

void HuffmanEnd(CoderInfo *coderInfo, unsigned int numChannels)
{
    unsigned int channel;

    for (channel = 0; channel < numChannels; channel++) {
        if (coderInfo[channel].data) FreeMemory(coderInfo[channel].data);
        if (coderInfo[channel].len) FreeMemory(coderInfo[channel].len);

#ifdef DRM
        if (coderInfo[channel].num_data_cw) FreeMemory(coderInfo[channel].num_data_cw);
#endif
    }
}

int BitSearch(CoderInfo *coderInfo,
              int *quant)  /* Quantized spectral values */
  /*
  This function inputs a vector of quantized spectral data, quant[][], and returns a vector,
  'book_vector[]' that describes how to group together the scalefactor bands into a smaller
  number of sections.  There are MAX_SCFAC_BANDS elements in book_vector (equal to 49 in the
  case of long blocks and 112 for short blocks), and each element has a huffman codebook
  number assigned to it.

  For a quick and simple algorithm, this function performs a binary
  search across the sfb's (scale factor bands).  On the first approach, it calculates the
  needed amount of bits if every sfb were its own section and transmitted its own huffman
  codebook value side information (equal to 9 bits for a long block, 7 for a short).  The
  next iteration combines adjacent sfb's, and calculates the bit rate for length two sfb
  sections.  If any wider two-sfb section requires fewer bits than the sum of the two
  single-sfb sections (below it in the binary tree), then the wider section will be chosen.
  This process occurs until the sections are split into three uniform parts, each with an
  equal amount of sfb's contained.

  The binary tree is stored as a two-dimensional array.  Since this tree is not full, (there
  are only 49 nodes, not 2^6 = 64), the numbering is a little complicated.  If the tree were
  full, the top node would be 1.  It's children would be 2 and 3.  But, since this tree
  is not full, the top row of three nodes are numbered {4,5,6}.  The row below it is
  {8,9,10,11,12,13}, and so on.

  The binary tree is called bit_stats[112][3].  There are 112 total nodes (some are not
  used since it's not full).  bit_stats[x][0] holds the bit totals needed for the sfb sectioning
  strategy represented by the node x in the tree.  bit_stats[x][1] holds the optimal huffman
  codebook table that minimizes the bit rate, given the sectioning boundaries dictated by node x.
*/

{
    int i,j,k;
    int hop;
    int min_book_choice[112][3];
    int bit_stats[240][3];
    int total_bit_count;
    int levels;
    int pow2levels;
    int fracpow2lev;

    /* Set local pointer to coderInfo book_vector */
    int* book_vector = coderInfo -> book_vector;

    levels = (int) ((log((double)coderInfo->nr_of_sfb)/log((double)2.0))+1);

/* #define SLOW */

#ifdef SLOW
    for(i = 0; i < 5; i++) {
#else
        i = 0;
#endif
        hop = 1 << i;

        NoiselessBitCount(coderInfo, quant, hop, min_book_choice);

        /* load up the (not-full) binary search tree with the min_book_choice values */
        k=0;
        total_bit_count = 0;

	pow2levels = 1 << (levels - i);
	fracpow2lev = pow2levels + (coderInfo->nr_of_sfb >> i);

        for (j=pow2levels; j < fracpow2lev; j++)
        {
            bit_stats[j][0] = min_book_choice[k][0]; /* the minimum bit cost for this section */
            bit_stats[j][1] = min_book_choice[k][1]; /* used with this huffman book number */

#ifdef SLOW
            if (i>0){  /* not on the lowest level, grouping more than one signle scalefactor band per section*/
                if  (bit_stats[j][0] < bit_stats[2*j][0] + bit_stats[2*j+1][0]){

                    /* it is cheaper to combine surrounding sfb secionts into one larger huffman book section */
                    for(n=k;n<k+hop;n++) { /* write the optimal huffman book value for the new larger section */
                        if ( (book_vector[n]!=INTENSITY_HCB)&&(book_vector[n]!=INTENSITY_HCB2) ) { /* Don't merge with IS bands */
                            book_vector[n] = bit_stats[j][1];
                        }
                    }
                } else {  /* it was cheaper to transmit the smaller huffman table sections */
                    bit_stats[j][0] = bit_stats[2*j][0] + bit_stats[2*j+1][0];
                }
	    } else
#endif
	    {  /* during the first stage of the iteration, all sfb's are individual sections */
                if ( (book_vector[k]!=INTENSITY_HCB)&&(book_vector[k]!=INTENSITY_HCB2) ) {
                    book_vector[k] = bit_stats[j][1];  /* initially, set all sfb's to their own optimal section table values */
                }
            }
            total_bit_count = total_bit_count +  bit_stats[j][0];
            k=k+hop;
        }
#ifdef SLOW
    }
#endif
    /*   book_vector[k] = book_vector[k-1]; */
    return(total_bit_count);
}


int NoiselessBitCount(CoderInfo *coderInfo,
                      int *quant,
                      int hop,
                      int min_book_choice[112][3])
{
  int i,j,k;

  /*
     This function inputs:
     - the quantized spectral data, 'quant[]';
     - all of the huffman codebooks, 'huff[][]';
     - the size of the sections, in scalefactor bands (SFB's), 'hop';
     - an empty matrix, min_book_choice[][] passed to it;

     This function outputs:
     - the matrix, min_book_choice.  It is a two dimensional matrix, with its
     rows corresponding to spectral sections.  The 0th column corresponds to
     the bits needed to code a section with 'hop' scalefactors bands wide, all using
     the same huffman codebook.  The 1st column contains the huffman codebook number
     that allows the minimum number of bits to be used.

     Other notes:
     - Initally, the dynamic range is calculated for each spectral section.  The section
     can only be entropy coded with books that have an equal or greater dynamic range
     than the section's spectral data.  The exception to this is for the 11th ESC codebook.
     If the dynamic range is larger than 16, then an escape code is appended after the
     table 11 codeword which encodes the larger value explicity in a pseudo-non-uniform
     quantization method.

     */

    int max_sb_coeff;
    int book_choice[12][2];
    int total_bits_cost = 0;
    int offset, length, end;
    int q;

    /* set local pointer to sfb_offset */
    int *sfb_offset = coderInfo->sfb_offset;
    int nr_of_sfb = coderInfo->nr_of_sfb;

    /* each section is 'hop' scalefactor bands wide */
    for (i=0; i < nr_of_sfb; i=i+hop){
#ifdef SLOW
        if ((i+hop) > nr_of_sfb)
            q = nr_of_sfb;
        else
#endif
            q = i+hop;

        {

            /* find the maximum absolute value in the current spectral section, to see what tables are available to use */
            max_sb_coeff = 0;
            for (j=sfb_offset[i]; j<sfb_offset[q]; j++){  /* snl */
                if (ABS(quant[j]) > max_sb_coeff)
                    max_sb_coeff = ABS(quant[j]);
            }

            j = 0;
            offset = sfb_offset[i];
#ifdef SLOW
            if ((i+hop) > nr_of_sfb){
                end = sfb_offset[nr_of_sfb];
            } else
#endif
                end = sfb_offset[q];
            length = end - offset;

            /* all spectral coefficients in this section are zero */
            if (max_sb_coeff == 0) {
                book_choice[j][0] = CalcBits(coderInfo,0,quant,offset,length);
                book_choice[j++][1] = 0;

            }
            else {  /* if the section does have non-zero coefficients */
                if(max_sb_coeff < 2){
                    book_choice[j][0] = CalcBits(coderInfo,1,quant,offset,length);
                    book_choice[j++][1] = 1;
                    book_choice[j][0] = CalcBits(coderInfo,2,quant,offset,length);
                    book_choice[j++][1] = 2;
                    book_choice[j][0] = CalcBits(coderInfo,3,quant,offset,length);
                    book_choice[j++][1] = 3;
                }
                else if (max_sb_coeff < 3){
                    book_choice[j][0] = CalcBits(coderInfo,3,quant,offset,length);
                    book_choice[j++][1] = 3;
                    book_choice[j][0] = CalcBits(coderInfo,4,quant,offset,length);
                    book_choice[j++][1] = 4;
                    book_choice[j][0] = CalcBits(coderInfo,5,quant,offset,length);
                    book_choice[j++][1] = 5;
                }
                else if (max_sb_coeff < 5){
                    book_choice[j][0] = CalcBits(coderInfo,5,quant,offset,length);
                    book_choice[j++][1] = 5;
                    book_choice[j][0] = CalcBits(coderInfo,6,quant,offset,length);
                    book_choice[j++][1] = 6;
                    book_choice[j][0] = CalcBits(coderInfo,7,quant,offset,length);
                    book_choice[j++][1] = 7;
                }
                else if (max_sb_coeff < 8){
                    book_choice[j][0] = CalcBits(coderInfo,7,quant,offset,length);
                    book_choice[j++][1] = 7;
                    book_choice[j][0] = CalcBits(coderInfo,8,quant,offset,length);
                    book_choice[j++][1] = 8;
                    book_choice[j][0] = CalcBits(coderInfo,9,quant,offset,length);
                    book_choice[j++][1] = 9;
                }
                else if (max_sb_coeff < 13){
                    book_choice[j][0] = CalcBits(coderInfo,9,quant,offset,length);
                    book_choice[j++][1] = 9;
                    book_choice[j][0] = CalcBits(coderInfo,10,quant,offset,length);
                    book_choice[j++][1] = 10;
                }
                /* (max_sb_coeff >= 13), choose table 11 */
                else {
                    book_choice[j][0] = CalcBits(coderInfo,11,quant,offset,length);
                    book_choice[j++][1] = 11;
                }
            }

            /* find the minimum bit cost and table number for huffman coding this scalefactor section */
            min_book_choice[i][1] = book_choice[0][1];
            min_book_choice[i][0] = book_choice[0][0];

            for(k=1;k<j;k++){
                if (book_choice[k][0] < min_book_choice[i][0]){
                    min_book_choice[i][1] = book_choice[k][1];
                    min_book_choice[i][0] = book_choice[k][0];
                }
            }
            total_bits_cost += min_book_choice[i][0];
        }
    }
    return(total_bits_cost);
}



static int CalculateEscSequence(int input, int *len_esc_sequence)
/*
   This function takes an element that is larger than 16 and generates the base10 value of the
   equivalent escape sequence.  It returns the escape sequence in the variable, 'output'.  It
   also passed the length of the escape sequence through the parameter, 'len_esc_sequence'.
*/

{
    float x,y;
    int output;
    int N;

    N = -1;
    y = (float)ABS(input);
    x = y / 16;

    while (x >= 1) {
        N++;
        x = x/2;
    }

    *len_esc_sequence = 2*N + 5;  /* the length of the escape sequence in bits */

    output = (int)((pow(2,N) - 1)*pow(2,N+5) + y - pow(2,N+4));
    return(output);
}

int OutputBits(CoderInfo *coderInfo,
#ifdef DRM
               int *book, /* we need to change book for VCB11 */
#else
               int book,
#endif
               int *quant,
               int offset,
               int length)
{
  /*
     This function inputs
     - a specific codebook number, 'book'
     - the quantized spectral data, 'quant[][]'
     - the offset into the spectral data to begin scanning, 'offset'
     - the 'length' of the segment to huffman code
     -> therefore, the segment quant[offset] to quant[offset+length-1]
     is huffman coded.

     This function outputs
     - the number of bits required, 'bits'  using the prescribed codebook, book applied to
     the given segment of spectral data.

     There are three parameters that are passed back and forth into this function.  data[]
     and len[] are one-dimensional arrays that store the codebook values and their respective
     bit lengths.  These are used when packing the data for the bitstream in OutputBits().  The
     index into these arrays is 'coderInfo->spectral_count''.  It gets incremented internally in this
     function as counter, then passed to the outside through outside_counter.  The next time
     OutputBits() is called, counter starts at the value it left off from the previous call.

   */
    int esc_sequence;
    int len_esc;
    int index;
    int bits=0;
    int tmp;
    int codebook,i,j;
    int counter;

    /* Set up local pointers to coderInfo elements data and len */
    int* data=      coderInfo->data;
    int* len=       coderInfo->len;
#ifdef DRM
    int* num_data = coderInfo->num_data_cw;
    int cur_cw_len;
    int max_esc_sequ = 0;
#endif

    counter = coderInfo->spectral_count;

#ifdef DRM
    switch (*book) {
#else
    switch (book) {
#endif
    case 0:
    case INTENSITY_HCB2:
    case INTENSITY_HCB:
#ifdef DRM
        for(i=offset;i<offset+length;i=i+4){
#endif
        /* This case also applies to intensity stereo encoding */
        coderInfo->data[counter] = 0;
        coderInfo->len[counter++] = 0;
        coderInfo->spectral_count = counter;  /* send the current count back to the outside world */

#ifdef DRM
        num_data[coderInfo->cur_cw++] = 1;
        }
#endif
        return(bits);
    case 1:
        for(i=offset;i<offset+length;i=i+4){
            index = 27*quant[i] + 9*quant[i+1] + 3*quant[i+2] + quant[i+3] + 40;
            codebook = huff1[index][LASTINTAB];
            tmp = huff1[index][FIRSTINTAB];
            bits += tmp;
            data[counter] = codebook;
            len[counter++] = tmp;
#ifdef DRM
            num_data[coderInfo->cur_cw++] = 1;
            coderInfo->iLenReordSpData += tmp;
            if (tmp > coderInfo->iLenLongestCW)
                coderInfo->iLenLongestCW = tmp;
#endif
        }
        coderInfo->spectral_count = counter;  /* send the current count back to the outside world */
        return(bits);
    case 2:
        for(i=offset;i<offset+length;i=i+4){
            index = 27*quant[i] + 9*quant[i+1] + 3*quant[i+2] + quant[i+3] + 40;
            codebook = huff2[index][LASTINTAB];
            tmp = huff2[index][FIRSTINTAB];
            bits += tmp;
            data[counter] = codebook;
            len[counter++] = tmp;
#ifdef DRM
            num_data[coderInfo->cur_cw++] = 1;
            coderInfo->iLenReordSpData += tmp;
            if (tmp > coderInfo->iLenLongestCW)
                coderInfo->iLenLongestCW = tmp;
#endif
        }
        coderInfo->spectral_count = counter;  /* send the current count back to the outside world */
        return(bits);
    case 3:
        for(i=offset;i<offset+length;i=i+4){
            index = 27*ABS(quant[i]) + 9*ABS(quant[i+1]) + 3*ABS(quant[i+2]) + ABS(quant[i+3]);
            codebook = huff3[index][LASTINTAB];
            tmp = huff3[index][FIRSTINTAB];
            bits = bits + tmp;
            data[counter] = codebook;
            len[counter++] = tmp;
#ifdef DRM
            num_data[coderInfo->cur_cw] = 1;
            cur_cw_len = tmp;
#endif
            for(j=0;j<4;j++){
                if(quant[i+j] > 0) {  /* send out '0' if a positive value */
                    data[counter] = 0;
                    len[counter++] = 1;
                    bits += 1;
#ifdef DRM
                    num_data[coderInfo->cur_cw]++;
                    cur_cw_len += 1;
#endif
                } else
                if(quant[i+j] < 0) {  /* send out '1' if a negative value */
                    data[counter] = 1;
                    len[counter++] = 1;
                    bits += 1;
#ifdef DRM
                    num_data[coderInfo->cur_cw]++;
                    cur_cw_len += 1;
#endif
                }
            }
#ifdef DRM
            coderInfo->iLenReordSpData += cur_cw_len;
            if (cur_cw_len > coderInfo->iLenLongestCW)
                coderInfo->iLenLongestCW = cur_cw_len;

            coderInfo->cur_cw++;
#endif
        }
        coderInfo->spectral_count = counter;  /* send the current count back to the outside world */
        return(bits);
    case 4:
        for(i=offset;i<offset+length;i=i+4){
            index = 27*ABS(quant[i]) + 9*ABS(quant[i+1]) + 3*ABS(quant[i+2]) + ABS(quant[i+3]);
            codebook = huff4[index][LASTINTAB];
            tmp = huff4[index][FIRSTINTAB];
            bits = bits + tmp;
            data[counter] = codebook;
            len[counter++] = tmp;
#ifdef DRM
            num_data[coderInfo->cur_cw] = 1;
            cur_cw_len = tmp;
#endif
            for(j=0;j<4;j++){
                if(quant[i+j] > 0) {  /* send out '0' if a positive value */
                    data[counter] = 0;
                    len[counter++] = 1;
                    bits += 1;
#ifdef DRM
                    num_data[coderInfo->cur_cw]++;
                    cur_cw_len += 1;
#endif
                } else
                if(quant[i+j] < 0) {  /* send out '1' if a negative value */
                    data[counter] = 1;
                    len[counter++] = 1;
                    bits += 1;
#ifdef DRM
                    num_data[coderInfo->cur_cw]++;
                    cur_cw_len += 1;
#endif
                }
            }
#ifdef DRM
            coderInfo->iLenReordSpData += cur_cw_len;
            if (cur_cw_len > coderInfo->iLenLongestCW)
                coderInfo->iLenLongestCW = cur_cw_len;

            coderInfo->cur_cw++;
#endif
        }
        coderInfo->spectral_count = counter;  /* send the current count back to the outside world */
        return(bits);
    case 5:
        for(i=offset;i<offset+length;i=i+2){
            index = 9*(quant[i]) + (quant[i+1]) + 40;
            codebook = huff5[index][LASTINTAB];
            tmp = huff5[index][FIRSTINTAB];
            bits = bits + tmp;
            data[counter] = codebook;
            len[counter++] = tmp;
#ifdef DRM
            num_data[coderInfo->cur_cw++] = 1;
            coderInfo->iLenReordSpData += tmp;
            if (tmp > coderInfo->iLenLongestCW)
                coderInfo->iLenLongestCW = tmp;
#endif
        }
        coderInfo->spectral_count = counter;  /* send the current count back to the outside world */
        return(bits);
    case 6:
        for(i=offset;i<offset+length;i=i+2){
            index = 9*(quant[i]) + (quant[i+1]) + 40;
            codebook = huff6[index][LASTINTAB];
            tmp = huff6[index][FIRSTINTAB];
            bits = bits + tmp;
            data[counter] = codebook;
            len[counter++] = tmp;
#ifdef DRM
            num_data[coderInfo->cur_cw++] = 1;
            coderInfo->iLenReordSpData += tmp;
            if (tmp > coderInfo->iLenLongestCW)
                coderInfo->iLenLongestCW = tmp;
#endif
        }
        coderInfo->spectral_count = counter;  /* send the current count back to the outside world */
        return(bits);
    case 7:
        for(i=offset;i<offset+length;i=i+2){
            index = 8*ABS(quant[i]) + ABS(quant[i+1]);
            codebook = huff7[index][LASTINTAB];
            tmp = huff7[index][FIRSTINTAB];
            bits = bits + tmp;
            data[counter] = codebook;
            len[counter++] = tmp;
#ifdef DRM
            num_data[coderInfo->cur_cw] = 1;
            cur_cw_len = tmp;
#endif
            for(j=0;j<2;j++){
                if(quant[i+j] > 0) {  /* send out '0' if a positive value */
                    data[counter] = 0;
                    len[counter++] = 1;
                    bits += 1;
#ifdef DRM
                    num_data[coderInfo->cur_cw]++;
                    cur_cw_len += 1;
#endif
                } else
                if(quant[i+j] < 0) {  /* send out '1' if a negative value */
                    data[counter] = 1;
                    len[counter++] = 1;
                    bits += 1;
#ifdef DRM
                    num_data[coderInfo->cur_cw]++;
                    cur_cw_len += 1;
#endif
                }
            }
#ifdef DRM
            coderInfo->iLenReordSpData += cur_cw_len;
            if (cur_cw_len > coderInfo->iLenLongestCW)
                coderInfo->iLenLongestCW = cur_cw_len;

            coderInfo->cur_cw++;
#endif
        }
        coderInfo->spectral_count = counter;  /* send the current count back to the outside world */
        return(bits);
    case 8:
        for(i=offset;i<offset+length;i=i+2){
            index = 8*ABS(quant[i]) + ABS(quant[i+1]);
            codebook = huff8[index][LASTINTAB];
            tmp = huff8[index][FIRSTINTAB];
            bits = bits + tmp;
            data[counter] = codebook;
            len[counter++] = tmp;
#ifdef DRM
            num_data[coderInfo->cur_cw] = 1;
            cur_cw_len = tmp;
#endif
            for(j=0;j<2;j++){
                if(quant[i+j] > 0) {  /* send out '0' if a positive value */
                    data[counter] = 0;
                    len[counter++] = 1;
                    bits += 1;
#ifdef DRM
                    num_data[coderInfo->cur_cw]++;
                    cur_cw_len += 1;
#endif
                } else
                if(quant[i+j] < 0) {  /* send out '1' if a negative value */
                    data[counter] = 1;
                    len[counter++] = 1;
                    bits += 1;
#ifdef DRM
                    num_data[coderInfo->cur_cw]++;
                    cur_cw_len += 1;
#endif
                }
            }
#ifdef DRM
            coderInfo->iLenReordSpData += cur_cw_len;
            if (cur_cw_len > coderInfo->iLenLongestCW)
                coderInfo->iLenLongestCW = cur_cw_len;

            coderInfo->cur_cw++;
#endif
        }
        coderInfo->spectral_count = counter;  /* send the current count back to the outside world */
        return(bits);
    case 9:
        for(i=offset;i<offset+length;i=i+2){
            index = 13*ABS(quant[i]) + ABS(quant[i+1]);
            codebook = huff9[index][LASTINTAB];
            tmp = huff9[index][FIRSTINTAB];
            bits = bits + tmp;
            data[counter] = codebook;
            len[counter++] = tmp;
#ifdef DRM
            num_data[coderInfo->cur_cw] = 1;
            cur_cw_len = tmp;
#endif
            for(j=0;j<2;j++){
                if(quant[i+j] > 0) {  /* send out '0' if a positive value */
                    data[counter] = 0;
                    len[counter++] = 1;
                    bits += 1;
#ifdef DRM
                    num_data[coderInfo->cur_cw]++;
                    cur_cw_len += 1;
#endif
                } else
                if(quant[i+j] < 0) {  /* send out '1' if a negative value */
                    data[counter] = 1;
                    len[counter++] = 1;
                    bits += 1;
#ifdef DRM
                    num_data[coderInfo->cur_cw]++;
                    cur_cw_len += 1;
#endif
                }
            }
#ifdef DRM
            coderInfo->iLenReordSpData += cur_cw_len;
            if (cur_cw_len > coderInfo->iLenLongestCW)
                coderInfo->iLenLongestCW = cur_cw_len;

            coderInfo->cur_cw++;
#endif
        }
        coderInfo->spectral_count = counter;  /* send the current count back to the outside world */
        return(bits);
    case 10:
        for(i=offset;i<offset+length;i=i+2){
            index = 13*ABS(quant[i]) + ABS(quant[i+1]);
            codebook = huff10[index][LASTINTAB];
            tmp = huff10[index][FIRSTINTAB];
            bits = bits + tmp;
            data[counter] = codebook;
            len[counter++] = tmp;
#ifdef DRM
            num_data[coderInfo->cur_cw] = 1;
            cur_cw_len = tmp;
#endif
            for(j=0;j<2;j++){
                if(quant[i+j] > 0) {  /* send out '0' if a positive value */
                    data[counter] = 0;
                    len[counter++] = 1;
                    bits += 1;
#ifdef DRM
                    num_data[coderInfo->cur_cw]++;
                    cur_cw_len += 1;
#endif
                } else
                if(quant[i+j] < 0) {  /* send out '1' if a negative value */
                    data[counter] = 1;
                    len[counter++] = 1;
                    bits += 1;
#ifdef DRM
                    num_data[coderInfo->cur_cw]++;
                    cur_cw_len += 1;
#endif
                }
            }
#ifdef DRM
            coderInfo->iLenReordSpData += cur_cw_len;
            if (cur_cw_len > coderInfo->iLenLongestCW)
                coderInfo->iLenLongestCW = cur_cw_len;

            coderInfo->cur_cw++;
#endif
        }
        coderInfo->spectral_count = counter;  /* send the current count back to the outside world */
        return(bits);
    case 11:
        /* First, calculate the indecies into the huffman tables */
        for(i=offset;i<offset+length;i=i+2){
            if ((ABS(quant[i]) >= 16) && (ABS(quant[i+1]) >= 16)) {  /* both codewords were above 16 */
                /* first, code the orignal pair, with the larger value saturated to +/- 16 */
                index = 17*16 + 16;
            }
            else if (ABS(quant[i]) >= 16) {  /* the first codeword was above 16, not the second one */
                /* first, code the orignal pair, with the larger value saturated to +/- 16 */
                index = 17*16 + ABS(quant[i+1]);
            }
            else if (ABS(quant[i+1]) >= 16) { /* the second codeword was above 16, not the first one */
                index = 17*ABS(quant[i]) + 16;
            }
            else {  /* there were no values above 16, so no escape sequences */
                index = 17*ABS(quant[i]) + ABS(quant[i+1]);
            }

            /* write out the codewords */
            tmp = huff11[index][FIRSTINTAB];
            codebook = huff11[index][LASTINTAB];
            bits += tmp;
            data[counter] = codebook;
            len[counter++] = tmp;
#ifdef DRM
            num_data[coderInfo->cur_cw] = 1;
            cur_cw_len = tmp;
#endif

            /* Take care of the sign bits */
            for(j=0;j<2;j++){
                if(quant[i+j] > 0) {  /* send out '0' if a positive value */
                    data[counter] = 0;
                    len[counter++] = 1;
                    bits += 1;
#ifdef DRM
                    num_data[coderInfo->cur_cw]++;
                    cur_cw_len += 1;
#endif
                } else
                if(quant[i+j] < 0) {  /* send out '1' if a negative value */
                    data[counter] = 1;
                    len[counter++] = 1;
                    bits += 1;
#ifdef DRM
                    num_data[coderInfo->cur_cw]++;
                    cur_cw_len += 1;
#endif
                }
            }

            /* write out the escape sequences */
            if ((ABS(quant[i]) >= 16) && (ABS(quant[i+1]) >= 16)) {  /* both codewords were above 16 */
                /* code and transmit the first escape_sequence */
                esc_sequence = CalculateEscSequence(quant[i],&len_esc);
                bits += len_esc;
                data[counter] = esc_sequence;
                len[counter++] = len_esc;
#ifdef DRM
                num_data[coderInfo->cur_cw]++;
                cur_cw_len += len_esc;

                if (esc_sequence > max_esc_sequ)
                    max_esc_sequ = esc_sequence;
#endif

                /* then code and transmit the second escape_sequence */
                esc_sequence = CalculateEscSequence(quant[i+1],&len_esc);
                bits += len_esc;
                data[counter] = esc_sequence;
                len[counter++] = len_esc;
#ifdef DRM
                num_data[coderInfo->cur_cw]++;
                cur_cw_len += len_esc;

                if (esc_sequence > max_esc_sequ)
                    max_esc_sequ = esc_sequence;
#endif
            }
            else if (ABS(quant[i]) >= 16) {  /* the first codeword was above 16, not the second one */
                /* code and transmit the escape_sequence */
                esc_sequence = CalculateEscSequence(quant[i],&len_esc);
                bits += len_esc;
                data[counter] = esc_sequence;
                len[counter++] = len_esc;
#ifdef DRM
                num_data[coderInfo->cur_cw]++;
                cur_cw_len += len_esc;

                if (esc_sequence > max_esc_sequ)
                    max_esc_sequ = esc_sequence;
#endif
            }
            else if (ABS(quant[i+1]) >= 16) { /* the second codeword was above 16, not the first one */
                /* code and transmit the escape_sequence */
                esc_sequence = CalculateEscSequence(quant[i+1],&len_esc);
                bits += len_esc;
                data[counter] = esc_sequence;
                len[counter++] = len_esc;
#ifdef DRM
                num_data[coderInfo->cur_cw]++;
                cur_cw_len += len_esc;

                if (esc_sequence > max_esc_sequ)
                    max_esc_sequ = esc_sequence;
#endif
            }
#ifdef DRM
            coderInfo->iLenReordSpData += cur_cw_len;
            if (cur_cw_len > coderInfo->iLenLongestCW)
                coderInfo->iLenLongestCW = cur_cw_len;

            coderInfo->cur_cw++;
#endif
        }
        coderInfo->spectral_count = counter;  /* send the current count back to the outside world */

#ifdef DRM
        /* VCB11: check which codebook should be used using max escape sequence */
        /* 8.5.3.1.3, table 157 */
        if (max_esc_sequ <= 15)
            *book = 16;
        else if (max_esc_sequ <= 31)
            *book = 17;
        else if (max_esc_sequ <= 47)
            *book = 18;
        else if (max_esc_sequ <= 63)
            *book = 19;
        else if (max_esc_sequ <= 95)
            *book = 20;
        else if (max_esc_sequ <= 127)
            *book = 21;
        else if (max_esc_sequ <= 159)
            *book = 22;
        else if (max_esc_sequ <= 191)
            *book = 23;
        else if (max_esc_sequ <= 223)
            *book = 24;
        else if (max_esc_sequ <= 255)
            *book = 25;
        else if (max_esc_sequ <= 319)
            *book = 26;
        else if (max_esc_sequ <= 383)
            *book = 27;
        else if (max_esc_sequ <= 511)
            *book = 28;
        else if (max_esc_sequ <= 767)
            *book = 29;
        else if (max_esc_sequ <= 1023)
            *book = 30;
        else if (max_esc_sequ <= 2047)
            *book = 31;
        /* else: codebook 11 -> it is already 11 */
#endif

        return(bits);
    }
    return 0;
}

int CalcBits(CoderInfo *coderInfo,
             int book,
             int *quant,
             int offset,
             int length)
{
  /*
     This function inputs
     - a specific codebook number, 'book'
     - the quantized spectral data, 'quant[]'
     - the offset into the spectral data to begin scanning, 'offset'
     - the 'length' of the segment to huffman code
     -> therefore, the segment quant[offset] to quant[offset+length-1]
     is huffman coded.

     This function outputs
     - the number of bits required, 'bits'  using the prescribed codebook, book applied to
     the given segment of spectral data.

   */

    int len_esc;
    int index;
    int bits = 0;
    int i, j;

    switch (book) {
    case 1:
        for(i=offset;i<offset+length;i=i+4){
            index = 27*quant[i] + 9*quant[i+1] + 3*quant[i+2] + quant[i+3] + 40;
            bits += huff1[index][FIRSTINTAB];
        }
        return (bits);
    case 2:
        for(i=offset;i<offset+length;i=i+4){
            index = 27*quant[i] + 9*quant[i+1] + 3*quant[i+2] + quant[i+3] + 40;
            bits += huff2[index][FIRSTINTAB];
        }
        return (bits);
    case 3:
        for(i=offset;i<offset+length;i=i+4){
            index = 27*ABS(quant[i]) + 9*ABS(quant[i+1]) + 3*ABS(quant[i+2]) + ABS(quant[i+3]);
            bits += huff3[index][FIRSTINTAB];
            for(j=0;j<4;j++){
                if(quant[i+j] != 0) bits += 1; /* only for non-zero spectral coefficients */
            }
        }
        return (bits);
    case 4:
        for(i=offset;i<offset+length;i=i+4){
            index = 27*ABS(quant[i]) + 9*ABS(quant[i+1]) + 3*ABS(quant[i+2]) + ABS(quant[i+3]);
            bits += huff4[index][FIRSTINTAB];
            for(j=0;j<4;j++){
                if(quant[i+j] != 0) bits += 1; /* only for non-zero spectral coefficients */
            }
        }
        return (bits);
    case 5:
        for(i=offset;i<offset+length;i=i+2){
            index = 9*(quant[i]) + (quant[i+1]) + 40;
            bits += huff5[index][FIRSTINTAB];
        }
        return (bits);
    case 6:
        for(i=offset;i<offset+length;i=i+2){
            index = 9*(quant[i]) + (quant[i+1]) + 40;
            bits += huff6[index][FIRSTINTAB];
        }
        return (bits);
    case 7:
        for(i=offset;i<offset+length;i=i+2){
            index = 8*ABS(quant[i]) + ABS(quant[i+1]);
            bits += huff7[index][FIRSTINTAB];
            for(j=0;j<2;j++){
                if(quant[i+j] != 0) bits += 1; /* only for non-zero spectral coefficients */
            }
        }
        return (bits);
    case 8:
        for(i=offset;i<offset+length;i=i+2){
            index = 8*ABS(quant[i]) + ABS(quant[i+1]);
            bits += huff8[index][FIRSTINTAB];
            for(j=0;j<2;j++){
                if(quant[i+j] != 0) bits += 1; /* only for non-zero spectral coefficients */
            }
        }
        return (bits);
    case 9:
        for(i=offset;i<offset+length;i=i+2){
            index = 13*ABS(quant[i]) + ABS(quant[i+1]);
            bits += huff9[index][FIRSTINTAB];
            for(j=0;j<2;j++){
                if(quant[i+j] != 0) bits += 1; /* only for non-zero spectral coefficients */
            }
        }
        return (bits);
    case 10:
        for(i=offset;i<offset+length;i=i+2){
            index = 13*ABS(quant[i]) + ABS(quant[i+1]);
            bits += huff10[index][FIRSTINTAB];
            for(j=0;j<2;j++){
                if(quant[i+j] != 0) bits += 1; /* only for non-zero spectral coefficients */
            }
        }
        return (bits);
    case 11:
        /* First, calculate the indecies into the huffman tables */
        for(i=offset;i<offset+length;i=i+2){
            if ((ABS(quant[i]) >= 16) && (ABS(quant[i+1]) >= 16)) {  /* both codewords were above 16 */
                /* first, code the orignal pair, with the larger value saturated to +/- 16 */
                index = 17*16 + 16;
            } else if (ABS(quant[i]) >= 16) {  /* the first codeword was above 16, not the second one */
                /* first, code the orignal pair, with the larger value saturated to +/- 16 */
                index = 17*16 + ABS(quant[i+1]);
            } else if (ABS(quant[i+1]) >= 16) { /* the second codeword was above 16, not the first one */
                index = 17*ABS(quant[i]) + 16;
            } else {  /* there were no values above 16, so no escape sequences */
                index = 17*ABS(quant[i]) + ABS(quant[i+1]);
            }

            /* write out the codewords */
            bits += huff11[index][FIRSTINTAB];

            /* Take care of the sign bits */
            for(j=0;j<2;j++){
                if(quant[i+j] != 0) bits += 1; /* only for non-zero spectral coefficients */
            }

            /* write out the escape sequences */
            if ((ABS(quant[i]) >= 16) && (ABS(quant[i+1]) >= 16)) {  /* both codewords were above 16 */
                /* code and transmit the first escape_sequence */
                CalculateEscSequence(quant[i],&len_esc);
                bits += len_esc;

                /* then code and transmit the second escape_sequence */
                CalculateEscSequence(quant[i+1],&len_esc);
                bits += len_esc;
            } else if (ABS(quant[i]) >= 16) {  /* the first codeword was above 16, not the second one */
                /* code and transmit the escape_sequence */
                CalculateEscSequence(quant[i],&len_esc);
                bits += len_esc;
            } else if (ABS(quant[i+1]) >= 16) { /* the second codeword was above 16, not the first one */
                /* code and transmit the escape_sequence */
                CalculateEscSequence(quant[i+1],&len_esc);
                bits += len_esc;
            }
        }
        return (bits);
    }
    return 0;
}

int SortBookNumbers(CoderInfo *coderInfo,
                    BitStream *bitStream,
                    int writeFlag)
{
  /*
    This function inputs the vector, 'book_vector[]', which is of length MAX_SCFAC_BANDS,
    and contains the optimal huffman tables of each sfb.  It returns the vector, 'output_book_vector[]', which
    has it's elements formatted for the encoded bit stream.  It's syntax is:

    {sect_cb[0], length_segment[0], ... ,sect_cb[num_of_sections], length_segment[num_of_sections]}

    The above syntax is true, unless there is an escape sequence.  An
    escape sequence occurs when a section is longer than 2 ^ (bit_len)
    long in units of scalefactor bands.  Also, the integer returned from
    this function is the number of bits written in the bitstream,
    'bit_count'.

    This function supports both long and short blocks.
    */

    int i;
    int repeat_counter;
    int bit_count = 0;
    int previous;
    int max, bit_len/*,sfbs*/;
    int max_sfb,g,band;
	int sect_cb_bits = 4;

    /* Set local pointers to coderInfo elements */
    int* book_vector = coderInfo->book_vector;

#ifdef DRM
    sect_cb_bits = 5; /* 5 bits in case of VCB11 */
#endif

    if (coderInfo->block_type == ONLY_SHORT_WINDOW){
        max = 7;
        bit_len = 3;
    } else {  /* the block_type is a long,start, or stop window */
        max = 31;
        bit_len = 5;
    }

    /* Compute number of scalefactor bands */
    max_sfb = coderInfo->nr_of_sfb / coderInfo->num_window_groups;


    for (g = 0; g < coderInfo->num_window_groups; g++) {
        band=g*max_sfb;

        repeat_counter=1;

        previous = book_vector[band];
        if (writeFlag) {
            PutBit(bitStream,book_vector[band],sect_cb_bits);
        }
        bit_count += sect_cb_bits;

        for (i=band+1;i<band+max_sfb;i++) {
#ifdef DRM
            /* sect_len is not transmitted in case the codebook for a */
            /* section is 11 or in the range of 16 and 31 */
            if ((previous == 11) ||
                ((previous >= 16) && (previous <= 32)))
            {
                if (writeFlag)
                    PutBit(bitStream,book_vector[i],sect_cb_bits);
                bit_count += sect_cb_bits;
                previous = book_vector[i];
                repeat_counter=1;

            } else
#endif
            if( (book_vector[i] != previous)) {
                if (writeFlag) {
                    PutBit(bitStream,repeat_counter,bit_len);
                }
                bit_count += bit_len;

                if (repeat_counter == max){  /* in case you need to terminate an escape sequence */
                    if (writeFlag)
                        PutBit(bitStream,0,bit_len);
                    bit_count += bit_len;
                }

                if (writeFlag)
                    PutBit(bitStream,book_vector[i],sect_cb_bits);
                bit_count += sect_cb_bits;
                previous = book_vector[i];
                repeat_counter=1;
            }
            /* if the length of the section is longer than the amount of bits available in */
            /* the bitsream, "max", then start up an escape sequence */
            else if ((book_vector[i] == previous) && (repeat_counter == max)) {
                if (writeFlag) {
                    PutBit(bitStream,repeat_counter,bit_len);
                }
                bit_count += bit_len;
                repeat_counter = 1;
            }
            else {
                repeat_counter++;
            }
        }

#ifdef DRM
        if (!((previous == 11) || ((previous >= 16) && (previous <= 32))))
#endif
        {
            if (writeFlag)
                PutBit(bitStream,repeat_counter,bit_len);
            bit_count += bit_len;

            if (repeat_counter == max) {  /* special case if the last section length is an */
                /* escape sequence */
                if (writeFlag)
                    PutBit(bitStream,0,bit_len);
                bit_count += bit_len;
            }
        }
    }  /* Bottom of group iteration */

    return bit_count;
}

int WriteScalefactors(CoderInfo *coderInfo,
                      BitStream *bitStream,
                      int writeFlag)

{
    /* this function takes care of counting the number of bits necessary */
    /* to encode the scalefactors.  In addition, if the writeFlag == 1, */
    /* then the scalefactors are written out the bitStream output bit */
    /* stream.  it returns k, the number of bits written to the bitstream*/

    int i,j,bit_count=0;
    int diff,length,codeword;
    int previous_scale_factor;
    int previous_is_factor;       /* Intensity stereo */
    int index = 0;
    int nr_of_sfb_per_group;

    /* set local pointer to coderInfo elements */
    int* scale_factors = coderInfo->scale_factor;

    if (coderInfo->block_type == ONLY_SHORT_WINDOW) { /* short windows */
        nr_of_sfb_per_group = coderInfo->nr_of_sfb/coderInfo->num_window_groups;
    } else {
        nr_of_sfb_per_group = coderInfo->nr_of_sfb;
        coderInfo->num_window_groups = 1;
        coderInfo->window_group_length[0] = 1;
    }

    previous_scale_factor = coderInfo->global_gain;
    previous_is_factor = 0;

    for(j=0; j<coderInfo->num_window_groups; j++){
        for(i=0;i<nr_of_sfb_per_group;i++) {
            /* test to see if any codebooks in a group are zero */
            if ( (coderInfo->book_vector[index]==INTENSITY_HCB) ||
                (coderInfo->book_vector[index]==INTENSITY_HCB2) ) {
                /* only send scalefactors if using non-zero codebooks */
                diff = scale_factors[index] - previous_is_factor;
                if ((diff < 60)&&(diff >= -60))
                    length = huff12[diff+60][FIRSTINTAB];
                else length = 0;
                bit_count+=length;
                previous_is_factor = scale_factors[index];
                if (writeFlag == 1 ) {
                    codeword = huff12[diff+60][LASTINTAB];
                    PutBit(bitStream,codeword,length);
                }
            } else if (coderInfo->book_vector[index]) {
                /* only send scalefactors if using non-zero codebooks */
                diff = scale_factors[index] - previous_scale_factor;
                if ((diff < 60)&&(diff >= -60))
                    length = huff12[diff+60][FIRSTINTAB];
                else length = 0;
                bit_count+=length;
                previous_scale_factor = scale_factors[index];
                if (writeFlag == 1 ) {
                    codeword = huff12[diff+60][LASTINTAB];
                    PutBit(bitStream,codeword,length);
                }
            }
            index++;
        }
    }
    return bit_count;
}


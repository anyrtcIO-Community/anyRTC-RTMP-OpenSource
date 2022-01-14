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
 * $Id: coder.h,v 1.13 2005/02/02 07:49:10 sur Exp $
 */

#ifndef CODER_H
#define CODER_H

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/* Allow encoding of Digital Radio Mondiale (DRM) */
//#define DRM

/* Allow encoding of Digital Radio Mondiale (DRM) with transform length 1024 */
//#define DRM_1024

#define MAX_CHANNELS 64

#ifdef DRM
#ifdef DRM_1024
# define FRAME_LEN 1024
# define BLOCK_LEN_LONG 1024
# define BLOCK_LEN_SHORT 128
#else
# define FRAME_LEN 960
# define BLOCK_LEN_LONG 960
# define BLOCK_LEN_SHORT 120
#endif /* DRM_1024 */
#else
# define FRAME_LEN 1024
# define BLOCK_LEN_LONG 1024
# define BLOCK_LEN_SHORT 128
#endif

#define NSFB_LONG  51
#define NSFB_SHORT 15
#define MAX_SHORT_WINDOWS 8
#define MAX_SCFAC_BANDS ((NSFB_SHORT+1)*MAX_SHORT_WINDOWS)

enum WINDOW_TYPE {
    ONLY_LONG_WINDOW,
    LONG_SHORT_WINDOW,
    ONLY_SHORT_WINDOW,
    SHORT_LONG_WINDOW
};

#define TNS_MAX_ORDER 20
#define DEF_TNS_GAIN_THRESH 1.4
#define DEF_TNS_COEFF_THRESH 0.1
#define DEF_TNS_COEFF_RES 4
#define DEF_TNS_RES_OFFSET 3
#define LEN_TNS_NFILTL 2
#define LEN_TNS_NFILTS 1

#define DELAY 2048
#define LEN_LTP_DATA_PRESENT 1
#define LEN_LTP_LAG 11
#define LEN_LTP_COEF 3
#define LEN_LTP_SHORT_USED 1
#define LEN_LTP_SHORT_LAG_PRESENT 1
#define LEN_LTP_SHORT_LAG 5
#define LTP_LAG_OFFSET 16
#define LEN_LTP_LONG_USED 1
#define MAX_LT_PRED_LONG_SFB 40
#define MAX_LT_PRED_SHORT_SFB 13
#define SHORT_SQ_OFFSET (BLOCK_LEN_LONG-(BLOCK_LEN_SHORT*4+BLOCK_LEN_SHORT/2))
#define CODESIZE 8
#define NOK_LT_BLEN (3 * BLOCK_LEN_LONG)

#define SBMAX_L 49
#define LPC 2

typedef struct {
    int order;                           /* Filter order */
    int direction;                       /* Filtering direction */
    int coefCompress;                    /* Are coeffs compressed? */
    int length;                          /* Length, in bands */
    double aCoeffs[TNS_MAX_ORDER+1];     /* AR Coefficients */
    double kCoeffs[TNS_MAX_ORDER+1];     /* Reflection Coefficients */
    int index[TNS_MAX_ORDER+1];          /* Coefficient indices */
} TnsFilterData;

typedef struct {
    int numFilters;                             /* Number of filters */
    int coefResolution;                         /* Coefficient resolution */
    TnsFilterData tnsFilter[1<<LEN_TNS_NFILTL]; /* TNS filters */
} TnsWindowData;

typedef struct {
    int tnsDataPresent;
    int tnsMinBandNumberLong;
    int tnsMinBandNumberShort;
    int tnsMaxBandsLong;
    int tnsMaxBandsShort;
    int tnsMaxOrderLong;
    int tnsMaxOrderShort;
    TnsWindowData windowData[MAX_SHORT_WINDOWS]; /* TNS data per window */
} TnsInfo;

typedef struct
{
    int weight_idx;
    double weight;
    int sbk_prediction_used[MAX_SHORT_WINDOWS];
    int sfb_prediction_used[MAX_SCFAC_BANDS];
    int delay[MAX_SHORT_WINDOWS];
    int global_pred_flag;
    int side_info;
    double *buffer;
    double *mdct_predicted;

    double *time_buffer;
    double *ltp_overlap_buffer;
} LtpInfo;

typedef struct
{
    int psy_init_mc;
    double dr_mc[LPC][BLOCK_LEN_LONG],e_mc[LPC+1+1][BLOCK_LEN_LONG];
    double K_mc[LPC+1][BLOCK_LEN_LONG], R_mc[LPC+1][BLOCK_LEN_LONG];
    double VAR_mc[LPC+1][BLOCK_LEN_LONG], KOR_mc[LPC+1][BLOCK_LEN_LONG];
    double sb_samples_pred_mc[BLOCK_LEN_LONG];
    int thisLineNeedsResetting_mc[BLOCK_LEN_LONG];
    int reset_count_mc;
} BwpInfo;


typedef struct {
    int window_shape;
    int prev_window_shape;
    int block_type;
    int desired_block_type;

    int global_gain;
    int scale_factor[MAX_SCFAC_BANDS];

    int num_window_groups;
    int window_group_length[8];
    int max_sfb;
    int nr_of_sfb;
    int sfb_offset[250];
    int lastx;
    double avgenrg;

    int spectral_count;

    /* Huffman codebook selected for each sf band */
    int book_vector[MAX_SCFAC_BANDS];

    /* Data of spectral bitstream elements, for each spectral pair,
       5 elements are required: 1*(esc)+2*(sign)+2*(esc value)=5 */
    int *data;

    /* Lengths of spectral bitstream elements */
    int *len;

#ifdef DRM
    int *num_data_cw;
    int cur_cw;
    int all_sfb;

    int iLenLongestCW;
    int iLenReordSpData;
#endif

    /* Holds the requantized spectrum */
    double *requantFreq;

    TnsInfo tnsInfo;
    LtpInfo ltpInfo;
    BwpInfo bwpInfo;

    int max_pred_sfb;
    int pred_global_flag;
    int pred_sfb_flag[MAX_SCFAC_BANDS];
    int reset_group_number;

} CoderInfo;

typedef struct {
  unsigned long sampling_rate;  /* the following entries are for this sampling rate */
  int num_cb_long;
  int num_cb_short;
  int cb_width_long[NSFB_LONG];
  int cb_width_short[NSFB_SHORT];
} SR_INFO;

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* CODER_H */

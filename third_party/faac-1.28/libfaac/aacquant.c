/*
 * FAAC - Freeware Advanced Audio Coder
 * Copyright (C) 2001 Menno Bakker
 * Copyright (C) 2002, 2003 Krzysztof Nikiel
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
 * $Id: aacquant.c,v 1.32 2008/03/23 23:00:25 menno Exp $
 */

#include <math.h>
#include <stdlib.h>

#include "frame.h"
#include "aacquant.h"
#include "coder.h"
#include "huffman.h"
#include "psych.h"
#include "util.h"

#define TAKEHIRO_IEEE754_HACK 1

#define XRPOW_FTOI(src,dest) ((dest) = (int)(src))
#define QUANTFAC(rx)  adj43[rx]
#define ROUNDFAC 0.4054

static int FixNoise(CoderInfo *coderInfo,
		    const double *xr,
		    double *xr_pow,
		    int *xi,
		    double *xmin,
		    double *pow43,
		    double *adj43);

static void CalcAllowedDist(CoderInfo *coderInfo, PsyInfo *psyInfo,
			    double *xr, double *xmin, int quality);


void AACQuantizeInit(CoderInfo *coderInfo, unsigned int numChannels,
		     AACQuantCfg *aacquantCfg)
{
    unsigned int channel, i;

    aacquantCfg->pow43 = (double*)AllocMemory(PRECALC_SIZE*sizeof(double));
    aacquantCfg->adj43 = (double*)AllocMemory(PRECALC_SIZE*sizeof(double));

    aacquantCfg->pow43[0] = 0.0;
    for(i=1;i<PRECALC_SIZE;i++)
        aacquantCfg->pow43[i] = pow((double)i, 4.0/3.0);

#if TAKEHIRO_IEEE754_HACK
    aacquantCfg->adj43[0] = 0.0;
    for (i = 1; i < PRECALC_SIZE; i++)
      aacquantCfg->adj43[i] = i - 0.5 - pow(0.5 * (aacquantCfg->pow43[i - 1] + aacquantCfg->pow43[i]),0.75);
#else // !TAKEHIRO_IEEE754_HACK
    for (i = 0; i < PRECALC_SIZE-1; i++)
        aacquantCfg->adj43[i] = (i + 1) - pow(0.5 * (aacquantCfg->pow43[i] + aacquantCfg->pow43[i + 1]), 0.75);
    aacquantCfg->adj43[i] = 0.5;
#endif

    for (channel = 0; channel < numChannels; channel++) {
        coderInfo[channel].requantFreq = (double*)AllocMemory(BLOCK_LEN_LONG*sizeof(double));
    }
}

void AACQuantizeEnd(CoderInfo *coderInfo, unsigned int numChannels,
		   AACQuantCfg *aacquantCfg)
{
    unsigned int channel;

    if (aacquantCfg->pow43)
	{
      FreeMemory(aacquantCfg->pow43);
      aacquantCfg->pow43 = NULL;
	}
    if (aacquantCfg->adj43)
	{
      FreeMemory(aacquantCfg->adj43);
      aacquantCfg->adj43 = NULL;
	}

    for (channel = 0; channel < numChannels; channel++) {
        if (coderInfo[channel].requantFreq) FreeMemory(coderInfo[channel].requantFreq);
    }
}

static void BalanceEnergy(CoderInfo *coderInfo,
			  const double *xr, const int *xi,
			  double *pow43)
{
  const double ifqstep = pow(2.0, 0.25);
  const double logstep_1 = 1.0 / log(ifqstep);
  int sb;
  int nsfb = coderInfo->nr_of_sfb;
  int start, end;
  int l;
  double en0, enq;
  int shift;

  for (sb = 0; sb < nsfb; sb++)
  {
    double qfac_1;

    start = coderInfo->sfb_offset[sb];
    end   = coderInfo->sfb_offset[sb+1];

    qfac_1 = pow(2.0, -0.25*(coderInfo->scale_factor[sb] - coderInfo->global_gain));

    en0 = 0.0;
    enq = 0.0;
    for (l = start; l < end; l++)
    {
      double xq;

      if (!sb && !xi[l])
	continue;

      xq = pow43[xi[l]];

      en0 += xr[l] * xr[l];
      enq += xq * xq;
    }

    if (enq == 0.0)
      continue;

    enq *= qfac_1 * qfac_1;

    shift = (int)(log(sqrt(enq / en0)) * logstep_1 + 1000.5);
    shift -= 1000;

    shift += coderInfo->scale_factor[sb];
    coderInfo->scale_factor[sb] = shift;
  }
}

static void UpdateRequant(CoderInfo *coderInfo, int *xi,
			  double *pow43)
{
  double *requant_xr = coderInfo->requantFreq;
  int sb;
  int i;

  for (sb = 0; sb < coderInfo->nr_of_sfb; sb++)
  {
    double invQuantFac =
      pow(2.0, -0.25*(coderInfo->scale_factor[sb] - coderInfo->global_gain));
    int start = coderInfo->sfb_offset[sb];
    int end = coderInfo->sfb_offset[sb + 1];

    for (i = start; i < end; i++)
      requant_xr[i] = pow43[xi[i]] * invQuantFac;
  }
}

int AACQuantize(CoderInfo *coderInfo,
                PsyInfo *psyInfo,
                ChannelInfo *channelInfo,
                int *cb_width,
                int num_cb,
                double *xr,
		AACQuantCfg *aacquantCfg)
{
    int sb, i, do_q = 0;
    int bits = 0, sign;
    double xr_pow[FRAME_LEN];
    double xmin[MAX_SCFAC_BANDS];
    int xi[FRAME_LEN];

    /* Use local copy's */
    int *scale_factor = coderInfo->scale_factor;

    /* Set all scalefactors to 0 */
    coderInfo->global_gain = 0;
    for (sb = 0; sb < coderInfo->nr_of_sfb; sb++)
        scale_factor[sb] = 0;

    /* Compute xr_pow */
    for (i = 0; i < FRAME_LEN; i++) {
        double temp = fabs(xr[i]);
        xr_pow[i] = sqrt(temp * sqrt(temp));
        do_q += (temp > 1E-20);
    }

    if (do_q) {
        CalcAllowedDist(coderInfo, psyInfo, xr, xmin, aacquantCfg->quality);
	coderInfo->global_gain = 0;
	FixNoise(coderInfo, xr, xr_pow, xi, xmin,
		 aacquantCfg->pow43, aacquantCfg->adj43);
	BalanceEnergy(coderInfo, xr, xi, aacquantCfg->pow43);
	UpdateRequant(coderInfo, xi, aacquantCfg->pow43);

        for ( i = 0; i < FRAME_LEN; i++ )  {
            sign = (xr[i] < 0) ? -1 : 1;
            xi[i] *= sign;
            coderInfo->requantFreq[i] *= sign;
        }
    } else {
        coderInfo->global_gain = 0;
        SetMemory(xi, 0, FRAME_LEN*sizeof(int));
    }

    BitSearch(coderInfo, xi);

    /* offset the difference of common_scalefac and scalefactors by SF_OFFSET  */
    for (i = 0; i < coderInfo->nr_of_sfb; i++) {
        if ((coderInfo->book_vector[i]!=INTENSITY_HCB)&&(coderInfo->book_vector[i]!=INTENSITY_HCB2)) {
            scale_factor[i] = coderInfo->global_gain - scale_factor[i] + SF_OFFSET;
        }
    }
    coderInfo->global_gain = scale_factor[0];
#if 0
	printf("global gain: %d\n", coderInfo->global_gain);
	for (i = 0; i < coderInfo->nr_of_sfb; i++)
	  printf("sf %d: %d\n", i, coderInfo->scale_factor[i]);
#endif
    // clamp to valid diff range
    {
      int previous_scale_factor = coderInfo->global_gain;
      int previous_is_factor = 0;
      for (i = 0; i < coderInfo->nr_of_sfb; i++) {
        if ((coderInfo->book_vector[i]==INTENSITY_HCB) ||
            (coderInfo->book_vector[i]==INTENSITY_HCB2)) {
            const int diff = scale_factor[i] - previous_is_factor;
            if (diff < -60) scale_factor[i] = previous_is_factor - 60;
            else if (diff > 59) scale_factor[i] = previous_is_factor + 59;
            previous_is_factor = scale_factor[i];
//            printf("sf %d: %d diff=%d **\n", i, coderInfo->scale_factor[i], diff);
        } else if (coderInfo->book_vector[i]) {
            const int diff = scale_factor[i] - previous_scale_factor;
            if (diff < -60) scale_factor[i] = previous_scale_factor - 60;
            else if (diff > 59) scale_factor[i] = previous_scale_factor + 59;
            previous_scale_factor = scale_factor[i];
//            printf("sf %d: %d diff=%d\n", i, coderInfo->scale_factor[i], diff);
        }
      }
    }

    /* place the codewords and their respective lengths in arrays data[] and len[] respectively */
    /* there are 'counter' elements in each array, and these are variable length arrays depending on the input */
#ifdef DRM
    coderInfo->iLenReordSpData = 0; /* init length of reordered spectral data */
    coderInfo->iLenLongestCW = 0; /* init length of longest codeword */
    coderInfo->cur_cw = 0; /* init codeword counter */
#endif
    coderInfo->spectral_count = 0;
    sb = 0;
    for(i = 0; i < coderInfo->nr_of_sfb; i++) {
        OutputBits(
            coderInfo,
#ifdef DRM
            &coderInfo->book_vector[i], /* needed for VCB11 */
#else
            coderInfo->book_vector[i],
#endif
            xi,
            coderInfo->sfb_offset[i],
            coderInfo->sfb_offset[i+1]-coderInfo->sfb_offset[i]);

        if (coderInfo->book_vector[i])
              sb = i;
    }

    // FIXME: Check those max_sfb/nr_of_sfb. Isn't it the same?
    coderInfo->max_sfb = coderInfo->nr_of_sfb = sb + 1;

    return bits;
}


#if TAKEHIRO_IEEE754_HACK

typedef union {
    float f;
    int i;
} fi_union;

#define MAGIC_FLOAT (65536*(128))
#define MAGIC_INT 0x4b000000

#if 0
static void Quantize(const double *xp, int *pi, double istep)
{
    int j;
    fi_union *fi;

    fi = (fi_union *)pi;
    for (j = FRAME_LEN/4 - 1; j >= 0; --j) {
        double x0 = istep * xp[0];
        double x1 = istep * xp[1];
        double x2 = istep * xp[2];
        double x3 = istep * xp[3];

        x0 += MAGIC_FLOAT; fi[0].f = x0;
        x1 += MAGIC_FLOAT; fi[1].f = x1;
        x2 += MAGIC_FLOAT; fi[2].f = x2;
        x3 += MAGIC_FLOAT; fi[3].f = x3;

        fi[0].f = x0 + (adj43asm - MAGIC_INT)[fi[0].i];
        fi[1].f = x1 + (adj43asm - MAGIC_INT)[fi[1].i];
        fi[2].f = x2 + (adj43asm - MAGIC_INT)[fi[2].i];
        fi[3].f = x3 + (adj43asm - MAGIC_INT)[fi[3].i];

        fi[0].i -= MAGIC_INT;
        fi[1].i -= MAGIC_INT;
        fi[2].i -= MAGIC_INT;
        fi[3].i -= MAGIC_INT;
        fi += 4;
        xp += 4;
    }
}
#endif
static void QuantizeBand(const double *xp, int *pi, double istep,
			 int offset, int end, double *adj43)
{
  int j;
  fi_union *fi;

  fi = (fi_union *)pi;
  for (j = offset; j < end; j++)
  {
    double x0 = istep * xp[j];

    x0 += MAGIC_FLOAT; fi[j].f = (float)x0;
    fi[j].f = x0 + (adj43 - MAGIC_INT)[fi[j].i];
    fi[j].i -= MAGIC_INT;
  }
}
#else
#if 0
static void Quantize(const double *xr, int *ix, double istep)
{
    int j;

    for (j = FRAME_LEN/8; j > 0; --j) {
        double x1, x2, x3, x4, x5, x6, x7, x8;
        int rx1, rx2, rx3, rx4, rx5, rx6, rx7, rx8;

        x1 = *xr++ * istep;
        x2 = *xr++ * istep;
        XRPOW_FTOI(x1, rx1);
        x3 = *xr++ * istep;
        XRPOW_FTOI(x2, rx2);
        x4 = *xr++ * istep;
        XRPOW_FTOI(x3, rx3);
        x5 = *xr++ * istep;
        XRPOW_FTOI(x4, rx4);
        x6 = *xr++ * istep;
        XRPOW_FTOI(x5, rx5);
        x7 = *xr++ * istep;
        XRPOW_FTOI(x6, rx6);
        x8 = *xr++ * istep;
        XRPOW_FTOI(x7, rx7);
        x1 += QUANTFAC(rx1);
        XRPOW_FTOI(x8, rx8);
        x2 += QUANTFAC(rx2);
        XRPOW_FTOI(x1,*ix++);
        x3 += QUANTFAC(rx3);
        XRPOW_FTOI(x2,*ix++);
        x4 += QUANTFAC(rx4);
        XRPOW_FTOI(x3,*ix++);
        x5 += QUANTFAC(rx5);
        XRPOW_FTOI(x4,*ix++);
        x6 += QUANTFAC(rx6);
        XRPOW_FTOI(x5,*ix++);
        x7 += QUANTFAC(rx7);
        XRPOW_FTOI(x6,*ix++);
        x8 += QUANTFAC(rx8);
        XRPOW_FTOI(x7,*ix++);
        XRPOW_FTOI(x8,*ix++);
    }
}
#endif
static void QuantizeBand(const double *xp, int *ix, double istep,
			 int offset, int end, double *adj43)
{
  int j;

  for (j = offset; j < end; j++)
  {
    double x0 = istep * xp[j];
    x0 += adj43[(int)x0];
    ix[j] = (int)x0;
  }
}
#endif

static void CalcAllowedDist(CoderInfo *coderInfo, PsyInfo *psyInfo,
                            double *xr, double *xmin, int quality)
{
  int sfb, start, end, l;
  const double globalthr = 132.0 / (double)quality;
  int last = coderInfo->lastx;
  int lastsb = 0;
  int *cb_offset = coderInfo->sfb_offset;
  int num_cb = coderInfo->nr_of_sfb;
  double avgenrg = coderInfo->avgenrg;

  for (sfb = 0; sfb < num_cb; sfb++)
  {
    if (last > cb_offset[sfb])
      lastsb = sfb;
  }

  for (sfb = 0; sfb < num_cb; sfb++)
  {
    double thr, tmp;
    double enrg = 0.0;

    start = cb_offset[sfb];
    end = cb_offset[sfb + 1];

    if (sfb > lastsb)
    {
      xmin[sfb] = 0;
      continue;
    }

    if (coderInfo->block_type != ONLY_SHORT_WINDOW)
    {
      double enmax = -1.0;
      double lmax;

      lmax = start;
      for (l = start; l < end; l++)
      {
	if (enmax < (xr[l] * xr[l]))
	{
	  enmax = xr[l] * xr[l];
	  lmax = l;
	}
      }

      start = lmax - 2;
      end = lmax + 3;
      if (start < 0)
	start = 0;
      if (end > last)
	end = last;
    }

    for (l = start; l < end; l++)
    {
      enrg += xr[l]*xr[l];
    }

    thr = enrg/((double)(end-start)*avgenrg);
    thr = pow(thr, 0.1*(lastsb-sfb)/lastsb + 0.3);

    tmp = 1.0 - ((double)start / (double)last);
    tmp = tmp * tmp * tmp + 0.075;

    thr = 1.0 / (1.4*thr + tmp);

    xmin[sfb] = ((coderInfo->block_type == ONLY_SHORT_WINDOW) ? 0.65 : 1.12)
      * globalthr * thr;
  }
}

static int FixNoise(CoderInfo *coderInfo,
		    const double *xr,
		    double *xr_pow,
		    int *xi,
		    double *xmin,
		    double *pow43,
		    double *adj43)
{
    int i, sb;
    int start, end;
    double diffvol;
    double tmp;
    const double ifqstep = pow(2.0, 0.1875);
    const double log_ifqstep = 1.0 / log(ifqstep);
    const double maxstep = 0.05;

    for (sb = 0; sb < coderInfo->nr_of_sfb; sb++)
    {
      double sfacfix;
      double fixstep = 0.25;
      int sfac;
      double fac;
      int dist;
      double sfacfix0 = 1.0, dist0 = 1e50;
      double maxx;

      start = coderInfo->sfb_offset[sb];
      end = coderInfo->sfb_offset[sb+1];

      if (!xmin[sb])
	goto nullsfb;

      maxx = 0.0;
      for (i = start; i < end; i++)
      {
	if (xr_pow[i] > maxx)
	  maxx = xr_pow[i];
      }

      //printf("band %d: maxx: %f\n", sb, maxx);
      if (maxx < 10.0)
      {
      nullsfb:
	for (i = start; i < end; i++)
	  xi[i] = 0;
	coderInfo->scale_factor[sb] = 10;
	continue;
      }

      sfacfix = 1.0 / maxx;
      sfac = (int)(log(sfacfix) * log_ifqstep - 0.5);
      for (i = start; i < end; i++)
	xr_pow[i] *= sfacfix;
      maxx *= sfacfix;
      coderInfo->scale_factor[sb] = sfac;
      QuantizeBand(xr_pow, xi, IPOW20(coderInfo->global_gain), start, end,
		   adj43);
      //printf("\tsfac: %d\n", sfac);

    calcdist:
      diffvol = 0.0;
      for (i = start; i < end; i++)
      {
	tmp = xi[i];
	diffvol += tmp * tmp;  // ~x^(3/2)
      }

      if (diffvol < 1e-6)
	diffvol = 1e-6;
      tmp = pow(diffvol / (double)(end - start), -0.666);

      if (fabs(fixstep) > maxstep)
      {
	double dd = 0.5*(tmp / xmin[sb] - 1.0);

	if (fabs(dd) < fabs(fixstep))
	{
	  fixstep = dd;

	  if (fabs(fixstep) < maxstep)
	    fixstep = maxstep * ((fixstep > 0) ? 1 : -1);
	}
      }

      if (fixstep > 0)
      {
	if (tmp < dist0)
	{
	  dist0 = tmp;
	  sfacfix0 = sfacfix;
	}
	else
	{
	  if (fixstep > .1)
	    fixstep = .1;
	}
      }
      else
      {
	dist0 = tmp;
	sfacfix0 = sfacfix;
      }

      dist = (tmp > xmin[sb]);
      fac = 0.0;
      if (fabs(fixstep) >= maxstep)
      {
	if ((dist && (fixstep < 0))
	    || (!dist && (fixstep > 0)))
	{
	  fixstep = -0.5 * fixstep;
	}

	fac = 1.0 + fixstep;
      }
      else if (dist)
      {
	fac = 1.0 + fabs(fixstep);
      }

      if (fac != 0.0)
      {
	if (maxx * fac >= IXMAX_VAL)
	{
	  // restore best noise
	  fac = sfacfix0 / sfacfix;
	  for (i = start; i < end; i++)
	    xr_pow[i] *= fac;
	  maxx *= fac;
	  sfacfix *= fac;
	  coderInfo->scale_factor[sb] = log(sfacfix) * log_ifqstep - 0.5;
	  QuantizeBand(xr_pow, xi, IPOW20(coderInfo->global_gain), start, end,
		       adj43);
	  continue;
	}

	if (coderInfo->scale_factor[sb] < -10)
	{
	  for (i = start; i < end; i++)
	    xr_pow[i] *= fac;
          maxx *= fac;
          sfacfix *= fac;
	  coderInfo->scale_factor[sb] = log(sfacfix) * log_ifqstep - 0.5;
	  QuantizeBand(xr_pow, xi, IPOW20(coderInfo->global_gain), start, end,
		       adj43);
	  goto calcdist;
	}
      }
    }
    return 0;
}

int SortForGrouping(CoderInfo* coderInfo,
                           PsyInfo *psyInfo,
                           ChannelInfo *channelInfo,
                           int *sfb_width_table,
                           double *xr)
{
    int i,j,ii;
    int index = 0;
    double xr_tmp[FRAME_LEN];
    int group_offset=0;
    int k=0;
    int windowOffset = 0;


    /* set up local variables for used quantInfo elements */
    int* sfb_offset = coderInfo->sfb_offset;
    int* nr_of_sfb = &(coderInfo->nr_of_sfb);
    int* window_group_length;
    int num_window_groups;
    *nr_of_sfb = coderInfo->max_sfb;              /* Init to max_sfb */
    window_group_length = coderInfo->window_group_length;
    num_window_groups = coderInfo->num_window_groups;

    /* calc org sfb_offset just for shortblock */
    sfb_offset[k]=0;
    for (k=1 ; k <*nr_of_sfb+1; k++) {
        sfb_offset[k] = sfb_offset[k-1] + sfb_width_table[k-1];
    }

    /* sort the input spectral coefficients */
    index = 0;
    group_offset=0;
    for (i=0; i< num_window_groups; i++) {
        for (k=0; k<*nr_of_sfb; k++) {
            for (j=0; j < window_group_length[i]; j++) {
                for (ii=0;ii< sfb_width_table[k];ii++)
                    xr_tmp[index++] = xr[ii+ sfb_offset[k] + BLOCK_LEN_SHORT*j +group_offset];
            }
        }
        group_offset +=  BLOCK_LEN_SHORT*window_group_length[i];
    }

    for (k=0; k<FRAME_LEN; k++){
        xr[k] = xr_tmp[k];
    }


    /* now calc the new sfb_offset table for the whole p_spectrum vector*/
    index = 0;
    sfb_offset[index++] = 0;
    windowOffset = 0;
    for (i=0; i < num_window_groups; i++) {
        for (k=0 ; k <*nr_of_sfb; k++) {
            sfb_offset[index] = sfb_offset[index-1] + sfb_width_table[k]*window_group_length[i] ;
            index++;
        }
        windowOffset += window_group_length[i];
    }

    *nr_of_sfb = *nr_of_sfb * num_window_groups;  /* Number interleaved bands. */

    return 0;
}

void CalcAvgEnrg(CoderInfo *coderInfo,
		 const double *xr)
{
  int end, l;
  int last = 0;
  double totenrg = 0.0;

  end = coderInfo->sfb_offset[coderInfo->nr_of_sfb];
  for (l = 0; l < end; l++)
  {
    if (xr[l])
    {
      last = l;
      totenrg += xr[l] * xr[l];
    }
    }
  last++;

  coderInfo->lastx = last;
  coderInfo->avgenrg = totenrg / last;
}

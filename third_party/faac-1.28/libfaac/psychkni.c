/*
 * FAAC - Freeware Advanced Audio Coder
 * Copyright (C) 2002 Krzysztof Nikiel
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
 * $Id: psychkni.c,v 1.17 2005/04/24 19:16:14 rjamorim Exp $
 */
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#include "psych.h"
#include "coder.h"
#include "fft.h"
#include "util.h"
#include "frame.h"

typedef float psyfloat;

typedef struct
{
  /* bandwidth */
  int bandS;
  int lastband;

  /* SFB energy */
  psyfloat *fftEnrgS[8];
  psyfloat *fftEnrgNextS[8];
  psyfloat *fftEnrgNext2S[8];
  psyfloat *fftEnrgPrevS[8];
}
psydata_t;


static void Hann(GlobalPsyInfo * gpsyInfo, double *inSamples, int size)
{
  int i;

  /* Applying Hann window */
  if (size == BLOCK_LEN_LONG * 2)
  {
    for (i = 0; i < size; i++)
      inSamples[i] *= gpsyInfo->hannWindow[i];
  }
  else
  {
    for (i = 0; i < size; i++)
      inSamples[i] *= gpsyInfo->hannWindowS[i];
  }
}

static void PsyCheckShort(PsyInfo * psyInfo)
{
  double totvol = 0.0;
  double totchg, totchg2;
  psydata_t *psydata = psyInfo->data;
  int lastband = psydata->lastband;
  int firstband = 1;
  int sfb;

  /* long/short block switch */
  totchg = totchg2 = 0.0;
  for (sfb = 0; sfb < lastband; sfb++)
  {
    int win;
    double volb[16];
    double vavg[13];
    double maxdif = 0.0;
    double totmaxdif = 0.0;
    double e, v;

    // previous frame
    for (win = 0; win < 4; win++)
    {
      e = psydata->fftEnrgPrevS[win + 4][sfb];

      volb[win] = sqrt(e);
      totvol += e;
    }

    // current frame
    for (win = 0; win < 8; win++)
      {
      e = psydata->fftEnrgS[win][sfb];

      volb[win + 4] = sqrt(e);
      totvol += e;
    }
    // next frame
    for (win = 0; win < 4; win++)
    {
      e = psydata->fftEnrgNextS[win][sfb];

      volb[win + 12] = sqrt(e);
    totvol += e;
    }

    // ignore lowest SFBs
    if (sfb < firstband)
      continue;

    v = 0.0;
    for (win = 0; win < 4; win++)
    {
      v += volb[win];
    }
    vavg[0] = 0.25 * v;

    for (win = 1; win < 13; win++)
      {
      v -= volb[win - 1];
      v += volb[win + 3];
      vavg[win] = 0.25 * v;
    }

    for (win = 0; win < 8; win++)
    {
      int i;
      double mina, maxv;
      double voldif;
      double totvoldif;

      mina = vavg[win];
      for (i = 1; i < 5; i++)
        mina = min(mina, vavg[win + i]);

      maxv = volb[win + 2];
      for (i = 3; i < 6; i++)
        maxv = max(maxv, volb[win + i]);

      if (!maxv || !mina)
        continue;

      voldif = (maxv - mina) / mina;
      totvoldif = (maxv - mina) * (maxv - mina);

	if (voldif > maxdif)
	  maxdif = voldif;

	if (totvoldif > totmaxdif)
	  totmaxdif = totvoldif;
      }
    totchg += maxdif;
    totchg2 += totmaxdif;
  }

  totvol = sqrt(totvol);

  totchg2 = sqrt(totchg2);

  totchg = totchg / lastband;
  if (totvol)
    totchg2 /= totvol;
  else
    totchg2 = 0.0;

  psyInfo->block_type = ((totchg > 1.0) && (totchg2 > 0.04))
    ? ONLY_SHORT_WINDOW : ONLY_LONG_WINDOW;

#if 0
  {
    static int total = 0, shorts = 0;
    char *flash = "    ";

    total++;
    if (psyInfo->block_type == ONLY_SHORT_WINDOW)
    {
      flash = "****";
      shorts++;
    }

    printf("totchg: %s %g %g\t%g\n", flash, totchg, totchg2,
	   (double)shorts/total);
  }
#endif
}

static void PsyInit(GlobalPsyInfo * gpsyInfo, PsyInfo * psyInfo, unsigned int numChannels,
		    unsigned int sampleRate, int *cb_width_long, int num_cb_long,
		    int *cb_width_short, int num_cb_short)
{
  unsigned int channel;
  int i, j, size;

  gpsyInfo->hannWindow =
    (double *) AllocMemory(2 * BLOCK_LEN_LONG * sizeof(double));
  gpsyInfo->hannWindowS =
    (double *) AllocMemory(2 * BLOCK_LEN_SHORT * sizeof(double));

  for (i = 0; i < BLOCK_LEN_LONG * 2; i++)
    gpsyInfo->hannWindow[i] = 0.5 * (1 - cos(2.0 * M_PI * (i + 0.5) /
					     (BLOCK_LEN_LONG * 2)));
  for (i = 0; i < BLOCK_LEN_SHORT * 2; i++)
    gpsyInfo->hannWindowS[i] = 0.5 * (1 - cos(2.0 * M_PI * (i + 0.5) /
					      (BLOCK_LEN_SHORT * 2)));
  gpsyInfo->sampleRate = (double) sampleRate;

  for (channel = 0; channel < numChannels; channel++)
  {
    psydata_t *psydata = AllocMemory(sizeof(psydata_t));
    psyInfo[channel].data = psydata;
  }

  size = BLOCK_LEN_LONG;
  for (channel = 0; channel < numChannels; channel++)
  {
    psyInfo[channel].size = size;

    psyInfo[channel].prevSamples =
      (double *) AllocMemory(size * sizeof(double));
    memset(psyInfo[channel].prevSamples, 0, size * sizeof(double));
  }

  size = BLOCK_LEN_SHORT;
  for (channel = 0; channel < numChannels; channel++)
  {
    psydata_t *psydata = psyInfo[channel].data;

    psyInfo[channel].sizeS = size;

    psyInfo[channel].prevSamplesS =
      (double *) AllocMemory(size * sizeof(double));
    memset(psyInfo[channel].prevSamplesS, 0, size * sizeof(double));

    for (j = 0; j < 8; j++)
    {
      psydata->fftEnrgPrevS[j] =
	(psyfloat *) AllocMemory(NSFB_SHORT * sizeof(psyfloat));
      memset(psydata->fftEnrgPrevS[j], 0, NSFB_SHORT * sizeof(psyfloat));
      psydata->fftEnrgS[j] =
	(psyfloat *) AllocMemory(NSFB_SHORT * sizeof(psyfloat));
      memset(psydata->fftEnrgS[j], 0, NSFB_SHORT * sizeof(psyfloat));
      psydata->fftEnrgNextS[j] =
	(psyfloat *) AllocMemory(NSFB_SHORT * sizeof(psyfloat));
      memset(psydata->fftEnrgNextS[j], 0, NSFB_SHORT * sizeof(psyfloat));
      psydata->fftEnrgNext2S[j] =
	(psyfloat *) AllocMemory(NSFB_SHORT * sizeof(psyfloat));
      memset(psydata->fftEnrgNext2S[j], 0, NSFB_SHORT * sizeof(psyfloat));
    }
  }
}

static void PsyEnd(GlobalPsyInfo * gpsyInfo, PsyInfo * psyInfo, unsigned int numChannels)
{
  unsigned int channel;
  int j;

  if (gpsyInfo->hannWindow)
    FreeMemory(gpsyInfo->hannWindow);
  if (gpsyInfo->hannWindowS)
    FreeMemory(gpsyInfo->hannWindowS);

  for (channel = 0; channel < numChannels; channel++)
  {
    if (psyInfo[channel].prevSamples)
      FreeMemory(psyInfo[channel].prevSamples);
  }

  for (channel = 0; channel < numChannels; channel++)
  {
    psydata_t *psydata = psyInfo[channel].data;

    if (psyInfo[channel].prevSamplesS)
      FreeMemory(psyInfo[channel].prevSamplesS);
    for (j = 0; j < 8; j++)
    {
      if (psydata->fftEnrgPrevS[j])
	FreeMemory(psydata->fftEnrgPrevS[j]);
      if (psydata->fftEnrgS[j])
	FreeMemory(psydata->fftEnrgS[j]);
      if (psydata->fftEnrgNextS[j])
	FreeMemory(psydata->fftEnrgNextS[j]);
      if (psydata->fftEnrgNext2S[j])
	FreeMemory(psydata->fftEnrgNext2S[j]);
    }
  }

  for (channel = 0; channel < numChannels; channel++)
  {
    if (psyInfo[channel].data)
      FreeMemory(psyInfo[channel].data);
  }
}

/* Do psychoacoustical analysis */
static void PsyCalculate(ChannelInfo * channelInfo, GlobalPsyInfo * gpsyInfo,
			 PsyInfo * psyInfo, int *cb_width_long, int
			 num_cb_long, int *cb_width_short,
			 int num_cb_short, unsigned int numChannels)
{
  unsigned int channel;

  for (channel = 0; channel < numChannels; channel++)
  {
    if (channelInfo[channel].present)
    {

      if (channelInfo[channel].cpe &&
	  channelInfo[channel].ch_is_left)
      {				/* CPE */

	int leftChan = channel;
	int rightChan = channelInfo[channel].paired_ch;

	PsyCheckShort(&psyInfo[leftChan]);
	PsyCheckShort(&psyInfo[rightChan]);
      }
      else if (!channelInfo[channel].cpe &&
	       channelInfo[channel].lfe)
      {				/* LFE */
        // Only set block type and it should be OK
	psyInfo[channel].block_type = ONLY_LONG_WINDOW;
      }
      else if (!channelInfo[channel].cpe)
      {				/* SCE */
	PsyCheckShort(&psyInfo[channel]);
      }
    }
  }
}

static void PsyBufferUpdate( FFT_Tables *fft_tables, GlobalPsyInfo * gpsyInfo, PsyInfo * psyInfo,
			    double *newSamples, unsigned int bandwidth,
			    int *cb_width_short, int num_cb_short)
{
  int win;
  double transBuff[2 * BLOCK_LEN_LONG];
  double transBuffS[2 * BLOCK_LEN_SHORT];
  psydata_t *psydata = psyInfo->data;
  psyfloat *tmp;
  int sfb;

  psydata->bandS = psyInfo->sizeS * bandwidth * 2 / gpsyInfo->sampleRate;

  memcpy(transBuff, psyInfo->prevSamples, psyInfo->size * sizeof(double));
  memcpy(transBuff + psyInfo->size, newSamples, psyInfo->size * sizeof(double));

  for (win = 0; win < 8; win++)
  {
    int first = 0;
    int last = 0;

    memcpy(transBuffS, transBuff + (win * BLOCK_LEN_SHORT) + (BLOCK_LEN_LONG - BLOCK_LEN_SHORT) / 2,
	   2 * psyInfo->sizeS * sizeof(double));

    Hann(gpsyInfo, transBuffS, 2 * psyInfo->sizeS);
    rfft( fft_tables, transBuffS, 8);

    // shift bufs
    tmp = psydata->fftEnrgPrevS[win];
    psydata->fftEnrgPrevS[win] = psydata->fftEnrgS[win];
    psydata->fftEnrgS[win] = psydata->fftEnrgNextS[win];
    psydata->fftEnrgNextS[win] = psydata->fftEnrgNext2S[win];
    psydata->fftEnrgNext2S[win] = tmp;

    for (sfb = 0; sfb < num_cb_short; sfb++)
    {
      double e;
      int l;

      first = last;
      last = first + cb_width_short[sfb];

      if (first < 1)
	first = 1;

      //if (last > psydata->bandS) // band out of range
      if (first >= psydata->bandS) // band out of range
	break;

      e = 0.0;
      for (l = first; l < last; l++)
      {
	double a = transBuffS[l];
	double b = transBuffS[l + psyInfo->sizeS];

	e += a * a + b * b;
      }

      psydata->fftEnrgNext2S[win][sfb] = e;
    }
    psydata->lastband = sfb;
    for (; sfb < num_cb_short; sfb++)
    {
      psydata->fftEnrgNext2S[win][sfb] = 0;
    }
  }

  memcpy(psyInfo->prevSamples, newSamples, psyInfo->size * sizeof(double));
}

static void BlockSwitch(CoderInfo * coderInfo, PsyInfo * psyInfo, unsigned int numChannels)
{
  unsigned int channel;
  int desire = ONLY_LONG_WINDOW;

  /* Use the same block type for all channels
     If there is 1 channel that wants a short block,
     use a short block on all channels.
   */
  for (channel = 0; channel < numChannels; channel++)
  {
    if (psyInfo[channel].block_type == ONLY_SHORT_WINDOW)
      desire = ONLY_SHORT_WINDOW;
  }

  for (channel = 0; channel < numChannels; channel++)
  {
    int lasttype = coderInfo[channel].block_type;

    if (desire == ONLY_SHORT_WINDOW
	|| coderInfo[channel].desired_block_type == ONLY_SHORT_WINDOW)
    {
      if (lasttype == ONLY_LONG_WINDOW || lasttype == SHORT_LONG_WINDOW)
	coderInfo[channel].block_type = LONG_SHORT_WINDOW;
      else
	coderInfo[channel].block_type = ONLY_SHORT_WINDOW;
    }
    else
    {
      if (lasttype == ONLY_SHORT_WINDOW || lasttype == LONG_SHORT_WINDOW)
	coderInfo[channel].block_type = SHORT_LONG_WINDOW;
      else
	coderInfo[channel].block_type = ONLY_LONG_WINDOW;
    }
    coderInfo[channel].desired_block_type = desire;
  }
}

psymodel_t psymodel2 =
{
  PsyInit,
  PsyEnd,
  PsyCalculate,
  PsyBufferUpdate,
  BlockSwitch
};

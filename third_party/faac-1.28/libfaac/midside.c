/*
 * FAAC - Freeware Advanced Audio Coder
 * Copyright (C) 2003 Krzysztof Nikiel
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
 * $Id: midside.c,v 1.1 2003/06/26 19:39:54 knik Exp $
 */

#include <math.h>
#include "channels.h"
#include "util.h"


void MSEncode(CoderInfo *coderInfo,
	      ChannelInfo *channelInfo,
	      double *spectrum[MAX_CHANNELS],
	      int maxchan,
	      int allowms)
{
  int chn;

  for (chn = 0; chn < maxchan; chn++)
  {
    if (channelInfo[chn].present)
    {
      if ((channelInfo[chn].cpe) && (channelInfo[chn].ch_is_left))
      {
	int rch = channelInfo[chn].paired_ch;

	channelInfo[chn].msInfo.is_present = 0;
	channelInfo[rch].msInfo.is_present = 0;

	/* Perform MS if block_types are the same */
	if ((coderInfo[chn].block_type == coderInfo[rch].block_type)
	    && allowms)
	{
	  int nsfb = coderInfo[chn].nr_of_sfb;
	  MSInfo *msInfoL = &(channelInfo[chn].msInfo);
	  MSInfo *msInfoR = &(channelInfo[rch].msInfo);
	  int sfb;

	  channelInfo[chn].common_window = 1;  /* Use common window */
	  channelInfo[chn].msInfo.is_present = 1;
	  channelInfo[rch].msInfo.is_present = 1;

          // make the same reference energy in both channels
	  coderInfo[chn].avgenrg = coderInfo[rch].avgenrg =
	    0.5 * (coderInfo[chn].avgenrg + coderInfo[rch].avgenrg);

	  for (sfb = 0; sfb < nsfb; sfb++)
	  {
            int ms = 0;
	    int l, start, end;
	    double sum, diff;
            double enrgs, enrgd, enrgl, enrgr;
	    double maxs, maxd, maxl, maxr;

	    start = coderInfo[chn].sfb_offset[sfb];
            end = coderInfo[chn].sfb_offset[sfb + 1];

            enrgs = enrgd = enrgl = enrgr = 0.0;
	    maxs = maxd = maxl = maxr = 0.0;
	    for (l = start; l < end; l++)
	    {
              double lx = spectrum[chn][l];
	      double rx = spectrum[rch][l];

	      sum = 0.5 * (lx + rx);
	      diff = 0.5 * (lx - rx);

	      enrgs += sum * sum;
	      maxs = max(maxs, fabs(sum));

	      enrgd += diff * diff;
	      maxd = max(maxd, fabs(diff));

	      enrgl += lx * lx;
	      enrgr += rx * rx;

              maxl = max(maxl, fabs(lx));
              maxr = max(maxr, fabs(rx));
	    }

#if 1
	    if ((min(enrgs, enrgd) < min(enrgl, enrgr))
		&& (min(maxs, maxd) < min(maxl, maxr)))
	      ms = 1;
#else
	    if (min(enrgs, enrgd) < min(enrgl, enrgr))
	      ms = 1;
#endif

	    //printf("%d:%d\n", sfb, ms);

	    msInfoR->ms_used[sfb] = msInfoL->ms_used[sfb] = ms;

	    if (ms)
	      for (l = start; l < end; l++)
	      {
		sum = spectrum[chn][l] + spectrum[rch][l];
		diff = spectrum[chn][l] - spectrum[rch][l];
		spectrum[chn][l] = 0.5 * sum;
		spectrum[rch][l] = 0.5 * diff;
	      }
	  }
	}
      }
    }
  }
}

void MSReconstruct(CoderInfo *coderInfo,
		   ChannelInfo *channelInfo,
		   int maxchan)
{
  int chn;

  for (chn = 0; chn < maxchan; chn++)
  {
    if (channelInfo[chn].present)
    {
      if (channelInfo[chn].cpe && channelInfo[chn].ch_is_left)
      {
	int rch = channelInfo[chn].paired_ch;

	MSInfo *msInfoL = &(channelInfo[chn].msInfo);

	if (msInfoL->is_present) {
	  int nsfb = coderInfo[chn].nr_of_sfb;
	  int sfb;

	  for (sfb = 0; sfb < nsfb; sfb++)
	  {
	    int l, start, end;

	    start = coderInfo[chn].sfb_offset[sfb];
	    end = coderInfo[chn].sfb_offset[sfb + 1];

	    if (msInfoL->ms_used[sfb])
	    {
	      for (l = start; l < end; l++)
	      {
		double sum, diff;

		sum = coderInfo[chn].requantFreq[l];
		diff = coderInfo[rch].requantFreq[l];
		coderInfo[chn].requantFreq[l] = sum + diff;
		coderInfo[rch].requantFreq[l] = sum - diff;
	      }
	    }
	  }
	}
      }
    }
  }
}

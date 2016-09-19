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
 * $Id: backpred.h,v 1.5 2001/06/08 18:01:09 menno Exp $
 */

#ifndef _AAC_BACK_H_INCLUDED
#define _AAC_BACK_H_INCLUDED

#define PRED_ALPHA  0.90625
#define PRED_A      0.953125
#define PRED_B      0.953125

#define ALPHA PRED_ALPHA
#define A PRED_A
#define B PRED_B
#define MINVAR 1.e-10

/* Reset every RESET_FRAME frames. */
#define RESET_FRAME 8

void PredCalcPrediction(double *act_spec,
                        double *last_spec,
                        int btype,
                        int nsfb,
                        int *isfb_width,
                        CoderInfo *coderInfo,
                        ChannelInfo *channelInfo,
                        int chanNum);

void PredInit(faacEncHandle hEncoder);

void CopyPredInfo(CoderInfo *right, CoderInfo *left);


#endif

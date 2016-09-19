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
 * $Id: filtbank.h,v 1.11 2005/02/02 07:51:49 sur Exp $
 */

#ifndef FILTBANK_H
#define FILTBANK_H

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#include "frame.h"

#ifdef DRM
#define NFLAT_LS (( BLOCK_LEN_LONG - BLOCK_LEN_SHORT ) / 2)
#else
#define NFLAT_LS 448
#endif

#define MOVERLAPPED     0
#define MNON_OVERLAPPED 1


#define SINE_WINDOW 0
#define KBD_WINDOW  1

void			FilterBankInit		( faacEncHandle hEncoder );

void			FilterBankEnd		( faacEncHandle hEncoder );

void			FilterBank( faacEncHandle hEncoder,
						CoderInfo *coderInfo,
						double *p_in_data,
						double *p_out_mdct,
						double *p_overlap,
						int overlap_select );

void			IFilterBank( faacEncHandle hEncoder,
						CoderInfo *coderInfo,
						double *p_in_data,
						double *p_out_mdct,
						double *p_overlap,
						int overlap_select );

void			specFilter(	double *freqBuff,
						int sampleRate,
						int lowpassFreq,
						int specLen );

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* FILTBANK_H */

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
 * $Id: ltp.h,v 1.3 2001/06/08 18:01:09 menno Exp $
 */

#ifndef LTP_H
#define LTP_H

#include "coder.h"



void LtpInit(faacEncHandle hEncoder);
void LtpEnd(faacEncHandle hEncoder);
int LtpEncode(faacEncHandle hEncoder,
                CoderInfo *coderInfo,
                LtpInfo *ltpInfo,
                TnsInfo *tnsInfo,
                double *p_spectrum,
                double *p_time_signal);
void LtpReconstruct(CoderInfo *coderInfo, LtpInfo *ltpInfo, double *p_spectrum);
void  LtpUpdate(LtpInfo *ltpInfo, double *time_signal,
                     double *overlap_signal, int block_size_long);

#endif /* not defined LTP_H */


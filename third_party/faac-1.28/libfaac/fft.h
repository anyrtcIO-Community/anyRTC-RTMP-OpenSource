/*
 * FAAC - Freeware Advanced Audio Coder
 * $Id: fft.h,v 1.6 2005/02/02 07:50:35 sur Exp $
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
 */

#ifndef _FFT_H_
#define _FFT_H_

typedef float fftfloat;

#if defined DRM && !defined DRM_1024

#define MAX_FFT 10

typedef struct
{
    /*      cfg[Max FFT][FFT and inverse FFT] */
    void*   cfg[MAX_FFT][2];
} FFT_Tables;

#else  /* use own FFT */

typedef struct
{
    fftfloat **costbl;
    fftfloat **negsintbl;
    unsigned short **reordertbl;
} FFT_Tables;

#endif /* defined DRM && !defined DRM_1024 */

void fft_initialize		( FFT_Tables *fft_tables );
void fft_terminate	( FFT_Tables *fft_tables );

void rfft			( FFT_Tables *fft_tables, double *x, int logm );
void fft			( FFT_Tables *fft_tables, double *xr, double *xi, int logm );
void ffti			( FFT_Tables *fft_tables, double *xr, double *xi, int logm );

#endif

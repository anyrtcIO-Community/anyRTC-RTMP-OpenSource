/*
 * FAAC - Freeware Advanced Audio Coder
 * $Id: fft.c,v 1.12 2005/02/02 07:49:55 sur Exp $
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

#include <math.h>
#include <stdlib.h>
#include <stdio.h>

#include "fft.h"
#include "util.h"

#define MAXLOGM 9
#define MAXLOGR 8

#if defined DRM && !defined DRM_1024

#include "kiss_fft/kiss_fft.h"
#include "kiss_fft/kiss_fftr.h"

static const int logm_to_nfft[] = 
{
/*  0       1       2       3       */
    0,      0,      0,      0,
/*  4       5       6       7       */
    0,      0,      60,     0,
/*  8       9                       */
    240,    480
};

void fft_initialize( FFT_Tables *fft_tables )
{
    memset( fft_tables->cfg, 0, sizeof( fft_tables->cfg ) );
}
void fft_terminate( FFT_Tables *fft_tables )
{
    unsigned int i;
    for ( i = 0; i < sizeof( fft_tables->cfg ) / sizeof( fft_tables->cfg[0] ); i++ )
    {
        if ( fft_tables->cfg[i][0] )
        {
            free( fft_tables->cfg[i][0] );
            fft_tables->cfg[i][0] = NULL;
        }
        if ( fft_tables->cfg[i][1] )
        {
            free( fft_tables->cfg[i][1] );
            fft_tables->cfg[i][1] = NULL;
        }
    }
}

void rfft( FFT_Tables *fft_tables, double *x, int logm )
{
#if 0
/* sur: do not use real-only optimized FFT */
    double xi[1 << MAXLOGR];

    int nfft;

    if ( logm > MAXLOGR )
	{
		fprintf(stderr, "rfft size too big\n");
		exit(1);
	}

    nfft = logm_to_nfft[logm];

    if ( nfft )
    {
        //unsigned int i;
        //for ( i = 0; i < nfft; i++ )
        //{
        //    xi[i] = 0.0;
        //}
	    memset( xi, 0, nfft * sizeof( xi[0] ) );

	    fft( fft_tables, x, xi, logm );

	    memcpy( x + nfft / 2, xi, ( nfft / 2 ) * sizeof(x[0]) );
    }
    else
    {
        fprintf( stderr, "bad config for logm = %d\n", logm);
        exit( 1 );
    }

#else
/* sur: use real-only optimized FFT */

    int nfft = 0;

    kiss_fft_scalar fin[1 << MAXLOGR];
    kiss_fft_cpx    fout[1 << MAXLOGR];

    if ( logm > MAXLOGR )
	{
		fprintf(stderr, "fft size too big\n");
		exit(1);
	}

    nfft = logm_to_nfft[logm];

    if ( fft_tables->cfg[logm][0] == NULL )
    {
        if ( nfft )
        {
            fft_tables->cfg[logm][0] = kiss_fftr_alloc( nfft, 0, NULL, NULL );
        }
        else
        {
	    	fprintf(stderr, "bad logm = %d\n", logm);
            exit( 1 );
        }
    }

    if ( fft_tables->cfg[logm][0] )
    {
        unsigned int i;
        
        for ( i = 0; i < nfft; i++ )
        {
            fin[i] = x[i];    
        }
        
        kiss_fftr( (kiss_fftr_cfg)fft_tables->cfg[logm][0], fin, fout );

        for ( i = 0; i < nfft / 2; i++ )
        {
            x[i]            = fout[i].r;
            x[i + nfft / 2] = fout[i].i;
        }
    }
    else
    {
        fprintf( stderr, "bad config for logm = %d\n", logm);
        exit( 1 );
    }
#endif
}

void fft( FFT_Tables *fft_tables, double *xr, double *xi, int logm )
{
    int nfft = 0;

    kiss_fft_cpx    fin[1 << MAXLOGM];
    kiss_fft_cpx    fout[1 << MAXLOGM];

    if ( logm > MAXLOGM )
	{
		fprintf(stderr, "fft size too big\n");
		exit(1);
	}

    nfft = logm_to_nfft[logm];

    if ( fft_tables->cfg[logm][0] == NULL )
    {
        if ( nfft )
        {
            fft_tables->cfg[logm][0] = kiss_fft_alloc( nfft, 0, NULL, NULL );
        }
        else
        {
	    	fprintf(stderr, "bad logm = %d\n", logm);
            exit( 1 );
        }
    }

    if ( fft_tables->cfg[logm][0] )
    {
        unsigned int i;
        
        for ( i = 0; i < nfft; i++ )
        {
            fin[i].r = xr[i];    
            fin[i].i = xi[i];
        }
        
        kiss_fft( (kiss_fft_cfg)fft_tables->cfg[logm][0], fin, fout );

        for ( i = 0; i < nfft; i++ )
        {
            xr[i]   = fout[i].r;
            xi[i]   = fout[i].i;
        }
    }
    else
    {
        fprintf( stderr, "bad config for logm = %d\n", logm);
        exit( 1 );
    }
}

void ffti( FFT_Tables *fft_tables, double *xr, double *xi, int logm )
{
    int nfft = 0;

    kiss_fft_cpx    fin[1 << MAXLOGM];
    kiss_fft_cpx    fout[1 << MAXLOGM];

    if ( logm > MAXLOGM )
	{
		fprintf(stderr, "fft size too big\n");
		exit(1);
	}

    nfft = logm_to_nfft[logm];

    if ( fft_tables->cfg[logm][1] == NULL )
    {
        if ( nfft )
        {
            fft_tables->cfg[logm][1] = kiss_fft_alloc( nfft, 1, NULL, NULL );
        }
        else
        {
	    	fprintf(stderr, "bad logm = %d\n", logm);
            exit( 1 );
        }
    }
    
    if ( fft_tables->cfg[logm][1] )
    {
        unsigned int i;
        double fac = 1.0 / (double)nfft;
        
        for ( i = 0; i < nfft; i++ )
        {
            fin[i].r = xr[i];    
            fin[i].i = xi[i];
        }
        
        kiss_fft( (kiss_fft_cfg)fft_tables->cfg[logm][1], fin, fout );

        for ( i = 0; i < nfft; i++ )
        {
            xr[i]   = fout[i].r * fac;
            xi[i]   = fout[i].i * fac;
        }
    }
    else
    {
        fprintf( stderr, "bad config for logm = %d\n", logm);
        exit( 1 );
    }
}

/* sur: Trying to use cfft from libfaad2 -- it does not work 'from scratch' */
//
//#include "cfft/common.h"
//
//void fft_initialize( FFT_Tables *fft_tables )
//{
//    memset( fft_tables->cfft, 0, sizeof( fft_tables->cfft ) );
//}
//void fft_terminate( FFT_Tables *fft_tables )
//{
//    unsigned int i;
//    for ( i = 0; i < sizeof( fft_tables->cfft ) / sizeof( fft_tables->cfft[0] ); i++ )
//    {
//        if ( fft_tables->cfft[i] )
//        {
//            cfftu( fft_tables->cfft[i] );
//            fft_tables->cfft[i] = NULL;
//        }
//    }
//}
//
//void rfft( FFT_Tables *fft_tables, double *x, int logm )
//{
//	double xi[1 << MAXLOGR];
//
//    int nfft;
//
//    if ( logm > MAXLOGR )
//	{
//		fprintf(stderr, "rfft size too big\n");
//		exit(1);
//	}
//
//    nfft = logm_to_nfft[logm];
//
//    if ( nfft )
//    {
//        unsigned int i;
//
//        for ( i = 0; i < nfft; i++ )
//        {
//            xi[i] = 0.0;
//        }
//	    //memset( xi, 0, nfft * sizeof( xi[0] ) );
//
//	    fft( fft_tables, x, xi, logm );
//
//	    memcpy( x + nfft / 2, xi, ( nfft / 2 ) * sizeof(x[0]) );
//
//#ifdef SUR_DEBUG_FFT
//        {
//            FILE* f = fopen( "fft.log", "at" );
//            
//            fprintf( f, "RFFT(%d)\n", nfft );
//            
//            for ( i = 0; i < nfft; i++ )
//            {
//                fprintf( f, ";%d;%g;%g\n", i, x[i], xi[i] );
//            }
//
//            fclose( f );
//        }
//#endif
//    }
//    else
//    {
//        fprintf( stderr, "bad config for logm = %d\n", logm);
//        exit( 1 );
//    }
//}
//
//void fft( FFT_Tables *fft_tables, double *xr, double *xi, int logm )
//{
//    int nfft;
//
//    complex_t c[1 << MAXLOGM];
//
//    if ( logm > MAXLOGM )
//	{
//		fprintf(stderr, "fft size too big\n");
//		exit(1);
//	}
//
//    nfft = logm_to_nfft[logm];
//
//    if ( fft_tables->cfft[logm] == NULL )
//    {
//        if ( nfft )
//        {
//            fft_tables->cfft[logm] = cffti( nfft );
//        }
//        else
//        {
//	    	fprintf(stderr, "bad logm = %d\n", logm);
//            exit( 1 );
//        }
//    }
//
//    if ( fft_tables->cfft[logm] )
//    {
//        unsigned int i;
//        
//        for ( i = 0; i < nfft; i++ )
//        {
//            RE( c[i] ) = xr[i];    
//            IM( c[i] ) = xi[i];
//        }
//        
//        cfftf( fft_tables->cfft[logm], c );
//
//        for ( i = 0; i < nfft; i++ )
//        {
//            xr[i]   = RE( c[i] );
//            xi[i]   = IM( c[i] );
//        }
//
//#ifdef SUR_DEBUG_FFT
//        {
//            FILE* f = fopen( "fft.log", "at" );
//            
//            fprintf( f, "FFT(%d)\n", nfft );
//            
//            for ( i = 0; i < nfft; i++ )
//            {
//                fprintf( f, ";%d;%g;%g\n", i, xr[i], xi[i] );
//            }
//
//            fclose( f );
//        }
//#endif
//    }
//    else
//    {
//        fprintf( stderr, "bad config for logm = %d\n", logm);
//        exit( 1 );
//    }
//}
//
//void ffti( FFT_Tables *fft_tables, double *xr, double *xi, int logm )
//{
//    int nfft;
//
//    complex_t c[1 << MAXLOGM];
//
//    if ( logm > MAXLOGM )
//	{
//		fprintf(stderr, "fft size too big\n");
//		exit(1);
//	}
//
//    nfft = logm_to_nfft[logm];
//
//    if ( fft_tables->cfft[logm] == NULL )
//    {
//        if ( nfft )
//        {
//            fft_tables->cfft[logm] = cffti( nfft );
//        }
//        else
//        {
//	    	fprintf(stderr, "bad logm = %d\n", logm);
//            exit( 1 );
//        }
//    }
//    
//    if ( fft_tables->cfft[logm] )
//    {
//        unsigned int i;
//        
//        for ( i = 0; i < nfft; i++ )
//        {
//            RE( c[i] )  = xr[i];    
//            IM( c[i] )  = xi[i];
//        }
//        
//        cfftb( fft_tables->cfft[logm], c );
//
//        for ( i = 0; i < nfft; i++ )
//        {
//            xr[i]   = RE( c[i] );
//            xi[i]   = IM( c[i] );
//        }
//
//#ifdef SUR_DEBUG_FFT
//        {
//            FILE* f = fopen( "fft.log", "at" );
//            
//            fprintf( f, "FFTI(%d)\n", nfft );
//            
//            for ( i = 0; i < nfft; i++ )
//            {
//                fprintf( f, ";%d;%g;%g\n", i, xr[i], xi[i] );
//            }
//
//            fclose( f );
//        }
//#endif
//    }
//    else
//    {
//        fprintf( stderr, "bad config for logm = %d\n", logm);
//        exit( 1 );
//    }
//}

#else /* !defined DRM || defined DRM_1024 */

void fft_initialize( FFT_Tables *fft_tables )
{
	int i;
	fft_tables->costbl		= AllocMemory( (MAXLOGM+1) * sizeof( fft_tables->costbl[0] ) );
	fft_tables->negsintbl	= AllocMemory( (MAXLOGM+1) * sizeof( fft_tables->negsintbl[0] ) );
	fft_tables->reordertbl	= AllocMemory( (MAXLOGM+1) * sizeof( fft_tables->reordertbl[0] ) );
	
	for( i = 0; i< MAXLOGM+1; i++ )
	{
		fft_tables->costbl[i]		= NULL;
		fft_tables->negsintbl[i]	= NULL;
		fft_tables->reordertbl[i]	= NULL;
	}
}

void fft_terminate( FFT_Tables *fft_tables )
{
	int i;

	for( i = 0; i< MAXLOGM+1; i++ )
	{
		if( fft_tables->costbl[i] != NULL )
			FreeMemory( fft_tables->costbl[i] );
		
		if( fft_tables->negsintbl[i] != NULL )
			FreeMemory( fft_tables->negsintbl[i] );
			
		if( fft_tables->reordertbl[i] != NULL )
			FreeMemory( fft_tables->reordertbl[i] );
	}

	FreeMemory( fft_tables->costbl );
	FreeMemory( fft_tables->negsintbl );
	FreeMemory( fft_tables->reordertbl );

	fft_tables->costbl		= NULL;
	fft_tables->negsintbl	= NULL;
	fft_tables->reordertbl	= NULL;
}

static void reorder( FFT_Tables *fft_tables, double *x, int logm)
{
	int i;
	int size = 1 << logm;
	unsigned short *r;	//size


	if ( fft_tables->reordertbl[logm] == NULL ) // create bit reversing table
	{
		fft_tables->reordertbl[logm] = AllocMemory(size * sizeof(*(fft_tables->reordertbl[0])));

		for (i = 0; i < size; i++)
		{
			int reversed = 0;
			int b0;
			int tmp = i;

			for (b0 = 0; b0 < logm; b0++)
			{
				reversed = (reversed << 1) | (tmp & 1);
				tmp >>= 1;
			}
			fft_tables->reordertbl[logm][i] = reversed;
		}
	}

	r = fft_tables->reordertbl[logm];

	for (i = 0; i < size; i++)
	{
		int j = r[i];
		double tmp;

		if (j <= i)
			continue;

		tmp = x[i];
		x[i] = x[j];
		x[j] = tmp;
	}
}

static void fft_proc(
		double *xr, 
		double *xi,
		fftfloat *refac, 
		fftfloat *imfac, 
		int size)	
{
	int step, shift, pos;
	int exp, estep;

	estep = size;
	for (step = 1; step < size; step *= 2)
	{
		int x1;
		int x2 = 0;
		estep >>= 1;
		for (pos = 0; pos < size; pos += (2 * step))
		{
			x1 = x2;
			x2 += step;
			exp = 0;
			for (shift = 0; shift < step; shift++)
			{
				double v2r, v2i;

				v2r = xr[x2] * refac[exp] - xi[x2] * imfac[exp];
				v2i = xr[x2] * imfac[exp] + xi[x2] * refac[exp];

				xr[x2] = xr[x1] - v2r;
				xr[x1] += v2r;

				xi[x2] = xi[x1] - v2i;

				xi[x1] += v2i;

				exp += estep;

				x1++;
				x2++;
			}
		}
	}
}

static void check_tables( FFT_Tables *fft_tables, int logm)
{
	if( fft_tables->costbl[logm] == NULL )
	{
		int i;
		int size = 1 << logm;

		if( fft_tables->negsintbl[logm] != NULL )
			FreeMemory( fft_tables->negsintbl[logm] );

		fft_tables->costbl[logm]	= AllocMemory((size / 2) * sizeof(*(fft_tables->costbl[0])));
		fft_tables->negsintbl[logm]	= AllocMemory((size / 2) * sizeof(*(fft_tables->negsintbl[0])));

		for (i = 0; i < (size >> 1); i++)
		{
			double theta = 2.0 * M_PI * ((double) i) / (double) size;
			fft_tables->costbl[logm][i]		= cos(theta);
			fft_tables->negsintbl[logm][i]	= -sin(theta);
		}
	}
}

void fft( FFT_Tables *fft_tables, double *xr, double *xi, int logm)
{
	if (logm > MAXLOGM)
	{
		fprintf(stderr, "fft size too big\n");
		exit(1);
	}

	if (logm < 1)
	{
		//printf("logm < 1\n");
		return;
	}

	check_tables( fft_tables, logm);

	reorder( fft_tables, xr, logm);
	reorder( fft_tables, xi, logm);

	fft_proc( xr, xi, fft_tables->costbl[logm], fft_tables->negsintbl[logm], 1 << logm );
}

void rfft( FFT_Tables *fft_tables, double *x, int logm)
{
	double xi[1 << MAXLOGR];

	if (logm > MAXLOGR)
	{
		fprintf(stderr, "rfft size too big\n");
		exit(1);
	}

	memset(xi, 0, (1 << logm) * sizeof(xi[0]));

	fft( fft_tables, x, xi, logm);

	memcpy(x + (1 << (logm - 1)), xi, (1 << (logm - 1)) * sizeof(*x));
}

void ffti( FFT_Tables *fft_tables, double *xr, double *xi, int logm)
{
	int i, size;
	double fac;
	double *xrp, *xip;

	fft( fft_tables, xi, xr, logm);

	size = 1 << logm;
	fac = 1.0 / size;
	xrp = xr;
	xip = xi;

	for (i = 0; i < size; i++)
	{
		*xrp++ *= fac;
		*xip++ *= fac;
	}
}

#endif /* defined DRM && !defined DRM_1024 */

/*
$Log: fft.c,v $
Revision 1.12  2005/02/02 07:49:55  sur
Added interface to kiss_fft library to implement FFT for 960 transform length.

Revision 1.11  2004/04/02 14:56:17  danchr
fix name clash w/ libavcodec: fft_init -> fft_initialize
bump version number to 1.24 beta

Revision 1.10  2003/11/16 05:02:51  stux
moved global tables from fft.c into hEncoder FFT_Tables. Add fft_init and fft_terminate, flowed through all necessary changes. This should remove at least one instance of a memory leak, and fix some thread-safety problems. Version update to 1.23.3

Revision 1.9  2003/09/07 16:48:01  knik
reduced arrays size

Revision 1.8  2002/11/23 17:32:54  knik
rfft: made xi a local variable

Revision 1.7  2002/08/21 16:52:25  knik
new simplier and faster fft routine and correct real fft
new real fft is just a complex fft wrapper so it is slower than optimal but
by surprise it seems to be at least as fast as the old buggy function

*/

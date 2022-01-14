/************************* MPEG-2 NBC Audio Decoder **************************
 *                                                                           *
"This software module was originally developed in the course of
development of the MPEG-2 NBC/MPEG-4 Audio standard ISO/IEC 13818-7,
14496-1,2 and 3. This software module is an implementation of a part of one or more
MPEG-2 NBC/MPEG-4 Audio tools as specified by the MPEG-2 NBC/MPEG-4
Audio standard. ISO/IEC  gives users of the MPEG-2 NBC/MPEG-4 Audio
standards free license to this software module or modifications thereof for use in
hardware or software products claiming conformance to the MPEG-2 NBC/MPEG-4
Audio  standards. Those intending to use this software module in hardware or
software products are advised that this use may infringe existing patents.
The original developer of this software module and his/her company, the subsequent
editors and their companies, and ISO/IEC have no liability for use of this software
module or modifications thereof in an implementation. Copyright is not released for
non MPEG-2 NBC/MPEG-4 Audio conforming products.The original developer
retains full right to use the code for his/her  own purpose, assign or donate the
code to a third party and to inhibit third party from using the code for non
MPEG-2 NBC/MPEG-4 Audio conforming products. This copyright notice must
be included in all copies or derivative works."
Copyright(c)1996.
 *                                                                           *
 ****************************************************************************/
/*
 * $Id: channels.c,v 1.5 2001/09/04 18:39:35 menno Exp $
 */

#include "channels.h"
#include "coder.h"
#include "util.h"

/* If LFE present                                                       */
/*  Num channels       # of SCE's       # of CPE's       #of LFE's      */
/*  ============       ==========       ==========       =========      */
/*      1                  1                0               0           */
/*      2                  0                1               0           */
/*      3                  1                1               0           */
/*      4                  1                1               1           */
/*      5                  1                2               0           */
/* For more than 5 channels, use the following elements:                */
/*      2*N                1                2*(N-1)         1           */
/*      2*N+1              1                2*N             0           */
/*                                                                      */
/* Else:                                                                */
/*                                                                      */
/*  Num channels       # of SCE's       # of CPE's       #of LFE's      */
/*  ============       ==========       ==========       =========      */
/*      1                  1                0               0           */
/*      2                  0                1               0           */
/*      3                  1                1               0           */
/*      4                  2                1               0           */
/*      5                  1                2               0           */
/* For more than 5 channels, use the following elements:                */
/*      2*N                2                2*(N-1)         0           */
/*      2*N+1              1                2*N             0           */

void GetChannelInfo(ChannelInfo *channelInfo, int numChannels, int useLfe)
{
    int sceTag = 0;
    int lfeTag = 0;
    int cpeTag = 0;
    int numChannelsLeft = numChannels;


    /* First element is sce, except for 2 channel case */
    if (numChannelsLeft != 2) {
        channelInfo[numChannels-numChannelsLeft].present = 1;
        channelInfo[numChannels-numChannelsLeft].tag = sceTag++;
        channelInfo[numChannels-numChannelsLeft].cpe = 0;
        channelInfo[numChannels-numChannelsLeft].lfe = 0;
        numChannelsLeft--;
    }

    /* Next elements are cpe's */
    while (numChannelsLeft > 1) {
        /* Left channel info */
        channelInfo[numChannels-numChannelsLeft].present = 1;
        channelInfo[numChannels-numChannelsLeft].tag = cpeTag++;
        channelInfo[numChannels-numChannelsLeft].cpe = 1;
        channelInfo[numChannels-numChannelsLeft].common_window = 0;
        channelInfo[numChannels-numChannelsLeft].ch_is_left = 1;
        channelInfo[numChannels-numChannelsLeft].paired_ch = numChannels-numChannelsLeft+1;
        channelInfo[numChannels-numChannelsLeft].lfe = 0;
        numChannelsLeft--;

        /* Right channel info */
        channelInfo[numChannels-numChannelsLeft].present = 1;
        channelInfo[numChannels-numChannelsLeft].cpe = 1;
        channelInfo[numChannels-numChannelsLeft].common_window = 0;
        channelInfo[numChannels-numChannelsLeft].ch_is_left = 0;
        channelInfo[numChannels-numChannelsLeft].paired_ch = numChannels-numChannelsLeft-1;
        channelInfo[numChannels-numChannelsLeft].lfe = 0;
        numChannelsLeft--;
    }

    /* Is there another channel left ? */
    if (numChannelsLeft) {
        if (useLfe) {
            channelInfo[numChannels-numChannelsLeft].present = 1;
            channelInfo[numChannels-numChannelsLeft].tag = lfeTag++;
            channelInfo[numChannels-numChannelsLeft].cpe = 0;
            channelInfo[numChannels-numChannelsLeft].lfe = 1;
        } else {
            channelInfo[numChannels-numChannelsLeft].present = 1;
            channelInfo[numChannels-numChannelsLeft].tag = sceTag++;
            channelInfo[numChannels-numChannelsLeft].cpe = 0;
            channelInfo[numChannels-numChannelsLeft].lfe = 0;
        }
        numChannelsLeft--;
    }
}

/**********************************************************************

This software module was originally developed by
and edited by Nokia in the course of
development of the MPEG-2 NBC/MPEG-4 Audio standard
ISO/IEC 13818-7, 14496-1,2 and 3. This software module is an
implementation of a part of one or more MPEG-2 NBC/MPEG-4 Audio tools
as specified by the MPEG-2 NBC/MPEG-4 Audio standard. ISO/IEC gives
users of the MPEG-2 NBC/MPEG-4 Audio standards free license to this
software module or modifications thereof for use in hardware or
software products claiming conformance to the MPEG-2 NBC/ MPEG-4 Audio
standards. Those intending to use this software module in hardware or
software products are advised that this use may infringe existing
patents. The original developer of this software module and his/her
company, the subsequent editors and their companies, and ISO/IEC have
no liability for use of this software module or modifications thereof
in an implementation. Copyright is not released for non MPEG-2
NBC/MPEG-4 Audio conforming products. The original developer retains
full right to use the code for his/her own purpose, assign or donate
the code to a third party and to inhibit third party from using the
code for non MPEG-2 NBC/MPEG-4 Audio conforming products. This
copyright notice must be included in all copies or derivative works.

Copyright (c) 1997.
**********************************************************************/
/*
 * $Id: backpred.c,v 1.5 2001/09/04 18:39:35 menno Exp $
 */

#include <math.h>
#include "frame.h"
#include "coder.h"
#include "channels.h"
#include "backpred.h"


void PredInit(faacEncHandle hEncoder)
{
    unsigned int channel;

    for (channel = 0; channel < hEncoder->numChannels; channel++) {
        BwpInfo *bwpInfo = &(hEncoder->coderInfo[channel].bwpInfo);

        bwpInfo->psy_init_mc = 0;
        bwpInfo->reset_count_mc = 0;
    }
}

void PredCalcPrediction(double *act_spec, double *last_spec, int btype,
                        int nsfb,
                        int *isfb_width,
                        CoderInfo *coderInfo,
                        ChannelInfo *channelInfo,
                        int chanNum)
{
    int i, k, j, cb_long;
    int leftChanNum;
    int isRightWithCommonWindow;
    double num_bit, snr[SBMAX_L];
    double energy[BLOCK_LEN_LONG], snr_p[BLOCK_LEN_LONG], temp1, temp2;
    ChannelInfo *thisChannel;

    /* Set pointers for specified channel number */
    /* int psy_init; */
    int *psy_init;
    double (*dr)[BLOCK_LEN_LONG],(*e)[BLOCK_LEN_LONG];
    double (*K)[BLOCK_LEN_LONG], (*R)[BLOCK_LEN_LONG];
    double (*VAR)[BLOCK_LEN_LONG], (*KOR)[BLOCK_LEN_LONG];
    double *sb_samples_pred;
    int *thisLineNeedsResetting;
    /* int reset_count; */
    int *reset_count;
    int *pred_global_flag;
    int *pred_sfb_flag;
    int *reset_group;

    /* Set pointers for this chanNum */
    pred_global_flag = &(coderInfo[chanNum].pred_global_flag);
    pred_sfb_flag = coderInfo[chanNum].pred_sfb_flag;
    reset_group = &(coderInfo[chanNum].reset_group_number);
    psy_init = &coderInfo[chanNum].bwpInfo.psy_init_mc;
    dr = &coderInfo[chanNum].bwpInfo.dr_mc[0];
    e = &coderInfo[chanNum].bwpInfo.e_mc[0];
    K = &coderInfo[chanNum].bwpInfo.K_mc[0];
    R = &coderInfo[chanNum].bwpInfo.R_mc[0];
    VAR = &coderInfo[chanNum].bwpInfo.VAR_mc[0];
    KOR = &coderInfo[chanNum].bwpInfo.KOR_mc[0];
    sb_samples_pred = &coderInfo[chanNum].bwpInfo.sb_samples_pred_mc[0];
    thisLineNeedsResetting = &coderInfo[chanNum].bwpInfo.thisLineNeedsResetting_mc[0];
    reset_count = &coderInfo[chanNum].bwpInfo.reset_count_mc;

    thisChannel = &(channelInfo[chanNum]);
    *psy_init = (*psy_init && (btype!=2));

    if((*psy_init) == 0) {
        for (j=0; j<BLOCK_LEN_LONG; j++) {
            thisLineNeedsResetting[j]=1;
        }
        *psy_init = 1;
    }

    if (btype==2) {
        pred_global_flag[0]=0;
        /* SHORT WINDOWS reset all the co-efficients    */
        if (thisChannel->ch_is_left) {
            (*reset_count)++;
            if (*reset_count >= 31 * RESET_FRAME)
                *reset_count = RESET_FRAME;
        }
        return;
    }


    /**************************************************/
    /*  Compute state using last_spec                 */
    /**************************************************/
    for (i=0;i<BLOCK_LEN_LONG;i++)
    {
        /* e[0][i]=last_spec[i]; */
        e[0][i]=last_spec[i]+sb_samples_pred[i];

        for(j=1;j<=LPC;j++)
            e[j][i] = e[j-1][i]-K[j][i]*R[j-1][i];

        for(j=1;j<LPC;j++)
            dr[j][i] = K[j][i]*e[j-1][i];

        for(j=1;j<=LPC;j++) {
            VAR[j][i] = ALPHA*VAR[j][i]+.5*(R[j-1][i]*R[j-1][i]+e[j-1][i]*e[j-1][i]);
            KOR[j][i] = ALPHA*KOR[j][i]+R[j-1][i]*e[j-1][i];
        }

        for(j=LPC-1;j>=1;j--)
            R[j][i] = A*(R[j-1][i]-dr[j][i]);
        R[0][i] = A*e[0][i];
    }


    /**************************************************/
    /* Reset state here if resets were sent           */
    /**************************************************/
    for (i=0;i<BLOCK_LEN_LONG;i++) {
        if (thisLineNeedsResetting[i]) {
            for (j = 0; j <= LPC; j++)
            {
                K[j][i] = 0.0;
                e[j][i] = 0.0;
                R[j][i] = 0.0;
                VAR[j][i] = 1.0;
                KOR[j][i] = 0.0;
                dr[j][i] = 0.0;
            }
        }
    }



    /**************************************************/
    /* Compute predictor coefficients, predicted data */
    /**************************************************/
    for (i=0;i<BLOCK_LEN_LONG;i++)
    {
        for(j=1;j<=LPC;j++) {
            if(VAR[j][i]>MINVAR)
                K[j][i] = KOR[j][i]/VAR[j][i]*B;
            else
                K[j][i] = 0;
        }
    }


    for (k=0; k<BLOCK_LEN_LONG; k++)
    {
        sb_samples_pred[k]=0.0;
        for (i=1; i<=LPC; i++)
            sb_samples_pred[k]+=K[i][k]*R[i-1][k];
    }


    /***********************************************************/
    /* If this is the right channel of a channel_pair_element, */
    /* AND common_window is 1 in this channel_pair_element,    */
    /* THEN copy predictor data to use from the left channel.  */
    /* ELSE determine independent predictor data and resets.   */
    /***********************************************************/
    /* BE CAREFUL HERE, this assumes that predictor data has   */
    /* already been determined for the left channel!!          */
    /***********************************************************/
    isRightWithCommonWindow = 0;     /* Is this a right channel with common_window?*/
    if ((thisChannel->cpe)&&( !(thisChannel->ch_is_left))) {
        leftChanNum = thisChannel->paired_ch;
        if (channelInfo[leftChanNum].common_window) {
            isRightWithCommonWindow = 1;
        }
    }

    if (isRightWithCommonWindow) {

        /**************************************************/
        /* Use predictor data from the left channel.      */
        /**************************************************/
        CopyPredInfo(&(coderInfo[chanNum]),&(coderInfo[leftChanNum]));

        /* Make sure to turn off bands with intensity stereo */
#if 0
        if (thisChannel->is_info.is_present) {
            for (i=0; i<nsfb; i++) {
                if (thisChannel->is_info.is_used[i]) {
                    pred_sfb_flag[i] = 0;
                }
            }
        }
#endif

        cb_long=0;
        for (i=0; i<nsfb; i++)
        {
            if (!pred_sfb_flag[i]) {
                for (j=cb_long; j<cb_long+isfb_width[i]; j++)
                    sb_samples_pred[j]=0.0;
            }
            cb_long+=isfb_width[i];
        }

        /* Disable prediction for bands nsfb through SBMAX_L */
        for (i=j;i<BLOCK_LEN_LONG;i++) {
            sb_samples_pred[i]=0.0;
        }
        for (i=nsfb;i<SBMAX_L;i++) {
            pred_sfb_flag[i]=0;
        }

        /* Is global enable set, if not enabled predicted samples are zeroed */
        if(!pred_global_flag[0]) {
            for (j=0; j<BLOCK_LEN_LONG; j++)
                sb_samples_pred[j]=0.0;
        }
        for (j=0; j<BLOCK_LEN_LONG; j++)
            act_spec[j]-=sb_samples_pred[j];

    } else {

        /**************************************************/
        /* Determine whether to enable/disable prediction */
        /**************************************************/

        for (k=0; k<BLOCK_LEN_LONG; k++) {
            energy[k]=act_spec[k]*act_spec[k];
            snr_p[k]=(act_spec[k]-sb_samples_pred[k])*(act_spec[k]-sb_samples_pred[k]);
        }

        cb_long=0;
        for (i=0; i<nsfb; i++) {
            pred_sfb_flag[i]=1;
            temp1=0.0;
            temp2=0.0;
            for (j=cb_long; j<cb_long+isfb_width[i]; j++) {
                temp1+=energy[j];
                temp2+=snr_p[j];
            }
            if(temp2<1.e-20)
                temp2=1.e-20;
            if(temp1!=0.0)
                snr[i]=-10.*log10((double ) temp2/temp1);
            else
                snr[i]=0.0;

            if(snr[i]<=0.0) {
                pred_sfb_flag[i]=0;
                for (j=cb_long; j<cb_long+isfb_width[i]; j++)
                    sb_samples_pred[j]=0.0;
            }
            cb_long+=isfb_width[i];
        }

        /* Disable prediction for bands nsfb through SBMAX_L */
        for (i=j;i<BLOCK_LEN_LONG;i++) {
            sb_samples_pred[i]=0.0;
        }
        for (i=nsfb;i<SBMAX_L;i++) {
            pred_sfb_flag[i]=0;
        }

        num_bit=0.0;
        for (i=0; i<nsfb; i++)
            if(snr[i]>0.0)
                num_bit+=snr[i]/6.*isfb_width[i];

        /* Determine global enable, if not enabled predicted samples are zeroed */
        pred_global_flag[0]=1;
        if(num_bit<50) {
            pred_global_flag[0]=0; num_bit=0.0;
            for (j=0; j<BLOCK_LEN_LONG; j++)
                sb_samples_pred[j]=0.0;
        }
        for (j=0; j<BLOCK_LEN_LONG; j++)
            act_spec[j]-=sb_samples_pred[j];

    }

    /**********************************************************/
    /* If this is a left channel, determine pred resets.      */
    /* If this is a right channel, using pred reset data from */
    /* left channel.  Keep left and right resets in sync.     */
    /**********************************************************/
    if ((thisChannel->cpe)&&( !(thisChannel->ch_is_left))) {
        /*  if (!thisChannel->ch_is_left) {*/
        /**********************************************************/
        /* Using predictor reset data from the left channel.      */
        /**********************************************************/
        reset_count = &coderInfo[leftChanNum].bwpInfo.reset_count_mc;
        /* Reset the frame counter */
        for (i=0;i<BLOCK_LEN_LONG;i++) {
            thisLineNeedsResetting[i]=0;
        }
        reset_group = &(coderInfo[chanNum].reset_group_number);
        if (*reset_count % RESET_FRAME == 0)
        { /* Send a reset in this frame */
            *reset_group = *reset_count / 8;
            for (i = *reset_group - 1; i < BLOCK_LEN_LONG; i += 30)
            {
                thisLineNeedsResetting[i]=1;
            }
        }
        else
            *reset_group = -1;
    } else {
        /******************************************************************/
        /* Determine whether a prediction reset is required - if so, then */
        /* set reset flag for the appropriate group.                      */
        /******************************************************************/

        /* Increase counter on left channel, keep left and right resets in sync */
        (*reset_count)++;

        /* Reset the frame counter */
        for (i=0;i<BLOCK_LEN_LONG;i++) {
            thisLineNeedsResetting[i]=0;
        }
        if (*reset_count >= 31 * RESET_FRAME)
            *reset_count = RESET_FRAME;
        if (*reset_count % RESET_FRAME == 0)
        { /* Send a reset in this frame */
            *reset_group = *reset_count / 8;
            for (i = *reset_group - 1; i < BLOCK_LEN_LONG; i += 30)
            {
                thisLineNeedsResetting[i]=1;
            }
        }
        else
            *reset_group = -1;
    }


    /* Ensure that prediction data is sent when there is a prediction
    * reset.
    */
    if (*reset_group != -1 && pred_global_flag[0] == 0)
    {
        pred_global_flag[0] = 1;
        for (i = 0; i < nsfb; i++)
            pred_sfb_flag[i] = 0;
    }
}


void CopyPredInfo(CoderInfo *right, CoderInfo *left)
{
    int band;

    right->pred_global_flag = left->pred_global_flag;
    right->reset_group_number = left->reset_group_number;

    for (band = 0; band<MAX_SCFAC_BANDS; band++) {
        right->pred_sfb_flag[band] = left->pred_sfb_flag[band];
    }
}





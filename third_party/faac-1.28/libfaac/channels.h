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
 * $Id: channels.h,v 1.7 2003/06/26 19:19:41 knik Exp $
 */

#ifndef CHANNEL_H
#define CHANNEL_H

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#include "coder.h"

typedef struct {
    int is_present;
    int ms_used[MAX_SCFAC_BANDS];
} MSInfo;

typedef struct {
    int tag;
    int present;
    int ch_is_left;
    int paired_ch;
    int common_window;
    int cpe;
    int sce;
    int lfe;
    MSInfo msInfo;
} ChannelInfo;

void GetChannelInfo(ChannelInfo *channelInfo, int numChannels, int useLfe);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* CHANNEL_H */

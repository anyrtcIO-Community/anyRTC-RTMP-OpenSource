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
 * $Id: midside.h,v 1.1 2003/06/26 19:40:23 knik Exp $
 */

#ifndef _MIDSIDE_H
#define _MIDSIDE_H

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#include "coder.h"


void MSEncode(CoderInfo *coderInfo, ChannelInfo *channelInfo, double *spectrum[MAX_CHANNELS],
              unsigned int numberOfChannels, unsigned int msenable);
void MSReconstruct(CoderInfo *coderInfo, ChannelInfo *channelInfo, int numberOfChannels);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* _MIDSIDE_H */

/*
*  Copyright (c) 2021 The AnyRTC project authors. All Rights Reserved.
*
*  Please visit https://www.anyrtc.io for detail.
*
* The GNU General Public License is a free, copyleft license for
* software and other kinds of works.
*
* The licenses for most software and other practical works are designed
* to take away your freedom to share and change the works.  By contrast,
* the GNU General Public License is intended to guarantee your freedom to
* share and change all versions of a program--to make sure it remains free
* software for all its users.  We, the Free Software Foundation, use the
* GNU General Public License for most of our software; it applies also to
* any other work released this way by its authors.  You can apply it to
* your programs, too.
* See the GNU LICENSE file for more info.
*/
#ifndef __PLATFORM_IMPL_H__
#define __PLATFORM_IMPL_H__
#include "media/base/adapted_video_track_source.h"
#include "ArLiveDef.hpp"

rtc::scoped_refptr<webrtc::VideoTrackSourceInterface> createPlatformVideoSouce();


void* createPlatformVideoCapture(const rtc::scoped_refptr<webrtc::VideoTrackSourceInterface>vidSource, size_t width, size_t height, size_t fps, size_t capture_device_index);
bool startPlatformVideoCapture(void*ptrCap);
bool stopPlatformVideoCapture(void*ptrCap);
void destoryPlatformVideoCapture(void*ptrCap);
#if TARGET_PLATFORM_PHONE
void platformVideoCaptureSetBeautyEffect(void*ptrCap, bool enable);
void switchPlatformVideoCapture(void*ptrCap,bool isFront);
#endif

#endif

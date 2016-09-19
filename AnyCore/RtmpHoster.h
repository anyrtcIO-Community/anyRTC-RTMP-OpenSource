/*
*  Copyright (c) 2016 The AnyRTC project authors. All Rights Reserved.
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

#ifndef __RTMP_HOSTER_H__
#define __RTMP_HOSTER_H__
#include <string>
#include "LIV_Export.h"
#include "RTMPCommon.h"

class RTMPHosterEvent
{
public:
	RTMPHosterEvent(void) {};
	virtual ~RTMPHosterEvent(void) {};

public:
	virtual void OnRtmpStreamOK() = 0;
	virtual void OnRtmpStreamReconnecting(int times) = 0;
	virtual void OnRtmpStreamStatus(int delayMs, int netBand) = 0;
	virtual void OnRtmpStreamFailed(int code) = 0;
	virtual void OnRtmpStreamClosed() = 0;
};

class LIV_API RTMPHoster
{
public:
	static RTMPHoster* Create(RTMPHosterEvent&callback);
	static void Destory(RTMPHoster*hoster);

	//* Common function
	virtual void SetAudioEnable(bool enabled) = 0;
	virtual void SetVideoEnable(bool enabled) = 0;
	virtual void SetVideoRender(void* render) = 0;
	virtual void SetVideoCapturer(void* handle) = 0;
	virtual void SetVideoMode(RTMPVideoMode videoMode) = 0;

	//* Rtmp function for push rtmp stream 
	virtual void StartRtmpStream(const char*url) = 0;
	virtual void StopRtmpStream() = 0;

protected:
	virtual void* GotSelfPtr() = 0;

protected:
	RTMPHoster() {};
	virtual ~RTMPHoster() {};
};

#endif	// __RTMP_HOSTER_H__
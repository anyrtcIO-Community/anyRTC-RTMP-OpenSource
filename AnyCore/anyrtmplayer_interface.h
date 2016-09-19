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
#ifndef __ANY_RTMP_PLAYER_INTERFACE_H__
#define __ANY_RTMP_PLAYER_INTERFACE_H__
#include "LIV_Export.h"
#include <stdint.h>

class AnyRtmplayerEvent;
class LIV_API AnyRtmplayer
{
public:
	virtual ~AnyRtmplayer(void){};
	static AnyRtmplayer* Create(AnyRtmplayerEvent&callback);

	virtual void StartPlay(const char* url) = 0;
	virtual void SetVideoRender(void* handle) = 0;
	virtual void StopPlay() = 0;

protected:
	AnyRtmplayer(AnyRtmplayerEvent&callback) :callback_(callback){};

protected:
	AnyRtmplayerEvent& callback_;
};

class AnyRtmplayerEvent
{
public:
	AnyRtmplayerEvent(void){};
	virtual ~AnyRtmplayerEvent(void){};

	virtual void OnRtmplayerOK() = 0;
	virtual void OnRtmplayerStatus(int cacheTime, int curBitrate) = 0;
	virtual void OnRtmplayerCache(int time) = 0;
	virtual void OnRtmplayerClose(int errcode) = 0;
};

#endif	// __ANY_RTMP_PLAYER_INTERFACE_H__
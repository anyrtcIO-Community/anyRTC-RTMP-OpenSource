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
#ifndef __ARP_PUSHER_H__
#define __ARP_PUSHER_H__
#include "ArLivePusherObserver.hpp"

class ARPusher
{
public:
	ARPusher(): callback_(NULL) {};
	virtual ~ARPusher(void) {};

	virtual void setObserver(AR::ArLivePusherObserver* observer) {
		callback_ = observer;
	};
	virtual int startTask(const char* strUrl) = 0;
	virtual int stopTask() = 0;

	virtual int runOnce() = 0;
	virtual int setAudioEnable(bool bAudioEnable) = 0;
	virtual int setVideoEnable(bool bVideoEnable) = 0;
	/*
	 */
	virtual int setRepeat(bool bEnable) = 0;
	virtual int setRetryCountDelay(int nCount, int nDelay) = 0;
	virtual int setAudioData(const char* pData, int nLen, uint32_t ts) { return 0; };
	virtual int setVideoData(const char* pData, int nLen, bool bKeyFrame, uint32_t ts) { return 0; };
	virtual int setSeiData(const char* pData, int nLen, uint32_t ts) { return 0; };

	virtual void SetRtcFactory(void* ptr) {};
	virtual void setRtcVideoSource(void* ptr) {};

protected:
	AR::ArLivePusherObserver* callback_;
};

V2_API ARPusher* V2_CALL createARPusher();


#endif	// __ARP_PUSHER_H__

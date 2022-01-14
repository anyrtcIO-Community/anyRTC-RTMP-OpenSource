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
#ifndef __RTC_TICK_H__
#define __RTC_TICK_H__
#include <map>
#include "rtc_base/deprecated/recursive_critical_section.h"

class RtcTick
{
public:
	RtcTick(void) : unAttach(false) {};
	virtual ~RtcTick(void) {};

	virtual void OnTick() = 0;
	virtual void OnTickUnAttach() = 0;

	bool unAttach;
};

class MThreadTick
{
public:
	virtual ~MThreadTick();

	void RegisteRtcTick(void* ptr, RtcTick* rtcTick);
	void UnRegisteRtcTick(void* ptr);
	void UnAttachRtcTick(void* ptr);


	void DoProcess();

protected:
	MThreadTick();

private:
	typedef std::map<void*, RtcTick*> MapRtcTick;
	rtc::RecursiveCriticalSection cs_rtc_tick_;
	MapRtcTick		map_rtc_tick_;
};

#endif
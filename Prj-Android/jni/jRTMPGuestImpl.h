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
#ifndef __J_RTMP_GUEST_IMPL_H__
#define __J_RTMP_GUEST_IMPL_H__
#include <jni.h>
#include "RTMPGuester.h"

class JRTMPGuestImpl : public RTMPGuesterEvent
{
public:
	JRTMPGuestImpl(jobject javaObj);
	virtual ~JRTMPGuestImpl(void);

	void Close();
	RTMPGuester* Guest() {return m_pGuest;};

public:
	//* For RTMPGuestEvent
	virtual void OnRtmplayerOK();
	virtual void OnRtmplayerStatus(int cacheTime, int curBitrate);
	virtual void OnRtmplayerCache(int time);
	virtual void OnRtmplayerClosed(int errcode);

private:
	jobject			m_jJavaObj;
	jclass			m_jClass;

	RTMPGuester	*m_pGuest;
};

#endif	// __J_RTMP_GUEST_IMPL_H__
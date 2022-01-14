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
#ifndef __MGR_RENDER_H__
#define __MGR_RENDER_H__
#include <map>
#include "rtc_base/deprecated/recursive_critical_section.h"
#include "video_renderer.h"

class MgrRender
{
public:
	MgrRender(void);
	virtual ~MgrRender(void);

	void SetRender(const char*pId, webrtc::VideoRenderer*render);

	void SetRotation(const char* pId, int nRotation);

	void SetFillMode(const char* pId, int nMode);

	void DoRenderFrame(const char*pId, const webrtc::VideoFrame& frame);

private:
	typedef std::map<std::string, webrtc::VideoRenderer*>MapRender;
	rtc::RecursiveCriticalSection cs_renders_;
	MapRender	map_renders_;

};

#endif // __MGR_RENDER_H__
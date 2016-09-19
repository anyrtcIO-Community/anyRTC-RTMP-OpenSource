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
#ifndef __VIDEO_FILTER_H__
#define __VIDEO_FILTER_H__
#include "webrtc/video_frame.h"
#include "webrtc/api/mediastreaminterface.h"
#include "webrtc/media/base/videoframe.h"
#include "webrtc/media/base/videobroadcaster.h"
#include "webrtc/media/engine/webrtcvideoframe.h"

class VideoFilter : public rtc::VideoSinkInterface<cricket::VideoFrame>
{
public:
	VideoFilter();
	virtual ~VideoFilter();

	rtc::VideoBroadcaster& VBroadcaster(){ return broadcaster_; };

	//* For VideoSinkInterface
	virtual void OnFrame(const cricket::VideoFrame& frame);

private:
	int		v_width_;
	int		v_height_;

	rtc::VideoBroadcaster		broadcaster_;
	cricket::WebRtcVideoFrame	video_frame_;
};

#endif	// __VIDEO_FILTER_H__
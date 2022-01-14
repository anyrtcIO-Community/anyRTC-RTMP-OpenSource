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
#ifndef __WIN_VIDEO_TRACK_SOURCE_H__
#define __WIN_VIDEO_TRACK_SOURCE_H__
#include "media/base/adapted_video_track_source.h"
#include "rtc_base/thread.h"

class WinVideoTrackSource : public rtc::AdaptedVideoTrackSource
{
public:
	WinVideoTrackSource(void);
	virtual ~WinVideoTrackSource(void);

	void FrameCaptured(const webrtc::VideoFrame& frame);

	SourceState state() const override {
		return SourceState::kLive;
	}

	bool remote() const override { return false; }

	bool is_screencast() const override {
		return false;
	}

	absl::optional<bool> needs_denoising() const override {
		return false;
	}

private:
	// Encoded sinks not implemented for JavaVideoTrackSourceImpl.
	bool SupportsEncodedOutput() const override { return false; }
	void GenerateKeyFrame() override {}
	void AddEncodedSink(
		rtc::VideoSinkInterface<webrtc::RecordableEncodedFrame>* sink) override {}
	void RemoveEncodedSink(
		rtc::VideoSinkInterface<webrtc::RecordableEncodedFrame>* sink) override {}

private:
};

#endif	// __WIN_VIDEO_TRACK_SOURCE_H__


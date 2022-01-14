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
#ifndef __AUD_DEVICE_EVENT_H__
#define __AUD_DEVICE_EVENT_H__
#include <stdint.h>
#include <stdio.h>

class AudDevCaptureEvent
{
public:
	AudDevCaptureEvent(void) {};
	virtual ~AudDevCaptureEvent(void) {};

	virtual void RecordedDataIsAvailable(const void* audioSamples, const size_t nSamples,
		const size_t nBytesPerSample, const size_t nChannels, const uint32_t samplesPerSec, const uint32_t totalDelayMS) {};
};

class AudDevSpeakerEvent
{
public:
	AudDevSpeakerEvent(void) {}
	virtual ~AudDevSpeakerEvent(void) {};


	virtual int MixAudioData(bool mix, void* audioSamples, uint32_t samplesPerSec, int nChannels) { return 0; };
};

#endif	// __AUD_DEVICE_EVENT_H__

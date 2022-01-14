#ifndef __VID_DEVICE_EVENT_H__
#define __VID_DEVICE_EVENT_H__
#include <stdint.h>

class VidDevCaptureEvent
{
public:
	VidDevCaptureEvent(void) {};
	virtual ~VidDevCaptureEvent(void) {};

	virtual void VideoFrameIsAvailable(int fmt, int ww, int hh, uint8_t** pData, int* linesize) {};
};

#endif	// __VID_DEVICE_EVENT_H__
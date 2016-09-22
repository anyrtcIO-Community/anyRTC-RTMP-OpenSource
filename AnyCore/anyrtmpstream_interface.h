#ifndef __ANY_RTMP_STREAM_INTERFACE_H__
#define __ANY_RTMP_STREAM_INTERFACE_H__
#include "LIV_Export.h"
#include <stdint.h>
#include <string>

class AnyRtmpstreamerEvent;
class LIV_API AnyRtmpstreamer
{
public:
	virtual ~AnyRtmpstreamer(void){};
	static AnyRtmpstreamer* Create(AnyRtmpstreamerEvent&callback);

	virtual void SetAudioEnable(bool enabled) = 0;
	virtual void SetVideoEnable(bool enabled) = 0;
    virtual void SetAutoAdjustBit(bool enabled) = 0;
	virtual void SetVideoParameter(int w, int h, int bitrate) = 0;
	virtual void SetBitrate(int bitrate) = 0;

	virtual void StartStream(const std::string&url) = 0;
	virtual void StopStream() = 0;

protected:
	AnyRtmpstreamer(void){};
};

class AnyRtmpstreamerEvent
{
public:
	AnyRtmpstreamerEvent(void){};
	virtual ~AnyRtmpstreamerEvent(void){};

public:
	virtual bool ExternalVideoEncoder() = 0;
	virtual void OnStreamOk() = 0;
	virtual void OnStreamReconnecting(int times) = 0;
	virtual void OnStreamFailed(int code) = 0;
	virtual void OnStreamClosed() = 0;
	virtual void OnStreamStatus(int delayMs, int netBand) = 0;
};

#endif	// __ANY_RTMP_STREAM_INTERFACE_H__
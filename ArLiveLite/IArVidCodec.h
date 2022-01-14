#ifndef __I_AR_VID_CODEC_H__
#define __I_AR_VID_CODEC_H__

class RtcVidEncoderEvent
{
public:
	RtcVidEncoderEvent(void) {};
	virtual ~RtcVidEncoderEvent(void) {};

	virtual void OnVideoEncoderData(void* pEncoder, int nCodecId, bool isMain, const char*pData, int nLen, bool bKeyFrame, int imageWidth, int imageHeight) {};
	virtual void OnVideoEncoderPreEnc(const char*yData, const char*uData, const char*vData, int strideY, int strideU, int strideV, int w, int h, int rotate) {};
};

class IArVidEncoder
{
protected:
	IArVidEncoder(void) {};
public:
	virtual ~IArVidEncoder(void) {};

	virtual bool Init(int w, int h, int fps, int bitrate) = 0;
	virtual void ResetVidRates(int nBitrate, int nFps) = 0;
	virtual void RequestKeyFrame() = 0;
	virtual void SetVideoData(const char*y, const char*u, const char*v, int strideY, int stridU, int strideV, int w, int h) = 0;
	virtual void DeInit() = 0;
};

IArVidEncoder* createArVidEncoder(RtcVidEncoderEvent&callback);



#endif	// __I_AR_VID_CODEC_H__

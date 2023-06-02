#ifndef __AR_LIVE2_PUSHER_H__
#define __AR_LIVE2_PUSHER_H__
#include "ArLive2Engine.h"
#include "IArLivePusher.hpp"
#include "ARPusher.h"
#include "SeiMsg.h"
#include "codec/AvCodec.h"
#include "api/media_stream_interface.h"

class ArLive2Pusher : public AR::IArLivePusher, public RtcTick, public AudDevCaptureEvent, public rtc::VideoSinkInterface<webrtc::VideoFrame>, public AR::ArLivePusherObserver, public webrtc::AVCodecCallback
{
public:
	ArLive2Pusher(ArLive2Engine*pEngine, const std::string& strPushId);
	virtual ~ArLive2Pusher(void);

	void setExVideoEncoderFactory(webrtc::VideoEncoderFactory *video_encoder_factory);
	const std::string& PushId() { return str_local_push_id_; };

	//* For IArLivePushKit
	virtual void setObserver(ArLivePusherObserver* observer);
	virtual void setLiveOem(anyrtc::ArLiveOem oem);
    virtual int32_t setRenderView(void* view);
    virtual int32_t setRenderMirror(AR::ArLiveMirrorType mirrorType);
    virtual int32_t setEncoderMirror(bool mirror);
    virtual int32_t setRenderRotation(AR::ArLiveRotation rotation);
#if TARGET_PLATFORM_PHONE
    virtual int32_t startCamera(bool frontCamera);
#ifdef __APPLE__
    virtual int32_t setBeautyEffect(bool enable);
#endif
#ifdef __ANDROID__
	virtual int startScreenCapture();
	virtual int stopScreenCapture();
#endif
#elif TARGET_PLATFORM_DESKTOP
    virtual int32_t startCamera(const char* cameraId);
#endif
    virtual int32_t stopCamera();
    virtual int32_t startMicrophone();
    virtual int32_t stopMicrophone();
    virtual int32_t startVirtualCamera(AR::ArLiveImage* image);
    virtual int32_t stopVirtualCamera();

    virtual int32_t pauseAudio();
    virtual int32_t resumeAudio();
    virtual int32_t pauseVideo();
    virtual int32_t resumeVideo();
    virtual int32_t startPush(const char* url);
    virtual int32_t stopPush();
    virtual int32_t isPushing();
    virtual int32_t setAudioQuality(AR::ArLiveAudioQuality quality);
    virtual int32_t setVideoQuality(const AR::ArLiveVideoEncoderParam& param);
	int32_t setVideoQuality_I(const AR::ArLiveVideoEncoderParam* param);
    virtual AR::ArAudioEffectManager* getAudioEffectManager();
    virtual AR::ArDeviceManager* getDeviceManager();
    virtual int32_t snapshot();
    virtual int32_t setWatermark(const char* watermarkPath, float x, float y, float scale);
    virtual int32_t enableVolumeEvaluation(int32_t intervalMs);
    virtual int32_t enableCustomVideoCapture(bool enable);
    virtual int32_t sendCustomVideoFrame(AR::ArLiveVideoFrame* videoFrame);
#ifdef _WIN32
    virtual int32_t enableCustomRendering(bool enable, ArLivePixelFormat pixelFormat, ArLiveBufferType bufferType);
#endif
    virtual int enableCustomAudioCapture(bool enable);
    virtual int sendCustomAudioFrame(AR::ArLiveAudioFrame* audioFrame);
    virtual int32_t sendSeiMessage(int payloadType, const uint8_t* data, uint32_t dataSize);

#if TARGET_PLATFORM_DESKTOP
    virtual int32_t startSystemAudioLoopback(const char* path = nullptr);
    virtual int32_t stopSystemAudioLoopback();
    virtual int32_t enableCustomVideoProcess(bool enable, ArLivePixelFormat pixelFormat, ArLiveBufferType bufferType);
#ifdef _WIN32
    virtual void startScreenCapture();
#endif
#ifdef _WIN32
    virtual void stopScreenCapture();
#endif
#ifdef _WIN32
    virtual IArLiveScreenCaptureSourceList* getScreenCaptureSources(const SIZE& thumbSize, const SIZE& iconSize);
#endif
#ifdef _WIN32
    virtual void setScreenCaptureSource(const ArLiveScreenCaptureSourceInfo& source, const RECT& captureRect, const ArLiveScreenCaptureProperty& property);
#endif
#endif
	virtual void showDebugView(bool isShow);

	//* For RtcTick
	virtual void OnTick();
	virtual void OnTickUnAttach();

	//* For ArLivePushSinkInterface
	void initAudioWithParameters(int nType, int sampleRate, int numChannels, int audBitrate);
	void deinitAudio();
	void initVideoWithParameters(int nType, int videoWidth, int videoHeight, int videoFps, int videoBitrate);
	void deinitVideo();

	//* For AudDevCaptureEvent
	virtual void RecordedDataIsAvailable(const void* audioSamples, const size_t nSamples,
		const size_t nBytesPerSample, const size_t nChannels, const uint32_t samplesPerSec, const uint32_t totalDelayMS);

	// rtc::VideoSinkInterface<webrtc::VideoFrame>
	void OnFrame(const webrtc::VideoFrame& frame) override;

	//* For AR::ArLivePusherObserver
	virtual void onError(int32_t code, const char* msg, void* extraInfo);
	virtual void onWarning(int32_t code, const char* msg, void* extraInfo);
	virtual void onPushStatusUpdate(AR::ArLivePushStatus state, const char* msg, void* extraInfo);
	virtual void onStatisticsUpdate(AR::ArLivePusherStatistics statistics);

	//* For AVCodecCallback
	virtual void OnEncodeDataCallback(bool audio, bool bKeyFrame, const uint8_t *pData, uint32_t length, uint32_t ts);

private:
	ArLive2Engine*					ar_engine_;
	rtc::Thread*					main_thread_;
	std::string						str_local_push_id_;

	AR::ArLivePusherObserver*		event_live_push_;

private:
	
	// Video devices
	rtc::scoped_refptr<webrtc::VideoTrackSourceInterface> video_source_;
	struct PtVideoCap
	{
		PtVideoCap(const rtc::scoped_refptr<webrtc::VideoTrackSourceInterface>vidSource, size_t width, size_t height, size_t fps, size_t capture_device_index) : ptrCap(NULL), bStarted(false) {
			ptrCap = createPlatformVideoCapture(vidSource, width, height, fps, capture_device_index);
		}
		virtual ~PtVideoCap(void) {
			destoryPlatformVideoCapture(ptrCap);
			ptrCap = NULL;
		}
		void* ptrCap;
		bool bStarted;
#if (defined(WEBRTC_ANDROID))
		bool bIsScreen;
#endif
	};

	AR::VideoDimensions			video_dimensions_;
	std::unique_ptr<PtVideoCap> platform_video_cap_;

private:
	bool						b_live_pushed_;
	bool						b_audio_paused_;
	bool						b_video_paused_;
	bool						b_auto_republish_;
	bool						b_micro_phpone_started_;
	bool						b_client_connected_;
	int							n_volume_captured_;
	int64_t						n_last_keyframe_time_;
	std::string					str_rtmp_url_;
	ARPusher*					ar_pusher_;

private:
	bool						b_audio_captured_;
	bool						b_video_captured_;

private:
    AR::ArLiveAudioQuality			audio_quality_;
    AR::ArLiveVideoEncoderParam		video_quality_;
	bool						b_snap_shot_;
	bool						b_enable_custom_video_capture_;
	bool						b_enable_custom_audio_capture_;
	int							n_volume_evaluation_interval_;
	int64_t						n_volume_evaluation_next_ms_;

	int64_t						n_stat_timer_ms_;
	AR::ArLivePusherStatistics		pusher_statistics_;
private:
	webrtc::Mutex cs_aac_encoder_;
	webrtc::A_AACEncoder*		aac_encoder_;
	webrtc::Mutex cs_h264_encoder_;
	webrtc::V_H264Encoder*		h264_encoder_;
	webrtc::VideoEncoderFactory*exVideo_encoder_factory;

private:
	typedef std::list<SeiMsg*>ListSeiMsg;
	ListSeiMsg	lst_sei_msg_;

private:
	int64_t		n_next_virtual_camera_time_;
	rtc::scoped_refptr<webrtc::I420Buffer> video_virtual_camera_;
};


#endif	// __AR_LIVE2_PUSHER_H__

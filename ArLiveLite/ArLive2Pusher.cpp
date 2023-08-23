#include "ArLive2Pusher.h"
#include "rtc_base/bind.h"
#include "rtc_base/time_utils.h"
#include "libyuv/include/libyuv.h"

using namespace anyrtc;
extern int16_t WebRtcSpl_MaxAbsValueW16_I(const int16_t* vector, size_t length);

ArLive2Pusher::ArLive2Pusher(ArLive2Engine*pEngine, const std::string& strPushId)
	: ar_engine_(pEngine)
	, main_thread_(NULL)
	, event_live_push_(NULL)
	, b_live_pushed_(false)
	, b_audio_paused_(false)
	, b_video_paused_(false)
	, b_auto_republish_(true)
	, b_micro_phpone_started_(false)
	, b_client_connected_(false)
	, n_volume_captured_(0)
	, n_last_keyframe_time_(0)
	, b_audio_captured_(false)
	, b_video_captured_(false)
	, audio_quality_(ArLiveAudioQualityDefault)
	, video_quality_(ArLiveVideoResolution640x360)
	, b_snap_shot_(false)
	, b_enable_custom_video_capture_(false)
	, b_enable_custom_audio_capture_(false)
	, n_volume_evaluation_next_ms_(0)
	, n_stat_timer_ms_(0)
	, n_next_virtual_camera_time_(NULL)
	, ar_pusher_(NULL)
	, aac_encoder_(NULL)
	, h264_encoder_(NULL)
{
	main_thread_ = pEngine;

	str_local_push_id_ = strPushId;

	video_source_ = createPlatformVideoSouce();

	if (video_source_ != NULL) {
		rtc::VideoSinkWants wants;
		video_source_->AddOrUpdateSink(this, wants);
	}
}
ArLive2Pusher::~ArLive2Pusher(void)
{
	if (video_source_ != NULL) {
		video_source_->RemoveSink(this);
	}
	video_source_ = nullptr;
}

void ArLive2Pusher::setExVideoEncoderFactory(webrtc::VideoEncoderFactory *video_encoder_factory) {
	exVideo_encoder_factory = video_encoder_factory;
}
//* For IArLive2Pusher
void ArLive2Pusher::setObserver(ArLivePusherObserver* observer)
{
	event_live_push_ = observer;
}
void ArLive2Pusher::setLiveOem(ArLiveOem oem)
{

}
int32_t ArLive2Pusher::setRenderView(void* view)
{
	VideoCanvas vidCanvas;
	vidCanvas.view = view;
	vidCanvas.uid = str_local_push_id_.c_str();
	ar_engine_->setVideoRenderView(str_local_push_id_.c_str(), vidCanvas);
	return ArLIVE_OK;
}
int32_t ArLive2Pusher::setRenderMirror(ArLiveMirrorType mirrorType)
{
	ar_engine_->setVideoRenderMirror(str_local_push_id_.c_str(), mirrorType);
	return ArLIVE_OK;
}
int32_t ArLive2Pusher::setEncoderMirror(bool mirror) 
{
	webrtc::MutexLock l(&cs_h264_encoder_);
	if (h264_encoder_ != NULL) {
		h264_encoder_->SetMirror(mirror);
	}
	return ArLIVE_OK;
}
int32_t ArLive2Pusher::setRenderRotation(ArLiveRotation rotation)
{
	ar_engine_->setVideoRenderRotation(str_local_push_id_.c_str(), rotation);
	return ArLIVE_OK;
}
#if TARGET_PLATFORM_PHONE
int32_t ArLive2Pusher::startCamera(bool frontCamera)
{

	if (platform_video_cap_ == NULL) {
#if (defined(WEBRTC_ANDROID))
		platform_video_cap_.reset(new PtVideoCap(video_source_, video_dimensions_.width, video_dimensions_.height, video_dimensions_.fps, frontCamera? 0 : 1));
		platform_video_cap_->bIsScreen = false;
#else
        platform_video_cap_.reset(new PtVideoCap(video_source_, video_dimensions_.width, video_dimensions_.height, video_dimensions_.fps, frontCamera ? 2 : 1));
#endif
    } else {
#if (defined(WEBRTC_IOS))
        switchPlatformVideoCapture(platform_video_cap_->ptrCap, NULL);
#endif
    }
    
	if (!platform_video_cap_->bStarted) {
		platform_video_cap_->bStarted = startPlatformVideoCapture(platform_video_cap_->ptrCap);
	}
	return ArLIVE_OK;
}
#ifdef __APPLE__
int32_t ArLive2Pusher::setBeautyEffect(bool enable) {
    if (platform_video_cap_ != NULL) {
        platformVideoCaptureSetBeautyEffect(platform_video_cap_->ptrCap, enable);
    }
    return ArLIVE_OK;
}
#endif

#ifdef __ANDROID__
int ArLive2Pusher::startScreenCapture()
{
	if (platform_video_cap_ == NULL) {
		platform_video_cap_.reset(new PtVideoCap(video_source_, video_dimensions_.width, video_dimensions_.height, video_dimensions_.fps, 8));
		platform_video_cap_->bIsScreen = true;
	}

	if (!platform_video_cap_->bStarted) {
		platform_video_cap_->bStarted = startPlatformVideoCapture(platform_video_cap_->ptrCap);
	}

	return 0;
}

int ArLive2Pusher::stopScreenCapture()
{
	if (platform_video_cap_ != NULL) {
		if (platform_video_cap_->bStarted) {
			platform_video_cap_->bStarted = false;
			stopPlatformVideoCapture(platform_video_cap_->ptrCap);
		}
	}
	platform_video_cap_ = nullptr;

	return 0;
}
#endif

#elif TARGET_PLATFORM_DESKTOP
int32_t ArLive2Pusher::startCamera(const char* cameraId)
{
	if (!main_thread_->IsCurrent()) {
		return main_thread_->Invoke<int>(RTC_FROM_HERE, rtc::Bind(&ArLive2Pusher::startCamera, this, cameraId));
	}
	if (b_enable_custom_video_capture_) {
		return ArLIVE_ERROR_REFUSED;
	}
	int nDevIdx = 0;
	if (cameraId != NULL && strlen(cameraId) > 0) {
		return ArLIVE_ERROR_INVALID_PARAMETER;
	}
	if (platform_video_cap_ == NULL) {
		platform_video_cap_.reset(new PtVideoCap(video_source_, video_dimensions_.width, video_dimensions_.height, video_dimensions_.fps, nDevIdx));
	}

	if (!platform_video_cap_->bStarted) {
		platform_video_cap_->bStarted = startPlatformVideoCapture(platform_video_cap_->ptrCap);
		if (!platform_video_cap_->bStarted) {
			return ArLIVE_WARNING_CAMERA_START_FAILED;
		}
	}

	if (!platform_video_cap_->bStarted) {
		return ArLIVE_ERROR_FAILED;
	}

	return ArLIVE_OK;
}
#endif
int32_t ArLive2Pusher::stopCamera()
{
	if (!main_thread_->IsCurrent()) {
		return main_thread_->Invoke<int>(RTC_FROM_HERE, rtc::Bind(&ArLive2Pusher::stopCamera, this));
	}
	if (platform_video_cap_ != NULL) {
		if (platform_video_cap_->bStarted) {
			platform_video_cap_->bStarted = false;
			stopPlatformVideoCapture(platform_video_cap_->ptrCap);
		}
	}
	platform_video_cap_ = nullptr;
	return ArLIVE_OK;
}
int32_t ArLive2Pusher::startMicrophone()
{
	if (!main_thread_->IsCurrent()) {
		return main_thread_->Invoke<int>(RTC_FROM_HERE, rtc::Bind(&ArLive2Pusher::startMicrophone, this));
	}
	if (b_enable_custom_audio_capture_) {
		return ArLIVE_ERROR_REFUSED;
	}
	if (!b_micro_phpone_started_) {
		b_micro_phpone_started_ = true;
		if (b_live_pushed_) {
			ar_engine_->AttachAudCapture(this);
		}
	}
	return ArLIVE_OK;
}
int32_t ArLive2Pusher::stopMicrophone()
{
	if (!main_thread_->IsCurrent()) {
		return main_thread_->Invoke<int>(RTC_FROM_HERE, rtc::Bind(&ArLive2Pusher::stopMicrophone, this));
	}

	if (b_micro_phpone_started_) {
		b_micro_phpone_started_ = false;
		if (b_live_pushed_) {
			ar_engine_->DetachAudCapture(this);
		}
	}
	return ArLIVE_OK;
}

int32_t ArLive2Pusher::startVirtualCamera(ArLiveImage* image)
{
	if (!main_thread_->IsCurrent()) {
		return main_thread_->Invoke<int>(RTC_FROM_HERE, rtc::Bind(&ArLive2Pusher::startVirtualCamera, this, image));
	}
	if (image->imageType == ArLiveImageTypeBGRA32 || image->imageType == ArLiveImageTypeRGBA32) {
		video_virtual_camera_ = webrtc::I420Buffer::Create(
			image->imageWidth, image->imageHeight, image->imageWidth, image->imageWidth/2, image->imageWidth/2);

		int ww = image->imageWidth;
		int hh = image->imageHeight;

		if (image->imageType == ArLiveImageTypeBGRA32) {
			libyuv::ARGBToI420((uint8_t*)(image->imageSrc), image->imageWidth * 4,
				(uint8_t*)video_virtual_camera_->DataY(), video_virtual_camera_->StrideY(), (uint8_t*)video_virtual_camera_->DataU(), video_virtual_camera_->StrideU(),
				(uint8_t*)video_virtual_camera_->DataV(), video_virtual_camera_->StrideV(), video_virtual_camera_->width(), video_virtual_camera_->height());
		}
		else if(image->imageType == ArLiveImageTypeRGBA32) {
			libyuv::ABGRToI420((uint8_t*)(image->imageSrc), image->imageWidth * 4,
				(uint8_t*)video_virtual_camera_->DataY(), video_virtual_camera_->StrideY(), (uint8_t*)video_virtual_camera_->DataU(), video_virtual_camera_->StrideU(),
				(uint8_t*)video_virtual_camera_->DataV(), video_virtual_camera_->StrideV(), video_virtual_camera_->width(), video_virtual_camera_->height());
		}
		n_next_virtual_camera_time_ = rtc::TimeUTCMillis();
		if (platform_video_cap_ != NULL) {
			stopPlatformVideoCapture(platform_video_cap_->ptrCap);
		}
	}

	return ArLIVE_OK;
}

int32_t ArLive2Pusher::stopVirtualCamera()
{
	if (!main_thread_->IsCurrent()) {
		return main_thread_->Invoke<int>(RTC_FROM_HERE, rtc::Bind(&ArLive2Pusher::stopVirtualCamera, this));
	}

	n_next_virtual_camera_time_ = 0;
	video_virtual_camera_ = NULL;
	if (platform_video_cap_ != NULL) {
		startPlatformVideoCapture(platform_video_cap_->ptrCap);
	}
	return ArLIVE_OK;
}

int32_t ArLive2Pusher::pauseAudio()
{
	if (!main_thread_->IsCurrent()) {
		return main_thread_->Invoke<int>(RTC_FROM_HERE, rtc::Bind(&ArLive2Pusher::pauseAudio, this));
	}
	b_audio_paused_ = true;
	if (ar_pusher_ != NULL) {
		ar_pusher_->setAudioEnable(!b_audio_paused_);
	}
	return ArLIVE_OK;
}
int32_t ArLive2Pusher::resumeAudio()
{
	if (!main_thread_->IsCurrent()) {
		return main_thread_->Invoke<int>(RTC_FROM_HERE, rtc::Bind(&ArLive2Pusher::resumeAudio, this));
	}
	b_audio_paused_ = false;
	if (ar_pusher_ != NULL) {
		ar_pusher_->setAudioEnable(!b_audio_paused_);
	}
	return ArLIVE_OK;
}
int32_t ArLive2Pusher::pauseVideo()
{
	if (!main_thread_->IsCurrent()) {
		return main_thread_->Invoke<int>(RTC_FROM_HERE, rtc::Bind(&ArLive2Pusher::pauseVideo, this));
	}
	b_video_paused_ = true;
	if (ar_pusher_ != NULL) {
		ar_pusher_->setVideoEnable(!b_video_paused_);
	}
	return ArLIVE_OK;
}
int32_t ArLive2Pusher::resumeVideo()
{
	if (!main_thread_->IsCurrent()) {
		return main_thread_->Invoke<int>(RTC_FROM_HERE, rtc::Bind(&ArLive2Pusher::resumeVideo, this));
	}
	b_video_paused_ = false;
	if (ar_pusher_ != NULL) {
		ar_pusher_->setVideoEnable(!b_video_paused_);
	}
	return ArLIVE_OK;
}
int ArLive2Pusher::startPush(const char* strPushUrl)
{
	if (!main_thread_->IsCurrent()) {
		return main_thread_->Invoke<int>(RTC_FROM_HERE, rtc::Bind(&ArLive2Pusher::startPush, this, strPushUrl));
	}
	if (!b_live_pushed_) {
		b_live_pushed_ = true;
		str_rtmp_url_ = strPushUrl;
		if (ar_pusher_ == NULL) {
			ar_pusher_ = createARPusher();
		}
		ar_pusher_->setObserver(this);
		ar_pusher_->setAudioEnable(!b_audio_paused_);
		ar_pusher_->setVideoEnable(!b_video_paused_);
		ar_pusher_->startTask(strPushUrl);

		{//* ��������Ƶ
			switch (audio_quality_) {
			case ArLiveAudioQualitySpeech: {
				initAudioWithParameters(0, 16000, 1, 16);
			}break;
			case ArLiveAudioQualityDefault: {
				initAudioWithParameters(0, 48000, 1, 50);
			}break;
			case ArLiveAudioQualityMusic: {
				initAudioWithParameters(0, 48000, 2, 128);
			}break;
			}

			int nWidth = video_dimensions_.width;
			int nHeight = video_dimensions_.height;
			int nFps = video_quality_.videoFps;
			int nBitrate = video_quality_.videoBitrate;
			if (video_quality_.videoResolutionMode == ArLiveVideoResolutionModePortrait) {
				int nn = nWidth;
				nWidth = nHeight;
				nHeight = nn;
			}
			initVideoWithParameters(0, nWidth, nHeight, nFps, nBitrate);
			{
				webrtc::MutexLock l(&cs_h264_encoder_);
				if (h264_encoder_ != NULL) {
					h264_encoder_->RequestKeyFrame();
				}
			}
		}

		if (b_micro_phpone_started_) {
			ar_engine_->AttachAudCapture(this);
		}
	
		n_stat_timer_ms_ = rtc::TimeUTCMillis();
		ar_engine_->RegisteRtcTick(this, this);
	}

	return ArLIVE_OK;
}
int ArLive2Pusher::stopPush()
{
	if (!main_thread_->IsCurrent()) {
		return main_thread_->Invoke<int>(RTC_FROM_HERE, rtc::Bind(&ArLive2Pusher::stopPush, this));
	}

	if (b_live_pushed_) {
		b_live_pushed_ = false;
		b_snap_shot_ = false;
		n_stat_timer_ms_ = 0;
		if (b_micro_phpone_started_) {
			ar_engine_->DetachAudCapture(this);
		}
		ar_engine_->UnRegisteRtcTick(this);
		ar_engine_->DetachAudCapture(this);

		deinitVideo();
		deinitAudio();

		if (ar_pusher_ != NULL) {
			ar_pusher_->stopTask();
			delete ar_pusher_;
			ar_pusher_ = NULL;
		}
	}
	return ArLIVE_OK;
}
int32_t ArLive2Pusher::isPushing()
{
	return b_live_pushed_;
}
int32_t ArLive2Pusher::setAudioQuality(ArLiveAudioQuality quality)
{
	if (!main_thread_->IsCurrent()) {
		return main_thread_->Invoke<int>(RTC_FROM_HERE, rtc::Bind(&ArLive2Pusher::setAudioQuality, this, quality));
	}

	if (b_live_pushed_) {
		return ArLIVE_ERROR_REFUSED;
	}

	audio_quality_ = quality;
	return ArLIVE_OK;
}
int32_t ArLive2Pusher::setVideoQuality(const ArLiveVideoEncoderParam& param)
{
	if (!main_thread_->IsCurrent()) {
		return main_thread_->Invoke<int>(RTC_FROM_HERE, rtc::Bind(&ArLive2Pusher::setVideoQuality_I, this, &param));
	}

	return setVideoQuality_I(&param);
}
int32_t ArLive2Pusher::setVideoQuality_I(const ArLiveVideoEncoderParam* param)
{
	if (b_live_pushed_) {
		return ArLIVE_ERROR_REFUSED;
	}

	video_quality_ = *param;
	int nWidth = 640;
	int nHeight = 480;
	switch (video_quality_.videoResolution) {
	case ArLiveVideoResolution160x160: {
		nWidth = 160;
		nHeight = 160;
	}break;
	case ArLiveVideoResolution270x270: {
		nWidth = 270;
		nHeight = 270;
	}break;
	case ArLiveVideoResolution480x480: {
		nWidth = 480;
		nHeight = 480;
	}break;
	case ArLiveVideoResolution320x240: {
		nWidth = 320;
		nHeight = 240;
	}break;
	case ArLiveVideoResolution480x360: {
		nWidth = 480;
		nHeight = 360;
	}break;
	case ArLiveVideoResolution640x480: {
		nWidth = 640;
		nHeight = 480;
	}break;
	case ArLiveVideoResolution320x180: {
		nWidth = 320;
		nHeight = 180;
	}break;
	case ArLiveVideoResolution480x270: {
		nWidth = 480;
		nHeight = 270;
	}break;
	case ArLiveVideoResolution640x360: {
		nWidth = 640;
		nHeight = 360;
	}break;
	case ArLiveVideoResolution960x540: {
		nWidth = 960;
		nHeight = 540;
	}break;
	case ArLiveVideoResolution1280x720: {
		nWidth = 1280;
		nHeight = 720;
	}break;
	case ArLiveVideoResolution1920x1080: {
		nWidth = 1920;
		nHeight = 1080;
	}break;
	}
	if (param->videoFps != 0) {
		video_quality_.videoFps = param->videoFps;
	}
	video_dimensions_.width = nWidth;
	video_dimensions_.height = nHeight;
	video_dimensions_.fps = video_quality_.videoFps;
	return ArLIVE_OK;
}
ArAudioEffectManager* ArLive2Pusher::getAudioEffectManager()
{//* Not support
	return NULL;
}
ArDeviceManager* ArLive2Pusher::getDeviceManager()
{//* Not support
	return NULL;
}
int32_t ArLive2Pusher::snapshot()
{
	if (!main_thread_->IsCurrent()) {
		return main_thread_->Invoke<int>(RTC_FROM_HERE, rtc::Bind(&ArLive2Pusher::snapshot, this));
	}
	if (!b_live_pushed_) {//�Ѿ�ֹͣ��������������ý�ͼ����
		return ArLIVE_ERROR_REFUSED;
	}
	b_snap_shot_ = true;
	return ArLIVE_OK;
}
int32_t ArLive2Pusher::setWatermark(const char* watermarkPath, float x, float y, float scale)
{//* Not support
	return ArLIVE_OK;
}
int32_t ArLive2Pusher::enableVolumeEvaluation(int32_t intervalMs)
{
	if (!main_thread_->IsCurrent()) {
		return main_thread_->Invoke<int>(RTC_FROM_HERE, rtc::Bind(&ArLive2Pusher::enableVolumeEvaluation, this, intervalMs));
	}
	if (intervalMs % 100 != 0) {
		return ArLIVE_ERROR_INVALID_PARAMETER;
	}

	n_volume_evaluation_interval_ = intervalMs;
	n_volume_evaluation_next_ms_ = intervalMs;
	return ArLIVE_OK;
}
int32_t ArLive2Pusher::enableCustomVideoCapture(bool enable)
{
	if (!main_thread_->IsCurrent()) {
		return main_thread_->Invoke<int>(RTC_FROM_HERE, rtc::Bind(&ArLive2Pusher::enableCustomVideoCapture, this, enable));
	}
	if (b_live_pushed_) {
		return ArLIVE_ERROR_REFUSED;
	}
	b_enable_custom_video_capture_ = enable;
	if (enable) {// ���Զ�����Ƶ�ɼ�����Ҫ�ر�Ĭ�ϵ���Ƶ�ɼ�
		stopCamera();
	}
	return ArLIVE_OK;
}
int32_t ArLive2Pusher::sendCustomVideoFrame(ArLiveVideoFrame* videoFrame)
{
	if (!b_enable_custom_video_capture_) {
		return ArLIVE_ERROR_REFUSED;
	}
	if (videoFrame->pixelFormat == ArLivePixelFormatI420 || videoFrame->pixelFormat == ArLivePixelFormatBGRA32 || videoFrame->pixelFormat == ArLivePixelFormatNV12||videoFrame->pixelFormat == ArLivePixelFormatNV21) {
		rtc::scoped_refptr<webrtc::I420Buffer> i420Buffer = webrtc::I420Buffer::Create(
			videoFrame->width, videoFrame->height, videoFrame->width, videoFrame->width / 2, videoFrame->width / 2);
		if (videoFrame->stride == 0) {
			if (videoFrame->pixelFormat == ArLivePixelFormatBGRA32) {
				videoFrame->stride = videoFrame->width * 4;
			}
			else {
				videoFrame->stride = videoFrame->width;
			}
		}
		if (videoFrame->pixelFormat == ArLivePixelFormatI420) {
			libyuv::I420Copy((uint8_t*)(videoFrame->data), videoFrame->stride, (uint8_t*)(videoFrame->data+(videoFrame->stride *videoFrame->height)), videoFrame->stride /2, (uint8_t*)(videoFrame->data + (videoFrame->stride * videoFrame->height)*5/4), videoFrame->stride /2,
				(uint8_t*)i420Buffer->DataY(), i420Buffer->StrideY(), (uint8_t*)i420Buffer->DataU(), i420Buffer->StrideU(),
				(uint8_t*)i420Buffer->DataV(), i420Buffer->StrideV(), i420Buffer->width(), i420Buffer->height());
		}
		else if (videoFrame->pixelFormat == ArLivePixelFormatBGRA32) {
			libyuv::ABGRToI420((uint8_t*)(videoFrame->data), videoFrame->stride,
				(uint8_t*)i420Buffer->DataY(), i420Buffer->StrideY(), (uint8_t*)i420Buffer->DataU(), i420Buffer->StrideU(),
				(uint8_t*)i420Buffer->DataV(), i420Buffer->StrideV(), i420Buffer->width(), i420Buffer->height());
		}
		else if (videoFrame->pixelFormat == ArLivePixelFormatNV12) {
			libyuv::NV12ToI420((uint8_t*)(videoFrame->data), videoFrame->stride, (uint8_t*)(videoFrame->data + (videoFrame->stride * videoFrame->height)), videoFrame->stride,
				(uint8_t*)i420Buffer->DataY(), i420Buffer->StrideY(), (uint8_t*)i420Buffer->DataU(), i420Buffer->StrideU(),
				(uint8_t*)i420Buffer->DataV(), i420Buffer->StrideV(), i420Buffer->width(), i420Buffer->height());
		}
		else if (videoFrame->pixelFormat == ArLivePixelFormatNV21) {
			int ww = videoFrame->width;
			int hh = videoFrame->height;
			uint8_t *dataY = (uint8_t *) (videoFrame->data);
			uint8_t *dataUV = dataY + ww * hh;
			libyuv::NV21ToI420(dataY, ww, dataUV, ww,
							   (uint8_t *) i420Buffer->DataY(),
							   i420Buffer->StrideY(),
							   (uint8_t *) i420Buffer->DataU(),
							   i420Buffer->StrideU(),
							   (uint8_t *) i420Buffer->DataV(),
							   i420Buffer->StrideV(),
							   i420Buffer->width(), i420Buffer->height());

		}

		webrtc::VideoFrame video_frame(i420Buffer, 0, rtc::TimeMillis(),
									   static_cast<webrtc::VideoRotation>(videoFrame->rotation));
		OnFrame(video_frame);
	}

	return ArLIVE_OK;
}
#ifdef _WIN32
int32_t ArLive2Pusher::enableCustomRendering(bool enable, ArLivePixelFormat pixelFormat, ArLiveBufferType bufferType)
{
	return ArLIVE_OK;
}
#endif
int ArLive2Pusher::enableCustomAudioCapture(bool enable)
{
	if (!main_thread_->IsCurrent()) {
		return main_thread_->Invoke<int>(RTC_FROM_HERE, rtc::Bind(&ArLive2Pusher::enableCustomAudioCapture, this, enable));
	}
	if (b_live_pushed_) {
		return ArLIVE_ERROR_REFUSED;
	}
	b_enable_custom_audio_capture_ = enable;
	if (enable) {// ���Զ�����Ƶ�ɼ�����Ҫ�ر�Ĭ�ϵ���Ƶ�ɼ�
		stopMicrophone();
	}
	return ArLIVE_OK;
}

int ArLive2Pusher::sendCustomAudioFrame(ArLiveAudioFrame* audioFrame)
{
	if (!b_enable_custom_audio_capture_) {
		return ArLIVE_ERROR_REFUSED;
	}
	RecordedDataIsAvailable(audioFrame->data, audioFrame->length / (audioFrame->channel * sizeof(short)), sizeof(short), audioFrame->channel, audioFrame->sampleRate, 0);

	return ArLIVE_OK;
}

int32_t ArLive2Pusher::sendSeiMessage(int payloadType, const uint8_t* data, uint32_t dataSize)
{
	if (!main_thread_->IsCurrent()) {
		return main_thread_->Invoke<int>(RTC_FROM_HERE, rtc::Bind(&ArLive2Pusher::sendSeiMessage, this, payloadType, data, dataSize));
	}
	if (data == NULL || dataSize <= 0 || (payloadType != 5 && payloadType != 242)) {
		return ArLIVE_ERROR_INVALID_PARAMETER;
	}
	if (!b_live_pushed_) {
		return ArLIVE_ERROR_REFUSED;
	}
	
	SeiMsg* seiMsg = new SeiMsg();
	seiMsg->ePayloadType = (sei_payload_type_e)payloadType;
	seiMsg->SetMsg((char*)data, dataSize);
	lst_sei_msg_.push_back(seiMsg);
	return ArLIVE_OK;
}
#if TARGET_PLATFORM_DESKTOP
int32_t ArLive2Pusher::startSystemAudioLoopback(const char* path)
{
	return ArLIVE_OK;
}
int32_t ArLive2Pusher::stopSystemAudioLoopback()
{
	return ArLIVE_OK;
}
int32_t ArLive2Pusher::enableCustomVideoProcess(bool enable, ArLivePixelFormat pixelFormat, ArLiveBufferType bufferType)
{
	return ArLIVE_OK;
}
#ifdef _WIN32
void ArLive2Pusher::startScreenCapture()
{
	return;
}
#endif
#ifdef _WIN32
void ArLive2Pusher::stopScreenCapture()
{
	return;
}
#endif
#ifdef _WIN32
IArLiveScreenCaptureSourceList* ArLive2Pusher::getScreenCaptureSources(const SIZE& thumbSize, const SIZE& iconSize)
{
	return NULL;
}
#endif
#ifdef _WIN32
void ArLive2Pusher::setScreenCaptureSource(const ArLiveScreenCaptureSourceInfo& source, const RECT& captureRect, const ArLiveScreenCaptureProperty& property)
{
	return;
}
#endif
#endif
void ArLive2Pusher::showDebugView(bool isShow)
{
	
}

//* For RtcTick
void ArLive2Pusher::OnTick()
{
	if (ar_pusher_ != NULL) {
		ar_pusher_->runOnce();
	}

	if (n_volume_evaluation_next_ms_ != 0) {
		if (n_volume_evaluation_next_ms_ <= rtc::TimeUTCMillis()) {
			n_volume_evaluation_next_ms_ = rtc::TimeUTCMillis() + n_volume_evaluation_interval_;
			if (event_live_push_ != NULL) {
				event_live_push_->onMicrophoneVolumeUpdate(n_volume_captured_);
			}
			n_volume_captured_ = 0;
		}
	}
	if (n_stat_timer_ms_ != 0) {
		if (n_stat_timer_ms_ <= rtc::TimeUTCMillis()) {
			n_stat_timer_ms_ = rtc::TimeUTCMillis() + 1000;
			if (event_live_push_ != NULL) {
				event_live_push_->onStatisticsUpdate(pusher_statistics_);
			}
			pusher_statistics_.fps = 0;
			pusher_statistics_.videoBitrate = 0;
			pusher_statistics_.audioBitrate = 0;
		}
	}

	if (lst_sei_msg_.size() > 0) {
		if (n_last_keyframe_time_ != 0 && n_last_keyframe_time_ + 250 <= rtc::TimeUTCMillis()) {
			n_last_keyframe_time_ = 0;

			webrtc::MutexLock l(&cs_h264_encoder_);
			if (h264_encoder_ != NULL) {
				h264_encoder_->RequestKeyFrame();
			}
		}
	}

	if (n_next_virtual_camera_time_ != 0 && n_next_virtual_camera_time_ <= rtc::TimeUTCMillis()) {
		n_next_virtual_camera_time_ = rtc::TimeUTCMillis() + 1000 / video_quality_.videoFps;
		if (video_virtual_camera_ != NULL) {
			webrtc::VideoFrame video_frame(video_virtual_camera_, 0, rtc::TimeMillis(), webrtc::kVideoRotation_0);
			OnFrame(video_frame);
		}
	}
	
}
void ArLive2Pusher::OnTickUnAttach()
{
}


//* For ArLivePushSinkInterface
void ArLive2Pusher::initAudioWithParameters(int nType, int sampleRate, int numChannels, int audBitrate)
{
	webrtc::MutexLock l(&cs_aac_encoder_);
	if (aac_encoder_ == NULL) {
		aac_encoder_ = new webrtc::A_AACEncoder(*this);
		aac_encoder_->Init(sampleRate, numChannels, audBitrate);
	}
}
void ArLive2Pusher::deinitAudio()
{
	webrtc::MutexLock l(&cs_aac_encoder_);
	if (aac_encoder_ != NULL) {
		aac_encoder_->DeInit();
		delete aac_encoder_;
		aac_encoder_ = NULL;
	}
}
void ArLive2Pusher::initVideoWithParameters(int nType, int videoWidth, int videoHeight, int videoFps, int videoBitrate)
{
	webrtc::MutexLock l(&cs_h264_encoder_);
	if (h264_encoder_ != NULL) {
		h264_encoder_->DestoryVideoEncoder();
		delete h264_encoder_;
		h264_encoder_ = NULL;
	}
	pusher_statistics_.width = videoWidth;
	pusher_statistics_.height = videoHeight;
	h264_encoder_ = new webrtc::V_H264Encoder(*this);
	h264_encoder_->SetParameter(videoWidth, videoHeight, videoFps, videoBitrate);
	h264_encoder_->SetVideoScaleMode((webrtc::VideoScaleMode)video_quality_.videoScaleMode);
#if TARGET_PLATFORM_PHONE
	if (exVideo_encoder_factory != NULL) {
		h264_encoder_->SetExVideoEncoderFactory(exVideo_encoder_factory);
	}
#endif
	h264_encoder_->CreateVideoEncoder();
	
}
void ArLive2Pusher::deinitVideo()
{
	webrtc::MutexLock l(&cs_h264_encoder_);
	if (h264_encoder_ != NULL) {
		h264_encoder_->DestoryVideoEncoder();
		delete h264_encoder_;
		h264_encoder_ = NULL;
	}
}

//* For AudDevCaptureEvent
void ArLive2Pusher::RecordedDataIsAvailable(const void* audioSamples, const size_t nSamples,
	const size_t nBytesPerSample, const size_t nChannels, const uint32_t samplesPerSec, const uint32_t totalDelayMS)
{
	if (!b_audio_captured_) {
		b_audio_captured_ = true;
		if (event_live_push_ != NULL) {
			event_live_push_->onCaptureFirstAudioFrame();
		}
	}
	if (b_live_pushed_) {
		int nAud10msLen = (samplesPerSec / 100);
		int nAudUsed = 0;
		while (nAudUsed + nAud10msLen <= nSamples) {
			const void* ptr = (char*)audioSamples + nAudUsed*nChannels*sizeof(short);
			if (b_audio_paused_) {
#if 0
				char muteAudData[1920] = { 0 };
				webrtc::MutexLock l(&cs_aac_encoder_);
				if (aac_encoder_ != NULL) {
					aac_encoder_->Encode(muteAudData, nSamples, nBytesPerSample, nChannels, samplesPerSec, totalDelayMS);
				}
#endif
			}
			else {
				webrtc::MutexLock l(&cs_aac_encoder_);
				if (aac_encoder_ != NULL) {
					aac_encoder_->Encode(ptr, nSamples, nBytesPerSample, nChannels, samplesPerSec, totalDelayMS);
				}
			}
			nAudUsed += nAud10msLen;
		}
		
	}

	if (n_volume_evaluation_next_ms_ != 0) {
		int max_abs = WebRtcSpl_MaxAbsValueW16_I((int16_t*)(audioSamples), nSamples * nChannels);
		max_abs = (max_abs * 100) / 32767;
		n_volume_captured_ = max_abs;
	}
}

// rtc::VideoSinkInterface<webrtc::VideoFrame>
void ArLive2Pusher::OnFrame(const webrtc::VideoFrame& frame)
{
	if (!b_video_captured_) {
		b_video_captured_ = true;
		if (event_live_push_ != NULL) {
			event_live_push_->onCaptureFirstVideoFrame();
		}
	}

	ar_engine_->GetMgrRender().DoRenderFrame(str_local_push_id_.c_str(), frame);

	if (b_live_pushed_) {
		if (!b_video_paused_ && b_client_connected_) {
			webrtc::MutexLock l(&cs_h264_encoder_);
			if (h264_encoder_ != NULL) {
				h264_encoder_->Encode(frame);
			}
		}

		if (b_snap_shot_) {
			b_snap_shot_ = false;

			if (event_live_push_ != NULL) {
				char* dst_frame = new char[frame.width() * frame.height() * 4];
				if (frame.video_frame_buffer()->GetI420() != NULL && frame.rotation() == webrtc::kVideoRotation_0) {
					const webrtc::I420BufferInterface* i420_buffer =
						frame.video_frame_buffer()->GetI420();
					libyuv::I420ToABGR(i420_buffer->DataY(), i420_buffer->StrideY(), i420_buffer->DataU(), i420_buffer->StrideU(), i420_buffer->DataV(), i420_buffer->StrideV(), (uint8_t*)dst_frame, frame.width() * 4, frame.width(), frame.height());
				}
				else {
					/* Apply pending rotation. */
					rtc::scoped_refptr<webrtc::VideoFrameBuffer> buffer(
						frame.video_frame_buffer());

					webrtc::VideoFrame rotated_frame(frame);
					if (buffer->GetI420() != NULL) {
						rotated_frame.set_video_frame_buffer(
							webrtc::I420Buffer::Rotate(*buffer->GetI420(), frame.rotation()));
					}
					else {
						rotated_frame.set_video_frame_buffer(
							webrtc::I420Buffer::Rotate(*buffer->ToI420(), frame.rotation()));
					}

					rotated_frame.set_rotation(webrtc::kVideoRotation_0);
					const webrtc::I420BufferInterface* i420_buffer =
						rotated_frame.video_frame_buffer()->GetI420();
					libyuv::I420ToABGR(i420_buffer->DataY(), i420_buffer->StrideY(), i420_buffer->DataU(), i420_buffer->StrideU(), i420_buffer->DataV(), i420_buffer->StrideV(), (uint8_t*)dst_frame, rotated_frame.width() * 4, rotated_frame.width(), rotated_frame.height());
				}

				if (frame.rotation() == webrtc::kVideoRotation_0 || frame.rotation() == webrtc::kVideoRotation_180) {
					event_live_push_->onSnapshotComplete(dst_frame, frame.width() * frame.height() * 4, frame.width(), frame.height(), ArLivePixelFormatBGRA32);
				}
				else {
					event_live_push_->onSnapshotComplete(dst_frame, frame.width() * frame.height() * 4, frame.height(), frame.width(),  ArLivePixelFormatBGRA32);
				}
				delete[] dst_frame;
			}
		}
	}
}

//* For AR::ArLivePusherObserver
void ArLive2Pusher::onError(int32_t code, const char* msg, void* extraInfo)
{
	if (event_live_push_ != NULL) {
		event_live_push_->onError(code, msg, extraInfo);
	}
}
void ArLive2Pusher::onWarning(int32_t code, const char* msg, void* extraInfo)
{
	if (event_live_push_ != NULL) {
		event_live_push_->onWarning(code, msg, extraInfo);
	}
}
void ArLive2Pusher::onPushStatusUpdate(ArLivePushStatus state, const char* msg, void* extraInfo)
{
	if (state == ArLivePushStatus::ArLivePushStatusConnectSuccess) {
		//initAudioWithParameters(0, 44100, 2, 64);
		//initVideoWithParameters(0, 640, 480, 25, 1024);
		b_client_connected_ = true;
	}
	else {
		//deinitVideo();
		//deinitAudio();
		b_client_connected_ = false;
	}

	if (event_live_push_ != NULL) {
		event_live_push_->onPushStatusUpdate(state, msg, extraInfo);
	}
}
void ArLive2Pusher::onStatisticsUpdate(ArLivePusherStatistics statistics)
{
	if (event_live_push_ != NULL) {
		event_live_push_->onStatisticsUpdate(statistics);
	}
}

//* For AVCodecCallback
void ArLive2Pusher::OnEncodeDataCallback(bool audio, bool bKeyFrame, const uint8_t *pData, uint32_t nLen, uint32_t ts)
{
	if (audio)
	{
		pusher_statistics_.audioBitrate += nLen;
		if (ar_pusher_ != NULL) {
			ar_pusher_->setAudioData((char*)pData, nLen, ts);
		}
	}
	else
	{
		pusher_statistics_.fps++;
		pusher_statistics_.videoBitrate += nLen;
		if (bKeyFrame) {
			n_last_keyframe_time_ = rtc::TimeUTCMillis();
			SeiMsg* seiMsg = NULL;
			if (lst_sei_msg_.size() > 0) {
				seiMsg = lst_sei_msg_.front();
				lst_sei_msg_.pop_front();
			}

			if (seiMsg != NULL) {
				int nKeySize = nLen + seiMsg->nLen + 16;
				char* pKeyData = new char[nKeySize];
				int nKeyLen = h264_insert_sei(pKeyData, (char*)pData, nLen, seiMsg->pMsg, seiMsg->nLen, seiMsg->ePayloadType);

				if (ar_pusher_ != NULL) {
					ar_pusher_->setVideoData((char*)pKeyData, nKeyLen, bKeyFrame, ts);
				}
				delete[] pKeyData;
				delete seiMsg;
				seiMsg = NULL;
				return;
			}
		}
		if (ar_pusher_ != NULL) {
			ar_pusher_->setVideoData((char*)pData, nLen, bKeyFrame, ts);
		}
	}
}



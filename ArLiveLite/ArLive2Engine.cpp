#include "ArLive2Engine.h"
#include "ArLive2Player.h"
#include "ArLive2Pusher.h"
#include "rtc_base/bind.h"
#include "api/task_queue/default_task_queue_factory.h"
#include "modules/audio_device/include/audio_device.h"
#include "rtc_base/internal/default_socket_server.h"
#include "rtc_base/task_queue_gcd.h"
#include "modules/video_capture/video_capture_impl.h"
#include "modules/audio_device/dummy/audio_device_dummy.h"
#include "api/audio_codecs/audio_decoder_factory.h"
#include "api/audio_codecs/audio_encoder_factory.h"
#include "api/audio_codecs/builtin_audio_decoder_factory.h"
#include "api/audio_codecs/builtin_audio_encoder_factory.h"
#include "api/audio_options.h"
#include "api/create_peerconnection_factory.h"
#include "api/video_codecs/builtin_video_decoder_factory.h"
#include "api/video_codecs/builtin_video_encoder_factory.h"
#include "api/video_codecs/video_decoder_factory.h"
#include "api/video_codecs/video_encoder_factory.h"
#ifdef WIN32
#include "rtc_base/win32_socket_server.h"
#include "rtc_base/win32_socket_init.h"
#endif
#ifdef WEBRTC_IOS
#import "sdk/objc/components/video_codec/RTCVideoDecoderFactoryH264.h"
#import "sdk/objc/components/video_codec/RTCVideoEncoderFactoryH264.h"
#include "sdk/objc/native/api/video_decoder_factory.h"
#include "sdk/objc/native/api/video_encoder_factory.h"
#include "sdk/objc/native/src/objc_video_encoder_factory.h"
#include "sdk/objc/native/src/objc_video_decoder_factory.h"
#include "api/call/audio_sink.h"
#endif

static const char sess_ascii[65] = {
	'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H', 'I', 'J', 'K', 'L', 'M',
	'N', 'O', 'P', 'Q', 'R', 'S', 'T', 'U', 'V', 'W', 'X', 'Y', 'Z',
	'a', 'b', 'c', 'd', 'e', 'f', 'g', 'h', 'i', 'j', 'k', 'l', 'm',
	'n', 'o', 'p', 'q', 'r', 's', 't', 'u', 'v', 'w', 'x', 'y', 'z',
	'0', '1', '2', '3', '4', '5', '6', '7', '8', '9', '0', '0', 0
};
void CreateRandomString(std::string& sRandStr, uint32_t len)
{
	rtc::CreateRandomString(len, sess_ascii, &sRandStr);
}
// RtcEngine 是单例只能创建一个
static ArLive2Engine* gInstance = NULL;

AR::IArLive2Engine* V2_CALL AR::createArLive2Engine()
{
	if (gInstance == NULL) {
		gInstance = new ArLive2Engine();
	}
	return gInstance;
}
ArLive2Engine::ArLive2Engine(void)
	: rtc::Thread(rtc::CreateDefaultSocketServer())
	, observer_(NULL)
	, b_running_(false)
	, b_app_background_(false)
	, b_aud_cap_exception_(false)
	, b_aud_ply_exception_(false)
	, b_video_preview_(false)
	, b_video_muted_(false)
	, n_timer_10ms_(0)
	, n_timer_100ms_(0)
{
	
}
ArLive2Engine::~ArLive2Engine(void)
{
	//* 引擎销毁时，清空单例变量
	gInstance = NULL;
}

//* For IArLive2Engine
int32_t ArLive2Engine::initialize(AR::IArLive2EngineObserver* observer)
{
	if (!b_running_) {
		b_running_ = true;
		observer_ = observer;
		rtc::Thread::Start();

		rtc::Thread::Invoke<void>(RTC_FROM_HERE, rtc::Bind(&ArLive2Engine::InitAudDevice, this));

		rtc::Thread::Invoke<void>(RTC_FROM_HERE, rtc::Bind(&ArLive2Engine::InitPeerConnection, this));
	}
	else {
		return AR::ArLIVE_ERROR_FAILED;
	}
	return AR::ArLIVE_OK;
}

void ArLive2Engine::release()
{
	if (b_running_) {
		{
			rtc::CritScope l(&cs_arlive2_pusher_);
			if (map_arlive2_pusher_.size() > 0) {
				//Warn
			}
		}
		{
			rtc::CritScope l(&cs_arlive2_player_);
			if (map_arlive2_player_.size() > 0) {
				//Warn
			}
		}
		rtc::Thread::Invoke<void>(RTC_FROM_HERE, rtc::Bind(&ArLive2Engine::DeInitPeerConnection, this));

		rtc::Thread::Invoke<void>(RTC_FROM_HERE, rtc::Bind(&ArLive2Engine::DeInitAudDevice, this));

		b_running_ = false;
		rtc::Thread::Stop();
	}

	delete this;
}

#ifdef __ANDROID__
AR::IArLivePusher* ArLive2Engine::createArLivePusher(void* context)
{
	rtc::CritScope l(&cs_arlive2_pusher_);
	std::string strPushId;
	CreateRandomString(strPushId, 16);
	while (map_arlive2_pusher_.find(strPushId) != map_arlive2_pusher_.end()) {
		CreateRandomString(strPushId, 16);
	}
	ArLive2Pusher* arLivePuser = new ArLive2Pusher(this, strPushId);
	map_arlive2_pusher_[strPushId] = arLivePuser;
	return arLivePuser;

}

AR::IArLivePlayer* ArLive2Engine::createArLivePlayer(void* context)
{
	rtc::CritScope l(&cs_arlive2_player_);
	std::string strPlayId;
	CreateRandomString(strPlayId, 16);
	while (map_arlive2_player_.find(strPlayId) != map_arlive2_player_.end()) {
		CreateRandomString(strPlayId, 16);
	}
	ArLive2Player* arLivePlayer = new ArLive2Player(this, strPlayId);
	map_arlive2_player_[strPlayId] = arLivePlayer;
	return arLivePlayer;
}
#else
AR::IArLivePusher* ArLive2Engine::createArLivePusher()
{
	rtc::CritScope l(&cs_arlive2_pusher_);
	std::string strPushId;
	CreateRandomString(strPushId, 16);
	while (map_arlive2_pusher_.find(strPushId) != map_arlive2_pusher_.end()) {
		CreateRandomString(strPushId, 16);
	}
	ArLive2Pusher* arLivePuser = new ArLive2Pusher(this, strPushId);
	map_arlive2_pusher_[strPushId] = arLivePuser;
#if WEBRTC_IOS
    std::unique_ptr<webrtc::VideoEncoderFactory> video_apple_encoder_factory_ = webrtc::ObjCToNativeVideoEncoderFactory([[RTCVideoEncoderFactoryH264 alloc] init]);
    arLivePuser->setExVideoEncoderFactory(video_apple_encoder_factory_.release());
#endif
	return arLivePuser;
}

AR::IArLivePlayer* ArLive2Engine::createArLivePlayer()
{
	rtc::CritScope l(&cs_arlive2_player_);
	std::string strPlayId;
	CreateRandomString(strPlayId, 16);
	while (map_arlive2_player_.find(strPlayId) != map_arlive2_player_.end()) {
		CreateRandomString(strPlayId, 16);
	}
	ArLive2Player* arLivePlayer = new ArLive2Player(this, strPlayId);
	map_arlive2_player_[strPlayId] = arLivePlayer;
	return arLivePlayer;
}
#endif
void ArLive2Engine::releaseArLivePusher(AR::IArLivePusher* pusher)
{
	if (pusher != NULL) {
		ArLive2Pusher* arLivePusher = (ArLive2Pusher*)pusher;
		{
			rtc::CritScope l(&cs_arlive2_pusher_);
			if (map_arlive2_pusher_.find(arLivePusher->PushId()) != map_arlive2_pusher_.end()) {
				map_arlive2_pusher_.erase(arLivePusher->PushId());
			}
		}
		arLivePusher->stopPush();
		delete arLivePusher;
		pusher = NULL;
	}
}

void ArLive2Engine::releaseArLivePlayer(AR::IArLivePlayer* player)
{
	if (player != NULL) {
		ArLive2Player* arLivePlayer = (ArLive2Player*)player;
		{
			rtc::CritScope l(&cs_arlive2_player_);
			if (map_arlive2_player_.find(arLivePlayer->PlayId()) != map_arlive2_player_.end()) {
				map_arlive2_player_.erase(arLivePlayer->PlayId());
			}
		}
		arLivePlayer->stopPlay();
		delete (ArLive2Player*)arLivePlayer;
		player = NULL;
	}
}

void ArLive2Engine::setAppInBackground(bool bBackground)
{
	b_app_background_ = bBackground;
	rtc::CritScope l(&cs_arlive2_player_);
	MapArLive2Player::iterator itpr = map_arlive2_player_.begin();
	while (itpr != map_arlive2_player_.end()) {
		ArLive2Player* arLivePlayer = (ArLive2Player*)itpr->second;
		arLivePlayer->SetAppInBackground(bBackground);
		itpr++;
	}

}


int32_t ArLive2Engine::setVideoRenderView(AR::uid_t renderId, const AR::VideoCanvas& canvas)
{
	if (renderId != NULL) {
		mgr_render_.SetRender(renderId, NULL);
		if (canvas.view != NULL) {
			webrtc::VideoRenderer *render = webrtc::VideoRenderer::CreatePlatformRenderer(canvas.view, 640, 480);
			mgr_render_.SetRender(renderId, render);
		}
	}
	return 0; 
}

int32_t ArLive2Engine::setVideoRenderMirror(AR::uid_t renderId, AR::ArLiveMirrorType mirrorType)
{
	return 0;
}

int32_t ArLive2Engine::setVideoRenderRotation(AR::uid_t renderId, AR::ArLiveRotation rotation)
{
	int nRotation = 0;
	if (rotation == AR::ArLiveRotation90) {
		nRotation = 90;
	} 
	else if (rotation == AR::ArLiveRotation180) {
		nRotation = 180;
	}
	else if (rotation == AR::ArLiveRotation270) {
		nRotation = 270;
	}
	if (renderId != NULL) {
		mgr_render_.SetRotation(renderId, nRotation);
	}
	return 0; 
}

int32_t ArLive2Engine::setVideoRenderFillMode(AR::uid_t renderId, AR::ArLiveFillMode mode)
{
	if (renderId != NULL) {
		mgr_render_.SetFillMode(renderId, mode == AR::ArLiveFillModeFill ? 0 : 1);
	}
	return 0;
}

//* For rtc::Thread
void ArLive2Engine::Run()
{
#ifdef WIN32
	rtc::WinsockInitializer winInit;
	// Need to pump messages on our main thread on Windows.
	rtc::Win32SocketServer server;
	rtc::Win32Thread w32_thread(&server);
#endif
	while (b_running_)
	{
		MThreadTick::DoProcess();
		if (n_timer_10ms_ == 0) {
			n_timer_10ms_ = rtc::TimeUTCMillis();
		}
		while (n_timer_10ms_ <= rtc::TimeUTCMillis()) {
			n_timer_10ms_ += 10;
		}

		if (n_timer_100ms_ <= rtc::TimeUTCMillis()) {
			n_timer_100ms_ = rtc::TimeUTCMillis() + 100;

			if (b_aud_cap_exception_) {// 音频采集设备打开异常，再次尝试打开
				audio_device_ptr_->InitRecording();
				if (audio_device_ptr_->StartRecording() != 0) {
					audio_device_ptr_->StopRecording();
					b_aud_cap_exception_ = true;
				}
				else {
					b_aud_cap_exception_ = false;
				}
			}

			if (b_aud_ply_exception_) {// 音频播放设备打开异常，再次尝试打开
				audio_device_ptr_->InitPlayout();
				if (audio_device_ptr_->StartPlayout() != 0) {
					audio_device_ptr_->StopPlayout();
					b_aud_ply_exception_ = true;
				}
				else {
					b_aud_ply_exception_ = false;
				}
			}
		}

		rtc::Thread::ProcessMessages(1);
#ifdef WIN32
		w32_thread.ProcessMessages(1);
#endif
		rtc::Thread::SleepMs(1);
	}
}

//* For webrtc::AudioTransport
int32_t ArLive2Engine::RecordedDataIsAvailable(const void* audioSamples, const size_t nSamples,
	const size_t nBytesPerSample, const size_t nChannels, const uint32_t samplesPerSec, const uint32_t totalDelayMS,
	const int32_t clockDrift, const uint32_t currentMicLevel, const bool keyPressed, uint32_t& newMicLevel)
{
	rtc::CritScope l(&cs_aud_capture_);
	MapAudDevCapture::iterator itadr = map_aud_dev_capture_.begin();
	while (itadr != map_aud_dev_capture_.end()) {
		itadr->first->RecordedDataIsAvailable(audioSamples, nSamples, nBytesPerSample, nChannels, samplesPerSec, totalDelayMS);
		itadr++;
	}

	if (rtc_adm_ != NULL)
	{
		webrtc::AudioSinkInterface::Data audData((int16_t*)audioSamples, nSamples, samplesPerSec, nChannels, rtc::Time32());
		((webrtc::AudioDeviceDummy*)rtc_adm_->DummyDevice())->SetRecordAudioData(audData);
	}
	return 0;
}

int32_t ArLive2Engine::NeedMorePlayData(const size_t nSamples, const size_t nBytesPerSample, const size_t nChannels,
	const uint32_t samplesPerSec, void* audioSamples, size_t& nSamplesOut, int64_t* elapsed_time_ms, int64_t* ntp_time_ms)
{
	bool bMix = false;
	memset(audioSamples, 0, nSamples* nBytesPerSample);
	{
		rtc::CritScope l(&cs_aud_speaker_);
		MapAudDevSpeaker::iterator iter = map_aud_dev_speaker_.begin();
		while (iter != map_aud_dev_speaker_.end()) {
			if (iter->first->MixAudioData(bMix, audioSamples, samplesPerSec, nChannels) > 0) {
				bMix = true;
			}
			iter++;
		}
	}

	int samples_per_channel_int = samplesPerSec / 100;
	nSamplesOut = samples_per_channel_int * nChannels;

	if (rtc_adm_ != NULL)
	{
		char pData[1920];
		uint32_t nSampleHz = 48000;
		size_t nChannel = 1;
		{
			((webrtc::AudioDeviceDummy*)rtc_adm_->DummyDevice())->GetNeedPlayAudio(pData, nSampleHz, nChannel);
		}
	}
	return 0;
}


void ArLive2Engine::InitAudDevice()
{
	RTC_CHECK(audio_device_ptr_ == NULL);
	if (task_queue_factory_ == NULL) {
#if defined(WEBRTC_IOS) | defined(WEBRTC_MAC)
		task_queue_factory_ = webrtc::CreateTaskQueueGcdFactory();
#else
		task_queue_factory_ = webrtc::CreateDefaultTaskQueueFactory();
#endif
	}
	audio_device_ptr_ = webrtc::AudioDeviceModule::Create(webrtc::AudioDeviceModule::kPlatformDefaultAudio, task_queue_factory_.get());
	audio_device_ptr_->Init();
	audio_device_ptr_->AddRef();
	if (audio_device_ptr_->BuiltInAECIsAvailable())
		audio_device_ptr_->EnableBuiltInAEC(false);
	if (audio_device_ptr_->BuiltInAGCIsAvailable())
		audio_device_ptr_->EnableBuiltInAGC(false);
	if (audio_device_ptr_->BuiltInNSIsAvailable())
		audio_device_ptr_->EnableBuiltInNS(false);
	audio_device_ptr_->RegisterAudioCallback(this);
	// Initialize the default microphone
#ifdef WIN32
	if (audio_device_ptr_->SetRecordingDevice(
		webrtc::AudioDeviceModule::kDefaultCommunicationDevice) != 0) {
		audio_device_ptr_->InitMicrophone();
	}
#endif

#ifdef WIN32
	if (audio_device_ptr_->SetPlayoutDevice(webrtc::AudioDeviceModule::kDefaultCommunicationDevice) == 0) {
		audio_device_ptr_->InitSpeaker();
		audio_device_ptr_->SetStereoPlayout(true);
	}
#endif
}
void ArLive2Engine::DeInitAudDevice()
{
	RTC_CHECK(audio_device_ptr_ != NULL);
	if (audio_device_ptr_->Recording())
		audio_device_ptr_->StopRecording();
	if (audio_device_ptr_->Playing())
		audio_device_ptr_->StopPlayout();
	audio_device_ptr_->RegisterAudioCallback(NULL);
	audio_device_ptr_->Release();
	audio_device_ptr_ = NULL;
}
void ArLive2Engine::InitPeerConnection()
{
	if (task_queue_factory_ == NULL) {
#if defined(WEBRTC_IOS) | defined(WEBRTC_MAC)
		task_queue_factory_ = webrtc::CreateTaskQueueGcdFactory();
#else
		task_queue_factory_ = webrtc::CreateDefaultTaskQueueFactory();
#endif
}
	rtc_adm_ = webrtc::AudioDeviceModule::Create(webrtc::AudioDeviceModule::kDummyAudio, task_queue_factory_.get());
	rtc_adm_->Init();

#if WEBRTC_IOS
    peer_connection_factory_ = webrtc::CreatePeerConnectionFactory(
        nullptr /* network_thread */, this /* worker_thread */,
        nullptr /* signaling_thread */, rtc_adm_ /* default_adm */,
        webrtc::CreateBuiltinAudioEncoderFactory(),
        webrtc::CreateBuiltinAudioDecoderFactory(),
        webrtc::ObjCToNativeVideoEncoderFactory([[RTCVideoEncoderFactoryH264 alloc] init]),
        webrtc::ObjCToNativeVideoDecoderFactory([[RTCVideoDecoderFactoryH264 alloc] init]), nullptr /* audio_mixer */,
        nullptr /* audio_processing */);
#else
    peer_connection_factory_ = webrtc::CreatePeerConnectionFactory(
        nullptr /* network_thread */, this /* worker_thread */,
        nullptr /* signaling_thread */, rtc_adm_ /* default_adm */,
        webrtc::CreateBuiltinAudioEncoderFactory(),
        webrtc::CreateBuiltinAudioDecoderFactory(),
        webrtc::CreateBuiltinVideoEncoderFactory(),
        webrtc::CreateBuiltinVideoDecoderFactory(), nullptr /* audio_mixer */,
        nullptr /* audio_processing */);
#endif

	//* 必须在peer_connection_factory_之后，否则audioBuffer里面的audioTransport会注册失败
	rtc_adm_->InitPlayout();
	rtc_adm_->InitRecording();
	rtc_adm_->StartPlayout();
	rtc_adm_->StartRecording();
}
void ArLive2Engine::DeInitPeerConnection()
{
	peer_connection_factory_ = nullptr;
	rtc_adm_->Terminate();
	rtc_adm_ = nullptr;
}
void ArLive2Engine::AttachAudCapture(AudDevCaptureEvent* pEvent)
{
	if (!rtc::Thread::IsCurrent()) {
		rtc::Thread::Invoke<void>(RTC_FROM_HERE, rtc::Bind(&ArLive2Engine::AttachAudCapture, this, pEvent));
		return;
	}
	RTC_CHECK(rtc::Thread::IsCurrent());
	bool needStartCaptuer = false;
	{
		rtc::CritScope l(&cs_aud_capture_);
		if (map_aud_dev_capture_.find(pEvent) == map_aud_dev_capture_.end()) {
			if (map_aud_dev_capture_.size() == 0) {
				needStartCaptuer = true;
			}
			map_aud_dev_capture_[pEvent] = pEvent;
		}
	}

	if (needStartCaptuer) {
		if (!audio_device_ptr_->Recording()) {
			audio_device_ptr_->InitRecording();
			if (audio_device_ptr_->StartRecording() != 0) {
				audio_device_ptr_->StopRecording();
				b_aud_cap_exception_ = true;
			}
			else {
				b_aud_cap_exception_ = false;
			}
		}
	}
}
void ArLive2Engine::DetachAudCapture(AudDevCaptureEvent*pEvent)
{
	if (!rtc::Thread::IsCurrent()) {
		rtc::Thread::Invoke<void>(RTC_FROM_HERE, rtc::Bind(&ArLive2Engine::DetachAudCapture, this, pEvent));
		return;
	}
	RTC_CHECK(rtc::Thread::IsCurrent());
	bool needStopCaptuer = false;
	{
		rtc::CritScope l(&cs_aud_capture_);
		if (map_aud_dev_capture_.find(pEvent) != map_aud_dev_capture_.end()) {
			map_aud_dev_capture_.erase(pEvent);
			if (map_aud_dev_capture_.size() == 0) {
				needStopCaptuer = true;
			}
		}
	}
	if (needStopCaptuer) {
		if (audio_device_ptr_->Recording()) {
			audio_device_ptr_->StopRecording();
		}
		b_aud_cap_exception_ = false;
	}
}

void ArLive2Engine::AttachAudSpeaker(AudDevSpeakerEvent*pEvent)
{
	if (!rtc::Thread::IsCurrent()) {
		rtc::Thread::Invoke<void>(RTC_FROM_HERE, rtc::Bind(&ArLive2Engine::AttachAudSpeaker, this, pEvent));
		return;
	}
	RTC_CHECK(rtc::Thread::IsCurrent());
	bool needStartSpeaker = false;
	{
		rtc::CritScope l(&cs_aud_speaker_);
		if (map_aud_dev_speaker_.find(pEvent) == map_aud_dev_speaker_.end()) {
			if (map_aud_dev_speaker_.size() == 0) {
				needStartSpeaker = true;
			}
			map_aud_dev_speaker_[pEvent] = pEvent;
		}
	}
	if (needStartSpeaker) {
		if (!audio_device_ptr_->Playing()) {
			audio_device_ptr_->InitPlayout();
			if (audio_device_ptr_->StartPlayout() != 0) {
				audio_device_ptr_->StopPlayout();
				b_aud_ply_exception_ = true;
			}
		}
	}
}
void ArLive2Engine::DetachAudSpeaker(AudDevSpeakerEvent*pEvent)
{
	if (!rtc::Thread::IsCurrent()) {
		rtc::Thread::Invoke<void>(RTC_FROM_HERE, rtc::Bind(&ArLive2Engine::DetachAudSpeaker, this, pEvent));
		return;
	}
	RTC_CHECK(rtc::Thread::IsCurrent());
	bool needStopSpeaker = false;
	{
		rtc::CritScope l(&cs_aud_speaker_);
		if (map_aud_dev_speaker_.find(pEvent) != map_aud_dev_speaker_.end()) {
			map_aud_dev_speaker_.erase(pEvent);

			if (map_aud_dev_speaker_.size() == 0) {
				needStopSpeaker = true;
			}
		}
	}
	if (needStopSpeaker) {
		if (audio_device_ptr_->Playing()) {
			audio_device_ptr_->StopPlayout();
		}
		b_aud_ply_exception_ = false;
	}
}

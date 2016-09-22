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
#include "RtmpHosterImpl.h"
#include "webrtc/base/bind.h"
#include "webrtc/media/base/device.h"
#include "webrtc/media/engine/webrtcvideocapturerfactory.h"
#if defined(WIN32)
#include "webrtc/modules/video_capture/video_capture_factory.h"
#endif

#define MSG_START_RTMP  1010
#define MSG_STOP_RTMP	1011

RTMPHoster* RTMPHoster::Create(RTMPHosterEvent&callback)
{
	return new RtmpHosterImpl(callback);
}
void RTMPHoster::Destory(RTMPHoster*hoster)
{
	RtmpHosterImpl* impl = (RtmpHosterImpl*)hoster->GotSelfPtr();
	delete impl;
	hoster = NULL;
}

RtmpHosterImpl::RtmpHosterImpl(RTMPHosterEvent&callback)
	: callback_(callback)
	, worker_thread_(&webrtc::AnyRtmpCore::Inst())
	, av_rtmp_started_(false)
	, av_rtmp_streamer_(NULL)
	, v_width_(640)
	, v_height_(480)
	, video_render_(NULL)
{
	if (av_rtmp_streamer_ == NULL)
		av_rtmp_streamer_ = AnyRtmpstreamer::Create(*this);

	SetVideoMode(RTMP_Video_SD);
}


RtmpHosterImpl::~RtmpHosterImpl()
{
	if (av_rtmp_streamer_ != NULL) {
		av_rtmp_streamer_->StopStream();
		delete av_rtmp_streamer_;
		av_rtmp_streamer_ = NULL;
	}

	if (video_render_ != NULL) {
		delete video_render_;
		video_render_ = NULL;
	}
}

//* Common function
void RtmpHosterImpl::SetAudioEnable(bool enabled)
{
	av_rtmp_streamer_->SetAudioEnable(enabled);
}
void RtmpHosterImpl::SetVideoEnable(bool enabled)
{
	// Not surpport?
}
void RtmpHosterImpl::SetVideoRender(void* render)
{
	if (video_render_ != NULL) {
		delete video_render_;
		video_render_ = NULL;
	}
	if (render != NULL) {
		video_render_ = webrtc::VideoRenderer::Create(render, req_w_, req_h_);
	}
}
void RtmpHosterImpl::SetVideoCapturer(void* handle)
{
	void* capturer = NULL;
	if (handle != NULL) {
#if defined(WIN32)
		SetVideoRender(handle);
		std::vector<std::string> device_names;
		{
			std::unique_ptr<webrtc::VideoCaptureModule::DeviceInfo> info(
				webrtc::VideoCaptureFactory::CreateDeviceInfo(0));
			if (!info) {
				return;
			}
			int num_devices = info->NumberOfDevices();
			for (int i = 0; i < num_devices; ++i) {
				const uint32_t kSize = 256;
				char name[kSize] = { 0 };
				char id[kSize] = { 0 };
				if (info->GetDeviceName(i, name, kSize, id, kSize) != -1) {
					device_names.push_back(name);
				}
			}
		}
		cricket::WebRtcVideoDeviceCapturerFactory factory;

		for (const auto& name : device_names) {
			capturer = factory.Create(cricket::Device(name, 0));
			if (capturer) {
				break;
			}
		}
#endif
#if defined(WEBRTC_ANDROID) || defined(WEBRTC_IOS)
		capturer = handle;
#endif
		worker_thread_->Invoke<void>(RTC_FROM_HERE, rtc::Bind(&RtmpHosterImpl::AddVideoCapturer_w, this, capturer));
	}
	else
	{
		worker_thread_->Invoke<void>(RTC_FROM_HERE, rtc::Bind(&RtmpHosterImpl::RemoveAVideoCapturer_w, this));
		SetVideoRender(NULL);
	}
}
void RtmpHosterImpl::SetVideoMode(RTMPVideoMode videoMode)
{
	req_w_ = 640;
	req_h_ = 480;
	v_width_ = 640;
	v_height_ = 360;
	int bitrate = 512;
#if defined(WEBRTC_ANDROID) || defined(WEBRTC_IOS)
		v_width_ = 360;
		v_height_ = 640;
#endif
	switch (videoMode) {
	case RTMP_Video_720P:
	{
		req_w_ = 1280;
		req_h_ = 720;
		v_width_ = 1280;
		v_height_ = 720;
#if defined(WEBRTC_ANDROID) || defined(WEBRTC_IOS)
			v_width_ = 720;
			v_height_ = 1280;
#endif
		bitrate = 1280;
	}
	break;
	case RTMP_Video_HD:
	{
		req_w_ = 1280;
		req_h_ = 720;
		v_width_ = 960;
		v_height_ = 540;
#if defined(WEBRTC_ANDROID) || defined(WEBRTC_IOS)
			v_width_ = 540;
			v_height_ = 960;
#endif
		bitrate = 1024;
	}
	break;
	case RTMP_Video_QHD:
	{
		bitrate = 768;
	}
	break;
	case RTMP_Video_SD:
	{
		bitrate = 512;
	}
	break;
	case RTMP_Video_Low:
	{
		bitrate = 384;
	}
	break;
	default:
		break;
	}
	av_rtmp_streamer_->SetVideoParameter(v_width_, v_height_, bitrate);
}

//* Rtmp function for push rtmp stream 
void RtmpHosterImpl::StartRtmpStream(const char*url)
{
	if (!av_rtmp_started_) {
		av_rtmp_started_ = true;
		rtc::VideoSinkWants wants;
		wants.rotation_applied = false;
		video_filter_.VBroadcaster().AddOrUpdateSink(((webrtc::AnyRtmpStreamerImpl*)av_rtmp_streamer_)->GetVideoSink(), wants);
		webrtc::AnyRtmpCore::Inst().StartAudioRecord(this, 44100, 2);

		rtmp_url_ = url;

		worker_thread_->PostDelayed(RTC_FROM_HERE, 500, this, MSG_START_RTMP);
	}
}
void RtmpHosterImpl::StopRtmpStream()
{
	if (av_rtmp_started_) {
		av_rtmp_started_ = false;
		webrtc::AnyRtmpCore::Inst().StopAudioRecord();
		video_filter_.VBroadcaster().RemoveSink(((webrtc::AnyRtmpStreamerImpl*)av_rtmp_streamer_)->GetVideoSink());

		worker_thread_->Post(RTC_FROM_HERE, this, MSG_STOP_RTMP);
	}
}

void RtmpHosterImpl::AddVideoCapturer_w(void* handle)
{
	cricket::VideoCapturer* capturer = (cricket::VideoCapturer *)handle;
	video_capturer_.reset(capturer);

	if (video_capturer_ != nullptr) {
		rtc::VideoSinkWants wants;
		wants.rotation_applied = true;
		if(video_render_ != NULL)
			video_capturer_->AddOrUpdateSink(video_render_, wants);
		video_capturer_->AddOrUpdateSink(&video_filter_, wants);

		{
			cricket::VideoFormat highest_asked_format;
			highest_asked_format.Construct(req_w_, req_h_, cricket::VideoFormat::FpsToInterval(30), cricket::FourCC::FOURCC_NV12);
			cricket::VideoFormat capture_format;
			if (!video_capturer_->GetBestCaptureFormat(highest_asked_format,
				&capture_format)) {
				LOG(LS_WARNING) << "Unsupported format:"
					<< " width=" << highest_asked_format.width
					<< " height=" << highest_asked_format.height
					<< ". Supported formats are:";
				const std::vector<cricket::VideoFormat>* formats =
					video_capturer_->GetSupportedFormats();
				ASSERT(formats != NULL);
				for (std::vector<cricket::VideoFormat>::const_iterator i = formats->begin();
					i != formats->end(); ++i) {
					const cricket::VideoFormat& format = *i;
					LOG(LS_WARNING) << "  " << cricket::GetFourccName(format.fourcc)
						<< ":" << format.width << "x" << format.height << "x"
						<< format.framerate();
				}
				return;
			}
			video_capturer_->StartCapturing(capture_format);
		}
	}
}
void RtmpHosterImpl::RemoveAVideoCapturer_w()
{
	if (video_capturer_)
	{
		if (video_render_ != NULL)
			video_capturer_->RemoveSink(video_render_);
		video_capturer_->RemoveSink(&video_filter_);
		if (video_capturer_->IsRunning())
		{
			video_capturer_->Stop();
		}
		video_capturer_.reset(nullptr);
	}
}

//* For AnyRtmpstreamerEvent
bool RtmpHosterImpl::ExternalVideoEncoder()
{
	return false;
}
void RtmpHosterImpl::OnStreamOk()
{
	callback_.OnRtmpStreamOK();
}
void RtmpHosterImpl::OnStreamReconnecting(int times)
{
	callback_.OnRtmpStreamReconnecting(times);
}
void RtmpHosterImpl::OnStreamFailed(int code)
{
	callback_.OnRtmpStreamFailed(code);
}
void RtmpHosterImpl::OnStreamClosed()
{
	callback_.OnRtmpStreamClosed();
}
void RtmpHosterImpl::OnStreamStatus(int delayMs, int netBand)
{
	callback_.OnRtmpStreamStatus(delayMs, netBand);
}


//* For rtc::MessageHandler
void RtmpHosterImpl::OnMessage(rtc::Message* msg)
{
	if (msg->message_id == MSG_START_RTMP) {
		av_rtmp_streamer_->StartStream(rtmp_url_);
		if (video_capturer_.get() == NULL) {
			av_rtmp_streamer_->SetVideoEnable(false);
		}
	}
	else if (msg->message_id == MSG_STOP_RTMP) {
		av_rtmp_streamer_->StopStream();
	}
}

//* For webrtc::AVAudioRecordCallback
void RtmpHosterImpl::OnRecordAudio(const void* audioSamples, const size_t nSamples,
	const size_t nBytesPerSample, const size_t nChannels, const uint32_t samplesPerSec, const uint32_t totalDelayMS)
{
	webrtc::AudioSinkInterface::Data audio((int16_t*)audioSamples, nSamples, samplesPerSec, nChannels, totalDelayMS);
	((webrtc::AnyRtmpStreamerImpl*)av_rtmp_streamer_)->GetAudioSink()->OnData(audio);
}


#include "ARRtmpPlayer.h"

static void rtmp_discovery_tc_url(std::string tcUrl, std::string& schema, std::string& host, std::string& app, std::string& port, std::string& stream)
{
	size_t pos = std::string::npos;
	std::string url = tcUrl;

	if ((pos = url.find("://")) != std::string::npos) {
		schema = url.substr(0, pos);
		url = url.substr(schema.length() + 3);
	}

	if ((pos = url.find("/")) != std::string::npos) {
		host = url.substr(0, pos);
		url = url.substr(host.length() + 1);
	}

	port = "1935";
	if ((pos = host.find(":")) != std::string::npos) {
		port = host.substr(pos + 1);
		host = host.substr(0, pos);
	}

	if ((pos = url.find("/")) != std::string::npos) {
		app = url.substr(0, pos);
		url = url.substr(app.length() + 1);
	}
	else {
		app = url;
	}
	stream = url;
}

static int rtmp_flv_demuxer_handler(void* param, int codec, const void* data, size_t bytes, uint32_t pts, uint32_t dts, int flags)
{

	ARRtmpPlayer* rtmpClient = (ARRtmpPlayer*)param;
	if (rtmpClient != NULL) {
		rtmpClient->on_flv_demuxer_data(codec, data, bytes, pts);
	}

	return 0;
}
static int rtmp_client_send(void* param, const void* header, size_t len, const void* payload, size_t bytes)
{
	ARRtmpPlayer* client;
	client = (ARRtmpPlayer*)param;
	return client->do_rtmp_client_send(header, len, payload, bytes);
}

static int rtmp_client_onaudio(void* param, const void* audio, size_t bytes, uint32_t timestamp)
{
	ARRtmpPlayer* client;
	client = (ARRtmpPlayer*)param;
	return client->do_rtmp_client_onaudio(audio, bytes, timestamp);
}

static int rtmp_client_onvideo(void* param, const void* video, size_t bytes, uint32_t timestamp)
{
	ARRtmpPlayer* client;
	client = (ARRtmpPlayer*)param;
	return client->do_rtmp_client_onvideo(video, bytes, timestamp);
}

static int rtmp_client_onscript(void* param, const void* script, size_t bytes, uint32_t timestamp)
{
	ARRtmpPlayer* client;
	client = (ARRtmpPlayer*)param;
	return client->do_rtmp_client_onscript(script, bytes, timestamp);
}

ARPlayer* ARP_CALL createRtmpPlayer(ARPlayerEvent& callback)
{
	return new ARRtmpPlayer(callback);
}
ARRtmpPlayer::ARRtmpPlayer(ARPlayerEvent& callback)
	: ARPlayer(callback)
	, main_thread_(NULL)
	, ar_net_client_(NULL)
	, rtmp_client_(NULL)
	, b_rtmp_connected_(false)
	, flv_demuxer_(NULL)
	, aac_decoder_(NULL)
	, audio_cache_(NULL)
	, a_cache_len_(0)
	, aac_sample_hz_(44100)
	, aac_channels_(2)
	, aac_frame_per10ms_size_(0)
	, h264_decoder_(NULL)
{
	main_thread_ = rtc::Thread::Current();
	audio_cache_ = new uint8_t[10240];
}
ARRtmpPlayer::~ARRtmpPlayer(void)
{
	RTC_CHECK(main_thread_->IsCurrent());

	delete[] audio_cache_;
}

//* For ARPlayer
int ARRtmpPlayer::StartTask(const char* strUrl)
{
	RTC_CHECK(main_thread_->IsCurrent());
	RTC_CHECK(strUrl != NULL && strlen(strUrl) > 0);
	str_rtmp_url_ = strUrl;
	std::string schema, host, app, port, stream;
	rtmp_discovery_tc_url(str_rtmp_url_, schema, host, app, port, stream);
	if (ar_net_client_ == NULL) {
		ar_net_client_ = createArNetTcpClient();
		ar_net_client_->setCallback(this);
		ar_net_client_->connect(host.c_str(), atoi(port.c_str()));
		b_rtmp_connected_ = false;

		struct rtmp_client_handler_t h2;
		h2.send = rtmp_client_send;
		h2.onaudio = rtmp_client_onaudio;
		h2.onvideo = rtmp_client_onvideo;
		h2.onscript = rtmp_client_onscript;
		char tcurl[1024] = { 0 };
		snprintf(tcurl, sizeof(tcurl), "rtmp://%s/%s", host.c_str(), app.c_str()); // tcurl
		rtmp_client_ = rtmp_client_create(app.c_str(), stream.c_str(), tcurl, this, &h2);

		//callback_->onPushStatusUpdate(AR::ArLivePushStatus::ArLivePushStatusConnecting, NULL, NULL);
	}
    return 0;
}
void ARRtmpPlayer::StopTask()
{
	RTC_CHECK(main_thread_->IsCurrent());
	if (rtmp_client_ != NULL) {
		//rtmp_client_stop(rtmp_client_);
		rtmp_client_destroy(rtmp_client_);
		rtmp_client_ = NULL;
	}
	if (ar_net_client_ != NULL) {
		ar_net_client_->disconnect();
		delete ar_net_client_;
		ar_net_client_ = NULL;
	}

	if (flv_demuxer_ != NULL) {
		flv_demuxer_destroy(flv_demuxer_);
		flv_demuxer_ = NULL;
	}

	if (aac_decoder_) {
		aac_decoder_close(aac_decoder_);
		aac_decoder_ = NULL;
	}

	if (h264_decoder_ != NULL) {
		delete h264_decoder_;
		h264_decoder_ = NULL;
	}
}
void ARRtmpPlayer::RunOnce()
{
	RTC_CHECK(main_thread_->IsCurrent());

	if (ar_net_client_ != NULL) {
		ar_net_client_->runOnce();
	}
	JitBuffer::GetData();
}
void ARRtmpPlayer::Play()
{

}
void ARRtmpPlayer::Pause()
{

}
void ARRtmpPlayer::SetAudioEnable(bool bAudioEnable)
{

}
void ARRtmpPlayer::SetVideoEnable(bool bVideoEnable)
{

}
void ARRtmpPlayer::SetRepeat(bool bEnable)
{

}
void ARRtmpPlayer::SetUseTcp(bool bUseTcp)
{//* Not implement
}
void ARRtmpPlayer::SetNoBuffer(bool bNoBuffer)
{

}
void ARRtmpPlayer::SetRepeatCount(int loopCount)
{

}
void ARRtmpPlayer::SeekTo(int nSeconds)
{//* Not implement
}
void ARRtmpPlayer::SetSpeed(float fSpeed)
{//* Not implement
}
void ARRtmpPlayer::SetVolume(float fVolume)
{

}
void ARRtmpPlayer::EnableVolumeEvaluation(int32_t intervalMs)
{

}
int ARRtmpPlayer::GetTotalDuration()
{//* Not implement
	return 0;
}
void ARRtmpPlayer::RePlay()
{

}
void ARRtmpPlayer::Config(bool bAuto, int nCacheTime, int nMinCacheTime, int nMaxCacheTime, int nVideoBlockThreshold)
{
	JitBuffer::SetPlaySetting(bAuto, nCacheTime, nMinCacheTime, nMaxCacheTime, nVideoBlockThreshold);
}
void ARRtmpPlayer::selectAudioTrack(int index)
{

}
int ARRtmpPlayer::getAudioTrackCount()
{
	return 1;
}

//* For PlayerBuffer
void ARRtmpPlayer::OnBufferDecodeVideoData(const uint8_t* pData, int len, bool bKeyrame, uint32_t ts)
{
	if (h264_decoder_ == NULL) {
		h264_decoder_ = new webrtc::V_H264Decoder(*this);
	}

	if (h264_decoder_ != NULL) {
		h264_decoder_->SetVideoData(bKeyrame, (char*)pData, len);
	}
}
void ARRtmpPlayer::OnBufferDecodeAudioData(const uint8_t* pData, int len, int seqn, uint32_t ts)
{
	if (aac_decoder_ == NULL) {
		aac_decoder_ = aac_decoder_open((unsigned char*)pData, len, &aac_channels_, &aac_sample_hz_);
		if (aac_channels_ == 0)
			aac_channels_ = 1;
		aac_frame_per10ms_size_ = (aac_sample_hz_ / 100) * sizeof(int16_t) * aac_channels_;
	}
	else {
		unsigned int outlen = 0;
		int pts = ts;
		pts -= (a_cache_len_ * 10) / aac_frame_per10ms_size_;
		if (aac_decoder_decode_frame(aac_decoder_, (unsigned char*)pData, len, audio_cache_ + a_cache_len_, &outlen) > 0) {
			a_cache_len_ += outlen;
			int ct = 0;
			int fsize = aac_frame_per10ms_size_;	
			while (a_cache_len_ > fsize) {
				//ply_buffer_->CachePcmData(audio_cache_ + ct * fsize, fsize, ts);
				callback_.OnArPlyAudio(this, (char*)audio_cache_ + ct * fsize, aac_sample_hz_, aac_channels_, pts);
				pts += 10;
				a_cache_len_ -= fsize;
				ct++;
			}

			memmove(audio_cache_, audio_cache_ + ct * fsize, a_cache_len_);
		}
	}
}
void ARRtmpPlayer::OnBufferStatusChanged(PlayStatus playStatus)
{

}
bool ARRtmpPlayer::OnBufferGetPuased()
{
	return false;
}


//* For ArNetClientEvent
void ARRtmpPlayer::OnArClientConnected()
{
	rtmp_client_start(rtmp_client_, 1);
	if (flv_demuxer_ == NULL) {
		flv_demuxer_ = flv_demuxer_create(rtmp_flv_demuxer_handler, this);
	}
}
void ARRtmpPlayer::OnArClientConnectFailure()
{

}
void ARRtmpPlayer::OnArClientDisconnect()
{
	rtmp_client_stop(rtmp_client_);
	if (flv_demuxer_ != NULL) {
		flv_demuxer_destroy(flv_demuxer_);
		flv_demuxer_ = NULL;
	}
}
void ARRtmpPlayer::OnArClientSent(int err)
{

}
void ARRtmpPlayer::OnArClientRecv(const char* pData, int nLen)
{
	if (rtmp_client_ != NULL) {
		int ret = rtmp_client_input(rtmp_client_, pData, nLen);
		if (ret != 0) {
			//callback_->onError(0, NULL, NULL);
		}

		if (!b_rtmp_connected_ && 4/*RTMP_STATE_START*/ == rtmp_client_getstate(rtmp_client_)) {
			b_rtmp_connected_ = true;

			//* Rtmp建立连接之后，才回调发布成功
			//callback_->onPushStatusUpdate(AR::ArLivePushStatus::ArLivePushStatusConnectSuccess, NULL, NULL);
		}
	}
}

//* For webrtc::RtcVidDecoderEvent
void ARRtmpPlayer::OnDecodeFrame(const char* strIdd, const char* yData, const char* uData, const char* vData, int strideY, int strideU, int strideV, int w, int h, int rotate, unsigned int timeStamp)
{
	uint8_t* pData[3];
	int linesize[3];
	pData[0] = (uint8_t*)yData;
	pData[1] = (uint8_t*)uData;
	pData[2] = (uint8_t*)vData;
	linesize[0] = strideY;
	linesize[1] = strideU;
	linesize[2] = strideV;
	callback_.OnArPlyVideo(this, 0, w, h, pData, linesize, timeStamp);
}

//* For Rtmp
void ARRtmpPlayer::on_flv_demuxer_data(int type, const void* data, size_t bytes, uint32_t timestamp)
{
	switch (type)
	{
	case FLV_AUDIO_AAC:
	{
		//printf("OnFlvDeMuxerData aac: %d ts: %u \r\n", bytes, timestamp);
		JitBuffer::CacheAudData((uint8_t*)data, bytes, timestamp, 0);
	}
	break;
	case FLV_VIDEO_H264:
	{
		//printf("OnFlvDeMuxerData h264: %d ts: %u \r\n", bytes, timestamp);
		int nType = ((char*)data)[4] & 0x1f;
		bool bKeyFrame = NaluIsKeyFrame((unsigned char*)data, bytes);
		JitBuffer::CacheVidData((uint8_t*)data, bytes, timestamp, bKeyFrame);
	}
	break;
	case FLV_TYPE_SCRIPT:
	{

	}
	break;
	default:
		break;
	}
}
int ARRtmpPlayer::do_rtmp_client_send(const void* header, size_t len, const void* payload, size_t bytes)
{
	if (ar_net_client_ != NULL) {
		ar_net_client_->sendData((char*)header, len);
		ar_net_client_->sendData((char*)payload, (int)bytes);
	}
	return len + bytes;
}
int ARRtmpPlayer::do_rtmp_client_onaudio(const void* audio, size_t bytes, uint32_t timestamp) 
{ 
	if (flv_demuxer_ != NULL) {
		flv_demuxer_input(flv_demuxer_, FLV_TYPE_AUDIO, audio, bytes, timestamp);
	}
	return 0; 
};
int ARRtmpPlayer::do_rtmp_client_onvideo(const void* video, size_t bytes, uint32_t timestamp) 
{
	if (flv_demuxer_ != NULL) {
		flv_demuxer_input(flv_demuxer_, FLV_TYPE_VIDEO, video, bytes, timestamp);
	}
	return 0;
};
int ARRtmpPlayer::do_rtmp_client_onscript(const void* script, size_t bytes, uint32_t timestamp) 
{
	if (flv_demuxer_ != NULL) {
		//flv_demuxer_script(flv_demuxer_, script, bytes);
		flv_demuxer_input(flv_demuxer_, FLV_TYPE_SCRIPT, script, bytes, timestamp);
	}
	return 0;
};

bool ARRtmpPlayer::NaluIsKeyFrame(const unsigned char* pData, int nLen)
{
	int limLen = 4;
	unsigned char* ptr = (unsigned char*)pData;
	if (ptr[0] == 0x00 && ptr[1] == 0x00 && ptr[2] == 0x01)
	{
		limLen = 3;
	}
	int len = nLen;
	int nType = (ptr[limLen] & 0x1f);
	if (nType == 7 || nType == 5) {
		return true;
	}
	else if (nType == 1) {
		return false;
	}
	else {

		ptr += limLen;
		len -= limLen;
		while (len > limLen) {
			if (ptr[0] == 0x00 && ptr[1] == 0x00 && ptr[2] == 0x00 && ptr[3] == 0x01) {
				return NaluIsKeyFrame(ptr, len);
				break;
			}
			else if (ptr[0] == 0x00 && ptr[1] == 0x00 && ptr[2] == 0x01) {
				return NaluIsKeyFrame(ptr, len);
				break;
			}
			else {
				ptr++;
				len--;
			}
		}
	}

	return false;
}

#include "ARRtmpPusher.h"
#include "rtc_base/time_utils.h"

ARPusher* V2_CALL createARPusher()
{
	return new ARRtmpPusher();
}
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
static int rtmp_flv_muxer_handler(void* param, int type, const void* data, size_t bytes, uint32_t timestamp)
{
	ARRtmpPusher* rtmpClient = (ARRtmpPusher*)(param);
	if (rtmpClient != NULL) {
		rtmpClient->on_flv_muxer_data(type, data, bytes, timestamp);
	}

	return 0;
}
static int rtmp_client_send(void* param, const void* header, size_t len, const void* payload, size_t bytes)
{
	ARRtmpPusher* client;
	client = (ARRtmpPusher*)param;
	return client->do_rtmp_client_send(header, len, payload, bytes);
}

static int rtmp_client_onaudio(void* param, const void* audio, size_t bytes, uint32_t timestamp)
{
	ARRtmpPusher* client;
	client = (ARRtmpPusher*)param;
	return client->do_rtmp_client_onaudio(audio, bytes, timestamp);
}

static int rtmp_client_onvideo(void* param, const void* video, size_t bytes, uint32_t timestamp)
{
	ARRtmpPusher* client;
	client = (ARRtmpPusher*)param;
	return client->do_rtmp_client_onvideo(video, bytes, timestamp);
}

static int rtmp_client_onscript(void* param, const void* script, size_t bytes, uint32_t timestamp)
{
	ARRtmpPusher* client;
	client = (ARRtmpPusher*)param;
	return client->do_rtmp_client_onscript(script, bytes, timestamp);
}

ARRtmpPusher::ARRtmpPusher()
	: ARPusher()
	, main_thread_(NULL)
	, ar_net_client_(NULL)
	, rtmp_client_(NULL)
	, b_rtmp_connected_(false)
	, b_vid_need_keyframe_(true)
	, n_flv_start_time_(0)
	, flv_muxer_(NULL)
	, n_set_retry_count_(5)
	, n_set_retry_delay_(3000)
	, n_retry_count_(0)
	, n_retry_time_(0)
{
	main_thread_ = rtc::Thread::Current();
}
ARRtmpPusher::~ARRtmpPusher(void)
{
	RTC_CHECK(main_thread_->IsCurrent());
}

//* For ARPusher
int ARRtmpPusher::startTask(const char* strUrl)
{
	RTC_CHECK(callback_ != NULL);
	RTC_CHECK(main_thread_->IsCurrent());
	RTC_CHECK(strUrl != NULL && strlen(strUrl) > 0);
	str_rtmp_url_ = strUrl;
	std::string schema, host, app, port, stream;
	rtmp_discovery_tc_url(str_rtmp_url_, schema, host, app, port, stream);
    
    if (strlen(host.c_str()) == 0) {
        callback_->onError(AR::ArLIVE_ERROR_INVALID_PARAMETER, NULL, NULL);
        return 0;
    }
	if (ar_net_client_ == NULL) {
		ar_net_client_ = createArNetTcpClient();
		ar_net_client_->setCallback(this);
		ar_net_client_->connect(host.c_str(), atoi(port.c_str()));
		b_rtmp_connected_ = false;

		n_retry_count_ = 0;
		n_retry_time_ = 0;

		

		callback_->onPushStatusUpdate(AR::ArLivePushStatus::ArLivePushStatusConnecting, NULL, NULL);
	}

	return 0;
}
int ARRtmpPusher::stopTask()
{
	RTC_CHECK(main_thread_->IsCurrent());
	if (rtmp_client_ != NULL) {
		rtmp_client_stop(rtmp_client_);
		rtmp_client_destroy(rtmp_client_);
		rtmp_client_ = NULL;
	}
	if (ar_net_client_ != NULL) {
		ar_net_client_->disconnect();
		delete ar_net_client_;
		ar_net_client_ = NULL;
	}
	if (flv_muxer_ != NULL) {
		flv_muxer_destroy(flv_muxer_);
		flv_muxer_ = NULL;
	}

	{
		webrtc::MutexLock l(&cs_send_data_);
		lst_send_data_.clear();
	}

	return 0;
}

int ARRtmpPusher::runOnce()
{
	RTC_CHECK(main_thread_->IsCurrent());
	{
		webrtc::MutexLock l(&cs_send_data_);
		while (lst_send_data_.size() > 0) {
			std::unique_ptr<FlvData>& flvData = lst_send_data_.front();
			int type = flvData->nType;
			const void* data = flvData->pData;
			size_t bytes = flvData->nLen;
			uint32_t timestamp = flvData->nTimestamp;

			if (rtmp_client_ != NULL) {
				if (type == FLV_TYPE_AUDIO) {
					rtmp_client_push_audio(rtmp_client_, data, bytes, timestamp);
				}
				else if (type == FLV_TYPE_VIDEO) {
					rtmp_client_push_video(rtmp_client_, data, bytes, timestamp);
				}
				else if (type == FLV_TYPE_SCRIPT) {
					rtmp_client_push_script(rtmp_client_, data, bytes, timestamp);
				}
			}
			lst_send_data_.pop_front();
		}
	}
	if (ar_net_client_ != NULL) {
		ar_net_client_->runOnce();
	}

	if (n_retry_time_ != 0) {
		if (n_retry_time_ <= rtc::TimeUTCMillis()) {
			n_retry_time_ = 0;

			if (ar_net_client_ != NULL) {
				ar_net_client_->disconnect();
				delete ar_net_client_;
				ar_net_client_ = NULL;
			}

			std::string schema, host, app, port, stream;
			rtmp_discovery_tc_url(str_rtmp_url_, schema, host, app, port, stream);
			if (ar_net_client_ == NULL) {
				ar_net_client_ = createArNetTcpClient();
				ar_net_client_->setCallback(this);
				ar_net_client_->connect(host.c_str(), atoi(port.c_str()));
				b_rtmp_connected_ = false;
			}

			callback_->onPushStatusUpdate(AR::ArLivePushStatus::ArLivePushStatusReconnecting, NULL, NULL);
		}
	}
	
	return 0;
}
int ARRtmpPusher::setRepeat(bool bEnable)
{
	return 0;
}
int ARRtmpPusher::setRetryCountDelay(int nCount, int nDelay)
{
	n_set_retry_count_ = nCount;
	n_set_retry_delay_ = nDelay;
	return 0;
}
int ARRtmpPusher::setAudioData(const char* pData, int nLen, uint32_t ts)
{
	if (b_rtmp_connected_) {
		uint32_t rts = ts - n_flv_start_time_;
		int ret = flv_muxer_aac(flv_muxer_, pData, nLen, rts, rts);
	}
	else {
		return -1;
	}
	return 0;
}
int ARRtmpPusher::setVideoData(const char* pData, int nLen, bool bKeyFrame, uint32_t ts)
{
	if (b_rtmp_connected_) {
		if (b_vid_need_keyframe_) {
			if (bKeyFrame) {
				b_vid_need_keyframe_ = false;
			}
			else {
				return -1;
			}
		}
		uint32_t rts = ts - n_flv_start_time_;
		int ret = flv_muxer_avc(flv_muxer_, pData, nLen, rts, rts);
		if (ret != 0) {
			//printf("flv_muxer_avc err: %d \r\n", ret);
		}
	}
	else {
		return -1;
	}
	return 0;
}
int ARRtmpPusher::setSeiData(const char* pData, int nLen, uint32_t ts)
{
	if (b_rtmp_connected_) {
		uint32_t rts = ts - n_flv_start_time_;
		//int ret = flv_muxer_metadata(flv_muxer_, pData, nLen, rts, rts);
	}
	else {
		return -1;
	}
	return 0;
}

//* For ArNetClientEvent
void ARRtmpPusher::OnArClientConnected()
{
	if (rtmp_client_ == NULL) {
		std::string schema, host, app, port, stream;
		rtmp_discovery_tc_url(str_rtmp_url_, schema, host, app, port, stream);

		struct rtmp_client_handler_t h2;
		h2.send = rtmp_client_send;
		h2.onaudio = rtmp_client_onaudio;
		h2.onvideo = rtmp_client_onvideo;
		h2.onscript = rtmp_client_onscript;
		char tcurl[1024] = { 0 };
		snprintf(tcurl, sizeof(tcurl), "rtmp://%s/%s", host.c_str(), app.c_str()); // tcurl
		rtmp_client_ = rtmp_client_create(app.c_str(), stream.c_str(), tcurl, this, &h2);
		rtmp_client_start(rtmp_client_, 0);
	}
	
	n_retry_count_ = 0;
}
void ARRtmpPusher::OnArClientConnectFailure()
{
	if (n_retry_count_++ < n_set_retry_count_) {
		n_retry_time_ = rtc::TimeUTCMillis() + n_set_retry_delay_;
	}
	else {
		callback_->onError(AR::ArLIVE_ERROR_REQUEST_TIMEOUT, NULL, NULL);
	}
}
void ARRtmpPusher::OnArClientDisconnect()
{
	b_rtmp_connected_ = false;
	if (rtmp_client_ != NULL) {
		//rtmp_client_stop(rtmp_client_);
		rtmp_client_destroy(rtmp_client_);
		rtmp_client_ = NULL;
	}
	callback_->onPushStatusUpdate(AR::ArLivePushStatus::ArLivePushStatusDisconnected, NULL, NULL);

	n_retry_time_ = rtc::TimeUTCMillis() + n_set_retry_delay_;
}

void ARRtmpPusher::OnArClientSent(int err)
{

}
void ARRtmpPusher::OnArClientRecv(const char* pData, int nLen)
{
	if (rtmp_client_ != NULL) {
		int ret = rtmp_client_input(rtmp_client_, pData, nLen);
		if (ret != 0) {
			callback_->onError(0, NULL, NULL);
		}

		if (!b_rtmp_connected_ && 4/*RTMP_STATE_START*/ == rtmp_client_getstate(rtmp_client_)) {
			if (flv_muxer_ == NULL) {
				flv_muxer_ = flv_muxer_create(rtmp_flv_muxer_handler, this);
			}
			b_vid_need_keyframe_ = true;
			n_flv_start_time_ = rtc::Time32();
			b_rtmp_connected_ = true;

			//* Rtmp建立连接之后，才回调发布成功
			callback_->onPushStatusUpdate(AR::ArLivePushStatus::ArLivePushStatusConnectSuccess, NULL, NULL);
		}
	}
}

void ARRtmpPusher::on_flv_muxer_data(int type, const void* data, size_t bytes, uint32_t timestamp)
{
	std::unique_ptr<  FlvData> flvData = std::make_unique< FlvData>();
	flvData->SetData((char*)data, bytes);
	flvData->nType = type;
	flvData->nTimestamp = timestamp;

	webrtc::MutexLock l(&cs_send_data_);
	lst_send_data_.push_back(std::move(flvData));
}

int ARRtmpPusher::do_rtmp_client_send(const void* header, size_t len, const void* payload, size_t bytes)
{
	if (ar_net_client_ != NULL) {
		ar_net_client_->sendData((char*)header, len);
		ar_net_client_->sendData((char*)payload, (int)bytes);
	}
	return len + bytes;
}

#include "ArNetClient.h"
#include "rtc_base/logging.h"

const uint16_t kDefaultServerPort = 8080;

ArNetClient::ArNetClient()
	: main_thread_(NULL)
	, resolver_(NULL)
    , state_(NOT_CONNECTED)
{
    main_thread_ = rtc::Thread::Current();
}
ArNetClient::~ArNetClient(void)
{
    RTC_DCHECK(state_ == NOT_CONNECTED);
}

void ArNetClient::setCallback(INetClientEvent* pCallback)
{
    callback_ = pCallback;
}
void ArNetClient::connect(const char* server, int port)
{
    RTC_DCHECK(main_thread_->IsCurrent());
    RTC_DCHECK(callback_ != NULL);
    RTC_DCHECK(server != NULL && strlen(server) > 0);

    if (state_ != NOT_CONNECTED) {
        RTC_LOG(WARNING)
            << "The client must not be connected before you can call Connect()";
        callback_->OnArClientConnectFailure();
        return;
    }

    if (port <= 0)
        port = kDefaultServerPort;

    server_address_.SetIP(server);
    server_address_.SetPort(port);
    str_server_addr_ = server;

    if (server_address_.IsUnresolvedIP()) {
        state_ = RESOLVING;
        resolver_ = new rtc::AsyncResolver();
        resolver_->SignalDone.connect(this, &ArNetClient::OnResolveResult);
        resolver_->Start(server_address_);
    }
    else {
        doConnect();
    }
}
void ArNetClient::disconnect()
{
    RTC_DCHECK(main_thread_->IsCurrent());
	if (resolver_ != NULL) {
		resolver_->Destroy(false);
		resolver_ = NULL;
	}
    doDisconnect();
    {
        webrtc::MutexLock l(&cs_send_data_);
        lst_send_data_.clear();
    }
}
void ArNetClient::runOnce()
{
    RTC_DCHECK(main_thread_->IsCurrent());

    if (state_ == CONNECTED) {
        webrtc::MutexLock l(&cs_send_data_);
        while (lst_send_data_.size() > 0) {
            std::unique_ptr< ArNetData>& sendData = lst_send_data_.front();
          
            doSendData(sendData->pData, sendData->nLen);

            lst_send_data_.pop_front();
        }
    }
}
void ArNetClient::sendData(const char* pData, int nLen)
{
   // RTC_DCHECK(state_ == CONNECTED);
    if (pData != NULL && nLen > 0)
    {
        if (main_thread_->IsCurrent()) {
            doSendData(pData, nLen);
        }
        else {
            std::unique_ptr< ArNetData> sendData = std::make_unique<ArNetData>(pData, nLen);
            webrtc::MutexLock l(&cs_send_data_);
            lst_send_data_.push_back(std::move(sendData));
        }
       
    }
}

void ArNetClient::OnResolveResult(rtc::AsyncResolverInterface* resolver)
{
    if (resolver_->GetError() != 0) {
        callback_->OnArClientConnectFailure();
        resolver_->Destroy(false);
        resolver_ = NULL;
        state_ = NOT_CONNECTED;
    }
    else {
        //* 支持IPv6，否则iOS上架可能无法通过
        if (resolver_->GetResolvedAddress(AF_INET6, &server_address_)) {
        }
        else if (resolver_->GetResolvedAddress(AF_INET, &server_address_)) {
        }
        else {
            state_ = NOT_CONNECTED;
            callback_->OnArClientConnectFailure();
        }
        if (state_ != NOT_CONNECTED)
        {
            state_ = CONNECTTING;

            doConnect();
        }
    }
}


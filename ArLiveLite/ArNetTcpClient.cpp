#include "ArNetTcpClient.h"
#include "rtc_base/checks.h"
#include "rtc_base/logging.h"
#include "rtc_base/net_helpers.h"
#ifdef WIN32
#include "rtc_base/win32_socket_server.h"
#endif

namespace {

	// This is our magical hangup signal.
	const char kByeMessage[] = "BYE";
	// Delay between server connection retries, in milliseconds
	const int kReconnectDelay = 2000;

	rtc::AsyncSocket* CreateClientSocket(int family) {
#ifdef WIN32
		rtc::Win32Socket* sock = new rtc::Win32Socket();
		sock->CreateT(family, SOCK_STREAM);
		return sock;
#elif defined(WEBRTC_POSIX)
		rtc::Thread* thread = rtc::Thread::Current();
		RTC_DCHECK(thread != NULL);
		return thread->socketserver()->CreateAsyncSocket(family, SOCK_STREAM);
#else
#error Platform not supported.
#endif
	}

}  // namespace

INetClient* createArNetTcpClient()
{
	return new ArNetTcpClient();
}

ArNetTcpClient::ArNetTcpClient()
{

}
ArNetTcpClient::~ArNetTcpClient(void)
{

}

//* For ArNetClient
void ArNetTcpClient::doConnect()
{
	control_socket_.reset(CreateClientSocket(server_address_.ipaddr().family()));
	InitSocketSignals();

	bool ret = ConnectControlSocket();
	if (ret)
	{
		state_ = CONNECTTING;
	}
	if (!ret) {
		callback_->OnArClientConnectFailure();
	}
}
void ArNetTcpClient::doDisconnect()
{
	if (control_socket_ != NULL) {
		control_socket_->Close();
	}
	state_ = NOT_CONNECTED;
}
void ArNetTcpClient::doRunOnce()
{
	if (state_ == CONNECTED) {
	}
}
void ArNetTcpClient::doSendData(const char* pData, int nLen)
{
	if (control_socket_ != NULL) {
		control_socket_->Send(pData, nLen);
	}
}

void ArNetTcpClient::InitSocketSignals()
{
	RTC_DCHECK(control_socket_.get() != NULL);
	control_socket_->SignalCloseEvent.connect(this,
		&ArNetTcpClient::OnClose);
	control_socket_->SignalConnectEvent.connect(this,
		&ArNetTcpClient::OnConnect);
	control_socket_->SignalReadEvent.connect(this, &ArNetTcpClient::OnRead);
}
bool ArNetTcpClient::ConnectControlSocket()
{
	RTC_DCHECK(control_socket_->GetState() == rtc::Socket::CS_CLOSED);
	int err = control_socket_->Connect(server_address_);
	if (err == SOCKET_ERROR) {
		doDisconnect();
		return false;
	}
	return true;
}
void ArNetTcpClient::OnConnect(rtc::AsyncSocket* socket)
{
	state_ = CONNECTED;
	callback_->OnArClientConnected();
}

void ArNetTcpClient::OnRead(rtc::AsyncSocket* socket)
{
	char buffer[0xffff];
	do {
		int bytes = socket->Recv(buffer, sizeof(buffer), nullptr);
		if (bytes <= 0)
			break;
		callback_->OnArClientRecv(buffer, bytes);
	} while (true);
}

void ArNetTcpClient::OnClose(rtc::AsyncSocket* socket, int err)
{
	RTC_LOG(INFO) << __FUNCTION__;

	if (state_ == CONNECTED) {
		callback_->OnArClientDisconnect();
	}
	else {
		callback_->OnArClientConnectFailure();
	}
}

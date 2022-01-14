#ifndef __I_NET_CLIENT_H__
#define __I_NET_CLIENT_H__
#include <stdio.h>

typedef enum {
	NOT_CONNECTED = 0,
	RESOLVING,
	CONNECTTING,
	CONNECTED
}INetState;

class INetClientEvent {
public:
	virtual void OnArClientConnected() = 0;
	virtual void OnArClientConnectFailure() = 0;
	virtual void OnArClientDisconnect() = 0;

	virtual void OnArClientSent(int err) = 0;
	virtual void OnArClientRecv(const char* pData, int nLen) = 0;

protected:
	virtual ~INetClientEvent() {}
};

class INetClient 
{
public:
	INetClient(): callback_(NULL){};
	virtual ~INetClient(void) {};

	virtual void setCallback(INetClientEvent* pCallback) = 0;
	virtual void connect(const char* server, int port) = 0;
	virtual void disconnect() = 0;
	virtual void runOnce() = 0;
	virtual void sendData(const char* pData, int nLen) = 0;

protected:
	INetClientEvent* callback_;
};


//* Create a tcp client
INetClient* createArNetTcpClient();

//* Create a quic client
//INetClient* createArNetQuicClient();

#endif	// __I_NET_CLIENT_H__

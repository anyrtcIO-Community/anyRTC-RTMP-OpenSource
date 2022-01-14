/*
*  Copyright (c) 2021 The AnyRTC project authors. All Rights Reserved.
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
#ifndef __AR_NET_CLIENT_H__
#define __AR_NET_CLIENT_H__
#include "INetClient.h"
#include <string>
#include "rtc_base/thread.h"
#include "rtc_base/async_resolver.h"
#include "rtc_base/synchronization/mutex.h"



struct ArNetData {
	ArNetData(const char* data, int len): pData(NULL), nLen(0) {
		if (data != NULL && len > 0) 
		{
			nLen = len;
			pData = new char[nLen];
			memcpy(pData, data, nLen);
		}
	}
	virtual ~ArNetData(void) {
		delete[] pData;
	}
	char* pData;
	int nLen;
};



class ArNetClient : public INetClient, public sigslot::has_slots<>
{
public:
	ArNetClient();
	virtual ~ArNetClient(void);

	virtual void setCallback(INetClientEvent* pCallback);
	virtual void connect(const char* server, int port) ;
	virtual void disconnect() ;
	virtual void runOnce() ;
	virtual void sendData(const char* pData, int nLen);

protected:
	virtual void doConnect() = 0;
	virtual void doDisconnect() = 0;
	virtual void doRunOnce() = 0;
	virtual void doSendData(const char* pData, int nLen) = 0;
	void OnResolveResult(rtc::AsyncResolverInterface* resolver);

protected:
	rtc::Thread*		main_thread_;

	std::string			str_server_addr_;
	rtc::SocketAddress	server_address_;

private:
	rtc::AsyncResolver* resolver_;


	webrtc::Mutex cs_send_data_;
	std::list<std::unique_ptr< ArNetData>> lst_send_data_;

protected:
	INetState		state_;
	
};



#endif	// __AR_NET_CLIENT_H__
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
#ifndef __AR_NET_TCP_CLIENT_H__
#define __AR_NET_TCP_CLIENT_H__
#include "ArNetClient.h"
#include "rtc_base/buffer.h"

class ArNetTcpClient :
    public ArNetClient
{
public:
    ArNetTcpClient();
    virtual ~ArNetTcpClient(void);

    //* For ArNetClient
    virtual void doConnect();
    virtual void doDisconnect();
    virtual void doRunOnce();
    virtual void doSendData(const char* pData, int nLen);

protected:
    void InitSocketSignals();
    bool ConnectControlSocket();
    void OnConnect(rtc::AsyncSocket* socket);
    void OnRead(rtc::AsyncSocket* socket);
    void OnWrite(rtc::AsyncSocket* socket);
    void OnClose(rtc::AsyncSocket* socket, int err);

private:
    std::unique_ptr<rtc::AsyncSocket> control_socket_;

    rtc::Buffer				outbuf_;
    size_t max_outsize_;
};

#endif  // __AR_NET_TCP_CLIENT_H__


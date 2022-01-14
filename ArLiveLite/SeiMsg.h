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
#ifndef __SEI_MSG_H__
#define __SEI_MSG_H__
#include "H264SeiPack.h"

struct SeiMsg
{
	SeiMsg(): ePayloadType(SEI_USER_DATA_UNREGISTERED), pMsg(NULL), nLen(0) {

	}
	virtual ~SeiMsg() {
		if (pMsg != NULL) {
			delete[] pMsg;
			pMsg = NULL;
		}
	}

	void SetMsg(const char* data, int len)
	{
		if (pMsg != NULL) {
			delete[] pMsg;
			pMsg = NULL;
		}
		nLen = len;
		pMsg = new char[nLen];
		memcpy(pMsg, data, nLen);
	}

	sei_payload_type_e ePayloadType;

	char* pMsg;
	int nLen;
};

#endif	// __SEI_MSG_H__

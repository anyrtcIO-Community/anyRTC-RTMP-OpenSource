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
#ifndef __I_AR_LIVE_PUSH_Kit_H__
#define __I_AR_LIVE_PUSH_Kit_H__
#include "ArLiveBase.h"

namespace ar {
namespace live {

class IArLivePushEvent
{
public:
	IArLivePushEvent(void) {};
	virtual ~IArLivePushEvent(void) {};

	virtual void onPushEvent(int nEvent, const ArParams&param, uint32_t nElapse) {
		(void)nEvent;
		(void)param;
		(void)nElapse;
	};
	virtual void onNetStatus(const ArParams& status) {
		(void)status;
	};
};

class IArBGMEvent
{
	IArBGMEvent(void) {};
	virtual ~IArBGMEvent(void) {};

	/* BGM文件的时长信息
	参数：
		duration	int	当前 BGM 总时间（ms）。
	*/
	virtual void onBGMInfo(int duration) {};

	/* BGM开始播放
	*/
	virtual void onBGMStart() {};
	/* BGM播放进度
	参数：
		progress	int	当前 BGM 已播放时间（ms）。
		duration	int	当前 BGM 总时间（ms）。
	*/
	virtual void onBGMProgress(int progress, int duration) {};

	/* BGM播放停止
	参数：
		err	int	0：正常结束；-1：出错结束。
	*/
	virtual void onBGMComplete(int err) {};
};

class IArLivePushKit
{
public:
	IArLivePushKit(void) {};
	virtual ~IArLivePushKit(void) {};
	/* 设置推流事件回调
	参数：
		pEvent	IArLivePushEvent*	回调推流事件接收对象
	*/
	virtual void setArLivePushEvent(IArLivePushEvent*pEvent) {
	};
	//**************************************************************************\\
	// 推流控制
	/* 开始推流
	参数：
		strPushUrl	char*	推流地址
	返回：
		0：调用成功		<0: 失败
	*/
	virtual int startPush(const char* strPushUrl) = 0;
	/* 停止推流
	返回：
		0：调用成功		<0: 失败
	*/
	virtual int stopPush() = 0;
	
	
	/* 暂停摄像头或屏幕采集并进入垫片推流状态。
	功能：
		暂时停止摄像头或屏幕采集，并使用 ArLivePushConfig.pauseImg 中指定的图片作为替代图像进行推流，也就是所谓的“垫片”。 
		这项功能常用于App被切到后台运行的场景，尤其是在 iOS 系统中，当 App 切到后台以后，操作系统不会再允许该 App 继续使用摄像头。
		此时就可以通过调用 pausePush() 进入垫片状态。
		对于绝大多数推流服务器而言，如果超过一定时间不推视频数据，服务器会断开当前的推流链接。
	返回：
		0：调用成功		<0: 失败
	*/
	virtual int pausePush() = 0;
	/* 恢复摄像头采集并结束垫片推流状态。
	返回：
		0：调用成功		<0: 失败
	*/
	virtual int resumePush() = 0;
	/* 查询是否正在推流。
	返回：
		true：正在推流，false：未推流。
	*/
	virtual bool isPushing() = 0;

	

	//* For BGM
	/* 设置推流事件回调
	参数：
		pEvent	IArBGMEvent*	回调BGM事件接收对象
	*/
	virtual void setArBGMEvent(IArBGMEvent*pEvent) = 0;
	/*播放背景音乐。
	功能：
		会将背景音乐和麦克风采集的声音进行混合并一起推送到云端。
	返回：
		true：播放成功；false：播放失败。
	*/
	virtual bool playBGM(const char* path, int repeat) = 0;
	/* 停止播放背景音乐。
	返回：
		true：停止播放成功； false：停止播放失败。
	*/
	virtual bool stopBGM() = 0;
	/* 暂停播放背景音乐。
	返回：
		true：停止播放成功； false：停止播放失败。
	*/
	virtual bool pauseBGM() = 0;
	/* 继续播放背音乐。
	返回：
		true：停止播放成功； false：停止播放失败。
	*/
	virtual int resumeBGM() = 0;
	/*设置混音时背景音乐的音量大小，仅在播放背景音乐混音时使用。
	参数：
		nVolume	int	音量大小，100为正常音量，范围是：[0 ~ 400] 之间的整数。
	返回：
		0：调用成功		<0: 失败
	*/
	virtual int adjustBGMVolume(int nVolume) = 0;
};

}
}

#endif	// __I_AR_LIVE_PUSH_Kit_H__

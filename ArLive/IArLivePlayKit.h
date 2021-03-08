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
#ifndef __I_AR_LIVE_PLAY_KIT_H__
#define __I_AR_LIVE_PLAY_KIT_H__
#include "ArLiveBase.h"

namespace ar {
namespace live {

struct ArLivePlayConfig
{
	/* 默认值：true。
	true：启用自动调整， SDK 将根据网络状况在一个范围内调整缓存时间；通过 setMaxAutoAdjustCacheTime 和 setMinAutoAdjustCacheTime 两个接口来进行设置。
	false：关闭自动调整， SDK 将使用固定缓存时长；通过 setCacheTime(float) 来进行设置。
	*/
	bool bAuto;	//设置是否自动调整缓存时间。

	/*
	设置播放器缓存时间，单位为秒，默认值为5秒。
	不建议设置过大，会影响秒开以及直播流播放的实时性。
	*/
	int nCacheTime;	//设置播放器缓存时间。

	
	/*
	默认值：1，单位为秒。
	仅在启用自动调用缓存时间接口时，有效。
	*/
	int nMinCacheTime;	//设置最大的缓存时间。

	/*
	默认值：5，单位为秒。
	仅在启用自动调用缓存时间接口时，有效。
	*/
	int nMaxCacheTime;	//设置最小的缓存时间。

	/*
	默认值：800，单位为毫秒。
	当渲染间隔超过此阈值时候，表明产生了卡顿；播放器会通过 IArLivePlayListener#onPlayEvent(int， Event) 回调 PLAY_WARNING_VIDEO_PLAY_LAG 事件通知。
	*/
	int nVideoBlockThreshold;	//设置播放器视频卡顿报警阈值。

	/*
	默认值：3；取值范围：1 - 10。
	当 SDK 与服务器异常断开连接时，SDK 会尝试与服务器重连；您可通过此接口设置重连次数。
	*/
	int nConnectRetryCount;		//设置播放器重连次数。

	/*
	默认值：3，单位为秒；取值范围：3 - 30。
	当 SDK 与服务器异常断开连接时， SDK 会尝试与服务器重连；您可通过此接口设置连续两次重连的时间间隔。
	*/
	int nConnectRetryInterval;	//设置播放器重连间隔。

	/*此参数在视频帧与消息需要高同步的情况使用，如：直播答题场景。
	接口说明：
	默认值：false。
	此接口需要搭配 TXLivePusher#sendMessageEx(byte[]) 使用。
	此接口存在一定的性能开销以及兼容性风险。
	*/
	bool bEnableMsg;

	/*此参数需配合连麦库使用
	默认值为：false。
	连麦时，麦克风和播放有回音，所以必须开启回声消除。非连麦情况下，建议不开启。
	*/
	bool bEnableAEC;	//设置回声消除。
};

class IArLivePlayListener
{
public:
	IArLivePlayListener(void) {};
	virtual ~IArLivePlayListener(void) {};

	virtual void onPlayEvent(int nEvent, const ArParams&param) {
		(void)nEvent;
		(void)param;
	};

	virtual void onNetStatus(const ArParams& status) {
		(void)status;
	};
};

class IArVideoRecordListener
{
public:
	IArVideoRecordListener(void) {};
	virtual ~IArVideoRecordListener(void) {};

	virtual void onRecordEvent(int nEvent, const ArParams&param) {
		(void)nEvent;
		(void)param;
	}

	virtual void onRecordProgress(long process) {
		(void)process;
	}

	virtual void onRecordComplete(int nCode, const char*strReason) {
		(void)nCode;
		(void)strReason;
	}
};

class IArSnapshotListener
{
public:
	IArSnapshotListener(void) {};
	virtual ~IArSnapshotListener(void) {};

	virtual void onSnapshot(const char*uid, const char* bmp, int nSize) {
		(void)uid;
		(void)bmp;
		(void)nSize;
	}
};

class IArLivePlayKit
{
public:
	IArLivePlayKit(void) {};
	virtual ~IArLivePlayKit(void) {};

	//### 内核 基础函数
	/* 设置播放配置项。
	参数：
		config	ArLivePlayConfig	播放器配置项，请参见 ArLivePlayConfig。
	*/
	virtual void setConfig(ArLivePlayConfig &config) = 0;

	/* 设置推流回调接口。
	参数：
		listener	IArLivePlayListener	播放器回调，请参见 IArLivePlayListener。
	*/
	virtual void setPlayListener(IArLivePlayListener* listener) = 0;

	//### 播放基础接口
	/* 播放器开始播放。
	参数：
		playUrl	String	播放的流地址。
		playType	int	播放类型。	0：使用Rtmp开源内核播放 1：使用Player内核播放
	返回：
		是否成功启动播放， 0：成功；-1：失败，playUrl 为空；-2：失败，playUrl 非法；-3：失败，playType 非法。
	*/
	virtual int startPlay(const char* playUrl, int playType) = 0;

	/* 停止播放。
	参数：
		isNeedClearLastImg	boolean	true：清除；false：不清除。
	*/
	virtual int stopPlay(bool isNeedClearLastImg) = 0;

	/* 是否正在播放。
	返回：
		true：正在播放；false：未播放。
	*/
	virtual bool isPlaying() = 0;

	/* 暂停播放。
	介绍：
		停止获取流数据，保留最后一帧画面。
	*/
	virtual void pause() = 0;

	/* 恢复播放。
	介绍：
		重新获取数据，获取当前直播数据。
	*/
	virtual void resume() = 0;

	/* FLV 多清晰度切换。
	参数：
		playUrl	String	播放的流地址。
	*/
	virtual int switchStream(const char* playUrl) = 0;

	//### 本地录制和截图
	/* 设置录制回调接口。
	参数：
		listener	IArVideoRecordListener	接口。
	*/
	virtual void setVideoRecordListener(IArVideoRecordListener* listener) = 0;

	/* 启动视频录制。
	参数：
		recordType	int	默认0(仅支持录制直播流)
	*/
	virtual int startRecord(int recordType) = 0;

	/* 停止视频录制。
	参数：

	*/
	virtual int stopRecord() = 0;

	/* 播放过程中本地截图。
	参数：
		listener	IArSnapshotListener	截图回调。
	*/
	virtual void snapshot(IArSnapshotListener* listener) = 0;

	//### 自定义数据处理
	/* 设置软解码数据载体 Buffer。
	参数：
		yuvBuffer	char*	-
	介绍：
		三个注意点：

		1，该 Buffer 用于接受软解回调出来的 I420 格式的 YUV 数据。
		2，Buffer 大小 = width * height * 3 / 2。
		3，视频 width 和 height，可通过 IArLivePlayListener#onPlayEvent(int， ArParams) 的 PLAY_EVT_CHANGE_RESOLUTION 事件获取到。
	*/
	virtual bool addVideoRawData(char* yuvBuffer) = 0;
};

}
}

#endif	// __I_AR_LIVE_PLAY_KIT_H__

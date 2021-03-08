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
#ifndef __I_AR_LIVE_ENGINE_H__
#define __I_AR_LIVE_ENGINE_H__
#include <stdint.h>
#include "ArLiveBase.h"
#include "IArRtcEngine.h"
#include "IArLivePushKit.h"
#include "IArLivePlayKit.h"

namespace ar {
namespace live {

class IArAudioVolumeEvaluationListener
{
public:
	IArAudioVolumeEvaluationListener(void) {};
	virtual ~IArAudioVolumeEvaluationListener(void) {};

	virtual void onAudioVolumeEvaluationNotify(AR::uid_t uid, int volume) {
		(void)uid;
		(void)volume;
	}
};

class IArVideoRawDataListener
{
public:
	IArVideoRawDataListener(void) {};
	virtual ~IArVideoRawDataListener(void) {};

	virtual void onVideoRawDataAvailable(const char*uid, char* yuvBuffer, int width, int height, unsigned int timestamp) {
		(void)uid;
		(void)yuvBuffer;
		(void)width;
		(void)height;
		(void)timestamp;
	}
};

class IArAudioRawDataListener
{
public:
	IArAudioRawDataListener(void) {};
	virtual ~IArAudioRawDataListener(void) {};

	virtual void onPcmDataAvailable(const char*uid, short* buf, int nSampleHz, int nChannel, unsigned int timestamp) {
		(void)uid;
		(void)buf;
		(void)nSampleHz;
		(void)nChannel;
		(void)timestamp;
	}
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

class IArLiveEngineEvent
{
public:
	IArLiveEngineEvent(void) {};
	virtual ~IArLiveEngineEvent(void) {};

};
	
class IArLiveEngine
{
public:

	IArLiveEngine(void) {};
	virtual ~IArLiveEngine(void) {};

	/* 初始化对象。
	参数：
		pEvent	IArLiveEngineEvent	引擎事件对象指针。
	*/
	virtual int initialize(IArLiveEngineEvent* pEvent) = 0;
	/* 释放对象。
	参数：
	*/
	virtual void release() = 0;

	/* 创建推流对象。
	参数：
		pEvent	IArLivePushEvent	推流接收事件对象指针。
	返回：
		IArLivePushKit对象
	*/
	virtual IArLivePushKit* createPushKit(IArLivePushEvent*pEvent) = 0;
	/* 创建拉流对象。
	参数：
		pEvent	IArLivePushEvent	拉流接收事件对象指针。
	返回：
		IArLivePlayKit对象
	*/
	virtual IArLivePlayKit* createPlayKit(IArLivePlayListener*pEvent) = 0;

	/* 调用实验性 API 接口(需连麦库)。
	参数：
		jsonStr	String	jsonStr 接口及参数描述的 JSON 字符串。
	*/
	virtual void callExperimentalAPI(const char* strJson) = 0;

	//**************************************************************************\\
	//**************************************************************************\\
	// Video Module
	/* 设置摄像头预览窗口
	返回：
		0：调用成功		<0: 失败
	*/
	virtual int setupCameraView(const AR::VideoCanvas& canvas) = 0;
	/* 设置视频质量
	参数：
		quality	int	画质类型（标清、高清、超高清）。
		adjustBitrate	boolean	动态码率开关。
		adjustResolution	boolean	动态切分辨率开关。
		orientationMode	ORIENTATION_MODE 编码的视频方向
	返回：
		0：调用成功		<0: 失败
	*/
	virtual int setVideoQuality(int quality, bool adjustBitrate, bool adjustResolution, AR::ORIENTATION_MODE orientationMode) = 0;
	/* 开始摄像头预览
	返回：
		0：调用成功		<0: 失败
	*/
	virtual int startVideoPreview() = 0;
	/* 停止摄像头预览
	返回：
		0：调用成功		<0: 失败
	*/
	virtual int stopVideoPreview() = 0;
	/* 本地视频禁用
	功能：
		开启禁用后，并不会继续推摄像头的图像，但是图像会继续以非常小的黑屏数据进行推送，这样做的目的是为了兼容 H5 上的 video 标签，并让录制出来的 MP4 文件有更好的兼容性。
	参数：
		mute: ture - 开启禁音 / false - 关闭禁音
	返回：
		0：调用成功		<0: 失败
	*/
	virtual int setVideoMute(bool mute) = 0;
	/* 摄像头禁用
	功能：
		开启禁用后，并不会继续采集摄像头图像，但是合流(连麦视频)会继续推流。
	参数：
		mute: ture - 开启禁音 / false - 关闭禁音
	返回：
		0：调用成功		<0: 失败
	*/
	virtual int muteLocalVideoStream(bool mute) = 0;
	/* 视频SEI数据
	*/
	virtual int setVideoSei(const char*pSei, int nLen) = 0;
	/* 自采集视频数据
	*/
	virtual int setVideoCustomData() = 0;

	//**************************************************************************\\
	// Audio Module
	/* 本地音频禁音
	功能：
		开启静音后(包括BGM)，并不会继续采集麦克风的声音，但是会用非常低（5kbps左右）的码率推送伪静音数据， 这样做的目的是为了兼容 H5 上的 video 标签，并让录制出来的 MP4 文件有更好的兼容性。
	参数：
		mute: ture - 开启禁音 / false - 关闭禁音
	返回：
		0：调用成功		<0: 失败
	*/
	virtual int setAudioMute(bool mute) = 0;
	/* Mic禁音
	功能：
		开启静音后(不包括BGM)，并不会继续采集麦克风的声音，但是BGM会继续推流。
	参数：
		mute: ture - 开启禁音 / false - 关闭禁音
	返回：
		0：调用成功		<0: 失败
	*/
	virtual int muteLocalAudioStream(bool mute) = 0;
	/*设置麦克风音量大小
	参数：
		nVolume	int	音量大小，100为正常音量，范围是：[0 ~ 400] 之间的整数。
	返回：
		0：调用成功		<0: 失败
	*/
	virtual int adjustMicVolume(int nVolume) = 0;


	//**************************************************************************\\
	//**************************************************************************\\
	// Play Module - 播放配置接口
	/* 设置播放器的视频渲染 View。
	参数：
		userId	AR::uid_t		视图对象的名称，播放器默认为:"0"，连麦人员为具体人员的UId
		canvas	ArVideoCanvas	视图对象
	*/
	virtual void setPlayerView(AR::uid_t userId, const AR::VideoCanvas& canvas) = 0;
	/* 设置播放渲染模式。
	参数：
		userId	AR::uid_t		视图对象的名称，播放器默认为:"0"，连麦人员为具体人员的UId
		mode	RENDER_MODE_TYPE	视频渲染的模式，请参见 RENDER_MODE_TYPE。
	*/
	virtual void setRenderMode(AR::uid_t uid, AR::RENDER_MODE_TYPE mode) = 0;

	/* 设置图像渲染角度。
	参数：
		userId	AR::uid_t		视图对象的名称，播放器默认为:"0"，连麦人员为具体人员的UId
		rotation	ORIENTATION_MODE	图像渲染角度，可设置值为：RENDER_ROTATION#RENDER_ROTATION_PORTRAIT、RENDER_ROTATION#RENDER_ROTATION_LANDSCAPE。
	*/
	virtual void setRenderRotation(AR::uid_t uid, AR::ORIENTATION_MODE rotation) = 0;
	/* 开启硬件加速。
	参数：
		enable	boolean	true：启用视频硬解码， false：禁用视频硬解码。
	*/
	virtual bool enableHardwareDecode(bool enable) = 0;

	/* 设置是否静音播放。
	参数：
		userId	AR::uid_t		视图对象的名称，播放器默认为:"0"，连麦人员为具体人员的UId
		mute	boolean	true：静音播放；false：不静音播放。
	*/
	virtual int muteRemoteAudioStream(AR::uid_t userId, bool mute) = 0;
	/* 设置是否禁播视频。
	参数：
		userId	AR::uid_t		视图对象的名称，播放器默认为:"0"，连麦人员为具体人员的UId
		mute	boolean	true：播放；false：不播放。
	*/
	virtual int muteRemoteVideoStream(AR::uid_t userId, bool mute) = 0;
	/* 设置是否静音所有播放。
	参数：
		mute	boolean	true：静音播放；false：不静音播放。
	*/
	virtual int muteAllRemoteAudioStreams(bool mute) = 0;
	/* 设置远端流的大小。
	参数：
		userId	AR::uid_t		视图对象的名称，播放器默认为:"0"，连麦人员为具体人员的UId
		streamType	REMOTE_VIDEO_STREAM_TYPE	流大小
	*/
	virtual int setRemoteVideoStreamType(AR::uid_t userId, AR::REMOTE_VIDEO_STREAM_TYPE streamType) = 0;

	/* 设置播放音量。
	参数：
		volume	int	音量大小，取值范围 0 - 100。
	*/
	virtual int adjustSpkrVolume(int volume) = 0;
	/* 设置声音播放模式。
	介绍：
		播放模式有两种(仅Android和iOS)：

		听筒：声音将从听筒播出。
		扬声器：声音将从扬声器播出。
	*/
	virtual int setAudioRoute(int audioRoute) = 0;

	virtual int setAudioCustomData() = 0;
	
	//**************************************************************************\\
	// 数据回调接口
	/* 设置音量大小回调接口。
	参数：
		listener	IArAudioVolumeEvaluationListener	音量大小回调接口。
	*/
	virtual void setAudioVolumeEvaluationListener(IArAudioVolumeEvaluationListener* listener) = 0;

	/* 启用音量大小评估。
	参数：
		intervalMs	int	intervalMs 决定了 onAudioVolumeEvaluationNotify 回调的触发间隔，单位为ms，最小间隔为 100ms，如果小于等于 0 则会关闭回调，建议设置为 300ms。
	*/
	virtual void enableAudioVolumeEvaluation(int intervalMs) = 0;
	/* 设置软解码视频数据回调。
	参数：
		listener	IArVideoRawDataListener	视频数据回调。
	*/
	virtual void setVideoRawDataListener(IArVideoRawDataListener* listener) = 0;

	/* 设置音频数据回调。
	参数：
		listener	ITXAudioRawDataListener	音频数据回调。
	*/
	virtual void setAudioRawDataListener(IArAudioRawDataListener* listener) = 0;

#ifdef ANY_RTC_CHAT
	//**************************************************************************\\
	//**************************************************************************\\
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

	//**************************************************************************\\
	//**************************************************************************\\
	// 屏幕共享接口
#if defined(_WIN32)
	virtual int startScreenCaptureByScreenRect(const AR::Rectangle& screenRect, const AR::Rectangle& regionRect, const AR::ScreenCaptureParameters& captureParams) = 0;
#endif
	virtual int startScreenCaptureByWindowId(AR::view_t windowId, const AR::Rectangle& regionRect, const AR::ScreenCaptureParameters& captureParams) = 0;
	virtual int updateScreenCaptureParameters(const AR::ScreenCaptureParameters& captureParams) = 0;
	virtual int updateScreenCaptureRegion(const AR::Rectangle& regionRect) = 0;
	virtual int stopScreenCapture() = 0;

	//**************************************************************************\\
	//**************************************************************************\\
	// 连麦接口
	virtual int initRtcEngine(const char*appId) = 0;
	virtual int setClientRole(AR::CLIENT_ROLE_TYPE role) = 0;
	virtual int joinChannel(const char* token, const char* channelId, const char* info, AR::uid_t uid) = 0;
	virtual int switchChannel(const char* token, const char* channelId) = 0;
	virtual int leaveChannel() = 0;
	virtual int renewToken(const char* token) = 0;

#endif
};
}
};



#endif	// __I_AR_LIVE_ENGINE_H__
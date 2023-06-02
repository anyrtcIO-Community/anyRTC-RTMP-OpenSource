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
#ifndef __I_AR_LIVE2_ENGINE_H__
#define __I_AR_LIVE2_ENGINE_H__
#include <stdint.h>
#include "IArLivePusher.hpp"
#include "IArLivePlayer.hpp"

namespace anyrtc {

class IArLive2EngineObserver
{
public:
	IArLive2EngineObserver(void) {};
	virtual ~IArLive2EngineObserver(void) {};

	
};

class IArLive2Engine
{
protected:
	IArLive2Engine(void) {};
	
public:
	virtual ~IArLive2Engine(void) {};

	/**
		* 设置引擎的回调。
		*
		* 通过设置回调，可以监听 IArLive2Engine 的一些回调事件，
		* 包括音视频数据、警告和错误信息等。
		*
		* @param observer 推流器的回调目标对象，更多信息请查看 {@link IArLive2EngineObserver}
		*/
	virtual int32_t initialize(IArLive2EngineObserver* observer) = 0;

	/**
		* 释放对象。
		* @param
		*/
	virtual void release() = 0;

public:
	/// @name 创建与销毁 ArLivePusher 实例
	/// @{
#ifdef __ANDROID__

/**
	* 用于动态加载 dll 时，获取 ArLivePusher 对象指针。
	*
	* @return 返回 ArLivePusher 对象的指针，注意：需要调用 releaseArLivePusher析构
	* @param context Android 上下文，内部会转为 ApplicationContext 用于系统 API 调用
	* @param mode 推流协议，RTMP还是ROOM
	* @note 本接口仅适用于Android平台
	*/
	virtual AR::IArLivePusher* createArLivePusher(void* context) = 0;

	/**
		* 用于动态加载 dll 时，获取 ArLivePlayer 对象指针
		*
		* @return 返回 ArLivePlayer 对象的指针，注意：请调用 releaseArLivePlayer 析构
		* @param context Android 上下文，内部会转为 ApplicationContext 用于系统 API 调用
		* @note 本接口仅适用于Android平台
		*/
	virtual AR::IArLivePlayer* createArLivePlayer(void* context) = 0;
#else

/**
	* 用于动态加载 dll 时，获取 ArLivePusher 对象指针。
	*
	* @return 返回 ArLivePusher 对象的指针，注意：需要调用 releaseArLivePusher析构。
	* @param mode 推流协议，RTMP还是ROOM
	* @note 本接口适用于Windows、Mac、iOS平台
	*/
	virtual AR::IArLivePusher* createArLivePusher() = 0;

	/**
		* 用于动态加载 dll 时，获取 ArLivePlayer 对象指针
		*
		* @return 返回 ArLivePlayer 对象的指针，注意：请调用 releaseArLivePlayer 析构
		* @note 本接口适用于Windows、Mac、iOS平台
		*/
	virtual AR::IArLivePlayer* createArLivePlayer() = 0;
#endif

	/**
		* 析构 ArLivePusher 对象
		*
		* @param pusher ArLivePusher 对象的指针
		*/
	virtual void releaseArLivePusher(AR::IArLivePusher* pusher) = 0;

	/**
		* 析构 ArLivePlayer 对象
		*
		* @param player ArLivePlayer 对象的指针
		*/
	virtual void releaseArLivePlayer(AR::IArLivePlayer* player) = 0;
	/// @}


	virtual void setAppInBackground(bool bBackground) = 0;

public:
	
};

V2_API IArLive2Engine* V2_CALL createArLive2Engine();

};//namespace anyrtc;

#endif	// __I_AR_LIVE2_ENGINE_H__

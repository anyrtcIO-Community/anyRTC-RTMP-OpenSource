// Copyright (c) 2020 anyRTC(https://www.anyrtc.io). All rights reserved.

/// @defgroup ArLiveDef_cplusplus ArLiveDef
/// anyRTC开源直播服务关键类型定义
/// @{

#ifndef ANYRTC_CPP_ARLIVEDEF_H_
#define ANYRTC_CPP_ARLIVEDEF_H_

#ifdef LITEAV_EXPORTS
#define LITEAV_API __declspec(dllexport)
#else
#define LITEAV_API __declspec(dllimport)
#endif

#include <stdint.h>
#include "ArLiveCode.hpp"

#ifdef _WIN32
#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
#include <windows.h>
#define V2_CALL __cdecl
#ifdef ANYRTC_EXPORTS
#define V2_API __declspec(dllexport)
#else
#define V2_API __declspec(dllimport)
#endif
#elif __APPLE__
#include <TargetConditionals.h>
#define V2_API __attribute__((visibility("default")))
#define V2_CALL 
#elif __ANDROID__
#define V2_API __attribute__((visibility("default")))
#define V2_CALL 
#else
#define V2_API
#define V2_CALL 
#endif

#define TARGET_PLATFORM_DESKTOP ((__APPLE__ && TARGET_OS_MAC && !TARGET_OS_IPHONE) || _WIN32)
#define TARGET_PLATFORM_PHONE (__ANDROID__ || (__APPLE__ && TARGET_OS_IOS))


#ifndef AR
#define AR anyrtc
#endif

#include <stdio.h>

namespace anyrtc {

/**
 * @brief 支持协议
 */
enum ArLiveMode {
	ArLiveModeNotSupported = 0,

	// 支持推流Rtmp协议
	ArLiveModePushRtmp,
    // 支持推流Rtsp协议
    ArLiveModePushRtsp,
    // 支持推流协议: WebRTC
    ArLiveModePushWebRTC,
    // 支持Gb2821视频推流
    ArLiveModePushGb28181,

    // 支持拉流Rtmp协议
    ArLiveModePlayRtmp = 10,
    // 支持拉流Rtsp协议
    ArLiveModePlayRtsp,
    // 支持拉流WebRtc协议
    ArLiveModePlayWebRtc,
    // 支持拉流Flv-http协议
    ArLiveModePlayFlvHttp,
    // 支持拉流Flv-WebSocket协议
    ArLiveModePlayFlvWs,

    // 支持推拉流WebRtc点对点音视频通话
    ArLiveModeP2pWebrtc = 100,
    
};
/// @}

/**
 * @brief 支持的厂家
 */
enum ArLiveOem {
    ArLiveOemUnknow = 0,
    //*开源服务商
    // 支持SRS
    ArLiveOemSrs,
    // 支持ZLMedia
    ArLiveOemZLMedia,
    // 支持Nginx
    ArLiveOemNginx,

    //*商业平台
    // 支持AnyRtc
    ArLiveOemAnyRtc = 10,
    // 支持阿里云AliYun
    ArLiveOemAliYun,
    // 支持腾讯云Tecent
    ArLiveOemTecent,
    // 支持华为云HuaWei
    ArLiveOemHuaWei,
    // 支持七牛云QiNiu
    ArLiveOemQiNiu,
    // 支持Aws
    ArLiveOemAws,

    ArLiveOemMax,
};
/// @}

/**
 * @brief 支持的协议
 */
enum ArLiveProtocol {
    //Tcp - 默认模式
    ArLiveProtocolTcp = 0,
    //Udp - WebRtc一般都是Udp传输
    ArLiveProtocolUdp,
    //Srt - 仅支持rtmp推拉流
    ArLiveProtocolSrt,
};
/// @}

typedef const char* uid_t;
typedef void* view_t;

/** Network quality types. */
enum QUALITY_TYPE
{
    /** 0: The network quality is unknown. */
    QUALITY_UNKNOWN = 0,
    /**  1: The network quality is excellent. */
    QUALITY_EXCELLENT = 1,
    /** 2: The network quality is quite good, but the bitrate may be slightly lower than excellent. */
    QUALITY_GOOD = 2,
    /** 3: Users can feel the communication slightly impaired. */
    QUALITY_POOR = 3,
    /** 4: Users cannot communicate smoothly. */
    QUALITY_BAD = 4,
    /** 5: The network is so bad that users can barely communicate. */
    QUALITY_VBAD = 5,
    /** 6: The network is down and users cannot communicate at all. */
    QUALITY_DOWN = 6,
    /** 7: Users cannot detect the network quality. (Not in use.) */
    QUALITY_UNSUPPORTED = 7,
    /** 8: Detecting the network quality. */
    QUALITY_DETECTING = 8,
};

/////////////////////////////////////////////////////////////////////////////////
//
//           （一）视频相关类型定义
//
/////////////////////////////////////////////////////////////////////////////////
/// @name 视频相关类型定义
/// @{

/** Video display modes. */
enum RENDER_MODE_TYPE
{
	/**
	 1: Uniformly scale the video until it fills the visible boundaries (cropped). One dimension of the video may have clipped contents.
	 */
	RENDER_MODE_HIDDEN = 1,
	/**
	 2: Uniformly scale the video until one of its dimension fits the boundary (zoomed to fit). Areas that are not filled due to disparity in the aspect ratio are filled with black.
	 */
	RENDER_MODE_FIT = 2,
	/** **DEPRECATED** 3: This mode is deprecated.
	 */
	RENDER_MODE_ADAPTIVE = 3,
	/**
	4: The fill mode. In this mode, the SDK stretches or zooms the video to fill the display window.
	*/
	RENDER_MODE_FILL = 4,
};
/** Video mirror modes. */
enum VIDEO_MIRROR_MODE_TYPE
{
	/** 0: (Default) The SDK enables the mirror mode.
	 */
	VIDEO_MIRROR_MODE_AUTO = 0,//determined by SDK
	/** 1: Enable mirror mode. */
	VIDEO_MIRROR_MODE_ENABLED = 1,//enabled mirror
	/** 2: Disable mirror mode. */
	VIDEO_MIRROR_MODE_DISABLED = 2,//disable mirror
};

/**
 * Video dimensions.
 */
struct VideoDimensions {
	/** Width (pixels) of the video. */
	int width;
	/** Height (pixels) of the video. */
	int height;
	/** Fps*/
	int fps;
	VideoDimensions()
		: width(640), height(480), fps(25)
	{}
	VideoDimensions(int w, int h)
		: width(w), height(h)
	{}
};

/** Video display settings of the VideoCanvas class.
*/
struct VideoCanvas
{
	/** Video display window (view).
	 */
	view_t view;
	/** The rendering mode of the video view. See RENDER_MODE_TYPE
	 */
	int renderMode;
	/** The user ID. */
	uid_t uid;
	void *priv; // private data (underlying video engine denotes it)
	/** The mirror mode of the video view. See VIDEO_MIRROR_MODE_TYPE
	 @note
	 - For the mirror mode of the local video view: If you use a front camera, the SDK enables the mirror mode by default; if you use a rear camera, the SDK disables the mirror mode by default.
	 - For the mirror mode of the remote video view: The SDK disables the mirror mode by default.
	*/
	VIDEO_MIRROR_MODE_TYPE mirrorMode;

	VideoCanvas()
		: view(NULL)
		, renderMode(RENDER_MODE_HIDDEN)
		, uid(NULL)
		, priv(NULL)
		, mirrorMode(VIDEO_MIRROR_MODE_AUTO)
	{
		
	}
	VideoCanvas(view_t v, int m, uid_t u)
		: view(v)
		, renderMode(m)
		, uid(u)
		, priv(NULL)
		, mirrorMode(VIDEO_MIRROR_MODE_AUTO)
	{
		
	}
	VideoCanvas(view_t v, int rm, uid_t u, VIDEO_MIRROR_MODE_TYPE mm)
		: view(v)
		, renderMode(rm)
		, uid(u)
		, priv(NULL)
		, mirrorMode(mm)
	{
	
	}
};

/**
 * @brief 视频编码器类型。
 */
enum ArLiveVideCodec {
    /// H264编码
    ArLiveVideCodecH264,
    /// H265编码
    ArLiveVideCodecH265,
    /// Jpeg编码
    ArLiveVideCodecMJpeg
};

/**
 * @brief 视频分辨率
 */
enum ArLiveVideoResolution {

    /// 分辨率 160*160，码率范围：100Kbps ~ 150Kbps，帧率：15fps
    ArLiveVideoResolution160x160,

    /// 分辨率 270*270，码率范围：200Kbps ~ 300Kbps，帧率：15fps
    ArLiveVideoResolution270x270,

    /// 分辨率 480*480，码率范围：350Kbps ~ 525Kbps，帧率：15fps
    ArLiveVideoResolution480x480,

    /// 分辨率 320*240，码率范围：250Kbps ~ 375Kbps，帧率：15fps
    ArLiveVideoResolution320x240,

    /// 分辨率 480*360，码率范围：400Kbps ~ 600Kbps，帧率：15fps
    ArLiveVideoResolution480x360,

    /// 分辨率 640*480，码率范围：600Kbps ~ 900Kbps，帧率：15fps
    ArLiveVideoResolution640x480,

    /// 分辨率 320*180，码率范围：250Kbps ~ 400Kbps，帧率：15fps
    ArLiveVideoResolution320x180,

    /// 分辨率 480*270，码率范围：350Kbps ~ 550Kbps，帧率：15fps
    ArLiveVideoResolution480x270,

    /// 分辨率 640*360，码率范围：500Kbps ~ 900Kbps，帧率：15fps
    ArLiveVideoResolution640x360,

    /// 分辨率 960*540，码率范围：800Kbps ~ 1500Kbps，帧率：15fps
    ArLiveVideoResolution960x540,

    /// 分辨率 1280*720，码率范围：1000Kbps ~ 1800Kbps，帧率：15fps
    ArLiveVideoResolution1280x720,

    /// 分辨率 1920*1080，码率范围：2500Kbps ~ 3000Kbps，帧率：15fps
    ArLiveVideoResolution1920x1080,

};

/**
 * @brief 视频宽高比模式。
 *
 * @note
 * - 横屏模式下的分辨率: ArLiveVideoResolution640_360 + ArLiveVideoResolutionModeLandscape = 640x360
 * - 竖屏模式下的分辨率: ArLiveVideoResolution640_360 + ArLiveVideoResolutionModePortrait = 360x640
 */
enum ArLiveVideoResolutionMode {

    /// 横屏模式
    ArLiveVideoResolutionModeLandscape,

    /// 竖屏模式
    ArLiveVideoResolutionModePortrait

};

/**
 * @brief 视频编码填充模式。
 */
enum ArLiveVideoScaleMode {
    
    /// 图像铺满屏幕，超出显示视窗的视频部分将被裁剪，画面显示可能不完整
    ArLiveVideoScaleModeFill,

    /// 图像长边填满屏幕，短边区域会被填充黑色，画面的内容完整
    ArLiveVideoScaleModeFit,

    /// 图像长边填满屏幕，根据设置的比例进行缩放，画面的内容完整
    ArLiveVideoScaleModeAuto,

};

/**
 * 视频编码参数。
 *
 * 该设置决定远端用户看到的画面质量。
 */
struct V2_API ArLiveVideoEncoderParam {
    ///【字段含义】 视频分辨率
    ///【特别说明】如需使用竖屏分辨率，请指定 videoResolutionMode 为 Portrait，例如： 640 × 360 + Portrait = 360 × 640。
    ///【推荐取值】
    /// - 桌面平台（Win + Mac）：建议选择 640 × 360 及以上分辨率，videoResolutionMode 选择 Landscape，即横屏分辨率。
    ArLiveVideoResolution videoResolution;

    ///【字段含义】分辨率模式（横屏分辨率 or 竖屏分辨率）
    ///【推荐取值】桌面平台（Windows、Mac）建议选择 Landscape。
    ///【特别说明】如需使用竖屏分辨率，请指定 resMode 为 Portrait，例如： 640 × 360 + Portrait = 360 × 640。
    ArLiveVideoResolutionMode videoResolutionMode;

    ///【字段含义】视频采集帧率
    ///【推荐取值】15fps 或 20fps。5fps 以下，卡顿感明显。10fps 以下，会有轻微卡顿感。20fps 以上，会浪费带宽（电影的帧率为 24fps）。
    uint32_t videoFps;

    ///【字段含义】目标视频码率，SDK 会按照目标码率进行编码，只有在弱网络环境下才会主动降低视频码率。
    ///【推荐取值】请参考 ArLiveVideoResolution 在各档位注释的最佳码率，也可以在此基础上适当调高。
    ///           比如：ArLiveVideoResolution1280x720 对应 1200kbps 的目标码率，您也可以设置为 1500kbps 用来获得更好的观感清晰度。
    ///【特别说明】您可以通过同时设置 videoBitrate 和 minVideoBitrate 两个参数，用于约束 SDK 对视频码率的调整范围：
    /// - 如果您将 videoBitrate 和 minVideoBitrate 设置为同一个值，等价于关闭 SDK 对视频码率的自适应调节能力。
    uint32_t videoBitrate;

    ///【字段含义】最低视频码率，SDK 会在网络不佳的情况下主动降低视频码率以保持流畅度，最低会降至 minVideoBitrate 所设定的数值。
    ///【推荐取值】您可以通过同时设置 videoBitrate 和 minVideoBitrate 两个参数，用于约束 SDK 对视频码率的调整范围：
    /// - 如果您将 videoBitrate 和 minVideoBitrate 设置为同一个值，等价于关闭 SDK 对视频码率的自适应调节能力。
    uint32_t minVideoBitrate;

    ///【字段含义】视频编码时，采集图像与设置的编码大小不一致时，采用什么策略进行缩放裁剪
    ///【推荐取值】ArLiveVideoScaleModeAuto可以保证图像的完整性，但是大小会与设置的略有不同
    ArLiveVideoScaleMode videoScaleMode;

    ArLiveVideoEncoderParam(ArLiveVideoResolution resolution)
        : videoResolution(resolution)
#if defined(WEBRTC_WIN) | defined(WEBRTC_MAC)
        , videoResolutionMode(ArLiveVideoResolutionModeLandscape)
#else
        , videoResolutionMode(ArLiveVideoResolutionModePortrait)
#endif
        , videoFps(15)
        , videoBitrate(900)
        , minVideoBitrate(500)
        , videoScaleMode(ArLiveVideoScaleModeAuto)
         {};
};

/**
 * @brief 本地摄像头镜像类型。
 */
enum ArLiveMirrorType {

    /// 系统默认镜像类型，前置摄像头镜像，后置摄像头不镜像
    ArLiveMirrorTypeAuto,

    /// 前置摄像头和后置摄像头，都切换为镜像模式
    ArLiveMirrorTypeEnable,

    /// 前置摄像头和后置摄像头，都切换为非镜像模式
    ArLiveMirrorTypeDisable

};

/**
 * @brief 视频画面填充模式。
 */
enum ArLiveFillMode {

    /// 图像铺满屏幕，超出显示视窗的视频部分将被裁剪，画面显示可能不完整
    ArLiveFillModeFill,

    /// 图像长边填满屏幕，短边区域会被填充黑色，画面的内容完整
    ArLiveFillModeFit

};

/**
 * @brief 视频播放模式。
 */
enum ArLivePlayMode {

    /// 直播模式 - 暂停的过程中，数据会丢失，保证实时性
    ArLivePlayModeLive,

    /// 点播模式 - 暂停的过程中，数据不会丢失，恢复后会继续播放
    ArLivePlayModeVod

};

/**
 * @brief 视频画面顺时针旋转角度。
 */
enum ArLiveRotation {

    /// 不旋转
    ArLiveRotation0,

    /// 顺时针旋转90度
    ArLiveRotation90,

    /// 顺时针旋转180度
    ArLiveRotation180,

    /// 顺时针旋转270度
    ArLiveRotation270

};

/**
 * @brief 视频帧的像素格式。
 */
enum ArLivePixelFormat {

    /// 未知
    ArLivePixelFormatUnknown,

    /// YUV420P I420
    ArLivePixelFormatI420,

    /// BGRA8888
    ArLivePixelFormatBGRA32,

    /// YUV420P NV12
    ArLivePixelFormatNV12,

    ArLivePixelFormatNV21

};

/**
 * @brief 视频数据包装格式。
 *
 * @note 在自定义采集和自定义渲染功能，您需要用到下列枚举值来指定您希望以什么样的格式来包装视频数据。
 */
enum ArLiveBufferType {

    /// 未知
    ArLiveBufferTypeUnknown,

    /// 二进制Buffer类型
    ArLiveBufferTypeByteBuffer

};

/**
 * @brief 视频帧信息类。
 *        ArLiveVideoFrame 描述了视频帧编码之前或解码之后图像的原始数据。
 * @note  在自定义采集和渲染期间使用。使用自定义采集时, 你需要向 ArLiveVideoFrame 中填充视频数据用来发送。 使用自定义渲染时, 视频帧是通过填充 ArLiveVideoFrame 返回。
 */
struct ArLiveVideoFrame {
    /// 【字段含义】视频帧像素格式
    ArLivePixelFormat pixelFormat;

    /// 【字段含义】视频数据包装格式
    ArLiveBufferType bufferType;

    /// 【字段含义】bufferType 为 ArLiveBufferTypeByteBuffer 时的视频数据。
    char* data;

    /// 【字段含义】视频数据的长度，单位是字节
    int32_t length;

    /// 【字段含义】视频宽度
    int32_t width;

    /// 【字段含义】视频高度
    int32_t height;

    /// 【字段含义】视频Stride
    int32_t stride;

    /// 【字段含义】视频帧的顺时针旋转角度
    ArLiveRotation rotation;

    ArLiveVideoFrame() : pixelFormat(ArLivePixelFormatUnknown), bufferType(ArLiveBufferTypeUnknown), data(nullptr), length(0), width(0), height(0), stride(0), rotation(ArLiveRotation0) {
    }
};
/// @}

/// @}
/////////////////////////////////////////////////////////////////////////////////
//
//          （二）音频相关类型定义
//
/////////////////////////////////////////////////////////////////////////////////

/// @name 音频相关类型定义
/// @{

/**
 * @brief 声音音质。
 */
enum ArLiveAudioQuality {

    /// 语音音质：采样率：16k；单声道；音频码率：16kbps；适合语音通话为主的场景，比如在线会议，语音通话
    ArLiveAudioQualitySpeech,

    /// 默认音质：采样率：48k；单声道；音频码率：50kbps；SDK 默认的音频质量，如无特殊需求推荐选择之
    ArLiveAudioQualityDefault,

    /// 音乐音质：采样率：48k；双声道 + 全频带；音频码率：128kbps；适合需要高保真传输音乐的场景，比如K歌、音乐直播等
    ArLiveAudioQualityMusic

};

/**
 * @brief 音频帧数据
 */
struct ArLiveAudioFrame {
    /// 【字段含义】音频数据
    char* data;

    /// 【字段含义】音频数据的长度
    uint32_t length;

    /// 【字段含义】采样率
    uint32_t sampleRate;

    /// 【字段含义】声道数
    uint32_t channel;

    ArLiveAudioFrame() : data(nullptr), length(0), sampleRate(48000), channel(1) {
    }
};

struct ArLiveAudioFrameObserverFormat {
    ///【字段含义】采样率
    ///【推荐取值】默认值：48000Hz。支持 16000, 32000, 44100, 48000。
    int sampleRate;

    ///【字段含义】声道数
    ///【推荐取值】默认值：1，代表单声道。可设定的数值只有两个数字：1-单声道，2-双声道。
    int channel;

    ///【字段含义】采样点数
    ///【推荐取值】取值必须是 sampleRate/100 的整数倍。
    int samplesPerCall;

    ArLiveAudioFrameObserverFormat() : sampleRate(0), channel(0), samplesPerCall(0) {
    }
};

/// @}
/////////////////////////////////////////////////////////////////////////////////
//
//          （三）推流器和播放器的一些统计指标数据定义
//
/////////////////////////////////////////////////////////////////////////////////

/// @name 推流器和播放器的一些统计指标数据定义
/// @{

/**
 * @brief 推流器的统计数据。
 */
struct ArLivePusherStatistics {
    /// 【字段含义】当前 App 的 CPU 使用率（％）
    int32_t appCpu;

    /// 【字段含义】当前系统的 CPU 使用率（％）
    int32_t systemCpu;

    /// 【字段含义】视频宽度
    int32_t width;

    /// 【字段含义】视频高度
    int32_t height;

    /// 【字段含义】帧率（fps）
    int32_t fps;

    /// 【字段含义】视频码率（Kbps）
    int32_t videoBitrate;

    /// 【字段含义】音频码率（Kbps）
    int32_t audioBitrate;

    /// 【字段含义】推流质量
    QUALITY_TYPE pushQuality;

    ArLivePusherStatistics() : appCpu(0), systemCpu(0), width(0), height(0), fps(0), audioBitrate(0), videoBitrate(0), pushQuality(QUALITY_UNKNOWN){
    }
};

/**
 * @brief 播放器的统计数据。
 */
struct ArLivePlayerStatistics {
    /// 【字段含义】当前 App 的 CPU 使用率（％）
    int32_t appCpu;

    /// 【字段含义】当前系统的 CPU 使用率（％）
    int32_t systemCpu;

    /// 【字段含义】视频宽度
    int32_t width;

    /// 【字段含义】视频高度
    int32_t height;

    /// 【字段含义】帧率（fps）
    int32_t fps;

    /// 【字段含义】视频码率（Kbps）
    int32_t videoBitrate;

    /// 【字段含义】音频码率（Kbps）
    int32_t audioBitrate;

    /// 【字段含义】拉流质量
    QUALITY_TYPE pullQuality;

    ArLivePlayerStatistics() : appCpu(0), systemCpu(0), width(0), height(0), fps(0), audioBitrate(0), videoBitrate(0), pullQuality(QUALITY_UNKNOWN) {
        videoFrameRate = 0;
        audioFrameRate = 0;
        rtt = 0;
        dropRate = 0;
        totalBytes = 0;
        totalFrozenTime = 0;
        frozenRate = 0;
        totalActiveTime = 0;
        videoRenderFrameRate = 0;
        videoDroppedFrameRate = 0;
    }

    //@Eric - 20230511 - 增加统计
    /// 【字段含义】视频帧率
    int32_t videoFrameRate;
    /// 【字段含义】音频帧率
    int32_t audioFrameRate;
    /// 【字段含义】rtt时延
    int32_t rtt;
    /// 【字段含义】丢包率
    int32_t dropRate;
    /// 【字段含义】当前周期总传输的字节数
    int32_t totalBytes;
    /// 【字段含义】卡顿的总时长
    int totalFrozenTime;
    /// 【字段含义】卡顿率
    int frozenRate;
    /// 【字段含义】播放总时长
    int totalActiveTime;
    /// 【字段含义】实际的视频渲染帧率
    int videoRenderFrameRate;
    /// 【字段含义】实际的视频丢帧率
    int videoDroppedFrameRate;
};

/// @}

/// @}
/////////////////////////////////////////////////////////////////////////////////
//
//          （四）连接状态相关枚举值定义
//
/////////////////////////////////////////////////////////////////////////////////
/// @name 连接状态相关枚举值定义
/// @{

/**
 * @brief 直播流连接状态
 */
enum ArLivePushStatus {

    /// 与服务器断开连接
    ArLivePushStatusDisconnected,

    /// 正在连接服务器
    ArLivePushStatusConnecting,

    /// 连接服务器成功
    ArLivePushStatusConnectSuccess,

    /// 重连服务器中
    ArLivePushStatusReconnecting

};
/// @}

/// @}
/////////////////////////////////////////////////////////////////////////////////
//
//          (五) 音视频状态有关的枚举值的定义
//
/////////////////////////////////////////////////////////////////////////////////

/// @name 音视频状态有关的枚举值的定义
/// @{

/**
 * @brief 直播流的播放状态。
 */
enum ArLivePlayStatus {

    /// 播放停止
    ArLivePlayStatusStopped,

    /// 正在播放
    ArLivePlayStatusPlaying,

    /// 正在缓冲(首次加载不会抛出 Loading 事件)
    ArLivePlayStatusLoading,

};

/**
 * @brief 直播流的播放状态变化原因
 */
enum ArLiveStatusChangeReason {

    /// 内部原因
    ArLiveStatusChangeReasonInternal,

    /// 开始网络缓冲
    ArLiveStatusChangeReasonBufferingBegin,

    /// 结束网络缓冲
    ArLiveStatusChangeReasonBufferingEnd,

    /// 本地启动播放
    ArLiveStatusChangeReasonLocalStarted,

    /// 本地停止播放
    ArLiveStatusChangeReasonLocalStopped,

    /// 远端可播放
    ArLiveStatusChangeReasonRemoteStarted,

    /// 远端流停止或中断
    ArLiveStatusChangeReasonRemoteStopped,

    /// 远端流离线
    ArLiveStatusChangeReasonRemoteOffline,

};
/// @}

/////////////////////////////////////////////////////////////////////////////////
//
//          (六) 公共配置组件
//
/////////////////////////////////////////////////////////////////////////////////
/// @name 公共配置组件有关的枚举值的定义
/// @{

/**
 * 图片类型.
 */
enum ArLiveImageType {

    ///图片文件路径，支持 GIF、JPEG、PNG文件格式
    ArLiveImageTypeFile = 0,

    /// BGRA32格式内存块
    ArLiveImageTypeBGRA32 = 1,

    /// RGBA32格式内存块
    ArLiveImageTypeRGBA32 = 2,
};

/**
 * 图片信息
 */
struct ArLiveImage {
    /// ArLiveImageTypeFile: 图片路径; 其他类型:图片内容
    const char* imageSrc;

    ///图片数据类型,默认为ArLiveImageFile {@link ArLiveImageType}
    ArLiveImageType imageType;

    ///图片宽度,默认为0(图片数据类型为ArLiveImageTypeFile,忽略该参数)
    uint32_t imageWidth;

    ///图片高度,默认为0(图片数据类型为ArLiveImageTypeFile,忽略该参数)
    uint32_t imageHeight;

    ///图片数据的长度;单位字节
    uint32_t imageLength;

    ArLiveImage() : imageSrc(nullptr), imageType(ArLiveImageTypeBGRA32), imageWidth(0), imageHeight(0), imageLength(0){};
};

/// @name 屏幕分享有关的定义
/// @{
/**
 * 屏幕分享目标信息
 */
enum ArLiveScreenCaptureSourceType {

    /// 未知
    ArLiveScreenCaptureSourceTypeUnknown = -1,

    /// 该分享目标是某一个窗口
    ArLiveScreenCaptureSourceTypeWindow = 0,

    /// 该分享目标是整个桌面
    ArLiveScreenCaptureSourceTypeScreen = 1,

    /// 自定义窗口类型
    ArLiveScreenCaptureSourceTypeCustom = 2,

};

/**
 * 屏幕分享窗口信息
 *
 * 您可以通过 getScreenCaptureSources() 枚举可共享的窗口列表，列表通过 IArLiveScreenCaptureSourceList 返回
 */
struct ArLiveScreenCaptureSourceInfo {
    /// 采集源类型
    ArLiveScreenCaptureSourceType sourceType;

    /// 采集源 ID；对于窗口，该字段指示窗口句柄；对于屏幕，该字段指示屏幕 ID
    void* sourceId;

    /// 采集源名称，UTF8 编码
    const char* sourceName;

    /// 缩略图内容
    ArLiveImage thumbBGRA;

    /// 图标内容
    ArLiveImage iconBGRA;

    /// 是否为最小化窗口，通过 getScreenCaptureSources 获取列表时的窗口状态，仅采集源为 Window 时才可用
    bool isMinimizeWindow;

    /// 是否为主屏，是否为主屏，仅采集源类型为 Screen 时才可用
    bool isMainScreen;

    ArLiveScreenCaptureSourceInfo() : sourceType(ArLiveScreenCaptureSourceTypeUnknown), sourceId(nullptr), sourceName(nullptr), isMinimizeWindow(false), isMainScreen(false){};
};

/**
 * 屏幕分享窗口列表
 */
class IArLiveScreenCaptureSourceList {
   protected:
    virtual ~IArLiveScreenCaptureSourceList() {
    }

   public:
    /**
     * @return 窗口个数
     */
    virtual uint32_t getCount() = 0;

    /**
     * @return 窗口信息
     */
    virtual ArLiveScreenCaptureSourceInfo getSourceInfo(uint32_t index) = 0;

    /**
     * @brief 遍历完窗口列表后，调用 release 释放资源。
     */
    virtual void release() = 0;
};

/**
 * 屏幕分享参数
 *
 * 您可以通过设置结构体内的参数控制屏幕分享边框的颜色、宽度、是否采集鼠标等参数
 */
struct ArLiveScreenCaptureProperty {
    /// 是否采集目标内容时顺带采集鼠标，默认为 true
    bool enableCaptureMouse;

    /// 是否高亮正在共享的窗口，默认为 true
    bool enableHighlightBorder;

    /// 是否开启高性能模式（只会在分享屏幕时会生效），开启后屏幕采集性能最佳，但无法过滤远端的高亮边框，默认为 true
    bool enableHighPerformance;

    /// 指定高亮边框颜色，RGB 格式，传入 0 时采用默认颜色，默认颜色为 #8CBF26
    int highlightBorderColor;

    /// 指定高亮边框的宽度，传入0时采用默认描边宽度，默认宽度为 5，最大值为 50
    int highlightBorderSize;

    /// 窗口采集时是否采集子窗口（与采集窗口具有 Owner 或 Popup 属性），默认为 false
    bool enableCaptureChildWindow;

    ArLiveScreenCaptureProperty() : enableCaptureMouse(true), enableHighlightBorder(true), enableHighPerformance(true), highlightBorderColor(0), highlightBorderSize(0), enableCaptureChildWindow(false) {
    }
};

struct ArLivePlayConfig
{
	ArLivePlayConfig(void) : bAuto(true), nCacheTime(5), nMinCacheTime(1), nMaxCacheTime(5), nVideoBlockThreshold(800), nConnectRetryCount(3), nConnectRetryInterval(3), bEnableMsg(false), strPlayerId(NULL) {

	}
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
	int nMinCacheTime;	//设置最小的缓存时间。

	/*
	默认值：5，单位为秒。
	仅在启用自动调用缓存时间接口时，有效。
	*/
	int nMaxCacheTime;	//设置最大的缓存时间。

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
	此接口需要搭配 ArLivePusher#sendMessageEx(byte[]) 使用。
	此接口存在一定的性能开销以及兼容性风险。
	*/
	bool bEnableMsg;

	/*播放器ID
	*/
	const char* strPlayerId;
};
/// @}

}  // namespace anyrtc

#endif  // ANYRTC_CPP_ARLIVEDEF_H_
/// @}

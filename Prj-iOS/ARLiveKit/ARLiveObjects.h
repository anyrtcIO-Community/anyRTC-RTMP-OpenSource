//
//  ARLiveObjects.h
//  ARLiveKit
//
//  Created by 余生丶 on 2021/9/24.
//

#import <Foundation/Foundation.h>
#import <UIKit/UIKit.h>
#import "ARLiveEnumerates.h"

NS_ASSUME_NONNULL_BEGIN

/**
 * @brief 视频画布对象的属性。
 */
@interface ARLiveVideoCanvas : NSObject

/// 视频显示视窗
@property (strong, nonatomic) UIView* _Nullable view;
/// 视频显示模式
@property (assign, nonatomic) ARLiveRenderMode renderMode;

/**
 * 频道id
 * 0 ~ 9
 * a ~ z A ~Z
 * "!", "#", "$", "%", "&", "(", ")", "+", "-", ":", ";", "<", "=", ".", ">", "?", "@", "[", "]", "^", "_", "{", "}", "|", "~", ",".
 
** Note **
 * 该参数默认值为空字符 ""。如果用户是通过 ARtcEngineKit 类的 joinChannelByToken 方法加入频道的，则将参数设为默认值，表示该用户在频道内的渲染视图。
 * 如果用户是通过 ARtcChannel 类的 joinChannelByToken 方法加入频道的，则将该参数设为该 ARtcChannel 类对应的 channelId，表示该用户在该 channelId 对应频道内的渲染视图。
*/
@property (copy, nonatomic) NSString * _Nullable channelId;
/// 用户id
@property (copy, nonatomic) NSString * _Nonnull uid;
/** 视图镜像模式，详见 ARVideoMirrorMode
 
 **Note**:
  
 * 本地视图镜像模式：如果你使用前置摄像头，默认启动本地视图镜像模式；如果你使用后置摄像头，默认关闭本地视图镜像模式。
 * 远端用户视图镜像模式：默认关闭远端用户的镜像模式。
 */
@property (assign, nonatomic) ARLiveMirrorMode mirrorMode;

@end

@interface ARLivePlayConfig : NSObject

/** 默认值：true。
 * true：启用自动调整， SDK 将根据网络状况在一个范围内调整缓存时间；通过 setMaxAutoAdjustCacheTime 和 setMinAutoAdjustCacheTime 两个接口来进行设置。
 * false：关闭自动调整， SDK 将使用固定缓存时长；通过 setCacheTime(float) 来进行设置。
*/
@property (nonatomic, assign) BOOL bAuto;

/**
 * 设置播放器缓存时间，单位为秒，默认值为5秒。
 * 不建议设置过大，会影响秒开以及直播流播放的实时性。
*/
@property (nonatomic, assign) int nCacheTime;

/**
 * 默认值：1，单位为秒。
 * 仅在启用自动调用缓存时间接口时，有效。
 */
@property (nonatomic, assign) int nMinCacheTime;

/**
 * 默认值：5，单位为秒。
 * 仅在启用自动调用缓存时间接口时，有效。
*/
@property (nonatomic, assign) int nMaxCacheTime;

/**
 * 默认值：800，单位为毫秒。
 * 当渲染间隔超过此阈值时候，表明产生了卡顿；播放器会通过 IArLivePlayListener#onPlayEvent(int， Event) 回调 * PLAY_WARNING_VIDEO_PLAY_LAG 事件通知。
*/
@property (nonatomic, assign) int nVideoBlockThreshold;

/**
 * 默认值：3；取值范围：1 - 10。
 * 当 SDK 与服务器异常断开连接时，SDK 会尝试与服务器重连；您可通过此接口设置重连次数。
*/
@property (nonatomic, assign) int nConnectRetryCount;

/**
 * 默认值：3，单位为秒；取值范围：3 - 30。
 * 当 SDK 与服务器异常断开连接时， SDK 会尝试与服务器重连；您可通过此接口设置连续两次重连的时间间隔。
*/
@property (nonatomic, assign) int nConnectRetryInterval;

/**
 * 此参数在视频帧与消息需要高同步的情况使用，如：直播答题场景。
 * 接口说明：
 * 默认值：false。
 * 此接口需要搭配 ARLivePusher#sendMessageEx(byte[]) 使用。
 * 此接口存在一定的性能开销以及兼容性风险。
*/
@property (nonatomic, assign) BOOL bEnableMsg;

/// 播放器ID
@property (nonatomic, copy) NSString *strPlayerId;

@end

/**
 * @brief 视频编码器配置的属性
 */
@interface ARVideoDimensions : NSObject

/// 视频编码的分辨率宽 (px)，用于衡量编码质量
@property (nonatomic, assign) CGFloat width;

/// 视频编码的分辨率高 (px)，用于衡量编码质量
@property (nonatomic, assign) CGFloat height;

/// 视频编码的帧率（fps）
@property (nonatomic, assign) NSInteger fps;

@end

@interface ARLiveVideoEncoderParam : NSObject

///【字段含义】 视频分辨率
///【特别说明】如需使用竖屏分辨率，请指定 videoResolutionMode 为 Portrait，例如： 640 × 360 + Portrait = 360 × 640。
///【推荐取值】
/// - 桌面平台（Win + Mac）：建议选择 640 × 360 及以上分辨率，videoResolutionMode 选择 Landscape，即横屏分辨率。
@property(nonatomic, assign) ARLiveVideoResolution videoResolution;

///【字段含义】分辨率模式（横屏分辨率 or 竖屏分辨率）
///【推荐取值】桌面平台（Windows、Mac）建议选择 Landscape。
///【特别说明】如需使用竖屏分辨率，请指定 resMode 为 Portrait，例如： 640 × 360 + Portrait = 360 × 640。
@property(nonatomic, assign) ARLiveVideoResolutionMode videoResolutionMode;

///【字段含义】视频采集帧率
///【推荐取值】15fps 或 20fps。5fps 以下，卡顿感明显。10fps 以下，会有轻微卡顿感。20fps 以上，会浪费带宽（电影的帧率为 24fps）。
@property(nonatomic, assign) int videoFps;

///【字段含义】目标视频码率，SDK 会按照目标码率进行编码，只有在弱网络环境下才会主动降低视频码率。
///【推荐取值】请参考 ARLiveVideoResolution 在各档位注释的最佳码率，也可以在此基础上适当调高。
///           比如：ARLiveVideoResolution1280x720 对应 1200kbps 的目标码率，您也可以设置为 1500kbps 用来获得更好的观感清晰度。
///【特别说明】您可以通过同时设置 videoBitrate 和 minVideoBitrate 两个参数，用于约束 SDK 对视频码率的调整范围：
/// - 如果您将 videoBitrate 和 minVideoBitrate 设置为同一个值，等价于关闭 SDK 对视频码率的自适应调节能力。
@property(nonatomic, assign) int videoBitrate;

///【字段含义】最低视频码率，SDK 会在网络不佳的情况下主动降低视频码率以保持流畅度，最低会降至 minVideoBitrate 所设定的数值。
///【推荐取值】您可以通过同时设置 videoBitrate 和 minVideoBitrate 两个参数，用于约束 SDK 对视频码率的调整范围：
/// - 如果您将 videoBitrate 和 minVideoBitrate 设置为同一个值，等价于关闭 SDK 对视频码率的自适应调节能力。
@property(nonatomic, assign) int minVideoBitrate;

///【字段含义】视频编码时，采集图像与设置的编码大小不一致时，采用什么策略进行缩放裁剪
///【推荐取值】ArLiveVideoScaleModeAuto可以保证图像的完整性，但是大小会与设置的略有不同
@property(nonatomic, assign) ARLiveVideoScaleMode videoScaleMode;

- (instancetype _Nonnull)initWith:(ARLiveVideoResolution)resolution;

@end

/**
 * @brief 播放器的统计数据。
 */
@interface ARLivePlayerStatistics : NSObject

/// 【字段含义】当前 App 的 CPU 使用率（％）
@property(nonatomic, assign) NSUInteger appCpu;

/// 【字段含义】当前系统的 CPU 使用率（％）
@property(nonatomic, assign) NSUInteger systemCpu;

/// 【字段含义】视频宽度
@property(nonatomic, assign) NSUInteger width;

/// 【字段含义】视频高度
@property(nonatomic, assign) NSUInteger height;

/// 【字段含义】帧率（fps）
@property(nonatomic, assign) NSUInteger fps;

/// 【字段含义】视频码率（Kbps）
@property(nonatomic, assign) NSUInteger videoBitrate;

/// 【字段含义】音频码率（Kbps）
@property(nonatomic, assign) NSUInteger audioBitrate;

@end

/**
 * @brief 视频帧信息。
 *        ARLiveVideoFrame 用来描述一帧视频画面的裸数据，它可以是一帧编码前的画面，也可以是一帧解码后的画面。
 * @note  自定义采集和自定义渲染时使用。自定义采集时，需要使用 ARLiveVideoFrame 来包装待发送的视频帧；自定义渲染时，会返回经过 ARLiveVideoFrame 包装的视频帧。
 */
@interface ARLiveVideoFrame : NSObject

/// 【字段含义】视频帧像素格式
/// 【推荐取值】ARLivePixelFormatNV12
@property(nonatomic, assign) ARLivePixelFormat pixelFormat;

/// 【字段含义】视频数据包装格式
/// 【推荐取值】ARLiveBufferTypePixelBuffer
@property(nonatomic, assign) ARLiveBufferType bufferType;

/// 【字段含义】bufferType 为 ARLiveBufferTypeNSData 时的视频数据
@property(nonatomic, strong, nullable) NSData *data;

/// 【字段含义】bufferType 为 ARLiveBufferTypePixelBuffer 时的视频数据
@property(nonatomic, assign, nullable) CVPixelBufferRef pixelBuffer;

/// 【字段含义】视频宽度
@property(nonatomic, assign) NSUInteger width;

/// 【字段含义】视频高度
@property(nonatomic, assign) NSUInteger height;

/// 【字段含义】视频帧的顺时针旋转角度
@property(nonatomic, assign) ARLiveRotation rotation;

/// 【字段含义】视频纹理ID
@property(nonatomic, assign) GLuint textureId;

/// 【字段含义】视频rowSize
@property(nonatomic, assign) NSUInteger stride;

@end

/**
 * @brief 音频帧数据
 */
@interface ARLiveAudioFrame : NSObject

/// 【字段含义】音频数据
@property(nonatomic, nonnull) void *data;

/// 【字段含义】采样率
@property(nonatomic, assign) int sampleRate;

/// 【字段含义】声道数
@property(nonatomic, assign) int channel;

@end

/**
 * @brief 推流器的统计数据。
 */
@interface ARLivePusherStatistics : NSObject

/// 【字段含义】当前 App 的 CPU 使用率（％）
@property(nonatomic, assign) NSUInteger appCpu;

/// 【字段含义】当前系统的 CPU 使用率（％）
@property(nonatomic, assign) NSUInteger systemCpu;

/// 【字段含义】视频宽度
@property(nonatomic, assign) NSUInteger width;

/// 【字段含义】视频高度
@property(nonatomic, assign) NSUInteger height;

/// 【字段含义】帧率（fps）
@property(nonatomic, assign) NSUInteger fps;

/// 【字段含义】视频码率（Kbps）
@property(nonatomic, assign) NSUInteger videoBitrate;

/// 【字段含义】音频码率（Kbps）
@property(nonatomic, assign) NSUInteger audioBitrate;

@end


NS_ASSUME_NONNULL_END

//
//  ARLivePlayer.h
//  ARLiveKit
//
//  Created by 余生丶 on 2021/9/23.
//

#import <Foundation/Foundation.h>
#import "ARLiveObjects.h"
#import "ARLiveDelegate.h"
#import "ARLiveEnumerates.h"

NS_ASSUME_NONNULL_BEGIN

@interface ARLivePlayer : NSObject

- (instancetype)init NS_UNAVAILABLE;

/**
 * 设置播放器器回调。
 *
 * 通过设置回调，可以监听 ARLivePlayer 播放器的一些回调事件，
 * 包括播放器状态、音量回调、统计数据、警告和错误信息等。
 *
 * @param delegate 播放器器的回调目标对象
 */
- (void)setDelegate:(id<ARLivePlayDelegate>)delegate;

/**
 * 设置播放器的视频渲染 View。
 *
 * 该控件负责显示视频内容。
 *
 * @param renderView 视频渲染 View。
 *
 * @return 0方法调用成功，<0方法调用失败
 */
- (int)setRenderView:(UIView *_Nullable)renderView;

/**
 * 设置播放器画面的旋转角度。
 *
 * @param rotation 旋转角度
 *
 * @return 0方法调用成功，<0方法调用失败
 */
- (int)setRenderRotation:(ARLiveRotation)rotation;

/**
 * 设置画面的填充模式。
 *
 * @param mode 画面填充模式
 *
 * @return 0方法调用成功，<0方法调用失败
 */
- (int)setRenderFillMode:(ARLiveRenderMode)mode;

/**
 * 开始播放音视频流。
 *
 * @param url 音视频流的播放地址，支持 RTMP, HTTP-FLV, TRTC。
 * @return 0方法调用成功，<0方法调用失败
 */
- (int)startPlay:(NSString *_Nonnull)url;

/**
 * 停止播放音视频流。
 *
 * @return 0方法调用成功，<0方法调用失败
 */
- (int)stopPlay;

/**
 * 设置视频播放模式
 *
 * @param mode 播放模式，详见 ARLivePlayMode。
 * @return 0方法调用成功，<0方法调用失败
 */
- (int)setPlayMode:(ARLivePlayMode)mode;

/**
 * 播放器是否正在播放中。
 *
 * @return 是否正在播放
 *         - 1: 正在播放中
 *         - 0: 已经停止播放
 */
- (int)isPlaying;

/**
 * 仅适用于点播，直播或WebRTC等实时流不会生效。
 *
 * @param seekTimeS 播放进度，单位秒
 * @return 0方法调用成功，<0方法调用失败
 */
- (int)seekTo:(int)seekTimeS;

/**
 * 倍速播放
 * 仅适用于点播，直播或WebRTC等实时流不会生效。
 *
 * @param speed 播放速度，0.5   0.75  1.0  1.25  1.5 1.75 2.0 3.0
 * @return 0方法调用成功，<0方法调用失败
 */
- (int)setSpeed:(CGFloat)speed;

/**
 * 重新开始播放。一般用于点播场景
 *
 * @return 0方法调用成功，<0方法调用失败
 */
- (int)replay;

/**
 * 暂停播放器的音频流。
 *
 * @return 0方法调用成功，<0方法调用失败
 */
- (int)pauseAudio;

/**
 * 恢复播放器的音频流。
 *
 * @return 0方法调用成功，<0方法调用失败
 */
- (int)resumeAudio;

/**
 * 暂停播放器的视频流。
 *
 * @return 0方法调用成功，<0方法调用失败
 */
- (int)pauseVideo;

/**
 * 恢复播放器的视频流。
 *
 * @return 0方法调用成功，<0方法调用失败
 */
- (int)resumeVideo;

/**
 * 设置播放器音量。
 *
 * @param volume 音量大小，取值范围0 - 100。【默认值】: 100
 * @return 0方法调用成功，<0方法调用失败
 */
- (int)setPlayoutVolume:(NSUInteger)volume;

/**
 * 设置播放器缓存自动调整的最小和最大时间 ( 单位：秒 )。
 *
 * @param minTime 缓存自动调整的最小时间，取值需要大于0。【默认值】：1
 * @param maxTime 缓存自动调整的最大时间，取值需要大于0。【默认值】：5
 * @return 0方法调用成功，<0方法调用失败
 */
- (int)setCacheParams:(CGFloat)minTime maxTime:(CGFloat)maxTime;

/**
 * 启用播放音量大小提示。
 *
 * 开启后可以在 [onPlayoutVolumeUpdate](@ref ARLivePlayDelegate#onPlayoutVolumeUpdate:volume:) 回调中获取到 SDK 对音量大小值的评估。
 *
 * @param intervalMs 决定了 onPlayoutVolumeUpdate 回调的触发间隔，单位为ms，最小间隔为100ms，如果小于等于0则会关闭回调，建议设置为300ms；【默认值】：0，不开启
 * @return 0方法调用成功，<0方法调用失败
 */
- (int)enableVolumeEvaluation:(NSUInteger)intervalMs;

/**
 * 截取播放过程中的视频画面。
 *
 * @return 0方法调用成功，<0方法调用失败
 */
- (int)snapshot;

/**
 * 设置视频自定义渲染回调。
 *
 * 通过该方法，可以获取解码后的每一帧视频画面，进行自定义渲染处理，添加自定义显示效果。
 *
 * @param enable      是否开启自定义渲染。【默认值】：NO
 * @param pixelFormat 自定义渲染回调的视频像素格式 {@link ARLivePixelFormat}。
 * @param bufferType  自定义渲染回调的视频数据格式 {@link ARLiveBufferType}。
 * @return  0方法调用成功，<0方法调用失败
 */
- (int)enableCustomRendering:(BOOL)enable pixelFormat:(ARLivePixelFormat)pixelFormat bufferType:(ARLiveBufferType)bufferType;

/**
 * 开启接收 SEI 消息
 *
 * @param enable      true: 开启接收 SEI 消息; false: 关闭接收 SEI 消息。【默认值】: false
 * @param payloadType 指定接收 SEI 消息的 payloadType，支持 5、242，请与发送端的 payloadType 保持一致。
 *
 * @return  0方法调用成功，<0方法调用失败
 */
- (int)enableReceiveSeiMessage:(BOOL)enable payloadType:(int)payloadType;

/**
 * 是否显示播放器状态信息的调试浮层。
 *
 * @param isShow 是否显示。【默认值】：NO
 */
- (void)showDebugView:(BOOL)isShow;

@end

NS_ASSUME_NONNULL_END

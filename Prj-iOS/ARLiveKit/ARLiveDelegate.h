//
//  ARLiveDelegate.h
//  ARLiveKit
//
//  Created by 余生丶 on 2021/9/23.
//

#ifndef ARLiveDelegate_h
#define ARLiveDelegate_h

#import "ARLiveEnumerates.h"
#import "ARLiveObjects.h"

@protocol ARLiveEngineDelegate <NSObject>

@end

@protocol ARLivePushDelegate <NSObject>

@optional

/**
 * 直播推流器错误通知，推流器出现错误时，会回调该通知
 *
 * @param code      错误码 {@link ARLiveCode}
 * @param msg       错误信息
 * @param extraInfo 扩展信息
 */
- (void)onError:(ARLiveCode)code message:(NSString *_Nullable)msg extraInfo:(NSDictionary *_Nullable)extraInfo;

/**
 * 直播推流器警告通知
 *
 * @param code      警告码 {@link ARLiveCode}
 * @param msg       警告信息
 * @param extraInfo 扩展信息
 */
- (void)onWarning:(ARLiveCode)code message:(NSString *_Nullable)msg extraInfo:(NSDictionary *_Nullable)extraInfo;

/**
 * 首帧音频采集完成的回调通知
 */
- (void)onCaptureFirstAudioFrame;

/**
 * 首帧视频采集完成的回调通知
 */
- (void)onCaptureFirstVideoFrame;

/**
 * 麦克风采集音量值回调
 *
 * @param volume 音量大小
 * @note  调用 [enableVolumeEvaluation](@ref ARLivePusher#enableVolumeEvaluation:) 开启采集音量大小提示之后，会收到这个回调通知。
 */
- (void)onMicrophoneVolumeUpdate:(NSInteger)volume;

/**
 * 推流器连接状态回调通知
 *
 * @param status    推流器连接状态 {@link ARLivePushStatus}
 * @param msg       连接状态信息
 * @param extraInfo 扩展信息
 */
- (void)onPushStatusUpdate:(ARLivePushStatus)status message:(NSString *_Nullable)msg extraInfo:(NSDictionary *_Nullable)extraInfo;

/**
 * 直播推流器统计数据回调
 *
 * @param statistics 推流器统计数据 {@link ARLivePusherStatistics}
 */
- (void)onStatisticsUpdate:(ARLivePusherStatistics *_Nonnull)statistics;

/**
 * 截图回调
 *
 * @param image 已截取的视频画面
 * @note 调用 [snapshot](@ref ARLivePusher#snapshot) 截图之后，会收到这个回调通知
 */
- (void)onSnapshotComplete:(UIImage *_Nonnull)image;

/**
 * 自定义视频处理回调
 *
 * @note 需要调用 [enableCustomVideoProcess](@ref ARLivePusher#enableCustomVideoProcess:pixelFormat:bufferType:)
 *       开启自定义视频处理，才会收到这个回调通知。
 *
 * 【情况一】美颜组件会产生新的纹理
 * 如果您使用的美颜组件会在处理图像的过程中产生一帧全新的纹理（用于承载处理后的图像），那请您在回调函数中将 dstFrame.textureId 设置为新纹理的 ID。
 * <pre>
 *   - (void) onProcessVideoFrame:(ARLiveVideoFrame * _Nonnull)srcFrame dstFrame:(ARLiveVideoFrame * _Nonnull)dstFrame
 *   {
 *       GLuint dstTextureId = renderItemWithTexture(srcFrame.textureId, srcFrame.width, srcFrame.height);
 *       dstFrame.textureId = dstTextureId;
 *       return 0;
 *   }
 * </pre>
 *
 * 【情况二】美颜组件并不自身产生新纹理
 * 如果您使用的第三方美颜模块并不生成新的纹理，而是需要您设置给该模块一个输入纹理和一个输出纹理，则可以考虑如下方案：
 * <pre>
 *   - (void) onProcessVideoFrame:(ARLiveVideoFrame * _Nonnull)srcFrame dstFrame:(ARLiveVideoFrame * _Nonnull)dstFrame
 *   {
 *       thirdparty_process(srcFrame.textureId, srcFrame.width, srcFrame.height, dstFrame.textureId);
 *       return 0;
 *   }
 * </pre>
 *
 * @param srcFrame 用于承载未处理的视频画面
 * @param dstFrame 用于承载处理过的视频画面
 */
- (void)onProcessVideoFrame:(ARLiveVideoFrame *_Nonnull)srcFrame dstFrame:(ARLiveVideoFrame *_Nonnull)dstFrame;

/**
 * SDK 内部的 OpenGL 环境的销毁通知
 */
- (void)onGLContextDestroyed;

/**
 * 设置云端的混流转码参数的回调，对应于 [setMixTranscodingConfig](@ref ARLivePusher#setMixTranscodingConfig:) 接口
 *
 * @param code 0表示成功，其余值表示失败
 * @param msg 具体错误原因
 */
- (void)onSetMixTranscodingConfig:(ARLiveCode)code message:(NSString *_Nullable)msg;

@end

@class ARLivePlayer;

@protocol ARLivePlayDelegate <NSObject>

@optional

/**
 * 直播播放器错误通知，播放器出现错误时，会回调该通知
 *
 * @param player    回调该通知的播放器对象
 * @param code      错误码 {@link ARLiveCode}
 * @param msg       错误信息
 * @param extraInfo 扩展信息
 */
- (void)onError:(ARLivePlayer *_Nonnull)player code:(ARLiveCode)code message:(NSString *_Nullable)msg extraInfo:(NSDictionary *_Nullable)extraInfo;

/**
 * 直播播放器警告通知
 *
 * @param player    回调该通知的播放器对象
 * @param code      警告码 {@link ARLiveCode}
 * @param msg       警告信息
 * @param extraInfo 扩展信息
 */
- (void)onWarning:(ARLivePlayer *_Nonnull)player code:(ARLiveCode)code message:(NSString *_Nullable)msg extraInfo:(NSDictionary *_Nullable)extraInfo;

/**
 * 直播播放器视频状态变化通知
 *
 * @param player    回调该通知的播放器对象
 * @param status    状态码 {@link ARLivePlayStatus}
 * @param reason    状态对应的原因 {@link ARLiveStatusChangeReason}
 * @param extraInfo 扩展信息
 */
- (void)onVideoPlayStatusUpdate:(ARLivePlayer *_Nonnull)player status:(ARLivePlayStatus)status reason:(ARLiveStatusChangeReason)reason extraInfo:(NSDictionary *_Nullable)extraInfo;

/**
 * 直播播放器音频状态变化通知
 *
 * @param player    回调该通知的播放器对象
 * @param status    状态码 {@link ARLivePlayStatus}
 * @param reason    状态对应的原因 {@link ARLiveStatusChangeReason}
 * @param extraInfo 扩展信息
 */
- (void)onAudioPlayStatusUpdate:(ARLivePlayer *_Nonnull)player status:(ARLivePlayStatus)status reason:(ARLiveStatusChangeReason)reason extraInfo:(NSDictionary *_Nullable)extraInfo;

/**
 * 播放器音量大小回调
 *
 * @param player 回调该通知的播放器对象
 * @param volume 音量大小
 * @note  调用 [enableVolumeEvaluation](@ref ARLivePlayer#enableVolumeEvaluation:) 开启播放音量大小提示之后，会收到这个回调通知。
 */
- (void)onPlayoutVolumeUpdate:(ARLivePlayer *_Nonnull)player volume:(NSInteger)volume;

/**
 * 播放器点播的进度回调
 *
 * @param player 回调该通知的播放器对象
 * @param allDuration 总时长，单位毫秒
 * @param currentPlaybackTime 当前已播放时长，单位毫秒
 * @param bufferDuration 已缓冲时长，单位毫秒
 * @note  调用 [enableVolumeEvaluation](@ref ARLivePlayer#enableVolumeEvaluation:) 开启播放音量大小提示之后，会收到这个回调通知。
 */
- (void)onVodPlaybackProcess:(ARLivePlayer *_Nonnull)player allDuration:(NSInteger)allDuration playTime:(NSInteger)currentPlaybackTime bufferDuration:(NSInteger)bufferDuration;

/**
 * 直播播放器统计数据回调
 *
 * @param player     回调该通知的播放器对象
 * @param statistics 播放器统计数据 {@link ARLivePlayerStatistics}
 */
- (void)onStatisticsUpdate:(ARLivePlayer *_Nonnull)player statistics:(ARLivePlayerStatistics *_Nullable)statistics;

/**
 * 截图回调
 *
 * @note  调用 [snapshot](@ref ARLivePlayer#snapshot) 截图之后，会收到这个回调通知
 * @param player 回调该通知的播放器对象
 * @param image  已截取的视频画面
 */
- (void)onSnapshotComplete:(ARLivePlayer *_Nonnull)player image:(UIImage *_Nonnull)image;

/**
 * 自定义视频渲染回调
 *
 * @param player     回调该通知的播放器对象
 * @param videoFrame 视频帧数据 {@link ARLiveVideoFrame}
 * @note  调用 [enableCustomRendering](@ref ARLivePlayer#enableCustomRendering:pixelFormat:bufferType:) 开启自定义渲染之后，会收到这个回调通知
 */
- (void)onRenderVideoFrame:(ARLivePlayer *_Nonnull)player frame:(ARLiveVideoFrame *_Nullable)videoFrame;

/**
 * 收到 SEI 消息的回调，发送端通过 {@link ARLivePusher} 中的 `sendSeiMessage` 来发送 SEI 消息。
 *
 * @note  调用 {@link ARLivePlayer} 中的 `enableReceiveSeiMessage` 开启接收 SEI 消息之后，会收到这个回调通知
 *
 * @param player   回调该通知的播放器对象。
 * @param payloadType    回调数据的SEI payloadType
 * @param data     数据
 */
- (void)onReceiveSeiMessage:(ARLivePlayer *_Nonnull)player payloadType:(int)payloadType data:(NSData *_Nullable)data;

@end

#endif /* ARLiveDelegate_h */

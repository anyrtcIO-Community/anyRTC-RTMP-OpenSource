//
//  ARLivePusher.h
//  ARLiveKit
//
//  Created by 余生丶 on 2021/9/23.
//

#import <Foundation/Foundation.h>
#import <UIKit/UIKit.h>
#import "ARLiveDelegate.h"
#import "ARLiveEnumerates.h"
#import "ARLiveObjects.h"

NS_ASSUME_NONNULL_BEGIN

@interface ARLivePusher : NSObject

- (instancetype)init NS_UNAVAILABLE;

/**
 * 设置推流器回调。
 *
 * 通过设置回调，可以监听 ARLivePusher 推流器的一些回调事件，
 * 包括推流器状态、音量回调、统计数据、警告和错误信息等。
 *
 * @param delegate 推流器的回调目标对象
 */
- (void)setDelegate:(id<ARLivePushDelegate>)delegate;

/**
 * 设置本地摄像头预览 View
 *
 * 本地摄像头采集到的画面，经过美颜、脸形调整、滤镜等多种效果叠加之后，最终会显示到传入的 View 上。
 *
 * @param renderView 通过 ARLiveVideoCanvas 设置本地视频显示属性。
 *
 * @return 0方法调用成功，<0方法调用失败
 */
- (int)setupCameraRender:(UIView *_Nullable)renderView;

/**
 * 设置本地摄像头预览镜像
 * 本地摄像头分为前置摄像头和后置摄像头，系统默认情况下，是前置摄像头镜像，后置摄像头不镜像，这里可以修改前置后置摄像头的默认镜像类型。
 *
 * @param mode 摄像头镜像类型。
 *
 * @return 0方法调用成功，<0方法调用失败
 */
- (int)setCameraRenderMirror:(ARLiveMirrorMode)mode;

/**
 * 设置本地摄像头预览画面的旋转角度。
 * 只旋转本地预览画面，不影响推流出去的画面。
 *
 * @param rotation 预览画面的旋转角度。
 *
 * @return 0方法调用成功，<0方法调用失败
 */
- (int)setCameraRenderRotation:(ARLiveRotation)rotation;

/**
 * 设置画面的填充模式。
 *
 * @param mode 画面填充模式
 *
 * @return 0方法调用成功，<0方法调用失败
 */
- (int)setRenderFillMode:(ARLiveRenderMode)mode;

/**
 * 打开本地摄像头
 *
 * @param frontCamera YES 前置摄像头，NO 后置摄像头
 *
 * @return 0方法调用成功，<0方法调用失败
 */
- (int)startCamera:(BOOL)frontCamera;

/**
 * 关闭本地摄像头
 *
 * @return 0方法调用成功，<0方法调用失败
 */
- (int)stopCamera;

/**
 * 打开麦克风
 *
 * @return 0方法调用成功，<0方法调用失败
 */
- (int)startMicrophone;

/**
 * 关闭麦克风
 *
 * @return 0方法调用成功，<0方法调用失败
 */
- (int)stopMicrophone;

/**
 * 开启图片推流。
 *
 * @param image 图片
 *
 * @note startVirtualCamera，startCamera，startScreenCapture，同一 Pusher 实例下，仅有一个能上行，三者为覆盖关系。例如先调用 startCamera，后调用 startVirtualCamera。此时表现为暂停摄像头推流，开启图片推流
 *
 * @return 0方法调用成功，<0方法调用失败
 */
- (int)startVirtualCamera:(UIImage *)image;

/**
 * 关闭图片推流。
 *
 * @return 0方法调用成功，<0方法调用失败
 */
- (int)stopVirtualCamera;

/**
 * 开始全系统的屏幕分享（该接口支持 iOS 11.0 及以上的 iPhone 和 iPad）。
 *
 * @note 该接口支持共享整个 iOS 系统的屏幕，可以实现类似腾讯会议的全系统级的屏幕分享。<br/>
 *         需要通过 iOS Broadcast Upload Extension 来开启屏幕采集，
 *         然后设置 [enableCustomVideoCapture](@ref ArLivePusher#enableCustomVideoCapture:) 开启自定义采集支持。
 *         最后通过 [sendCustomVideoFrame](@ref ArLivePusher#sendCustomVideoFrame:) 把 Broadcast Upload Extension 中采集到的屏幕画面送出去。
 *
 * @note startVirtualCamera，startCamera，startScreenCapture，同一 Pusher 实例下，仅有一个能上行，三者为覆盖关系。例如先调用 startCamera，后调用 startVirtualCamera。此时表现为暂停摄像头推流，开启图片推流
 *
 * @param appGroup 主 App 与 Broadcast 共享的 Application Group Identifier，可以指定为 nil，但按照文档设置会使功能更加可靠。
 *
 * @return 0方法调用成功，<0方法调用失败
 */
- (int)startScreenCapture:(NSString *_Nonnull)appGroup;

/**
 * 关闭屏幕采集。
 *
 * @note iOS 端暂不支持通过此接口关闭屏幕采集。<br/>
 *         可以直接在 iOS 系统的屏幕录制界面上关闭屏幕录制，
 * @return 0方法调用成功，<0方法调用失败
 */
- (int)stopScreenCapture;

/**
 * 设置视频编码镜像。
 *
 * @note  编码镜像只影响观众端看到的视频效果。
 * @param mirror 是否镜像
 *         - NO【默认值】: 播放端看到的是非镜像画面
 *         - YES: 播放端看到的是镜像画面
 * @return 0方法调用成功，<0方法调用失败
 */
- (int)setEncoderMirror:(BOOL)mirror;

/**
 * 静音本地音频。
 *
 * 静音本地音频后，SDK 不会继续采集麦克风的声音
 * 与 stopMicrophone 不同之处在于 pauseAudio 并不会停止发送音频数据，而是继续发送码率极低的静音包
 * 由于 MP4 等视频文件格式，对于音频的连续性是要求很高的，使用 stopMicrophone 会导致录制出的 MP4 不易播放
 * 因此在对录制质量要求很高的场景中，建议选择 pauseAudio，从而录制出兼容性更好的 MP4 文件
 *
 * @return 0方法调用成功，<0方法调用失败
 */
- (int)pauseAudio;

/**
 * 取消静音本地音频。
 *
 * @return 0方法调用成功，<0方法调用失败
 */
- (int)resumeAudio;

/**
 * 暂停推流器的视频流。
 *
 * @return 0方法调用成功，<0方法调用失败
 */
- (int)pauseVideo;

/**
 * 恢复推流器的视频流。
 *
 * @return 0方法调用成功，<0方法调用失败
 */
- (int)resumeVideo;

/**
 * 开始音视频数据推流。
 *
 * @param url 推流的目标地址，支持任意推流服务端
 * @return 0方法调用成功，<0方法调用失败
 */
- (int)startPush:(NSString *_Nonnull)url;

/**
 * 停止推送音视频数据。
 *
 * @return 0方法调用成功，<0方法调用失败
 */
- (int)stopPush;

/**
 * 当前推流器是否正在推流中。
 *
 * @return 是否正在推流
 *         - 1: 正在推流中
 *         - 0: 已经停止推流
 */
- (int)isPushing;

/**
 * 设置推流音频质量。
 *
 * @param quality 音频质量 {@link ARLiveAudioQuality}
 *         - ARLiveAudioQualityDefault 【默认值】: 通用
 *         - ARLiveAudioQualitySpeech: 语音
 *         - ARLiveAudioQualityMusic:  音乐
 * @return 0方法调用成功，<0方法调用失败
 */
- (int)setAudioQuality:(ARLiveAudioQuality)quality;

/**
 * 设置推流视频编码参数
 *
 * @param param  视频编码参数 {@link ARLiveVideoEncoderParam}
 * @return 0方法调用成功，<0方法调用失败
 */
- (int)setVideoQuality:(ARLiveVideoEncoderParam *)param;

/**
 * 美颜开关
 *
 * @param enable YES打开， NO关闭，默认关闭
 */
- (int)setBeautyEffect:(BOOL)enable;

/**
 * 截取推流过程中的本地画面。
 *
 * @return 0方法调用成功，<0方法调用失败
 */
- (int)snapshot;

/**
 * 设置推流器水印。默认情况下，水印不开启。
 *
 * @param image 水印图片。如果该值为 nil，则等效于禁用水印
 * @param x     水印的横坐标，取值范围为0 - 1的浮点数。
 * @param y     水印的纵坐标，取值范围为0 - 1的浮点数。
 * @param scale 水印图片的缩放比例，取值范围为0 - 1的浮点数。
 * @return 0方法调用成功，<0方法调用失败
 */
- (int)setWatermark:(UIImage *)image x:(float)x y:(float)y scale:(float)scale;

/**
 * 启用采集音量大小提示。
 *
 * 开启后可以在 [onMicrophoneVolumeUpdate](@ref ARLivePushDelegate#onMicrophoneVolumeUpdate:) 回调中获取到 SDK 对音量大小值的评估。
 * @param intervalMs 决定了音量大小回调的触发间隔，单位为 ms，最小间隔为 100ms，如果小于等于0则会关闭回调，建议设置为 300ms。【默认值】：0，不开启
 * @return 0方法调用成功，<0方法调用失败
 */
- (int)enableVolumeEvaluation:(NSUInteger)intervalMs;

/**
 * 开启/关闭自定义视频采集。
 *
 * 在自定义视频采集模式下，SDK 不再从摄像头采集图像，只保留编码和发送能力。
 * @note  需要在 [startPush](@ref ArLivePusher#startPush:) 之前调用，才会生效。
 * @param enable YES：开启自定义采集；NO：关闭自定义采集。【默认值】：NO
 * @return 0方法调用成功，<0方法调用失败
 */
- (int)enableCustomVideoCapture:(BOOL)enable;

/**
 * 开启/关闭自定义音频采集
 *
 *  @brief 开启/关闭自定义音频采集。<br/>
 *         在自定义音频采集模式下，SDK 不再从麦克风采集声音，只保留编码和发送能力。
 *  @note   需要在 [startPush]({@link ArLivePusher#startPush(String)}) 前调用才会生效。
 *  @param enable YES: 开启自定义采集; NO: 关闭自定义采集。【默认值】: NO
 *  @return 0方法调用成功，<0方法调用失败
 */
- (int)enableCustomAudioCapture:(BOOL)enable;

/**
 * 在自定义视频采集模式下，将采集的视频数据发送到SDK。
 *
 * 在自定义视频采集模式下，SDK不再采集摄像头数据，仅保留编码和发送功能。
 * 您可以把采集到的 SampleBuffer 打包到 ArLiveVideoFrame 中，然后通过该API定期的发送。
 *
 * @note  需要在 [startPush](@ref ArLivePusher#startPush:) 之前调用 [enableCustomVideoCapture](@ref ArLivePusher#enableCustomVideoCapture:) 开启自定义采集。
 * @param videoFrame 向 SDK 发送的 视频帧数据 {@link ArLiveVideoFrame}
 * @return 0方法调用成功，<0方法调用失败
 */
- (int)sendCustomVideoFrame:(ARLiveVideoFrame *)videoFrame;

/**
 * 在自定义音频采集模式下，将采集的音频数据发送到SDK
 *
 *  @brief 在自定义音频采集模式下，将采集的音频数据发送到SDK，SDK不再采集麦克风数据，仅保留编码和发送功能。
 *  @note   需要在 [startPush]({@link ArLivePusher#startPush(String)}) 之前调用  {@link ArLivePusher#enableCustomAudioCapture(boolean)} 开启自定义采集。
 *  @param audioFrame 向 SDK 发送的 音频帧数据 {@link ArLiveAudioFrame}
 *  @return 0方法调用成功，<0方法调用失败
 */
- (int)sendCustomAudioFrame:(ARLiveAudioFrame *)audioFrame;

/**
 * 发送 SEI 消息
 *
 * 播放端 {@link ARLivePlayer} 通过 {@link ARLivePushDelegate} 中的  `onReceiveSeiMessage` 回调来接收该消息。
 *
 * @param payloadType 数据类型，支持 5、242。推荐填：242
 * @param data        待发送的数据
 * @return 0方法调用成功，<0方法调用失败
 */
- (int)sendSeiMessage:(int)payloadType data:(NSData *)data;

/**
 * 显示仪表盘。
 *
 * @param isShow 是否显示。【默认值】：NO
 */
- (void)showDebugView:(BOOL)isShow;

@end

NS_ASSUME_NONNULL_END

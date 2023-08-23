//
//  ARLiveEnumerates.h
//  ARLiveKit
//
//  Created by 余生丶 on 2021/9/27.
//

#ifndef ARLiveEnumerates_h
#define ARLiveEnumerates_h

/**
 * @brief 直播推拉流 SDK 错误码。
 */
typedef NS_ENUM(NSInteger, ARLiveCode) {

    /// 没有错误
    ARLive_OK = 0,

    /// 暂未归类的通用错误
    ARLive_ERROR_FAILED = -1,

    /// 调用 API 时，传入的参数不合法
    ARLive_ERROR_INVALID_PARAMETER = -2,

    /// API 调用被拒绝
    ARLive_ERROR_REFUSED = -3,

    /// 当前 API 不支持调用
    ARLive_ERROR_NOT_SUPPORTED = -4,

    /// license 不合法，调用失败
    ARLive_ERROR_INVALID_LICENSE = -5,

    /// 请求服务器超时
    ARLive_ERROR_REQUEST_TIMEOUT = -6,

    /// 服务器无法处理您的请求
    ARLive_ERROR_SERVER_PROCESS_FAILED = -7,

    /// 连接断开
    ARLive_ERROR_DISCONNECTED = -8,

    /// 网络状况不佳：上行带宽太小，上传数据受阻
    ARLive_WARNING_NETWORK_BUSY = 1101,

    /// 当前视频播放出现卡顿
    ARLive_WARNING_VIDEO_BLOCK = 2105,

    /// 摄像头打开失败
    ARLive_WARNING_CAMERA_START_FAILED = -1301,

    /// 摄像头正在被占用中，可尝试打开其他摄像头
    ARLive_WARNING_CAMERA_OCCUPIED = -1316,

    /// 摄像头设备未授权，通常在移动设备出现，可能是权限被用户拒绝了
    ARLive_WARNING_CAMERA_NO_PERMISSION = -1314,

    /// 麦克风打开失败
    ARLive_WARNING_MICROPHONE_START_FAILED = -1302,

    /// 麦克风正在被占用中，例如移动设备正在通话时，打开麦克风会失败
    ARLive_WARNING_MICROPHONE_OCCUPIED = -1319,

    /// 麦克风设备未授权，通常在移动设备出现，可能是权限被用户拒绝了
    ARLive_WARNING_MICROPHONE_NO_PERMISSION = -1317,

    /// 当前系统不支持屏幕分享
    ARLive_WARNING_SCREEN_CAPTURE_NOT_SUPPORTED = -1309,

    /// 开始录屏失败，如果在移动设备出现，可能是权限被用户拒绝了
    ARLive_WARNING_SCREEN_CAPTURE_START_FAILED = -1308,

    /// 录屏被系统中断
    ARLive_WARNING_SCREEN_CAPTURE_INTERRUPTED = -7001,
};

/**
 * @brief 视频显示模式。
 */
typedef NS_ENUM(NSUInteger, ARLiveRenderMode ) {
  /**
   1:优先保证视窗被填满。视频尺寸等比缩放，直至整个视窗被视频填满。如果视频长宽与显示窗口不同，则视频流会按照显示视窗的比例进行周边裁剪或图像拉伸后填满视窗。
   */
    ARLiveRenderModeHidden = 1,
  /**
   2:优先保证视频内容全部显示。视频尺寸等比缩放，直至视频窗口的一边与视窗边框对齐。如果视频尺寸与显示视窗尺寸不一致，在保持长宽比的前提下，将视频进行缩放后填满视窗，缩放后的视频四周会有一圈黑边。
   */
    ARLiveRenderModeFit = 2,
   /**
   4:视频尺寸进行缩放和拉伸以充满显示视窗。
   */
    ARLiveRenderModeFill = 4
};

/**
 * @brief 视频镜像模式。
 */
typedef NS_ENUM(NSUInteger, ARLiveMirrorMode ) {
    
    /// 0: (Default) 由 SDK 决定镜像模式
    ARLiveMirrorModeAuto = 0,

    /// 1: 启用镜像模式
    ARLiveMirrorModeEnabled = 1,

    /// 2: 关闭镜像模式
    ARLiveMirrorModeDisabled = 2
};

/**
 * @brief 播放类型 。
 */
typedef NS_ENUM(NSInteger, ARLivePlayerType ) {

    /// 0：使用Rtmp开源内核播放
    ARLivePlayerTypeDefault = 0,

    /// 1：使用Player内核播放
    ARLivePlayerTypeCustom = 1
};

/**
 * @brief 设置视频质量 。
 */
typedef NS_ENUM(NSInteger, ARLiveDimensionsType ) {
    /**
    0：
    */
    ARLiveDimensionsTypeDefault = 0,
};

/**
 * @brief 编码的视频方向 。
 */
typedef NS_ENUM(NSInteger, ARLiveOrientationMode ) {
    /**
    0：
    */
    ARLiveOrientationModeDefault = 0,
};

/**
 * @brief 选择高码率高分辨率视频或低码率低分辨率视频。
 */
typedef NS_ENUM(NSInteger, ARLiveStreamType ) {
    
    /// 0:高码率、高分辨率视频
    ARLiveStreamTypeHigh = 0,
    
    /// 1:低码率、低分辨率视频
    ARLiveStreamTypeLow = 1
};

/**
 * @brief 预览画面的旋转角度。
 */
typedef NS_ENUM(NSInteger, ARLiveRotation) {

    /// 0:  不旋转，默认
    ARLiveRotation0 = 0,

    /// 1: 顺时针旋转90度
    ARLiveRotation90 = 1,
    
    /// 2:  顺时针旋转180度
    ARLiveRotation180 = 2,

    /// 3:  顺时针旋转270度
    ARLiveRotation270 = 3
};

/**
 * @brief 音频质量。
 */
typedef NS_ENUM(NSInteger, ARLiveAudioQuality) {

    /// 0: 【默认值】: 通用
    ARLiveAudioQualityDefault = 0,
    
    /// 1: 语音
    ARLiveAudioQualitySpeech = 1,
    
    /// 2: 音乐
    ARLiveAudioQualityMusic = 2
};

/**
 * @brief 视频帧的像素格式。
 */
typedef NS_ENUM(NSInteger, ARLivePixelFormat) {

    /// 未知
    ARLivePixelFormatUnknown,

    /// YUV420P I420
    ARLivePixelFormatI420,

    /// YUV420SP NV12
    ARLivePixelFormatNV12,

    /// BGRA8888
    ARLivePixelFormatBGRA32,

    /// OpenGL 2D 纹理
    ARLivePixelFormatTexture2D

};

/**
 * @brief 视频数据包装格式。
 *
 * @note 在自定义采集和自定义渲染功能，您需要用到下列枚举值来指定您希望以什么样的格式来包装视频数据。
 * - PixelBuffer：直接使用效率最高，iOS 系统提供了众多 API 获取或处理 PixelBuffer
 * - NSData：     当使用自定义渲染时，PixelBuffer拷贝一次到NSData。当使用自定义采集时，NSData拷贝一次到PixelBuffer。因此，性能会受到一定程度的影响
 */
typedef NS_ENUM(NSInteger, ARLiveBufferType) {

    /// 未知
    ARLiveBufferTypeUnknown,

    /// 直接使用效率最高，iOS 系统提供了众多 API 获取或处理 PixelBuffer
    ARLiveBufferTypePixelBuffer,

    /// 会有一定的性能消耗，SDK 内部是直接处理 PixelBuffer 的，所以会存在 NSData 和 PixelBuffer 之间类型转换所产生的内存拷贝开销
    ARLiveBufferTypeNSData,

    /// 直接操作纹理 ID，性能最好
    ARLiveBufferTypeTexture
};

/**
 * @brief 直播流的播放状态。
 */
typedef NS_ENUM(NSInteger, ARLivePlayStatus) {

    /// 播放停止
    ARLivePlayStatusStopped,
    
    /// 正在播放
    ARLivePlayStatusPlaying,
    
    /// 正在缓冲（首次加载不会抛出 Loading 事件）
    ARLivePlayStatusLoading
};

/**
 * @brief 直播流的播放状态变化原因
 */
typedef NS_ENUM(NSInteger, ARLiveStatusChangeReason) {

    /// 内部原因
    ARLiveStatusChangeReasonInternal,

    /// 开始网络缓冲
    ARLiveStatusChangeReasonBufferingBegin,

    /// 结束网络缓冲
    ARLiveStatusChangeReasonBufferingEnd,

    /// 本地启动播放
    ARLiveStatusChangeReasonLocalStarted,

    /// 本地停止播放
    ARLiveStatusChangeReasonLocalStopped,

    /// 远端可播放
    ARLiveStatusChangeReasonRemoteStarted,

    /// 远端流停止或中断
    ARLiveStatusChangeReasonRemoteStopped,

    /// 远端流离线
    ARLiveStatusChangeReasonRemoteOffline,
};

/**
 * @brief 直播流的连接状态。
 */
typedef NS_ENUM(NSInteger, ARLivePushStatus) {

    /// 与服务器断开连接
    ARLivePushStatusDisconnected,

    /// 正在连接服务器
    ARLivePushStatusConnecting,

    /// 连接服务器成功
    ARLivePushStatusConnectSuccess,

    /// 重连服务器中
    ARLivePushStatusReconnecting
};

/**
 * @brief 视频分辨率
 */
typedef NS_ENUM(NSInteger, ARLiveVideoResolution) {

    /// 分辨率 160*160，码率范围：100Kbps ~ 150Kbps，帧率：15fps
    ARLiveVideoResolution160x160,

    /// 分辨率 270*270，码率范围：200Kbps ~ 300Kbps，帧率：15fps
    ARLiveVideoResolution270x270,

    /// 分辨率 480*480，码率范围：350Kbps ~ 525Kbps，帧率：15fps
    ARLiveVideoResolution480x480,

    /// 分辨率 320*240，码率范围：250Kbps ~ 375Kbps，帧率：15fps
    ARLiveVideoResolution320x240,

    /// 分辨率 480*360，码率范围：400Kbps ~ 600Kbps，帧率：15fps
    ARLiveVideoResolution480x360,

    /// 分辨率 640*480，码率范围：600Kbps ~ 900Kbps，帧率：15fps
    ARLiveVideoResolution640x480,

    /// 分辨率 320*180，码率范围：250Kbps ~ 400Kbps，帧率：15fps
    ARLiveVideoResolution320x180,

    /// 分辨率 480*270，码率范围：350Kbps ~ 550Kbps，帧率：15fps
    ARLiveVideoResolution480x270,

    /// 分辨率 640*360，码率范围：500Kbps ~ 900Kbps，帧率：15fps
    ARLiveVideoResolution640x360,

    /// 分辨率 960*540，码率范围：800Kbps ~ 1500Kbps，帧率：15fps
    ARLiveVideoResolution960x540,

    /// 分辨率 1280*720，码率范围：1000Kbps ~ 1800Kbps，帧率：15fps
    ARLiveVideoResolution1280x720,

    /// 分辨率 1920*1080，码率范围：2500Kbps ~ 3000Kbps，帧率：15fps
    ARLiveVideoResolution1920x1080
};

/**
 * @brief 视频宽高比模式。
 *
 * @note
 * - 横屏模式下的分辨率: ARLiveVideoResolution640x360 + ARLiveVideoResolutionModeLandscape = 640 × 360
 * - 竖屏模式下的分辨率: ARLiveVideoResolution640x360 + ARLiveVideoResolutionModePortrait  = 360 × 640
 */
typedef NS_ENUM(NSInteger, ARLiveVideoResolutionMode) {

    /// 横屏模式
    ARLiveVideoResolutionModeLandscape = 0,

    /// 竖屏模式
    ARLiveVideoResolutionModePortrait = 1

};

/**
 * @brief 视频编码填充模式。
 */
typedef NS_ENUM(NSInteger, ARLiveVideoScaleMode) {
    
    /// 图像铺满屏幕，超出显示视窗的视频部分将被裁剪，画面显示可能不完整
    ARLiveVideoScaleModeFill,

    /// 图像长边填满屏幕，短边区域会被填充黑色，画面的内容完整
    ARLiveVideoScaleModeFit,

    /// 图像长边填满屏幕，根据设置的比例进行缩放，画面的内容完整
    ARLiveVideoScaleModeAuto
};

/**
 * @brief 视频播放模式。
 */
typedef NS_ENUM(NSInteger, ARLivePlayMode) {
    
    /// 直播模式 - 暂停的过程中，数据会丢失，保证实时性
    ARLivePlayModeLive,

    /// 点播模式 - 暂停的过程中，数据不会丢失，恢复后会继续播放
    ARLivePlayModeVod
};



#endif /* ARLiveEnumerates_h */

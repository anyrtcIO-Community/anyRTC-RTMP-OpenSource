//
//  ARLivePusher.m
//  ARLiveKit
//
//  Created by 余生丶 on 2021/9/23.
//

#import "ARLivePusher.h"
#include "IArLive2Engine.h"
#include "IArLivePusher.hpp"
#import "sdk/objc/components/renderer/metal/RTCMTLVideoView.h"
#import "ARLiveMacros.h"
#include "ArLiveDef.hpp"
#import "ARImageHelper.h"
#import <ReplayKit/ReplayKit.h>

class ARLivePushHandle: public anyrtc::ArLivePusherObserver {
    public:
    ARLivePushHandle() {};
    
    void setDelegate(id<ARLivePushDelegate>delegate) {
        push_delegate_ = delegate;
    }
    
    virtual ~ARLivePushHandle(void) {
        push_delegate_ = nil;
    };
    
    virtual void onError(int32_t code, const char* msg, void* extraInfo) {
        /// 直播推流器错误通知，推流器出现错误时，会回调该通知
        void(^functionBlock)() = ^(){
            if ([push_delegate_ respondsToSelector:@selector(onError:message:extraInfo:)]) {
                [push_delegate_ onError:(ARLiveCode)code message:@"" extraInfo:nil];
            }
        };
        
        CallForMainQueue(functionBlock());
    };

    virtual void onWarning(int32_t code, const char* msg, void* extraInfo) {
        /// 直播推流器警告通知
        void(^functionBlock)() = ^(){
            if ([push_delegate_ respondsToSelector:@selector(onWarning:message:extraInfo:)]) {
                [push_delegate_ onWarning:(ARLiveCode)code message:@"" extraInfo:nil];
            }
        };
        
        CallForMainQueue(functionBlock());
    };
    
    virtual void onCaptureFirstAudioFrame() {
        /// 首帧音频采集完成的回调通知
        void(^functionBlock)() = ^(){
            if ([push_delegate_ respondsToSelector:@selector(onCaptureFirstAudioFrame)]) {
                [push_delegate_ onCaptureFirstAudioFrame];
            }
        };
        
        CallForMainQueue(functionBlock());
    }
    
    virtual void onCaptureFirstVideoFrame() {
        /// 首帧视频采集完成的回调通知
        void(^functionBlock)() = ^(){
            if ([push_delegate_ respondsToSelector:@selector(onCaptureFirstVideoFrame)]) {
                [push_delegate_ onCaptureFirstVideoFrame];
            }
        };
        
        CallForMainQueue(functionBlock());
    }
    
    virtual void onMicrophoneVolumeUpdate(int32_t volume) {
        /// 麦克风采集音量值回调
        void(^functionBlock)() = ^(){
            if ([push_delegate_ respondsToSelector:@selector(onMicrophoneVolumeUpdate:)]) {
                [push_delegate_ onMicrophoneVolumeUpdate:volume];
            }
        };
        
        CallForMainQueue(functionBlock());
    }

    virtual void onPushStatusUpdate(anyrtc::ArLivePushStatus state, const char* msg, void* extraInfo) {
        /// 推流器连接状态回调通知
        void(^functionBlock)() = ^(){
            if ([push_delegate_ respondsToSelector:@selector(onPushStatusUpdate:message:extraInfo:)]) {
                [push_delegate_ onPushStatusUpdate:(ARLivePushStatus)state message:@"" extraInfo:@{}];
            }
        };
        
        CallForMainQueue(functionBlock());
    };

    virtual void onStatisticsUpdate(anyrtc::ArLivePusherStatistics statistics) {
        /// 直播推流器统计数据回调
        ARLivePusherStatistics *playerStatistics = [[ARLivePusherStatistics alloc] init];
        playerStatistics.width = statistics.width;
        playerStatistics.height = statistics.height;
        playerStatistics.fps = statistics.fps;
        playerStatistics.videoBitrate = statistics.videoBitrate;
        playerStatistics.audioBitrate = statistics.audioBitrate;
        
        void(^functionBlock)() = ^(){
            if ([push_delegate_ respondsToSelector:@selector(onStatisticsUpdate:)]) {
                [push_delegate_ onStatisticsUpdate:playerStatistics];
            }
        };
        
        CallForMainQueue(functionBlock());
    };

    virtual void onSnapshotComplete(const char* image, int length, int width, int height, anyrtc::ArLivePixelFormat format) {
        /// 截图回调
        UIImage *resultImage = [ARImageHelper convertBitmapRGBA8ToUIImage:(unsigned char *)image withWidth:width withHeight:height];
        void(^functionBlock)() = ^(){
            if ([push_delegate_ respondsToSelector:@selector(onSnapshotComplete:)]) {
                [push_delegate_ onSnapshotComplete:resultImage];
            }
        };
        
        CallForMainQueue(functionBlock());
    };
    
    virtual void onRenderVideoFrame(const AR::ArLiveVideoFrame *videoFrame) {
        /// 自定义视频渲染回调
    }
    
    virtual int onProcessVideoFrame(AR::ArLiveVideoFrame *srcFrame, AR::ArLiveVideoFrame *dstFrame) {
        /// 自定义视频预处理数据回调
        return -1;
    }
    
    virtual void onScreenCaptureStarted(){
        ///  当屏幕分享开始时，SDK 会通过此回调通知
    }
    
    virtual void onScreenCaptureStoped(int reason) {
        /// 当屏幕分享停止时，SDK 会通过此回调通知
    }
    
    private:
    __weak id<ARLivePushDelegate> push_delegate_;
};

@interface ARLiveVideoCanvas()

@property (nonatomic, assign) CGSize videoSize;
@property (nonatomic, strong) RTCMTLVideoView *renderView;

- (RTCMTLVideoView *)getRenderView;

@end


@interface ARLivePusher()<RTCVideoViewDelegate>

@property (nonatomic, strong) ARLiveVideoCanvas *videoCanvas;
@property (nonatomic, strong) RTCMTLVideoView *localVideoView;
@property (nonatomic, strong) RPSystemBroadcastPickerView * broadPickerView API_AVAILABLE(ios(12.0));

@end

extern void* GetLiveEngine();

@implementation ARLivePusher {
    ARLivePushHandle *_livePushHandle;
    anyrtc::IArLivePusher *_livePusher;
}

- (instancetype)initWithLivePusher {
    if (self = [super init]) {
        anyrtc::IArLive2Engine *liveEngine = (anyrtc::IArLive2Engine *)GetLiveEngine();
        if (liveEngine != NULL) {
            _livePusher = liveEngine->createArLivePusher();
        }
    }
    return self;
}

- (void)releaseLivePusher {
    if (_livePusher) {
        anyrtc::IArLive2Engine *liveEngine = (anyrtc::IArLive2Engine *)GetLiveEngine();
        if (liveEngine != NULL) {
            liveEngine->releaseArLivePusher(_livePusher);
            delete _livePushHandle;
            _livePushHandle = NULL;
        }
    }
}

- (void)setDelegate:(id<ARLivePushDelegate>)delegate {
    /// 设置推流器回调
    if (_livePusher) {
        if (_livePushHandle == NULL) {
            _livePushHandle = new ARLivePushHandle();
        }
        _livePushHandle->setDelegate(delegate);
        _livePusher->setObserver(_livePushHandle);
    }
}

- (int)setupCameraRender:(UIView *_Nullable)renderView {
    /// 设置本地摄像头预览 View
    if (_livePusher) {
        self.videoCanvas = nil;
        [self.localVideoView removeFromSuperview];
        self.localVideoView = nil;
        
        if (renderView != nil) {
            self.videoCanvas = [[ARLiveVideoCanvas alloc] init];
            self.videoCanvas.view = renderView;
            self.localVideoView = [[RTCMTLVideoView alloc] initWithFrame:CGRectZero];
            self.localVideoView.delegate = self;
            self.videoCanvas.renderView = self.localVideoView;
            [renderView addSubview:self.localVideoView];
            [renderView sendSubviewToBack:self.localVideoView];
            
            int result = _livePusher->setRenderView((__bridge void*)self.localVideoView);
            [self setCameraRenderMirror:ARLiveMirrorModeEnabled];
            return result;
        } else {
            return _livePusher->setRenderView(NULL);
        }
    }
    return -1;
}

- (int)setCameraRenderMirror:(ARLiveMirrorMode)mode {
    /// 设置本地摄像头预览镜像
    if (_livePusher) {
        self.videoCanvas.mirrorMode = mode;
        return _livePusher->setRenderMirror((anyrtc::ArLiveMirrorType)mode);
    }
    return -1;
}

- (int)setCameraRenderRotation:(ARLiveRotation)rotation {
    /// 设置本地摄像头预览画面的旋转角度
    if (_livePusher) {
        return _livePusher->setRenderRotation((anyrtc::ArLiveRotation)rotation);
    }
    return -1;
}

- (int)setRenderFillMode:(ARLiveRenderMode)mode {
    /// 设置画面的填充模式
    if (_livePusher) {
        self.videoCanvas.renderMode = mode;
        return 0;
    }
    return -1;
}

- (int)startCamera:(BOOL)frontCamera {
    /// 打开本地摄像头
    if (_livePusher) {
        return _livePusher->startCamera(frontCamera);
    }
    return -1;
}

- (int)stopCamera {
    /// 关闭本地摄像头
    if (_livePusher) {
        return _livePusher->stopCamera();
    }
    return -1;
}

- (int)startMicrophone {
    /// 打开麦克风
    if (_livePusher) {
        return _livePusher->startMicrophone();
    }
    return -1;
}

- (int)stopMicrophone {
    ///关闭麦克风
    if (_livePusher) {
        return _livePusher->stopMicrophone();
    }
    return -1;
}

- (int)startVirtualCamera:(UIImage *)image {
    /// 开启图片推流
    if (_livePusher) {
        ARImageObject *imageObject = [ARImageHelper convertUIImageToARImageObject: image];
        if (imageObject.width != 0 && imageObject.height != 0) {
            AR::ArLiveImage liveImage = AR::ArLiveImage();
            liveImage.imageSrc = (const char *)imageObject.data;
            liveImage.imageType = AR::ArLiveImageTypeRGBA32;
            liveImage.imageWidth = imageObject.width;
            liveImage.imageHeight = imageObject.height;
            return _livePusher->startVirtualCamera(&liveImage);
        }
    }
    return -1;
}

- (int)stopVirtualCamera {
    /// 关闭图片推流
    if (_livePusher) {
        return _livePusher->stopVirtualCamera();
    }
    return -1;
}

- (int)startScreenCapture:(NSString *_Nonnull)appGroup {
    /// 开始全系统的屏幕分享（该接口支持 iOS 11.0 及以上的 iPhone 和 iPad）
    if (_livePusher) {
        if (@available(iOS 12.0, *)) {
            self.broadPickerView.preferredExtension = appGroup;
            
            for (UIView *subView in self.broadPickerView.subviews) {
                if ([subView isKindOfClass:[UIButton class]]) {
                    [(UIButton *)subView sendActionsForControlEvents:UIControlEventAllTouchEvents];
                    break;
                }
            }
        } else if (@available(iOS 11.0, *)) {
            /// iOS 11.0 手动开启
            [RPBroadcastActivityViewController loadBroadcastActivityViewControllerWithPreferredExtension:appGroup handler:^(RPBroadcastActivityViewController * _Nullable broadcastActivityViewController, NSError * _Nullable error) {
                
            }];
        }
    }
    return -1;
}

- (int)stopScreenCapture {
    /// 关闭屏幕采集
    return 0;
}

- (int)setEncoderMirror:(BOOL)mirror {
    /// 设置视频编码镜像
    if (_livePusher) {
        return _livePusher->setEncoderMirror(mirror);
    }
    return -1;
}

- (int)pauseAudio {
    /// 静音本地音频
    if (_livePusher) {
        return _livePusher->pauseAudio();
    }
    return -1;
}

- (int)resumeAudio {
    /// 取消静音本地音频
    if (_livePusher) {
        return _livePusher->resumeAudio();
    }
    return -1;
}

- (int)pauseVideo {
    /// 暂停推流器的视频流
    if (_livePusher) {
        return _livePusher->pauseVideo();
    }
    return -1;
}

- (int)resumeVideo {
    /// 恢复推流器的视频流
    if (_livePusher) {
        return _livePusher->resumeVideo();
    }
    return -1;
}

- (int)startPush:(NSString *_Nonnull)url {
    /// 开始音视频数据推流
    if (_livePusher) {
        return _livePusher->startPush([url UTF8String]);
    }
    return -1;
}

- (int)stopPush {
    /// 停止推送音视频数据
    if (_livePusher) {
        return _livePusher->stopPush();
    }
    return -1;
}

- (int)isPushing {
    /// 当前推流器是否正在推流中
    if (_livePusher) {
        return _livePusher->isPushing();
    }
    return -1;
}

- (int)setAudioQuality:(ARLiveAudioQuality)quality {
    /// 设置推流音频质量
    if (_livePusher) {
        return _livePusher->setAudioQuality((anyrtc::ArLiveAudioQuality)quality);
    }
    return -1;
}

- (int)setVideoQuality:(ARLiveVideoEncoderParam *)param {
    /// 设置推流视频编码参数
    if (_livePusher) {
        AR::ArLiveVideoEncoderParam encoderParam = AR::ArLiveVideoEncoderParam((AR::ArLiveVideoResolution)int(param.videoResolution));
        
        encoderParam.videoResolutionMode = (param.videoResolutionMode == ARLiveVideoResolutionModePortrait) ? AR::ArLiveVideoResolutionModePortrait : AR::ArLiveVideoResolutionModeLandscape;
        encoderParam.videoFps = param.videoFps;
        encoderParam.videoBitrate = param.videoBitrate;
        encoderParam.minVideoBitrate = param.minVideoBitrate;
        encoderParam.videoScaleMode = AR::ArLiveVideoScaleMode(int(param.videoScaleMode));
        _livePusher->setVideoQuality(encoderParam);
    }
    return -1;
}

- (int)setBeautyEffect:(BOOL)enable {
    /// 美颜开关
    if (_livePusher) {
        return _livePusher->setBeautyEffect(enable);
    }
    return -1;
}

- (int)snapshot {
    /// 截取推流过程中的本地画面
    if (_livePusher) {
        return _livePusher->snapshot();
    }
    return -1;
}

- (int)setWatermark:(UIImage *)image x:(float)x y:(float)y scale:(float)scale {
    /// 设置推流器水印。默认情况下，水印不开启
    return -1;
}

- (int)enableVolumeEvaluation:(NSUInteger)intervalMs {
    /// 启用采集音量大小提示
    if (_livePusher) {
        return _livePusher->enableVolumeEvaluation((int32_t)intervalMs);
    }
    return -1;
}

- (int)enableCustomVideoCapture:(BOOL)enable {
    /// 开启/关闭自定义视频采集
    if (_livePusher) {
        return _livePusher->enableCustomVideoCapture(enable);
    }
    return -1;
}

- (int)enableCustomAudioCapture:(BOOL)enable {
    /// 开启/关闭自定义音频采集
    if (_livePusher) {
        return _livePusher->enableCustomAudioCapture(enable);
    }
    return -1;
}

- (int)sendCustomVideoFrame:(ARLiveVideoFrame *)videoFrame {
    /// 在自定义视频采集模式下，将采集的视频数据发送到SDK
    if (_livePusher) {
        AR::ArLiveVideoFrame liveVideoFrame;
        liveVideoFrame.width = (int32_t)videoFrame.width;
        liveVideoFrame.height = (int32_t)videoFrame.height;
        liveVideoFrame.stride = (int32_t)videoFrame.stride;
        liveVideoFrame.pixelFormat = AR::ArLivePixelFormatNV12;
        liveVideoFrame.rotation = AR::ArLiveRotation(int(videoFrame.rotation));
        liveVideoFrame.length = (int)(videoFrame.width * videoFrame.height * 3 >> 1);
        
        CVPixelBufferRef pixelBuffer = videoFrame.pixelBuffer;
        CVPixelBufferLockBaseAddress(pixelBuffer, 0);
        
        if (CVPixelBufferIsPlanar(pixelBuffer)) {
            int basePlane = 0;
            liveVideoFrame.data = (char *)CVPixelBufferGetBaseAddressOfPlane(pixelBuffer, basePlane);
        } else {
            liveVideoFrame.data = (char *)CVPixelBufferGetBaseAddress(pixelBuffer);
        }
        CVPixelBufferUnlockBaseAddress(pixelBuffer, 0);
        return _livePusher->sendCustomVideoFrame(&liveVideoFrame);
    }
    return -1;
}

- (int)sendCustomAudioFrame:(ARLiveAudioFrame *)audioFrame {
    /// 在自定义音频采集模式下，将采集的音频数据发送到SDK
    if (_livePusher) {
        AR::ArLiveAudioFrame liveAudioFrame;
        liveAudioFrame.data = (char *)audioFrame.data;
        liveAudioFrame.sampleRate = audioFrame.sampleRate;
        liveAudioFrame.channel = audioFrame.channel;
        liveAudioFrame.length = (audioFrame.sampleRate/100)* audioFrame.channel * sizeof(int16_t);
        return _livePusher->sendCustomAudioFrame(&liveAudioFrame);
    }
    return -1;
}

- (int)sendSeiMessage:(int)payloadType data:(NSData *)data {
    /// 发送 SEI 消息
    if (_livePusher) {
        return _livePusher->sendSeiMessage(payloadType, (const uint8_t *)[data bytes], (uint32_t)data.length);
    }
    return -1;
}

- (void)showDebugView:(BOOL)isShow {
    /// 显示仪表盘
}

//MARK: - RTCVideoViewDelegate

- (void)videoView:(id<RTCVideoRenderer>)videoView didChangeVideoSize:(CGSize)size {
    if (self.localVideoView == videoView) {
        self.videoCanvas.videoSize = size;
    }
}

//MARK: - lazy

- (RPSystemBroadcastPickerView *)broadPickerView  API_AVAILABLE(ios(12.0)){
    if (!_broadPickerView) {
        _broadPickerView = [[RPSystemBroadcastPickerView alloc] initWithFrame:CGRectZero];
        _broadPickerView.showsMicrophoneButton = YES;
    }
    return _broadPickerView;
}

@end

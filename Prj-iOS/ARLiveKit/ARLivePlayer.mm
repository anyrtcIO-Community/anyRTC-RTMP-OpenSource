//
//  ARLivePlayer.m
//  ARLiveKit
//
//  Created by 余生丶 on 2021/9/23.
//

#import "ARLivePlayer.h"
#import "ARLiveMacros.h"
#include "IArLive2Engine.h"
#import "sdk/objc/components/renderer/metal/RTCMTLVideoView.h"
#include <unordered_set>
#import "ARImageHelper.h"

#ifdef ENVIRONMENT32
using address_t = unsigned int;
#else
using address_t = unsigned long long;
#endif

static std::unordered_set<address_t> objc_address;

class ARLivePlayHandle: public anyrtc::ArLivePlayerObserver {
    public:
    
    ARLivePlayHandle(ARLivePlayer *player) {
        playerKit_ = player;
        objc_address.insert((address_t)this);
    };
    
    void setDelegate(id<ARLivePlayDelegate>delegate) {
        play_delegate_ = delegate;
    }
    
    virtual ~ARLivePlayHandle(void) {
        objc_address.erase((address_t)this);
        play_delegate_ = nil;
    };
    
    inline const bool isDestroy() const {
        bool isFind = objc_address.find((address_t)this) != objc_address.end();
        return !isFind;
    }
    
    void onError(anyrtc::IArLivePlayer *player, int32_t code, const char *msg, void *extraInfo) override {
        /// 直播播放器错误通知，播放器出现错误时，会回调该通知
        void(^functionBlock)() = ^() {
            if (!isDestroy()) {
                if ([play_delegate_ respondsToSelector:@selector(onError:code:message:extraInfo:)]) {
                    [play_delegate_ onError:(id)playerKit_ code:(ARLiveCode)code message:[NSString stringWithUTF8String:msg] extraInfo:@{}];
                }
            }
        };
        
        CallForMainQueue(functionBlock());
    }
    
    void onWarning(anyrtc::IArLivePlayer *player, int32_t code, const char *msg, void *extraInfo) override {
        /// 直播播放器警告通知
        void(^functionBlock)() = ^(){
            if (!isDestroy()) {
                if ([play_delegate_ respondsToSelector:@selector(onWarning:code:message:extraInfo:)]) {
                    [play_delegate_ onWarning:(id)playerKit_ code:(ARLiveCode)code message:[NSString stringWithUTF8String:msg] extraInfo:@{}];
                }
            }
        };
        
        CallForMainQueue(functionBlock());
    }
    
    void onVideoPlayStatusUpdate(anyrtc::IArLivePlayer *player, anyrtc::ArLivePlayStatus status, anyrtc::ArLiveStatusChangeReason reason, void *extraInfo) override {
        /// 直播播放器视频状态变化通知
        void(^functionBlock)() = ^(){
            if (!isDestroy()) {
                if ([play_delegate_ respondsToSelector:@selector(onVideoPlayStatusUpdate:status:reason:extraInfo:)]) {
                    [play_delegate_ onVideoPlayStatusUpdate:(id)playerKit_ status:(ARLivePlayStatus)status reason:(ARLiveStatusChangeReason)reason extraInfo:@{}];
                }
            }
        };
        
        CallForMainQueue(functionBlock());
    }
    
    void onAudioPlayStatusUpdate(anyrtc::IArLivePlayer *player, anyrtc::ArLivePlayStatus status, anyrtc::ArLiveStatusChangeReason reason, void *extraInfo) override {
        /// 直播播放器音频状态变化通知
        void(^functionBlock)() = ^(){
            if (!isDestroy()) {
                if ([play_delegate_ respondsToSelector:@selector(onAudioPlayStatusUpdate:status:reason:extraInfo:)]) {
                    [play_delegate_ onAudioPlayStatusUpdate:(id)play_delegate_ status:(ARLivePlayStatus)status reason:(ARLiveStatusChangeReason)reason extraInfo:@{}];
                }
            }
        };
        
        CallForMainQueue(functionBlock());
    }
    
    void onPlayoutVolumeUpdate(anyrtc::IArLivePlayer *player, int32_t volume) override {
        /// 播放器音量大小回调
        void(^functionBlock)() = ^(){
            if (!isDestroy()) {
                if ([play_delegate_ respondsToSelector:@selector(onPlayoutVolumeUpdate:volume:)]) {
                    [play_delegate_ onPlayoutVolumeUpdate:(id)playerKit_ volume:volume];
                }
            }
        };
        
        CallForMainQueue(functionBlock());
    }
    
    void onVodPlaybackProcess(anyrtc::IArLivePlayer *player, int allDuration, int currentPlaybackTime, int bufferDuration) override {
        /// 播放器点播的进度回调
        void(^functionBlock)() = ^(){
            if (!isDestroy()) {
                if ([play_delegate_ respondsToSelector:@selector(onVodPlaybackProcess:allDuration:playTime:bufferDuration:)]) {
                    [play_delegate_ onVodPlaybackProcess:playerKit_ allDuration:allDuration playTime:currentPlaybackTime bufferDuration:bufferDuration];
                }
            }
        };
        
        CallForMainQueue(functionBlock());
    }
    
    void onStatisticsUpdate(anyrtc::IArLivePlayer *player, anyrtc::ArLivePlayerStatistics statistics) override {
        /// 直播播放器统计数据回调
        ARLivePlayerStatistics *playerStatistics = [[ARLivePlayerStatistics alloc] init];
        playerStatistics.width = statistics.width;
        playerStatistics.height = statistics.height;
        playerStatistics.fps = statistics.fps;
        playerStatistics.videoBitrate = statistics.videoBitrate;
        playerStatistics.audioBitrate = statistics.audioBitrate;
        
        void(^functionBlock)() = ^(){
            if (!isDestroy()) {
                if ([play_delegate_ respondsToSelector:@selector(onStatisticsUpdate:statistics:)]) {
                    [play_delegate_ onStatisticsUpdate:playerKit_ statistics:playerStatistics];
                }
            }
        };
        
        CallForMainQueue(functionBlock());
    }

    void onSnapshotComplete(anyrtc::IArLivePlayer *player, const char *image, int length, int width, int height, anyrtc::ArLivePixelFormat format) override {
        /// 截图回调
        UIImage *resultImage = [ARImageHelper convertBitmapRGBA8ToUIImage:(unsigned char *)image withWidth:width withHeight:height];
        void(^functionBlock)() = ^(){
            if (!isDestroy()) {
                if ([play_delegate_ respondsToSelector:@selector(onSnapshotComplete:image:)]) {
                    [play_delegate_ onSnapshotComplete:playerKit_ image:resultImage];
                }
            }
        };
        
        CallForMainQueue(functionBlock());
    }
    
    void onRenderVideoFrame(anyrtc::IArLivePlayer *player, const anyrtc::ArLiveVideoFrame *videoFrame) override {
        /// 自定义视频渲染回调
    }
    
    void onReceiveSeiMessage(anyrtc::IArLivePlayer *player, int payloadType, const uint8_t *data, uint32_t dataSize) override {
        /// 收到 SEI 消息的回调，发送端通过 {@link ArLivePusher} 中的 `sendSeiMessage` 来发送 SEI 消息
        NSData *seiData = [NSData dataWithBytes:data length:dataSize];
        void(^functionBlock)() = ^(){
            if (!isDestroy()) {
                if ([play_delegate_ respondsToSelector:@selector(onReceiveSeiMessage:payloadType:data:)]) {
                    [play_delegate_ onReceiveSeiMessage:playerKit_ payloadType:payloadType data:seiData];
                }
            }
        };
        
        CallForMainQueue(functionBlock());
    }
    
    private:
    __weak id<ARLivePlayDelegate> play_delegate_;
    ARLivePlayer *playerKit_;
};

@interface ARLiveVideoCanvas()

@property (nonatomic, assign) CGSize videoSize;
@property (nonatomic, strong) RTCMTLVideoView *renderView;

- (RTCMTLVideoView *)getRenderView;

@end

extern void* GetLiveEngine();

@interface ARLivePlayer()<RTCVideoViewDelegate>

@property (nonatomic, strong) RTCMTLVideoView *playerVideoView;
@property (nonatomic, strong) ARLiveVideoCanvas *videoCanvas;

@end

@implementation ARLivePlayer {
    ARLivePlayHandle *_livePlayHandle;
    anyrtc::IArLivePlayer *_livePlayer;
}

- (instancetype)initWithLivePlayer {
    self = [super init];
    if (self) {
        anyrtc::IArLive2Engine *liveEngine = (anyrtc::IArLive2Engine *)GetLiveEngine();
        if (liveEngine != NULL) {
            _livePlayer = liveEngine->createArLivePlayer();
        }
    }
    return self;
}

- (void)releaseLivePlayer {
    if (_livePlayer) {
        anyrtc::IArLive2Engine *liveEngine = (anyrtc::IArLive2Engine *)GetLiveEngine();
        if (liveEngine != NULL) {
            _livePlayer->setObserver(NULL);
            liveEngine->releaseArLivePlayer(_livePlayer);
            delete _livePlayHandle;
            _livePlayHandle = NULL;
            _livePlayer = NULL;
        }
    }
}

- (void)setDelegate:(id<ARLivePlayDelegate>)delegate {
    /// 设置播放器器回调
    if (_livePlayer) {
        if (_livePlayHandle == NULL) {
            _livePlayHandle = new ARLivePlayHandle(self);
            _livePlayer->setObserver(_livePlayHandle);
        }
        _livePlayHandle->setDelegate(delegate);
    }
}

- (int)setRenderView:(UIView *)renderView {
    /// 设置播放器的视频渲染 View
    if (_livePlayer) {
        self.videoCanvas = nil;
        self.playerVideoView.delegate = nil;
        [self.playerVideoView removeFromSuperview];
        self.playerVideoView = nil;
        
        if (renderView != nil) {
            self.videoCanvas = [[ARLiveVideoCanvas alloc] init];
            self.videoCanvas.view = renderView;
            self.playerVideoView = [[RTCMTLVideoView alloc] initWithFrame:CGRectZero];
            self.playerVideoView.delegate = self;
            self.videoCanvas.renderView = self.playerVideoView;
            [renderView addSubview:self.playerVideoView];
            [renderView sendSubviewToBack:self.playerVideoView];
            return _livePlayer->setRenderView((__bridge void*)self.playerVideoView);
        } else {
            _livePlayer->setRenderView(NULL);
        }
    }
    return -1;
}

- (int)setRenderRotation:(ARLiveRotation)rotation {
    /// 设置播放器画面的旋转角度。
    if (_livePlayer) {
        return _livePlayer->setRenderRotation((anyrtc::ArLiveRotation)rotation);
    }
    return -1;
}

- (int)setRenderFillMode:(ARLiveRenderMode)mode {
    /// 设置画面的填充模式。
    if (_livePlayer) {
        self.videoCanvas.renderMode = mode;
        return _livePlayer->setRenderFillMode((anyrtc::ArLiveFillMode)mode);
    }
    return -1;
}

- (int)startPlay:(NSString *_Nonnull)url {
    /// 开始播放音视频流
    if (_livePlayer) {
        return _livePlayer->startPlay([url UTF8String]);
    }
    return -1;
}

- (int)stopPlay {
    /// 停止播放音视频流
    if (_livePlayer) {
        return _livePlayer->stopPlay();
    }
    return -1;
}

- (int)setPlayMode:(ARLivePlayMode)mode {
    /// 设置视频播放模式
    if (_livePlayer) {
        return _livePlayer->setPlayMode((anyrtc::ArLivePlayMode)mode);
    }
    return -1;
}

- (int)isPlaying {
    /// 播放器是否正在播放中
    if (_livePlayer) {
        return _livePlayer->isPlaying();
    }
    return -1;
}

- (int)seekTo:(int)seekTimeS {
    /// 跳转进度
    if (_livePlayer) {
        return _livePlayer->seekTo(seekTimeS);
    }
    return -1;
}

- (int)setSpeed:(CGFloat)speed {
    /// 倍速播放
    if (_livePlayer) {
        return _livePlayer->setSpeed(speed);
    }
    return -1;
}

- (int)replay {
    /// 重新开始播放。一般用于点播场景
    if (_livePlayer) {
        _livePlayer->rePlay();
    }
    return -1;
}

- (int)pauseAudio {
    /// 暂停播放器的音频流
    if (_livePlayer) {
        return _livePlayer->pauseAudio();
    }
    return -1;
}

- (int)resumeAudio {
    /// 恢复播放器的音频流
    if (_livePlayer) {
        return _livePlayer->resumeAudio();
    }
    return -1;
}

- (int)pauseVideo {
    /// 暂停播放器的视频流
    if (_livePlayer) {
        return _livePlayer->pauseVideo();
    }
    return -1;
}

- (int)resumeVideo {
    /// 恢复播放器的视频流
    if (_livePlayer) {
        return _livePlayer->resumeVideo();
    }
    return -1;
}

- (int)setPlayoutVolume:(NSUInteger)volume {
    /// 设置播放器音量
    if (_livePlayer) {
        return _livePlayer->setPlayoutVolume((int32_t)volume);
    }
    return -1;
}

- (int)setCacheParams:(CGFloat)minTime maxTime:(CGFloat)maxTime {
    /// 设置播放器缓存自动调整的最小和最大时间 ( 单位：秒 )
    if (_livePlayer) {
        return _livePlayer->setCacheParams(minTime, maxTime);
    }
    return -1;
}

- (int)enableVolumeEvaluation:(NSUInteger)intervalMs {
    /// 启用播放音量大小提示
    if (_livePlayer) {
        return _livePlayer->enableVolumeEvaluation((int32_t)intervalMs);
    }
    return -1;
}

- (int)snapshot {
    /// 截取播放过程中的视频画面
    if (_livePlayer) {
        return _livePlayer->snapshot();
    }
    return -1;
}

- (int)enableCustomRendering:(BOOL)enable pixelFormat:(ARLivePixelFormat)pixelFormat bufferType:(ARLiveBufferType)bufferType {
    /// 设置视频自定义渲染回调
    if (_livePlayer) {
        return _livePlayer->enableCustomRendering(enable, (anyrtc::ArLivePixelFormat)pixelFormat, (anyrtc::ArLiveBufferType)bufferType);
    }
    return -1;
}

- (int)enableReceiveSeiMessage:(BOOL)enable payloadType:(int)payloadType {
    /// 开启接收 SEI 消息
    if (_livePlayer) {
        return _livePlayer->enableReceiveSeiMessage(enable, payloadType);
    }
    return -1;
}

- (void)showDebugView:(BOOL)isShow {
    /// 是否显示播放器状态信息的调试浮层
}

//MARK: - RTCVideoViewDelegate

- (void)videoView:(id<RTCVideoRenderer>)videoView didChangeVideoSize:(CGSize)size {
    if (self.playerVideoView == videoView) {
        self.videoCanvas.videoSize = size;
    }
}

@end

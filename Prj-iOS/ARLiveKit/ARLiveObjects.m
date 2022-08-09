//
//  ARLiveObjects.m
//  ARLiveKit
//
//  Created by 余生丶 on 2021/9/24.
//

#import "ARLiveObjects.h"
#import "components/renderer/metal/RTCMTLVideoView.h"

@implementation ARLiveVideoCanvas {
    CGSize _videoSize;
    RTCMTLVideoView *_renderView;
}

- (instancetype)init {
    if (self = [super init]) {
        self.channelId = @"";
        self.renderMode = ARLiveRenderModeFit;
        self.mirrorMode = ARLiveMirrorModeAuto;
    }
    return self;
}

- (void)setView:(UIView *)view {
    _view = view;
    _view.layer.masksToBounds = YES;
    [_view addObserver:self forKeyPath:@"frame" options:NSKeyValueObservingOptionNew |NSKeyValueObservingOptionOld  context:nil];
    [_view addObserver:self forKeyPath:@"center" options:NSKeyValueObservingOptionNew|NSKeyValueObservingOptionOld context:nil];
}

- (void)setVideoSize:(CGSize)videoSize {
    if (_videoSize.width != videoSize.height && _videoSize.height != videoSize.height) {
        _videoSize = videoSize;
        dispatch_async(dispatch_get_main_queue(), ^{
            [self changeRenderSize];
        });
    }
}

- (void)setRenderMode:(ARLiveRenderMode)renderMode {
    _renderMode = renderMode;
    dispatch_async(dispatch_get_main_queue(), ^{
        [self changeRenderSize];
    });
}

- (void)setRenderView:(RTCMTLVideoView *)renderView {
    _renderView = renderView;
}

- (void)setMirrorMode:(ARLiveMirrorMode)mirrorMode {
    if (mirrorMode != ARLiveMirrorModeDisabled) {
        _renderView.transform = CGAffineTransformMakeScale(-1.0, 1.0);
    } else {
        _renderView.transform = CGAffineTransformIdentity;
    }
}

- (RTCMTLVideoView *)getRenderView {
    return _renderView;
}

- (void)changeRenderSize {
    UIView *mySelfView = _renderView;
    UIView *mySelfSuperView = self.view;
    
    if (!mySelfView || _videoSize.width == 0 ||  _videoSize.height == 0) {
        return;
    }
    
    /// 此处仅供参考
    if (self.renderMode == ARLiveRenderModeFit) {
        CGRect remoteVideoFrame = AVMakeRectWithAspectRatioInsideRect(CGSizeMake(_videoSize.width, _videoSize.height), mySelfSuperView.frame);
        float X = mySelfSuperView.frame.size.width - remoteVideoFrame.size.width;
        float Y = mySelfSuperView.frame.size.height - remoteVideoFrame.size.height;
        mySelfView.frame = CGRectMake(fabs(X/2.0), fabs(Y/2.0), remoteVideoFrame.size.width,remoteVideoFrame.size.height);
    } else {
        //默认 hidden
        if (_videoSize.width > _videoSize.height) {
            CGRect remoteVideoFrame = AVMakeRectWithAspectRatioInsideRect(CGSizeMake(_videoSize.width, _videoSize.height), mySelfSuperView.frame);
            // 宽度扩充
            if(remoteVideoFrame.size.width < mySelfSuperView.frame.size.width) {
                float scale = mySelfSuperView.frame.size.width/remoteVideoFrame.size.width;
                remoteVideoFrame.size.height = remoteVideoFrame.size.height * scale;
                remoteVideoFrame.size.width = remoteVideoFrame.size.width * scale;
            }
            // 居中显示
            float Y = (mySelfSuperView.frame.size.height-remoteVideoFrame.size.height)/2;
            if (Y > 0) {
                float scaleY = mySelfSuperView.frame.size.height/remoteVideoFrame.size.height;
                remoteVideoFrame.size.height = remoteVideoFrame.size.height * scaleY;
                remoteVideoFrame.size.width = remoteVideoFrame.size.width * scaleY;
                float X = (mySelfSuperView.frame.size.width-remoteVideoFrame.size.width)/2;
                mySelfView.frame = CGRectMake(X, 0, remoteVideoFrame.size.width, remoteVideoFrame.size.height);
            } else {
                mySelfView.frame = CGRectMake(0, Y, remoteVideoFrame.size.width, remoteVideoFrame.size.height);
            }
            
        } else {
            // 竖屏
            // 480*640
            CGRect remoteVideoFrame = AVMakeRectWithAspectRatioInsideRect(CGSizeMake(_videoSize.width, _videoSize.height), mySelfSuperView.frame);
            // 高度扩充
            if(remoteVideoFrame.size.height < mySelfSuperView.frame.size.height) {
                float scale = mySelfSuperView.frame.size.height/remoteVideoFrame.size.height;
                remoteVideoFrame.size.height = remoteVideoFrame.size.height * scale;
                remoteVideoFrame.size.width = remoteVideoFrame.size.width * scale;
            }
            float X = (mySelfSuperView.frame.size.width - remoteVideoFrame.size.width)/2;
            if (X > 0) {
                float scaleX = mySelfSuperView.frame.size.width/remoteVideoFrame.size.width;
                remoteVideoFrame.size.height = remoteVideoFrame.size.height * scaleX;
                remoteVideoFrame.size.width = remoteVideoFrame.size.width * scaleX;
                float Y = (mySelfSuperView.frame.size.height-remoteVideoFrame.size.height)/2;
                mySelfView.frame = CGRectMake(0, Y, remoteVideoFrame.size.width, remoteVideoFrame.size.height);
            } else {
                mySelfView.frame = CGRectMake(X, 0, remoteVideoFrame.size.width, remoteVideoFrame.size.height);
            }
        }
    }
}

- (void)observeValueForKeyPath:(NSString *)keyPath ofObject:(id)object change:(NSDictionary<NSKeyValueChangeKey,id> *)change context:(void *)context {
    if (([keyPath isEqualToString:@"frame"] || [keyPath isEqualToString:@"center"]) && object == _view) {
        [self changeRenderSize];
    }
}

- (void)dealloc {
    [NSObject cancelPreviousPerformRequestsWithTarget:self];
    if (_view) {
        [_view removeObserver:self forKeyPath:@"frame"];
        [_view removeObserver:self forKeyPath:@"center"];
    }
}

@end

@implementation ARLivePlayConfig

- (instancetype)init {
    if (self = [super init]) {
        self.bAuto = YES;
        self.nCacheTime = 5;
        self.nMinCacheTime = 1;
        self.nMaxCacheTime = 5;
        self.nVideoBlockThreshold = 800;
        self.nConnectRetryCount = 3;
        self.nConnectRetryInterval = 3;
        self.bEnableMsg = NO;
    }
    return self;
}

@end

@implementation ARVideoDimensions

- (instancetype)init {
    if (self = [super init]) {
        self.width = 640;
        self.height = 480;
        self.fps = 15;
    }
    return self;
}

@end

@implementation ARLiveVideoEncoderParam

- (instancetype)init {
    if (self = [super init]) {
        self.videoResolutionMode = ARLiveVideoResolutionModePortrait;
        self.videoFps = 15;
        self.videoBitrate = 900;
        self.minVideoBitrate = 500;
        self.videoScaleMode = ARLiveVideoScaleModeAuto;
    }
    return self;
}

- (instancetype)initWith:(ARLiveVideoResolution)resolution {
    if (self = [self init]) {
        self.videoResolution = resolution;
    }
    return self;
}

@end

@implementation ARLivePlayerStatistics


@end

@implementation ARLiveVideoFrame

@end

@implementation ARLiveAudioFrame

@end

@implementation ARLivePusherStatistics

@end

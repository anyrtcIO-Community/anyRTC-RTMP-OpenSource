//
//  ARLiveEngineKit.m
//  ARLiveKit
//
//  Created by 余生丶 on 2021/9/19.
//

#import "ARLiveEngineKit.h"
#import <AVFoundation/AVFoundation.h>
#include "ArLive2Engine.h"
#include "IArLive2Engine.h"

class ARLiveKitHandle: public anyrtc::IArLive2EngineObserver {
    public:
    ARLiveKitHandle(id<ARLiveEngineDelegate>delegate, ARLiveEngineKit *engineKit) {
        live_delegate_ = delegate;
        liveEngineKit_ = engineKit;
    };
    
    virtual ~ARLiveKitHandle(void) {
        live_delegate_ = nil;
    };
    
    private:
    __weak id<ARLiveEngineDelegate> live_delegate_;
    ARLiveEngineKit *liveEngineKit_;
};

static anyrtc::IArLive2Engine *liveEngine;

extern void* GetLiveEngine() {
    return &*liveEngine;
}

@interface ARLivePusher()

- (instancetype)initWithLivePusher;
- (void)releaseLivePusher;

@end

@interface ARLivePlayer()

- (instancetype)initWithLivePlayer;
- (void)releaseLivePlayer;

@end

@implementation ARLiveEngineKit {
    ARLiveKitHandle *_liveEngineHandle;
}

- (instancetype)initWithDelegate:(id<ARLiveEngineDelegate>)delegate {
    if (self = [super init]) {
        if (liveEngine == NULL) {
            liveEngine = anyrtc::createArLive2Engine();
            liveEngine->initialize(nil);
            
            [NSNotificationCenter.defaultCenter addObserver:self selector:@selector(enterBackground:) name:UIApplicationDidEnterBackgroundNotification object:nil];
            [NSNotificationCenter.defaultCenter addObserver:self selector:@selector(becomeActive:) name:UIApplicationDidBecomeActiveNotification object:nil];
        } else {
            return nil;
        }
    }
    return self;
}

+ (void)destroy {
    /// 销毁 ARLiveEngineKit 实例
    liveEngine->release();
}

- (ARLivePusher *)createArLivePusher {
    /// 创建推流对象
    if (liveEngine) {
        ARLivePusher *pusher = [[ARLivePusher alloc] initWithLivePusher];
        return pusher;
    }
    return nil;
}

- (ARLivePlayer *)createArLivePlayer {
    /// 创建拉流对象
    if (liveEngine) {
        ARLivePlayer *player = [[ARLivePlayer alloc] initWithLivePlayer];
        return player;
    }
    return nil;
}

- (void)releaseArLivePusher:(ARLivePusher *)pusher {
    /// 析构 ARLivePusher 对象
    if (liveEngine && pusher) {
        [pusher stopCamera];
        [pusher releaseLivePusher];
    }
}

- (void)releaseArLivePlayer: (ARLivePlayer *)player {
    /// 析构 ARLivePlayer 对象
    if (liveEngine && player) {
        [player releaseLivePlayer];
    }
}

// MARK: - private

- (void)enterBackground:(NSNotification *)notification {
    if (liveEngine) {
        liveEngine->setAppInBackground(true);
    }
}

- (void)becomeActive:(NSNotification *)notification {
    if (liveEngine) {
        liveEngine->setAppInBackground(false);
    }
}

@end

//
//  ARAudioTube.h
//  ARLive-ScreenShare-Extension
//
//  Copyright © 2021年 anyRTC. All rights reserved.
//

#import <Foundation/Foundation.h>
#import <ARLiveKit/ARLiveKit.h>
#import <CoreMedia/CoreMedia.h>

NS_ASSUME_NONNULL_BEGIN

typedef NS_OPTIONS(NSUInteger, AudioType) {
    AudioTypeApp = 1,
    AudioTypeMic = 2
};

@interface ARAudioTube : NSObject

+ (void)liverPusher:(ARLivePusher * _Nonnull)livePusher pushAudioCMSampleBuffer:(CMSampleBufferRef _Nonnull)sampleBuffer resampleRate:(NSUInteger)resampleRate type:(AudioType)type;

@end

NS_ASSUME_NONNULL_END

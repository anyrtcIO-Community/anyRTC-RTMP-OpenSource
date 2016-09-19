//
//  AudioManager.m
//  Teameeting
//
//  Created by jianqiangzhang on 16/2/29.
//  Copyright © 2016年 zjq. All rights reserved.
//

#import "AudioManager.h"
#import <AudioToolbox/AudioToolbox.h>
#import <AVFoundation/AVFoundation.h>
#import <UIKit/UIKit.h>

@interface AudioManager()
{
    BOOL isSpeaker;
    BOOL isProximity;
}
@end

@implementation AudioManager

- (void)OpenProximityMonitorEnable:(BOOL)isOpen
{
    if (isOpen) {
        
        if (!isProximity) {
            [[UIDevice currentDevice] setProximityMonitoringEnabled:YES];
        }
        isProximity = YES;
        
    }else{
        if (isProximity) {
              [[UIDevice currentDevice] setProximityMonitoringEnabled:NO];
        }
        isProximity = NO;
      
    }
   
}
- (void)dealloc
{
    [[NSNotificationCenter defaultCenter] removeObserver:self name:UIDeviceProximityStateDidChangeNotification object:nil];
     AudioSessionRemovePropertyListenerWithUserData(kAudioSessionProperty_AudioRouteChange,audioRouteChangeCallback,(__bridge void *)(self));
}
- (id)init
{
    self = [super init];
    if (self) {
//        [[UIDevice currentDevice] setProximityMonitoringEnabled:NO];
//        isProximity = NO;
        //红外线感应监听
        [[NSNotificationCenter defaultCenter] addObserver:self
                                                 selector:@selector(sensorStateChange:)
                                                     name:UIDeviceProximityStateDidChangeNotification
                                                   object:nil];
        
        AVAudioSession *audioSession = [AVAudioSession sharedInstance];
        NSError *audioSessionError;
        [audioSession setCategory:AVAudioSessionCategoryPlayback error:&audioSessionError];
        
        [[AVAudioSession sharedInstance] setActive:YES error:nil];
        
        //添加耳机拔掉侦听事件
        AudioSessionInitialize(NULL, NULL, NULL, NULL);
        AudioSessionAddPropertyListener(kAudioSessionProperty_AudioRouteChange,         audioRouteChangeCallback, (__bridge void *)(self));
        
        
    }
    return self;
}
- (void)setSpeakerOn
{
    if ((UI_USER_INTERFACE_IDIOM() == UIUserInterfaceIdiomPad)) {
        return;
    }
    if ([self hasHeadset]) {
        return;
    }
    if (!isSpeaker) {
       [[AVAudioSession sharedInstance] overrideOutputAudioPort:AVAudioSessionPortOverrideSpeaker error:nil];
        isSpeaker = YES;
    }
   
}
- (void)setSpeakerOff
{
    if ((UI_USER_INTERFACE_IDIOM() == UIUserInterfaceIdiomPad)) {
        return;
    }
    if (isSpeaker) {
        [[AVAudioSession sharedInstance] overrideOutputAudioPort:AVAudioSessionPortOverrideNone error:nil];
        isSpeaker = NO;
    }
}

- (BOOL)hasHeadset
{
    CFStringRef route;
    UInt32 propertySize = sizeof(CFStringRef);
    AudioSessionGetProperty(kAudioSessionProperty_AudioRoute, &propertySize, &route);
    
    if((route == NULL) || (CFStringGetLength(route) == 0))
    {
        // Silent Mode
    }
    else
    {
        NSString* routeStr = (__bridge NSString*)route;
        
        NSRange headphoneRange = [routeStr rangeOfString : @"Headphone"];
        NSRange headsetRange = [routeStr rangeOfString : @"Headset"];
        if (headphoneRange.location != NSNotFound)
        {
            return YES;
        } else if(headsetRange.location != NSNotFound)
        {
            return YES;
        }
    }
    isSpeaker = NO;
    return NO;
}

//处理监听触发事件
-(void)sensorStateChange:(NSNotificationCenter *)notification;
{
    if ([[UIDevice currentDevice] proximityState] == YES){
        [[AVAudioSession sharedInstance] setCategory:AVAudioSessionCategoryPlayAndRecord error:nil];
    }
    else{
        [[AVAudioSession sharedInstance] setCategory:AVAudioSessionCategoryPlayback error:nil];
    }
}

//耳机拔掉侦听事件回调 [AudioManager 为当前类，即为self]
void audioRouteChangeCallback(void *inClientData, AudioSessionPropertyID inID, UInt32inDataSize, const void *inData)
{
    CFDictionaryRef routeChangeDictionary = inData;
    CFNumberRef routeChangeReasonRef = CFDictionaryGetValue(routeChangeDictionary,CFSTR(kAudioSession_AudioRouteChangeKey_Reason));
    SInt32 routeChangeReason;
    CFNumberGetValue (routeChangeReasonRef, kCFNumberSInt32Type, &routeChangeReason);
    AudioManager *_self = (__bridge AudioManager *)inClientData;
    if (routeChangeReason == kAudioSessionRouteChangeReason_OldDeviceUnavailable) {     //拔掉耳机
        [_self setSpeakerOn];
    } else if (routeChangeReason ==kAudioSessionRouteChangeReason_NewDeviceAvailable) {    //插入耳机
    }
}


@end

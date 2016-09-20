/**
*  Copyright (c) 2016 The AnyRTC project authors. All Rights Reserved.
*
*  Please visit https://www.anyrtc.io for detail.
*
* The GNU General Public License is a free, copyleft license for
* software and other kinds of works.
*
* The licenses for most software and other practical works are designed
* to take away your freedom to share and change the works.  By contrast,
* the GNU General Public License is intended to guarantee your freedom to
* share and change all versions of a program--to make sure it remains free
* software for all its users.  We, the Free Software Foundation, use the
* GNU General Public License for most of our software; it applies also to
* any other work released this way by its authors.  You can apply it to
* your programs, too.
* See the GNU LICENSE file for more info.
*/

#import "RTMPGuestKit.h"
#import <AVFoundation/AVFoundation.h>
#include "RTMPGuester.h"
#include "webrtc/base/thread.h"
#import "WebRTC/RTCEAGLVideoView.h"
#pragma mark - RTMPGuestIOS C++
class RTMPGuestIOS : public RTMPGuesterEvent
{
public:
    RTMPGuestIOS(id<RTMPGuestRtmpDelegate> delegate):ui_avalible_(YES), rtmp_delegate_(delegate){
        guest_ = RTMPGuester::Create(*this);
    };
    virtual ~RTMPGuestIOS(void){
        ui_avalible_ = false;
        rtmp_delegate_ = nil;
        RTMPGuester::Destory(guest_);
    };
    
    RTMPGuester&Guest() {return *guest_;};
    void SetUIAvalible(BOOL avalible) {ui_avalible_ = avalible;};
    
public:
    //* For RTMPCGuestEvent
    virtual void OnRtmplayerOK(){
        if(!ui_avalible_)
            return;
        dispatch_async(dispatch_get_main_queue(), ^{
            [rtmp_delegate_ OnRtmplayerOK];
        });
    };
    virtual void OnRtmplayerStatus(int cacheTime, int curBitrate){
        if(!ui_avalible_)
            return;
        dispatch_async(dispatch_get_main_queue(), ^{
            [rtmp_delegate_ OnRtmplayerStatus:cacheTime withBitrate:curBitrate];
        });
    };
    virtual void OnRtmplayerCache(int time){
        if(!ui_avalible_)
            return;
        dispatch_async(dispatch_get_main_queue(), ^{
            [rtmp_delegate_ OnRtmplayerCache:time];
        });
    };
    virtual void OnRtmplayerClosed(int errcode){
        if(!ui_avalible_)
            return;
        dispatch_async(dispatch_get_main_queue(), ^{
            [rtmp_delegate_ OnRtmplayerClosed:errcode];
        });
    };
    
private:
    BOOL    ui_avalible_;
    __weak  id<RTMPGuestRtmpDelegate> rtmp_delegate_;
    RTMPGuester *guest_;
};

#pragma mark - RTMPGuestKit
@interface RTMPGuestKit() {
    RTMPGuestIOS* rtmpc_guest_;
}
@property (nonatomic,weak) UIView *parView;
@property (nonatomic, strong)  RTCEAGLVideoView *videoShowView;
@property (nonatomic, assign) CGSize showVideoSize; // 显示视频的窗口大小
@property (nonatomic, assign) BOOL isPlay; //已经开始播放
@end

@implementation RTMPGuestKit

- (instancetype)initWithDelegate:(id<RTMPGuestRtmpDelegate>)delegate
{
    if (self = [super init]) {
        rtc::ThreadManager::Instance()->WrapCurrentThread();
        rtmpc_guest_ = new RTMPGuestIOS(delegate);
        _videoContentMode = VideoShowModeScaleAspectFill;
    }
    return self;
}
- (void) clear
{
    rtmpc_guest_->SetUIAvalible(NO);
    [self StopRtmpPlay];
    
    [self.videoShowView removeFromSuperview];
    self.videoShowView = nil;
}
- (void)dealloc
{
    delete rtmpc_guest_;
}

//* Rtmp function for pull rtmp stream
- (BOOL)StartRtmpPlay:(NSString*)strUrl andRender:(UIView*)render
{
    if (strUrl && render) {
        [UIApplication sharedApplication].idleTimerDisabled = YES;
        self.videoShowView = [[RTCEAGLVideoView alloc] initWithFrame:render.frame];
        [render addSubview:self.videoShowView];
        self.parView = render;
        self.parView.backgroundColor = [UIColor blackColor];
        rtmpc_guest_->Guest().StartRtmpPlay([strUrl UTF8String], (__bridge void*)_videoShowView);
        return YES;
    }
    return NO;
}
- (void)StopRtmpPlay
{
    [UIApplication sharedApplication].idleTimerDisabled = NO;
    
    rtmpc_guest_->Guest().StopRtmpPlay();
}
@end

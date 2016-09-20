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

#import "RTMPHosterKit.h"
#include "RTMPHoster.h"
#import "GPUImageVideoCapturer.h"
#import "WebRTC/RTCEAGLVideoView.h"
#import "AudioManager.h"

#pragma mark - RTMPHosterIOS C++
class RTMPHosterIOS : public RTMPHosterEvent
{
public:
    RTMPHosterIOS(id<RTMPHosterRtmpDelegate> delegate):ui_avalible_(YES), rtmp_delegate_(delegate){
        hoster_ = RTMPHoster::Create(*this);
        peersArray = [[NSMutableArray alloc] initWithCapacity:3];
        audioManager = [[AudioManager alloc] init];
    };
    virtual ~RTMPHosterIOS(void){
        ui_avalible_ = NO;
        rtmp_delegate_ = nil;
        RTMPHoster::Destory(hoster_);
        audioManager = nil;
    };
    
    RTMPHoster&Hoster() {return *hoster_;};
    void SetUIAvalible(BOOL avalible) {ui_avalible_ = avalible;};
    
public:
    //* For RTMPHosterEvent
    virtual void OnRtmpStreamOK(){
        if(!ui_avalible_)
            return;
        dispatch_async(dispatch_get_main_queue(), ^{
           [rtmp_delegate_ OnRtmpStreamOK];
        });
    };
    virtual void OnRtmpStreamReconnecting(int times){
        if(!ui_avalible_)
            return;
        dispatch_async(dispatch_get_main_queue(), ^{
            [rtmp_delegate_ OnRtmpStreamReconnecting:times];
        });
    };
    virtual void OnRtmpStreamStatus(int delayMs, int netBand){
        if(!ui_avalible_)
            return;
        dispatch_async(dispatch_get_main_queue(), ^{
            [rtmp_delegate_ OnRtmpStreamStatus:delayMs withNetBand:netBand];
        });
    };
    virtual void OnRtmpStreamFailed(int code){
        if(!ui_avalible_)
            return;
        dispatch_async(dispatch_get_main_queue(), ^{
            [rtmp_delegate_ OnRtmpStreamFailed:code];
        });
    };
    virtual void OnRtmpStreamClosed(){
        if(!ui_avalible_)
            return;
        dispatch_async(dispatch_get_main_queue(), ^{
            [rtmp_delegate_ OnRtmpStreamClosed];
        });
    };
    
private:
    BOOL    ui_avalible_;
    __weak id<RTMPHosterRtmpDelegate> rtmp_delegate_;
    RTMPHoster *hoster_;
    AudioManager   *audioManager;
public:
    NSMutableArray *peersArray;
};

#pragma mark - RTMPHosterKit
@interface RTMPHosterKit() {
    webrtc::GPUImageVideoCapturer* beauty_capturer_;
    RTMPHosterIOS* RTMP_hoster_;
    bool    use_front_cam_;
    
}
@end

@implementation RTMPHosterKit

- (instancetype)initWithDelegate:(id<RTMPHosterRtmpDelegate>)delegate
{
    if (self = [super init]) {
        use_front_cam_ = false;
        rtc::ThreadManager::Instance()->WrapCurrentThread();
        RTMP_hoster_ = new RTMPHosterIOS(delegate);
        
    }
    return self;
}
- (void) clear
{
    RTMP_hoster_->SetUIAvalible(NO);
    [self SetVideoCapturer:NULL andUseFront:YES];
    [self StopRtmpStream];
}
- (void)dealloc
{
    delete RTMP_hoster_;
}
//* Common function
- (void)SetAudioEnable:(bool) enabled
{
    RTMP_hoster_->Hoster().SetAudioEnable(enabled);
}
- (void)SetVideoEnable:(bool) enabled
{
    RTMP_hoster_->Hoster().SetVideoEnable(enabled);
}
- (void)SetVideoCapturer:(UIView*) render andUseFront:(bool)front;
{
    if(render == NULL) {
        [UIApplication sharedApplication].idleTimerDisabled = NO;
        if(beauty_capturer_ != nil)
            beauty_capturer_->Stop();
        RTMP_hoster_->Hoster().SetVideoCapturer(NULL);
        beauty_capturer_ = nil;
    } else {
        [UIApplication sharedApplication].idleTimerDisabled = YES;
        use_front_cam_ = front;
        beauty_capturer_ = new webrtc::GPUImageVideoCapturer(render, !front);
        RTMP_hoster_->Hoster().SetVideoCapturer((void*)beauty_capturer_);
    }
}
- (void)SwitchCamera
{
    if(beauty_capturer_ != nil) {
        use_front_cam_ = !use_front_cam_;
        beauty_capturer_->SetUseBackCamera(!use_front_cam_);
    }
}
- (void)SetBeautyEnable:(bool) enabled
{
    if (beauty_capturer_ != nil) {
        beauty_capturer_->SetBeautyFace(enabled);
    }
}
- (void)SetVideoMode:(RTMPVideoMode) videoMode
{
    RTMP_hoster_->Hoster().SetVideoMode(videoMode);
}

//* Rtmp function for push rtmp stream
- (BOOL)StartPushRtmpStream:(NSString*)strUrl
{
    if (strUrl == nil || [strUrl length] == 0) {
        return NO;
    }
    RTMP_hoster_->Hoster().StartRtmpStream([strUrl UTF8String]);
    return YES;
}
- (void)StopRtmpStream
{
    RTMP_hoster_->Hoster().StopRtmpStream();
}

@end

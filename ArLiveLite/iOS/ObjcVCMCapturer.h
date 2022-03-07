//
//  ObjcVCMCapturer.h
//  ARLiveLibrary
//
//  Created by 余生丶 on 2021/10/29.
//

#ifndef __IOS_RENDERER_H__
#define __IOS_RENDERER_H__

#include "modules/video_capture/video_capture.h"
#include "api/video/video_frame.h"
#include "MgrRender.h"
#include "api/media_stream_interface.h"
#import <Foundation/Foundation.h>
#import "ObjcVCMCapturer.h"
#import "ARCustomVideoFilter.h"

@interface ObjcCapturer: NSObject

@end

class ObjcVCMCapturer : public MgrRender, public rtc::VideoSinkInterface <webrtc::VideoFrame> {
public:
    
    ObjcVCMCapturer(size_t width,
                    size_t height,
                    size_t target_fps,
                    size_t capture_device_index);
    virtual ~ObjcVCMCapturer();
    
    void SetVideoSource(const rtc::scoped_refptr<webrtc::VideoTrackSourceInterface>vidSource);
    
    void StartCapture();
    void StopCapture();
    void SwitchCapture();
    void SetBeautyEffect(bool enable);
    
    void OnFrame(const webrtc::VideoFrame& frame) override;

private:
	size_t width_;
	size_t height_;
    size_t target_fps_;
    size_t capture_device_index_;
    rtc::scoped_refptr<webrtc::VideoTrackSourceInterface> video_source_;
    ObjcCapturer *objcCapturer_;
};

#endif	/// __IOS_RENDERER_H__


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

#ifndef WEBRTC_API_OBJC_GPUIMAGE_VIDEO_CAPTURER_H_
#define WEBRTC_API_OBJC_GPUIMAGE_VIDEO_CAPTURER_H_

#include "webrtc/base/thread.h"
#include "webrtc/media/base/videocapturer.h"
#include "webrtc/video_frame.h"
#include "VideoCapture.h"

#import <AVFoundation/AVFoundation.h>
#import <UIKit/UIKit.h>

namespace webrtc {
    
    class GPUImageVideoCapturer : public cricket::VideoCapturer {
    public:
        GPUImageVideoCapturer(UIView *view, bool bBackCamera);
        ~GPUImageVideoCapturer();
        
        cricket::CaptureState Start(const cricket::VideoFormat& format) override;
        void Stop() override;
        bool IsRunning() override;
        bool IsScreencast() const override {
            return false;
        }
        bool GetPreferredFourccs(std::vector<uint32_t> *fourccs) override {
            fourccs->push_back(cricket::FOURCC_NV12);
            return true;
        }
                
        /** Returns the active capture session. */
        VideoCapture* GetCaptureSession();
        
        /** Switches the camera being used (either front or back). */
        void SetUseBackCamera(bool useBackCamera);
        
        bool GetUseBackCamera() const;
        
        void SetBeautyFace(bool beautyFace);
        
        void SetVideoEnable(bool enabled);
        
        /**
         * Converts the sample buffer into a cricket::CapturedFrame and signals the
         * frame for capture.
         */
        void CaptureYUVData(uint8_t *pData, int32_t yPlaneWidth, int32_t yPlaneHeight, int32_t frameSize);
        
    private:
        /**
         * Used to signal frame capture on the thread that capturer was started on.
         */
        void SignalFrameCapturedOnStartThread(const cricket::CapturedFrame *frame);
        
        void OnFrameMessage(CVImageBufferRef image_buffer, int64_t capture_time_ns);
        
        rtc::Thread *_startThread;  // Set in Start(), unset in Stop().
        VideoCapture *_capturer;
        webrtc::I420BufferPool _buffer_pool;
       
        bool _useBackCamera;
        bool _useBeautyFace;
    };  // GPUImageVideoCapturer
    
}  // namespace webrtc

#endif  // WEBRTC_API_OBJC_GPUIMAGE_VIDEO_CAPTURER_H_

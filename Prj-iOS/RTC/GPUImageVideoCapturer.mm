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

#include "GPUImageVideoCapturer.h"

#include "webrtc/base/bind.h"

#import <AVFoundation/AVFoundation.h>
#import <Foundation/Foundation.h>
#import <UIKit/UIKit.h>

#import "WebRTC/RTCDispatcher.h"
#include "webrtc/common_video/include/corevideo_frame_buffer.h"

static cricket::VideoFormat const kDefaultFormat_QHD = cricket::VideoFormat(1280, 720,
                                                                            cricket::VideoFormat::FpsToInterval(30),
                                                                            cricket::FOURCC_NV12);
static cricket::VideoFormat const kDefaultFormat_VGA = cricket::VideoFormat(640, 480,
                     cricket::VideoFormat::FpsToInterval(30),
                     cricket::FOURCC_NV12);
static cricket::VideoFormat const kDefaultFormat_QVGA = cricket::VideoFormat(320, 240,
                                                                            cricket::VideoFormat::FpsToInterval(30),
                                                                            cricket::FOURCC_NV12);
static cricket::VideoFormat const kDefaultFormat_QQVGA = cricket::VideoFormat(160, 120,
                                                                             cricket::VideoFormat::FpsToInterval(30),
                                                                             cricket::FOURCC_NV12);

static NSString* VideoFormatToSessionPreset(const cricket::VideoFormat& format)
{
    if (format == kDefaultFormat_QHD) {
        return AVCaptureSessionPreset1280x720;
    } else if(format == kDefaultFormat_VGA) {
        return AVCaptureSessionPreset640x480;
    } else if(format == kDefaultFormat_QVGA) {
        return AVCaptureSessionPreset352x288;
    }
    
    return AVCaptureSessionPreset640x480;
}

namespace webrtc {
    
    GPUImageVideoCapturer::GPUImageVideoCapturer(UIView *view, bool bBackCamera)
    : _capturer(nil), _startThread(nullptr) {
        // Set our supported formats. This matches kDefaultPreset.
        std::vector<cricket::VideoFormat> supportedFormats;
        supportedFormats.push_back(cricket::VideoFormat(kDefaultFormat_QHD));
        supportedFormats.push_back(cricket::VideoFormat(kDefaultFormat_VGA));
        supportedFormats.push_back(cricket::VideoFormat(kDefaultFormat_QVGA));
        supportedFormats.push_back(cricket::VideoFormat(kDefaultFormat_QQVGA));
        SetSupportedFormats(supportedFormats);
        _capturer = [[VideoCapture alloc] initWithVideo:this rect:view.bounds backCamera:bBackCamera];
        _capturer.preView = view;
        _useBackCamera = bBackCamera;
    }
    
    GPUImageVideoCapturer::~GPUImageVideoCapturer() {
        _capturer = nil;
    }
    
    cricket::CaptureState GPUImageVideoCapturer::Start(
                                                           const cricket::VideoFormat& format) {
       if (!_capturer) {
            LOG(LS_ERROR) << "Failed to create AVFoundation capturer.";
            return cricket::CaptureState::CS_FAILED;
        }
        if (_capturer.isRunning) {
            LOG(LS_ERROR) << "The capturer is already running.";
            return cricket::CaptureState::CS_FAILED;
        }
        
        // Keep track of which thread capture started on. This is the thread that
        // frames need to be sent to.
        RTC_DCHECK(!_startThread);
        _startThread = rtc::Thread::Current();
        
        SetCaptureFormat(&format);
        // This isn't super accurate because it takes a while for the AVCaptureSession
        // to spin up, and this call returns async.
        // TODO(tkchin): make this better.
        [_capturer startPreviewWithPreset:VideoFormatToSessionPreset(format)];
        SetCaptureState(cricket::CaptureState::CS_RUNNING);
        
        return cricket::CaptureState::CS_STARTING;
    }
    
    void GPUImageVideoCapturer::Stop() {
        [_capturer stopPreview];
        SetCaptureFormat(NULL);
        _startThread = nullptr;
    }
    
    bool GPUImageVideoCapturer::IsRunning() {
        return _capturer.isRunning;
    }
    
    VideoCapture* GPUImageVideoCapturer::GetCaptureSession() {
        return _capturer;
    }
    
    void GPUImageVideoCapturer::SetUseBackCamera(bool useBackCamera) {
        _useBackCamera = useBackCamera;
        if(_capturer)
        {
            if(useBackCamera)
                _capturer.captureDevicePosition =  AVCaptureDevicePositionBack;
            else
                _capturer.captureDevicePosition = AVCaptureDevicePositionFront;
        }
    }
    
    bool GPUImageVideoCapturer::GetUseBackCamera() const {
        return _useBackCamera;
    }
    
    void GPUImageVideoCapturer::SetBeautyFace(bool beautyFace)
    {
        _useBeautyFace = beautyFace;
        if(_capturer)
            _capturer.beautyFace = beautyFace;
    }
    
    void GPUImageVideoCapturer::SetVideoEnable(bool enabled)
    {
        if(_capturer)
           [_capturer setVideoEnable:enabled];
    }
    
    void GPUImageVideoCapturer::CaptureYUVData(uint8_t *pData, int32_t yPlaneWidth, int32_t yPlaneHeight, int32_t frameSize)
    {
        int64_t currentTime = rtc::TimeNanos();
        cricket::CapturedFrame frame;
        frame.width = yPlaneWidth;
        frame.height = yPlaneHeight;
        frame.pixel_width = 1;
        frame.pixel_height = 1;
        frame.fourcc = static_cast<uint32_t>(cricket::FOURCC_YU12);
        frame.time_stamp = currentTime;
        frame.data = pData;
        frame.data_size = frameSize;
        
        if (_startThread->IsCurrent()) {
            SignalFrameCaptured(this, &frame);
        } else {
            _startThread->Invoke<void>(RTC_FROM_HERE, 
                                       rtc::Bind(&GPUImageVideoCapturer::SignalFrameCapturedOnStartThread,
                                                 this, &frame));
        }
    }
    
    void GPUImageVideoCapturer::SignalFrameCapturedOnStartThread(
                                                                     const cricket::CapturedFrame *frame) {
        RTC_DCHECK(_startThread->IsCurrent());
        // This will call a superclass method that will perform the frame conversion
        // to I420.
        SignalFrameCaptured(this, frame);
    }
    
    void GPUImageVideoCapturer::OnFrameMessage(CVImageBufferRef image_buffer, int64_t capture_time_ns)
    {
        RTC_DCHECK(_startThread->IsCurrent());
        rtc::scoped_refptr<webrtc::VideoFrameBuffer> buffer =
        new rtc::RefCountedObject<webrtc::CoreVideoFrameBuffer>(image_buffer);
        const int captured_width = buffer->width();
        const int captured_height = buffer->height();
        int adapted_width;
        int adapted_height;
        int crop_width;
        int crop_height;
        int crop_x;
        int crop_y;
        int64_t translated_camera_time_us;
        if (!AdaptFrame(captured_width, captured_height,
                        capture_time_ns / rtc::kNumNanosecsPerMicrosec,
                        rtc::TimeMicros(), &adapted_width, &adapted_height,
                        &crop_width, &crop_height, &crop_x, &crop_y,
                        &translated_camera_time_us)) {
            CVBufferRelease(image_buffer);
            return;
        }
        if (adapted_width != captured_width || crop_width != captured_width ||
            adapted_height != captured_height || crop_height != captured_height) {
            // TODO(magjed): Avoid converting to I420.
            rtc::scoped_refptr<webrtc::I420Buffer> scaled_buffer(
                                                                 _buffer_pool.CreateBuffer(adapted_width, adapted_height));
            scaled_buffer->CropAndScaleFrom(buffer->NativeToI420Buffer(), crop_x,
                                            crop_y, crop_width, crop_height);
            buffer = scaled_buffer;
        }
        OnFrame(cricket::WebRtcVideoFrame(buffer, webrtc::kVideoRotation_0,
                                          translated_camera_time_us),
                captured_width, captured_height);
        CVBufferRelease(image_buffer);
    }
    
}  // namespace webrtc

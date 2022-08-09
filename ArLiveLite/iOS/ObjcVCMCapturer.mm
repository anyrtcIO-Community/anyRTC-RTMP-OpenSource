//
//  ObjcVCMCapturer.h
//  ARLiveLibrary
//
//  Created by 余生丶 on 2021/10/29.
//

#include "ObjcVCMCapturer.h"
#import "sdk/objc/components/capturer/RTCCameraVideoCapturer.h"
#import "sdk/objc/native/src/objc_frame_buffer.h"
#import "rtc_base/time_utils.h"
#include "WinVideoTrackSource.h"

@interface ObjcCapturer()<RTCVideoCapturerDelegate>

@property (nonatomic) ObjcVCMCapturer *video_capturer;
@property (nonatomic, strong) ARCustomVideoFilter *videoFilter;

- (void)startCapture:(int)widht height:(int)height fps:(int)fps position:(int)position;
- (void)stopCapture;

@end


@implementation ObjcCapturer {
    RTCCameraVideoCapturer *capturers_;
    int width_, height_, fps_;
    // 1 back 2 front
    int position_;
}

- (instancetype)init {
    if (self = [super init]) {
        capturers_ = [[RTCCameraVideoCapturer alloc] initWithDelegate:self];
    }
    return self;
}

- (void)startCapture:(int)widht height:(int)height fps:(int)fps position:(int)position {
    if (capturers_) {
        width_ = widht; height_ = height; fps_ = fps; position_ = position;
        dispatch_async(dispatch_get_main_queue(), ^{
            AVCaptureDevice *device = [self findDeviceForPosition:(AVCaptureDevicePosition)position];
            AVCaptureDeviceFormat *format = [self selectFormatForDevice:device];
            NSInteger result = [self selectFpsForFormat:format withFps:fps];
            [self->capturers_ startCaptureWithDevice:device format:format fps:result];
        });
    }
}

- (void)stopCapture {
    if (capturers_) {
        [capturers_ stopCapture];
    }
}

- (int)switchCamera {
    position_ = (position_ == 1) ? 2 : 1;
    if (capturers_) {
        AVCaptureDevice *device = [self findDeviceForPosition:(AVCaptureDevicePosition)position_];
        AVCaptureDeviceFormat *format = [self selectFormatForDevice:device];
        NSInteger result = [self selectFpsForFormat:format withFps:self->fps_];
        [capturers_ startCaptureWithDevice:device format:format fps:result];
    }
    return 0;
}

- (void)setBeauty:(bool)enable {
    self.videoFilter.isBeauty = enable;
}

- (void)capturer:(nonnull RTCVideoCapturer *)capturer didCaptureVideoFrame:(nonnull RTCVideoFrame *)frame {
    const int64_t timestamp_us = frame.timeStampNs / rtc::kNumNanosecsPerMicrosec;
       rtc::scoped_refptr<webrtc::VideoFrameBuffer> buffer =
           new rtc::RefCountedObject<webrtc::ObjCFrameBuffer>(frame.buffer);
       webrtc::VideoRotation rotation = (webrtc::VideoRotation)frame.rotation;
    if (_video_capturer) {
        _video_capturer->OnFrame(webrtc::VideoFrame::Builder()
                               .set_video_frame_buffer(buffer)
                               .set_rotation(rotation)
                               .set_timestamp_us(timestamp_us)
                               .build());
    }
}

- (CVPixelBufferRef)didCaptureBeautyOutput:(CVPixelBufferRef)bufferRef {
    if (self.videoFilter.isBeauty) {
        self.videoFilter.pixelBuffer = bufferRef;
        CVPixelBufferRef outputPixelBuffer = self.videoFilter.outputPixelBuffer;
        return outputPixelBuffer;
    }
    return nil;
}

- (ARCustomVideoFilter *)videoFilter {
    if (!_videoFilter) {
        _videoFilter = [[ARCustomVideoFilter alloc] init];
        _videoFilter.isBeauty = NO;
        _videoFilter.lighteningLevel= 0.7;
        _videoFilter.rednessLevel = 0.1;
        _videoFilter.smoothnessLevel = 0.5;
    }
    return _videoFilter;
}

- (void)dealloc {
    NSLog(@"ObjcCapturer dealloc");
}

//MARK: - Private

- (AVCaptureDevice *)findDeviceForPosition:(AVCaptureDevicePosition)position {
    NSArray<AVCaptureDevice *> *captureDevices = [RTCCameraVideoCapturer captureDevices];
    for (AVCaptureDevice *device in captureDevices) {
        if (device.position == position) {
            return device;
        }
    }
    return captureDevices[0];
}

- (AVCaptureDeviceFormat *)selectFormatForDevice:(AVCaptureDevice *)device {
    NSArray<AVCaptureDeviceFormat *> *formats =
      [RTCCameraVideoCapturer supportedFormatsForDevice:device];
    int targetWidth = width_;
    int targetHeight = height_;
    if (width_ > height_) {
        targetWidth = width_;
        targetHeight = height_;
    } else {
        targetWidth = height_;
        targetHeight = width_;
    }
    
    AVCaptureDeviceFormat *selectedFormat = nil;
    int currentDiff = INT_MAX;

    for (AVCaptureDeviceFormat *format in formats) {
        CMVideoDimensions dimension = CMVideoFormatDescriptionGetDimensions(format.formatDescription);
        FourCharCode pixelFormat = CMFormatDescriptionGetMediaSubType(format.formatDescription);
        int diff = abs(targetWidth - dimension.width) + abs(targetHeight - dimension.height);
        if (diff < currentDiff) {
          selectedFormat = format;
          currentDiff = diff;
        } else if (diff == currentDiff && pixelFormat == [capturers_ preferredOutputPixelFormat]) {
          selectedFormat = format;
        }
    }
    return selectedFormat;
}

- (NSInteger)selectFpsForFormat:(AVCaptureDeviceFormat *)format withFps:(int)fps {
    Float64 maxSupportedFramerate = 0;
    for (AVFrameRateRange *fpsRange in format.videoSupportedFrameRateRanges) {
        maxSupportedFramerate = fmax(maxSupportedFramerate, fpsRange.maxFrameRate);
    }
    
    NSInteger result = 30;
    result = fmin(maxSupportedFramerate, 30);
    if (result < fps) {
        return result;
    } else {
        return fps;
    }
}

@end

//MARK: - ObjcVCMCapturer

ObjcVCMCapturer::ObjcVCMCapturer(size_t width,
                size_t height,
                size_t target_fps,
                size_t capture_device_index) {
    if (objcCapturer_ == NULL) {
        objcCapturer_ = [[ObjcCapturer alloc] init];
        objcCapturer_.video_capturer = this;
        width_ = width;
        height_ = height;
        target_fps_ = target_fps;
        capture_device_index_ = capture_device_index;
    }
}

ObjcVCMCapturer::~ObjcVCMCapturer() {
    if (objcCapturer_) {
        objcCapturer_.video_capturer = nil;
        StopCapture();
        objcCapturer_ = nil;
    }
}

void ObjcVCMCapturer::SetVideoSource(const rtc::scoped_refptr<webrtc::VideoTrackSourceInterface>vidSource) {
    video_source_ = vidSource;
}

void ObjcVCMCapturer::StartCapture() {
    if (objcCapturer_) {
        [objcCapturer_ startCapture:(int)width_ height:(int)height_ fps:(int)target_fps_ position:(int)capture_device_index_];
    }
}

void ObjcVCMCapturer::StopCapture() {
    if (objcCapturer_) {
        [objcCapturer_ stopCapture];
    }
}

void ObjcVCMCapturer::SwitchCapture() {
    if (objcCapturer_) {
        [objcCapturer_ switchCamera];
    }
}

void ObjcVCMCapturer::SetBeautyEffect(bool enable) {
    if (objcCapturer_) {
        [objcCapturer_ setBeauty:enable];
    }
}

void ObjcVCMCapturer::OnFrame(const webrtc::VideoFrame& frame) {
    if (video_source_ != NULL) {
        ((WinVideoTrackSource *)video_source_.get())->FrameCaptured(frame);
    }
}

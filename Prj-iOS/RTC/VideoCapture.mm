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

#import "VideoCapture.h"
#import "GPUImage.h"
#import "GPUImageBeautifyFilter.h"
#import "GPUImageEmptyFilter.h"
#include "GPUImageVideoCapturer.h"
#include "third_party/libyuv/include/libyuv.h"
@interface VideoCapture() {
    rtc::CriticalSection cs_capture_;
}
@property (nonatomic, strong) GPUImageVideoCamera *videoCamera;
@property (nonatomic, strong) GPUImageView *gpuImageView;
@property (nonatomic, strong) UIButton *beautifyButton;
//@property(nonatomic, strong) GPUImageCropFilter *cropfilter;
@property(nonatomic, strong) GPUImageOutput<GPUImageInput> *filter;
@property(nonatomic, strong) GPUImageOutput<GPUImageInput> *emptyFilter;
@property (nonatomic, assign) BOOL isPreviewing;
@property (nonatomic, assign) BOOL isRunning;
@end
@implementation VideoCapture
@synthesize nWidth;
@synthesize nHeight;
@synthesize bVideoEnable;

- (void)dealloc{
    [UIApplication sharedApplication].idleTimerDisabled = NO;
    [[NSNotificationCenter defaultCenter] removeObserver:self];
    [_videoCamera stopCameraCapture];
    if(_gpuImageView.superview)
        [_gpuImageView removeFromSuperview];
    _gpuImageView = nil;
    if(_dst != NULL)
    {
        delete []_dst;
        _dst = NULL;
    }
}

#pragma mark -- LifeCycle
- (instancetype)initWithVideo:(webrtc::GPUImageVideoCapturer *)capturer rect:(CGRect)rect backCamera:(bool)bBackCamera{
    if(self = [super init]){
        _capturer = capturer;
        AVCaptureDevicePosition position = AVCaptureDevicePositionFront;
        if(bBackCamera)
            position = AVCaptureDevicePositionBack;
        _videoCamera = [[GPUImageVideoCamera alloc] initWithCameraPosition:position];
        _videoCamera.outputImageOrientation = UIInterfaceOrientationPortrait;
        _videoCamera.horizontallyMirrorFrontFacingCamera = YES;
        _videoCamera.horizontallyMirrorRearFacingCamera = NO;
        _videoCamera.frameRate = 20;
        self.nWidth = 480;
        self.nHeight = 640;
        self.bVideoEnable = false;
        
        _gpuImageView = [[GPUImageView alloc] initWithFrame:rect];
        [_gpuImageView setFillMode:kGPUImageFillModePreserveAspectRatioAndFill];
        [_gpuImageView setAutoresizingMask:UIViewAutoresizingFlexibleWidth | UIViewAutoresizingFlexibleHeight];
        
        [[NSNotificationCenter defaultCenter] addObserver:self selector:@selector(willEnterBackground:) name:UIApplicationWillResignActiveNotification object:nil];
        [[NSNotificationCenter defaultCenter] addObserver:self selector:@selector(willEnterForeground:) name:UIApplicationDidBecomeActiveNotification object:nil];
        
        self.beautyFace = YES;
        self.isPreviewing = NO;
        self.isClipVideo = NO;
        _dst = NULL;
    }
    return self;
}

- (void)startPreviewWithPreset:(NSString*)sessionPreset{
    rtc::CritScope cs(&cs_capture_);
    if(_isRunning)
        return;
    if (!_isPreviewing) {
        _isPreviewing = YES;
        [UIApplication sharedApplication].idleTimerDisabled = YES;
        dispatch_async(dispatch_get_main_queue(), ^
        {
            [_videoCamera configureCaptureSession:sessionPreset];
            [_videoCamera startCameraCapture];
        });
        
    }
    _isRunning = true;
}
- (void)stopPreview {
    rtc::CritScope cs(&cs_capture_);
    if(!_isRunning)
        return;
    if (_isPreviewing) {
        _isPreviewing = NO;
        [UIApplication sharedApplication].idleTimerDisabled = YES;
        dispatch_async(dispatch_get_main_queue(), ^{
             [_videoCamera stopCameraCapture];
        });
       
    }
    _isRunning = false;
    _capturer = nil;
}

#pragma mark -- Setter Getter
- (void)setPreView:(UIView *)preView{
     if(_gpuImageView.superview)
        [_gpuImageView removeFromSuperview];
    [preView insertSubview:_gpuImageView atIndex:0];
}

- (UIView*)preView{
    return _gpuImageView.superview;
}

- (void)setCaptureDevicePosition:(AVCaptureDevicePosition)captureDevicePosition{
    [_videoCamera rotateCamera];
}

- (AVCaptureDevicePosition)captureDevicePosition{
    return [_videoCamera cameraPosition];
}

- (void)setVideoFrameRate:(NSInteger)videoFrameRate{
    if(videoFrameRate <= 0) return;
    if(videoFrameRate == _videoCamera.frameRate) return;
    _videoCamera.frameRate = (uint32_t)videoFrameRate;
}

- (NSInteger)videoFrameRate{
    return _videoCamera.frameRate;
}
- (void)setBeautyFace:(BOOL)beautyFace{
    if(_beautyFace == beautyFace) return;
    
    _beautyFace = beautyFace;
    [_emptyFilter removeAllTargets];
    [_filter removeAllTargets];
   // [_cropfilter removeAllTargets];
    [_videoCamera removeAllTargets];
    
    if(_beautyFace){
        _filter = [[GPUImageBeautifyFilter alloc] init];
        _emptyFilter = [[GPUImageEmptyFilter alloc] init];
    }else{
        _filter = [[GPUImageEmptyFilter alloc] init];
    }
    
    __weak typeof(self) _self = self;
    [_filter setFrameProcessingCompletionBlock:^(GPUImageOutput *output, CMTime time) {
        [_self processVideo:output];
    }];
    
//    if(self.isClipVideo){///<  裁剪为16:9
//        _cropfilter = [[GPUImageCropFilter alloc] initWithCropRegion:CGRectMake(0.125, 0, 0.75, 1)];
//        [_videoCamera addTarget:_cropfilter];
//        [_cropfilter addTarget:_filter];
//    }else{
//        [_videoCamera addTarget:_filter];
//    }
     [_videoCamera addTarget:_filter];
    
    if (beautyFace) {
        [_filter addTarget:_emptyFilter];
        if(_gpuImageView) [_emptyFilter addTarget:_gpuImageView];
    } else {
        if(_gpuImageView) [_filter addTarget:_gpuImageView];
    }
    
}

- (void)setVideoEnable:(bool)enabled
{
    self.bVideoEnable = enabled;
}

#pragma mark -- Custom Method
- (void) processVideo:(GPUImageOutput *)output{
    rtc::CritScope cs(&cs_capture_);
    if (!_isRunning) {
        return;
    }
    @autoreleasepool {
        GPUImageFramebuffer *imageFramebuffer = output.framebufferForOutput;
		
        size_t width = imageFramebuffer.size.width;
        size_t height = imageFramebuffer.size.height;
        uint32_t size = width * height * 3 / 2;
        
        if(self.nWidth != width || self.nHeight != height)
        {
            self.nWidth = width;
            self.nHeight = height;
            if(_dst)
                delete[] _dst;
            _dst = NULL;
        }
        if(_dst == NULL)
        {
            _dst = new uint8_t[size];
        }
        uint8_t* y_pointer = (uint8_t*)_dst;
        uint8_t* u_pointer = (uint8_t*)y_pointer + width*height;
        uint8_t* v_pointer = (uint8_t*)u_pointer + width*height/4;
        int y_pitch = width;
        int u_pitch = (width + 1) >> 1;
        int v_pitch = (width + 1) >> 1;
        
        libyuv::ARGBToI420([imageFramebuffer byteBuffer], width * 4, y_pointer, y_pitch, u_pointer, u_pitch, v_pointer, v_pitch, width, height);
        if(self.bVideoEnable)
            libyuv::I420Rect(y_pointer, y_pitch, u_pointer, u_pitch, v_pointer, v_pitch, 0, 0, width, height, 32, 128, 128);
        
        if(_capturer != nil)
            _capturer->CaptureYUVData(_dst, width, height, size);
    }
}

#pragma mark Notification

- (void)willEnterBackground:(NSNotification*)notification{
    [UIApplication sharedApplication].idleTimerDisabled = NO;
    [_videoCamera pauseCameraCapture];
    runSynchronouslyOnVideoProcessingQueue(^{
        glFinish();
    });
}

- (void)willEnterForeground:(NSNotification*)notification{
    [_videoCamera resumeCameraCapture];
    [UIApplication sharedApplication].idleTimerDisabled = YES;
}
@end

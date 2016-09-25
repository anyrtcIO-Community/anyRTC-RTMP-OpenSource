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

#import <Foundation/Foundation.h>
#import <AVFoundation/AVFoundation.h>
#import <UIKit/UIKit.h>
NS_ASSUME_NONNULL_BEGIN
namespace webrtc {
    class GPUImageVideoCapturer;
}

@class VideoCapture;
/** LMVideoCapture callback videoData */
@protocol VideoCaptureDelegate <NSObject>
- (void)captureOutput:(nullable VideoCapture*)capture pixelBuffer:(nullable CVImageBufferRef)pixelBuffer;
@end

@interface VideoCapture : NSObject
{
    webrtc::GPUImageVideoCapturer *_capturer;
    uint8_t *_dst;;
    
}

#pragma mark - Attribute
///=============================================================================
/// @name Attribute
///=============================================================================

/** The delegate of the capture. captureData callback */
@property (nullable,nonatomic, weak) id<VideoCaptureDelegate> delegate;

/** The running control start capture or stop capture*/
@property(nonatomic, readonly) BOOL isRunning;

/** The preView will show OpenGL ES view*/
@property (null_resettable,nonatomic, strong) UIView * preView;

/** The captureDevicePosition control camraPosition ,default front*/
@property (nonatomic, assign) AVCaptureDevicePosition captureDevicePosition;

/** The beautyFace control capture shader filter empty or beautiy default YES*/
@property (nonatomic, assign) BOOL beautyFace;

@property (nonatomic, assign) BOOL bVideoEnable;

@property (nonatomic, assign) BOOL isClipVideo;

@property (nonatomic, assign) size_t nWidth;

@property (nonatomic, assign) size_t nHeight;

/** The videoFrameRate control videoCapture output data count */
@property (nonatomic, assign) NSInteger videoFrameRate;

#pragma mark - Initializer
///=============================================================================
/// @name Initializer
///=============================================================================
- (nullable instancetype)init UNAVAILABLE_ATTRIBUTE;
+ (nullable instancetype)new UNAVAILABLE_ATTRIBUTE;
/**
 The designated initializer. Multiple instances with the same configuration will make the
 capture unstable.
 */
- (nullable instancetype)initWithVideo:(webrtc::GPUImageVideoCapturer *)capturer rect:(CGRect)rect backCamera:(bool)bBackCamera;
/**
 *  开始预览
 */
- (void)startPreviewWithPreset:(NSString*)sessionPreset;
/**
 *  停止预览
 */
- (void)stopPreview;

- (void)setVideoEnable:(bool)enabled;
NS_ASSUME_NONNULL_END
@end

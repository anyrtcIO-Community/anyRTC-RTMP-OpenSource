/*
 *  Copyright 2015 The WebRTC project@AnyRTC authors. All Rights Reserved.
 *
 *  Use of this source code is governed by a BSD-style license
 *  that can be found in the LICENSE file in the root of the source
 *  tree. An additional intellectual property rights grant can be found
 *  in the file PATENTS.  All contributing project authors may
 *  be found in the AUTHORS file in the root of the source tree.
 */

#import "RTCVideoRendererAdapter+Private.h"

#import "RTCVideoFrame+Private.h"

#include <memory>

#include "webrtc/media/engine/webrtcvideoframe.h"

namespace webrtc {

class VideoRendererAdapter
    : public rtc::VideoSinkInterface<cricket::VideoFrame> {
 public:
  VideoRendererAdapter(RTCVideoRendererAdapter* adapter) {
    adapter_ = adapter;
    size_ = CGSizeZero;
  }

  void OnFrame(const cricket::VideoFrame& nativeVideoFrame) override {
    RTCVideoFrame *videoFrame = nil;
    // Rotation of native handles is unsupported right now. Convert to CPU
    // I420 buffer for rotation before calling the rotation method otherwise
    // it will hit a DCHECK.
    if (nativeVideoFrame.rotation() != webrtc::kVideoRotation_0 &&
        nativeVideoFrame.video_frame_buffer()->native_handle()) {
      rtc::scoped_refptr<webrtc::VideoFrameBuffer> i420Buffer =
          nativeVideoFrame.video_frame_buffer()->NativeToI420Buffer();
      std::unique_ptr<cricket::VideoFrame> cpuFrame(
          new cricket::WebRtcVideoFrame(i420Buffer,
                                        nativeVideoFrame.rotation(),
                                        nativeVideoFrame.timestamp_us()));
      const cricket::VideoFrame *rotatedFrame =
          cpuFrame->GetCopyWithRotationApplied();
      videoFrame = [[RTCVideoFrame alloc] initWithNativeFrame:rotatedFrame];
    } else {
      const cricket::VideoFrame *rotatedFrame =
          nativeVideoFrame.GetCopyWithRotationApplied();
      videoFrame = [[RTCVideoFrame alloc] initWithNativeFrame:rotatedFrame];
    }
    CGSize current_size = CGSizeMake(videoFrame.width, videoFrame.height);
    if (!CGSizeEqualToSize(size_, current_size)) {
      size_ = current_size;
      [adapter_.videoRenderer setSize:size_];
    }
    [adapter_.videoRenderer renderFrame:videoFrame];
  }

 private:
  __weak RTCVideoRendererAdapter *adapter_;
  CGSize size_;
};
}

@implementation RTCVideoRendererAdapter {
  std::unique_ptr<webrtc::VideoRendererAdapter> _adapter;
}

@synthesize videoRenderer = _videoRenderer;

- (instancetype)initWithNativeRenderer:(id<RTCVideoRenderer>)videoRenderer {
  NSParameterAssert(videoRenderer);
  if (self = [super init]) {
    _videoRenderer = videoRenderer;
    _adapter.reset(new webrtc::VideoRendererAdapter(self));
  }
  return self;
}

- (rtc::VideoSinkInterface<cricket::VideoFrame> *)nativeVideoRenderer {
  return _adapter.get();
}

@end

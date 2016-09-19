/*
 *  Copyright 2015 The WebRTC@AnyRTC project authors. All Rights Reserved.
 *
 *  Use of this source code is governed by a BSD-style license
 *  that can be found in the LICENSE file in the root of the source
 *  tree. An additional intellectual property rights grant can be found
 *  in the file PATENTS.  All contributing project authors may
 *  be found in the AUTHORS file in the root of the source tree.
 */

#import "RTCVideoFrame+Private.h"

#include <memory>

@implementation RTCVideoFrame {
  std::unique_ptr<cricket::VideoFrame> _videoFrame;
  rtc::scoped_refptr<webrtc::VideoFrameBuffer> _i420Buffer;
}

- (size_t)width {
  return _videoFrame->width();
}

- (size_t)height {
  return _videoFrame->height();
}

// TODO(nisse): chromaWidth and chromaHeight are used only in
// RTCOpenGLVideoRenderer.mm. Update, and then delete these
// properties.
- (size_t)chromaWidth {
  return (self.width + 1) / 2;
}

- (size_t)chromaHeight {
  return (self.height + 1) / 2;
}

- (const uint8_t *)yPlane {
  if (!self.i420Buffer) {
    return nullptr;
  }
  return self.i420Buffer->DataY();
}

- (const uint8_t *)uPlane {
  if (!self.i420Buffer) {
    return nullptr;
  }
  return self.i420Buffer->DataU();
}

- (const uint8_t *)vPlane {
  if (!self.i420Buffer) {
    return nullptr;
  }
  return self.i420Buffer->DataV();
}

- (int32_t)yPitch {
  if (!self.i420Buffer) {
    return 0;
  }
  return self.i420Buffer->StrideY();
}

- (int32_t)uPitch {
  if (!self.i420Buffer) {
    return 0;
  }
  return self.i420Buffer->StrideU();
}

- (int32_t)vPitch {
  if (!self.i420Buffer) {
    return 0;
  }
  return self.i420Buffer->StrideV();
}

- (int64_t)timeStamp {
  return _videoFrame->GetTimeStamp();
}

- (CVPixelBufferRef)nativeHandle {
  return static_cast<CVPixelBufferRef>(
      _videoFrame->video_frame_buffer()->native_handle());
}

- (void)convertBufferIfNeeded {
  if (!_i420Buffer) {
    if (_videoFrame->video_frame_buffer()->native_handle()) {
      // Convert to I420.
      _i420Buffer = _videoFrame->video_frame_buffer()->NativeToI420Buffer();
    } else {
      // Should already be I420.
      _i420Buffer = _videoFrame->video_frame_buffer();
    }
  }
}

#pragma mark - Private

- (instancetype)initWithNativeFrame:(const cricket::VideoFrame *)nativeFrame {
  if (self = [super init]) {
    // Keep a shallow copy of the video frame. The underlying frame buffer is
    // not copied.
    _videoFrame.reset(nativeFrame->Copy());
  }
  return self;
}

- (rtc::scoped_refptr<webrtc::VideoFrameBuffer>)i420Buffer {
  [self convertBufferIfNeeded];
  return _i420Buffer;
}

@end

/*
 *  Copyright (c) 2004 The WebRTC@AnyRTC project authors. All Rights Reserved.
 *
 *  Use of this source code is governed by a BSD-style license
 *  that can be found in the LICENSE file in the root of the source
 *  tree. An additional intellectual property rights grant can be found
 *  in the file PATENTS.  All contributing project authors may
 *  be found in the AUTHORS file in the root of the source tree.
 */

#ifndef WEBRTC_MEDIA_BASE_VIDEOFRAME_H_
#define WEBRTC_MEDIA_BASE_VIDEOFRAME_H_

#include "webrtc/base/basictypes.h"
#include "webrtc/base/stream.h"
#include "webrtc/common_video/include/video_frame_buffer.h"
#include "webrtc/common_video/rotation.h"

namespace cricket {

// Represents a YUV420 (a.k.a. I420) video frame.

// TODO(nisse): This class duplicates webrtc::VideoFrame. There's
// ongoing work to merge the classes. See
// https://bugs.chromium.org/p/webrtc/issues/detail?id=5682.
class VideoFrame {
 public:
  VideoFrame() {}
  virtual ~VideoFrame() {}

  // Basic accessors.
  // Note this is the width and height without rotation applied.
  virtual int width() const = 0;
  virtual int height() const = 0;

  // Returns the underlying video frame buffer. This function is ok to call
  // multiple times, but the returned object will refer to the same memory.
  virtual const rtc::scoped_refptr<webrtc::VideoFrameBuffer>&
  video_frame_buffer() const = 0;

  // System monotonic clock, same timebase as rtc::TimeMicros().
  virtual int64_t timestamp_us() const = 0;
  virtual void set_timestamp_us(int64_t time_us) = 0;

  // Deprecated methods, for backwards compatibility.
  // TODO(nisse): Delete when usage in Chrome and other applications
  // have been replaced.
  virtual int64_t GetTimeStamp() const {
    return rtc::kNumNanosecsPerMicrosec * timestamp_us();
  }
  virtual void SetTimeStamp(int64_t time_ns) {
    set_timestamp_us(time_ns / rtc::kNumNanosecsPerMicrosec);
  }

  // Indicates the rotation angle in degrees.
  virtual webrtc::VideoRotation rotation() const = 0;

  // Make a shallow copy of the frame. The frame buffer itself is not copied.
  // Both the current and new VideoFrame will share a single reference-counted
  // frame buffer.

  // TODO(nisse): Deprecated, to be deleted in the cricket::VideoFrame and
  // webrtc::VideoFrame merge. To make a copy, use the cricket::WebRtcVideoFrame
  // constructor passing video_frame_buffer(), rotation() and timestamp_us() as
  // arguments.
  virtual VideoFrame *Copy() const = 0;

  // Return a copy of frame which has its pending rotation applied. The
  // ownership of the returned frame is held by this frame.

  // TODO(nisse): Deprecated. Should be moved or deleted in the
  // cricket::VideoFrame and webrtc::VideoFrame merge, possibly with a helper
  // method on VideoFrameBuffer.
  virtual const VideoFrame* GetCopyWithRotationApplied() const = 0;

  // Converts the I420 data to RGB of a certain type such as ARGB and ABGR.
  // Returns the frame's actual size, regardless of whether it was written or
  // not (like snprintf). Parameters size and stride_rgb are in units of bytes.
  // If there is insufficient space, nothing is written.

  // TODO(nisse): Deprecated. Should be moved or deleted in the
  // cricket::VideoFrame and webrtc::VideoFrame merge. Use
  // libyuv::ConvertFromI420 directly instead.
  virtual size_t ConvertToRgbBuffer(uint32_t to_fourcc,
                                    uint8_t* buffer,
                                    size_t size,
                                    int stride_rgb) const;

  // Tests if sample is valid. Returns true if valid.

  // TODO(nisse): Deprecated. Should be deleted in the cricket::VideoFrame and
  // webrtc::VideoFrame merge. Validation of sample_size possibly moved to
  // libyuv::ConvertToI420. As an initial step, demote this method to protected
  // status. Used only by WebRtcVideoFrame::Reset.
  static bool Validate(uint32_t fourcc,
                       int w,
                       int h,
                       const uint8_t* sample,
                       size_t sample_size);
};

}  // namespace cricket

#endif  // WEBRTC_MEDIA_BASE_VIDEOFRAME_H_

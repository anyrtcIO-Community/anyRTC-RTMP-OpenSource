/*
 *  Copyright (c) 2011 The WebRTC@AnyRTC project authors. All Rights Reserved.
 *
 *  Use of this source code is governed by a BSD-style license
 *  that can be found in the LICENSE file in the root of the source
 *  tree. An additional intellectual property rights grant can be found
 *  in the file PATENTS.  All contributing project authors may
 *  be found in the AUTHORS file in the root of the source tree.
 */

#ifndef WEBRTC_MEDIA_ENGINE_WEBRTCVIDEOFRAME_H_
#define WEBRTC_MEDIA_ENGINE_WEBRTCVIDEOFRAME_H_

#include <memory>

#include "webrtc/base/buffer.h"
#include "webrtc/base/refcount.h"
#include "webrtc/base/scoped_ref_ptr.h"
#include "webrtc/common_types.h"
#include "webrtc/common_video/include/video_frame_buffer.h"
#include "webrtc/media/base/videoframe.h"

namespace cricket {

struct CapturedFrame;

// TODO(nisse): This class will be deleted when the cricket::VideoFrame and
// webrtc::VideoFrame classes are merged. See
// https://bugs.chromium.org/p/webrtc/issues/detail?id=5682. Try to use only the
// preferred constructor, and the non-deprecated methods of the VideoFrame base
// class.
class WebRtcVideoFrame : public VideoFrame {
 public:
  // TODO(nisse): Deprecated. Using the default constructor violates the
  // reasonable assumption that video_frame_buffer() returns a valid buffer.
  WebRtcVideoFrame();

  // Preferred constructor.
  WebRtcVideoFrame(const rtc::scoped_refptr<webrtc::VideoFrameBuffer>& buffer,
                   webrtc::VideoRotation rotation,
                   int64_t timestamp_us);

  // TODO(nisse): Deprecated, delete as soon as all callers have switched to the
  // above constructor with microsecond timestamp.
  WebRtcVideoFrame(const rtc::scoped_refptr<webrtc::VideoFrameBuffer>& buffer,
                   int64_t time_stamp_ns,
                   webrtc::VideoRotation rotation);

  ~WebRtcVideoFrame();

  // Creates a frame from a raw sample with FourCC "format" and size "w" x "h".
  // "h" can be negative indicating a vertically flipped image.
  // "dh" is destination height if cropping is desired and is always positive.
  // Returns "true" if successful.
  bool Init(uint32_t format,
            int w,
            int h,
            int dw,
            int dh,
            uint8_t* sample,
            size_t sample_size,
            int64_t time_stamp_ns,
            webrtc::VideoRotation rotation);

  // TODO(nisse): We're moving to have all timestamps use the same
  // time scale as rtc::TimeMicros. However, this method is used by
  // WebRtcVideoFrameFactory::CreateAliasedFrame this code path
  // currently does not conform to the new timestamp conventions and
  // may use the camera's own clock instead. It's unclear if this
  // should be fixed, or if instead all of the VideoFrameFactory
  // abstraction should be eliminated.
  bool Init(const CapturedFrame* frame, int dw, int dh, bool apply_rotation);

  void InitToEmptyBuffer(int w, int h);

  int width() const override;
  int height() const override;

  const rtc::scoped_refptr<webrtc::VideoFrameBuffer>& video_frame_buffer()
      const override;

  /* System monotonic clock */
  int64_t timestamp_us() const override { return timestamp_us_; }
  void set_timestamp_us(int64_t time_us) override { timestamp_us_ = time_us; };

  webrtc::VideoRotation rotation() const override { return rotation_; }

  VideoFrame* Copy() const override;

  size_t ConvertToRgbBuffer(uint32_t to_fourcc,
                            uint8_t* buffer,
                            size_t size,
                            int stride_rgb) const override;

  const VideoFrame* GetCopyWithRotationApplied() const override;

 protected:
  // Creates a frame from a raw sample with FourCC |format| and size |w| x |h|.
  // |h| can be negative indicating a vertically flipped image.
  // |dw| is destination width; can be less than |w| if cropping is desired.
  // |dh| is destination height, like |dw|, but must be a positive number.
  // Returns whether the function succeeded or failed.
  bool Reset(uint32_t format,
             int w,
             int h,
             int dw,
             int dh,
             uint8_t* sample,
             size_t sample_size,
             int64_t timestamp_us,
             webrtc::VideoRotation rotation,
             bool apply_rotation);

 private:
  // Tests mutate |rotation_|, so the base test class is a friend.
  friend class WebRtcVideoFrameTest;

  // An opaque reference counted handle that stores the pixel data.
  rtc::scoped_refptr<webrtc::VideoFrameBuffer> video_frame_buffer_;
  int64_t timestamp_us_;
  webrtc::VideoRotation rotation_;

  // This is mutable as the calculation is expensive but once calculated, it
  // remains const.
  mutable std::unique_ptr<VideoFrame> rotated_frame_;
};

}  // namespace cricket

#endif  // WEBRTC_MEDIA_ENGINE_WEBRTCVIDEOFRAME_H_

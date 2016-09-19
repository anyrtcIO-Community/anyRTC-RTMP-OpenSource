/*
 *  Copyright (c) 2014 The WebRTC@AnyRTC project authors. All Rights Reserved.
 *
 *  Use of this source code is governed by a BSD-style license
 *  that can be found in the LICENSE file in the root of the source
 *  tree. An additional intellectual property rights grant can be found
 *  in the file PATENTS.  All contributing project authors may
 *  be found in the AUTHORS file in the root of the source tree.
 */

#ifndef WEBRTC_MEDIA_BASE_VIDEOFRAMEFACTORY_H_
#define WEBRTC_MEDIA_BASE_VIDEOFRAMEFACTORY_H_

#include <memory>

#include "webrtc/common_video/include/i420_buffer_pool.h"
#include "webrtc/media/base/videoframe.h"

namespace cricket {

struct CapturedFrame;
class VideoFrame;

// Creates cricket::VideoFrames, or a subclass of cricket::VideoFrame
// depending on the subclass of VideoFrameFactory.
class VideoFrameFactory {
 public:
  VideoFrameFactory() : apply_rotation_(false) {}
  virtual ~VideoFrameFactory() {}

  // The returned frame aliases the aliased_frame if the input color
  // space allows for aliasing, otherwise a color conversion will
  // occur. Returns NULL if conversion fails.

  // The returned frame will be a center crop of |input_frame| with
  // size |cropped_width| x |cropped_height|.
  virtual VideoFrame* CreateAliasedFrame(const CapturedFrame* input_frame,
                                         int cropped_width,
                                         int cropped_height) const = 0;

  // The returned frame will be a center crop of |input_frame| with size
  // |cropped_width| x |cropped_height|, scaled to |output_width| x
  // |output_height|.
  virtual VideoFrame* CreateAliasedFrame(const CapturedFrame* input_frame,
                                         int cropped_input_width,
                                         int cropped_input_height,
                                         int output_width,
                                         int output_height) const;

  void SetApplyRotation(bool enable) { apply_rotation_ = enable; }

 protected:
  bool apply_rotation_;

 private:
  // An internal pool to avoid reallocations. It is mutable because it
  // does not affect behaviour, only performance.
  mutable webrtc::I420BufferPool pool_;
};

}  // namespace cricket

#endif  // WEBRTC_MEDIA_BASE_VIDEOFRAMEFACTORY_H_

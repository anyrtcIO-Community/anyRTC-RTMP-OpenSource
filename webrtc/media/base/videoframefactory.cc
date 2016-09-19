/*
 *  Copyright (c) 2014 The WebRTC@AnyRTC project authors. All Rights Reserved.
 *
 *  Use of this source code is governed by a BSD-style license
 *  that can be found in the LICENSE file in the root of the source
 *  tree. An additional intellectual property rights grant can be found
 *  in the file PATENTS.  All contributing project authors may
 *  be found in the AUTHORS file in the root of the source tree.
 */

#include "webrtc/media/base/videoframefactory.h"

#include <algorithm>
#include "webrtc/media/base/videocapturer.h"

namespace cricket {

VideoFrame* VideoFrameFactory::CreateAliasedFrame(
    const CapturedFrame* input_frame,
    int cropped_input_width,
    int cropped_input_height,
    int output_width,
    int output_height) const {
  std::unique_ptr<VideoFrame> cropped_input_frame(CreateAliasedFrame(
      input_frame, cropped_input_width, cropped_input_height));
  if (!cropped_input_frame)
    return nullptr;

  if (cropped_input_width == output_width &&
      cropped_input_height == output_height) {
    // No scaling needed.
    return cropped_input_frame.release();
  }

  // If the frame is rotated, we need to switch the width and height.
  if (apply_rotation_ &&
      (input_frame->rotation == webrtc::kVideoRotation_90 ||
       input_frame->rotation == webrtc::kVideoRotation_270)) {
    std::swap(output_width, output_height);
  }

  rtc::scoped_refptr<webrtc::I420Buffer> scaled_buffer(
      pool_.CreateBuffer(output_width, output_height));
  scaled_buffer->CropAndScaleFrom(cropped_input_frame->video_frame_buffer());

  return new WebRtcVideoFrame(scaled_buffer, cropped_input_frame->rotation(),
                              cropped_input_frame->timestamp_us());
}

}  // namespace cricket

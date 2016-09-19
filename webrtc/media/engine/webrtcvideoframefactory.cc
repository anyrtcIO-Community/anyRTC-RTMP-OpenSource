/*
 *  Copyright (c) 2014 The WebRTC@AnyRTC project authors. All Rights Reserved.
 *
 *  Use of this source code is governed by a BSD-style license
 *  that can be found in the LICENSE file in the root of the source
 *  tree. An additional intellectual property rights grant can be found
 *  in the file PATENTS.  All contributing project authors may
 *  be found in the AUTHORS file in the root of the source tree.
 */

#include <memory>

#include "webrtc/base/logging.h"
#include "webrtc/media/engine/webrtcvideoframe.h"
#include "webrtc/media/engine/webrtcvideoframefactory.h"

namespace cricket {

VideoFrame* WebRtcVideoFrameFactory::CreateAliasedFrame(
    const CapturedFrame* aliased_frame, int width, int height) const {
  std::unique_ptr<WebRtcVideoFrame> frame(new WebRtcVideoFrame());
  if (!frame->Init(aliased_frame, width, height, apply_rotation_)) {
    LOG(LS_ERROR) <<
        "Failed to create WebRtcVideoFrame in CreateAliasedFrame.";
    return NULL;
  }
  return frame.release();
}

}  // namespace cricket

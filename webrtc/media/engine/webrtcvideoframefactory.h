/*
 *  Copyright (c) 2014 The WebRTC@AnyRTC project authors. All Rights Reserved.
 *
 *  Use of this source code is governed by a BSD-style license
 *  that can be found in the LICENSE file in the root of the source
 *  tree. An additional intellectual property rights grant can be found
 *  in the file PATENTS.  All contributing project authors may
 *  be found in the AUTHORS file in the root of the source tree.
 */

#ifndef WEBRTC_MEDIA_ENGINE_WEBRTCVIDEOFRAMEFACTORY_H_
#define WEBRTC_MEDIA_ENGINE_WEBRTCVIDEOFRAMEFACTORY_H_

#include "webrtc/media/base/videoframefactory.h"

namespace cricket {

struct CapturedFrame;

// Creates instances of cricket::WebRtcVideoFrame.
class WebRtcVideoFrameFactory : public VideoFrameFactory {
 public:
  VideoFrame* CreateAliasedFrame(const CapturedFrame* aliased_frame,
                                 int width,
                                 int height) const override;
};

}  // namespace cricket

#endif  // WEBRTC_MEDIA_ENGINE_WEBRTCVIDEOFRAMEFACTORY_H_

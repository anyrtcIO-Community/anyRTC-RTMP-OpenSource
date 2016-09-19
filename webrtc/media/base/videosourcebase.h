/*
 *  Copyright (c) 2016 The WebRTC project authors. All Rights Reserved.
 *
 *  Use of this source code is governed by a BSD-style license
 *  that can be found in the LICENSE file in the root of the source
 *  tree. An additional intellectual property rights grant can be found
 *  in the file PATENTS.  All contributing project authors may
 *  be found in the AUTHORS file in the root of the source tree.
 */

#ifndef WEBRTC_MEDIA_BASE_VIDEOSOURCEBASE_H_
#define WEBRTC_MEDIA_BASE_VIDEOSOURCEBASE_H_

#include <vector>

#include "webrtc/base/thread_checker.h"
#include "webrtc/media/base/videoframe.h"
#include "webrtc/media/base/videosourceinterface.h"

namespace rtc {

// VideoSourceBase is not thread safe.
class VideoSourceBase : public VideoSourceInterface<cricket::VideoFrame> {
 public:
  VideoSourceBase();
  void AddOrUpdateSink(VideoSinkInterface<cricket::VideoFrame>* sink,
                       const VideoSinkWants& wants) override;
  void RemoveSink(VideoSinkInterface<cricket::VideoFrame>* sink) override;

 protected:
  struct SinkPair {
    SinkPair(VideoSinkInterface<cricket::VideoFrame>* sink,
             VideoSinkWants wants)
        : sink(sink), wants(wants) {}
    VideoSinkInterface<cricket::VideoFrame>* sink;
    VideoSinkWants wants;
  };
  SinkPair* FindSinkPair(const VideoSinkInterface<cricket::VideoFrame>* sink);

  const std::vector<SinkPair>& sink_pairs() const { return sinks_; }
  ThreadChecker thread_checker_;

 private:
  std::vector<SinkPair> sinks_;
};

}  // namespace rtc

#endif  // WEBRTC_MEDIA_BASE_VIDEOSOURCEBASE_H_

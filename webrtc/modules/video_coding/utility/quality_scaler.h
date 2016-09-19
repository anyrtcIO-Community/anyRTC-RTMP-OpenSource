/*
 *  Copyright (c) 2014 The WebRTC@AnyRTC project authors. All Rights Reserved.
 *
 *  Use of this source code is governed by a BSD-style license
 *  that can be found in the LICENSE file in the root of the source
 *  tree. An additional intellectual property rights grant can be found
 *  in the file PATENTS.  All contributing project authors may
 *  be found in the AUTHORS file in the root of the source tree.
 */

#ifndef WEBRTC_MODULES_VIDEO_CODING_UTILITY_QUALITY_SCALER_H_
#define WEBRTC_MODULES_VIDEO_CODING_UTILITY_QUALITY_SCALER_H_

#include "webrtc/common_video/include/i420_buffer_pool.h"
#include "webrtc/modules/video_coding/utility/moving_average.h"

namespace webrtc {
class QualityScaler {
 public:
  struct Resolution {
    int width;
    int height;
  };

  QualityScaler();
  void Init(int low_qp_threshold,
            int high_qp_threshold,
            int initial_bitrate_kbps,
            int width,
            int height,
            int fps);
  void ReportFramerate(int framerate);
  void ReportQP(int qp);
  void ReportDroppedFrame();
  void OnEncodeFrame(int width, int height);
  Resolution GetScaledResolution() const;
  rtc::scoped_refptr<VideoFrameBuffer> GetScaledBuffer(
      const rtc::scoped_refptr<VideoFrameBuffer>& frame);
  int downscale_shift() const { return downscale_shift_; }

  // QP is obtained from VP8-bitstream for HW, so the QP corresponds to the
  // bitstream range of [0, 127] and not the user-level range of [0,63].
  static const int kLowVp8QpThreshold;
  static const int kBadVp8QpThreshold;

  // H264 QP is in the range [0, 51].
  static const int kLowH264QpThreshold;
  static const int kBadH264QpThreshold;

 private:
  void AdjustScale(bool up);
  void UpdateTargetResolution(int frame_width, int frame_height);
  void ClearSamples();
  void UpdateSampleCounts();

  I420BufferPool pool_;

  size_t num_samples_downscale_;
  size_t num_samples_upscale_;
  int measure_seconds_upscale_;
  MovingAverage<int> average_qp_upscale_;
  MovingAverage<int> average_qp_downscale_;

  int framerate_;
  int low_qp_threshold_;
  int high_qp_threshold_;
  MovingAverage<int> framedrop_percent_;
  Resolution res_;

  int downscale_shift_;
};

}  // namespace webrtc

#endif  // WEBRTC_MODULES_VIDEO_CODING_UTILITY_QUALITY_SCALER_H_

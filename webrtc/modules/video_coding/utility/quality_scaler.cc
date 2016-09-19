/*
 *  Copyright (c) 2014 The WebRTC@AnyRTC project authors. All Rights Reserved.
 *
 *  Use of this source code is governed by a BSD-style license
 *  that can be found in the LICENSE file in the root of the source
 *  tree. An additional intellectual property rights grant can be found
 *  in the file PATENTS.  All contributing project authors may
 *  be found in the AUTHORS file in the root of the source tree.
 */

#include "webrtc/modules/video_coding/utility/quality_scaler.h"

namespace webrtc {

namespace {
static const int kMinFps = 5;
// Threshold constant used until first downscale (to permit fast rampup).
static const int kMeasureSecondsFastUpscale = 2;
static const int kMeasureSecondsUpscale = 5;
static const int kMeasureSecondsDownscale = 5;
static const int kFramedropPercentThreshold = 60;
// Min width/height to downscale to, set to not go below QVGA, but with some
// margin to permit "almost-QVGA" resolutions, such as QCIF.
static const int kMinDownscaleDimension = 140;
// Initial resolutions corresponding to a bitrate. Aa bit above their actual
// values to permit near-VGA and near-QVGA resolutions to use the same
// mechanism.
static const int kVgaBitrateThresholdKbps = 500;
static const int kVgaNumPixels = 700 * 500;  // 640x480
static const int kQvgaBitrateThresholdKbps = 250;
static const int kQvgaNumPixels = 400 * 300;  // 320x240
}  // namespace

// QP thresholds are chosen to be high enough to be hit in practice when quality
// is good, but also low enough to not cause a flip-flop behavior (e.g. going up
// in resolution shouldn't give so bad quality that we should go back down).

const int QualityScaler::kLowVp8QpThreshold = 29;
const int QualityScaler::kBadVp8QpThreshold = 95;

const int QualityScaler::kLowH264QpThreshold = 22;
const int QualityScaler::kBadH264QpThreshold = 35;

QualityScaler::QualityScaler() : low_qp_threshold_(-1) {}

void QualityScaler::Init(int low_qp_threshold,
                         int high_qp_threshold,
                         int initial_bitrate_kbps,
                         int width,
                         int height,
                         int fps) {
  ClearSamples();
  low_qp_threshold_ = low_qp_threshold;
  high_qp_threshold_ = high_qp_threshold;
  downscale_shift_ = 0;
  // Use a faster window for upscaling initially (but be more graceful later).
  // This enables faster initial rampups without risking strong up-down
  // behavior later.
  measure_seconds_upscale_ = kMeasureSecondsFastUpscale;
  const int init_width = width;
  const int init_height = height;
  if (initial_bitrate_kbps > 0) {
    int init_num_pixels = width * height;
    if (initial_bitrate_kbps < kVgaBitrateThresholdKbps)
      init_num_pixels = kVgaNumPixels;
    if (initial_bitrate_kbps < kQvgaBitrateThresholdKbps)
      init_num_pixels = kQvgaNumPixels;
    while (width * height > init_num_pixels) {
      ++downscale_shift_;
      width /= 2;
      height /= 2;
    }
  }

  // Zero out width/height so they can be checked against inside
  // UpdateTargetResolution.
  res_.width = res_.height = 0;
  UpdateTargetResolution(init_width, init_height);
  ReportFramerate(fps);
}

// Report framerate(fps) to estimate # of samples.
void QualityScaler::ReportFramerate(int framerate) {
  framerate_ = framerate;
  UpdateSampleCounts();
}

void QualityScaler::ReportQP(int qp) {
  framedrop_percent_.AddSample(0);
  average_qp_downscale_.AddSample(qp);
  average_qp_upscale_.AddSample(qp);
}

void QualityScaler::ReportDroppedFrame() {
  framedrop_percent_.AddSample(100);
}

void QualityScaler::OnEncodeFrame(int width, int height) {
  // Should be set through InitEncode -> Should be set by now.
  RTC_DCHECK_GE(low_qp_threshold_, 0);
  RTC_DCHECK_GT(num_samples_upscale_, 0u);
  RTC_DCHECK_GT(num_samples_downscale_, 0u);

  // Update scale factor.
  int avg_drop = 0;
  int avg_qp = 0;

  if ((framedrop_percent_.GetAverage(num_samples_downscale_, &avg_drop) &&
       avg_drop >= kFramedropPercentThreshold) ||
      (average_qp_downscale_.GetAverage(num_samples_downscale_, &avg_qp) &&
       avg_qp > high_qp_threshold_)) {
    AdjustScale(false);
  } else if (average_qp_upscale_.GetAverage(num_samples_upscale_, &avg_qp) &&
             avg_qp <= low_qp_threshold_) {
    AdjustScale(true);
  }
  UpdateTargetResolution(width, height);
}

QualityScaler::Resolution QualityScaler::GetScaledResolution() const {
  return res_;
}

rtc::scoped_refptr<VideoFrameBuffer> QualityScaler::GetScaledBuffer(
    const rtc::scoped_refptr<VideoFrameBuffer>& frame) {
  Resolution res = GetScaledResolution();
  int src_width = frame->width();
  int src_height = frame->height();

  if (res.width == src_width && res.height == src_height)
    return frame;
  rtc::scoped_refptr<I420Buffer> scaled_buffer =
      pool_.CreateBuffer(res.width, res.height);

  scaled_buffer->ScaleFrom(frame);

  return scaled_buffer;
}

void QualityScaler::UpdateTargetResolution(int frame_width, int frame_height) {
  RTC_DCHECK_GE(downscale_shift_, 0);
  int shifts_performed = 0;
  for (int shift = downscale_shift_;
       shift > 0 && (frame_width / 2 >= kMinDownscaleDimension) &&
       (frame_height / 2 >= kMinDownscaleDimension);
       --shift, ++shifts_performed) {
    frame_width /= 2;
    frame_height /= 2;
  }
  // Clamp to number of shifts actually performed to not be stuck trying to
  // scale way beyond QVGA.
  downscale_shift_ = shifts_performed;
  if (res_.width == frame_width && res_.height == frame_height) {
    // No reset done/needed, using same resolution.
    return;
  }
  res_.width = frame_width;
  res_.height = frame_height;
  ClearSamples();
}

void QualityScaler::ClearSamples() {
  framedrop_percent_.Reset();
  average_qp_downscale_.Reset();
  average_qp_upscale_.Reset();
}

void QualityScaler::UpdateSampleCounts() {
  num_samples_downscale_ = static_cast<size_t>(
      kMeasureSecondsDownscale * (framerate_ < kMinFps ? kMinFps : framerate_));
  num_samples_upscale_ = static_cast<size_t>(
      measure_seconds_upscale_ * (framerate_ < kMinFps ? kMinFps : framerate_));
}

void QualityScaler::AdjustScale(bool up) {
  downscale_shift_ += up ? -1 : 1;
  if (downscale_shift_ < 0)
    downscale_shift_ = 0;
  if (!up) {
    // First downscale hit, start using a slower threshold for going up.
    measure_seconds_upscale_ = kMeasureSecondsUpscale;
    UpdateSampleCounts();
  }
}

}  // namespace webrtc

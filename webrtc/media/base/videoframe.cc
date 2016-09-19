/*
 *  Copyright (c) 2011 The WebRTC@AnyRTC project authors. All Rights Reserved.
 *
 *  Use of this source code is governed by a BSD-style license
 *  that can be found in the LICENSE file in the root of the source
 *  tree. An additional intellectual property rights grant can be found
 *  in the file PATENTS.  All contributing project authors may
 *  be found in the AUTHORS file in the root of the source tree.
 */

#include "webrtc/media/base/videoframe.h"

#include <string.h>

#include "libyuv/compare.h"
#include "libyuv/planar_functions.h"
#include "libyuv/scale.h"
#include "webrtc/base/arraysize.h"
#include "webrtc/base/checks.h"
#include "webrtc/base/logging.h"
#include "webrtc/media/base/videocommon.h"

namespace cricket {

size_t VideoFrame::ConvertToRgbBuffer(uint32_t to_fourcc,
                                      uint8_t* buffer,
                                      size_t size,
                                      int stride_rgb) const {
  const size_t needed = std::abs(stride_rgb) * static_cast<size_t>(height());
  if (size < needed) {
    LOG(LS_WARNING) << "RGB buffer is not large enough";
    return needed;
  }

  if (libyuv::ConvertFromI420(
          video_frame_buffer()->DataY(), video_frame_buffer()->StrideY(),
          video_frame_buffer()->DataU(), video_frame_buffer()->StrideU(),
          video_frame_buffer()->DataV(), video_frame_buffer()->StrideV(),
          buffer, stride_rgb, width(), height(), to_fourcc)) {
    LOG(LS_ERROR) << "RGB type not supported: " << to_fourcc;
    return 0;  // 0 indicates error
  }
  return needed;
}

static const size_t kMaxSampleSize = 1000000000u;
// Returns whether a sample is valid.
bool VideoFrame::Validate(uint32_t fourcc,
                          int w,
                          int h,
                          const uint8_t* sample,
                          size_t sample_size) {
  if (h < 0) {
    h = -h;
  }
  // 16384 is maximum resolution for VP8 codec.
  if (w < 1 || w > 16384 || h < 1 || h > 16384) {
    LOG(LS_ERROR) << "Invalid dimensions: " << w << "x" << h;
    return false;
  }
  uint32_t format = CanonicalFourCC(fourcc);
  int expected_bpp = 8;
  switch (format) {
    case FOURCC_I400:
    case FOURCC_RGGB:
    case FOURCC_BGGR:
    case FOURCC_GRBG:
    case FOURCC_GBRG:
      expected_bpp = 8;
      break;
    case FOURCC_I420:
    case FOURCC_I411:
    case FOURCC_YU12:
    case FOURCC_YV12:
    case FOURCC_M420:
    case FOURCC_NV21:
    case FOURCC_NV12:
      expected_bpp = 12;
      break;
    case FOURCC_I422:
    case FOURCC_YV16:
    case FOURCC_YUY2:
    case FOURCC_UYVY:
    case FOURCC_RGBP:
    case FOURCC_RGBO:
    case FOURCC_R444:
      expected_bpp = 16;
      break;
    case FOURCC_I444:
    case FOURCC_YV24:
    case FOURCC_24BG:
    case FOURCC_RAW:
      expected_bpp = 24;
      break;

    case FOURCC_ABGR:
    case FOURCC_BGRA:
    case FOURCC_ARGB:
      expected_bpp = 32;
      break;

    case FOURCC_MJPG:
    case FOURCC_H264:
      expected_bpp = 0;
      break;
    default:
      expected_bpp = 8;  // Expect format is at least 8 bits per pixel.
      break;
  }
  size_t expected_size = (w * expected_bpp + 7) / 8 * h;
  // For compressed formats, expect 4 bits per 16 x 16 macro.  I420 would be
  // 6 bits, but grey can be 4 bits.
  if (expected_bpp == 0) {
    expected_size = ((w + 15) / 16) * ((h + 15) / 16) * 4 / 8;
  }
  if (sample == NULL) {
    LOG(LS_ERROR) << "NULL sample pointer."
                  << " format: " << GetFourccName(format)
                  << " bpp: " << expected_bpp
                  << " size: " << w << "x" << h
                  << " expected: " << expected_size
                  << " " << sample_size;
    return false;
  }
  // TODO(fbarchard): Make function to dump information about frames.
  uint8_t four_samples[4] = {0, 0, 0, 0};
  for (size_t i = 0; i < arraysize(four_samples) && i < sample_size; ++i) {
    four_samples[i] = sample[i];
  }
  if (sample_size < expected_size) {
    LOG(LS_ERROR) << "Size field is too small."
                  << " format: " << GetFourccName(format)
                  << " bpp: " << expected_bpp
                  << " size: " << w << "x" << h
                  << " " << sample_size
                  << " expected: " << expected_size
                  << " sample[0..3]: " << static_cast<int>(four_samples[0])
                  << ", " << static_cast<int>(four_samples[1])
                  << ", " << static_cast<int>(four_samples[2])
                  << ", " << static_cast<int>(four_samples[3]);
    return false;
  }
  if (sample_size > kMaxSampleSize) {
    LOG(LS_WARNING) << "Size field is invalid."
                    << " format: " << GetFourccName(format)
                    << " bpp: " << expected_bpp
                    << " size: " << w << "x" << h
                    << " " << sample_size
                    << " expected: " << 2 * expected_size
                    << " sample[0..3]: " << static_cast<int>(four_samples[0])
                    << ", " << static_cast<int>(four_samples[1])
                    << ", " << static_cast<int>(four_samples[2])
                    << ", " << static_cast<int>(four_samples[3]);
    return false;
  }
  // Show large size warning once every 100 frames.
  // TODO(fbarchard): Make frame counter atomic for thread safety.
  static int large_warn100 = 0;
  size_t large_expected_size = expected_size * 2;
  if (expected_bpp >= 8 &&
      (sample_size > large_expected_size || sample_size > kMaxSampleSize) &&
      large_warn100 % 100 == 0) {
    ++large_warn100;
    LOG(LS_WARNING) << "Size field is too large."
                    << " format: " << GetFourccName(format)
                    << " bpp: " << expected_bpp
                    << " size: " << w << "x" << h
                    << " bytes: " << sample_size
                    << " expected: " << large_expected_size
                    << " sample[0..3]: " << static_cast<int>(four_samples[0])
                    << ", " << static_cast<int>(four_samples[1])
                    << ", " << static_cast<int>(four_samples[2])
                    << ", " << static_cast<int>(four_samples[3]);
  }

  // TODO(fbarchard): Add duplicate pixel check.
  // TODO(fbarchard): Use frame counter atomic for thread safety.
  static bool valid_once = true;
  if (valid_once) {
    valid_once = false;
    LOG(LS_INFO) << "Validate frame passed."
                 << " format: " << GetFourccName(format)
                 << " bpp: " << expected_bpp
                 << " size: " << w << "x" << h
                 << " bytes: " << sample_size
                 << " expected: " << expected_size
                 << " sample[0..3]: " << static_cast<int>(four_samples[0])
                 << ", " << static_cast<int>(four_samples[1])
                 << ", " << static_cast<int>(four_samples[2])
                 << ", " << static_cast<int>(four_samples[3]);
  }
  return true;
}

}  // namespace cricket

/*
 *  Copyright (c) 2015 The WebRTC@AnyRTC project authors. All Rights Reserved.
 *
 *  Use of this source code is governed by a BSD-style license
 *  that can be found in the LICENSE file in the root of the source
 *  tree. An additional intellectual property rights grant can be found
 *  in the file PATENTS.  All contributing project authors may
 *  be found in the AUTHORS file in the root of the source tree.
 *
 */

#include "webrtc/modules/video_coding/codecs/h264/include/h264.h"

#if defined(WEBRTC_USE_H264)
#include "webrtc/modules/video_coding/codecs/h264/h264_decoder_impl.h"
#include "webrtc/modules/video_coding/codecs/h264/h264_encoder_impl.h"
#endif
#if defined(WEBRTC_IOS)
#include "webrtc/modules/video_coding/codecs/h264/h264_video_toolbox_decoder.h"
#include "webrtc/modules/video_coding/codecs/h264/h264_video_toolbox_encoder.h"
#endif

#include "webrtc/base/checks.h"
#include "webrtc/base/logging.h"

namespace webrtc {

namespace {

#if defined(WEBRTC_USE_H264)
bool g_rtc_use_h264 = true;
#endif

}  // namespace

void DisableRtcUseH264() {
#if defined(WEBRTC_USE_H264)
  g_rtc_use_h264 = false;
#endif
}

// We need this file to be C++ only so it will compile properly for all
// platforms. In order to write ObjC specific implementations we use private
// externs. This function is defined in h264.mm.
#if defined(WEBRTC_IOS)
extern bool IsH264CodecSupportedObjC();
#endif

// If any H.264 codec is supported (iOS HW or OpenH264/FFmpeg).
bool IsH264CodecSupported() {
#if defined(WEBRTC_IOS) && defined(WEBRTC_VIDEO_TOOLBOX_SUPPORTED)
  if (IsH264CodecSupportedObjC()) {
    return true;
  }
#endif
#if defined(WEBRTC_USE_H264)
  return g_rtc_use_h264;
#else
  return false;
#endif
}

H264Encoder* H264Encoder::Create() {
  RTC_DCHECK(H264Encoder::IsSupported());
#if defined(WEBRTC_IOS) && defined(WEBRTC_VIDEO_TOOLBOX_SUPPORTED)
  if (IsH264CodecSupportedObjC()) {
    LOG(LS_INFO) << "Creating H264VideoToolboxEncoder.";
    return new H264VideoToolboxEncoder();
  }
#endif
#if defined(WEBRTC_USE_H264)
  RTC_CHECK(g_rtc_use_h264);
  LOG(LS_INFO) << "Creating H264EncoderImpl.";
  return new H264EncoderImpl();
#else
  RTC_NOTREACHED();
  return nullptr;
#endif
}

bool H264Encoder::IsSupported() {
  return IsH264CodecSupported();
}

H264Decoder* H264Decoder::Create() {
  RTC_DCHECK(H264Decoder::IsSupported());
#if defined(WEBRTC_IOS) && defined(WEBRTC_VIDEO_TOOLBOX_SUPPORTED)
  if (IsH264CodecSupportedObjC()) {
    LOG(LS_INFO) << "Creating H264VideoToolboxDecoder.";
    return new H264VideoToolboxDecoder();
  }
#endif
#if defined(WEBRTC_USE_H264)
  RTC_CHECK(g_rtc_use_h264);
  LOG(LS_INFO) << "Creating H264DecoderImpl.";
  return new H264DecoderImpl();
#else
  RTC_NOTREACHED();
  return nullptr;
#endif
}

bool H264Decoder::IsSupported() {
  return IsH264CodecSupported();
}

}  // namespace webrtc

/*
 *  Copyright (c) 2013 The WebRTC project authors. All Rights Reserved.
 *
 *  Use of this source code is governed by a BSD-style license
 *  that can be found in the LICENSE file in the root of the source
 *  tree. An additional intellectual property rights grant can be found
 *  in the file PATENTS.  All contributing project authors may
 *  be found in the AUTHORS file in the root of the source tree.
 */

#include "webrtc/modules/audio_device/android/opensles_common.h"

#include <assert.h>
#include <SLES/OpenSLES.h>

#include "webrtc/base/arraysize.h"
#include "webrtc/modules/audio_device/android/audio_common.h"

using webrtc::kNumChannels;

namespace webrtc {

// Returns a string representation given an integer SL_RESULT_XXX code.
// The mapping can be found in <SLES/OpenSLES.h>.
const char* GetSLErrorString(size_t code) {
  static const char* sl_error_strings[] = {
      "SL_RESULT_SUCCESS",                 // 0
      "SL_RESULT_PRECONDITIONS_VIOLATED",  // 1
      "SL_RESULT_PARAMETER_INVALID",       // 2
      "SL_RESULT_MEMORY_FAILURE",          // 3
      "SL_RESULT_RESOURCE_ERROR",          // 4
      "SL_RESULT_RESOURCE_LOST",           // 5
      "SL_RESULT_IO_ERROR",                // 6
      "SL_RESULT_BUFFER_INSUFFICIENT",     // 7
      "SL_RESULT_CONTENT_CORRUPTED",       // 8
      "SL_RESULT_CONTENT_UNSUPPORTED",     // 9
      "SL_RESULT_CONTENT_NOT_FOUND",       // 10
      "SL_RESULT_PERMISSION_DENIED",       // 11
      "SL_RESULT_FEATURE_UNSUPPORTED",     // 12
      "SL_RESULT_INTERNAL_ERROR",          // 13
      "SL_RESULT_UNKNOWN_ERROR",           // 14
      "SL_RESULT_OPERATION_ABORTED",       // 15
      "SL_RESULT_CONTROL_LOST",            // 16
  };

  if (code >= arraysize(sl_error_strings)) {
    return "SL_RESULT_UNKNOWN_ERROR";
  }
  return sl_error_strings[code];
}

SLDataFormat_PCM CreatePcmConfiguration(int sample_rate) {
  SLDataFormat_PCM configuration;
  configuration.formatType = SL_DATAFORMAT_PCM;
  configuration.numChannels = kNumChannels;
  // According to the opensles documentation in the ndk:
  // samplesPerSec is actually in units of milliHz, despite the misleading name.
  // It further recommends using constants. However, this would lead to a lot
  // of boilerplate code so it is not done here.
  configuration.samplesPerSec = sample_rate * 1000;
  configuration.bitsPerSample = SL_PCMSAMPLEFORMAT_FIXED_16;
  configuration.containerSize = SL_PCMSAMPLEFORMAT_FIXED_16;
  configuration.channelMask = SL_SPEAKER_FRONT_CENTER;
  if (2 == configuration.numChannels) {
    configuration.channelMask =
        SL_SPEAKER_FRONT_LEFT | SL_SPEAKER_FRONT_RIGHT;
  }
  configuration.endianness = SL_BYTEORDER_LITTLEENDIAN;
  return configuration;
}

}  // namespace webrtc_opensl

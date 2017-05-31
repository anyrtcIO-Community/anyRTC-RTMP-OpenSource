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

#include "webrtc/modules/video_coding/codecs/h264/h264_decoder_impl.h"

#include <algorithm>
#include <limits>

#include "webrtc/base/checks.h"
#include "webrtc/base/criticalsection.h"
#include "webrtc/base/keep_ref_until_done.h"
#include "webrtc/base/logging.h"
#include "webrtc/system_wrappers/include/metrics.h"

namespace webrtc {

namespace {
  // Used by histograms. Values of entries should not be changed.
  enum H264DecoderImplEvent {
    kH264DecoderEventInit = 0,
    kH264DecoderEventError = 1,
    kH264DecoderEventMax = 16,
  };
}
H264DecoderImpl::H264DecoderImpl() : decoder_(nullptr),
                                     decoded_image_callback_(nullptr),
                                     has_reported_init_(false),
                                     has_reported_error_(false) {
}

H264DecoderImpl::~H264DecoderImpl() {
  Release();
}

int32_t H264DecoderImpl::InitDecode(const VideoCodec* codec_settings,
                                    int32_t number_of_cores) {
  ReportInit();
  if (codec_settings &&
      codec_settings->codecType != kVideoCodecH264) {
    ReportError();
    return WEBRTC_VIDEO_CODEC_ERR_PARAMETER;
  }

  // Release necessary in case of re-initializing.
  int32_t ret = Release();
  if (ret != WEBRTC_VIDEO_CODEC_OK) {
    ReportError();
    return ret;
  }
  RTC_DCHECK(!decoder_);

  ret = WelsCreateDecoder(&decoder_);
  if (ret == 0) {
    SDecodingParam decParam;
    memset(&decParam, 0, sizeof(SDecodingParam));
    decParam.eOutputColorFormat = videoFormatI420;
    decParam.uiTargetDqLayer = UCHAR_MAX;
    decParam.eEcActiveIdc = ERROR_CON_SLICE_COPY;
    decParam.sVideoProperty.eVideoBsType = VIDEO_BITSTREAM_DEFAULT;
    ret = decoder_->Initialize(&decParam);
  }

  if (ret != WEBRTC_VIDEO_CODEC_OK) {
    // This is an indication that FFmpeg has not been initialized or it has not
    // been compiled/initialized with the correct set of codecs.
    LOG(LS_ERROR) << "OpenH.264 decoder not found.";
    Release();
    ReportError();
    return WEBRTC_VIDEO_CODEC_ERROR;
  }
  return WEBRTC_VIDEO_CODEC_OK;
}

int32_t H264DecoderImpl::Release() {
  if (decoder_) {
    WelsDestroyDecoder(decoder_);
    decoder_ = NULL;
  }
  return WEBRTC_VIDEO_CODEC_OK;
}

int32_t H264DecoderImpl::RegisterDecodeCompleteCallback(
    DecodedImageCallback* callback) {
  decoded_image_callback_ = callback;
  return WEBRTC_VIDEO_CODEC_OK;
}

int32_t H264DecoderImpl::Decode(const EncodedImage& input_image,
                                bool /*missing_frames*/,
                                const RTPFragmentationHeader* /*fragmentation*/,
                                const CodecSpecificInfo* codec_specific_info,
                                int64_t /*render_time_ms*/) {
  if (!IsInitialized()) {
    ReportError();
    return WEBRTC_VIDEO_CODEC_UNINITIALIZED;
  }
  if (!decoded_image_callback_) {
    LOG(LS_WARNING) << "InitDecode() has been called, but a callback function "
        "has not been set with RegisterDecodeCompleteCallback()";
    ReportError();
    return WEBRTC_VIDEO_CODEC_UNINITIALIZED;
  }
  if (!input_image._buffer || !input_image._length) {
    ReportError();
    return WEBRTC_VIDEO_CODEC_ERR_PARAMETER;
  }
  if (codec_specific_info &&
      codec_specific_info->codecType != kVideoCodecH264) {
    ReportError();
    return WEBRTC_VIDEO_CODEC_ERR_PARAMETER;
  }

  int32_t ret = 0;
  uint8_t* nal[3] = {0};
  SBufferInfo sbi = {0};
  sbi.uiInBsTimeStamp = input_image.capture_time_ms_;
  int result = decoder_->DecodeFrame2(input_image._buffer, (int)input_image._size, nal, &sbi);
  if (result != 0) {
    LOG(LS_ERROR) << "DecodeFrame2 error: " << result;
    ReportError();
    return WEBRTC_VIDEO_CODEC_ERROR;
  }

  if (sbi.iBufferStatus == 1) {
    SSysMEMBuffer& ssb = sbi.UsrData.sSystemBuffer;
    VideoFrame frame;
    frame.CreateFrame(nal[0], nal[1], nal[2], 
      ssb.iWidth, ssb.iHeight,
      ssb.iStride[0], ssb.iStride[1], ssb.iStride[1],
      kVideoRotation_0);
    frame.set_timestamp(input_image._timeStamp);
    ret = decoded_image_callback_->Decoded(frame);
  }

  if (ret) {
    LOG(LS_WARNING) << "DecodedImageCallback::Decoded returned " << ret;
    return ret;
  }
  return WEBRTC_VIDEO_CODEC_OK;
}

const char* H264DecoderImpl::ImplementationName() const {
  return "OpenH264";
}

bool H264DecoderImpl::IsInitialized() const {
  return decoder_ != nullptr;
}

void H264DecoderImpl::ReportInit() {
  if (has_reported_init_)
    return;
  RTC_HISTOGRAM_ENUMERATION("WebRTC.Video.H264DecoderImpl.Event",
                            kH264DecoderEventInit,
                            kH264DecoderEventMax);
  has_reported_init_ = true;
}

void H264DecoderImpl::ReportError() {
  if (has_reported_error_)
    return;
  RTC_HISTOGRAM_ENUMERATION("WebRTC.Video.H264DecoderImpl.Event",
                            kH264DecoderEventError,
                            kH264DecoderEventMax);
  has_reported_error_ = true;
}

}  // namespace webrtc

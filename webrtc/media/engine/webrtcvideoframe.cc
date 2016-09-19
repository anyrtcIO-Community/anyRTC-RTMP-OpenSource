/*
 *  Copyright (c) 2011 The WebRTC@AnyRTC project authors. All Rights Reserved.
 *
 *  Use of this source code is governed by a BSD-style license
 *  that can be found in the LICENSE file in the root of the source
 *  tree. An additional intellectual property rights grant can be found
 *  in the file PATENTS.  All contributing project authors may
 *  be found in the AUTHORS file in the root of the source tree.
 */

#include "webrtc/media/engine/webrtcvideoframe.h"

#include "libyuv/convert.h"
#include "webrtc/base/logging.h"
#include "webrtc/media/base/videocapturer.h"
#include "webrtc/media/base/videocommon.h"
#include "webrtc/video_frame.h"

using webrtc::kYPlane;
using webrtc::kUPlane;
using webrtc::kVPlane;

namespace cricket {

WebRtcVideoFrame::WebRtcVideoFrame()
    : timestamp_us_(0), rotation_(webrtc::kVideoRotation_0) {}

WebRtcVideoFrame::WebRtcVideoFrame(
    const rtc::scoped_refptr<webrtc::VideoFrameBuffer>& buffer,
    webrtc::VideoRotation rotation,
    int64_t timestamp_us)
    : video_frame_buffer_(buffer),
      timestamp_us_(timestamp_us),
      rotation_(rotation) {}

WebRtcVideoFrame::WebRtcVideoFrame(
    const rtc::scoped_refptr<webrtc::VideoFrameBuffer>& buffer,
    int64_t time_stamp_ns,
    webrtc::VideoRotation rotation)
    : WebRtcVideoFrame(buffer,
                       rotation,
                       time_stamp_ns / rtc::kNumNanosecsPerMicrosec) {}

WebRtcVideoFrame::~WebRtcVideoFrame() {}

bool WebRtcVideoFrame::Init(uint32_t format,
                            int w,
                            int h,
                            int dw,
                            int dh,
                            uint8_t* sample,
                            size_t sample_size,
                            int64_t time_stamp_ns,
                            webrtc::VideoRotation rotation) {
  return Reset(format, w, h, dw, dh, sample, sample_size,
               time_stamp_ns / rtc::kNumNanosecsPerMicrosec, rotation,
               true /*apply_rotation*/);
}

bool WebRtcVideoFrame::Init(const CapturedFrame* frame, int dw, int dh,
                            bool apply_rotation) {
  return Reset(frame->fourcc, frame->width, frame->height, dw, dh,
               static_cast<uint8_t*>(frame->data), frame->data_size,
               frame->time_stamp / rtc::kNumNanosecsPerMicrosec,
               frame->rotation, apply_rotation);
}

int WebRtcVideoFrame::width() const {
  return video_frame_buffer_ ? video_frame_buffer_->width() : 0;
}

int WebRtcVideoFrame::height() const {
  return video_frame_buffer_ ? video_frame_buffer_->height() : 0;
}

const rtc::scoped_refptr<webrtc::VideoFrameBuffer>&
WebRtcVideoFrame::video_frame_buffer() const {
  return video_frame_buffer_;
}

VideoFrame* WebRtcVideoFrame::Copy() const {
  return new WebRtcVideoFrame(video_frame_buffer_, rotation_, timestamp_us_);
}

size_t WebRtcVideoFrame::ConvertToRgbBuffer(uint32_t to_fourcc,
                                            uint8_t* buffer,
                                            size_t size,
                                            int stride_rgb) const {
  RTC_CHECK(video_frame_buffer_);
  RTC_CHECK(video_frame_buffer_->native_handle() == nullptr);
  return VideoFrame::ConvertToRgbBuffer(to_fourcc, buffer, size, stride_rgb);
}

bool WebRtcVideoFrame::Reset(uint32_t format,
                             int w,
                             int h,
                             int dw,
                             int dh,
                             uint8_t* sample,
                             size_t sample_size,
                             int64_t timestamp_us,
                             webrtc::VideoRotation rotation,
                             bool apply_rotation) {
  if (!Validate(format, w, h, sample, sample_size)) {
    return false;
  }
  // Translate aliases to standard enums (e.g., IYUV -> I420).
  format = CanonicalFourCC(format);

  // Set up a new buffer.
  // TODO(fbarchard): Support lazy allocation.
  int new_width = dw;
  int new_height = dh;
  // If rotated swap width, height.
  if (apply_rotation && (rotation == 90 || rotation == 270)) {
    new_width = dh;
    new_height = dw;
  }

  InitToEmptyBuffer(new_width, new_height);
  rotation_ = apply_rotation ? webrtc::kVideoRotation_0 : rotation;

  int horiz_crop = ((w - dw) / 2) & ~1;
  // ARGB on Windows has negative height.
  // The sample's layout in memory is normal, so just correct crop.
  int vert_crop = ((abs(h) - dh) / 2) & ~1;
  // Conversion functions expect negative height to flip the image.
  int idh = (h < 0) ? -dh : dh;
  int r = libyuv::ConvertToI420(
      sample, sample_size,
      video_frame_buffer_->MutableDataY(),
      video_frame_buffer_->StrideY(),
      video_frame_buffer_->MutableDataU(),
      video_frame_buffer_->StrideU(),
      video_frame_buffer_->MutableDataV(),
      video_frame_buffer_->StrideV(),
      horiz_crop, vert_crop,
      w, h,
      dw, idh,
      static_cast<libyuv::RotationMode>(
          apply_rotation ? rotation : webrtc::kVideoRotation_0),
      format);
  if (r) {
    LOG(LS_ERROR) << "Error parsing format: " << GetFourccName(format)
                  << " return code : " << r;
    return false;
  }
  timestamp_us_ = timestamp_us;
  return true;
}

void WebRtcVideoFrame::InitToEmptyBuffer(int w, int h) {
  video_frame_buffer_ = new rtc::RefCountedObject<webrtc::I420Buffer>(w, h);
  rotation_ = webrtc::kVideoRotation_0;
}

const VideoFrame* WebRtcVideoFrame::GetCopyWithRotationApplied() const {
  // If the frame is not rotated, the caller should reuse this frame instead of
  // making a redundant copy.
  if (rotation() == webrtc::kVideoRotation_0) {
    return this;
  }

  // If the video frame is backed up by a native handle, it resides in the GPU
  // memory which we can't rotate here. The assumption is that the renderers
  // which uses GPU to render should be able to rotate themselves.
  RTC_DCHECK(!video_frame_buffer()->native_handle());

  if (rotated_frame_) {
    return rotated_frame_.get();
  }

  int current_width = width();
  int current_height = height();

  int rotated_width = current_width;
  int rotated_height = current_height;
  if (rotation() == webrtc::kVideoRotation_90 ||
      rotation() == webrtc::kVideoRotation_270) {
    std::swap(rotated_width, rotated_height);
  }

  rtc::scoped_refptr<webrtc::I420Buffer> buffer =
      new rtc::RefCountedObject<webrtc::I420Buffer>(rotated_width,
                                                    rotated_height);

  // TODO(guoweis): Add a function in webrtc_libyuv.cc to convert from
  // VideoRotation to libyuv::RotationMode.
  int ret = libyuv::I420Rotate(
      video_frame_buffer_->DataY(), video_frame_buffer_->StrideY(),
      video_frame_buffer_->DataU(), video_frame_buffer_->StrideU(),
      video_frame_buffer_->DataV(), video_frame_buffer_->StrideV(),
      buffer->MutableDataY(), buffer->StrideY(), buffer->MutableDataU(),
      buffer->StrideU(), buffer->MutableDataV(), buffer->StrideV(),
      current_width, current_height,
      static_cast<libyuv::RotationMode>(rotation()));
  if (ret == 0) {
    rotated_frame_.reset(
        new WebRtcVideoFrame(buffer, webrtc::kVideoRotation_0, timestamp_us_));
  }

  return rotated_frame_.get();
}

}  // namespace cricket

/*
 *  Copyright (c) 2014 The WebRTC project authors. All Rights Reserved.
 *
 *  Use of this source code is governed by a BSD-style license
 *  that can be found in the LICENSE file in the root of the source
 *  tree. An additional intellectual property rights grant can be found
 *  in the file PATENTS.  All contributing project authors may
 *  be found in the AUTHORS file in the root of the source tree.
 */

#ifndef WEBRTC_VIDEO_FRAME_H_
#define WEBRTC_VIDEO_FRAME_H_

#include "webrtc/base/scoped_ref_ptr.h"
#include "webrtc/common_types.h"
#include "webrtc/common_video/include/video_frame_buffer.h"
#include "webrtc/common_video/rotation.h"
#include "webrtc/typedefs.h"

namespace webrtc {

// TODO(nisse): This class duplicates cricket::VideoFrame. There's
// ongoing work to merge the classes. See
// https://bugs.chromium.org/p/webrtc/issues/detail?id=5682.
class VideoFrame {
 public:
  // TODO(nisse): Deprecated. Using the default constructor violates the
  // reasonable assumption that video_frame_buffer() returns a valid buffer.
  VideoFrame();

  // Preferred constructor.
  VideoFrame(const rtc::scoped_refptr<webrtc::VideoFrameBuffer>& buffer,
             uint32_t timestamp,
             int64_t render_time_ms,
             VideoRotation rotation);

  // CreateEmptyFrame: Sets frame dimensions and allocates buffers based
  // on set dimensions - height and plane stride.
  // If required size is bigger than the allocated one, new buffers of adequate
  // size will be allocated.

  // TODO(nisse): Deprecated. Should be deleted in the cricket::VideoFrame and
  // webrtc::VideoFrame merge. If you need to write into the frame, create a
  // VideoFrameBuffer of the desired size, e.g, using I420Buffer::Create and
  // write to that. And if you need to wrap it into a VideoFrame, pass it to the
  // constructor.
  void CreateEmptyFrame(int width,
                        int height,
                        int stride_y,
                        int stride_u,
                        int stride_v);

  // CreateFrame: Sets the frame's members and buffers. If required size is
  // bigger than allocated one, new buffers of adequate size will be allocated.

  // TODO(nisse): Deprecated. Should be deleted in the cricket::VideoFrame and
  // webrtc::VideoFrame merge. Instead, create a VideoFrameBuffer and pass to
  // the constructor. E.g, use I420Buffer::Copy(WrappedI420Buffer(...)).
  void CreateFrame(const uint8_t* buffer_y,
                   const uint8_t* buffer_u,
                   const uint8_t* buffer_v,
                   int width,
                   int height,
                   int stride_y,
                   int stride_u,
                   int stride_v,
                   VideoRotation rotation);

  // CreateFrame: Sets the frame's members and buffers. If required size is
  // bigger than allocated one, new buffers of adequate size will be allocated.
  // |buffer| must be a packed I420 buffer.

  // TODO(nisse): Deprecated, see above method for advice.
  void CreateFrame(const uint8_t* buffer,
                  int width,
                  int height,
                  VideoRotation rotation);

  // Deep copy frame: If required size is bigger than allocated one, new
  // buffers of adequate size will be allocated.
  // TODO(nisse): Should be deleted in the cricket::VideoFrame and
  // webrtc::VideoFrame merge. Instead, use I420Buffer::Copy to make a copy of
  // the pixel data, and use the constructor to wrap it into a VideoFrame.
  void CopyFrame(const VideoFrame& videoFrame);

  // Creates a shallow copy of |videoFrame|, i.e, the this object will retain a
  // reference to the video buffer also retained by |videoFrame|.
  // TODO(nisse): Deprecated. Should be deleted in the cricket::VideoFrame and
  // webrtc::VideoFrame merge. Instead, pass video_frame_buffer() and timestamps
  // to the constructor.
  void ShallowCopy(const VideoFrame& videoFrame);

  // Get allocated size per plane.

  // TODO(nisse): Deprecated. Should be deleted in the cricket::VideoFrame and
  // webrtc::VideoFrame merge. When used with memset, consider using
  // libyuv::I420Rect instead.
  int allocated_size(PlaneType type) const;

  // Get frame width.
  int width() const;

  // Get frame height.
  int height() const;

  // TODO(nisse): After the cricket::VideoFrame and webrtc::VideoFrame
  // merge, we'll have methods timestamp_us and set_timestamp_us, all
  // other frame timestamps will likely be deprecated.

  // Set frame timestamp (90kHz).
  void set_timestamp(uint32_t timestamp) { timestamp_ = timestamp; }

  // Get frame timestamp (90kHz).
  uint32_t timestamp() const { return timestamp_; }

  // Set capture ntp time in milliseconds.
  void set_ntp_time_ms(int64_t ntp_time_ms) {
    ntp_time_ms_ = ntp_time_ms;
  }

  // Get capture ntp time in milliseconds.
  int64_t ntp_time_ms() const { return ntp_time_ms_; }

  // Naming convention for Coordination of Video Orientation. Please see
  // http://www.etsi.org/deliver/etsi_ts/126100_126199/126114/12.07.00_60/ts_126114v120700p.pdf
  //
  // "pending rotation" or "pending" = a frame that has a VideoRotation > 0.
  //
  // "not pending" = a frame that has a VideoRotation == 0.
  //
  // "apply rotation" = modify a frame from being "pending" to being "not
  //                    pending" rotation (a no-op for "unrotated").
  //
  VideoRotation rotation() const { return rotation_; }
  void set_rotation(VideoRotation rotation) {
    rotation_ = rotation;
  }

  // Set render time in milliseconds.
  void set_render_time_ms(int64_t render_time_ms) {
    render_time_ms_ = render_time_ms;
  }

  // Get render time in milliseconds.
  int64_t render_time_ms() const { return render_time_ms_; }

  // Return true if and only if video_frame_buffer() is null. Which is possible
  // only if the object was default-constructed.
  // TODO(nisse): Deprecated. Should be deleted in the cricket::VideoFrame and
  // webrtc::VideoFrame merge. The intention is that video_frame_buffer() never
  // should return nullptr. To handle potentially uninitialized or non-existent
  // frames, consider using rtc::Optional. Otherwise, IsZeroSize() can be
  // replaced by video_frame_buffer() == nullptr.
  bool IsZeroSize() const;

  // Return the underlying buffer. Never nullptr for a properly
  // initialized VideoFrame.
  // Creating a new reference breaks the HasOneRef and IsMutable
  // logic. So return a const ref to our reference.
  const rtc::scoped_refptr<webrtc::VideoFrameBuffer>& video_frame_buffer()
      const;

  // Return true if the frame is stored in a texture.
  bool is_texture() {
    return video_frame_buffer() &&
           video_frame_buffer()->native_handle() != nullptr;
  }

 private:
  // An opaque reference counted handle that stores the pixel data.
  rtc::scoped_refptr<webrtc::VideoFrameBuffer> video_frame_buffer_;
  uint32_t timestamp_;
  int64_t ntp_time_ms_;
  int64_t render_time_ms_;
  VideoRotation rotation_;
};


// TODO(pbos): Rename EncodedFrame and reformat this class' members.
class EncodedImage {
 public:
  static const size_t kBufferPaddingBytesH264;

  // Some decoders require encoded image buffers to be padded with a small
  // number of additional bytes (due to over-reading byte readers).
  static size_t GetBufferPaddingBytes(VideoCodecType codec_type);

  EncodedImage() : EncodedImage(nullptr, 0, 0) {}

  EncodedImage(uint8_t* buffer, size_t length, size_t size)
      : _buffer(buffer), _length(length), _size(size) {}

  struct AdaptReason {
    AdaptReason()
        : quality_resolution_downscales(-1),
          bw_resolutions_disabled(-1) {}

    int quality_resolution_downscales;  // Number of times this frame is down
                                        // scaled in resolution due to quality.
                                        // Or -1 if information is not provided.
    int bw_resolutions_disabled;  // Number of resolutions that are not sent
                                  // due to bandwidth for this frame.
                                  // Or -1 if information is not provided.
  };
  uint32_t _encodedWidth = 0;
  uint32_t _encodedHeight = 0;
  uint32_t _timeStamp = 0;
  // NTP time of the capture time in local timebase in milliseconds.
  int64_t ntp_time_ms_ = 0;
  int64_t capture_time_ms_ = 0;
  FrameType _frameType = kVideoFrameDelta;
  uint8_t* _buffer;
  size_t _length;
  size_t _size;
  VideoRotation rotation_ = kVideoRotation_0;
  bool _completeFrame = false;
  AdaptReason adapt_reason_;
  int qp_ = -1;  // Quantizer value.

  // When an application indicates non-zero values here, it is taken as an
  // indication that all future frames will be constrained with those limits
  // until the application indicates a change again.
  PlayoutDelay playout_delay_ = {-1, -1};
};

}  // namespace webrtc
#endif  // WEBRTC_VIDEO_FRAME_H_

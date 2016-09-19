/*
 *  Copyright 2015 The WebRTC project authors. All Rights Reserved.
 *
 *  Use of this source code is governed by a BSD-style license
 *  that can be found in the LICENSE file in the root of the source
 *  tree. An additional intellectual property rights grant can be found
 *  in the file PATENTS.  All contributing project authors may
 *  be found in the AUTHORS file in the root of the source tree.
 */

#include "webrtc/api/androidvideocapturer.h"

#include <memory>

#include "webrtc/api/java/jni/native_handle_impl.h"
#include "webrtc/base/common.h"
#include "webrtc/base/timeutils.h"
#include "webrtc/media/engine/webrtcvideoframe.h"

namespace webrtc {

AndroidVideoCapturer::AndroidVideoCapturer(
    const rtc::scoped_refptr<AndroidVideoCapturerDelegate>& delegate)
    : running_(false),
      delegate_(delegate),
      current_state_(cricket::CS_STOPPED) {
  thread_checker_.DetachFromThread();
  SetSupportedFormats(delegate_->GetSupportedFormats());
}

AndroidVideoCapturer::~AndroidVideoCapturer() {
  RTC_CHECK(!running_);
}

cricket::CaptureState AndroidVideoCapturer::Start(
    const cricket::VideoFormat& capture_format) {
  RTC_CHECK(thread_checker_.CalledOnValidThread());
  RTC_CHECK(!running_);
  const int fps = cricket::VideoFormat::IntervalToFps(capture_format.interval);
  LOG(LS_INFO) << " AndroidVideoCapturer::Start " << capture_format.width << "x"
               << capture_format.height << "@" << fps;

  running_ = true;
  delegate_->Start(capture_format.width, capture_format.height, fps, this);
  SetCaptureFormat(&capture_format);
  current_state_ = cricket::CS_STARTING;
  return current_state_;
}

void AndroidVideoCapturer::Stop() {
  LOG(LS_INFO) << " AndroidVideoCapturer::Stop ";
  RTC_CHECK(thread_checker_.CalledOnValidThread());
  RTC_CHECK(running_);
  running_ = false;
  SetCaptureFormat(NULL);

  delegate_->Stop();
  current_state_ = cricket::CS_STOPPED;
  SetCaptureState(current_state_);
}

bool AndroidVideoCapturer::IsRunning() {
  RTC_CHECK(thread_checker_.CalledOnValidThread());
  return running_;
}

bool AndroidVideoCapturer::GetPreferredFourccs(std::vector<uint32_t>* fourccs) {
  RTC_CHECK(thread_checker_.CalledOnValidThread());
  fourccs->push_back(cricket::FOURCC_YV12);
  return true;
}

void AndroidVideoCapturer::OnCapturerStarted(bool success) {
  RTC_CHECK(thread_checker_.CalledOnValidThread());
  cricket::CaptureState new_state =
      success ? cricket::CS_RUNNING : cricket::CS_FAILED;
  if (new_state == current_state_)
    return;
  current_state_ = new_state;
  SetCaptureState(new_state);
}

void AndroidVideoCapturer::OnOutputFormatRequest(
    int width, int height, int fps) {
  RTC_CHECK(thread_checker_.CalledOnValidThread());
  cricket::VideoFormat format(width, height,
                              cricket::VideoFormat::FpsToInterval(fps), 0);
  video_adapter()->OnOutputFormatRequest(format);
}

bool AndroidVideoCapturer::GetBestCaptureFormat(
    const cricket::VideoFormat& desired,
    cricket::VideoFormat* best_format) {
  // Delegate this choice to VideoCapturer.startCapture().
  *best_format = desired;
  return true;
}

}  // namespace webrtc

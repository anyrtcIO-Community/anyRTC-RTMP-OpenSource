/*
 *  Copyright 2015 The WebRTC project authors. All Rights Reserved.
 *
 *  Use of this source code is governed by a BSD-style license
 *  that can be found in the LICENSE file in the root of the source
 *  tree. An additional intellectual property rights grant can be found
 *  in the file PATENTS.  All contributing project authors may
 *  be found in the AUTHORS file in the root of the source tree.
 */

#ifndef WEBRTC_API_ANDROIDVIDEOCAPTURER_H_
#define WEBRTC_API_ANDROIDVIDEOCAPTURER_H_

#include <string>
#include <vector>

#include "webrtc/base/thread_checker.h"
#include "webrtc/common_video/include/video_frame_buffer.h"
#include "webrtc/media/base/videocapturer.h"

namespace webrtc {

class AndroidVideoCapturer;

class AndroidVideoCapturerDelegate : public rtc::RefCountInterface {
 public:
  virtual ~AndroidVideoCapturerDelegate() {}
  // Start capturing. The implementation of the delegate must call
  // AndroidVideoCapturer::OnCapturerStarted with the result of this request.
  virtual void Start(int width, int height, int framerate,
                     AndroidVideoCapturer* capturer) = 0;

  // Stops capturing.
  // The delegate may not call into AndroidVideoCapturer after this call.
  virtual void Stop() = 0;

  virtual std::vector<cricket::VideoFormat> GetSupportedFormats() = 0;
};

// Android implementation of cricket::VideoCapturer for use with WebRtc
// PeerConnection.
class AndroidVideoCapturer : public cricket::VideoCapturer {
 public:
  explicit AndroidVideoCapturer(
      const rtc::scoped_refptr<AndroidVideoCapturerDelegate>& delegate);
  virtual ~AndroidVideoCapturer();

  // Called from JNI when the capturer has been started.
  void OnCapturerStarted(bool success);

  // Called from JNI to request a new video format.
  void OnOutputFormatRequest(int width, int height, int fps);

  AndroidVideoCapturerDelegate* delegate() { return delegate_.get(); }

  // cricket::VideoCapturer implementation.
  bool GetBestCaptureFormat(const cricket::VideoFormat& desired,
                            cricket::VideoFormat* best_format) override;

  // Expose these protected methods as public, to be used by the
  // AndroidVideoCapturerJni.
  using VideoCapturer::AdaptFrame;
  using VideoCapturer::OnFrame;

 private:
  // cricket::VideoCapturer implementation.
  // Video frames will be delivered using
  // cricket::VideoCapturer::SignalFrameCaptured on the thread that calls Start.
  cricket::CaptureState Start(
      const cricket::VideoFormat& capture_format) override;
  void Stop() override;
  bool IsRunning() override;
  bool IsScreencast() const override { return false; }
  bool GetPreferredFourccs(std::vector<uint32_t>* fourccs) override;

  bool running_;
  rtc::scoped_refptr<AndroidVideoCapturerDelegate> delegate_;

  rtc::ThreadChecker thread_checker_;

  cricket::CaptureState current_state_;
};

}  // namespace webrtc

#endif  // WEBRTC_API_ANDROIDVIDEOCAPTURER_H_

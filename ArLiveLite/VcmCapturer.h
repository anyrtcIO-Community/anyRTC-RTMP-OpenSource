/*
*  Copyright (c) 2021 The AnyRTC project authors. All Rights Reserved.
*
*  Please visit https://www.anyrtc.io for detail.
*
* The GNU General Public License is a free, copyleft license for
* software and other kinds of works.
*
* The licenses for most software and other practical works are designed
* to take away your freedom to share and change the works.  By contrast,
* the GNU General Public License is intended to guarantee your freedom to
* share and change all versions of a program--to make sure it remains free
* software for all its users.  We, the Free Software Foundation, use the
* GNU General Public License for most of our software; it applies also to
* any other work released this way by its authors.  You can apply it to
* your programs, too.
* See the GNU LICENSE file for more info.
*/
#ifndef __VCM_CAPTURER_H__
#define __VCM_CAPTURER_H__

#include <memory>
#include <vector>

#include "api/scoped_refptr.h"
#include "modules/video_capture/video_capture.h"
#include "rtc_base/thread.h"
#include "media/base/adapted_video_track_source.h"

// This file is borrowed from webrtc project.

class VcmCapturer : public rtc::VideoSinkInterface<webrtc::VideoFrame> {
 public:
  static VcmCapturer* Create(size_t width,
                             size_t height,
                             size_t target_fps,
                             size_t capture_device_index);
  virtual ~VcmCapturer();

  void SetVideoSource(const rtc::scoped_refptr<webrtc::VideoTrackSourceInterface>vidSource);

  bool Start();
  bool Stop();

  void OnFrame(const webrtc::VideoFrame& frame) override;

 private:
  VcmCapturer();
  bool Init(size_t width,
            size_t height,
            size_t target_fps,
            size_t capture_device_index);
  void Destroy();
  rtc::scoped_refptr<webrtc::VideoCaptureModule> CreateDeviceOnVCMThread(
      const char* unique_device_utf8);
  int32_t StartCaptureOnVCMThread(webrtc::VideoCaptureCapability);
  int32_t StopCaptureOnVCMThread();
  void ReleaseOnVCMThread();
  rtc::scoped_refptr<webrtc::VideoCaptureModule> vcm_;
  webrtc::VideoCaptureCapability capability_;
  std::unique_ptr<rtc::Thread> vcm_thread_;

private:
	bool	b_started_;
	rtc::scoped_refptr<webrtc::VideoTrackSourceInterface> video_source_;
};


#endif  // __VCM_CAPTURER_H__

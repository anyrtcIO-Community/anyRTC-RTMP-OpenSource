/*
 *  Copyright 2015 The WebRTC project authors. All Rights Reserved.
 *
 *  Use of this source code is governed by a BSD-style license
 *  that can be found in the LICENSE file in the root of the source
 *  tree. An additional intellectual property rights grant can be found
 *  in the file PATENTS.  All contributing project authors may
 *  be found in the AUTHORS file in the root of the source tree.
 */

#ifndef WEBRTC_API_JAVA_JNI_ANDROIDVIDEOCAPTURER_JNI_H_
#define WEBRTC_API_JAVA_JNI_ANDROIDVIDEOCAPTURER_JNI_H_

#include <memory>
#include <string>

#include "webrtc/api/androidvideocapturer.h"
#include "webrtc/api/java/jni/jni_helpers.h"
#include "webrtc/base/asyncinvoker.h"
#include "webrtc/base/constructormagic.h"
#include "webrtc/base/criticalsection.h"
#include "webrtc/base/thread_checker.h"
#include "webrtc/common_video/include/i420_buffer_pool.h"

namespace webrtc_jni {

struct NativeHandleImpl;
class SurfaceTextureHelper;

// AndroidVideoCapturerJni implements AndroidVideoCapturerDelegate.
// The purpose of the delegate is to hide the JNI specifics from the C++ only
// AndroidVideoCapturer.
class AndroidVideoCapturerJni : public webrtc::AndroidVideoCapturerDelegate {
 public:
  static int SetAndroidObjects(JNIEnv* jni, jobject appliction_context);

  AndroidVideoCapturerJni(JNIEnv* jni,
                          jobject j_video_capturer,
                          jobject j_egl_context);

  void Start(int width, int height, int framerate,
             webrtc::AndroidVideoCapturer* capturer) override;
  void Stop() override;

  std::vector<cricket::VideoFormat> GetSupportedFormats() override;

  // Called from VideoCapturer::NativeObserver on a Java thread.
  void OnCapturerStarted(bool success);
  void OnMemoryBufferFrame(void* video_frame, int length, int width,
                           int height, int rotation, int64_t timestamp_ns);
  void OnTextureFrame(int width, int height, int rotation, int64_t timestamp_ns,
                      const NativeHandleImpl& handle);
  void OnOutputFormatRequest(int width, int height, int fps);

 protected:
  ~AndroidVideoCapturerJni();

 private:
  JNIEnv* jni();

  // To avoid deducing Args from the 3rd parameter of AsyncCapturerInvoke.
  template <typename T>
  struct Identity {
    typedef T type;
  };

  // Helper function to make safe asynchronous calls to |capturer_|. The calls
  // are not guaranteed to be delivered.
  template <typename... Args>
  void AsyncCapturerInvoke(
      const rtc::Location& posted_from,
      void (webrtc::AndroidVideoCapturer::*method)(Args...),
      typename Identity<Args>::type... args);

  const ScopedGlobalRef<jobject> j_video_capturer_;
  const ScopedGlobalRef<jclass> j_video_capturer_class_;
  const ScopedGlobalRef<jclass> j_observer_class_;

  // Used on the Java thread running the camera.
  webrtc::I420BufferPool pre_scale_pool_;
  webrtc::I420BufferPool post_scale_pool_;
  rtc::scoped_refptr<SurfaceTextureHelper> surface_texture_helper_;
  rtc::ThreadChecker thread_checker_;

  // |capturer| is a guaranteed to be a valid pointer between a call to
  // AndroidVideoCapturerDelegate::Start
  // until AndroidVideoCapturerDelegate::Stop.
  rtc::CriticalSection capturer_lock_;
  webrtc::AndroidVideoCapturer* capturer_ GUARDED_BY(capturer_lock_);
  // |invoker_| is used to communicate with |capturer_| on the thread Start() is
  // called on.
  std::unique_ptr<rtc::GuardedAsyncInvoker> invoker_ GUARDED_BY(capturer_lock_);

  static jobject application_context_;

  RTC_DISALLOW_COPY_AND_ASSIGN(AndroidVideoCapturerJni);
};

}  // namespace webrtc_jni

#endif  // WEBRTC_API_JAVA_JNI_ANDROIDVIDEOCAPTURER_JNI_H_

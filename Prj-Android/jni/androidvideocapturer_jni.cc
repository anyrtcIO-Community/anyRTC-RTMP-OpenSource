/*
 *  Copyright 2015 The WebRTC@AnyRTC project authors. All Rights Reserved.
 *
 *  Use of this source code is governed by a BSD-style license
 *  that can be found in the LICENSE file in the root of the source
 *  tree. An additional intellectual property rights grant can be found
 *  in the file PATENTS.  All contributing project authors may
 *  be found in the AUTHORS file in the root of the source tree.
 */

#include "webrtc/api/java/jni/androidvideocapturer_jni.h"
#include "webrtc/api/java/jni/classreferenceholder.h"
#include "webrtc/api/java/jni/native_handle_impl.h"
#include "webrtc/api/java/jni/surfacetexturehelper_jni.h"
#include "third_party/libyuv/include/libyuv/convert.h"
#include "webrtc/base/bind.h"

namespace webrtc_jni {

jobject AndroidVideoCapturerJni::application_context_ = nullptr;

// static
int AndroidVideoCapturerJni::SetAndroidObjects(JNIEnv* jni,
                                               jobject appliction_context) {
  if (application_context_) {
    jni->DeleteGlobalRef(application_context_);
  }
  application_context_ = NewGlobalRef(jni, appliction_context);

  return 0;
}

AndroidVideoCapturerJni::AndroidVideoCapturerJni(JNIEnv* jni,
                                                 jobject j_video_capturer,
                                                 jobject j_egl_context)
    : j_video_capturer_(jni, j_video_capturer),
      j_video_capturer_class_(jni, FindClass(jni, "org/webrtc/VideoCapturer")),
      j_observer_class_(
          jni,
          FindClass(jni,
                    "org/webrtc/VideoCapturer$NativeObserver")),
      surface_texture_helper_(SurfaceTextureHelper::create(
          jni, "Camera SurfaceTextureHelper", j_egl_context)),
      capturer_(nullptr) {
  LOG(LS_INFO) << "AndroidVideoCapturerJni ctor";
  thread_checker_.DetachFromThread();
}

AndroidVideoCapturerJni::~AndroidVideoCapturerJni() {
  LOG(LS_INFO) << "AndroidVideoCapturerJni dtor";
  jni()->CallVoidMethod(
      *j_video_capturer_,
      GetMethodID(jni(), *j_video_capturer_class_, "dispose", "()V"));
  CHECK_EXCEPTION(jni()) << "error during VideoCapturer.dispose()";
}

void AndroidVideoCapturerJni::Start(int width, int height, int framerate,
                                    webrtc::AndroidVideoCapturer* capturer) {
  LOG(LS_INFO) << "AndroidVideoCapturerJni start";
  RTC_DCHECK(thread_checker_.CalledOnValidThread());
  {
    rtc::CritScope cs(&capturer_lock_);
    RTC_CHECK(capturer_ == nullptr);
    RTC_CHECK(invoker_.get() == nullptr);
    capturer_ = capturer;
    invoker_.reset(new rtc::GuardedAsyncInvoker());
  }
  jobject j_frame_observer =
      jni()->NewObject(*j_observer_class_,
                       GetMethodID(jni(), *j_observer_class_, "<init>", "(J)V"),
                       jlongFromPointer(this));
  CHECK_EXCEPTION(jni()) << "error during NewObject";

  jmethodID m = GetMethodID(
      jni(), *j_video_capturer_class_, "startCapture",
      "(IIILorg/webrtc/SurfaceTextureHelper;Landroid/content/Context;"
      "Lorg/webrtc/VideoCapturer$CapturerObserver;)V");
  jni()->CallVoidMethod(
      *j_video_capturer_, m, width, height, framerate,
      surface_texture_helper_
          ? surface_texture_helper_->GetJavaSurfaceTextureHelper()
          : nullptr,
      application_context_, j_frame_observer);
  CHECK_EXCEPTION(jni()) << "error during VideoCapturer.startCapture";
}

void AndroidVideoCapturerJni::Stop() {
  LOG(LS_INFO) << "AndroidVideoCapturerJni stop";
  RTC_DCHECK(thread_checker_.CalledOnValidThread());
  {
    // TODO(nisse): Consider moving this block until *after* the call to
    // stopCapturer. stopCapturer should ensure that we get no
    // more frames, and then we shouldn't need the if (!capturer_)
    // checks in OnMemoryBufferFrame and OnTextureFrame.
    rtc::CritScope cs(&capturer_lock_);
    // Destroying |invoker_| will cancel all pending calls to |capturer_|.
    invoker_ = nullptr;
    capturer_ = nullptr;
  }
  jmethodID m = GetMethodID(jni(), *j_video_capturer_class_,
                            "stopCapture", "()V");
  jni()->CallVoidMethod(*j_video_capturer_, m);
  CHECK_EXCEPTION(jni()) << "error during VideoCapturer.stopCapture";
  LOG(LS_INFO) << "AndroidVideoCapturerJni stop done";
}

template <typename... Args>
void AndroidVideoCapturerJni::AsyncCapturerInvoke(
    const rtc::Location& posted_from,
    void (webrtc::AndroidVideoCapturer::*method)(Args...),
    typename Identity<Args>::type... args) {
  rtc::CritScope cs(&capturer_lock_);
  if (!invoker_) {
    LOG(LS_WARNING) << posted_from.function_name()
                    << "() called for closed capturer.";
    return;
  }
  invoker_->AsyncInvoke<void>(posted_from,
                              rtc::Bind(method, capturer_, args...));
}

std::vector<cricket::VideoFormat>
AndroidVideoCapturerJni::GetSupportedFormats() {
  JNIEnv* jni = AttachCurrentThreadIfNeeded();
  jobject j_list_of_formats = jni->CallObjectMethod(
      *j_video_capturer_,
      GetMethodID(jni, *j_video_capturer_class_, "getSupportedFormats",
                  "()Ljava/util/List;"));
  CHECK_EXCEPTION(jni) << "error during getSupportedFormats";

  // Extract Java List<CaptureFormat> to std::vector<cricket::VideoFormat>.
  jclass j_list_class = jni->FindClass("java/util/List");
  jclass j_format_class =
      jni->FindClass("org/webrtc/CameraEnumerationAndroid$CaptureFormat");
  jclass j_framerate_class = jni->FindClass(
      "org/webrtc/CameraEnumerationAndroid$CaptureFormat$FramerateRange");
  const int size = jni->CallIntMethod(
      j_list_of_formats, GetMethodID(jni, j_list_class, "size", "()I"));
  jmethodID j_get =
      GetMethodID(jni, j_list_class, "get", "(I)Ljava/lang/Object;");
  jfieldID j_framerate_field = GetFieldID(
      jni, j_format_class, "framerate",
      "Lorg/webrtc/CameraEnumerationAndroid$CaptureFormat$FramerateRange;");
  jfieldID j_width_field = GetFieldID(jni, j_format_class, "width", "I");
  jfieldID j_height_field = GetFieldID(jni, j_format_class, "height", "I");
  jfieldID j_max_framerate_field =
      GetFieldID(jni, j_framerate_class, "max", "I");

  std::vector<cricket::VideoFormat> formats;
  formats.reserve(size);
  for (int i = 0; i < size; ++i) {
    jobject j_format = jni->CallObjectMethod(j_list_of_formats, j_get, i);
    jobject j_framerate = GetObjectField(jni, j_format, j_framerate_field);
    const int frame_interval = cricket::VideoFormat::FpsToInterval(
        (GetIntField(jni, j_framerate, j_max_framerate_field) + 999) / 1000);
    formats.emplace_back(GetIntField(jni, j_format, j_width_field),
                         GetIntField(jni, j_format, j_height_field),
                         frame_interval, cricket::FOURCC_NV21);
  }
  CHECK_EXCEPTION(jni) << "error while extracting formats";
  return formats;
}

void AndroidVideoCapturerJni::OnCapturerStarted(bool success) {
  LOG(LS_INFO) << "AndroidVideoCapturerJni capture started: " << success;
  AsyncCapturerInvoke(
      RTC_FROM_HERE, &webrtc::AndroidVideoCapturer::OnCapturerStarted, success);
}

void AndroidVideoCapturerJni::OnMemoryBufferFrame(void* video_frame,
                                                  int length,
                                                  int width,
                                                  int height,
                                                  int rotation,
                                                  int64_t timestamp_ns) {
  RTC_DCHECK(rotation == 0 || rotation == 90 || rotation == 180 ||
             rotation == 270);
  rtc::CritScope cs(&capturer_lock_);
  if (!capturer_) {
    LOG(LS_WARNING) << "OnMemoryBufferFrame() called for closed capturer.";
    return;
  }
  int adapted_width;
  int adapted_height;
  int crop_width;
  int crop_height;
  int crop_x;
  int crop_y;
  int64_t translated_camera_time_us;

  if (!capturer_->AdaptFrame(width, height,
                             timestamp_ns / rtc::kNumNanosecsPerMicrosec,
                             rtc::TimeMicros(),
                             &adapted_width, &adapted_height,
                             &crop_width, &crop_height, &crop_x, &crop_y,
                             &translated_camera_time_us)) {
    return;
  }

  int rotated_width = crop_width;
  int rotated_height = crop_height;

  if (capturer_->apply_rotation() && (rotation == 90 || rotation == 270)) {
    std::swap(adapted_width, adapted_height);
    std::swap(rotated_width, rotated_height);
  }

  rtc::scoped_refptr<webrtc::VideoFrameBuffer> buffer =
      pre_scale_pool_.CreateBuffer(rotated_width, rotated_height);

  const uint8_t* y_plane = static_cast<const uint8_t*>(video_frame);
  const uint8_t* uv_plane = y_plane + width * height;

  // Can only crop at even pixels.
  crop_x &= ~1;
  crop_y &= ~1;
  int uv_width = (width + 1) / 2;

  libyuv::NV12ToI420Rotate(
      y_plane + width * crop_y + crop_x, width,
      uv_plane + uv_width * crop_y + crop_x, width,
      buffer->MutableDataY(), buffer->StrideY(),
      // Swap U and V, since we have NV21, not NV12.
      buffer->MutableDataV(), buffer->StrideV(),
      buffer->MutableDataU(), buffer->StrideU(),
      crop_width, crop_height, static_cast<libyuv::RotationMode>(
          capturer_->apply_rotation() ? rotation : 0));

  if (adapted_width != buffer->width() || adapted_height != buffer->height()) {
    rtc::scoped_refptr<webrtc::I420Buffer> scaled_buffer(
        post_scale_pool_.CreateBuffer(adapted_width, adapted_height));
    scaled_buffer->ScaleFrom(buffer);
    buffer = scaled_buffer;
  }
  capturer_->OnFrame(cricket::WebRtcVideoFrame(
                         buffer,
                         capturer_->apply_rotation()
                             ? webrtc::kVideoRotation_0
                             : static_cast<webrtc::VideoRotation>(rotation),
                         translated_camera_time_us),
                     width, height);
}

void AndroidVideoCapturerJni::OnTextureFrame(int width,
                                             int height,
                                             int rotation,
                                             int64_t timestamp_ns,
                                             const NativeHandleImpl& handle) {
  RTC_DCHECK(rotation == 0 || rotation == 90 || rotation == 180 ||
             rotation == 270);
  rtc::CritScope cs(&capturer_lock_);
  if (!capturer_) {
    LOG(LS_WARNING) << "OnTextureFrame() called for closed capturer.";
    surface_texture_helper_->ReturnTextureFrame();
    return;
  }
  int adapted_width;
  int adapted_height;
  int crop_width;
  int crop_height;
  int crop_x;
  int crop_y;
  int64_t translated_camera_time_us;

  if (!capturer_->AdaptFrame(width, height,
                             timestamp_ns / rtc::kNumNanosecsPerMicrosec,
                             rtc::TimeMicros(),
                             &adapted_width, &adapted_height,
                             &crop_width, &crop_height, &crop_x, &crop_y,
                             &translated_camera_time_us)) {
    surface_texture_helper_->ReturnTextureFrame();
    return;
  }

  Matrix matrix = handle.sampling_matrix;

  matrix.Crop(crop_width / static_cast<float>(width),
              crop_height / static_cast<float>(height),
              crop_x / static_cast<float>(width),
              crop_y / static_cast<float>(height));

  if (capturer_->apply_rotation()) {
    if (rotation == webrtc::kVideoRotation_90 ||
        rotation == webrtc::kVideoRotation_270) {
      std::swap(adapted_width, adapted_height);
    }
    matrix.Rotate(static_cast<webrtc::VideoRotation>(rotation));
  }

  capturer_->OnFrame(
      cricket::WebRtcVideoFrame(
          surface_texture_helper_->CreateTextureFrame(
              adapted_width, adapted_height,
              NativeHandleImpl(handle.oes_texture_id, matrix)),
          capturer_->apply_rotation()
              ? webrtc::kVideoRotation_0
              : static_cast<webrtc::VideoRotation>(rotation),
          translated_camera_time_us),
      width, height);
}

void AndroidVideoCapturerJni::OnOutputFormatRequest(int width,
                                                    int height,
                                                    int fps) {
  AsyncCapturerInvoke(RTC_FROM_HERE,
                      &webrtc::AndroidVideoCapturer::OnOutputFormatRequest,
                      width, height, fps);
}

JNIEnv* AndroidVideoCapturerJni::jni() { return AttachCurrentThreadIfNeeded(); }

JOW(void,
    VideoCapturer_00024NativeObserver_nativeOnByteBufferFrameCaptured)
    (JNIEnv* jni, jclass, jlong j_capturer, jbyteArray j_frame, jint length,
        jint width, jint height, jint rotation, jlong timestamp) {
  jboolean is_copy = true;
  jbyte* bytes = jni->GetByteArrayElements(j_frame, &is_copy);
  reinterpret_cast<AndroidVideoCapturerJni*>(j_capturer)
      ->OnMemoryBufferFrame(bytes, length, width, height, rotation, timestamp);
  jni->ReleaseByteArrayElements(j_frame, bytes, JNI_ABORT);
}

JOW(void, VideoCapturer_00024NativeObserver_nativeOnTextureFrameCaptured)
    (JNIEnv* jni, jclass, jlong j_capturer, jint j_width, jint j_height,
        jint j_oes_texture_id, jfloatArray j_transform_matrix,
        jint j_rotation, jlong j_timestamp) {
   reinterpret_cast<AndroidVideoCapturerJni*>(j_capturer)
         ->OnTextureFrame(j_width, j_height, j_rotation, j_timestamp,
                          NativeHandleImpl(jni, j_oes_texture_id,
                                           j_transform_matrix));
}

JOW(void, VideoCapturer_00024NativeObserver_nativeCapturerStarted)
    (JNIEnv* jni, jclass, jlong j_capturer, jboolean j_success) {
  LOG(LS_INFO) << "NativeObserver_nativeCapturerStarted";
  reinterpret_cast<AndroidVideoCapturerJni*>(j_capturer)->OnCapturerStarted(
      j_success);
}

JOW(void, VideoCapturer_00024NativeObserver_nativeOnOutputFormatRequest)
    (JNIEnv* jni, jclass, jlong j_capturer, jint j_width, jint j_height,
        jint j_fps) {
  LOG(LS_INFO) << "NativeObserver_nativeOnOutputFormatRequest";
  reinterpret_cast<AndroidVideoCapturerJni*>(j_capturer)->OnOutputFormatRequest(
      j_width, j_height, j_fps);
}

}  // namespace webrtc_jni

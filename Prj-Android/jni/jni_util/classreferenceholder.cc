/*
 *  Copyright 2015 The WebRTC@AnyRTC project authors. All Rights Reserved.
 *
 *  Use of this source code is governed by a BSD-style license
 *  that can be found in the LICENSE file in the root of the source
 *  tree. An additional intellectual property rights grant can be found
 *  in the file PATENTS.  All contributing project authors may
 *  be found in the AUTHORS file in the root of the source tree.
 */
#include "webrtc/api/java/jni/classreferenceholder.h"

#include "webrtc/api/java/jni/jni_helpers.h"

namespace webrtc_jni {

// ClassReferenceHolder holds global reference to Java classes in app/webrtc.
class ClassReferenceHolder {
 public:
  explicit ClassReferenceHolder(JNIEnv* jni);
  ~ClassReferenceHolder();

  void FreeReferences(JNIEnv* jni);
  jclass GetClass(const std::string& name);

 private:
  void LoadClass(JNIEnv* jni, const std::string& name);

  std::map<std::string, jclass> classes_;
};

// Allocated in LoadGlobalClassReferenceHolder(),
// freed in FreeGlobalClassReferenceHolder().
static ClassReferenceHolder* g_class_reference_holder = nullptr;

void LoadGlobalClassReferenceHolder() {
  RTC_CHECK(g_class_reference_holder == nullptr);
  g_class_reference_holder = new ClassReferenceHolder(GetEnv());
}

void FreeGlobalClassReferenceHolder() {
  g_class_reference_holder->FreeReferences(AttachCurrentThreadIfNeeded());
  delete g_class_reference_holder;
  g_class_reference_holder = nullptr;
}

ClassReferenceHolder::ClassReferenceHolder(JNIEnv* jni) {
  LoadClass(jni, "android/graphics/SurfaceTexture");
  LoadClass(jni, "java/nio/ByteBuffer");
  LoadClass(jni, "java/util/ArrayList");
  LoadClass(jni, "org/webrtc/CameraEnumerator");
  LoadClass(jni, "org/webrtc/Camera2Enumerator");
  LoadClass(jni, "org/webrtc/CameraEnumerationAndroid");
  LoadClass(jni, "org/webrtc/EglBase");
  LoadClass(jni, "org/webrtc/EglBase$Context");
  LoadClass(jni, "org/webrtc/EglBase14$Context");
  LoadClass(jni, "org/webrtc/MediaCodecVideoEncoder");
  LoadClass(jni, "org/webrtc/MediaCodecVideoEncoder$OutputBufferInfo");
  LoadClass(jni, "org/webrtc/MediaCodecVideoEncoder$VideoCodecType");
  LoadClass(jni, "org/webrtc/MediaCodecVideoDecoder");
  LoadClass(jni, "org/webrtc/MediaCodecVideoDecoder$DecodedTextureBuffer");
  LoadClass(jni, "org/webrtc/MediaCodecVideoDecoder$DecodedOutputBuffer");
  LoadClass(jni, "org/webrtc/MediaCodecVideoDecoder$VideoCodecType");
  LoadClass(jni, "org/webrtc/SurfaceTextureHelper");
  LoadClass(jni, "org/webrtc/VideoCapturer");
  LoadClass(jni, "org/webrtc/VideoCapturer$NativeObserver");
  LoadClass(jni, "org/webrtc/VideoRenderer$I420Frame");
}

ClassReferenceHolder::~ClassReferenceHolder() {
  RTC_CHECK(classes_.empty()) << "Must call FreeReferences() before dtor!";
}

void ClassReferenceHolder::FreeReferences(JNIEnv* jni) {
  for (std::map<std::string, jclass>::const_iterator it = classes_.begin();
      it != classes_.end(); ++it) {
    jni->DeleteGlobalRef(it->second);
  }
  classes_.clear();
}

jclass ClassReferenceHolder::GetClass(const std::string& name) {
  std::map<std::string, jclass>::iterator it = classes_.find(name);
  RTC_CHECK(it != classes_.end()) << "Unexpected GetClass() call for: " << name;
  return it->second;
}

void ClassReferenceHolder::LoadClass(JNIEnv* jni, const std::string& name) {
  jclass localRef = jni->FindClass(name.c_str());
  CHECK_EXCEPTION(jni) << "error during FindClass: " << name;
  RTC_CHECK(localRef) << name;
  jclass globalRef = reinterpret_cast<jclass>(jni->NewGlobalRef(localRef));
  CHECK_EXCEPTION(jni) << "error during NewGlobalRef: " << name;
  RTC_CHECK(globalRef) << name;
  bool inserted = classes_.insert(std::make_pair(name, globalRef)).second;
  RTC_CHECK(inserted) << "Duplicate class name: " << name;
}

// Returns a global reference guaranteed to be valid for the lifetime of the
// process.
jclass FindClass(JNIEnv* jni, const char* name) {
  return g_class_reference_holder->GetClass(name);
}

}  // namespace webrtc_jni

/*
*  Copyright (c) 2016 The AnyRTC project authors. All Rights Reserved.
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
#include <stdio.h>
#include <stdlib.h>
#include <android/log.h>
#include "anyrtmpcore.h"
#include "jRTMPHosterImpl.h"
#include "jRTMPGuestImpl.h"

#include "webrtc/api/java/jni/androidvideocapturer_jni.h"
#include "webrtc/api/java/jni/androidmediaencoder_jni.h"
#include "webrtc/api/java/jni/classreferenceholder.h"
#include "webrtc/api/java/jni/jni_helpers.h"
#include "webrtc/api/java/jni/native_handle_impl.h"
#include "webrtc/base/scoped_ptr.h"
#include "webrtc/media/base/videoframe.h"
#include "webrtc/modules/utility/include/helpers_android.h"
#include "webrtc/modules/utility/include/jvm_android.h"

static bool av_static_initialized = false;
#define TAG		"JAnyRTC-RTMP-APP"
#define COPY_BY "AnyRTC@BoYuan(SH) Information"
#define AUTHOR	"Eric@AnyRTC"
#define ALOGD(...) __android_log_print(ANDROID_LOG_DEBUG, TAG, __VA_ARGS__)
#define ALOGE(...) __android_log_print(ANDROID_LOG_ERROR, TAG, __VA_ARGS__)

namespace webrtc_jni {
// Wrapper dispatching rtc::VideoSinkInterface to a Java VideoRenderer
// instance.
class JavaVideoRendererWrapper
    : public rtc::VideoSinkInterface<cricket::VideoFrame> {
 public:
  JavaVideoRendererWrapper(JNIEnv* jni, jobject j_callbacks)
      : j_callbacks_(jni, j_callbacks),
        j_render_frame_id_(GetMethodID(
            jni, GetObjectClass(jni, j_callbacks), "renderFrame",
            "(Lorg/webrtc/VideoRenderer$I420Frame;)V")),
        j_frame_class_(jni,
                       FindClass(jni, "org/webrtc/VideoRenderer$I420Frame")),
        j_i420_frame_ctor_id_(GetMethodID(
            jni, *j_frame_class_, "<init>", "(III[I[Ljava/nio/ByteBuffer;J)V")),
        j_texture_frame_ctor_id_(GetMethodID(
            jni, *j_frame_class_, "<init>",
            "(IIII[FJ)V")),
        j_byte_buffer_class_(jni, FindClass(jni, "java/nio/ByteBuffer")) {
    CHECK_EXCEPTION(jni);
  }

  virtual ~JavaVideoRendererWrapper() {}

  void OnFrame(const cricket::VideoFrame& video_frame) override {
    ScopedLocalRefFrame local_ref_frame(jni());
    jobject j_frame =
        (video_frame.video_frame_buffer()->native_handle() != nullptr)
            ? CricketToJavaTextureFrame(&video_frame)
            : CricketToJavaI420Frame(&video_frame);
    // |j_callbacks_| is responsible for releasing |j_frame| with
    // VideoRenderer.renderFrameDone().
    jni()->CallVoidMethod(*j_callbacks_, j_render_frame_id_, j_frame);
    CHECK_EXCEPTION(jni());
  }

 private:
  // Make a shallow copy of |frame| to be used with Java. The callee has
  // ownership of the frame, and the frame should be released with
  // VideoRenderer.releaseNativeFrame().
  static jlong javaShallowCopy(const cricket::VideoFrame* frame) {
    return jlongFromPointer(frame->Copy());
  }

  // Return a VideoRenderer.I420Frame referring to the data in |frame|.
  jobject CricketToJavaI420Frame(const cricket::VideoFrame* frame) {
    jintArray strides = jni()->NewIntArray(3);
    jint* strides_array = jni()->GetIntArrayElements(strides, NULL);
    strides_array[0] = frame->video_frame_buffer()->StrideY();
    strides_array[1] = frame->video_frame_buffer()->StrideU();
    strides_array[2] = frame->video_frame_buffer()->StrideV();
    jni()->ReleaseIntArrayElements(strides, strides_array, 0);
    jobjectArray planes = jni()->NewObjectArray(3, *j_byte_buffer_class_, NULL);
    jobject y_buffer = jni()->NewDirectByteBuffer(
        const_cast<uint8_t*>(frame->video_frame_buffer()->DataY()),
        frame->video_frame_buffer()->StrideY() *
            frame->video_frame_buffer()->height());
    size_t chroma_height = (frame->height() + 1) / 2;
    jobject u_buffer = jni()->NewDirectByteBuffer(
        const_cast<uint8_t*>(frame->video_frame_buffer()->DataU()),
        frame->video_frame_buffer()->StrideU() * chroma_height);
    jobject v_buffer = jni()->NewDirectByteBuffer(
        const_cast<uint8_t*>(frame->video_frame_buffer()->DataV()),
        frame->video_frame_buffer()->StrideV() * chroma_height);

    jni()->SetObjectArrayElement(planes, 0, y_buffer);
    jni()->SetObjectArrayElement(planes, 1, u_buffer);
    jni()->SetObjectArrayElement(planes, 2, v_buffer);
    return jni()->NewObject(
        *j_frame_class_, j_i420_frame_ctor_id_,
        frame->width(), frame->height(),
        static_cast<int>(frame->rotation()),
        strides, planes, javaShallowCopy(frame));
  }

  // Return a VideoRenderer.I420Frame referring texture object in |frame|.
  jobject CricketToJavaTextureFrame(const cricket::VideoFrame* frame) {
    NativeHandleImpl* handle = reinterpret_cast<NativeHandleImpl*>(
        frame->video_frame_buffer()->native_handle());
    jfloatArray sampling_matrix = handle->sampling_matrix.ToJava(jni());

    return jni()->NewObject(
        *j_frame_class_, j_texture_frame_ctor_id_,
        frame->width(), frame->height(),
        static_cast<int>(frame->rotation()),
        handle->oes_texture_id, sampling_matrix, javaShallowCopy(frame));
  }

  JNIEnv* jni() {
    return AttachCurrentThreadIfNeeded();
  }

  ScopedGlobalRef<jobject> j_callbacks_;
  jmethodID j_render_frame_id_;
  ScopedGlobalRef<jclass> j_frame_class_;
  jmethodID j_i420_frame_ctor_id_;
  jmethodID j_texture_frame_ctor_id_;
  ScopedGlobalRef<jclass> j_byte_buffer_class_;
};

// Macro for native functions that can be found by way of jni-auto discovery.
// Note extern "C" is needed for "discovery" of native methods to work.
#define JOWW(rettype, name)                                             \
  extern "C" rettype JNIEXPORT JNICALL Java_org_anyrtc_core_##name


//=================================================================
namespace {
jlong GetJApp(JNIEnv* jni, jobject j_app)
{
  jclass j_app_class = jni->GetObjectClass(j_app);
  jfieldID native_id =
      jni->GetFieldID(j_app_class, "fNativeAppId", "J");
  return jni->GetLongField(j_app, native_id);
}

}

//=================================================================
//* AnyRTM.class
//=================================================================
JOWW(void, AnyRTMP_nativeInitCtx)(JNIEnv* jni, jclass, jobject context, jobject egl_context)
{
	if(!av_static_initialized)
	{
        // talk/ assumes pretty widely that the current Thread is ThreadManager'd, but
        // ThreadManager only WrapCurrentThread()s the thread where it is first
        // created.  Since the semantics around when auto-wrapping happens in
        // webrtc/base/ are convoluted, we simply wrap here to avoid having to think
        // about ramifications of auto-wrapping there.
        rtc::ThreadManager::Instance()->WrapCurrentThread();

		 ALOGD("JVM::Initialize nativeInitCtx");
		//* Set Video Context
		webrtc_jni::AndroidVideoCapturerJni::SetAndroidObjects(jni, context);
		//* Set Audio Context
		webrtc::JVM::Initialize(webrtc_jni::GetJVM(), context);
		av_static_initialized = true;
        
        jclass j_eglbase14_context_class = webrtc_jni::FindClass(jni, "org/webrtc/EglBase14$Context");
        if (jni->IsInstanceOf(egl_context, j_eglbase14_context_class)) {
            webrtc_jni::MediaCodecVideoEncoderFactory* java_encoder = new webrtc_jni::MediaCodecVideoEncoderFactory();
            rtc::scoped_ptr<cricket::WebRtcVideoEncoderFactory> external_encoder(java_encoder);
            java_encoder->SetEGLContext(jni, egl_context);
            
            webrtc::AnyRtmpCore::Inst().SetExternalVideoEncoderFactory(external_encoder.release());
        }
        

        rtc::LogMessage::LogToDebug(rtc::LS_ERROR);
	}
}

//=================================================================
//* RTMPHosterKit.class
//=================================================================
JOWW(jlong, RTMPHosterKit_nativeCreate)(JNIEnv* jni, jclass, jobject j_obj)
{
	JRTMPHosterImpl* jApp = new JRTMPHosterImpl(j_obj);
	return webrtc_jni::jlongFromPointer(jApp);
}

JOWW(void, RTMPHosterKit_nativeSetAudioEnable)(JNIEnv* jni, jobject j_app, jboolean j_enable)
{
	JRTMPHosterImpl* jApp = (JRTMPHosterImpl*)GetJApp(jni, j_app);
	jApp->Hoster()->SetAudioEnable(j_enable);
}

JOWW(void, RTMPHosterKit_nativeSetVideoEnable)(JNIEnv* jni, jobject j_app, jboolean j_enable)
{
	JRTMPHosterImpl* jApp = (JRTMPHosterImpl*)GetJApp(jni, j_app);
	jApp->Hoster()->SetVideoEnable(j_enable);
}

JOWW(void, RTMPHosterKit_nativeSetVideoCapturer)(JNIEnv* jni, jobject j_app, jobject j_video_capturer, jlong j_renderer_pointer) 
{
	JRTMPHosterImpl* jApp = (JRTMPHosterImpl*)GetJApp(jni, j_app);

	if(j_video_capturer != NULL) {
		jobject j_egl_context = NULL;
		// Create a cricket::VideoCapturer from |j_video_capturer|.
		rtc::scoped_refptr<webrtc::AndroidVideoCapturerDelegate> delegate =
		  new rtc::RefCountedObject<AndroidVideoCapturerJni>(
			  jni, j_video_capturer, j_egl_context);
		std::unique_ptr<cricket::VideoCapturer> capturer(
		  new webrtc::AndroidVideoCapturer(delegate));

		jApp->Hoster()->SetVideoRender(reinterpret_cast<rtc::VideoSinkInterface<cricket::VideoFrame>*>(j_renderer_pointer));
		jApp->Hoster()->SetVideoCapturer(capturer.release());
	}
	else 
	{
		jApp->Hoster()->SetVideoCapturer(NULL);
	}
}

JOWW(void, RTMPHosterKit_nativeStartRtmpStream)(JNIEnv* jni, jobject j_app, jstring j_rtmp_url)
{
	JRTMPHosterImpl* jApp = (JRTMPHosterImpl*)GetJApp(jni, j_app);
	
	std::string rtmp_url = JavaToStdString(jni, j_rtmp_url);
	jApp->Hoster()->StartRtmpStream(rtmp_url.c_str());
}

JOWW(void, RTMPHosterKit_nativeStopRtmpStream)(JNIEnv* jni, jobject j_app)
{
	JRTMPHosterImpl* jApp = (JRTMPHosterImpl*)GetJApp(jni, j_app);
	
	jApp->Hoster()->StopRtmpStream();
}

JOWW(void, RTMPHosterKit_nativeDestroy)(JNIEnv* jni, jobject j_app)
{
	JRTMPHosterImpl* jApp = (JRTMPHosterImpl*)GetJApp(jni, j_app);
	jApp->Close();
	delete jApp;
}

//=================================================================
//* RTMPGuestKit.class
//=================================================================
JOWW(jlong, RTMPGuestKit_nativeCreate)(JNIEnv* jni, jclass, jobject j_obj)
{
	JRTMPGuestImpl* jApp = new JRTMPGuestImpl(j_obj);
	return webrtc_jni::jlongFromPointer(jApp);
}

JOWW(void, RTMPGuestKit_nativeStartRtmpPlay)(JNIEnv* jni, jobject j_app, jstring j_rtmp_url, jlong j_renderer_pointer)
{
	JRTMPGuestImpl* jApp = (JRTMPGuestImpl*)GetJApp(jni, j_app);
	
	std::string rtmp_url = JavaToStdString(jni, j_rtmp_url);
	jApp->Guest()->StartRtmpPlay(rtmp_url.c_str(), reinterpret_cast<rtc::VideoSinkInterface<cricket::VideoFrame>*>(j_renderer_pointer));
}

JOWW(void, RTMPGuestKit_nativeStopRtmpPlay)(JNIEnv* jni, jobject j_app)
{
	JRTMPGuestImpl* jApp = (JRTMPGuestImpl*)GetJApp(jni, j_app);
	
	jApp->Guest()->StopRtmpPlay();
}

JOWW(void, RTMPGuestKit_nativeDestroy)(JNIEnv* jni, jobject j_app)
{
	JRTMPGuestImpl* jApp = (JRTMPGuestImpl*)GetJApp(jni, j_app);
	jApp->Close();
	delete jApp;
}

//=============================================================================
//* For VideoRenderer
JOW(void, VideoRenderer_freeWrappedVideoRenderer)(JNIEnv*, jclass, jlong j_p) {
  delete reinterpret_cast<JavaVideoRendererWrapper*>(j_p);
}

JOW(void, VideoRenderer_releaseNativeFrame)(
    JNIEnv* jni, jclass, jlong j_frame_ptr) {
  delete reinterpret_cast<const cricket::VideoFrame*>(j_frame_ptr);
}

JOW(jlong, VideoRenderer_nativeWrapVideoRenderer)(
    JNIEnv* jni, jclass, jobject j_callbacks) {
  std::unique_ptr<JavaVideoRendererWrapper> renderer(
      new JavaVideoRendererWrapper(jni, j_callbacks));
  return (jlong)renderer.release();
}

JOW(void, VideoRenderer_nativeCopyPlane)(
    JNIEnv *jni, jclass, jobject j_src_buffer, jint width, jint height,
    jint src_stride, jobject j_dst_buffer, jint dst_stride) {
  size_t src_size = jni->GetDirectBufferCapacity(j_src_buffer);
  size_t dst_size = jni->GetDirectBufferCapacity(j_dst_buffer);
  RTC_CHECK(src_stride >= width) << "Wrong source stride " << src_stride;
  RTC_CHECK(dst_stride >= width) << "Wrong destination stride " << dst_stride;
  RTC_CHECK(src_size >= src_stride * height)
      << "Insufficient source buffer capacity " << src_size;
  RTC_CHECK(dst_size >= dst_stride * height)
      << "Insufficient destination buffer capacity " << dst_size;
  uint8_t *src =
      reinterpret_cast<uint8_t*>(jni->GetDirectBufferAddress(j_src_buffer));
  uint8_t *dst =
      reinterpret_cast<uint8_t*>(jni->GetDirectBufferAddress(j_dst_buffer));
  if (src_stride == dst_stride) {
    memcpy(dst, src, src_stride * height);
  } else {
    for (int i = 0; i < height; i++) {
      memcpy(dst, src, width);
      src += src_stride;
      dst += dst_stride;
    }
  }
}

}	// namespace webrtc_jni
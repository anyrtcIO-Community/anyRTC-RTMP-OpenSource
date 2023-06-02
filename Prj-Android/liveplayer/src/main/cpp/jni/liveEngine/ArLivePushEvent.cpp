//
// Created by liu on 2021/9/22.
//

#include "ArLivePushEvent.h"
#include "NativePushObserver_JNI.h"
namespace anyrtc {
    LivePushEvent::LivePushEvent(jobject event):
            m_jJavaObj(NULL),
            m_jClass(NULL) {
        if (event){
            JNIEnv *jni = webrtc::AttachCurrentThreadIfNeeded();
            m_jJavaObj = jni->NewGlobalRef(event);
            m_jClass = static_cast<jclass>(jni->NewGlobalRef(
                    jni->GetObjectClass(m_jJavaObj)));
        }
    }

    LivePushEvent::~LivePushEvent() {
        if (m_jJavaObj){
            JNIEnv *jni = webrtc::AttachCurrentThreadIfNeeded();
            jni->DeleteGlobalRef(m_jClass);
            jni->DeleteGlobalRef(m_jJavaObj);
            m_jClass = NULL;
            m_jJavaObj = NULL;
        }
    }
    void LivePushEvent::onError(int32_t code, const char* msg, void* extraInfo){
        JNIEnv *jni = webrtc::AttachCurrentThreadIfNeeded();
        if (m_jJavaObj != NULL) {
            const base::android::JavaParamRef<jobject> j_context(m_jJavaObj);
            Java_NativePushObserver_onError(jni, j_context, code, webrtc::NativeToJavaString(jni, msg),
                                            webrtc::NativeToJavaString(jni, msg));
        }
    }

    void LivePushEvent::onWarning(int32_t code, const char* msg, void* extraInfo){
        JNIEnv *jni = webrtc::AttachCurrentThreadIfNeeded();
        if (m_jJavaObj != NULL) {
            const base::android::JavaParamRef<jobject> j_context(m_jJavaObj);
            Java_NativePushObserver_onWarning(jni, j_context, code, webrtc::NativeToJavaString(jni, msg),
                                            webrtc::NativeToJavaString(jni, msg));
        }
    }

    void LivePushEvent::onPushStatusUpdate(ArLivePushStatus state, const char* msg, void* extraInfo){
        JNIEnv *jni = webrtc::AttachCurrentThreadIfNeeded();
        if (m_jJavaObj != NULL) {
            const base::android::JavaParamRef<jobject> j_context(m_jJavaObj);
            Java_NativePushObserver_onPushStatusUpdate(jni, j_context,(jint)state, webrtc::NativeToJavaString(jni, msg),
                                              webrtc::NativeToJavaString(jni, msg));
        }
    }

    void LivePushEvent::onStatisticsUpdate(ArLivePusherStatistics statistics){
        JNIEnv *jni = webrtc::AttachCurrentThreadIfNeeded();
        if (m_jJavaObj != NULL) {
            const base::android::JavaParamRef<jobject> j_context(m_jJavaObj);
            Java_NativePushObserver_onStatisticsUpdate(jni, j_context,(jint)statistics.appCpu,
                                                       (jint)statistics.systemCpu,
                                                       (jint)statistics.width,(jint)statistics.height,(jint)statistics.fps,(jint)statistics.videoBitrate,(jint)statistics.audioBitrate);
        }
    }

    void LivePushEvent::onSnapshotComplete(const char* image, int length, int width, int height, ArLivePixelFormat format){

    }
    void LivePushEvent::onMicrophoneVolumeUpdate(int32_t volume) {
        JNIEnv *jni = webrtc::AttachCurrentThreadIfNeeded();
        if (m_jJavaObj != NULL) {
            const base::android::JavaParamRef<jobject> j_context(m_jJavaObj);
            Java_NativePushObserver_onMicrophoneVolumeUpdate(jni, j_context,(jint)volume);
        }
    }
}

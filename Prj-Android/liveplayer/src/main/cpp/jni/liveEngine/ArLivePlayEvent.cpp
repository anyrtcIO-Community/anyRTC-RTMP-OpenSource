//
// Created by liu on 2021/9/22.
//

#include "ArLivePlayEvent.h"
#include "NativePlayObserver_JNI.h"

using namespace anyrtc;

LivePlayEvent::LivePlayEvent(jobject event):
        m_jJavaObj(NULL),
        m_jClass(NULL){
    if (event){
        JNIEnv *jni = webrtc::AttachCurrentThreadIfNeeded();
        m_jJavaObj = jni->NewGlobalRef(event);
        m_jClass = static_cast<jclass>(jni->NewGlobalRef(
                jni->GetObjectClass(m_jJavaObj)));
    }


}

LivePlayEvent::~LivePlayEvent(){
    if (m_jJavaObj){
        JNIEnv *jni = webrtc::AttachCurrentThreadIfNeeded();
        jni->DeleteGlobalRef(m_jClass);
        jni->DeleteGlobalRef(m_jJavaObj);
        m_jClass = NULL;
        m_jJavaObj = NULL;
    }

}


void LivePlayEvent::onError(IArLivePlayer* player, int32_t code, const char* msg, void* extraInfo){
   
    JNIEnv *jni = webrtc::AttachCurrentThreadIfNeeded();
    if (m_jJavaObj != NULL) {
        const base::android::JavaParamRef<jobject> j_context(m_jJavaObj);
        Java_NativePlayObserver_onError(jni, j_context, code, webrtc::NativeToJavaString(jni, msg),
                                        webrtc::NativeToJavaString(jni, msg));
    }
}

void LivePlayEvent::onWarning(IArLivePlayer* player, int32_t code, const char* msg, void* extraInfo){
   
    JNIEnv *jni = webrtc::AttachCurrentThreadIfNeeded();
    if (m_jJavaObj != NULL) {
        const base::android::JavaParamRef<jobject> j_context(m_jJavaObj);
        Java_NativePlayObserver_onWarning(jni, j_context, code, webrtc::NativeToJavaString(jni, msg),
                                        webrtc::NativeToJavaString(jni, msg));
    }
}

void LivePlayEvent::onVideoPlayStatusUpdate(IArLivePlayer* player, ArLivePlayStatus status, ArLiveStatusChangeReason reason, void* extraInfo){
   
    JNIEnv *jni = webrtc::AttachCurrentThreadIfNeeded();
    if (m_jJavaObj != NULL) {
        const base::android::JavaParamRef<jobject> j_context(m_jJavaObj);
        Java_NativePlayObserver_onVideoPlayStatusUpdate(jni, j_context, status, reason,extraInfo==NULL?NULL:
                                        webrtc::NativeToJavaString(jni, (char*)extraInfo));
    }


}

void LivePlayEvent::onAudioPlayStatusUpdate(IArLivePlayer* player, ArLivePlayStatus status, ArLiveStatusChangeReason reason, void* extraInfo){

   
    JNIEnv *jni = webrtc::AttachCurrentThreadIfNeeded();
    if (m_jJavaObj != NULL) {
        const base::android::JavaParamRef<jobject> j_context(m_jJavaObj);
        Java_NativePlayObserver_onAudioPlayStatusUpdate(jni, j_context, status, reason,extraInfo==NULL?NULL:
                                                                                       webrtc::NativeToJavaString(jni, (char*)extraInfo));
    }

}

void LivePlayEvent::onPlayoutVolumeUpdate(IArLivePlayer* player, int32_t volume){

   
    JNIEnv *jni = webrtc::AttachCurrentThreadIfNeeded();
    if (m_jJavaObj != NULL) {
        const base::android::JavaParamRef<jobject> j_context(m_jJavaObj);
        Java_NativePlayObserver_onPlayoutVolumeUpdate(jni, j_context, volume);
    }


}

void LivePlayEvent::onStatisticsUpdate(IArLivePlayer* player, ArLivePlayerStatistics statistics){

    JNIEnv *jni = webrtc::AttachCurrentThreadIfNeeded();
    if (m_jJavaObj != NULL) {
        const base::android::JavaParamRef<jobject> j_context(m_jJavaObj);
        Java_NativePlayObserver_onStatisticsUpdate(jni, j_context, statistics.appCpu,statistics.systemCpu,statistics.width,statistics.height,statistics.fps,statistics.videoBitrate,statistics.audioBitrate);
    }

}

void LivePlayEvent::onSnapshotComplete(IArLivePlayer* player, const char* image, int length, int width, int height, ArLivePixelFormat format){

}

void LivePlayEvent::onRenderVideoFrame(IArLivePlayer* player, const ArLiveVideoFrame* videoFrame){
    if (m_jJavaObj != NULL && m_jClass !=NULL) {
       
        JNIEnv *jni = webrtc::AttachCurrentThreadIfNeeded();
        {

            jmethodID j_callJavaMId = jni->GetMethodID(m_jClass, "onRenderVideoFrame",
                                                               "(II[BLjava/nio/ByteBuffer;III)V");
                jbyteArray dataArray = jni->NewByteArray(videoFrame->length);
               jobject _buf = jni->NewDirectByteBuffer(videoFrame->data, videoFrame->length);
                jni->SetByteArrayRegion(dataArray, 0, videoFrame->length,
                                        reinterpret_cast<const jbyte *>(videoFrame->data));

                jni->CallVoidMethod(m_jJavaObj,j_callJavaMId,videoFrame->pixelFormat,videoFrame->bufferType,dataArray,_buf,videoFrame->width,videoFrame->height,videoFrame->rotation);
                jni->ReleaseByteArrayElements(dataArray, jni->GetByteArrayElements(dataArray, 0),
                                              0);
                jni->DeleteLocalRef(_buf);
        }
    }
}

void LivePlayEvent::onReceiveSeiMessage(IArLivePlayer* player, int payloadType, const uint8_t* data, uint32_t dataSize){


    if (m_jJavaObj != NULL && m_jClass !=NULL) {

        JNIEnv *jni = webrtc::AttachCurrentThreadIfNeeded();
        {

            jmethodID j_callJavaMId = jni->GetMethodID(m_jClass, "onReceiveSeiMessage",
                                                       "(I[B)V");
            jbyteArray dataArray = jni->NewByteArray(dataSize);
            jni->SetByteArrayRegion(dataArray, 0, dataSize,
                                    reinterpret_cast<const jbyte *>(data));

            jni->CallVoidMethod(m_jJavaObj,j_callJavaMId,(jint)payloadType,dataArray);
            jni->ReleaseByteArrayElements(dataArray, jni->GetByteArrayElements(dataArray, 0),
                                          0);
            jni->DeleteLocalRef(dataArray);
        }
    }
}

void LivePlayEvent::onVodPlaybackProcess(IArLivePlayer *player, int nAllDuration,
                                         int nCurrentPlaybackTime, int nBufferDuration) {
   
    JNIEnv *jni = webrtc::AttachCurrentThreadIfNeeded();
    if (m_jJavaObj != NULL) {
        const base::android::JavaParamRef<jobject> j_context(m_jJavaObj);
        Java_NativePlayObserver_onVodPlaybackProcess(jni, j_context, nAllDuration,nCurrentPlaybackTime,nBufferDuration);
    }
}

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
#include "jRTMPHosterImpl.h"
#include "webrtc/api/java/jni/classreferenceholder.h"
#include "webrtc/api/java/jni/jni_helpers.h"
#include "webrtc/modules/utility/include/helpers_android.h"

JRTMPHosterImpl::JRTMPHosterImpl(jobject javaObj)
: m_pHoster(NULL)
, m_jJavaObj(NULL)
, m_jClass(NULL)
{
	if(javaObj)
	{
		webrtc::AttachThreadScoped ats(webrtc_jni::GetJVM());
		m_jJavaObj = ats.env()->NewGlobalRef(javaObj);
		m_jClass = reinterpret_cast<jclass> (ats.env()->NewGlobalRef(ats.env()->GetObjectClass(m_jJavaObj)));
	}
	m_pHoster = RTMPHoster::Create(*this);
} 
JRTMPHosterImpl::~JRTMPHosterImpl(void)
{
	Close();
}

void JRTMPHosterImpl::Close()
{
	if (m_pHoster) {
		m_pHoster->SetVideoCapturer(NULL);
		m_pHoster->StopRtmpStream();
		RTMPHoster::Destory(m_pHoster);
		m_pHoster = NULL;
	}

	if(m_jJavaObj)
	{
		webrtc::AttachThreadScoped ats(webrtc_jni::GetJVM());
		ats.env()->DeleteGlobalRef(m_jClass);
		m_jClass = NULL;
		ats.env()->DeleteGlobalRef(m_jJavaObj);
		m_jJavaObj = NULL;
	}
}

void JRTMPHosterImpl::OnRtmpStreamOK()
{
	webrtc::AttachThreadScoped ats(webrtc_jni::GetJVM());
	JNIEnv* jni = ats.env();
	{
		// Get *** callback interface method id
		jmethodID j_callJavaMId = webrtc_jni::GetMethodID(jni, m_jClass, "OnRtmpStreamOK", "()V");
		// Callback with params
		jni->CallVoidMethod(m_jJavaObj, j_callJavaMId);
	}
}
void JRTMPHosterImpl::OnRtmpStreamReconnecting(int times)
{
	webrtc::AttachThreadScoped ats(webrtc_jni::GetJVM());
	JNIEnv* jni = ats.env();
	{
		// Get *** callback interface method id
		jmethodID j_callJavaMId = webrtc_jni::GetMethodID(jni, m_jClass, "OnRtmpStreamReconnecting", "(I)V");
		// Callback with params
		jni->CallVoidMethod(m_jJavaObj, j_callJavaMId, times);
	}
}
void JRTMPHosterImpl::OnRtmpStreamStatus(int delayMs, int netBand)
{
	webrtc::AttachThreadScoped ats(webrtc_jni::GetJVM());
	JNIEnv* jni = ats.env();
	{
		// Get *** callback interface method id
		jmethodID j_callJavaMId = webrtc_jni::GetMethodID(jni, m_jClass, "OnRtmpStreamStatus", "(II)V");
		// Callback with params
		jni->CallVoidMethod(m_jJavaObj, j_callJavaMId, delayMs, netBand);
	}
}
void JRTMPHosterImpl::OnRtmpStreamFailed(int code)
{
	webrtc::AttachThreadScoped ats(webrtc_jni::GetJVM());
	JNIEnv* jni = ats.env();
	{
		// Get *** callback interface method id
		jmethodID j_callJavaMId = webrtc_jni::GetMethodID(jni, m_jClass, "OnRtmpStreamFailed", "(I)V");
		// Callback with params
		jni->CallVoidMethod(m_jJavaObj, j_callJavaMId, code);
	}
}
void JRTMPHosterImpl::OnRtmpStreamClosed()
{
	webrtc::AttachThreadScoped ats(webrtc_jni::GetJVM());
	JNIEnv* jni = ats.env();
	{
		// Get *** callback interface method id
		jmethodID j_callJavaMId = webrtc_jni::GetMethodID(jni, m_jClass, "OnRtmpStreamClosed", "()V");
		// Callback with params
		jni->CallVoidMethod(m_jJavaObj, j_callJavaMId);
	}
}

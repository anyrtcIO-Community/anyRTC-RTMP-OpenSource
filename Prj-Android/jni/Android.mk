# Copyright (C) 2009 The Android Open Source Project
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#      http://www.apache.cpprg/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.
#
################################################################
# Ucc Jni
LOCAL_PATH := $(call my-dir)
include $(CLEAR_VARS)

LOCAL_MODULE    := anyrtmp-jni
LOCAL_SRC_FILES := ./jni_util/classreferenceholder.cc \
		./jni_util/jni_helpers.cc \
		./jni_util/jni_onload.cc \
		./jni_util/native_handle_impl.cc \
		./androidmediaencoder_jni.cc \
		./androidvideocapturer_jni.cc \
		./surfacetexturehelper_jni.cc \
		./japp_jni.cc \
		./jRTMPGuestImpl.cc \
		./jRTMPHosterImpl.cc \
		./video_render.cc 
	
## 
## Widows (call host-path,/cygdrive/path/to/your/file/libstlport_shared.so) 	
#
#LOCAL_LDLIBS += -L$(call host-path,$(LOCAL_PATH)/library) -lopenh264   
LOCAL_LDLIBS := -llog -lz -lOpenSLES
ifeq ($(TARGET_ARCH_ABI),armeabi)  
LOCAL_LDLIBS += -L$(call host-path,$(LOCAL_PATH)/library/arm) -lavformat -lavcodec -lavutil
else ifeq ($(TARGET_ARCH_ABI),armeabi-v7a)  
LOCAL_LDLIBS += -L$(call host-path,$(LOCAL_PATH)/library/v7) -lavformat -lavcodec -lavutil
else
LOCAL_LDLIBS += -L$(call host-path,$(LOCAL_PATH)/library/v64) -lavformat -lavcodec -lavutil
endif  	

LOCAL_C_INCLUDES += $(NDK_STL_INC) \
					$(LOCAL_PATH)/../../ \
					$(LOCAL_PATH)/../../AnyCore \
					$(LOCAL_PATH)/../../AnyCore/srs_librtmp \
					$(LOCAL_PATH)/avstreamer \
					$(LOCAL_PATH)/jni_util
					
LOCAL_CFLAGS := -std=gnu++11 -DWEBRTC_POSIX -DWEBRTC_ANDROID -D__STDC_CONSTANT_MACROS 
 
LOCAL_STATIC_LIBRARIES := anycore
LOCAL_STATIC_LIBRARIES += webrtc
LOCAL_STATIC_LIBRARIES += yuv_static
LOCAL_SHARED_LIBRARIES := openh264-p 
LOCAL_SHARED_LIBRARIES += faac
LOCAL_SHARED_LIBRARIES += faad2
include $(BUILD_SHARED_LIBRARY)
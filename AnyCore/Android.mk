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
ANYCORE := ./
include $(CLEAR_VARS)

LOCAL_MODULE    := anycore
			
LOCAL_SRC_FILES := $(ANYCORE)/srs_librtmp/srs_librtmp.cpp \
		$(ANYCORE)/aacencode.cc \
		$(ANYCORE)/aacdecode.cc \
		$(ANYCORE)/anyrtmpcore.cc \
		$(ANYCORE)/anyrtmplayer.cc \
		$(ANYCORE)/anyrtmpstreamer.cc \
		$(ANYCORE)/anyrtmpull.cc \
		$(ANYCORE)/anyrtmpush.cc \
		$(ANYCORE)/avcodec.cc \
		$(ANYCORE)/plybuffer.cc \
		$(ANYCORE)/plydecoder.cc \
		$(ANYCORE)/RtmpGuesterImpl.cc \
		$(ANYCORE)/RtmpHosterImpl.cc \
		$(ANYCORE)/videofilter.cc
	
## 
## Widows (call host-path,/cygdrive/path/to/your/file/libstlport_shared.so) 	
#		   
#LOCAL_LDLIBS := -llog
LOCAL_CFLAGS := -std=gnu++11 -frtti -Wno-literal-suffix -DWEBRTC_POSIX -DWEBRTC_ANDROID -DWEBRTC_INCLUDE_INTERNAL_AUDIO_DEVICE -D__STDC_CONSTANT_MACROS 
#

LOCAL_C_INCLUDES += $(NDK_STL_INC) \
		$(LOCAL_PATH)/srs_librtmp \
		$(LOCAL_PATH)/../ \
		$(LOCAL_PATH)/../third_party/faac-1.28/include \
		$(LOCAL_PATH)/../third_party/faad2-2.7/include
					
include $(BUILD_STATIC_LIBRARY)
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
RTMP := ../../
include $(CLEAR_VARS)

LOCAL_MODULE    := rtmp
			
LOCAL_SRC_FILES := $(RTMP)/LiveFlv/srs_librtmp/srs_librtmp.cpp
	
## 
## Widows (call host-path,/cygdrive/path/to/your/file/libstlport_shared.so) 	
#		   
#LOCAL_LDLIBS := -llog
LOCAL_CFLAGS := -std=gnu++11 -frtti -Wno-literal-suffix
#

LOCAL_C_INCLUDES += $(NDK_STL_INC) 
					
include $(BUILD_STATIC_LIBRARY)
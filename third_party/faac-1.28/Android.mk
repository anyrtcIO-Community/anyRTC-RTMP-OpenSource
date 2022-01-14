# Copyright (C) 2009 The Android Open Source Project
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#      http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.
#

LOCAL_PATH := $(call my-dir)
include $(CLEAR_VARS)

LOCAL_ARM_MODE := arm
LOCAL_CFLAGS := $(LOCAL_CFLAGS) -O3 -ffast-math -fprefetch-loop-arrays
LOCAL_CPPFLAGS := $(LOCAL_CFLAGS)

LOCAL_MODULE := libfaac
LOCAL_SRC_FILES := libfaac/aacquant.c \
	libfaac/bitstream.c \
	libfaac/fft.c \
	libfaac/frame.c \
	libfaac/midside.c \
	libfaac/psychkni.c \
	libfaac/util.c \
	libfaac/backpred.c \
	libfaac/channels.c \
	libfaac/filtbank.c \
	libfaac/huffman.c \
	libfaac/ltp.c \
	libfaac/tns.c
LOCAL_C_INCLUDES := $(LOCAL_PATH)/include/

LOCAL_LDLIBS := -ldl -lc -lz -lm

include $(BUILD_SHARED_LIBRARY)
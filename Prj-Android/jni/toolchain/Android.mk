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
MY_ROOT_PATH := $(call my-dir)/..

# library
include $(MY_ROOT_PATH)/../../AnyCore/Android.mk
include $(MY_ROOT_PATH)/../../webrtc/Android.mk
include $(MY_ROOT_PATH)/../../third_party/libyuv/Android.mk
include $(MY_ROOT_PATH)/../../third_party/faac-1.28/Android.mk
include $(MY_ROOT_PATH)/../../third_party/faad2-2.7/Android.mk
include $(MY_ROOT_PATH)/library/Android.mk
include $(MY_ROOT_PATH)/Android.mk

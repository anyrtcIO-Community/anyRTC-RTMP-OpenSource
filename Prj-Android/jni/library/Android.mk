LOCAL_PATH := $(call my-dir)  
  
include $(CLEAR_VARS)  
LOCAL_MODULE := openh264-p     			# 模块名  
ifeq ($(TARGET_ARCH_ABI),armeabi)  
LOCAL_SRC_FILES := ./arm/libopenh264.so		# 模块的文件路径（相对于 LOCAL_PATH） 
else ifeq ($(TARGET_ARCH_ABI),armeabi-v7a)   
LOCAL_SRC_FILES := ./v7/libopenh264.so
else ifeq ($(TARGET_ARCH_ABI),x86)   
LOCAL_SRC_FILES := ./x86/libopenh264.so
else
LOCAL_SRC_FILES := ./v64/libopenh264.so  
endif  

  
include $(PREBUILT_SHARED_LIBRARY) 		# 注意这里不是 BUILD_SHARED_LIBRARY  
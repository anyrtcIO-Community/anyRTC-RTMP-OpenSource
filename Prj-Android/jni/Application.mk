APP_BUILD_SCRIPT := $(call my-dir)/toolchain/Android.mk
APP_MODULES := anyrtmp-jni
#APP_ABI := armeabi armeabi-v7a arm64-v8a x86

NDK_PATH := /cygdrive/c/Android/NDK/android-ndk-r10e
NDK_STL_INC := $(NDK_PATH)/sources/cxx-stl/gnu-libstdc++/4.9/include

APP_OPTIM        := release 
APP_CFLAGS       += -O3
#APP_STL := gnustl_shared
APP_STL := gnustl_static
NDK_TOOLCHAIN_VERSION = 4.9

APP_PLATFORM := android-14
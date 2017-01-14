# This is the Android makefile for libyuv for both platform and NDK.
LOCAL_PATH:= $(call my-dir)

include $(CLEAR_VARS)

LOCAL_CPP_EXTENSION := .cc

LOCAL_SRC_FILES := \
	common_types.cc
	
LOCAL_SRC_FILES += api/androidvideocapturer.cc

LOCAL_SRC_FILES += \
		base/asyncinvoker.cc \
		base/asyncfile.cc \
		base/asyncresolverinterface.cc \
		base/asyncsocket.cc \
		base/asyncpacketsocket.cc \
		base/asynctcpsocket.cc \
		base/asyncudpsocket.cc \
		base/base64.cc \
		base/bitbuffer.cc \
		base/bytebuffer.cc \
		base/checks.cc \
		base/criticalsection.cc \
		base/event.cc \
		base/event_tracer.cc \
		base/ifaddrs-android.cc \
		base/ipaddress.cc \
		base/logging.cc \
		base/location.cc \
		base/messagehandler.cc \
		base/messagequeue.cc \
		base/nullsocketserver.cc \
		base/nethelpers.cc \
		base/physicalsocketserver.cc \
		base/platform_thread.cc \
		base/sharedexclusivelock.cc \
		base/signalthread.cc \
		base/sigslot.cc \
		base/socketaddress.cc \
		base/stringencode.cc \
		base/thread.cc \
		base/timeutils.cc \
		base/timing.cc \
		base/timestampaligner.cc
	
LOCAL_SRC_FILES += \
		common_audio/resampler/push_resampler.cc \
		common_audio/resampler/push_sinc_resampler.cc \
		common_audio/resampler/resampler.cc \
		common_audio/resampler/sinc_resampler.cc \
		common_audio/signal_processing/spl_init.c \
		common_audio/signal_processing/cross_correlation.c \
		common_audio/signal_processing/downsample_fast.c \
		common_audio/signal_processing/min_max_operations.c \
		common_audio/signal_processing/vector_scaling_operations.c \
		common_audio/audio_util.cc \
		common_audio/ring_buffer.c
ifeq ($(TARGET_ARCH_ABI),x86)	
LOCAL_SRC_FILES += common_audio/resampler/sinc_resampler_sse.cc
endif

LOCAL_SRC_FILES += \
		common_video/h264/h264_common.cc \
		common_video/h264/pps_parser.cc \
		common_video/h264/sps_parser.cc \
		common_video/h264/sps_vui_rewriter.cc \
		common_video/libyuv/webrtc_libyuv.cc \
		common_video/i420_buffer_pool.cc \
		common_video/video_frame.cc \
		common_video/incoming_video_stream.cc \
		common_video/video_frame_buffer.cc \
		common_video/video_render_frames.cc
	
LOCAL_SRC_FILES += \
		media/base/mediaconstants.cc \
		media/base/videoadapter.cc \
		media/base/videobroadcaster.cc \
		media/base/videocapturer.cc \
		media/base/videocommon.cc \
		media/base/videoframe.cc \
		media/base/videoframefactory.cc \
		media/base/videosourcebase.cc \
		media/engine/webrtcvideoframe.cc \
		media/engine/webrtcvideoframefactory.cc
	
LOCAL_SRC_FILES += \
		modules/audio_coding/acm2/acm_resampler.cc \
		modules/audio_device/audio_device_impl.cc \
		modules/audio_device/audio_device_buffer.cc \
		modules/audio_device/audio_device_generic.cc \
		modules/audio_device/android/audio_manager.cc \
		modules/audio_device/android/audio_record_jni.cc \
		modules/audio_device/android/audio_track_jni.cc \
		modules/audio_device/android/build_info.cc \
		modules/audio_device/android/opensles_common.cc \
		modules/audio_device/android/opensles_player.cc \
		modules/audio_device/dummy/audio_device_dummy.cc \
		modules/audio_device/fine_audio_buffer.cc \
		modules/utility/source/helpers_android.cc \
		modules/utility/source/jvm_android.cc \
		modules/video_coding/codecs/h264/h264.cc \
		modules/video_coding/codecs/h264/h264_encoder_impl.cc \
		modules/video_coding/codecs/h264/h264_decoder_impl.cc \
		modules/video_coding/utility/h264_bitstream_parser.cc \
		modules/video_coding/utility/quality_scaler.cc
	
LOCAL_SRC_FILES += \
		system_wrappers/source/aligned_malloc.cc \
		system_wrappers/source/atomic32_non_darwin_unix.cc \
        system_wrappers/source/clock.cc \
        system_wrappers/source/cpu_info.cc \
        system_wrappers/source/cpu_features.cc \
        system_wrappers/source/data_log_c.cc \
        system_wrappers/source/event.cc \
        system_wrappers/source/event_timer_posix.cc \
        system_wrappers/source/file_impl.cc \
        system_wrappers/source/logging.cc \
        system_wrappers/source/rtp_to_ntp.cc \
        system_wrappers/source/rw_lock.cc \
        system_wrappers/source/rw_lock_posix.cc \
        system_wrappers/source/sleep.cc \
        system_wrappers/source/sort.cc \
        system_wrappers/source/timestamp_extrapolator.cc \
        system_wrappers/source/trace_impl.cc \
        system_wrappers/source/trace_impl.h \
        system_wrappers/source/trace_posix.cc \
		system_wrappers/source/field_trial_default.cc \
		system_wrappers/source/metrics_default.cc

LOCAL_EXPORT_C_INCLUDES := $(LOCAL_PATH)/include
LOCAL_C_INCLUDES += $(LOCAL_PATH)/../ \
		$(LOCAL_PATH)/common_video/include \
		$(LOCAL_PATH)/common_video/libyuv/include \
		$(LOCAL_PATH)/../third_party/libyuv/include \
		$(LOCAL_PATH)/../third_party/ffmpeg

LOCAL_MODULE := libwebrtc
LOCAL_MODULE_TAGS := optional

LOCAL_CFLAGS := -std=gnu++11 -frtti -D__UCLIBC__ -DWEBRTC_POSIX -DWEBRTC_LINUX -DWEBRTC_ANDROID -D__STDC_FORMAT_MACROS -D__STDC_CONSTANT_MACROS -D__STDC_LIMIT_MACROS 
LOCAL_CFLAGS += -DWEBRTC_THREAD_RR -DWEBRTC_INCLUDE_INTERNAL_AUDIO_DEVICE -DWEBRTC_USE_H264 -DWEBRTC_INITIALIZE_FFMPEG -DNO_STL

include $(BUILD_STATIC_LIBRARY)


//
//  ARRtcEngine SDK
//
//  Copyright (c) 2019 AR.io. All rights reserved.
//

/**
 @defgroup createARRtcEngine Create an ARRtcEngine
 */

#ifndef __I_AR_RTC_ENGINE_H__
#define __I_AR_RTC_ENGINE_H__
#include "ArBase.h"
#include "IArService.h"

#if defined(_WIN32)
#include "IArMediaEngine.h"
#endif

#ifndef AR
#define AR ar::rtc
#endif
#ifndef AU
#define AU ar::util
#endif

namespace ar {
namespace rtc {
    typedef const char* uid_t;
    typedef void* view_t;
/** Maximum length of the device ID.
*/
enum MAX_DEVICE_ID_LENGTH_TYPE
{
  /** The maximum length of the device ID is 512 bytes.
  */
    MAX_DEVICE_ID_LENGTH = 512
};
/** Maximum length of user account.
 */
enum MAX_USER_ACCOUNT_LENGTH_TYPE
{
  /** The maximum length of user account is 255 bytes.
   */
  MAX_USER_ACCOUNT_LENGTH = 256
};
/** Maximum length of channel ID.
 */
enum MAX_CHANNEL_ID_LENGTH_TYPE
{
    /** The maximum length of channel id is 64 bytes.
     */
    MAX_CHANNEL_ID_LENGTH = 65
};
/** Formats of the quality report.
*/
enum QUALITY_REPORT_FORMAT_TYPE
{
  /** 0: The quality report in JSON format,
  */
    QUALITY_REPORT_JSON = 0,
    /** 1: The quality report in HTML format.
    */
    QUALITY_REPORT_HTML = 1,
};

enum MEDIA_ENGINE_EVENT_CODE_TYPE
{
    /** 0: For internal use only.
     */
    MEDIA_ENGINE_RECORDING_ERROR = 0,
    /** 1: For internal use only.
     */
    MEDIA_ENGINE_PLAYOUT_ERROR = 1,
    /** 2: For internal use only.
     */
    MEDIA_ENGINE_RECORDING_WARNING = 2,
    /** 3: For internal use only.
     */
    MEDIA_ENGINE_PLAYOUT_WARNING = 3,
    /** 10: For internal use only.
     */
    MEDIA_ENGINE_AUDIO_FILE_MIX_FINISH = 10,
    /** 12: For internal use only.
     */
    MEDIA_ENGINE_AUDIO_FAREND_MUSIC_BEGINS = 12,
    /** 13: For internal use only.
     */
    MEDIA_ENGINE_AUDIO_FAREND_MUSIC_ENDS = 13,
    /** 14: For internal use only.
     */
    MEDIA_ENGINE_LOCAL_AUDIO_RECORD_ENABLED = 14,
    /** 15: For internal use only.
     */
    MEDIA_ENGINE_LOCAL_AUDIO_RECORD_DISABLED = 15,
    // media engine role changed
    /** 20: For internal use only.
     */
    MEDIA_ENGINE_ROLE_BROADCASTER_SOLO = 20,
    /** 21: For internal use only.
     */
    MEDIA_ENGINE_ROLE_BROADCASTER_INTERACTIVE = 21,
    /** 22: For internal use only.
     */
    MEDIA_ENGINE_ROLE_AUDIENCE = 22,
    /** 23: For internal use only.
     */
    MEDIA_ENGINE_ROLE_COMM_PEER = 23,
    /** 24: For internal use only.
     */
    MEDIA_ENGINE_ROLE_GAME_PEER = 24,
    // iOS adm sample rate changed
    /** 110: For internal use only.
     */
    MEDIA_ENGINE_AUDIO_ADM_REQUIRE_RESTART = 110,
    /** 111: For internal use only.
     */
    MEDIA_ENGINE_AUDIO_ADM_SPECIAL_RESTART = 111,
    /** 112: For internal use only.
     */
    MEDIA_ENGINE_AUDIO_ADM_USING_COMM_PARAMS = 112,
    /** 113: For internal use only.
     */
    MEDIA_ENGINE_AUDIO_ADM_USING_NORM_PARAMS = 113,
    // audio mix state
    /** 710: For internal use only.
     */
    MEDIA_ENGINE_AUDIO_EVENT_MIXING_PLAY = 710,
    /** 711: For internal use only.
     */
    MEDIA_ENGINE_AUDIO_EVENT_MIXING_PAUSED = 711,
    /** 712: For internal use only.
     */
    MEDIA_ENGINE_AUDIO_EVENT_MIXING_RESTART         = 712,
    /** 713: For internal use only.
     */
    MEDIA_ENGINE_AUDIO_EVENT_MIXING_STOPPED = 713,
    /** 714: For internal use only.
     */
    MEDIA_ENGINE_AUDIO_EVENT_MIXING_ERROR = 714,
    //Mixing error codes
    /** 701: For internal use only.
     */
    MEDIA_ENGINE_AUDIO_ERROR_MIXING_OPEN = 701,
    /** 702: For internal use only.
     */
    MEDIA_ENGINE_AUDIO_ERROR_MIXING_TOO_FREQUENT = 702,
    /** 703: The audio mixing file playback is interrupted. For internal use only.
     */
    MEDIA_ENGINE_AUDIO_ERROR_MIXING_INTERRUPTED_EOF = 703,
    /** 0: For internal use only.
     */
    MEDIA_ENGINE_AUDIO_ERROR_MIXING_NO_ERROR = 0,
};

/** The states of the local user's audio mixing file.
*/
enum AUDIO_MIXING_STATE_TYPE{
    /** 710: The audio mixing file is playing.
    */
    AUDIO_MIXING_STATE_PLAYING = 710,
    /** 711: The audio mixing file pauses playing.
    */
    AUDIO_MIXING_STATE_PAUSED = 711,
    /** 713: The audio mixing file stops playing.
    */
    AUDIO_MIXING_STATE_STOPPED = 713,
    /** 714: An exception occurs when playing the audio mixing file. See #AUDIO_MIXING_ERROR_TYPE.
    */
    AUDIO_MIXING_STATE_FAILED = 714,
};

/** The error codes of the local user's audio mixing file.
*/
enum AUDIO_MIXING_ERROR_TYPE{
    /** 701: The SDK cannot open the audio mixing file.
    */
    AUDIO_MIXING_ERROR_CAN_NOT_OPEN = 701,
    /** 702: The SDK opens the audio mixing file too frequently.
    */
    AUDIO_MIXING_ERROR_TOO_FREQUENT_CALL = 702,
    /** 703: The audio mixing file playback is interrupted.
     */
    AUDIO_MIXING_ERROR_INTERRUPTED_EOF = 703,
    /** 0: The SDK can open the audio mixing file.
    */
    AUDIO_MIXING_ERROR_OK = 0,
};

/** Media device states.
 */
enum MEDIA_DEVICE_STATE_TYPE
{
  /** 1: The device is active.
  */
    MEDIA_DEVICE_STATE_ACTIVE = 1,
    /** 2: The device is disabled.
    */
    MEDIA_DEVICE_STATE_DISABLED = 2,
    /** 4: The device is not present.
    */
    MEDIA_DEVICE_STATE_NOT_PRESENT = 4,
    /** 8: The device is unplugged.
    */
    MEDIA_DEVICE_STATE_UNPLUGGED = 8
};

/** Media device types.
 */
enum MEDIA_DEVICE_TYPE
{
  /** -1: Unknown device type.
  */
    UNKNOWN_AUDIO_DEVICE = -1,
    /** 0: Audio playback device.
    */
    AUDIO_PLAYOUT_DEVICE = 0,
    /** 1: Audio recording device.
    */
    AUDIO_RECORDING_DEVICE = 1,
    /** 2: Video renderer.
    */
    VIDEO_RENDER_DEVICE = 2,
    /** 3: Video capturer.
    */
    VIDEO_CAPTURE_DEVICE = 3,
    /** 4: Application audio playback device.
    */
    AUDIO_APPLICATION_PLAYOUT_DEVICE = 4,
};

/** Local video state types
 */
enum LOCAL_VIDEO_STREAM_STATE
{
    /** 0: Initial state */
    LOCAL_VIDEO_STREAM_STATE_STOPPED = 0,
    /** 1: The local video capturing device starts successfully.
     *
     * The SDK also reports this state when you share a maximized window by calling \ref IRtcEngine::startScreenCaptureByWindowId "startScreenCaptureByWindowId".
     */
    LOCAL_VIDEO_STREAM_STATE_CAPTURING = 1,
    /** 2: The first video frame is successfully encoded. */
    LOCAL_VIDEO_STREAM_STATE_ENCODING = 2,
    /** 3: The local video fails to start. */
    LOCAL_VIDEO_STREAM_STATE_FAILED = 3
};

/** Local video state error codes
 */
enum LOCAL_VIDEO_STREAM_ERROR {
    /** 0: The local video is normal. */
    LOCAL_VIDEO_STREAM_ERROR_OK = 0,
    /** 1: No specified reason for the local video failure. */
    LOCAL_VIDEO_STREAM_ERROR_FAILURE = 1,
    /** 2: No permission to use the local video capturing device. */
    LOCAL_VIDEO_STREAM_ERROR_DEVICE_NO_PERMISSION = 2,
    /** 3: The local video capturing device is in use. */
    LOCAL_VIDEO_STREAM_ERROR_DEVICE_BUSY = 3,
    /** 4: The local video capture fails. Check whether the capturing device is working properly. */
    LOCAL_VIDEO_STREAM_ERROR_CAPTURE_FAILURE = 4,
    /** 5: The local video encoding fails. */
    LOCAL_VIDEO_STREAM_ERROR_ENCODE_FAILURE = 5,
    /** 11: The shared window is minimized when you call \ref IRtcEngine::startScreenCaptureByWindowId "startScreenCaptureByWindowId" to share a window.
     */
    LOCAL_VIDEO_STREAM_ERROR_SCREEN_CAPTURE_WINDOW_MINIMIZED = 11,
    /** 12: The error code indicates that a window shared by the window ID has been closed, or a full-screen window
     * shared by the window ID has exited full-screen mode.
     * After exiting full-screen mode, remote users cannot see the shared window. To prevent remote users from seeing a
     * black screen, AR recommends that you immediately stop screen sharing.
     *
     * Common scenarios for reporting this error code:
     * - When the local user closes the shared window, the SDK reports this error code.
     * - The local user shows some slides in full-screen mode first, and then shares the windows of the slides. After
     * the user exits full-screen mode, the SDK reports this error code.
     * - The local user watches web video or reads web document in full-screen mode first, and then shares the window of
     * the web video or document. After the user exits full-screen mode, the SDK reports this error code.
     */
    LOCAL_VIDEO_STREAM_ERROR_SCREEN_CAPTURE_WINDOW_CLOSED = 12,
};

/** Local audio state types.
 */
enum LOCAL_AUDIO_STREAM_STATE
{
    /** 0: The local audio is in the initial state.
     */
    LOCAL_AUDIO_STREAM_STATE_STOPPED = 0,
    /** 1: The recording device starts successfully.
     */
    LOCAL_AUDIO_STREAM_STATE_RECORDING = 1,
    /** 2: The first audio frame encodes successfully.
     */
    LOCAL_AUDIO_STREAM_STATE_ENCODING = 2,
    /** 3: The local audio fails to start.
     */
    LOCAL_AUDIO_STREAM_STATE_FAILED = 3
};

/** Local audio state error codes.
 */
enum LOCAL_AUDIO_STREAM_ERROR
{
    /** 0: The local audio is normal.
     */
    LOCAL_AUDIO_STREAM_ERROR_OK = 0,
    /** 1: No specified reason for the local audio failure.
     */
    LOCAL_AUDIO_STREAM_ERROR_FAILURE = 1,
    /** 2: No permission to use the local audio device.
     */
    LOCAL_AUDIO_STREAM_ERROR_DEVICE_NO_PERMISSION = 2,
    /** 3: The microphone is in use.
     */
    LOCAL_AUDIO_STREAM_ERROR_DEVICE_BUSY = 3,
    /** 4: The local audio recording fails. Check whether the recording device
     * is working properly.
     */
    LOCAL_AUDIO_STREAM_ERROR_RECORD_FAILURE = 4,
    /** 5: The local audio encoding fails.
     */
    LOCAL_AUDIO_STREAM_ERROR_ENCODE_FAILURE = 5
};

/** Audio recording qualities.
*/
enum AUDIO_RECORDING_QUALITY_TYPE
{
    /** 0: Low quality. The sample rate is 32 kHz, and the file size is around
     * 1.2 MB after 10 minutes of recording.
    */
    AUDIO_RECORDING_QUALITY_LOW = 0,
    /** 1: Medium quality. The sample rate is 32 kHz, and the file size is
     * around 2 MB after 10 minutes of recording.
    */
    AUDIO_RECORDING_QUALITY_MEDIUM = 1,
    /** 2: High quality. The sample rate is 32 kHz, and the file size is
     * around 3.75 MB after 10 minutes of recording.
    */
    AUDIO_RECORDING_QUALITY_HIGH = 2,
};

/** Network quality types. */
enum QUALITY_TYPE
{
    /** 0: The network quality is unknown. */
    QUALITY_UNKNOWN = 0,
    /**  1: The network quality is excellent. */
    QUALITY_EXCELLENT = 1,
    /** 2: The network quality is quite good, but the bitrate may be slightly lower than excellent. */
    QUALITY_GOOD = 2,
    /** 3: Users can feel the communication slightly impaired. */
    QUALITY_POOR = 3,
    /** 4: Users cannot communicate smoothly. */
    QUALITY_BAD = 4,
    /** 5: The network is so bad that users can barely communicate. */
    QUALITY_VBAD = 5,
    /** 6: The network is down and users cannot communicate at all. */
    QUALITY_DOWN = 6,
    /** 7: Users cannot detect the network quality. (Not in use.) */
    QUALITY_UNSUPPORTED = 7,
    /** 8: Detecting the network quality. */
    QUALITY_DETECTING = 8,
};

/** Video display modes. */
enum RENDER_MODE_TYPE
{
  /**
1: Uniformly scale the video until it fills the visible boundaries (cropped). One dimension of the video may have clipped contents.
 */
    RENDER_MODE_HIDDEN = 1,
    /**
2: Uniformly scale the video until one of its dimension fits the boundary (zoomed to fit). Areas that are not filled due to disparity in the aspect ratio are filled with black.
 */
    RENDER_MODE_FIT = 2,
    /** **DEPRECATED** 3: This mode is deprecated.
     */
    RENDER_MODE_ADAPTIVE = 3,
    /**
    4: The fill mode. In this mode, the SDK stretches or zooms the video to fill the display window.
    */
    RENDER_MODE_FILL = 4,
};

/** Video mirror modes. */
enum VIDEO_MIRROR_MODE_TYPE
{
      /** 0: (Default) The SDK enables the mirror mode. 
       */
    VIDEO_MIRROR_MODE_AUTO = 0,//determined by SDK
        /** 1: Enable mirror mode. */
    VIDEO_MIRROR_MODE_ENABLED = 1,//enabled mirror
        /** 2: Disable mirror mode. */
    VIDEO_MIRROR_MODE_DISABLED = 2,//disable mirror
};

/** **DEPRECATED** Video profiles. */
enum VIDEO_PROFILE_TYPE
{
    /** 0: 160 * 120, frame rate 15 fps, bitrate 65 Kbps. */
    VIDEO_PROFILE_LANDSCAPE_120P = 0,
    /** 2: 120 * 120, frame rate 15 fps, bitrate 50 Kbps. */
    VIDEO_PROFILE_LANDSCAPE_120P_3 = 2,
    /** 10: 320*180, frame rate 15 fps, bitrate 140 Kbps. */
    VIDEO_PROFILE_LANDSCAPE_180P = 10,
    /** 12: 180 * 180, frame rate 15 fps, bitrate 100 Kbps. */
    VIDEO_PROFILE_LANDSCAPE_180P_3 = 12,
    /** 13: 240 * 180, frame rate 15 fps, bitrate 120 Kbps. */
    VIDEO_PROFILE_LANDSCAPE_180P_4 = 13,
    /** 20: 320 * 240, frame rate 15 fps, bitrate 200 Kbps. */
    VIDEO_PROFILE_LANDSCAPE_240P = 20,
    /** 22: 240 * 240, frame rate 15 fps, bitrate 140 Kbps. */
    VIDEO_PROFILE_LANDSCAPE_240P_3 = 22,
    /** 23: 424 * 240, frame rate 15 fps, bitrate 220 Kbps. */
    VIDEO_PROFILE_LANDSCAPE_240P_4 = 23,
    /** 30: 640 * 360, frame rate 15 fps, bitrate 400 Kbps. */
    VIDEO_PROFILE_LANDSCAPE_360P = 30,
    /** 32: 360 * 360, frame rate 15 fps, bitrate 260 Kbps. */
    VIDEO_PROFILE_LANDSCAPE_360P_3 = 32,
    /** 33: 640 * 360, frame rate 30 fps, bitrate 600 Kbps. */
    VIDEO_PROFILE_LANDSCAPE_360P_4 = 33,
    /** 35: 360 * 360, frame rate 30 fps, bitrate 400 Kbps. */
    VIDEO_PROFILE_LANDSCAPE_360P_6 = 35,
    /** 36: 480 * 360, frame rate 15 fps, bitrate 320 Kbps. */
    VIDEO_PROFILE_LANDSCAPE_360P_7 = 36,
    /** 37: 480 * 360, frame rate 30 fps, bitrate 490 Kbps. */
    VIDEO_PROFILE_LANDSCAPE_360P_8 = 37,
    /** 38: 640 * 360, frame rate 15 fps, bitrate 800 Kbps.
     @note `LIVE_BROADCASTING` profile only.
     */
    VIDEO_PROFILE_LANDSCAPE_360P_9 = 38,
    /** 39: 640 * 360, frame rate 24 fps, bitrate 800 Kbps.
     @note `LIVE_BROADCASTING` profile only.
     */
    VIDEO_PROFILE_LANDSCAPE_360P_10 = 39,
    /** 100: 640 * 360, frame rate 24 fps, bitrate 1000 Kbps.
     @note `LIVE_BROADCASTING` profile only.
     */
    VIDEO_PROFILE_LANDSCAPE_360P_11 = 100,
    /** 40: 640 * 480, frame rate 15 fps, bitrate 500 Kbps. */
    VIDEO_PROFILE_LANDSCAPE_480P = 40,
    /** 42: 480 * 480, frame rate 15 fps, bitrate 400 Kbps. */
    VIDEO_PROFILE_LANDSCAPE_480P_3 = 42,
    /** 43: 640 * 480, frame rate 30 fps, bitrate 750 Kbps. */
    VIDEO_PROFILE_LANDSCAPE_480P_4 = 43,
    /** 45: 480 * 480, frame rate 30 fps, bitrate 600 Kbps. */
    VIDEO_PROFILE_LANDSCAPE_480P_6 = 45,
    /** 47: 848 * 480, frame rate 15 fps, bitrate 610 Kbps. */
    VIDEO_PROFILE_LANDSCAPE_480P_8 = 47,
    /** 48: 848 * 480, frame rate 30 fps, bitrate 930 Kbps. */
    VIDEO_PROFILE_LANDSCAPE_480P_9 = 48,
    /** 49: 640 * 480, frame rate 10 fps, bitrate 400 Kbps. */
    VIDEO_PROFILE_LANDSCAPE_480P_10 = 49,
    /** 50: 1280 * 720, frame rate 15 fps, bitrate 1130 Kbps. */
    VIDEO_PROFILE_LANDSCAPE_720P = 50,
    /** 52: 1280 * 720, frame rate 30 fps, bitrate 1710 Kbps. */
    VIDEO_PROFILE_LANDSCAPE_720P_3 = 52,
    /** 54: 960 * 720, frame rate 15 fps, bitrate 910 Kbps. */
    VIDEO_PROFILE_LANDSCAPE_720P_5 = 54,
    /** 55: 960 * 720, frame rate 30 fps, bitrate 1380 Kbps. */
    VIDEO_PROFILE_LANDSCAPE_720P_6 = 55,
    /** 60: 1920 * 1080, frame rate 15 fps, bitrate 2080 Kbps. */
    VIDEO_PROFILE_LANDSCAPE_1080P = 60,
    /** 62: 1920 * 1080, frame rate 30 fps, bitrate 3150 Kbps. */
    VIDEO_PROFILE_LANDSCAPE_1080P_3 = 62,
    /** 64: 1920 * 1080, frame rate 60 fps, bitrate 4780 Kbps. */
    VIDEO_PROFILE_LANDSCAPE_1080P_5 = 64,
    /** 66: 2560 * 1440, frame rate 30 fps, bitrate 4850 Kbps. */
    VIDEO_PROFILE_LANDSCAPE_1440P = 66,
    /** 67: 2560 * 1440, frame rate 60 fps, bitrate 6500 Kbps. */
    VIDEO_PROFILE_LANDSCAPE_1440P_2 = 67,
    /** 70: 3840 * 2160, frame rate 30 fps, bitrate 6500 Kbps. */
    VIDEO_PROFILE_LANDSCAPE_4K = 70,
    /** 72: 3840 * 2160, frame rate 60 fps, bitrate 6500 Kbps. */
    VIDEO_PROFILE_LANDSCAPE_4K_3 = 72,
    /** 1000: 120 * 160, frame rate 15 fps, bitrate 65 Kbps. */
    VIDEO_PROFILE_PORTRAIT_120P = 1000,
    /** 1002: 120 * 120, frame rate 15 fps, bitrate 50 Kbps. */
    VIDEO_PROFILE_PORTRAIT_120P_3 = 1002,
    /** 1010: 180 * 320, frame rate 15 fps, bitrate 140 Kbps. */
    VIDEO_PROFILE_PORTRAIT_180P = 1010,
    /** 1012: 180 * 180, frame rate 15 fps, bitrate 100 Kbps. */
    VIDEO_PROFILE_PORTRAIT_180P_3 = 1012,
    /** 1013: 180 * 240, frame rate 15 fps, bitrate 120 Kbps. */
    VIDEO_PROFILE_PORTRAIT_180P_4 = 1013,
    /** 1020: 240 * 320, frame rate 15 fps, bitrate 200 Kbps. */
    VIDEO_PROFILE_PORTRAIT_240P = 1020,
    /** 1022: 240 * 240, frame rate 15 fps, bitrate 140 Kbps. */
    VIDEO_PROFILE_PORTRAIT_240P_3 = 1022,
    /** 1023: 240 * 424, frame rate 15 fps, bitrate 220 Kbps. */
    VIDEO_PROFILE_PORTRAIT_240P_4 = 1023,
    /** 1030: 360 * 640, frame rate 15 fps, bitrate 400 Kbps. */
    VIDEO_PROFILE_PORTRAIT_360P = 1030,
    /** 1032: 360 * 360, frame rate 15 fps, bitrate 260 Kbps. */
    VIDEO_PROFILE_PORTRAIT_360P_3 = 1032,
    /** 1033: 360 * 640, frame rate 30 fps, bitrate 600 Kbps. */
    VIDEO_PROFILE_PORTRAIT_360P_4 = 1033,
    /** 1035: 360 * 360, frame rate 30 fps, bitrate 400 Kbps. */
    VIDEO_PROFILE_PORTRAIT_360P_6 = 1035,
    /** 1036: 360 * 480, frame rate 15 fps, bitrate 320 Kbps. */
    VIDEO_PROFILE_PORTRAIT_360P_7 = 1036,
    /** 1037: 360 * 480, frame rate 30 fps, bitrate 490 Kbps. */
    VIDEO_PROFILE_PORTRAIT_360P_8 = 1037,
    /** 1038: 360 * 640, frame rate 15 fps, bitrate 800 Kbps.
     @note `LIVE_BROADCASTING` profile only.
     */
    VIDEO_PROFILE_PORTRAIT_360P_9 = 1038,
    /** 1039: 360 * 640, frame rate 24 fps, bitrate 800 Kbps.
     @note `LIVE_BROADCASTING` profile only.
     */
    VIDEO_PROFILE_PORTRAIT_360P_10 = 1039,
    /** 1100: 360 * 640, frame rate 24 fps, bitrate 1000 Kbps.
     @note `LIVE_BROADCASTING` profile only.
     */
    VIDEO_PROFILE_PORTRAIT_360P_11 = 1100,
    /** 1040: 480 * 640, frame rate 15 fps, bitrate 500 Kbps. */
    VIDEO_PROFILE_PORTRAIT_480P = 1040,
    /** 1042: 480 * 480, frame rate 15 fps, bitrate 400 Kbps. */
    VIDEO_PROFILE_PORTRAIT_480P_3 = 1042,
    /** 1043: 480 * 640, frame rate 30 fps, bitrate 750 Kbps. */
    VIDEO_PROFILE_PORTRAIT_480P_4 = 1043,
    /** 1045: 480 * 480, frame rate 30 fps, bitrate 600 Kbps. */
    VIDEO_PROFILE_PORTRAIT_480P_6 = 1045,
    /** 1047: 480 * 848, frame rate 15 fps, bitrate 610 Kbps. */
    VIDEO_PROFILE_PORTRAIT_480P_8 = 1047,
    /** 1048: 480 * 848, frame rate 30 fps, bitrate 930 Kbps. */
    VIDEO_PROFILE_PORTRAIT_480P_9 = 1048,
    /** 1049: 480 * 640, frame rate 10 fps, bitrate 400 Kbps. */
    VIDEO_PROFILE_PORTRAIT_480P_10 = 1049,
    /** 1050: 720 * 1280, frame rate 15 fps, bitrate 1130 Kbps. */
    VIDEO_PROFILE_PORTRAIT_720P = 1050,
    /** 1052: 720 * 1280, frame rate 30 fps, bitrate 1710 Kbps. */
    VIDEO_PROFILE_PORTRAIT_720P_3 = 1052,
    /** 1054: 720 * 960, frame rate 15 fps, bitrate 910 Kbps. */
    VIDEO_PROFILE_PORTRAIT_720P_5 = 1054,
    /** 1055: 720 * 960, frame rate 30 fps, bitrate 1380 Kbps. */
    VIDEO_PROFILE_PORTRAIT_720P_6 = 1055,
    /** 1060: 1080 * 1920, frame rate 15 fps, bitrate 2080 Kbps. */
    VIDEO_PROFILE_PORTRAIT_1080P = 1060,
    /** 1062: 1080 * 1920, frame rate 30 fps, bitrate 3150 Kbps. */
    VIDEO_PROFILE_PORTRAIT_1080P_3 = 1062,
    /** 1064: 1080 * 1920, frame rate 60 fps, bitrate 4780 Kbps. */
    VIDEO_PROFILE_PORTRAIT_1080P_5 = 1064,
    /** 1066: 1440 * 2560, frame rate 30 fps, bitrate 4850 Kbps. */
    VIDEO_PROFILE_PORTRAIT_1440P = 1066,
    /** 1067: 1440 * 2560, frame rate 60 fps, bitrate 6500 Kbps. */
    VIDEO_PROFILE_PORTRAIT_1440P_2 = 1067,
    /** 1070: 2160 * 3840, frame rate 30 fps, bitrate 6500 Kbps. */
    VIDEO_PROFILE_PORTRAIT_4K = 1070,
    /** 1072: 2160 * 3840, frame rate 60 fps, bitrate 6500 Kbps. */
    VIDEO_PROFILE_PORTRAIT_4K_3 = 1072,
    /** Default 640 * 360, frame rate 15 fps, bitrate 400 Kbps. */
    VIDEO_PROFILE_DEFAULT = VIDEO_PROFILE_LANDSCAPE_360P,
};

/** Audio profiles.

Sets the sample rate, bitrate, encoding mode, and the number of channels:*/
enum AUDIO_PROFILE_TYPE // sample rate, bit rate, mono/stereo, speech/music codec
{
    /**
     0: Default audio profile:
     - For the interactive streaming profile: A sample rate of 48 KHz, music encoding, mono, and a bitrate of up to 64 Kbps.
     - For the `COMMUNICATION` profile:
        - Windows: A sample rate of 16 KHz, music encoding, mono, and a bitrate of up to 16 Kbps.
        - Android/macOS/iOS: A sample rate of 32 KHz, music encoding, mono, and a bitrate of up to 18 Kbps.
    */
    AUDIO_PROFILE_DEFAULT = 0, // use default settings
    /**
     1: A sample rate of 32 KHz, audio encoding, mono, and a bitrate of up to 18 Kbps.
     */
    AUDIO_PROFILE_SPEECH_STANDARD = 1, // 32Khz, 18Kbps, mono, speech
    /**
     2: A sample rate of 48 KHz, music encoding, mono, and a bitrate of up to 64 Kbps.
     */
    AUDIO_PROFILE_MUSIC_STANDARD = 2, // 48Khz, 48Kbps, mono, music
    /**
     3: A sample rate of 48 KHz, music encoding, stereo, and a bitrate of up to 80 Kbps.
     */
    AUDIO_PROFILE_MUSIC_STANDARD_STEREO = 3, // 48Khz, 56Kbps, stereo, music
    /**
     4: A sample rate of 48 KHz, music encoding, mono, and a bitrate of up to 96 Kbps.
     */
    AUDIO_PROFILE_MUSIC_HIGH_QUALITY = 4, // 48Khz, 128Kbps, mono, music
    /**
     5: A sample rate of 48 KHz, music encoding, stereo, and a bitrate of up to 128 Kbps.
     */
    AUDIO_PROFILE_MUSIC_HIGH_QUALITY_STEREO = 5, // 48Khz, 192Kbps, stereo, music
    /**
     6: A sample rate of 16 KHz, audio encoding, mono, and Acoustic Echo Cancellation (AES) enabled.
     */
    AUDIO_PROFILE_IOT                       = 6,
    AUDIO_PROFILE_NUM = 7,
};

/** Audio application scenarios.
*/
enum AUDIO_SCENARIO_TYPE // set a suitable scenario for your app type
{
    /** 0: Default audio scenario. */
    AUDIO_SCENARIO_DEFAULT = 0,
    /** 1: Entertainment scenario where users need to frequently switch the user role. */
    AUDIO_SCENARIO_CHATROOM_ENTERTAINMENT = 1,
    /** 2: Education scenario where users want smoothness and stability. */
    AUDIO_SCENARIO_EDUCATION = 2,
    /** 3: High-quality audio chatroom scenario where hosts mainly play music. */
    AUDIO_SCENARIO_GAME_STREAMING = 3,
    /** 4: Showroom scenario where a single host wants high-quality audio. */
    AUDIO_SCENARIO_SHOWROOM = 4,
    /** 5: Gaming scenario for group chat that only contains the human voice. */
    AUDIO_SCENARIO_CHATROOM_GAMING = 5,
    /** 6: IoT (Internet of Things) scenario where users use IoT devices with low power consumption. */
    AUDIO_SCENARIO_IOT = 6,
    /** 8: Meeting scenario that mainly contains the human voice.
     *
     * @since v3.2.0
     */
    AUDIO_SCENARIO_MEETING = 8,
    /** The number of elements in the enumeration.
     */
    AUDIO_SCENARIO_NUM = 9,
};

 /** The channel profile of the AR RtcEngine.
 */
enum CHANNEL_PROFILE_TYPE
{
   /** (Default) Communication. This profile applies to scenarios such as an audio call or video call,
    * where all users can publish and subscribe to streams.
    */
    CHANNEL_PROFILE_COMMUNICATION = 0,
   /** Live streaming. In this profile, uses have roles, namely, host and audience (default).
    * A host both publishes and subscribes to streams, while an audience subscribes to streams only.
    * This profile applies to scenarios such as a chat room or interactive video streaming.
    */
    CHANNEL_PROFILE_LIVE_BROADCASTING = 1,
   /** 2: Gaming. This profile uses a codec with a lower bitrate and consumes less power. Applies to the gaming scenario, where all game players can talk freely.
    *
    * @note AR does not recommend using this setting.
    */
    CHANNEL_PROFILE_GAME = 2,
};
/// @cond
/** The role of a user in a live interactive streaming. */
enum CLIENT_ROLE_TYPE
{
    /** 1: Host. A host can both send and receive streams. */
    CLIENT_ROLE_BROADCASTER = 1,
    /** 2: (Default) Audience. An `audience` member can only receive streams. */
    CLIENT_ROLE_AUDIENCE = 2,
};

/** The latency level of an audience member in a live interactive streaming.
 *
 * @note Takes effect only when the user role is `CLIENT_ROLE_BROADCASTER`.
 */
enum AUDIENCE_LATENCY_LEVEL_TYPE
{
    /** 1: Low latency. */
    AUDIENCE_LATENCY_LEVEL_LOW_LATENCY = 1,
    /** 2: (Default) Ultra low latency. */
    AUDIENCE_LATENCY_LEVEL_ULTRA_LOW_LATENCY = 2,
};
/// @cond
/** The reason why the super-resolution algorithm is not successfully enabled.
 */
enum SUPER_RESOLUTION_STATE_REASON
{
    /** 0: The super-resolution algorithm is successfully enabled.
     */
    SR_STATE_REASON_SUCCESS = 0,
    /** 1: The origin resolution of the remote video is beyond the range where
     * the super-resolution algorithm can be applied.
     */
    SR_STATE_REASON_STREAM_OVER_LIMITATION = 1,
    /** 2: Another user is already using the super-resolution algorithm.
     */
    SR_STATE_REASON_USER_COUNT_OVER_LIMITATION = 2,
    /** 3: The device does not support the super-resolution algorithm.
     */
    SR_STATE_REASON_DEVICE_NOT_SUPPORTED = 3,
};
/// @endcond

/** Reasons for a user being offline. */
enum USER_OFFLINE_REASON_TYPE
{
    /** 0: The user quits the call. */
    USER_OFFLINE_QUIT = 0,
    /** 1: The SDK times out and the user drops offline because no data packet is received within a certain period of time. If the user quits the call and the message is not passed to the SDK (due to an unreliable channel), the SDK assumes the user dropped offline. */
    USER_OFFLINE_DROPPED = 1,
      /** 2: (Live broadcast only.) The client role switched from the host to the audience. */
    USER_OFFLINE_BECOME_AUDIENCE = 2,
};
/**
 States of the RTMP streaming.
 */
enum RTMP_STREAM_PUBLISH_STATE
{
  /** The RTMP streaming has not started or has ended. This state is also triggered after you remove an RTMP address from the CDN by calling removePublishStreamUrl.
   */
  RTMP_STREAM_PUBLISH_STATE_IDLE = 0,
  /** The SDK is connecting to AR's streaming server and the RTMP server. This state is triggered after you call the \ref IRtcEngine::addPublishStreamUrl "addPublishStreamUrl" method.
   */
  RTMP_STREAM_PUBLISH_STATE_CONNECTING = 1,
  /** The RTMP streaming publishes. The SDK successfully publishes the RTMP streaming and returns this state.
   */
  RTMP_STREAM_PUBLISH_STATE_RUNNING = 2,
  /** The RTMP streaming is recovering. When exceptions occur to the CDN, or the streaming is interrupted, the SDK tries to resume RTMP streaming and returns this state.

   - If the SDK successfully resumes the streaming, #RTMP_STREAM_PUBLISH_STATE_RUNNING (2) returns.
   - If the streaming does not resume within 60 seconds or server errors occur, #RTMP_STREAM_PUBLISH_STATE_FAILURE (4) returns. You can also reconnect to the server by calling the \ref IRtcEngine::removePublishStreamUrl "removePublishStreamUrl" and \ref IRtcEngine::addPublishStreamUrl "addPublishStreamUrl" methods.
   */
  RTMP_STREAM_PUBLISH_STATE_RECOVERING = 3,
  /** The RTMP streaming fails. See the errCode parameter for the detailed error information. You can also call the \ref IRtcEngine::addPublishStreamUrl "addPublishStreamUrl" method to publish the RTMP streaming again.
   */
  RTMP_STREAM_PUBLISH_STATE_FAILURE = 4,
};

/**
 Error codes of the RTMP streaming.
 */
enum RTMP_STREAM_PUBLISH_ERROR
{
  /** The RTMP streaming publishes successfully. */
  RTMP_STREAM_PUBLISH_ERROR_OK = 0,
  /** Invalid argument used. If, for example, you do not call the \ref IRtcEngine::setLiveTranscoding "setLiveTranscoding" method to configure the LiveTranscoding parameters before calling the addPublishStreamUrl method, the SDK returns this error. Check whether you set the parameters in the *setLiveTranscoding* method properly. */
  RTMP_STREAM_PUBLISH_ERROR_INVALID_ARGUMENT = 1,
  /** The RTMP streaming is encrypted and cannot be published. */
  RTMP_STREAM_PUBLISH_ERROR_ENCRYPTED_STREAM_NOT_ALLOWED = 2,
  /** Timeout for the RTMP streaming. Call the \ref IRtcEngine::addPublishStreamUrl "addPublishStreamUrl" method to publish the streaming again. */
  RTMP_STREAM_PUBLISH_ERROR_CONNECTION_TIMEOUT = 3,
  /** An error occurs in AR's streaming server. Call the addPublishStreamUrl method to publish the streaming again. */
  RTMP_STREAM_PUBLISH_ERROR_INTERNAL_SERVER_ERROR = 4,
  /** An error occurs in the RTMP server. */
  RTMP_STREAM_PUBLISH_ERROR_RTMP_SERVER_ERROR = 5,
  /** The RTMP streaming publishes too frequently. */
  RTMP_STREAM_PUBLISH_ERROR_TOO_OFTEN = 6,
  /** The host publishes more than 10 URLs. Delete the unnecessary URLs before adding new ones. */
  RTMP_STREAM_PUBLISH_ERROR_REACH_LIMIT = 7,
  /** The host manipulates other hosts' URLs. Check your app logic. */
  RTMP_STREAM_PUBLISH_ERROR_NOT_AUTHORIZED = 8,
  /** AR's server fails to find the RTMP streaming. */
  RTMP_STREAM_PUBLISH_ERROR_STREAM_NOT_FOUND = 9,
  /** The format of the RTMP streaming URL is not supported. Check whether the URL format is correct. */
  RTMP_STREAM_PUBLISH_ERROR_FORMAT_NOT_SUPPORTED = 10,
  /** 推流中的背景图片或者水印地址无法拉取 **/
  RTMP_STREAMING_ERROR_FAILED_LOAD_IMAGE = 11,

};

/** Events during the RTMP streaming. */
enum RTMP_STREAMING_EVENT
{
  /** An error occurs when you add a background image or a watermark image to the RTMP stream.
   */
  RTMP_STREAMING_EVENT_FAILED_LOAD_IMAGE = 1,
};

/** States of importing an external video stream in the live interactive streaming. */
enum INJECT_STREAM_STATUS
{
    /** 0: The external video stream imported successfully. */
    INJECT_STREAM_STATUS_START_SUCCESS = 0,
    /** 1: The external video stream already exists. */
    INJECT_STREAM_STATUS_START_ALREADY_EXISTS = 1,
    /** 2: The external video stream to be imported is unauthorized. */
    INJECT_STREAM_STATUS_START_UNAUTHORIZED = 2,
    /** 3: Import external video stream timeout. */
    INJECT_STREAM_STATUS_START_TIMEDOUT = 3,
    /** 4: Import external video stream failed. */
    INJECT_STREAM_STATUS_START_FAILED = 4,
    /** 5: The external video stream stopped importing successfully. */
    INJECT_STREAM_STATUS_STOP_SUCCESS = 5,
    /** 6: No external video stream is found. */
    INJECT_STREAM_STATUS_STOP_NOT_FOUND = 6,
    /** 7: The external video stream to be stopped importing is unauthorized. */
    INJECT_STREAM_STATUS_STOP_UNAUTHORIZED = 7,
    /** 8: Stop importing external video stream timeout. */
    INJECT_STREAM_STATUS_STOP_TIMEDOUT = 8,
    /** 9: Stop importing external video stream failed. */
    INJECT_STREAM_STATUS_STOP_FAILED = 9,
    /** 10: The external video stream is corrupted. */
    INJECT_STREAM_STATUS_BROKEN = 10,
};
/** Remote video stream types. */
enum REMOTE_VIDEO_STREAM_TYPE
{
      /** 0: High-stream video. */
    REMOTE_VIDEO_STREAM_HIGH = 0,
      /** 1: Low-stream video. */
    REMOTE_VIDEO_STREAM_LOW = 1,
};

/** Use modes of the \ref media::IAudioFrameObserver::onRecordAudioFrame "onRecordAudioFrame" callback. */
enum RAW_AUDIO_FRAME_OP_MODE_TYPE
{
    /** 0: Read-only mode: Users only read the \ref AM::IAudioFrameObserver::AudioFrame "AudioFrame" data without modifying anything. For example, when users acquire the data with the AR SDK, then push the RTMP streams. */
    RAW_AUDIO_FRAME_OP_MODE_READ_ONLY = 0,
    /** 1: Write-only mode: Users replace the \ref AM::IAudioFrameObserver::AudioFrame "AudioFrame" data with their own data and pass the data to the SDK for encoding. For example, when users acquire the data. */
    RAW_AUDIO_FRAME_OP_MODE_WRITE_ONLY = 1,
    /** 2: Read and write mode: Users read the data from \ref AM::IAudioFrameObserver::AudioFrame "AudioFrame", modify it, and then play it. For example, when users have their own sound-effect processing module and perform some voice pre-processing, such as a voice change. */
    RAW_AUDIO_FRAME_OP_MODE_READ_WRITE = 2,
};

/** Audio-sample rates. */
enum AUDIO_SAMPLE_RATE_TYPE
{
    /** 32000: 32 kHz */
    AUDIO_SAMPLE_RATE_32000 = 32000,
    /** 44100: 44.1 kHz */
    AUDIO_SAMPLE_RATE_44100 = 44100,
      /** 48000: 48 kHz */
    AUDIO_SAMPLE_RATE_48000 = 48000,
};

/** Video codec profile types. */
enum VIDEO_CODEC_PROFILE_TYPE
{  /** 66: Baseline video codec profile. Generally used in video calls on mobile phones. */
    VIDEO_CODEC_PROFILE_BASELINE = 66,
    /** 77: Main video codec profile. Generally used in mainstream electronics such as MP4 players, portable video players, PSP, and iPads. */
    VIDEO_CODEC_PROFILE_MAIN = 77,
      /** 100: (Default) High video codec profile. Generally used in high-resolution broadcasts or television. */
    VIDEO_CODEC_PROFILE_HIGH = 100,
};

/** Video codec types */
enum VIDEO_CODEC_TYPE {
    /** Standard VP8 */
    VIDEO_CODEC_VP8 = 1,
    /** Standard H264 */
    VIDEO_CODEC_H264 = 2,
    /** Enhanced VP8 */
    VIDEO_CODEC_EVP = 3,
    /** Enhanced H264 */
    VIDEO_CODEC_E264 = 4,
	/** Standard Jpeg*/
	VIDEO_CODEC_MJPG = 5,
};

/**
 * Audio codec type list.
 */
enum AUDIO_CODEC_TYPE {
	/**
	 * 1: OPUS
	 */
	AUDIO_CODEC_OPUS = 1,
	/**
	 * 3: G711
	 */
	AUDIO_CODEC_G711A = 3,
	AUDIO_CODEC_G711U = 4,
	/**
	 * 5: G722
	 */
	AUDIO_CODEC_G722 = 5,
	/**
	 * 8: AACLC
	 */
	AUDIO_CODEC_AACLC = 8,
	/**
	 * 9: HEAAC
	 */
	AUDIO_CODEC_HEAAC = 9,
	/**
	 * 253: GENERIC
	 */
	AUDIO_CODEC_GENERIC = 253,
};

/** Video Codec types for publishing streams. */
enum VIDEO_CODEC_TYPE_FOR_STREAM
{
    VIDEO_CODEC_H264_FOR_STREAM = 1,
    VIDEO_CODEC_H265_FOR_STREAM = 2,
};

/** Audio equalization band frequencies. */
enum AUDIO_EQUALIZATION_BAND_FREQUENCY
{
    /** 0: 31 Hz */
    AUDIO_EQUALIZATION_BAND_31 = 0,
      /** 1: 62 Hz */
    AUDIO_EQUALIZATION_BAND_62 = 1,
    /** 2: 125 Hz */
    AUDIO_EQUALIZATION_BAND_125 = 2,
      /** 3: 250 Hz */
    AUDIO_EQUALIZATION_BAND_250 = 3,
      /** 4: 500 Hz */
    AUDIO_EQUALIZATION_BAND_500 = 4,
        /** 5: 1 kHz */
    AUDIO_EQUALIZATION_BAND_1K = 5,
        /** 6: 2 kHz */
    AUDIO_EQUALIZATION_BAND_2K = 6,
        /** 7: 4 kHz */
    AUDIO_EQUALIZATION_BAND_4K = 7,
        /** 8: 8 kHz */
    AUDIO_EQUALIZATION_BAND_8K = 8,
      /** 9: 16 kHz */
    AUDIO_EQUALIZATION_BAND_16K = 9,
};

/** Audio reverberation types. */
enum AUDIO_REVERB_TYPE
{
    /** 0: The level of the dry signal (db). The value is between -20 and 10. */
    AUDIO_REVERB_DRY_LEVEL = 0, // (dB, [-20,10]), the level of the dry signal
    /** 1: The level of the early reflection signal (wet signal) (dB). The value is between -20 and 10. */
    AUDIO_REVERB_WET_LEVEL = 1, // (dB, [-20,10]), the level of the early reflection signal (wet signal)
    /** 2: The room size of the reflection. The value is between 0 and 100. */
    AUDIO_REVERB_ROOM_SIZE = 2, // ([0,100]), the room size of the reflection
    /** 3: The length of the initial delay of the wet signal (ms). The value is between 0 and 200. */
    AUDIO_REVERB_WET_DELAY = 3, // (ms, [0,200]), the length of the initial delay of the wet signal in ms
    /** 4: The reverberation strength. The value is between 0 and 100. */
    AUDIO_REVERB_STRENGTH = 4, // ([0,100]), the strength of the reverberation
};

/**
 * Local voice changer options.
 */
enum VOICE_CHANGER_PRESET {
    /**
     * The original voice (no local voice change).
     */
    VOICE_CHANGER_OFF = 0x00000000, //Turn off the voice changer
    /**
     * The voice of an old man.
     */
    VOICE_CHANGER_OLDMAN = 0x00000001,
    /**
     * The voice of a little boy.
     */
    VOICE_CHANGER_BABYBOY = 0x00000002,
    /**
     * The voice of a little girl.
     */
    VOICE_CHANGER_BABYGIRL = 0x00000003,
    /**
     * The voice of Zhu Bajie, a character in Journey to the West who has a voice like that of a growling bear.
     */
    VOICE_CHANGER_ZHUBAJIE = 0x00000004,
    /**
     * The ethereal voice.
     */
    VOICE_CHANGER_ETHEREAL = 0x00000005,
    /**
     * The voice of Hulk.
     */
    VOICE_CHANGER_HULK = 0x00000006,
    /**
     * A more vigorous voice.
     */
    VOICE_BEAUTY_VIGOROUS = 0x00100001,//7,
    /**
     * A deeper voice.
     */
    VOICE_BEAUTY_DEEP = 0x00100002,
    /**
     * A mellower voice.
     */
    VOICE_BEAUTY_MELLOW = 0x00100003,
    /**
     * Falsetto.
     */
    VOICE_BEAUTY_FALSETTO = 0x00100004,
    /**
     * A fuller voice.
     */
    VOICE_BEAUTY_FULL = 0x00100005,
    /**
     * A clearer voice.
     */
    VOICE_BEAUTY_CLEAR = 0x00100006,
    /**
     * A more resounding voice.
     */
    VOICE_BEAUTY_RESOUNDING = 0x00100007,
    /**
     * A more ringing voice.
     */
    VOICE_BEAUTY_RINGING = 0x00100008,
    /**
     * A more spatially resonant voice.
     */
    VOICE_BEAUTY_SPACIAL = 0x00100009,
    /**
     * (For male only) A more magnetic voice. Do not use it when the speaker is a female; otherwise, voice distortion occurs.
     */
    GENERAL_BEAUTY_VOICE_MALE_MAGNETIC = 0x00200001,
    /**
     * (For female only) A fresher voice. Do not use it when the speaker is a male; otherwise, voice distortion occurs.
     */
    GENERAL_BEAUTY_VOICE_FEMALE_FRESH = 0x00200002,
    /**
     * 	(For female only) A more vital voice. Do not use it when the speaker is a male; otherwise, voice distortion occurs.
     */
    GENERAL_BEAUTY_VOICE_FEMALE_VITALITY = 0x00200003

};

/** Local voice reverberation presets. */
enum AUDIO_REVERB_PRESET {
    /**
     * Turn off local voice reverberation, that is, to use the original voice.
     */
    AUDIO_REVERB_OFF = 0x00000000, // Turn off audio reverb
    /**
     * The reverberation style typical of a KTV venue (enhanced).
     */
    AUDIO_REVERB_FX_KTV = 0x00100001,
    /**
     * The reverberation style typical of a concert hall (enhanced).
     */
    AUDIO_REVERB_FX_VOCAL_CONCERT = 0x00100002,
    /**
     * The reverberation style typical of an uncle's voice.
     */
    AUDIO_REVERB_FX_UNCLE = 0x00100003,
    /**
     * The reverberation style typical of a little sister's voice.
     */
    AUDIO_REVERB_FX_SISTER = 0x00100004,
    /**
     * The reverberation style typical of a recording studio (enhanced).
     */
    AUDIO_REVERB_FX_STUDIO = 0x00100005,
    /**
     * The reverberation style typical of popular music (enhanced).
     */
    AUDIO_REVERB_FX_POPULAR = 0x00100006,
    /**
     * The reverberation style typical of R&B music (enhanced).
     */
    AUDIO_REVERB_FX_RNB = 0x00100007,
    /**
     * The reverberation style typical of the vintage phonograph.
     */
    AUDIO_REVERB_FX_PHONOGRAPH = 0x00100008,
    /**
     * The reverberation style typical of popular music.
     */
    AUDIO_REVERB_POPULAR = 0x00000001,
    /**
     * The reverberation style typical of R&B music.
     */
    AUDIO_REVERB_RNB = 0x00000002,
    /**
     * The reverberation style typical of rock music.
     */
    AUDIO_REVERB_ROCK = 0x00000003,
    /**
     * The reverberation style typical of hip-hop music.
     */
     AUDIO_REVERB_HIPHOP = 0x00000004,
    /**
     * The reverberation style typical of a concert hall.
     */
    AUDIO_REVERB_VOCAL_CONCERT = 0x00000005,
    /**
     * The reverberation style typical of a KTV venue.
     */
    AUDIO_REVERB_KTV = 0x00000006,
    /**
     * The reverberation style typical of a recording studio.
     */
    AUDIO_REVERB_STUDIO = 0x00000007,
    /**
     * The reverberation of the virtual stereo. The virtual stereo is an effect that renders the monophonic
     * audio as the stereo audio, so that all users in the channel can hear the stereo voice effect.
     * To achieve better virtual stereo reverberation, AR recommends setting `profile` in `setAudioProfile`
     * as `AUDIO_PROFILE_MUSIC_HIGH_QUALITY_STEREO(5)`.
     */
    AUDIO_VIRTUAL_STEREO = 0x00200001,
    /** 1: Electronic Voice.*/
    AUDIO_ELECTRONIC_VOICE = 0x00300001,
    /** 1: 3D Voice.*/
    AUDIO_THREEDIM_VOICE = 0x00400001
};
/** The options for SDK preset voice beautifier effects.
 */
enum VOICE_BEAUTIFIER_PRESET
{
    /** Turn off voice beautifier effects and use the original voice.
     */
    VOICE_BEAUTIFIER_OFF = 0x00000000,
    /** A more magnetic voice.
     *
     * @note AR recommends using this enumerator to process a male-sounding voice; otherwise, you may experience vocal distortion.
     */
    CHAT_BEAUTIFIER_MAGNETIC = 0x01010100,
    /** A fresher voice.
     *
     * @note AR recommends using this enumerator to process a female-sounding voice; otherwise, you may experience vocal distortion.
     */
    CHAT_BEAUTIFIER_FRESH = 0x01010200,
    /** A more vital voice.
     *
     * @note AR recommends using this enumerator to process a female-sounding voice; otherwise, you may experience vocal distortion.
     */
    CHAT_BEAUTIFIER_VITALITY = 0x01010300,
    /** A more vigorous voice.
     */
    TIMBRE_TRANSFORMATION_VIGOROUS = 0x01030100,
    /** A deeper voice.
     */
    TIMBRE_TRANSFORMATION_DEEP = 0x01030200,
    /** A mellower voice.
     */
    TIMBRE_TRANSFORMATION_MELLOW = 0x01030300,
    /** A falsetto voice.
     */
    TIMBRE_TRANSFORMATION_FALSETTO = 0x01030400,
    /** A falsetto voice.
     */
    TIMBRE_TRANSFORMATION_FULL = 0x01030500,
    /** A clearer voice.
     */
    TIMBRE_TRANSFORMATION_CLEAR = 0x01030600,
    /** A more resounding voice.
     */
    TIMBRE_TRANSFORMATION_RESOUNDING = 0x01030700,
    /** A more ringing voice.
     */
    TIMBRE_TRANSFORMATION_RINGING = 0x01030800
};
/** The options for SDK preset audio effects.
 */
enum AUDIO_EFFECT_PRESET
{
    /** Turn off audio effects and use the original voice.
     */
    AUDIO_EFFECT_OFF = 0x00000000,
    /** An audio effect typical of a KTV venue.
     *
     * @note To achieve better audio effect quality, AR recommends calling \ref IRtcEngine::setAudioProfile "setAudioProfile"
     * and setting the `profile` parameter to `AUDIO_PROFILE_MUSIC_HIGH_QUALITY(4)` or `AUDIO_PROFILE_MUSIC_HIGH_QUALITY_STEREO(5)`
     * before setting this enumerator.
     */
    ROOM_ACOUSTICS_KTV = 0x02010100,
    /** An audio effect typical of a concert hall.
     *
     * @note To achieve better audio effect quality, AR recommends calling \ref IRtcEngine::setAudioProfile "setAudioProfile"
     * and setting the `profile` parameter to `AUDIO_PROFILE_MUSIC_HIGH_QUALITY(4)` or `AUDIO_PROFILE_MUSIC_HIGH_QUALITY_STEREO(5)`
     * before setting this enumerator.
     */
    ROOM_ACOUSTICS_VOCAL_CONCERT = 0x02010200,
    /** An audio effect typical of a recording studio.
     *
     * @note To achieve better audio effect quality, AR recommends calling \ref IRtcEngine::setAudioProfile "setAudioProfile"
     * and setting the `profile` parameter to `AUDIO_PROFILE_MUSIC_HIGH_QUALITY(4)` or `AUDIO_PROFILE_MUSIC_HIGH_QUALITY_STEREO(5)`
     * before setting this enumerator.
     */
    ROOM_ACOUSTICS_STUDIO = 0x02010300,
    /** An audio effect typical of a vintage phonograph.
     *
     * @note To achieve better audio effect quality, AR recommends calling \ref IRtcEngine::setAudioProfile "setAudioProfile"
     * and setting the `profile` parameter to `AUDIO_PROFILE_MUSIC_HIGH_QUALITY(4)` or `AUDIO_PROFILE_MUSIC_HIGH_QUALITY_STEREO(5)`
     * before setting this enumerator.
     */
    ROOM_ACOUSTICS_PHONOGRAPH = 0x02010400,
    /** A virtual stereo effect that renders monophonic audio as stereo audio.
     *
     * @note Call \ref IRtcEngine::setAudioProfile "setAudioProfile" and set the `profile` parameter to
     * `AUDIO_PROFILE_MUSIC_STANDARD_STEREO(3)` or `AUDIO_PROFILE_MUSIC_HIGH_QUALITY_STEREO(5)` before setting this
     * enumerator; otherwise, the enumerator setting does not take effect.
     */
    ROOM_ACOUSTICS_VIRTUAL_STEREO = 0x02010500,
    /** A more spatial audio effect.
     *
     * @note To achieve better audio effect quality, AR recommends calling \ref IRtcEngine::setAudioProfile "setAudioProfile"
     * and setting the `profile` parameter to `AUDIO_PROFILE_MUSIC_HIGH_QUALITY(4)` or `AUDIO_PROFILE_MUSIC_HIGH_QUALITY_STEREO(5)`
     * before setting this enumerator.
     */
    ROOM_ACOUSTICS_SPACIAL = 0x02010600,
    /** A more ethereal audio effect.
     *
     * @note To achieve better audio effect quality, AR recommends calling \ref IRtcEngine::setAudioProfile "setAudioProfile"
     * and setting the `profile` parameter to `AUDIO_PROFILE_MUSIC_HIGH_QUALITY(4)` or `AUDIO_PROFILE_MUSIC_HIGH_QUALITY_STEREO(5)`
     * before setting this enumerator.
     */
    ROOM_ACOUSTICS_ETHEREAL = 0x02010700,
    /** A 3D voice effect that makes the voice appear to be moving around the user. The default cycle period of the 3D
     * voice effect is 10 seconds. To change the cycle period, call \ref IRtcEngine::setAudioEffectParameters "setAudioEffectParameters"
     * after this method.
     *
     * @note
     * - Call \ref IRtcEngine::setAudioProfile "setAudioProfile" and set the `profile` parameter to `AUDIO_PROFILE_MUSIC_STANDARD_STEREO(3)`
     * or `AUDIO_PROFILE_MUSIC_HIGH_QUALITY_STEREO(5)` before setting this enumerator; otherwise, the enumerator setting does not take effect.
     * - If the 3D voice effect is enabled, users need to use stereo audio playback devices to hear the anticipated voice effect.
     */
    ROOM_ACOUSTICS_3D_VOICE = 0x02010800,
    /** The voice of an uncle.
     *
     * @note
     * - AR recommends using this enumerator to process a male-sounding voice; otherwise, you may not hear the anticipated voice effect.
     * - To achieve better audio effect quality, AR recommends calling \ref IRtcEngine::setAudioProfile "setAudioProfile" and
     * setting the `profile` parameter to `AUDIO_PROFILE_MUSIC_HIGH_QUALITY(4)` or `AUDIO_PROFILE_MUSIC_HIGH_QUALITY_STEREO(5)` before
     * setting this enumerator.
     */
    VOICE_CHANGER_EFFECT_UNCLE = 0x02020100,
    /** The voice of an old man.
     *
     * @note
     * - AR recommends using this enumerator to process a male-sounding voice; otherwise, you may not hear the anticipated voice effect.
     * - To achieve better audio effect quality, AR recommends calling \ref IRtcEngine::setAudioProfile "setAudioProfile" and setting
     * the `profile` parameter to `AUDIO_PROFILE_MUSIC_HIGH_QUALITY(4)` or `AUDIO_PROFILE_MUSIC_HIGH_QUALITY_STEREO(5)` before setting
     * this enumerator.
     */
    VOICE_CHANGER_EFFECT_OLDMAN = 0x02020200,
    /** The voice of a boy.
     *
     * @note
     * - AR recommends using this enumerator to process a male-sounding voice; otherwise, you may not hear the anticipated voice effect.
     * - To achieve better audio effect quality, AR recommends calling \ref IRtcEngine::setAudioProfile "setAudioProfile" and setting
     * the `profile` parameter to `AUDIO_PROFILE_MUSIC_HIGH_QUALITY(4)` or `AUDIO_PROFILE_MUSIC_HIGH_QUALITY_STEREO(5)` before
     * setting this enumerator.
     */
    VOICE_CHANGER_EFFECT_BOY = 0x02020300,
    /** The voice of a young woman.
     *
     * @note
     * - AR recommends using this enumerator to process a female-sounding voice; otherwise, you may not hear the anticipated voice effect.
     * - To achieve better audio effect quality, AR recommends calling \ref IRtcEngine::setAudioProfile "setAudioProfile" and setting
     * the `profile` parameter to `AUDIO_PROFILE_MUSIC_HIGH_QUALITY(4)` or `AUDIO_PROFILE_MUSIC_HIGH_QUALITY_STEREO(5)` before
     * setting this enumerator.
     */
    VOICE_CHANGER_EFFECT_SISTER = 0x02020400,
    /** The voice of a girl.
     *
     * @note
     * - AR recommends using this enumerator to process a female-sounding voice; otherwise, you may not hear the anticipated voice effect.
     * - To achieve better audio effect quality, AR recommends calling \ref IRtcEngine::setAudioProfile "setAudioProfile" and setting
     * the `profile` parameter to `AUDIO_PROFILE_MUSIC_HIGH_QUALITY(4)` or `AUDIO_PROFILE_MUSIC_HIGH_QUALITY_STEREO(5)` before
     * setting this enumerator.
     */
    VOICE_CHANGER_EFFECT_GIRL = 0x02020500,
    /** The voice of Pig King, a character in Journey to the West who has a voice like a growling bear.
     *
     * @note To achieve better audio effect quality, AR recommends calling \ref IRtcEngine::setAudioProfile "setAudioProfile" and
     * setting the `profile` parameter to `AUDIO_PROFILE_MUSIC_HIGH_QUALITY(4)` or `AUDIO_PROFILE_MUSIC_HIGH_QUALITY_STEREO(5)` before
     * setting this enumerator.
     */
    VOICE_CHANGER_EFFECT_PIGKING = 0x02020600,
    /** The voice of Hulk.
     *
     * @note To achieve better audio effect quality, AR recommends calling \ref IRtcEngine::setAudioProfile "setAudioProfile" and
     * setting the `profile` parameter to `AUDIO_PROFILE_MUSIC_HIGH_QUALITY(4)` or `AUDIO_PROFILE_MUSIC_HIGH_QUALITY_STEREO(5)` before
     * setting this enumerator.
     */
    VOICE_CHANGER_EFFECT_HULK = 0x02020700,
    /** An audio effect typical of R&B music.
     *
     * @note To achieve better audio effect quality, AR recommends calling \ref IRtcEngine::setAudioProfile "setAudioProfile" and
     * setting the `profile` parameter to `AUDIO_PROFILE_MUSIC_HIGH_QUALITY(4)` or `AUDIO_PROFILE_MUSIC_HIGH_QUALITY_STEREO(5)` before
     * setting this enumerator.
     */
    STYLE_TRANSFORMATION_RNB = 0x02030100,
    /** An audio effect typical of popular music.
     *
     * @note To achieve better audio effect quality, AR recommends calling \ref IRtcEngine::setAudioProfile "setAudioProfile" and
     * setting the `profile` parameter to `AUDIO_PROFILE_MUSIC_HIGH_QUALITY(4)` or `AUDIO_PROFILE_MUSIC_HIGH_QUALITY_STEREO(5)` before
     * setting this enumerator.
     */
    STYLE_TRANSFORMATION_POPULAR = 0x02030200,
    /** A pitch correction effect that corrects the user's pitch based on the pitch of the natural C major scale.
     * To change the basic mode and tonic pitch, call \ref IRtcEngine::setAudioEffectParameters "setAudioEffectParameters" after this method.
     *
     * @note To achieve better audio effect quality, AR recommends calling \ref IRtcEngine::setAudioProfile "setAudioProfile" and
     * setting the `profile` parameter to `AUDIO_PROFILE_MUSIC_HIGH_QUALITY(4)` or `AUDIO_PROFILE_MUSIC_HIGH_QUALITY_STEREO(5)` before
     * setting this enumerator.
     */
    PITCH_CORRECTION = 0x02040100
};
/** Audio codec profile types. The default value is LC_ACC. */
enum AUDIO_CODEC_PROFILE_TYPE
{
    /** 0: LC-AAC, which is the low-complexity audio codec type. */
    AUDIO_CODEC_PROFILE_LC_AAC = 0,
    /** 1: HE-AAC, which is the high-efficiency audio codec type. */
    AUDIO_CODEC_PROFILE_HE_AAC = 1,
};

/** Remote audio states.
 */
enum REMOTE_AUDIO_STATE
{
      /** 0: The remote audio is in the default state, probably due to
       * #REMOTE_AUDIO_REASON_LOCAL_MUTED (3),
       * #REMOTE_AUDIO_REASON_REMOTE_MUTED (5), or
       * #REMOTE_AUDIO_REASON_REMOTE_OFFLINE (7).
       */
      REMOTE_AUDIO_STATE_STOPPED = 0,  // Default state, audio is started or remote user disabled/muted audio stream
      /** 1: The first remote audio packet is received.
       */
      REMOTE_AUDIO_STATE_STARTING = 1,  // The first audio frame packet has been received
      /** 2: The remote audio stream is decoded and plays normally, probably
       * due to #REMOTE_AUDIO_REASON_NETWORK_RECOVERY (2),
       * #REMOTE_AUDIO_REASON_LOCAL_UNMUTED (4), or
       * #REMOTE_AUDIO_REASON_REMOTE_UNMUTED (6).
       */
      REMOTE_AUDIO_STATE_DECODING = 2,  // The first remote audio frame has been decoded or fronzen state ends
      /** 3: The remote audio is frozen, probably due to
       * #REMOTE_AUDIO_REASON_NETWORK_CONGESTION (1).
       */
      REMOTE_AUDIO_STATE_FROZEN = 3,    // Remote audio is frozen, probably due to network issue
      /** 4: The remote audio fails to start, probably due to
       * #REMOTE_AUDIO_REASON_INTERNAL (0).
       */
      REMOTE_AUDIO_STATE_FAILED = 4,    // Remote audio play failed
};

/** Remote audio state reasons.
 */
enum REMOTE_AUDIO_STATE_REASON
{
      /** 0: Internal reasons.
       */
      REMOTE_AUDIO_REASON_INTERNAL = 0,
      /** 1: Network congestion.
       */
      REMOTE_AUDIO_REASON_NETWORK_CONGESTION = 1,
      /** 2: Network recovery.
       */
      REMOTE_AUDIO_REASON_NETWORK_RECOVERY = 2,
      /** 3: The local user stops receiving the remote audio stream or
       * disables the audio module.
       */
      REMOTE_AUDIO_REASON_LOCAL_MUTED = 3,
      /** 4: The local user resumes receiving the remote audio stream or
       * enables the audio module.
       */
      REMOTE_AUDIO_REASON_LOCAL_UNMUTED = 4,
      /** 5: The remote user stops sending the audio stream or disables the
       * audio module.
       */
      REMOTE_AUDIO_REASON_REMOTE_MUTED = 5,
      /** 6: The remote user resumes sending the audio stream or enables the
       * audio module.
       */
      REMOTE_AUDIO_REASON_REMOTE_UNMUTED = 6,
      /** 7: The remote user leaves the channel.
       */
      REMOTE_AUDIO_REASON_REMOTE_OFFLINE = 7,
};

/** Remote video states. */
// enum REMOTE_VIDEO_STATE
// {
//     // REMOTE_VIDEO_STATE_STOPPED is not used at this version. Ignore this value.
//     // REMOTE_VIDEO_STATE_STOPPED = 0,  // Default state, video is started or remote user disabled/muted video stream
//       /** 1: The remote video is playing. */
//       REMOTE_VIDEO_STATE_RUNNING = 1,  // Running state, remote video can be displayed normally
//       /** 2: The remote video is frozen. */
//       REMOTE_VIDEO_STATE_FROZEN = 2,    // Remote video is frozen, probably due to network issue.
// };

/** The state of the remote video. */
enum REMOTE_VIDEO_STATE {
    /** 0: The remote video is in the default state, probably due to #REMOTE_VIDEO_STATE_REASON_LOCAL_MUTED (3), #REMOTE_VIDEO_STATE_REASON_REMOTE_MUTED (5), or #REMOTE_VIDEO_STATE_REASON_REMOTE_OFFLINE (7).
     */
    REMOTE_VIDEO_STATE_STOPPED = 0,

    /** 1: The first remote video packet is received.
     */
    REMOTE_VIDEO_STATE_STARTING = 1,

    /** 2: The remote video stream is decoded and plays normally, probably due to #REMOTE_VIDEO_STATE_REASON_NETWORK_RECOVERY (2), #REMOTE_VIDEO_STATE_REASON_LOCAL_UNMUTED (4), #REMOTE_VIDEO_STATE_REASON_REMOTE_UNMUTED (6), or #REMOTE_VIDEO_STATE_REASON_AUDIO_FALLBACK_RECOVERY (9).
     */
    REMOTE_VIDEO_STATE_DECODING = 2,

    /** 3: The remote video is frozen, probably due to #REMOTE_VIDEO_STATE_REASON_NETWORK_CONGESTION (1) or #REMOTE_VIDEO_STATE_REASON_AUDIO_FALLBACK (8).
     */
    REMOTE_VIDEO_STATE_FROZEN = 3,

    /** 4: The remote video fails to start, probably due to #REMOTE_VIDEO_STATE_REASON_INTERNAL (0).
     */
    REMOTE_VIDEO_STATE_FAILED = 4
};
/** The publishing state.
 */
enum STREAM_PUBLISH_STATE {
    /** 0: The initial publishing state after joining the channel.
     */
    PUB_STATE_IDLE = 0,
    /** 1: Fails to publish the local stream. Possible reasons:
     * - The local user calls \ref IRtcEngine::muteLocalAudioStream "muteLocalAudioStream(true)" or \ref IRtcEngine::muteLocalVideoStream "muteLocalVideoStream(true)" to stop sending local streams.
     * - The local user calls \ref IRtcEngine::disableAudio "disableAudio" or \ref IRtcEngine::disableVideo "disableVideo" to disable the entire audio or video module.
     * - The local user calls \ref IRtcEngine::enableLocalAudio "enableLocalAudio(false)" or \ref IRtcEngine::enableLocalVideo "enableLocalVideo(false)" to disable the local audio sampling or video capturing.
     * - The role of the local user is `AUDIENCE`.
     */
    PUB_STATE_NO_PUBLISHED = 1,
    /** 2: Publishing.
     */
    PUB_STATE_PUBLISHING = 2,
    /** 3: Publishes successfully.
     */
    PUB_STATE_PUBLISHED = 3
};
/** The subscribing state.
 */
enum STREAM_SUBSCRIBE_STATE {
    /** 0: The initial subscribing state after joining the channel.
     */
    SUB_STATE_IDLE = 0,
    /** 1: Fails to subscribe to the remote stream. Possible reasons:
     * - The remote user:
     *  - Calls \ref IRtcEngine::muteLocalAudioStream "muteLocalAudioStream(true)" or \ref IRtcEngine::muteLocalVideoStream "muteLocalVideoStream(true)" to stop sending local streams.
     *  - Calls \ref IRtcEngine::disableAudio "disableAudio" or \ref IRtcEngine::disableVideo "disableVideo" to disable the entire audio or video modules.
     *  - Calls \ref IRtcEngine::enableLocalAudio "enableLocalAudio(false)" or \ref IRtcEngine::enableLocalVideo "enableLocalVideo(false)" to disable the local audio sampling or video capturing.
     *  - The role of the remote user is `AUDIENCE`.
     * - The local user calls the following methods to stop receiving remote streams:
     *  - Calls \ref IRtcEngine::muteRemoteAudioStream "muteRemoteAudioStream(true)", \ref IRtcEngine::muteAllRemoteAudioStreams "muteAllRemoteAudioStreams(true)", or \ref IRtcEngine::setDefaultMuteAllRemoteAudioStreams "setDefaultMuteAllRemoteAudioStreams(true)" to stop receiving remote audio streams.
     *  - Calls \ref IRtcEngine::muteRemoteVideoStream "muteRemoteVideoStream(true)", \ref IRtcEngine::muteAllRemoteVideoStreams "muteAllRemoteVideoStreams(true)", or \ref IRtcEngine::setDefaultMuteAllRemoteVideoStreams "setDefaultMuteAllRemoteVideoStreams(true)" to stop receiving remote video streams.
     */
    SUB_STATE_NO_SUBSCRIBED = 1,
    /** 2: Subscribing.
     */
    SUB_STATE_SUBSCRIBING = 2,
    /** 3: Subscribes to and receives the remote stream successfully.
     */
    SUB_STATE_SUBSCRIBED = 3
};

/** The remote video frozen type. */
enum XLA_REMOTE_VIDEO_FROZEN_TYPE {
    /** 0: 500ms video frozen type.
     */
    XLA_REMOTE_VIDEO_FROZEN_500MS = 0,
    /** 1: 200ms video frozen type.
     */
    XLA_REMOTE_VIDEO_FROZEN_200MS = 1,
    /** 2: 600ms video frozen type.
     */
    XLA_REMOTE_VIDEO_FROZEN_600MS = 2,
    /** 3: max video frozen type.
     */
    XLA_REMOTE_VIDEO_FROZEN_TYPE_MAX = 3,
};

/** The remote audio frozen type. */
enum XLA_REMOTE_AUDIO_FROZEN_TYPE {
    /** 0: 80ms audio frozen.
     */
    XLA_REMOTE_AUDIO_FROZEN_80MS = 0,
    /** 1: 200ms audio frozen.
     */
    XLA_REMOTE_AUDIO_FROZEN_200MS = 1,
    /** 2: max audio frozen type.
     */
    XLA_REMOTE_AUDIO_FROZEN_TYPE_MAX = 2,
};

/** The reason for the remote video state change. */
enum REMOTE_VIDEO_STATE_REASON {
    /** 0: The SDK reports this reason when the video state changes.
     */
    REMOTE_VIDEO_STATE_REASON_INTERNAL = 0,

    /** 1: Network congestion.
     */
    REMOTE_VIDEO_STATE_REASON_NETWORK_CONGESTION = 1,

    /** 2: Network recovery.
     */
    REMOTE_VIDEO_STATE_REASON_NETWORK_RECOVERY = 2,

    /** 3: The local user stops receiving the remote video stream or disables the video module.
     */
    REMOTE_VIDEO_STATE_REASON_LOCAL_MUTED = 3,

    /** 4: The local user resumes receiving the remote video stream or enables the video module.
     */
    REMOTE_VIDEO_STATE_REASON_LOCAL_UNMUTED = 4,

    /** 5: The remote user stops sending the video stream or disables the video module.
     */
    REMOTE_VIDEO_STATE_REASON_REMOTE_MUTED = 5,

    /** 6: The remote user resumes sending the video stream or enables the video module.
     */
    REMOTE_VIDEO_STATE_REASON_REMOTE_UNMUTED = 6,

    /** 7: The remote user leaves the channel.
     */
    REMOTE_VIDEO_STATE_REASON_REMOTE_OFFLINE = 7,

    /** 8: The remote audio-and-video stream falls back to the audio-only stream due to poor network conditions.
     */
    REMOTE_VIDEO_STATE_REASON_AUDIO_FALLBACK = 8,

    /** 9: The remote audio-only stream switches back to the audio-and-video stream after the network conditions improve.
     */
    REMOTE_VIDEO_STATE_REASON_AUDIO_FALLBACK_RECOVERY = 9

};

/** Video frame rates. */
enum FRAME_RATE
{
      /** 1: 1 fps */
    FRAME_RATE_FPS_1 = 1,
        /** 7: 7 fps */
    FRAME_RATE_FPS_7 = 7,
      /** 10: 10 fps */
    FRAME_RATE_FPS_10 = 10,
    /** 15: 15 fps */
    FRAME_RATE_FPS_15 = 15,
        /** 24: 24 fps */
    FRAME_RATE_FPS_24 = 24,
    /** 30: 30 fps */
    FRAME_RATE_FPS_30 = 30,
    /** 60: 60 fps (Windows and macOS only) */
    FRAME_RATE_FPS_60 = 60,
};

/** Video output orientation modes.
 */
enum ORIENTATION_MODE {
  /** 0: (Default) Adaptive mode.

   The video encoder adapts to the orientation mode of the video input device.

   - If the width of the captured video from the SDK is greater than the height, the encoder sends the video in landscape mode. The encoder also sends the rotational information of the video, and the receiver uses the rotational information to rotate the received video.
   - When you use a custom video source, the output video from the encoder inherits the orientation of the original video. If the original video is in portrait mode, the output video from the encoder is also in portrait mode. The encoder also sends the rotational information of the video to the receiver.
   */
    ORIENTATION_MODE_ADAPTIVE = 0,
    /** 1: Landscape mode.

     The video encoder always sends the video in landscape mode. The video encoder rotates the original video before sending it and the rotational infomation is 0. This mode applies to scenarios involving CDN live streaming.
     */
    ORIENTATION_MODE_FIXED_LANDSCAPE = 1,
    /** 2: Portrait mode.

     The video encoder always sends the video in portrait mode. The video encoder rotates the original video before sending it and the rotational infomation is 0. This mode applies to scenarios involving CDN live streaming.
     */
    ORIENTATION_MODE_FIXED_PORTRAIT = 2,
};

/** Video degradation preferences when the bandwidth is a constraint. */
enum DEGRADATION_PREFERENCE {
    /** 0: (Default) Degrade the frame rate in order to maintain the video quality. */
    MAINTAIN_QUALITY = 0,
    /** 1: Degrade the video quality in order to maintain the frame rate. */
    MAINTAIN_FRAMERATE = 1,
    /** 2: (For future use) Maintain a balance between the frame rate and video quality. */
    MAINTAIN_BALANCED = 2,
};

/** Stream fallback options. */
enum STREAM_FALLBACK_OPTIONS
{
    /** 0: No fallback behavior for the local/remote video stream when the uplink/downlink network conditions are poor. The quality of the stream is not guaranteed. */
    STREAM_FALLBACK_OPTION_DISABLED = 0,
    /** 1: Under poor downlink network conditions, the remote video stream, to which you subscribe, falls back to the low-stream (low resolution and low bitrate) video. You can set this option only in the \ref IRtcEngine::setRemoteSubscribeFallbackOption "setRemoteSubscribeFallbackOption" method. Nothing happens when you set this in the \ref IRtcEngine::setLocalPublishFallbackOption "setLocalPublishFallbackOption" method. */
    STREAM_FALLBACK_OPTION_VIDEO_STREAM_LOW = 1,
    /** 2: Under poor uplink network conditions, the published video stream falls back to audio only.

    Under poor downlink network conditions, the remote video stream, to which you subscribe, first falls back to the low-stream (low resolution and low bitrate) video; and then to an audio-only stream if the network conditions worsen.*/
    STREAM_FALLBACK_OPTION_AUDIO_ONLY = 2,
};

 /** Camera capturer configuration.
 */
 enum CAPTURER_OUTPUT_PREFERENCE
 {
     /** 0: (Default) self-adapts the camera output parameters to the system performance and network conditions to balance CPU consumption and video preview quality.
     */
     CAPTURER_OUTPUT_PREFERENCE_AUTO = 0,
     /** 1: Prioritizes the system performance. The SDK chooses the dimension and frame rate of the local camera capture closest to those set by \ref IRtcEngine::setVideoEncoderConfiguration "setVideoEncoderConfiguration".
     */
     CAPTURER_OUTPUT_PREFERENCE_PERFORMANCE = 1,
     /** 2: Prioritizes the local preview quality. The SDK chooses higher camera output parameters to improve the local video preview quality. This option requires extra CPU and RAM usage for video pre-processing.
     */
     CAPTURER_OUTPUT_PREFERENCE_PREVIEW = 2,
 };

/** The priority of the remote user.
 */
enum PRIORITY_TYPE
{
  /** 50: The user's priority is high.
   */
  PRIORITY_HIGH = 50,
  /** 100: (Default) The user's priority is normal.
  */
  PRIORITY_NORMAL = 100,
};

/** Connection states. */
enum CONNECTION_STATE_TYPE
{
  /** 1: The SDK is disconnected from AR's edge server.

   - This is the initial state before calling the \ref ar::rtc::IRtcEngine::joinChannel "joinChannel" method.
   - The SDK also enters this state when the application calls the \ref ar::rtc::IRtcEngine::leaveChannel "leaveChannel" method.
   */
  CONNECTION_STATE_DISCONNECTED = 1,
  /** 2: The SDK is connecting to AR's edge server.

   - When the application calls the \ref ar::rtc::IRtcEngine::joinChannel "joinChannel" method, the SDK starts to establish a connection to the specified channel, triggers the \ref ar::rtc::IRtcEngineEventHandler::onConnectionStateChanged "onConnectionStateChanged" callback, and switches to the #CONNECTION_STATE_CONNECTING state.
   - When the SDK successfully joins the channel, it triggers the \ref ar::rtc::IRtcEngineEventHandler::onConnectionStateChanged "onConnectionStateChanged" callback and switches to the #CONNECTION_STATE_CONNECTED state.
   - After the SDK joins the channel and when it finishes initializing the media engine, the SDK triggers the \ref ar::rtc::IRtcEngineEventHandler::onJoinChannelSuccess "onJoinChannelSuccess" callback.
   */
  CONNECTION_STATE_CONNECTING = 2,
  /** 3: The SDK is connected to AR's edge server and has joined a channel. You can now publish or subscribe to a media stream in the channel.

   If the connection to the channel is lost because, for example, if the network is down or switched, the SDK automatically tries to reconnect and triggers:
   - The \ref ar::rtc::IRtcEngineEventHandler::onConnectionInterrupted "onConnectionInterrupted" callback (deprecated).
   - The \ref ar::rtc::IRtcEngineEventHandler::onConnectionStateChanged "onConnectionStateChanged" callback and switches to the #CONNECTION_STATE_RECONNECTING state.
   */
  CONNECTION_STATE_CONNECTED = 3,
  /** 4: The SDK keeps rejoining the channel after being disconnected from a joined channel because of network issues.

   - If the SDK cannot rejoin the channel within 10 seconds after being disconnected from AR's edge server, the SDK triggers the \ref ar::rtc::IRtcEngineEventHandler::onConnectionLost "onConnectionLost" callback, stays in the #CONNECTION_STATE_RECONNECTING state, and keeps rejoining the channel.
   - If the SDK fails to rejoin the channel 20 minutes after being disconnected from AR's edge server, the SDK triggers the \ref ar::rtc::IRtcEngineEventHandler::onConnectionStateChanged "onConnectionStateChanged" callback, switches to the #CONNECTION_STATE_FAILED state, and stops rejoining the channel.
   */
  CONNECTION_STATE_RECONNECTING = 4,
  /** 5: The SDK fails to connect to AR's edge server or join the channel.

   You must call the \ref ar::rtc::IRtcEngine::leaveChannel "leaveChannel" method to leave this state, and call the \ref ar::rtc::IRtcEngine::joinChannel "joinChannel" method again to rejoin the channel.

   If the SDK is banned from joining the channel by AR's edge server (through the RESTful API), the SDK triggers the \ref ar::rtc::IRtcEngineEventHandler::onConnectionBanned "onConnectionBanned" (deprecated) and \ref ar::rtc::IRtcEngineEventHandler::onConnectionStateChanged "onConnectionStateChanged" callbacks.
   */
  CONNECTION_STATE_FAILED = 5,
};

/** Reasons for a connection state change. */
enum CONNECTION_CHANGED_REASON_TYPE
{
  /** 0: The SDK is connecting to AR's edge server. */
  CONNECTION_CHANGED_CONNECTING = 0,
  /** 1: The SDK has joined the channel successfully. */
  CONNECTION_CHANGED_JOIN_SUCCESS = 1,
  /** 2: The connection between the SDK and AR's edge server is interrupted. */
  CONNECTION_CHANGED_INTERRUPTED = 2,
  /** 3: The connection between the SDK and AR's edge server is banned by AR's edge server. */
  CONNECTION_CHANGED_BANNED_BY_SERVER = 3,
  /** 4: The SDK fails to join the channel for more than 20 minutes and stops reconnecting to the channel. */
  CONNECTION_CHANGED_JOIN_FAILED = 4,
  /** 5: The SDK has left the channel. */
  CONNECTION_CHANGED_LEAVE_CHANNEL = 5,
  /** 6: The connection failed since Appid is not valid. */
  CONNECTION_CHANGED_INVALID_APP_ID = 6,
  /** 7: The connection failed since channel name is not valid. */
  CONNECTION_CHANGED_INVALID_CHANNEL_NAME = 7,
  /** 8: The connection failed since token is not valid, possibly because:

   - The App Certificate for the project is enabled in Console, but you do not use Token when joining the channel. If you enable the App Certificate, you must use a token to join the channel.
   - The uid that you specify in the \ref ar::rtc::IRtcEngine::joinChannel "joinChannel" method is different from the uid that you pass for generating the token.
   */
  CONNECTION_CHANGED_INVALID_TOKEN = 8,
  /** 9: The connection failed since token is expired. */
  CONNECTION_CHANGED_TOKEN_EXPIRED = 9,
  /** 10: The connection is rejected by server. This error usually occurs in the following situations:
   * - When the user is already in the channel, and still calls the method to join the channel, for example,
   * \ref IRtcEngine::joinChannel "joinChannel".
   * - When the user tries to join a channel during \ref IRtcEngine::startEchoTest "startEchoTest". Once you
   * call \ref IRtcEngine::startEchoTest "startEchoTest", you need to call \ref IRtcEngine::stopEchoTest "stopEchoTest" before joining a channel.
   *
   */
  CONNECTION_CHANGED_REJECTED_BY_SERVER = 10,
  /** 11: The connection changed to reconnecting since SDK has set a proxy server. */
  CONNECTION_CHANGED_SETTING_PROXY_SERVER = 11,
  /** 12: When SDK is in connection failed, the renew token operation will make it connecting. */
  CONNECTION_CHANGED_RENEW_TOKEN = 12,
  /** 13: The IP Address of SDK client has changed. i.e., Network type or IP/Port changed by network operator might change client IP address. */
  CONNECTION_CHANGED_CLIENT_IP_ADDRESS_CHANGED = 13,
  /** 14: Timeout for the keep-alive of the connection between the SDK and AR's edge server. The connection state changes to CONNECTION_STATE_RECONNECTING(4). */
  CONNECTION_CHANGED_KEEP_ALIVE_TIMEOUT = 14,
};

/** Network type. */
enum NETWORK_TYPE
{
  /** -1: The network type is unknown. */
  NETWORK_TYPE_UNKNOWN = -1,
  /** 0: The SDK disconnects from the network. */
  NETWORK_TYPE_DISCONNECTED = 0,
  /** 1: The network type is LAN. */
  NETWORK_TYPE_LAN = 1,
  /** 2: The network type is Wi-Fi(including hotspots). */
  NETWORK_TYPE_WIFI = 2,
  /** 3: The network type is mobile 2G. */
  NETWORK_TYPE_MOBILE_2G = 3,
  /** 4: The network type is mobile 3G. */
  NETWORK_TYPE_MOBILE_3G = 4,
  /** 5: The network type is mobile 4G. */
  NETWORK_TYPE_MOBILE_4G = 5,
};

/** States of the last-mile network probe test. */
enum LASTMILE_PROBE_RESULT_STATE {
  /** 1: The last-mile network probe test is complete. */
  LASTMILE_PROBE_RESULT_COMPLETE = 1,
  /** 2: The last-mile network probe test is incomplete and the bandwidth estimation is not available, probably due to limited test resources. */
  LASTMILE_PROBE_RESULT_INCOMPLETE_NO_BWE = 2,
  /** 3: The last-mile network probe test is not carried out, probably due to poor network conditions. */
  LASTMILE_PROBE_RESULT_UNAVAILABLE = 3
};
/** Audio output routing. */
enum AUDIO_ROUTE_TYPE {
    /** Default.
     */
    AUDIO_ROUTE_DEFAULT = -1,
    /** Headset.
     */
    AUDIO_ROUTE_HEADSET = 0,
    /** Earpiece.
     */
    AUDIO_ROUTE_EARPIECE = 1,
    /** Headset with no microphone.
     */
    AUDIO_ROUTE_HEADSET_NO_MIC = 2,
    /** Speakerphone.
     */
    AUDIO_ROUTE_SPEAKERPHONE = 3,
    /** Loudspeaker.
     */
    AUDIO_ROUTE_LOUDSPEAKER = 4,
    /** Bluetooth headset.
     */
    AUDIO_ROUTE_BLUETOOTH = 5,
    /** USB peripheral.
     */
    AUDIO_ROUTE_USB = 6,
    /** HDMI peripheral.
     */
    AUDIO_ROUTE_HDMI = 7,
    /** DisplayPort peripheral.
     */
    AUDIO_ROUTE_DISPLAYPORT = 8,
    /** Apple AirPlay.
     */
    AUDIO_ROUTE_AIRPLAY = 9,
};

#if (defined(__APPLE__) && TARGET_OS_IOS)
/** Audio session restriction. */
enum AUDIO_SESSION_OPERATION_RESTRICTION {
    /** No restriction, the SDK has full control of the audio session operations. */
    AUDIO_SESSION_OPERATION_RESTRICTION_NONE = 0,
    /** The SDK does not change the audio session category. */
    AUDIO_SESSION_OPERATION_RESTRICTION_SET_CATEGORY = 1,
    /** The SDK does not change any setting of the audio session (category, mode, categoryOptions). */
    AUDIO_SESSION_OPERATION_RESTRICTION_CONFIGURE_SESSION = 1 << 1,
    /** The SDK keeps the audio session active when leaving a channel. */
    AUDIO_SESSION_OPERATION_RESTRICTION_DEACTIVATE_SESSION = 1 << 2,
    /** The SDK does not configure the audio session anymore. */
    AUDIO_SESSION_OPERATION_RESTRICTION_ALL = 1 << 7,
};
#endif

#if defined(__ANDROID__) || (defined(__APPLE__) && TARGET_OS_IOS)
enum CAMERA_DIRECTION {
    /** The rear camera. */
    CAMERA_REAR = 0,
    /** The front camera. */
    CAMERA_FRONT = 1,
};
#endif

/** The uplink or downlink last-mile network probe test result. */
struct LastmileProbeOneWayResult {
  /** The packet loss rate (%). */
  unsigned int packetLossRate;
  /** The network jitter (ms). */
  unsigned int jitter;
  /* The estimated available bandwidth (bps). */
  unsigned int availableBandwidth;
};

/** The uplink and downlink last-mile network probe test result. */
struct LastmileProbeResult{
  /** The state of the probe test. */
  LASTMILE_PROBE_RESULT_STATE state;
  /** The uplink last-mile network probe test result. */
  LastmileProbeOneWayResult uplinkReport;
  /** The downlink last-mile network probe test result. */
  LastmileProbeOneWayResult downlinkReport;
  /** The round-trip delay time (ms). */
  unsigned int rtt;
};

/** Configurations of the last-mile network probe test. */
struct LastmileProbeConfig {
  /** Sets whether or not to test the uplink network. Some users, for example, the audience in a `LIVE_BROADCASTING` channel, do not need such a test:
  - true: test.
  - false: do not test. */
  bool probeUplink;
  /** Sets whether or not to test the downlink network:
  - true: test.
  - false: do not test. */
  bool probeDownlink;
  /** The expected maximum sending bitrate (bps) of the local user. The value ranges between 100000 and 5000000. We recommend setting this parameter according to the bitrate value set by \ref IRtcEngine::setVideoEncoderConfiguration "setVideoEncoderConfiguration". */
  unsigned int expectedUplinkBitrate;
  /** The expected maximum receiving bitrate (bps) of the local user. The value ranges between 100000 and 5000000. */
  unsigned int expectedDownlinkBitrate;
};

/** Properties of the audio volume information.

 An array containing the user ID and volume information for each speaker.
 */
struct AudioVolumeInfo
{
   /**
    User ID of the speaker. The uid of the local user is 0.
    */
    uid_t uid;
   /** The volume of the speaker. The volume ranges between 0 (lowest volume) and 255 (highest volume).
    */
    unsigned int volume;
    /** Voice activity status of the local user.
     * - 0: The local user is not speaking.
     * - 1: The local user is speaking.
     * 
     * @note
     * - The `vad` parameter cannot report the voice activity status of the remote users. In the remote users' callback, `vad` = 0.
     * - Ensure that you set `report_vad`(true) in the \ref ar::rtc::IRtcEngine::enableAudioVolumeIndication(int, int, bool) "enableAudioVolumeIndication" method to enable the voice activity detection of the local user.
     */
    unsigned int vad;
    /** The channel ID, which indicates which channel the speaker is in.
     */
    const char * channelId;
};
/// @cond
/** The detailed options of a user.
 */
struct ClientRoleOptions
{
    /** The latency level of an audience member in a live interactive streaming. See #AUDIENCE_LATENCY_LEVEL_TYPE.
     */
    AUDIENCE_LATENCY_LEVEL_TYPE audienceLatencyLevel;
    ClientRoleOptions()
        : audienceLatencyLevel(AUDIENCE_LATENCY_LEVEL_ULTRA_LOW_LATENCY) {}
};
/// @endcond
/** Statistics of the channel.
 */
struct RtcStats
{
  /**
   Call duration (s), represented by an aggregate value.
   */
    unsigned int duration;
    /**
     Total number of bytes transmitted, represented by an aggregate value.
     */
    unsigned int txBytes;
    /**
     Total number of bytes received, represented by an aggregate value.
     */
    unsigned int rxBytes;
     /** Total number of audio bytes sent (bytes), represented
     * by an aggregate value.
     */
    unsigned int txAudioBytes;
    /** Total number of video bytes sent (bytes), represented
     * by an aggregate value.
     */
    unsigned int txVideoBytes;
    /** Total number of audio bytes received (bytes) before
     * network countermeasures, represented by an aggregate value.
     */
    unsigned int rxAudioBytes;
    /** Total number of video bytes received (bytes),
     * represented by an aggregate value.
     */
    unsigned int rxVideoBytes;

    /**
     Transmission bitrate (Kbps), represented by an instantaneous value.
     */
    unsigned short txKBitRate;
    /**
     Receive bitrate (Kbps), represented by an instantaneous value.
     */
    unsigned short rxKBitRate;
    /**
     Audio receive bitrate (Kbps), represented by an instantaneous value.
     */
    unsigned short rxAudioKBitRate;
    /**
     Audio transmission bitrate (Kbps), represented by an instantaneous value.
     */
    unsigned short txAudioKBitRate;
    /**
     Video receive bitrate (Kbps), represented by an instantaneous value.
     */
    unsigned short rxVideoKBitRate;
    /**
     Video transmission bitrate (Kbps), represented by an instantaneous value.
     */
    unsigned short txVideoKBitRate;
    /** Client-server latency (ms)
     */
    unsigned short lastmileDelay;
    /** The packet loss rate (%) from the local client to AR's edge server,
     * before using the anti-packet-loss method.
     */
    unsigned short txPacketLossRate;
    /** The packet loss rate (%) from AR's edge server to the local client,
     * before using the anti-packet-loss method.
     */
    unsigned short rxPacketLossRate;
    /** Number of users in the channel.

     - Communication profile: The number of users in the channel.
     - Live broadcast profile:

         -  If the local user is an audience: The number of users in the channel = The number of hosts in the channel + 1.
         -  If the user is a host: The number of users in the channel = The number of hosts in the channel.
     */
    unsigned int userCount;
    /**
     Application CPU usage (%).
     */
    double cpuAppUsage;
    /**
     System CPU usage (%).
     */
    double cpuTotalUsage;
    /** The round-trip time delay from the client to the local router.
     */
    int gatewayRtt;
    /**
     The memory usage ratio of the app (%).
     @note This value is for reference only. Due to system limitations, you may not get the value of this member.
     */
    double memoryAppUsageRatio;
    /**
     The memory usage ratio of the system (%).
     @note This value is for reference only. Due to system limitations, you may not get the value of this member.
     */
    double memoryTotalUsageRatio;
    /**
     The memory usage of the app (KB).
     @note This value is for reference only. Due to system limitations, you may not get the value of this member.
     */
    int memoryAppUsageInKbytes;
  RtcStats()
      : duration(0)
      , txBytes(0)
      , rxBytes(0)
      , txAudioBytes(0)
      , txVideoBytes(0)
      , rxAudioBytes(0)
      , rxVideoBytes(0)
      , txKBitRate(0)
      , rxKBitRate(0)
      , rxAudioKBitRate(0)
      , txAudioKBitRate(0)
      , rxVideoKBitRate(0)
      , txVideoKBitRate(0)
      , lastmileDelay(0)
      , txPacketLossRate(0)
      , rxPacketLossRate(0)
      , userCount(0)
      , cpuAppUsage(0)
      , cpuTotalUsage(0)
      , gatewayRtt(0)
      , memoryAppUsageRatio(0)
      , memoryTotalUsageRatio(0)
      , memoryAppUsageInKbytes(0) {}
};

/** Quality change of the local video in terms of target frame rate and target bit rate since last count.
  */
enum QUALITY_ADAPT_INDICATION {
  /** The quality of the local video stays the same. */
  ADAPT_NONE = 0,
  /** The quality improves because the network bandwidth increases. */
  ADAPT_UP_BANDWIDTH = 1,
  /** The quality worsens because the network bandwidth decreases. */
  ADAPT_DOWN_BANDWIDTH = 2,
};

/** The error code in CHANNEL_MEDIA_RELAY_ERROR. */
enum CHANNEL_MEDIA_RELAY_ERROR {
    /** 0: The state is normal.
     */
    RELAY_OK = 0,
    /** 1: An error occurs in the server response.
     */
    RELAY_ERROR_SERVER_ERROR_RESPONSE = 1,
    /** 2: No server response. You can call the
     * \ref ar::rtc::IRtcEngine::leaveChannel "leaveChannel" method to
     * leave the channel.
     */
    RELAY_ERROR_SERVER_NO_RESPONSE = 2,
    /** 3: The SDK fails to access the service, probably due to limited
     * resources of the server.
     */
    RELAY_ERROR_NO_RESOURCE_AVAILABLE = 3,
    /** 4: Fails to send the relay request.
     */
    RELAY_ERROR_FAILED_JOIN_SRC = 4,
    /** 5: Fails to accept the relay request.
     */
    RELAY_ERROR_FAILED_JOIN_DEST = 5,
    /** 6: The server fails to receive the media stream.
     */
    RELAY_ERROR_FAILED_PACKET_RECEIVED_FROM_SRC = 6,
    /** 7: The server fails to send the media stream.
     */
    RELAY_ERROR_FAILED_PACKET_SENT_TO_DEST = 7,
    /** 8: The SDK disconnects from the server due to poor network
     * connections. You can call the \ref ar::rtc::IRtcEngine::leaveChannel
     * "leaveChannel" method to leave the channel.
     */
    RELAY_ERROR_SERVER_CONNECTION_LOST = 8,
    /** 9: An internal error occurs in the server.
     */
    RELAY_ERROR_INTERNAL_ERROR = 9,
    /** 10: The token of the source channel has expired.
     */
    RELAY_ERROR_SRC_TOKEN_EXPIRED = 10,
    /** 11: The token of the destination channel has expired.
     */
    RELAY_ERROR_DEST_TOKEN_EXPIRED = 11,
};

/** The event code in CHANNEL_MEDIA_RELAY_EVENT. */
enum CHANNEL_MEDIA_RELAY_EVENT {
    /** 0: The user disconnects from the server due to poor network
     * connections.
     */
    RELAY_EVENT_NETWORK_DISCONNECTED = 0,
    /** 1: The network reconnects.
     */
    RELAY_EVENT_NETWORK_CONNECTED = 1,
    /** 2: The user joins the source channel.
     */
    RELAY_EVENT_PACKET_JOINED_SRC_CHANNEL = 2,
    /** 3: The user joins the destination channel.
     */
    RELAY_EVENT_PACKET_JOINED_DEST_CHANNEL = 3,
    /** 4: The SDK starts relaying the media stream to the destination channel.
     */
    RELAY_EVENT_PACKET_SENT_TO_DEST_CHANNEL = 4,
    /** 5: The server receives the video stream from the source channel.
     */
    RELAY_EVENT_PACKET_RECEIVED_VIDEO_FROM_SRC = 5,
    /** 6: The server receives the audio stream from the source channel.
     */
    RELAY_EVENT_PACKET_RECEIVED_AUDIO_FROM_SRC = 6,
    /** 7: The destination channel is updated.
     */
    RELAY_EVENT_PACKET_UPDATE_DEST_CHANNEL = 7,
    /** 8: The destination channel update fails due to internal reasons.
     */
    RELAY_EVENT_PACKET_UPDATE_DEST_CHANNEL_REFUSED = 8,
    /** 9: The destination channel does not change, which means that the
     * destination channel fails to be updated.
     */
    RELAY_EVENT_PACKET_UPDATE_DEST_CHANNEL_NOT_CHANGE = 9,
    /** 10: The destination channel name is NULL.
     */
    RELAY_EVENT_PACKET_UPDATE_DEST_CHANNEL_IS_NULL = 10,
    /** 11: The video profile is sent to the server.
     */
    RELAY_EVENT_VIDEO_PROFILE_UPDATE = 11,
};

/** The state code in CHANNEL_MEDIA_RELAY_STATE. */
enum CHANNEL_MEDIA_RELAY_STATE {
    /** 0: The SDK is initializing.
     */
    RELAY_STATE_IDLE = 0,
    /** 1: The SDK tries to relay the media stream to the destination channel.
     */
    RELAY_STATE_CONNECTING = 1,
    /** 2: The SDK successfully relays the media stream to the destination
     * channel.
     */
    RELAY_STATE_RUNNING = 2,
    /** 3: A failure occurs. See the details in code.
     */
    RELAY_STATE_FAILURE = 3,
};

/** Statistics of the local video stream.
 */
struct LocalVideoStats
{
  /** Bitrate (Kbps) sent in the reported interval, which does not include
   * the bitrate of the retransmission video after packet loss.
   */
  int sentBitrate;
  /** Frame rate (fps) sent in the reported interval, which does not include
   * the frame rate of the retransmission video after packet loss.
   */
  int sentFrameRate;
  /** The encoder output frame rate (fps) of the local video.
   */
  int encoderOutputFrameRate;
  /** The render output frame rate (fps) of the local video.
   */
  int rendererOutputFrameRate;
  /** The target bitrate (Kbps) of the current encoder. This value is estimated by the SDK based on the current network conditions.
    */
  int targetBitrate;
  /** The target frame rate (fps) of the current encoder.
    */
  int targetFrameRate;
  /** Quality change of the local video in terms of target frame rate and
   * target bit rate in this reported interval. See #QUALITY_ADAPT_INDICATION.
   */
  QUALITY_ADAPT_INDICATION qualityAdaptIndication;
  /** The encoding bitrate (Kbps), which does not include the bitrate of the
   * re-transmission video after packet loss.
   */
  int encodedBitrate;
  /** The width of the encoding frame (px).
   */
  int encodedFrameWidth;
  /** The height of the encoding frame (px).
   */
  int encodedFrameHeight;
  /** The value of the sent frames, represented by an aggregate value.
   */
  int encodedFrameCount;
  /** The codec type of the local video:
   * - VIDEO_CODEC_VP8 = 1: VP8.
   * - VIDEO_CODEC_H264 = 2: (Default) H.264.
   */
  VIDEO_CODEC_TYPE codecType;
  /** The video packet loss rate (%) from the local client to the AR edge server before applying the anti-packet loss strategies.
   */
  unsigned short txPacketLossRate;
  /** The capture frame rate (fps) of the local video.
   */
  int captureFrameRate;
};

/** Statistics of the remote video stream.
 */
struct RemoteVideoStats
{
/**
 User ID of the remote user sending the video streams.
 */
uid_t uid;
/** **DEPRECATED** Time delay (ms).
 *
 * In scenarios where audio and video is synchronized, you can use the value of
 * `networkTransportDelay` and `jitterBufferDelay` in `RemoteAudioStats` to know the delay statistics of the remote video.
 */
int delay;
/** Width (pixels) of the video stream.
 */
int width;
/**
 Height (pixels) of the video stream.
 */
int height;
/**
 Bitrate (Kbps) received since the last count.
 */
int receivedBitrate;
/** The decoder output frame rate (fps) of the remote video.
 */
int decoderOutputFrameRate;
/** The render output frame rate (fps) of the remote video.
 */
int rendererOutputFrameRate;
/** Packet loss rate (%) of the remote video stream after using the anti-packet-loss method.
 */
int packetLossRate;
/** The type of the remote video stream: #REMOTE_VIDEO_STREAM_TYPE
 */
REMOTE_VIDEO_STREAM_TYPE rxStreamType;
/**
 The total freeze time (ms) of the remote video stream after the remote user joins the channel.
 In a video session where the frame rate is set to no less than 5 fps, video freeze occurs when
 the time interval between two adjacent renderable video frames is more than 500 ms.
 */
int totalFrozenTime;
/**
 The total video freeze time as a percentage (%) of the total time when the video is available.
 */
int frozenRate;
/**
 The total time (ms) when the remote user in the Communication profile or the remote
 broadcaster in the Live-broadcast profile neither stops sending the video stream nor
 disables the video module after joining the channel.

 @since v3.0.1
*/
int totalActiveTime;
/**
 * The total publish duration (ms) of the remote video stream.
 */
int publishDuration;
};

/** Audio statistics of the local user */
struct LocalAudioStats
{
    /** The number of channels.
     */
    int numChannels;
    /** The sample rate (Hz).
     */
    int sentSampleRate;
    /** The average sending bitrate (Kbps).
     */
    int sentBitrate;
    /** The audio packet loss rate (%) from the local client to the AR edge server before applying the anti-packet loss strategies.
     */
    unsigned short txPacketLossRate;
};

/** Audio statistics of a remote user */
struct RemoteAudioStats
{
    /** User ID of the remote user sending the audio streams.
     *
     */
    uid_t uid;
    /** Audio quality received by the user: #QUALITY_TYPE.
     */
    int quality;
    /** Network delay (ms) from the sender to the receiver.
     */
    int networkTransportDelay;
    /** Network delay (ms) from the receiver to the jitter buffer.
     */
    int jitterBufferDelay;
    /** The audio frame loss rate in the reported interval.
     */
    int audioLossRate;
    /** The number of channels.
     */
    int numChannels;
    /** The sample rate (Hz) of the received audio stream in the reported
     * interval.
     */
    int receivedSampleRate;
    /** The average bitrate (Kbps) of the received audio stream in the
     * reported interval. */
    int receivedBitrate;
    /** The total freeze time (ms) of the remote audio stream after the remote user joins the channel. In a session, audio freeze occurs when the audio frame loss rate reaches 4%.
     */
    int totalFrozenTime;
    /** The total audio freeze time as a percentage (%) of the total time when the audio is available. */
    int frozenRate;
    /** The total time (ms) when the remote user in the `COMMUNICATION` profile or the remote host in
     the `LIVE_BROADCASTING` profile neither stops sending the audio stream nor disables the audio module after joining the channel.
     */
    int totalActiveTime;
    /**
     * The total publish duration (ms) of the remote audio stream.
     */
    int publishDuration;
};

/**
 * Video dimensions.
 */
struct VideoDimensions {
    /** Width (pixels) of the video. */
    int width;
      /** Height (pixels) of the video. */
    int height;
    VideoDimensions()
        : width(640), height(480)
    {}
    VideoDimensions(int w, int h)
        : width(w), height(h)
    {}
};

/** (Recommended) The standard bitrate set in the \ref IRtcEngine::setVideoEncoderConfiguration "setVideoEncoderConfiguration" method.

 In this mode, the bitrates differ between the live interactive streaming and communication profiles:

 - `COMMUNICATION` profile: The video bitrate is the same as the base bitrate.
 - `LIVE_BROADCASTING` profile: The video bitrate is twice the base bitrate.

 */
const int STANDARD_BITRATE = 0;

/** The compatible bitrate set in the \ref IRtcEngine::setVideoEncoderConfiguration "setVideoEncoderConfiguration" method.

 The bitrate remains the same regardless of the channel profile. If you choose this mode in the Live-broadcast profile, the video frame rate may be lower than the set value.
 */
const int COMPATIBLE_BITRATE = -1;

/** Use the default minimum bitrate.
 */
const int DEFAULT_MIN_BITRATE = -1;

/** Video encoder configurations.
 */
struct VideoEncoderConfiguration {
  /** The video frame dimensions (px) used to specify the video quality and measured by the total number of pixels along a frame's width and height: VideoDimensions. The default value is 640 x 360.
  */
    VideoDimensions dimensions;
    /** The frame rate of the video: #FRAME_RATE. The default value is 15.

     Note that we do not recommend setting this to a value greater than 30.
    */
    FRAME_RATE frameRate;
    /** The minimum frame rate of the video. The default value is -1.
     */
    int minFrameRate;
    /** The video encoding bitrate (Kbps).

     Choose one of the following options:

     - #STANDARD_BITRATE: (Recommended) The standard bitrate.
        - the `COMMUNICATION` profile: the encoding bitrate equals the base bitrate.
        - the `LIVE_BROADCASTING` profile: the encoding bitrate is twice the base bitrate.
     - #COMPATIBLE_BITRATE: The compatible bitrate: the bitrate stays the same regardless of the profile.

     the `COMMUNICATION` profile prioritizes smoothness, while the `LIVE_BROADCASTING` profile prioritizes video quality (requiring a higher bitrate). We recommend setting the bitrate mode as #STANDARD_BITRATE to address this difference.

     The following table lists the recommended video encoder configurations, where the base bitrate applies to the `COMMUNICATION` profile. Set your bitrate based on this table. If you set a bitrate beyond the proper range, the SDK automatically sets it to within the range.

     @note
     In the following table, **Base Bitrate** applies to the `COMMUNICATION` profile, and **Live Bitrate** applies to the `LIVE_BROADCASTING` profile.

     | Resolution             | Frame Rate (fps) | Base Bitrate (Kbps)                    | Live Bitrate (Kbps)                    |
     |------------------------|------------------|----------------------------------------|----------------------------------------|
     | 160 * 120              | 15               | 65                                     | 130                                    |
     | 120 * 120              | 15               | 50                                     | 100                                    |
     | 320 * 180              | 15               | 140                                    | 280                                    |
     | 180 * 180              | 15               | 100                                    | 200                                    |
     | 240 * 180              | 15               | 120                                    | 240                                    |
     | 320 * 240              | 15               | 200                                    | 400                                    |
     | 240 * 240              | 15               | 140                                    | 280                                    |
     | 424 * 240              | 15               | 220                                    | 440                                    |
     | 640 * 360              | 15               | 400                                    | 800                                    |
     | 360 * 360              | 15               | 260                                    | 520                                    |
     | 640 * 360              | 30               | 600                                    | 1200                                   |
     | 360 * 360              | 30               | 400                                    | 800                                    |
     | 480 * 360              | 15               | 320                                    | 640                                    |
     | 480 * 360              | 30               | 490                                    | 980                                    |
     | 640 * 480              | 15               | 500                                    | 1000                                   |
     | 480 * 480              | 15               | 400                                    | 800                                    |
     | 640 * 480              | 30               | 750                                    | 1500                                   |
     | 480 * 480              | 30               | 600                                    | 1200                                   |
     | 848 * 480              | 15               | 610                                    | 1220                                   |
     | 848 * 480              | 30               | 930                                    | 1860                                   |
     | 640 * 480              | 10               | 400                                    | 800                                    |
     | 1280 * 720             | 15               | 1130                                   | 2260                                   |
     | 1280 * 720             | 30               | 1710                                   | 3420                                   |
     | 960 * 720              | 15               | 910                                    | 1820                                   |
     | 960 * 720              | 30               | 1380                                   | 2760                                   |
     | 1920 * 1080            | 15               | 2080                                   | 4160                                   |
     | 1920 * 1080            | 30               | 3150                                   | 6300                                   |
     | 1920 * 1080            | 60               | 4780                                   | 6500                                   |
     | 2560 * 1440            | 30               | 4850                                   | 6500                                   |
     | 2560 * 1440            | 60               | 6500                                   | 6500                                   |
     | 3840 * 2160            | 30               | 6500                                   | 6500                                   |
     | 3840 * 2160            | 60               | 6500                                   | 6500                                   |

     */
    int bitrate;
    /** The minimum encoding bitrate (Kbps).

     The SDK automatically adjusts the encoding bitrate to adapt to the network conditions. Using a value greater than the default value forces the video encoder to output high-quality images but may cause more packet loss and hence sacrifice the smoothness of the video transmission. That said, unless you have special requirements for image quality, AR does not recommend changing this value.

     @note This parameter applies only to the `LIVE_BROADCASTING` profile.
     */
    int minBitrate;
    /** The video orientation mode of the video: #ORIENTATION_MODE.
    */
    ORIENTATION_MODE orientationMode;
    /** The video encoding degradation preference under limited bandwidth: #DEGRADATION_PREFERENCE.
     */
    DEGRADATION_PREFERENCE degradationPreference;
    /** Sets the mirror mode of the published local video stream. It only affects the video that the remote user sees. See #VIDEO_MIRROR_MODE_TYPE

    @note: The SDK disables the mirror mode by default.
    */
    VIDEO_MIRROR_MODE_TYPE mirrorMode;

    VideoEncoderConfiguration(
        const VideoDimensions& d, FRAME_RATE f,
        int b, ORIENTATION_MODE m, VIDEO_MIRROR_MODE_TYPE mr = VIDEO_MIRROR_MODE_AUTO)
        : dimensions(d), frameRate(f), minFrameRate(-1), bitrate(b),
          minBitrate(DEFAULT_MIN_BITRATE), orientationMode(m),
          degradationPreference(MAINTAIN_QUALITY), mirrorMode(mr)
    {}
    VideoEncoderConfiguration(
        int width, int height, FRAME_RATE f,
        int b, ORIENTATION_MODE m, VIDEO_MIRROR_MODE_TYPE mr = VIDEO_MIRROR_MODE_AUTO)
        : dimensions(width, height), frameRate(f),
          minFrameRate(-1), bitrate(b),
          minBitrate(DEFAULT_MIN_BITRATE), orientationMode(m),
          degradationPreference(MAINTAIN_QUALITY), mirrorMode(mr)
    {}
    VideoEncoderConfiguration()
        : dimensions(640, 480)
        , frameRate(FRAME_RATE_FPS_15)
        , minFrameRate(-1)
        , bitrate(STANDARD_BITRATE)
        , minBitrate(DEFAULT_MIN_BITRATE)
        , orientationMode(ORIENTATION_MODE_ADAPTIVE)
        , degradationPreference(MAINTAIN_QUALITY)
        , mirrorMode(VIDEO_MIRROR_MODE_AUTO)
    {}
};

/** The video and audio properties of the user displaying the video in the CDN live. AR supports a maximum of 17 transcoding users in a CDN streaming channel.
*/
typedef struct TranscodingUser {
  /** User ID of the user displaying the video in the CDN live.
  */
    uid_t uid;

/** Horizontal position (pixel) of the video frame relative to the top left corner.
*/
    int x;
    /** Vertical position (pixel) of the video frame relative to the top left corner.
    */
    int y;
    /** Width (pixel) of the video frame. The default value is 360.
    */
    int width;
    /** Height (pixel) of the video frame. The default value is 640.
    */
    int height;

    /** The layer index of the video frame. An integer. The value range is [0, 100].

     - 0: (Default) Bottom layer.
     - 100: Top layer.

     @note
     - If zOrder is beyond this range, the SDK reports #ERR_INVALID_ARGUMENT.
     - As of v2.3, the SDK supports zOrder = 0.
     */
    int zOrder;
    /** The transparency level of the user's video. The value ranges between 0 and 1.0:

     - 0: Completely transparent
     - 1.0: (Default) Opaque
     */
    double alpha;
    /** The audio channel of the sound. The default value is 0:

     - 0: (Default) Supports dual channels at most, depending on the upstream of the host.
     - 1: The audio stream of the host uses the FL audio channel. If the upstream of the host uses multiple audio channels, these channels are mixed into mono first.
     - 2: The audio stream of the host uses the FC audio channel. If the upstream of the host uses multiple audio channels, these channels are mixed into mono first.
     - 3: The audio stream of the host uses the FR audio channel. If the upstream of the host uses multiple audio channels, these channels are mixed into mono first.
     - 4: The audio stream of the host uses the BL audio channel. If the upstream of the host uses multiple audio channels, these channels are mixed into mono first.
     - 5: The audio stream of the host uses the BR audio channel. If the upstream of the host uses multiple audio channels, these channels are mixed into mono first.

     @note If your setting is not 0, you may need a specialized player.
     */
    int audioChannel;
    TranscodingUser()
        : uid(0)
        , x(0)
        , y(0)
        , width(0)
        , height(0)
        , zOrder(0)
        , alpha(1.0)
        , audioChannel(0)
    {}

} TranscodingUser;

/** Image properties.

 The properties of the watermark and background images.
 */
typedef struct RtcImage {
    RtcImage() :
       url(NULL),
       x(0),
       y(0),
       width(0),
       height(0)
    {}
    /** HTTP/HTTPS URL address of the image on the live video. The maximum length of this parameter is 1024 bytes. */
    const char* url;
    /** Horizontal position of the image from the upper left of the live video. */
    int x;
    /** Vertical position of the image from the upper left of the live video. */
    int y;
    /** Width of the image on the live video. */
    int width;
    /** Height of the image on the live video. */
    int height;
} RtcImage;
/** The configuration for advanced features of the RTMP streaming with transcoding.
 */
typedef struct LiveStreamAdvancedFeature {
    LiveStreamAdvancedFeature() : featureName(NULL) , opened(false) {
    }

    /** The advanced feature for high-quality video with a lower bitrate. */
    const char* LBHQ = "lbhq";
    /** The advanced feature for the optimized video encoder. */
    const char* VEO = "veo";

    /** The name of the advanced feature. It contains LBHQ and VEO.
     */
    const char* featureName;

    /** Whether to enable the advanced feature:
     * - true: Enable the advanced feature.
     * - false: (Default) Disable the advanced feature.
     */
    bool opened;
} LiveStreamAdvancedFeature;

/** A struct for managing CDN live audio/video transcoding settings.
*/
typedef struct LiveTranscoding {
   /** The width of the video in pixels. The default value is 360.
    * - When pushing video streams to the CDN, ensure that `width` is at least 64; otherwise, the AR server adjusts the value to 64.
    * - When pushing audio streams to the CDN, set `width` and `height` as 0.
    */
    int width;
    /** The height of the video in pixels. The default value is 640.
     * - When pushing video streams to the CDN, ensure that `height` is at least 64; otherwise, the AR server adjusts the value to 64.
     * - When pushing audio streams to the CDN, set `width` and `height` as 0.
    */
    int height;
    /** Bitrate of the CDN live output video stream. The default value is 400 Kbps.

    Set this parameter according to the Video Bitrate Table. If you set a bitrate beyond the proper range, the SDK automatically adapts it to a value within the range.
    */
    int videoBitrate;
    /** Frame rate of the output video stream set for the CDN live streaming. The default value is 15 fps, and the value range is (0,30].

    @note The AR server adjusts any value over 30 to 30.
    */
    int videoFramerate;

    /** **DEPRECATED** Latency mode:

     - true: Low latency with unassured quality.
     - false: (Default) High latency with assured quality.
     */
    bool lowLatency;

    /** Video GOP in frames. The default value is 30 fps.
    */
    int videoGop;
    /** Self-defined video codec profile: #VIDEO_CODEC_PROFILE_TYPE.

	@note If you set this parameter to other values, AR adjusts it to the default value of 100.
    */
    VIDEO_CODEC_PROFILE_TYPE videoCodecProfile;
    /** The background color in RGB hex value. Value only. Do not include a preceeding #. For example, 0xFFB6C1 (light pink). The default value is 0x000000 (black).
     */
    unsigned int backgroundColor;

    /** video codec type */
    VIDEO_CODEC_TYPE_FOR_STREAM videoCodecType;

    /** The number of users in the live interactive streaming.
     */
    unsigned int userCount;
    /** TranscodingUser
    */
    TranscodingUser *transcodingUsers;
    /** Reserved property. Extra user-defined information to send SEI for the H.264/H.265 video stream to the CDN live client. Maximum length: 4096 Bytes.

     For more information on SEI frame, see [SEI-related questions](https://docs.ar.io/en/faq/sei).
     */
    const char *transcodingExtraInfo;

    /** **DEPRECATED** The metadata sent to the CDN live client defined by the RTMP or FLV metadata.
     */
    const char *metadata;
    /** The watermark image added to the CDN live publishing stream.

	Ensure that the format of the image is PNG. Once a watermark image is added, the audience of the CDN live publishing stream can see the watermark image. See RtcImage.
    */
    RtcImage* watermark;
    /** The background image added to the CDN live publishing stream.

     Once a background image is added, the audience of the CDN live publishing stream can see the background image. See RtcImage.
    */
    RtcImage* backgroundImage;
    /** Self-defined audio-sample rate: #AUDIO_SAMPLE_RATE_TYPE.
    */
    AUDIO_SAMPLE_RATE_TYPE audioSampleRate;
    /** Bitrate of the CDN live audio output stream. The default value is 48 Kbps, and the highest value is 128.
     */
    int audioBitrate;
    /** The numbder of audio channels for the CDN live stream. ar recommends choosing 1 (mono), or 2 (stereo) audio channels. Special players are required if you choose option 3, 4, or 5:

     - 1: (Default) Mono.
     - 2: Stereo.
     - 3: Three audio channels.
     - 4: Four audio channels.
     - 5: Five audio channels.
     */
    int audioChannels;
    /** Self-defined audio codec profile: #AUDIO_CODEC_PROFILE_TYPE.
     */

    AUDIO_CODEC_PROFILE_TYPE audioCodecProfile;

    /** Advanced features of the RTMP streaming with transcoding. See LiveStreamAdvancedFeature.
     *
     * @since v3.1.0
     */
    LiveStreamAdvancedFeature* advancedFeatures;

    /** The number of enabled advanced features. The default value is 0. */
    unsigned int advancedFeatureCount;

    LiveTranscoding()
        : width(360)
        , height(640)
        , videoBitrate(400)
        , videoFramerate(15)
        , lowLatency(false)
        , videoGop(30)
        , videoCodecProfile(VIDEO_CODEC_PROFILE_HIGH)
        , backgroundColor(0x000000)
        , videoCodecType(VIDEO_CODEC_H264_FOR_STREAM)
        , userCount(0)
        , transcodingUsers(NULL)
        , transcodingExtraInfo(NULL)
        , metadata(NULL)
        , watermark(NULL)
        , backgroundImage(NULL)
        , audioSampleRate(AUDIO_SAMPLE_RATE_48000)
        , audioBitrate(48)
        , audioChannels(1)
        , audioCodecProfile(AUDIO_CODEC_PROFILE_LC_AAC)
        , advancedFeatures(NULL)
        , advancedFeatureCount(0)
    {}
} LiveTranscoding;

 /** Camera capturer configuration.
  */
 struct CameraCapturerConfiguration{

     /** Camera capturer preference settings. See: #CAPTURER_OUTPUT_PREFERENCE. */
     CAPTURER_OUTPUT_PREFERENCE preference;
     #if defined(__ANDROID__) || (defined(__APPLE__) && TARGET_OS_IOS)
     /** Camera direction settings (for Android/iOS only). See: #CAMERA_DIRECTION. */
     CAMERA_DIRECTION cameraDirection;
     #endif
 };

/** Configuration of the injected media stream.
 */
struct InjectStreamConfig {
    /** Width of the injected stream in the live interactive streaming. The default value is 0 (same width as the original stream).
     */
    int width;
    /** Height of the injected stream in the live interactive streaming. The default value is 0 (same height as the original stream).
     */
    int height;
    /** Video GOP (in frames) of the injected stream in the live interactive streaming. The default value is 30 fps.
     */
    int videoGop;
    /** Video frame rate of the injected stream in the live interactive streaming. The default value is 15 fps.
     */
    int videoFramerate;
    /** Video bitrate of the injected stream in the live interactive streaming. The default value is 400 Kbps.

     @note The setting of the video bitrate is closely linked to the resolution. If the video bitrate you set is beyond a reasonable range, the SDK sets it within a reasonable range.
     */
    int videoBitrate;
    /** Audio-sample rate of the injected stream in the live interactive streaming: #AUDIO_SAMPLE_RATE_TYPE. The default value is 48000 Hz.

     @note We recommend setting the default value.
     */
    AUDIO_SAMPLE_RATE_TYPE audioSampleRate;
    /** Audio bitrate of the injected stream in the live interactive streaming. The default value is 48.

     @note We recommend setting the default value.
     */
    int audioBitrate;
    /** Audio channels in the live interactive streaming.

     - 1: (Default) Mono
     - 2: Two-channel stereo

     @note We recommend setting the default value.
     */
    int audioChannels;

    // width / height default set to 0 means pull the stream with its original resolution
    InjectStreamConfig()
        : width(0)
        , height(0)
        , videoGop(30)
        , videoFramerate(15)
        , videoBitrate(400)
        , audioSampleRate(AUDIO_SAMPLE_RATE_48000)
        , audioBitrate(48)
        , audioChannels(1)
    {}
};
/** The definition of ChannelMediaInfo.
 */
struct ChannelMediaInfo {
    /** The channel name. 
     */
	const char* channelName;
    /** The token that enables the user to join the channel.
     */
	const char* token;
    /** The user ID.
     */
	uid_t uid;
};

/** The definition of ChannelMediaRelayConfiguration.
 */
struct ChannelMediaRelayConfiguration {
    /** Pointer to the information of the source channel: ChannelMediaInfo. It contains the following members:
     * - `channelName`: The name of the source channel. The default value is `NULL`, which means the SDK applies the name of the current channel.
     * - `uid`: ID of the host whose media stream you want to relay. The default value is 0, which means the SDK generates a random UID. You must set it as 0.
     * - `token`: The token for joining the source channel. It is generated with the `channelName` and `uid` you set in `srcInfo`.
     *   - If you have not enabled the App Certificate, set this parameter as the default value `NULL`, which means the SDK applies the App ID.
     *   - If you have enabled the App Certificate, you must use the `token` generated with the `channelName` and `uid`, and the `uid` must be set as 0.
     */
    ChannelMediaInfo *srcInfo;
    /** Pointer to the information of the destination channel: ChannelMediaInfo. It contains the following members:
     * - `channelName`: The name of the destination channel.
     * - `uid`: ID of the host in the destination channel. The value ranges from 0 to (2<sup>32</sup>-1). To avoid UID conflicts, this `uid` must be different from any other UIDs in the destination channel. The default value is 0, which means the SDK generates a random UID.
     * - `token`: The token for joining the destination channel. It is generated with the `channelName` and `uid` you set in `destInfos`.
     *   - If you have not enabled the App Certificate, set this parameter as the default value `NULL`, which means the SDK applies the App ID.
     *   - If you have enabled the App Certificate, you must use the `token` generated with the `channelName` and `uid`.
     */
	ChannelMediaInfo *destInfos;
    /** The number of destination channels. The default value is 0, and the
     * value range is [0,4). Ensure that the value of this parameter
     * corresponds to the number of ChannelMediaInfo structs you define in
     * `destInfos`.
     */
	int destCount;

	ChannelMediaRelayConfiguration()
			: srcInfo(nullptr)
			, destInfos(nullptr)
			, destCount(0)
	{}
};

/**  **DEPRECATED** Lifecycle of the CDN live video stream.
*/
enum RTMP_STREAM_LIFE_CYCLE_TYPE
{
  /** Bind to the channel lifecycle. If all hosts leave the channel, the CDN live streaming stops after 30 seconds.
  */
	RTMP_STREAM_LIFE_CYCLE_BIND2CHANNEL = 1,
  /** Bind to the owner of the RTMP stream. If the owner leaves the channel, the CDN live streaming stops immediately.
  */
	RTMP_STREAM_LIFE_CYCLE_BIND2OWNER = 2,
};

/** Content hints for screen sharing.
*/
enum VideoContentHint
{
    /** (Default) No content hint.
     */
    CONTENT_HINT_NONE,
    /** Motion-intensive content. Choose this option if you prefer smoothness or when you are sharing a video clip, movie, or video game.
     */
    CONTENT_HINT_MOTION,
    /** Motionless content. Choose this option if you prefer sharpness or when you are sharing a picture, PowerPoint slide, or text.
     */
    CONTENT_HINT_DETAILS
};

/** The relative location of the region to the screen or window.
 */
struct Rectangle
{
    /** The horizontal offset from the top-left corner.
    */
    int x;
    /** The vertical offset from the top-left corner.
    */
    int y;
    /** The width of the region.
    */
    int width;
    /** The height of the region.
    */
    int height;

    Rectangle(): x(0), y(0), width(0), height(0) {}
    Rectangle(int xx, int yy, int ww, int hh): x(xx), y(yy), width(ww), height(hh) {}
};

/**  **DEPRECATED** Definition of the rectangular region. */
typedef struct Rect {
    /** Y-axis of the top line.
     */
    int top;
    /** X-axis of the left line.
     */
    int left;
    /** Y-axis of the bottom line.
     */
    int bottom;
    /** X-axis of the right line.
     */
    int right;

    Rect(): top(0), left(0), bottom(0), right(0) {}
    Rect(int t, int l, int b, int r): top(t), left(l), bottom(b), right(r) {}
} Rect;

/** The options of the watermark image to be added. */
typedef struct WatermarkOptions {
    /** Sets whether or not the watermark image is visible in the local video preview: 
     * - true: (Default) The watermark image is visible in preview.
     * - false: The watermark image is not visible in preview. 
     */
    bool visibleInPreview;
    /**
     * The watermark position in the landscape mode. See Rectangle.
     * For detailed information on the landscape mode, see the advanced guide *Video Rotation*.
     */
    Rectangle positionInLandscapeMode;
    /**
     * The watermark position in the portrait mode. See Rectangle.
     * For detailed information on the portrait mode, see the advanced guide *Video Rotation*.
     */
    Rectangle positionInPortraitMode;

    WatermarkOptions()
        : visibleInPreview(true)
        , positionInLandscapeMode(0, 0, 0, 0)
        , positionInPortraitMode(0, 0, 0, 0)
    {}
} WatermarkOptions;

/** Screen sharing encoding parameters.
*/
struct ScreenCaptureParameters
{
    /** The maximum encoding dimensions of the shared region in terms of width * height.

     The default value is 1920 * 1080 pixels, that is, 2073600 pixels. AR uses the value of this parameter to calculate the charges.

     If the aspect ratio is different between the encoding dimensions and screen dimensions, AR applies the following algorithms for encoding. Suppose the encoding dimensions are 1920 x 1080:

	 - If the value of the screen dimensions is lower than that of the encoding dimensions, for example, 1000 &times; 1000, the SDK uses 1000 &times; 1000 for encoding.
	 - If the value of the screen dimensions is higher than that of the encoding dimensions, for example, 2000 &times; 1500, the SDK uses the maximum value under 1920 &times; 1080 with the aspect ratio of the screen dimension (4:3) for encoding, that is, 1440 &times; 1080.
     */
    VideoDimensions dimensions;
    /** The frame rate (fps) of the shared region.

	The default value is 5. We do not recommend setting this to a value greater than 15.
     */
    int frameRate;
    /** The bitrate (Kbps) of the shared region.

	The default value is 0 (the SDK works out a bitrate according to the dimensions of the current screen).
     */
    int bitrate;
    /** Sets whether or not to capture the mouse for screen sharing:

	- true: (Default) Capture the mouse.
	- false: Do not capture the mouse.
     */
    bool captureMouseCursor;
	
	/** Whether to bring the window to the front when calling \ref IRtcEngine::startScreenCaptureByWindowId "startScreenCaptureByWindowId" to share the window:
     * - true: Bring the window to the front.
     * - false: (Default) Do not bring the window to the front.
     */
    bool windowFocus;
    /** A list of IDs of windows to be blocked.
     *
     * When calling \ref IRtcEngine::startScreenCaptureByScreenRect "startScreenCaptureByScreenRect" to start screen sharing, you can use this parameter to block the specified windows.
     * When calling \ref IRtcEngine::updateScreenCaptureParameters "updateScreenCaptureParameters" to update the configuration for screen sharing, you can use this parameter to dynamically block the specified windows during screen sharing.
     */
    view_t* excludeWindowList;
    /** The number of windows to be blocked.
     */
    int excludeWindowCount;
	
	/** Sets whether or not to capture the speaker audio for screen sharing:

	- true: Capture the speaker audio.
	- false: (Default) Do not capture the speaker audio.
	 */
	bool captureAudio;

    ScreenCaptureParameters() : dimensions(1920, 1080), frameRate(5), bitrate(STANDARD_BITRATE), captureMouseCursor(true), windowFocus(false), excludeWindowList(NULL), excludeWindowCount(0) {}
    ScreenCaptureParameters(const VideoDimensions& d, int f, int b, bool c, bool focus, view_t *ex = NULL, int cnt = 0) : dimensions(d), frameRate(f), bitrate(b), captureMouseCursor(c), windowFocus(focus), excludeWindowList(ex), excludeWindowCount(cnt) {}
    ScreenCaptureParameters(int width, int height, int f, int b, bool c, bool focus, view_t *ex = NULL, int cnt = 0) : dimensions(width, height), frameRate(f), bitrate(b), captureMouseCursor(c), windowFocus(focus), excludeWindowList(ex), excludeWindowCount(cnt) {}
};

/** Video display settings of the VideoCanvas class.
*/
struct VideoCanvas
{
    /** Video display window (view).
     */
    view_t view;
    /** The rendering mode of the video view. See RENDER_MODE_TYPE
     */
    int renderMode;
    /** The unique channel name for the ARRTC session in the string format. The string length must be less than 64 bytes. Supported character scopes are:
     - All lowercase English letters: a to z. 
     - All uppercase English letters: A to Z. 
     - All numeric characters: 0 to 9. 
     - The space character. 
     - Punctuation characters and other symbols, including: "!", "#", "$", "%", "&", "(", ")", "+", "-", ":", ";", "<", "=", ".", ">", "?", "@", "[", "]", "^", "_", " {", "}", "|", "~", ",".

     @note 
     - The default value is the empty string "". Use the default value if the user joins the channel using the \ref IRtcEngine::joinChannel "joinChannel" method in the IRtcEngine class. The `VideoCanvas` struct defines the video canvas of the user in the channel.
     - If the user joins the channel using the \ref IRtcEngine::joinChannel "joinChannel" method in the IChannel class, set this parameter as the `channelId` of the `IChannel` object. The `VideoCanvas` struct defines the video canvas of the user in the channel with the specified channel ID.
     */
    char channelId[MAX_CHANNEL_ID_LENGTH];
    /** The user ID. */
    uid_t uid;
    void *priv; // private data (underlying video engine denotes it)
    /** The mirror mode of the video view. See VIDEO_MIRROR_MODE_TYPE
     @note
     - For the mirror mode of the local video view: If you use a front camera, the SDK enables the mirror mode by default; if you use a rear camera, the SDK disables the mirror mode by default.
     - For the mirror mode of the remote video view: The SDK disables the mirror mode by default.
    */
    VIDEO_MIRROR_MODE_TYPE mirrorMode;

    VideoCanvas()
        : view(NULL)
        , renderMode(RENDER_MODE_HIDDEN)
        , uid(0)
        , priv(NULL)
        , mirrorMode(VIDEO_MIRROR_MODE_AUTO)
    {
        channelId[0] = '\0';
    }
    VideoCanvas(view_t v, int m, uid_t u)
        : view(v)
        , renderMode(m)
        , uid(u)
        , priv(NULL)
        , mirrorMode(VIDEO_MIRROR_MODE_AUTO)
    {
        channelId[0] = '\0';
    }
    VideoCanvas(view_t v, int m, const char *ch, uid_t u)
        : view(v)
        , renderMode(m)
        , uid(u)
        , priv(NULL)
        , mirrorMode(VIDEO_MIRROR_MODE_AUTO)
    {
        strncpy(channelId, ch, MAX_CHANNEL_ID_LENGTH);
        channelId[MAX_CHANNEL_ID_LENGTH - 1] = '\0';
    }
    VideoCanvas(view_t v, int rm, uid_t u, VIDEO_MIRROR_MODE_TYPE mm)
        : view(v)
        , renderMode(rm)
        , uid(u)
        , priv(NULL)
        , mirrorMode(mm)
    {
        channelId[0] = '\0';
    }
    VideoCanvas(view_t v, int rm, const char *ch, uid_t u, VIDEO_MIRROR_MODE_TYPE mm)
        : view(v)
        , renderMode(rm)
        , uid(u)
        , priv(NULL)
        , mirrorMode(mm)
    {
        strncpy(channelId, ch, MAX_CHANNEL_ID_LENGTH);
        channelId[MAX_CHANNEL_ID_LENGTH - 1] = '\0';
    }
};

/** Image enhancement options.
*/
struct BeautyOptions {
    /** The contrast level, used with the @p lightening parameter.
    */
    enum LIGHTENING_CONTRAST_LEVEL
    {
        /** Low contrast level. */
        LIGHTENING_CONTRAST_LOW = 0,
        /** (Default) Normal contrast level. */
        LIGHTENING_CONTRAST_NORMAL,
        /** High contrast level. */
        LIGHTENING_CONTRAST_HIGH
    };

/** The contrast level, used with the @p lightening parameter.
*/
LIGHTENING_CONTRAST_LEVEL lighteningContrastLevel;

/** The brightness level. The value ranges from 0.0 (original) to 1.0. */
float lighteningLevel;

/** The sharpness level. The value ranges between 0 (original) and 1. This parameter is usually used to remove blemishes.
 */
float smoothnessLevel;

/** The redness level. The value ranges between 0 (original) and 1. This parameter adjusts the red saturation level.
*/
float rednessLevel;

BeautyOptions(LIGHTENING_CONTRAST_LEVEL contrastLevel, float lightening, float smoothness, float redness)
    : lighteningLevel(lightening),
    smoothnessLevel(smoothness),
    rednessLevel(redness),
    lighteningContrastLevel(contrastLevel) {}

BeautyOptions()
    : lighteningLevel(0),
    smoothnessLevel(0),
    rednessLevel(0),
    lighteningContrastLevel(LIGHTENING_CONTRAST_NORMAL) {}
};

/**
 * The UserInfo struct.
 */
struct UserInfo {
  /**
   * The user ID.
   */
  uid_t uid;
  /**
   * The user account.
   */
  char userAccount[MAX_USER_ACCOUNT_LENGTH];
  UserInfo()
      : uid(0) {
    userAccount[0] = '\0';
  }
};

/**
 *  Regions for connetion.
 */
enum AREA_CODE {
    /**
     * Mainland China.
     */
    AREA_CODE_CN = 0x00000001,
    /**
     * North America.
     */
    AREA_CODE_NA = 0x00000002,
    /**
     * Europe.
     */
    AREA_CODE_EU = 0x00000004,
    /**
     * Asia, excluding Mainland China.
     */
    AREA_CODE_AS = 0x00000008,
    /**
     * Japan.
     */
    AREA_CODE_JP = 0x00000010,
    /**
     * India.
     */
    AREA_CODE_IN = 0x00000020,
    /**
     * (Default) Global.
     */
    AREA_CODE_GLOB = 0xFFFFFFFF
};

enum ENCRYPTION_CONFIG {
    /**
     * - 1: Force set master key and mode;
     * - 0: Not force set, checking whether encryption plugin exists
     */
    ENCRYPTION_FORCE_SETTING = (1 << 0),
    /**
     * - 1: Force not encrypting packet;
     * - 0: Not force encrypting;
     */
    ENCRYPTION_FORCE_DISABLE_PACKET = (1 << 1)
};
/** Definition of IPacketObserver.
*/
class IPacketObserver
{
public:
/** Definition of Packet.
 */
	struct Packet
	{
        /** Buffer address of the sent or received data.
         * @note AR recommends that the value of buffer is more than 2048 bytes, otherwise, you may meet undefined behaviors such as a crash.
         */
		const unsigned char* buffer;
        /** Buffer size of the sent or received data.
         */
		unsigned int size;
	};
	/** Occurs when the local user sends an audio packet.

     @param packet The sent audio packet. See Packet.
     @return
     - true: The audio packet is sent successfully.
     - false: The audio packet is discarded.
     */
	virtual bool onSendAudioPacket(Packet& packet) = 0;
	/** Occurs when the local user sends a video packet.

     @param packet The sent video packet. See Packet.
     @return
     - true: The video packet is sent successfully.
     - false: The video packet is discarded.
     */
	virtual bool onSendVideoPacket(Packet& packet) = 0;
	/** Occurs when the local user receives an audio packet.

     @param packet The received audio packet. See Packet.
     @return
     - true: The audio packet is received successfully.
     - false: The audio packet is discarded.
	 */
	virtual bool onReceiveAudioPacket(Packet& packet) = 0;
	/** Occurs when the local user receives a video packet.

     @param packet The received video packet. See Packet.
     @return
     - true: The video packet is received successfully.
     - false: The video packet is discarded.
     */
    virtual bool onReceiveVideoPacket(Packet& packet) = 0;
};


#if defined(_WIN32)
/** The capture type of the custom video source.
 */
enum VIDEO_CAPTURE_TYPE {
    /** Unknown type.
     */
    VIDEO_CAPTURE_UNKNOWN,
    /** (Default) Video captured by the camera.
     */
    VIDEO_CAPTURE_CAMERA,
    /** Video for screen sharing.
     */
    VIDEO_CAPTURE_SCREEN,
};

/** The IVideoFrameConsumer class. The SDK uses it to receive the video frame that you capture.
 */
class IVideoFrameConsumer {
public:
    /** Receives the raw video frame.
     *
     * @note Ensure that the video frame type that you specify in this method is the same as that in the \ref ar::rtc::IVideoSource::getBufferType "getBufferType" callback.
     *
     * @param buffer The video buffer.
     * @param frameType The video frame type. See \ref AM::ExternalVideoFrame::VIDEO_PIXEL_FORMAT "VIDEO_PIXEL_FORMAT".
     * @param width The width (px) of the video frame.
     * @param height The height (px) of the video frame.
     * @param rotation The angle (degree) at which the video frame rotates clockwise. If you set the rotation angle, the
     * SDK rotates the video frame after receiving it. You can set the rotation angle as `0`, `90`, `180`, and `270`.
     * @param timestamp The Unix timestamp (ms) of the video frame. You must set a timestamp for each video frame.
     */
    virtual void consumeRawVideoFrame(const unsigned char *buffer, AM::ExternalVideoFrame::VIDEO_PIXEL_FORMAT frameType, int width, int height, int rotation, long timestamp) = 0;
};

/** The IVideoSource class. You can use it to customize the video source.
 */
class IVideoSource {
public:
    /** Notification for initializing the custom video source.
     *
     * The SDK triggers this callback to remind you to initialize the custom video source. After receiving this callback,
     * you can do some preparation, such as enabling the camera, and then use the return value to tell the SDK whether the
     * custom video source is prepared.
     *
     * @param consumer An IVideoFrameConsumer object that the SDK passes to you. You need to reserve this object and use it
     * to send the video frame to the SDK once the custom video source is started. See IVideoFrameConsumer.
     *
     * @return
     * - true: The custom video source is initialized.
     * - false: The custom video source is not ready or fails to initialize. The SDK stops and reports the error.
     */
    virtual bool onInitialize(IVideoFrameConsumer *consumer) = 0;

    /** Notification for disabling the custom video source.
     *
     * The SDK triggers this callback to remind you to disable the custom video source device. This callback tells you
     * that the SDK is about to release the IVideoFrameConsumer object. Ensure that you no longer use IVideoFrameConsumer
     * after receiving this callback.
     */
    virtual void onDispose() = 0;

    /** Notification for starting the custom video source.
     *
     * The SDK triggers this callback to remind you to start the custom video source for capturing video. The SDK uses
     * IVideoFrameConsumer to receive the video frame that you capture after the video source is started. You must use
     * the return value to tell the SDK whether the custom video source is started.
     *
     * @return
     * - true: The custom video source is started.
     * - false: The custom video source fails to start. The SDK stops and reports the error.
     */
    virtual bool onStart() = 0;

    /** Notification for stopping capturing video.
     *
     * The SDK triggers this callback to remind you to stop capturing video. This callback tells you that the SDK is about
     * to stop using IVideoFrameConsumer to receive the video frame that you capture.
     */
    virtual void onStop() = 0;

    /** Gets the video frame type.
     *
     * Before you initialize the custom video source, the SDK triggers this callback to query the video frame type. You
     * must specify the video frame type in the return value and then pass it to the SDK.
     *
     * @note Ensure that the video frame type that you specify in this callback is the same as that in the \ref ar::rtc::IVideoFrameConsumer::consumeRawVideoFrame "consumeRawVideoFrame" method.
     *
     * @return \ref AM::ExternalVideoFrame::VIDEO_PIXEL_FORMAT "VIDEO_PIXEL_FORMAT"
     */
    virtual AM::ExternalVideoFrame::VIDEO_PIXEL_FORMAT getBufferType() = 0;
    /** Gets the capture type of the custom video source.
     *
     * Before you initialize the custom video source, the SDK triggers this callback to query the capture type of the video source.
     * You must specify the capture type in the return value and then pass it to the SDK. The SDK enables the corresponding video
     * processing algorithm according to the capture type after receiving the video frame.
     *
     * @return #VIDEO_CAPTURE_TYPE
     */
    virtual VIDEO_CAPTURE_TYPE getVideoCaptureType() = 0;
    /** Gets the content hint of the custom video source.
     *
     * If you specify the custom video source as a screen-sharing video, the SDK triggers this callback to query the
     * content hint of the video source before you initialize the video source. You must specify the content hint in the
     * return value and then pass it to the SDK. The SDK enables the corresponding video processing algorithm according
     * to the content hint after receiving the video frame.
     *
     * @return \ref ar::rtc::VideoContentHint "VideoContentHint"
     */
    virtual VideoContentHint getVideoContentHint() = 0;
};
#endif

/** The SDK uses the IRtcEngineEventHandler interface class to send callbacks to the application. The application inherits the methods of this interface class to retrieve these callbacks.

 All methods in this interface class have default (empty) implementations. Therefore, the application can only inherit some required events. In the callbacks, avoid time-consuming tasks or calling blocking APIs, such as the SendMessage method. Otherwise, the SDK may not work properly.
 */
class IRtcEngineEventHandler
{
public:
    virtual ~IRtcEngineEventHandler() {}
    
    /** Reports a warning during SDK runtime.

     In most cases, the application can ignore the warning reported by the SDK because the SDK can usually fix the issue and resume running. For example, when losing connection with the server, the SDK may report #WARN_LOOKUP_CHANNEL_TIMEOUT and automatically try to reconnect.

     @param warn Warning code: #WARN_CODE_TYPE.
     @param msg Pointer to the warning message.
     */
    virtual void onWarning(int warn, const char* msg) {
        (void)warn;
        (void)msg;
    }

    /** Reports an error during SDK runtime.

     In most cases, the SDK cannot fix the issue and resume running. The SDK requires the application to take action or informs the user about the issue.

     For example, the SDK reports an #ERR_START_CALL error when failing to initialize a call. The application informs the user that the call initialization failed and invokes the \ref IRtcEngine::leaveChannel "leaveChannel" method to leave the channel.

     @param err Error code: #ERROR_CODE_TYPE.
     @param msg Pointer to the error message.
     */
    virtual void onError(int err, const char* msg) {
        (void)err;
        (void)msg;
    }

    /** Occurs when a user joins a channel.

     This callback notifies the application that a user joins a specified channel when the application calls the \ref IRtcEngine::joinChannel "joinChannel" method.

     The channel name assignment is based on @p channelName specified in the \ref IRtcEngine::joinChannel "joinChannel" method.

     If the @p uid is not specified in the *joinChannel* method, the server automatically assigns a @p uid.

     @param channel  Pointer to the channel name.
     @param  uid User ID of the user joining the channel.
     @param  elapsed Time elapsed (ms) from the user calling the \ref IRtcEngine::joinChannel "joinChannel" method until the SDK triggers this callback.
     */
    virtual void onJoinChannelSuccess(const char* channel, uid_t uid, int elapsed) {
        (void)channel;
        (void)uid;
        (void)elapsed;
    }

    /** Occurs when a user rejoins the channel after disconnection due to network problems.

    When a user loses connection with the server because of network problems, the SDK automatically tries to reconnect and triggers this callback upon reconnection.

     @param channel Pointer to the channel name.
     @param uid User ID of the user rejoining the channel.
     @param elapsed Time elapsed (ms) from starting to reconnect until the SDK triggers this callback.
     */
    virtual void onRejoinChannelSuccess(const char* channel, uid_t uid, int elapsed) {
        (void)channel;
        (void)uid;
        (void)elapsed;
    }

    /** Occurs when a user leaves the channel.

    This callback notifies the application that a user leaves the channel when the application calls the \ref IRtcEngine::leaveChannel "leaveChannel" method.

    The application retrieves information, such as the call duration and statistics.

     @param stats Pointer to the statistics of the call: RtcStats.
     */
    virtual void onLeaveChannel(const RtcStats& stats) {
        (void)stats;
    }

    /** Occurs when the user role switches in a live broadcast. For example, from a host to an audience or vice versa.

    This callback notifies the application of a user role switch when the application calls the \ref IRtcEngine::setClientRole "setClientRole" method.

    The SDK triggers this callback when the local user switches the user role by calling the \ref ar::rtc::IRtcEngine::setClientRole "setClientRole" method after joining the channel.
     @param oldRole Role that the user switches from: #CLIENT_ROLE_TYPE.
     @param newRole Role that the user switches to: #CLIENT_ROLE_TYPE.
     */
    virtual void onClientRoleChanged(CLIENT_ROLE_TYPE oldRole, CLIENT_ROLE_TYPE newRole) {
    }

    /** Occurs when a remote user (Communication)/ host (Live Broadcast) joins the channel.

     - Communication profile: This callback notifies the application that another user joins the channel. If other users are already in the channel, the SDK also reports to the application on the existing users.
     - Live-broadcast profile: This callback notifies the application that the host joins the channel. If other hosts are already in the channel, the SDK also reports to the application on the existing hosts. We recommend limiting the number of hosts to 17.

     The SDK triggers this callback under one of the following circumstances:
     - A remote user/host joins the channel by calling the \ref ar::rtc::IRtcEngine::joinChannel "joinChannel" method.
     - A remote user switches the user role to the host by calling the \ref ar::rtc::IRtcEngine::setClientRole "setClientRole" method after joining the channel.
     - A remote user/host rejoins the channel after a network interruption.
     - The host injects an online media stream into the channel by calling the \ref ar::rtc::IRtcEngine::addInjectStreamUrl "addInjectStreamUrl" method.

     @note In the Live-broadcast profile:
     - The host receives this callback when another host joins the channel.
     - The audience in the channel receives this callback when a new host joins the channel.
     - When a web application joins the channel, the SDK triggers this callback as long as the web application publishes streams.

     @param uid User ID of the user or host joining the channel.
     @param elapsed Time delay (ms) from the local user calling the \ref IRtcEngine::joinChannel "joinChannel" method until the SDK triggers this callback.
     */
    virtual void onUserJoined(uid_t uid, int elapsed) {
        (void)uid;
        (void)elapsed;
    }

    /** Occurs when a remote user (Communication)/host (Live Broadcast) leaves the channel.

    Reasons why the user is offline:

    - Leave the channel: When the user/host leaves the channel, the user/host sends a goodbye message. When the message is received, the SDK assumes that the user/host leaves the channel.
    - Drop offline: When no data packet of the user or host is received for a certain period of time, the SDK assumes that the user/host drops offline. Unreliable network connections may lead to false detections, so we recommend using a signaling system for more reliable offline detection.

     @param uid User ID of the user leaving the channel or going offline.
     @param reason Reason why the user is offline: #USER_OFFLINE_REASON_TYPE.
     */
    virtual void onUserOffline(uid_t uid, USER_OFFLINE_REASON_TYPE reason) {
        (void)uid;
        (void)reason;
    }

    /** Reports the last mile network quality of the local user once every two seconds before the user joins the channel.

     Last mile refers to the connection between the local device and AR's edge server. After the application calls the \ref IRtcEngine::enableLastmileTest "enableLastmileTest" method, this callback reports once every two seconds the uplink and downlink last mile network conditions of the local user before the user joins the channel.

     @param quality The last mile network quality: #QUALITY_TYPE.
     */
    virtual void onLastmileQuality(int quality) {
        (void)quality;
    }

    /** Reports the last-mile network probe result.

    The SDK triggers this callback within 30 seconds after the app calls the \ref ar::rtc::IRtcEngine::startLastmileProbeTest "startLastmileProbeTest" method.

    @param result The uplink and downlink last-mile network probe test result. See LastmileProbeResult.
    */
    virtual void onLastmileProbeResult(const LastmileProbeResult& result) {
        (void)result;
    }

    /** **DEPRECATED** Occurs when the connection between the SDK and the server is interrupted.

     Deprecated as of v2.3.2. Replaced by the \ref ar::rtc::IRtcEngineEventHandler::onConnectionStateChanged "onConnectionStateChanged(CONNECTION_STATE_RECONNECTING, CONNECTION_CHANGED_INTERRUPTED)" callback.

     The SDK triggers this callback when it loses connection with the server for more than four seconds after the connection is established.

     After triggering this callback, the SDK tries reconnecting to the server. You can use this callback to implement pop-up reminders.

     This callback is different from \ref ar::rtc::IRtcEngineEventHandler::onConnectionLost "onConnectionLost":
     - The SDK triggers the \ref ar::rtc::IRtcEngineEventHandler::onConnectionInterrupted "onConnectionInterrupted" callback when it loses connection with the server for more than four seconds after it successfully joins the channel.
     - The SDK triggers the \ref ar::rtc::IRtcEngineEventHandler::onConnectionLost "onConnectionLost" callback when it loses connection with the server for more than 10 seconds, whether or not it joins the channel.

     If the SDK fails to rejoin the channel 20 minutes after being disconnected from AR's edge server, the SDK stops rejoining the channel.

    */
    virtual void onConnectionInterrupted() {}

    /** Occurs when the SDK cannot reconnect to AR's edge server 10 seconds after its connection to the server is interrupted.

    The SDK triggers this callback when it cannot connect to the server 10 seconds after calling the \ref IRtcEngine::joinChannel "joinChannel" method, whether or not it is in the channel.

    This callback is different from \ref ar::rtc::IRtcEngineEventHandler::onConnectionInterrupted "onConnectionInterrupted":

    - The SDK triggers the \ref ar::rtc::IRtcEngineEventHandler::onConnectionInterrupted "onConnectionInterrupted" callback when it loses connection with the server for more than four seconds after it successfully joins the channel.
    - The SDK triggers the \ref ar::rtc::IRtcEngineEventHandler::onConnectionLost "onConnectionLost" callback when it loses connection with the server for more than 10 seconds, whether or not it joins the channel.

    If the SDK fails to rejoin the channel 20 minutes after being disconnected from AR's edge server, the SDK stops rejoining the channel.

     */
    virtual void onConnectionLost() {}

    /** **DEPRECATED** Deprecated as of v2.3.2. Replaced by the \ref ar::rtc::IRtcEngineEventHandler::onConnectionStateChanged "onConnectionStateChanged(CONNECTION_STATE_FAILED, CONNECTION_CHANGED_BANNED_BY_SERVER)" callback.

    Occurs when your connection is banned by the AR Server.
     */
    virtual void onConnectionBanned() {}

    /** Occurs when a method is executed by the SDK.

     @param err The error code (#ERROR_CODE_TYPE) returned by the SDK when a method call fails. If the SDK returns 0, then the method call is successful.
     @param api Pointer to the method executed by the SDK.
     @param result Pointer to the result of the method call.
     */
    virtual void onApiCallExecuted(int err, const char* api, const char* result) {
        (void)err;
        (void)api;
        (void)result;
    }

    /** Occurs when the token expires.

     After a token is specified by calling the \ref IRtcEngine::joinChannel "joinChannel" method, if the SDK losses connection with the AR server due to network issues, the token may expire after a certain period of time and a new token may be required to reconnect to the server.

     This callback notifies the application to generate a new token. Call the \ref IRtcEngine::renewToken "renewToken" method to renew the token.
     */
    virtual void onRequestToken() {
    }

    /** Occurs when the token expires in 30 seconds.

     The user becomes offline if the token used in the \ref IRtcEngine::joinChannel "joinChannel" method expires. The SDK triggers this callback 30 seconds before the token expires to remind the application to get a new token. Upon receiving this callback, generate a new token on the server and call the \ref IRtcEngine::renewToken "renewToken" method to pass the new token to the SDK.

     @param token Pointer to the token that expires in 30 seconds.
     */
    virtual void onTokenPrivilegeWillExpire(const char* token) {
        (void)token;
    }

    /** **DEPRECATED** Reports the statistics of the audio stream from each remote user/host.

    Deprecated as of v2.3.2. Use the \ref ar::rtc::IRtcEngineEventHandler::onRemoteAudioStats "onRemoteAudioStats" callback instead.

     The SDK triggers this callback once every two seconds to report the audio quality of each remote user/host sending an audio stream. If a channel has multiple users/hosts sending audio streams, the SDK triggers this callback as many times.

     @param uid User ID of the speaker.
     @param quality Audio quality of the user: #QUALITY_TYPE.
     @param delay Time delay (ms) of sending the audio packet from the sender to the receiver, including the time delay of audio sampling pre-processing, transmission, and the jitter buffer.
     @param lost Packet loss rate (%) of the audio packet sent from the sender to the receiver.
     */
    virtual void onAudioQuality(uid_t uid, int quality, unsigned short delay, unsigned short lost) {
        (void)uid;
        (void)quality;
        (void)delay;
        (void)lost;
    }

    /** Reports the statistics of the current call. 
    
     The SDK triggers this callback once every two seconds after the user joins the channel.
     
     @param stats Statistics of the RtcEngine: RtcStats.
     */
    virtual void onRtcStats(const RtcStats& stats) {
        (void)stats;
    }

    /** Reports the last mile network quality of each user in the channel once every two seconds.

     Last mile refers to the connection between the local device and AR's edge server. This callback reports once every two seconds the last mile network conditions of each user in the channel. If a channel includes multiple users, the SDK triggers this callback as many times.

     @param uid User ID. The network quality of the user with this @p uid is reported. If @p uid is 0, the local network quality is reported.
     @param txQuality Uplink transmission quality rating of the user in terms of the transmission bitrate, packet loss rate, average RTT (Round-Trip Time), and jitter of the uplink network. @p txQuality is a quality rating helping you understand how well the current uplink network conditions can support the selected VideoEncoderConfiguration. For example, a 1000 Kbps uplink network may be adequate for video frames with a resolution of 640 &times; 480 and a frame rate of 15 fps in the Live-broadcast profile, but may be inadequate for resolutions higher than 1280 &times; 720. See #QUALITY_TYPE.
     @param rxQuality Downlink network quality rating of the user in terms of the packet loss rate, average RTT, and jitter of the downlink network. See #QUALITY_TYPE.
     */
    virtual void onNetworkQuality(uid_t uid, int txQuality, int rxQuality) {
    (void)uid;
    (void)txQuality;
    (void)rxQuality;
    }

    /** Reports the statistics of the local video stream.
     *
     * The SDK triggers this callback once every two seconds for each
     * user/host. If there are multiple users/hosts in the channel, the SDK
     * triggers this callback as many times.
     *
     * @note
     * If you have called the \ref ar::rtc::IRtcEngine::enableDualStream
     * "enableDualStream" method, the \ref onLocalVideoStats()
     * "onLocalVideoStats" callback reports the statistics of the high-video
     * stream (high bitrate, and high-resolution video stream).
     *
     * @param stats Statistics of the local video stream. See LocalVideoStats.
     */
  virtual void onLocalVideoStats(const LocalVideoStats& stats) {
    (void)stats;
    }

    /** Reports the statistics of the video stream from each remote user/host.
     *
     * The SDK triggers this callback once every two seconds for each remote
     * user/host. If a channel includes multiple remote users, the SDK
     * triggers this callback as many times.
     *
     * @param stats Statistics of the remote video stream. See
     * RemoteVideoStats.
     */
    virtual void onRemoteVideoStats(const RemoteVideoStats& stats) {
      (void)stats;
      }

    /** Reports the statistics of the local audio stream.
     *
     * The SDK triggers this callback once every two seconds.
     *
     * @param stats The statistics of the local audio stream.
     * See LocalAudioStats.
     */
    virtual void onLocalAudioStats(const LocalAudioStats& stats) {
        (void)stats;
    }

    /** Reports the statistics of the audio stream from each remote user/host.

     This callback replaces the \ref ar::rtc::IRtcEngineEventHandler::onAudioQuality "onAudioQuality" callback.

     The SDK triggers this callback once every two seconds for each remote user/host. If a channel includes multiple remote users, the SDK triggers this callback as many times.

     @param stats Pointer to the statistics of the received remote audio streams. See RemoteAudioStats.
     */
    virtual void onRemoteAudioStats(const RemoteAudioStats& stats) {
        (void)stats;
    }

    /** Occurs when the local audio state changes.
     *
     * This callback indicates the state change of the local audio stream,
     * including the state of the audio recording and encoding, and allows
     * you to troubleshoot issues when exceptions occur.
     *
     * @note
     * When the state is #LOCAL_AUDIO_STREAM_STATE_FAILED (3), see the `error`
     * parameter for details.
     *
     * @param state State of the local audio. See #LOCAL_AUDIO_STREAM_STATE.
     * @param error The error information of the local audio.
     * See #LOCAL_AUDIO_STREAM_ERROR.
     */
    virtual void onLocalAudioStateChanged(LOCAL_AUDIO_STREAM_STATE state, LOCAL_AUDIO_STREAM_ERROR error) {
        (void)state;
        (void)error;
    }

    /** Occurs when the remote audio state changes.
     *
     * This callback indicates the state change of the remote audio stream.
     *
     * @param uid ID of the remote user whose audio state changes.
     * @param state State of the remote audio. See #REMOTE_AUDIO_STATE.
     * @param reason The reason of the remote audio state change.
     * See #REMOTE_AUDIO_STATE_REASON.
     * @param elapsed Time elapsed (ms) from the local user calling the
     * \ref IRtcEngine::joinChannel "joinChannel" method until the SDK
     * triggers this callback.
     */
    virtual void onRemoteAudioStateChanged(uid_t uid, REMOTE_AUDIO_STATE state, REMOTE_AUDIO_STATE_REASON reason, int elapsed) {
        (void)uid;
        (void)state;
        (void)reason;
        (void)elapsed;
    }

    /** Occurs when the audio publishing state changes.
     *
     * @since v3.1.0
     *
     * This callback indicates the publishing state change of the local audio stream.
     *
     * @param channel The channel name.
     * @param oldState The previous publishing state. For details, see #STREAM_PUBLISH_STATE.
     * @param newState The current publishing state. For details, see #STREAM_PUBLISH_STATE.
     * @param elapseSinceLastState The time elapsed (ms) from the previous state to the current state.
     */
    virtual void onAudioPublishStateChanged(const char* channel, STREAM_PUBLISH_STATE oldState, STREAM_PUBLISH_STATE newState, int elapseSinceLastState) {
        (void)channel;
        (void)oldState;
        (void)newState;
        (void)elapseSinceLastState;
    }

    /** Occurs when the video publishing state changes.
     *
     * @since v3.1.0
     *
     * This callback indicates the publishing state change of the local video stream.
     *
     * @param channel The channel name.
     * @param oldState The previous publishing state. For details, see #STREAM_PUBLISH_STATE.
     * @param newState The current publishing state. For details, see #STREAM_PUBLISH_STATE.
     * @param elapseSinceLastState The time elapsed (ms) from the previous state to the current state.
     */
    virtual void onVideoPublishStateChanged(const char* channel, STREAM_PUBLISH_STATE oldState, STREAM_PUBLISH_STATE newState, int elapseSinceLastState) {
        (void)channel;
        (void)oldState;
        (void)newState;
        (void)elapseSinceLastState;
    }

    /** Occurs when the audio subscribing state changes.
     *
     * @since v3.1.0
     *
     * This callback indicates the subscribing state change of a remote audio stream.
     *
     * @param channel The channel name.
     * @param uid The ID of the remote user.
     * @param oldState The previous subscribing state. For details, see #STREAM_SUBSCRIBE_STATE.
     * @param newState The current subscribing state. For details, see #STREAM_SUBSCRIBE_STATE.
     * @param elapseSinceLastState The time elapsed (ms) from the previous state to the current state.
     */
    virtual void onAudioSubscribeStateChanged(const char* channel, uid_t uid, STREAM_SUBSCRIBE_STATE oldState, STREAM_SUBSCRIBE_STATE newState, int elapseSinceLastState) {
        (void)channel;
        (void)uid;
        (void)oldState;
        (void)newState;
        (void)elapseSinceLastState;
    }

    /** Occurs when the audio subscribing state changes.
     *
     * @since v3.1.0
     *
     * This callback indicates the subscribing state change of a remote video stream.
     *
     * @param channel The channel name.
     * @param uid The ID of the remote user.
     * @param oldState The previous subscribing state. For details, see #STREAM_SUBSCRIBE_STATE.
     * @param newState The current subscribing state. For details, see #STREAM_SUBSCRIBE_STATE.
     * @param elapseSinceLastState The time elapsed (ms) from the previous state to the current state.
     */
    virtual void onVideoSubscribeStateChanged(const char* channel, uid_t uid, STREAM_SUBSCRIBE_STATE oldState, STREAM_SUBSCRIBE_STATE newState, int elapseSinceLastState) {
        (void)channel;
        (void)uid;
        (void)oldState;
        (void)newState;
        (void)elapseSinceLastState;
    }

    /** Reports which users are speaking, the speakers' volume and whether the local user is speaking.

     This callback reports the IDs and volumes of the loudest speakers at the moment in the channel, and whether the local user is speaking.

     By default, this callback is disabled. You can enable it by calling the \ref IRtcEngine::enableAudioVolumeIndication(int, int, bool) "enableAudioVolumeIndication" method.
     Once enabled, this callback is triggered at the set interval, regardless of whether a user speaks or not.

     The SDK triggers two independent `onAudioVolumeIndication` callbacks at one time, which separately report the volume information of the local user and all the remote speakers. 
     For more information, see the detailed parameter descriptions.

     @note
     - To enable the voice activity detection of the local user, ensure that you set `report_vad`(true) in the `enableAudioVolumeIndication` method.
     - Calling the \ref ar::rtc::IRtcEngine::muteLocalAudioStream "muteLocalAudioStream" method affects the SDK's behavior:
        - If the local user calls the \ref ar::rtc::IRtcEngine::muteLocalAudioStream "muteLocalAudioStream" method, the SDK stops triggering the local user's callback.
        - 20 seconds after a remote speaker calls the *muteLocalAudioStream* method, the remote speakers' callback excludes this remote user's information; 20 seconds after all remote users call the *muteLocalAudioStream* method, the SDK stops triggering the remote speakers' callback.
     - An empty @p speakers array in the *onAudioVolumeIndication* callback suggests that no remote user is speaking at the moment.

     @param speakers A pointer to AudioVolumeInfo:
     - In the local user's callback, this struct contains the following members:
       - `uid` = 0, 
       - `volume` = `totalVolume`, which reports the sum of the voice volume and audio-mixing volume of the local user, and
       - `vad`, which reports the voice activity status of the local user.
     - In the remote speakers' callback, this array contains the following members:
       - `uid` of the remote speaker,
       - `volume`, which reports the sum of the voice volume and audio-mixing volume of each remote speaker, and
       - `vad` = 0.
       
       An empty speakers array in the callback indicates that no remote user is speaking at the moment.
     @param speakerNumber Total number of speakers. The value range is [0, 3].
     - In the local user’s callback, `speakerNumber` = 1, regardless of whether the local user speaks or not.
     - In the remote speakers' callback, the callback reports the IDs and volumes of the three loudest speakers when there are more than three remote users in the channel, and `speakerNumber` = 3.
     @param totalVolume Total volume after audio mixing. The value ranges between 0 (lowest volume) and 255 (highest volume).
     - In the local user’s callback, `totalVolume` is the sum of the voice volume and audio-mixing volume of the local user.
     - In the remote speakers' callback, `totalVolume` is the sum of the voice volume and audio-mixing volume of all the remote speakers.
     */
    virtual void onAudioVolumeIndication(const AudioVolumeInfo* speakers, unsigned int speakerNumber, int totalVolume) {
        (void)speakers;
        (void)speakerNumber;
        (void)totalVolume;
    }

    /** Occurs when the most active speaker is detected.

     After a successful call of \ref IRtcEngine::enableAudioVolumeIndication(int, int, bool) "enableAudioVolumeIndication", 
     the SDK continuously detects which remote user has the loudest volume. During the current period, the remote user, 
     who is detected as the loudest for the most times, is the most active user.

     When the number of user is no less than two and an active speaker exists, the SDK triggers this callback and reports the `uid` of the most active speaker.
     - If the most active speaker is always the same user, the SDK triggers this callback only once.
     - If the most active speaker changes to another user, the SDK triggers this callback again and reports the `uid` of the new active speaker.
    
     @param uid The user ID of the most active speaker.
    */
    virtual void onActiveSpeaker(uid_t uid) {
        (void)uid;
    }

    /** **DEPRECATED** Occurs when the video stops playing.

     The application can use this callback to change the configuration of the view (for example, displaying other pictures in the view) after the video stops playing.

     Deprecated as of v2.4.1. Use LOCAL_VIDEO_STREAM_STATE_STOPPED(0) in the \ref ar::rtc::IRtcEngineEventHandler::onLocalVideoStateChanged "onLocalVideoStateChanged" callback instead.
     */
    virtual void onVideoStopped() {}

    /** Occurs when the first local video frame is displayed/rendered on the local video view.

    @param width Width (px) of the first local video frame.
    @param height Height (px) of the first local video frame.
    @param elapsed Time elapsed (ms) from the local user calling the \ref IRtcEngine::joinChannel "joinChannel" method until the SDK triggers this callback.
    If you call the \ref IRtcEngine::startPreview "startPreview" method  before calling the *joinChannel* method, then @p elapsed is the time elapsed from calling the *startPreview* method until the SDK triggers this callback.
    */
    virtual void onFirstLocalVideoFrame(int width, int height, int elapsed) {
        (void)width;
        (void)height;
        (void)elapsed;
    }

    /** Occurs when the first video frame is published.
     *
     * @since v3.1.0
     *
     * The SDK triggers this callback under one of the following circumstances:
     * - The local client enables the video module and calls \ref IRtcEngine::joinChannel "joinChannel" successfully.
     * - The local client calls \ref IRtcEngine::muteLocalVideoStream "muteLocalVideoStream(true)" and \ref IRtcEngine::muteLocalVideoStream "muteLocalVideoStream(false)" in sequence.
     * - The local client calls \ref IRtcEngine::disableVideo "disableVideo" and \ref IRtcEngine::enableVideo "enableVideo" in sequence.
     *
     * @param elapsed The time elapsed (ms) from the local client calling \ref IRtcEngine::joinChannel "joinChannel" until the SDK triggers this callback.
     */
    virtual void onFirstLocalVideoFramePublished(int elapsed) {
        (void)elapsed;
    }

    /** Occurs when the first remote video frame is received and decoded.
     *
     * @deprecated
     * This callback is deprecated and replaced by the
     * \ref onRemoteVideoStateChanged() "onRemoteVideoStateChanged" callback
     * with the following parameters:
     * - #REMOTE_VIDEO_STATE_STARTING (1)
     * - #REMOTE_VIDEO_STATE_DECODING (2)
     *
     * This callback is triggered in either of the following scenarios:
     *
     * - The remote user joins the channel and sends the video stream.
     * - The remote user stops sending the video stream and re-sends it after
     * 15 seconds. Reasons for such an interruption include:
     *  - The remote user leaves the channel.
     *  - The remote user drops offline.
     *  - The remote user calls the
     * \ref ar::rtc::IRtcEngine::muteLocalVideoStream "muteLocalVideoStream"
     *  method to stop sending the video stream.
     *  - The remote user calls the
     * \ref ar::rtc::IRtcEngine::disableVideo "disableVideo" method to
     * disable video.
     *
     * The application can configure the user view settings in this callback.
     *
     * @param uid User ID of the remote user sending the video stream.
     * @param width Width (px) of the video stream.
     * @param height Height (px) of the video stream.
     * @param elapsed Time elapsed (ms) from the local user calling the
     * \ref IRtcEngine::joinChannel "joinChannel" method until the SDK
     * triggers this callback.
     */
    virtual void onFirstRemoteVideoDecoded(uid_t uid, int width, int height, int elapsed) {
        (void)uid;
        (void)width;
        (void)height;
        (void)elapsed;
    }

    /** Occurs when the first remote video frame is rendered.

    The SDK triggers this callback when the first frame of the remote video is displayed in the user's video window. The application can retrieve the time elapsed from a user joining the channel until the first video frame is displayed.

    @param uid User ID of the remote user sending the video stream.
    @param width Width (px) of the video frame.
    @param height Height (px) of the video stream.
    @param elapsed Time elapsed (ms) from the local user calling the \ref IRtcEngine::joinChannel "joinChannel" method until the SDK triggers this callback.
    */
    virtual void onFirstRemoteVideoFrame(uid_t uid, int width, int height, int elapsed) {
        (void)uid;
        (void)width;
        (void)height;
        (void)elapsed;
    }

    /** @deprecated This method is deprecated from v3.0.0, use the \ref ar::rtc::IRtcEngineEventHandler::onRemoteAudioStateChanged "onRemoteAudioStateChanged" callback instead.

     Occurs when a remote user's audio stream playback pauses/resumes.

     The SDK triggers this callback when the remote user stops or resumes sending the audio stream by calling the \ref ar::rtc::IRtcEngine::muteLocalAudioStream "muteLocalAudioStream" method.

     @note This callback does not work properly when the number of users (in the `COMMUNICATION` profile) or hosts (in the `LIVE_BROADCASTING` profile) in the channel exceeds 17.

     @param uid User ID of the remote user.
     @param muted Whether the remote user's audio stream is muted/unmuted:
     - true: Muted.
     - false: Unmuted.
     */
    virtual void onUserMuteAudio(uid_t uid, bool muted) {
        (void)uid;
        (void)muted;
    }
     
    /** Occurs when a remote user's video stream playback pauses/resumes.
     *
     * You can also use the
     * \ref onRemoteVideoStateChanged() "onRemoteVideoStateChanged" callback
     * with the following parameters:
     * - #REMOTE_VIDEO_STATE_STOPPED (0) and
     * #REMOTE_VIDEO_STATE_REASON_REMOTE_MUTED (5).
     * - #REMOTE_VIDEO_STATE_DECODING (2) and
     * #REMOTE_VIDEO_STATE_REASON_REMOTE_UNMUTED (6).
     *
     * The SDK triggers this callback when the remote user stops or resumes
     * sending the video stream by calling the
     * \ref ar::rtc::IRtcEngine::muteLocalVideoStream
     * "muteLocalVideoStream" method.
     *
     * @note This callback does not work properly when the number of users (in the `COMMUNICATION` profile) or hosts (in the `LIVE_BROADCASTING` profile) in the channel exceeds 17.
     *
     * @param uid User ID of the remote user.
     * @param muted Whether the remote user's video stream playback is
     * paused/resumed:
     * - true: Paused.
     * - false: Resumed.
     */
    virtual void onUserMuteVideo(uid_t uid, bool muted) {
        (void)uid;
        (void)muted;
    }

    /** Occurs when a specific remote user enables/disables the video
     * module.
     *
     * @deprecated
     * This callback is deprecated and replaced by the
     * \ref onRemoteVideoStateChanged() "onRemoteVideoStateChanged" callback
     * with the following parameters:
     * - #REMOTE_VIDEO_STATE_STOPPED (0) and
     * #REMOTE_VIDEO_STATE_REASON_REMOTE_MUTED (5).
     * - #REMOTE_VIDEO_STATE_DECODING (2) and
     * #REMOTE_VIDEO_STATE_REASON_REMOTE_UNMUTED (6).
     *
     * Once the video module is disabled, the remote user can only use a
     * voice call. The remote user cannot send or receive any video from
     * other users.
     *
     * The SDK triggers this callback when the remote user enables or disables
     * the video module by calling the
     * \ref ar::rtc::IRtcEngine::enableVideo "enableVideo" or
     * \ref ar::rtc::IRtcEngine::disableVideo "disableVideo" method.
     *
     * @note This callback returns invalid when the number of users in a
     * channel exceeds 20.
     *
     * @param uid User ID of the remote user.
     * @param enabled Whether the remote user enables/disables the video
     * module:
     * - true: Enable. The remote user can enter a video session.
     * - false: Disable. The remote user can only enter a voice session, and
     * cannot send or receive any video stream.
     */
    	virtual void onUserEnableVideo(uid_t uid, bool enabled) {
    		(void)uid;
    		(void)enabled;
    	}

    /** Occurs when the audio device state changes.

     This callback notifies the application that the system's audio device state is changed. For example, a headset is unplugged from the device.

     @param deviceId Pointer to the device ID.
     @param deviceType Device type: #MEDIA_DEVICE_TYPE.
     @param deviceState Device state: #MEDIA_DEVICE_STATE_TYPE.
     */
    virtual void onAudioDeviceStateChanged(const char* deviceId, int deviceType, int deviceState) {
        (void)deviceId;
        (void)deviceType;
        (void)deviceState;
    }

    /** Occurs when the volume of the playback device, microphone, or application changes.

     @param deviceType Device type: #MEDIA_DEVICE_TYPE.
     @param volume Volume of the device. The value ranges between 0 and 255.
     @param muted
     - true: The audio device is muted.
     - false: The audio device is not muted.
     */
    virtual void onAudioDeviceVolumeChanged(MEDIA_DEVICE_TYPE deviceType, int volume, bool muted) {
        (void)deviceType;
        (void)volume;
        (void)muted;
    }

    /** **DEPRECATED** Occurs when the camera turns on and is ready to capture the video.

     If the camera fails to turn on, fix the error reported in the \ref IRtcEngineEventHandler::onError "onError" callback.

     Deprecated as of v2.4.1. Use #LOCAL_VIDEO_STREAM_STATE_CAPTURING (1) in the \ref ar::rtc::IRtcEngineEventHandler::onLocalVideoStateChanged "onLocalVideoStateChanged" callback instead.
     */
    virtual void onCameraReady() {}

    /** Occurs when the camera focus area changes.

     The SDK triggers this callback when the local user changes the camera focus position by calling the setCameraFocusPositionInPreview method.
     
     @note This callback is for Android and iOS only.

     @param x x coordinate of the changed camera focus area.
     @param y y coordinate of the changed camera focus area.
     @param width Width of the changed camera focus area.
     @param height Height of the changed camera focus area.
     */
    virtual void onCameraFocusAreaChanged(int x, int y, int width, int height) {
        (void)x;
        (void)y;
        (void)width;
        (void)height;
    }
#if defined(__ANDROID__) || (defined(__APPLE__) && TARGET_OS_IOS)
    /**
     * Reports the face detection result of the local user. Applies to Android and iOS only.
     * @since v3.0.1
     *
     * Once you enable face detection by calling \ref IRtcEngine::enableFaceDetection "enableFaceDetection"(true), you can get the following information on the local user in real-time:
     * - The width and height of the local video.
     * - The position of the human face in the local video.
     * - The distance between the human face and the device screen. This value is based on the fitting calculation of the local video size and the position of the human face.
     *
     * @note
     * - If the SDK does not detect a face, it reduces the frequency of this callback to reduce power consumption on the local device.
     * - The SDK stops triggering this callback when a human face is in close proximity to the screen.
     * - On Android, the `distance` value reported in this callback may be slightly different from the actual distance. Therefore, AR does not recommend using it for
     * accurate calculation.
     * @param imageWidth The width (px) of the local video.
     * @param imageHeight The height (px) of the local video.
     * @param vecRectangle The position and size of the human face on the local video:
     * - `x`: The x coordinate (px) of the human face in the local video. Taking the top left corner of the captured video as the origin,
     * the x coordinate represents the relative lateral displacement of the top left corner of the human face to the origin.
     * - `y`: The y coordinate (px) of the human face in the local video. Taking the top left corner of the captured video as the origin,
     * the y coordinate represents the relative longitudinal displacement of the top left corner of the human face to the origin.
     * - `width`: The width (px) of the human face in the captured video.
     * - `height`: The height (px) of the human face in the captured video.
     * @param vecDistance The distance (cm) between the human face and the screen.
     * @param numFaces The number of faces detected. If the value is 0, it means that no human face is detected.
     */
    virtual void onFacePositionChanged(int imageWidth, int imageHeight, Rectangle* vecRectangle, int* vecDistance, int numFaces){
       (void)imageWidth;
       (void)imageHeight;
       (void)vecRectangle;
       (void)vecDistance;
        (void)numFaces;
    }
#endif
    /** Occurs when the camera exposure area changes.

    The SDK triggers this callback when the local user changes the camera exposure position by calling the setCameraExposurePosition method.
     
     @note This callback is for Android and iOS only.
     
     @param x x coordinate of the changed camera exposure area.
     @param y y coordinate of the changed camera exposure area.
     @param width Width of the changed camera exposure area.
     @param height Height of the changed camera exposure area.
     */
    virtual void onCameraExposureAreaChanged(int x, int y, int width, int height) {
        (void)x;
        (void)y;
        (void)width;
        (void)height;
    }

    /** Occurs when the audio mixing file playback finishes.

     **DEPRECATED**  use onAudioMixingStateChanged instead.

     You can start an audio mixing file playback by calling the \ref IRtcEngine::startAudioMixing "startAudioMixing" method. The SDK triggers this callback when the audio mixing file playback finishes.

     If the *startAudioMixing* method call fails, an error code returns in the \ref IRtcEngineEventHandler::onError "onError" callback.

     */
    virtual void onAudioMixingFinished() {
    }

    /** Occurs when the state of the local user's audio mixing file changes.

     When you call the \ref IRtcEngine::startAudioMixing "startAudioMixing" method and the state of audio mixing file changes, the SDK triggers this callback.
     - When the audio mixing file plays, pauses playing, or stops playing, this callback returns 710, 711, or 713 in @p state, and 0 in @p errorCode.
     - When exceptions occur during playback, this callback returns 714 in @p state and an error in @p errorCode.
     - If the local audio mixing file does not exist, or if the SDK does not support the file format or cannot access the music file URL, the SDK returns WARN_AUDIO_MIXING_OPEN_ERROR = 701.

     @param state The state code. See #AUDIO_MIXING_STATE_TYPE.
     @param errorCode The error code. See #AUDIO_MIXING_ERROR_TYPE.
     */
    virtual void onAudioMixingStateChanged(AUDIO_MIXING_STATE_TYPE state, AUDIO_MIXING_ERROR_TYPE errorCode){
    }
    /** Occurs when a remote user starts audio mixing.

     When a remote user calls \ref IRtcEngine::startAudioMixing "startAudioMixing" to play the background music, the SDK reports this callback.
     */
    virtual void onRemoteAudioMixingBegin() {
    }
    /** Occurs when a remote user finishes audio mixing.
     */
    virtual void onRemoteAudioMixingEnd() {
    }

    /** Occurs when the local audio effect playback finishes.

     The SDK triggers this callback when the local audio effect file playback finishes.

     @param soundId ID of the local audio effect. Each local audio effect has a unique ID.
     */
    virtual void onAudioEffectFinished(int soundId) {
    }


    /**
     Occurs when the SDK decodes the first remote audio frame for playback.

     @deprecated v3.0.0

     This callback is deprecated. Use `onRemoteAudioStateChanged` instead.

     This callback is triggered in either of the following scenarios:

     - The remote user joins the channel and sends the audio stream.
     - The remote user stops sending the audio stream and re-sends it after 15 seconds. Reasons for such an interruption include:
         - The remote user leaves channel.
         - The remote user drops offline.
         - The remote user calls the \ref ar::rtc::IRtcEngine::muteLocalAudioStream "muteLocalAudioStream" method to stop sending the local audio stream.
         - The remote user calls the \ref ar::rtc::IRtcEngine::disableAudio "disableAudio" method to disable audio.

     @param uid User ID of the remote user sending the audio stream.
     @param elapsed Time elapsed (ms) from the local user calling the \ref IRtcEngine::joinChannel "joinChannel" method until the SDK triggers this callback.
     */
    virtual void onFirstRemoteAudioDecoded(uid_t uid, int elapsed) {
        (void)uid;
        (void)elapsed;
    }

    /** Occurs when the video device state changes.

     @note On a Windows device with an external camera for video capturing, the video disables once the external camera is unplugged.

     @param deviceId Pointer to the device ID of the video device that changes state.
     @param deviceType Device type: #MEDIA_DEVICE_TYPE.
     @param deviceState Device state: #MEDIA_DEVICE_STATE_TYPE.
     */
    virtual void onVideoDeviceStateChanged(const char* deviceId, int deviceType, int deviceState) {
        (void)deviceId;
        (void)deviceType;
        (void)deviceState;
    }

    /** Occurs when the local video stream state changes.

     This callback indicates the state of the local video stream, including camera capturing and video encoding, and allows you to troubleshoot issues when exceptions occur.

     @note For some device models, the SDK will not trigger this callback when the state of the local video changes while the local video capturing device is in use, so you have to make your own timeout judgment.

     @param localVideoState State type #LOCAL_VIDEO_STREAM_STATE. When the state is LOCAL_VIDEO_STREAM_STATE_FAILED (3), see the `error` parameter for details.
     @param error The detailed error information: #LOCAL_VIDEO_STREAM_ERROR.
     */
    virtual void onLocalVideoStateChanged(LOCAL_VIDEO_STREAM_STATE localVideoState, LOCAL_VIDEO_STREAM_ERROR error) {
        (void)localVideoState;
        (void)error;
    }

    /** Occurs when the video size or rotation of a specified user changes.

     @param uid User ID of the remote user or local user (0) whose video size or rotation changes.
     @param width New width (pixels) of the video.
     @param height New height (pixels) of the video.
     @param rotation New rotation of the video [0 to 360).
     */
    virtual void onVideoSizeChanged(uid_t uid, int width, int height, int rotation) {
        (void)uid;
        (void)width;
        (void)height;
        (void)rotation;
    }
    /** Occurs when the remote video state changes.
     *
     * @param uid ID of the remote user whose video state changes.
     * @param state State of the remote video. See #REMOTE_VIDEO_STATE.
     * @param reason The reason of the remote video state change. See
     * #REMOTE_VIDEO_STATE_REASON.
     * @param elapsed Time elapsed (ms) from the local user calling the
     * \ref ar::rtc::IRtcEngine::joinChannel "joinChannel" method until the
     * SDK triggers this callback.
     */
    virtual void onRemoteVideoStateChanged(uid_t uid, REMOTE_VIDEO_STATE state, REMOTE_VIDEO_STATE_REASON reason, int elapsed) {
        (void)uid;
        (void)state;
        (void)reason;
        (void)elapsed;
    }

	/** Occurs when a specified remote user enables/disables the local video
     * capturing function.
     *
     * @deprecated
     * This callback is deprecated and replaced by the
     * \ref onRemoteVideoStateChanged() "onRemoteVideoStateChanged" callback
     * with the following parameters:
     * - #REMOTE_VIDEO_STATE_STOPPED (0) and
     * #REMOTE_VIDEO_STATE_REASON_REMOTE_MUTED (5).
     * - #REMOTE_VIDEO_STATE_DECODING (2) and
     * #REMOTE_VIDEO_STATE_REASON_REMOTE_UNMUTED (6).
     *
     * This callback is only applicable to the scenario when the user only
     * wants to watch the remote video without sending any video stream to the
     * other user.
     *
     * The SDK triggers this callback when the remote user resumes or stops
     * capturing the video stream by calling the
     * \ref ar::rtc::IRtcEngine::enableLocalVideo "enableLocalVideo" method.
     *
     * @param uid User ID of the remote user.
     * @param enabled Whether the specified remote user enables/disables the
     * local video capturing function:
     * - true: Enable. Other users in the channel can see the video of this
     * remote user.
     * - false: Disable. Other users in the channel can no longer receive the
     * video stream from this remote user, while this remote user can still
     * receive the video streams from other users.
     */
    virtual void onUserEnableLocalVideo(uid_t uid, bool enabled) {
        (void)uid;
        (void)enabled;
    }

//    virtual void onStreamError(int streamId, int code, int parameter, const char* message, size_t length) {}
    /** Occurs when the local user receives the data stream from the remote user within five seconds.

    The SDK triggers this callback when the local user receives the stream message that the remote user sends by calling the \ref ar::rtc::IRtcEngine::sendStreamMessage "sendStreamMessage" method.
    @param uid User ID of the remote user sending the message.
    @param streamId Stream ID.
    @param data Pointer to the data received by the local user.
    @param length Length of the data in bytes.
    */
    virtual void onStreamMessage(uid_t uid, int streamId, const char* data, size_t length) {
        (void)uid;
        (void)streamId;
        (void)data;
        (void)length;
    }

	/** Occurs when the local user does not receive the data stream from the remote user within five seconds.

     The SDK triggers this callback when the local user fails to receive the stream message that the remote user sends by calling the \ref ar::rtc::IRtcEngine::sendStreamMessage "sendStreamMessage" method.
     @param uid User ID of the remote user sending the message.
     @param streamId Stream ID.
     @param code Error code: #ERROR_CODE_TYPE.
     @param missed Number of lost messages.
     @param cached Number of incoming cached messages when the data stream is interrupted.
     */
	virtual void onStreamMessageError(uid_t uid, int streamId, int code, int missed, int cached) {
        (void)uid;
        (void)streamId;
        (void)code;
        (void)missed;
        (void)cached;
    }

    /** Occurs when the media engine loads.*/
    virtual void onMediaEngineLoadSuccess() {
    }
    /** Occurs when the media engine call starts.*/
    virtual void onMediaEngineStartCallSuccess() {
    }
    /// @cond
    /** Reports whether the super-resolution algorithm is enabled.
     *
     * @since v3.2.0
     *
     * After calling \ref IRtcEngine::enableRemoteSuperResolution "enableRemoteSuperResolution", the SDK triggers this
     * callback to report whether the super-resolution algorithm is successfully enabled. If not successfully enabled,
     * you can use reason for troubleshooting.
     *
     * @param uid The ID of the remote user.
     * @param enabled Whether the super-resolution algorithm is successfully enabled:
     * - true: The super-resolution algorithm is successfully enabled.
     * - false: The super-resolution algorithm is not successfully enabled.
     * @param reason The reason why the super-resolution algorithm is not successfully enabled. See #SUPER_RESOLUTION_STATE_REASON.
     */
    virtual void onUserSuperResolutionEnabled(uid_t uid, bool enabled, SUPER_RESOLUTION_STATE_REASON reason) {
        (void)uid;
        (void)enabled;
        (void)reason;
    }
    /// @endcond

    /** Occurs when the state of the media stream relay changes.
     *
     * The SDK returns the state of the current media relay with any error
     * message.
     *
     * @param state The state code in #CHANNEL_MEDIA_RELAY_STATE.
     * @param code The error code in #CHANNEL_MEDIA_RELAY_ERROR.
     */
    virtual void onChannelMediaRelayStateChanged(CHANNEL_MEDIA_RELAY_STATE state,CHANNEL_MEDIA_RELAY_ERROR code) {
    }

    /** Reports events during the media stream relay.
     *
     * @param code The event code in #CHANNEL_MEDIA_RELAY_EVENT.
     */
    virtual void onChannelMediaRelayEvent(CHANNEL_MEDIA_RELAY_EVENT code) {
    }

    /** Occurs when the engine sends the first local audio frame.

     @deprecated Deprecated as of v3.1.0. Use the \ref IRtcEngineEventHandler::onFirstLocalAudioFramePublished "onFirstLocalAudioFramePublished" callback instead.

     @param elapsed Time elapsed (ms) from the local user calling \ref IRtcEngine::joinChannel "joinChannel" until the SDK triggers this callback.
     */
    virtual void onFirstLocalAudioFrame(int elapsed) {
        (void)elapsed;
    }

    /** Occurs when the first audio frame is published.
     *
     * @since v3.1.0
     *
     * The SDK triggers this callback under one of the following circumstances:
     * - The local client enables the audio module and calls \ref IRtcEngine::joinChannel "joinChannel" successfully.
     * - The local client calls \ref IRtcEngine::muteLocalAudioStream "muteLocalAudioStream(true)" and \ref IRtcEngine::muteLocalAudioStream "muteLocalAudioStream(false)" in sequence.
     * - The local client calls \ref IRtcEngine::disableAudio "disableAudio" and \ref IRtcEngine::enableAudio "enableAudio" in sequence.
     *
     * @param elapsed The time elapsed (ms) from the local client calling \ref IRtcEngine::joinChannel "joinChannel" until the SDK triggers this callback.
     */
    virtual void onFirstLocalAudioFramePublished(int elapsed) {
        (void)elapsed;
    }

    /** Occurs when the engine receives the first audio frame from a specific remote user.

    @param uid User ID of the remote user.
    @param elapsed Time elapsed (ms) from the remote user calling \ref IRtcEngine::joinChannel "joinChannel" until the SDK triggers this callback.
    */
    virtual void onFirstRemoteAudioFrame(uid_t uid, int elapsed) {
        (void)uid;
        (void)elapsed;
    }

  /**
   Occurs when the state of the RTMP streaming changes.

   The SDK triggers this callback to report the result of the local user calling the \ref ar::rtc::IRtcEngine::addPublishStreamUrl "addPublishStreamUrl" or \ref ar::rtc::IRtcEngine::removePublishStreamUrl "removePublishStreamUrl" method.

   This callback indicates the state of the RTMP streaming. When exceptions occur, you can troubleshoot issues by referring to the detailed error descriptions in the *errCode* parameter.

   @param url The RTMP URL address.
   @param state The RTMP streaming state. See: #RTMP_STREAM_PUBLISH_STATE.
   @param errCode The detailed error information for streaming. See: #RTMP_STREAM_PUBLISH_ERROR.
   */
  virtual void onRtmpStreamingStateChanged(const char *url, RTMP_STREAM_PUBLISH_STATE state, RTMP_STREAM_PUBLISH_ERROR errCode) {
    (void) url;
    (void) state;
    (void) errCode;
  }

 /** Reports events during the RTMP streaming.
  *
  * @since v3.1.0
  *
  * @param url The RTMP streaming URL.
  * @param eventCode The event code. See #RTMP_STREAMING_EVENT
  */
  virtual void onRtmpStreamingEvent(const char* url, RTMP_STREAMING_EVENT eventCode) {
      (void) url;
      (void) eventCode;
  }

    /** @deprecated This method is deprecated, use the \ref ar::rtc::IRtcEngineEventHandler::onRtmpStreamingStateChanged "onRtmpStreamingStateChanged" callback instead.

     Reports the result of calling the \ref IRtcEngine::addPublishStreamUrl "addPublishStreamUrl" method. (CDN live only.)

     @param url The RTMP URL address.
     @param error Error code: #ERROR_CODE_TYPE. Main errors include:
     - #ERR_OK (0): The publishing succeeds.
     - #ERR_FAILED (1): The publishing fails.
     - #ERR_INVALID_ARGUMENT (2): Invalid argument used. If, for example, you did not call \ref ar::rtc::IRtcEngine::setLiveTranscoding "setLiveTranscoding" to configure LiveTranscoding before calling \ref ar::rtc::IRtcEngine::addPublishStreamUrl "addPublishStreamUrl", the SDK reports #ERR_INVALID_ARGUMENT.
     - #ERR_TIMEDOUT (10): The publishing timed out.
     - #ERR_ALREADY_IN_USE (19): The chosen URL address is already in use for CDN live streaming.
     - #ERR_RESOURCE_LIMITED (22): The backend system does not have enough resources for the CDN live streaming.
     - #ERR_ENCRYPTED_STREAM_NOT_ALLOWED_PUBLISH (130): You cannot publish an encrypted stream.
     - #ERR_PUBLISH_STREAM_CDN_ERROR (151)
     - #ERR_PUBLISH_STREAM_NUM_REACH_LIMIT (152)
     - #ERR_PUBLISH_STREAM_NOT_AUTHORIZED (153)
     - #ERR_PUBLISH_STREAM_INTERNAL_SERVER_ERROR (154)
     - #ERR_PUBLISH_STREAM_FORMAT_NOT_SUPPORTED (156)
     */
    virtual void onStreamPublished(const char *url, int error) {
        (void)url;
        (void)error;
    }
    /** Reports the result of calling the \ref ar::rtc::IRtcEngine::removePublishStreamUrl "removePublishStreamUrl" method. (CDN live only.)

     This callback indicates whether you have successfully removed an RTMP stream from the CDN.

     @param url The RTMP URL address.
     */
    virtual void onStreamUnpublished(const char *url) {
        (void)url;
    }
/** Occurs when the publisher's transcoding is updated. 
 * 
 * When the `LiveTranscoding` class in the \ref ar::rtc::IRtcEngine::setLiveTranscoding "setLiveTranscoding" method updates, the SDK triggers the `onTranscodingUpdated` callback to report the update information to the local host.
 * 
 * @note If you call the `setLiveTranscoding` method to set the LiveTranscoding class for the first time, the SDK does not trigger the `onTranscodingUpdated` callback.
 * 
 */
    virtual void onTranscodingUpdated() {
    }
   /** Occurs when a voice or video stream URL address is added to a live broadcast.

    @param url Pointer to the URL address of the externally injected stream.
    @param uid User ID.
    @param status State of the externally injected stream: #INJECT_STREAM_STATUS.
    */
    virtual void onStreamInjectedStatus(const char* url, uid_t uid, int status) {
        (void)url;
        (void)uid;
        (void)status;
    }

    /** Occurs when the local audio route changes.

     The SDK triggers this callback when the local audio route switches to an earpiece, speakerphone, headset, or Bluetooth device.

     @note This callback is for Android and iOS only.

     @param routing Audio output routing. See: #AUDIO_ROUTE_TYPE.
     */
    virtual void onAudioRouteChanged(AUDIO_ROUTE_TYPE routing) {
		(void)routing;
	}

   /** Occurs when the locally published media stream falls back to an audio-only stream due to poor network conditions or switches back to the video after the network conditions improve.

    If you call \ref IRtcEngine::setLocalPublishFallbackOption "setLocalPublishFallbackOption" and set *option* as #STREAM_FALLBACK_OPTION_AUDIO_ONLY, the SDK triggers this callback when the locally published stream falls back to audio-only mode due to poor uplink conditions, or when the audio stream switches back to the video after the uplink network condition improves.

    @param isFallbackOrRecover Whether the locally published stream falls back to audio-only or switches back to the video:
    - true: The locally published stream falls back to audio-only due to poor network conditions.
    - false: The locally published stream switches back to the video after the network conditions improve.
    */
    virtual void onLocalPublishFallbackToAudioOnly(bool isFallbackOrRecover) {
        (void)isFallbackOrRecover;
    }

    /** Occurs when the remote media stream falls back to audio-only stream
     * due to poor network conditions or switches back to the video stream
     * after the network conditions improve.
     *
     * If you call
     * \ref IRtcEngine::setRemoteSubscribeFallbackOption
     * "setRemoteSubscribeFallbackOption" and set
     * @p option as #STREAM_FALLBACK_OPTION_AUDIO_ONLY, the SDK triggers this
     * callback when the remote media stream falls back to audio-only mode due
     * to poor uplink conditions, or when the remote media stream switches
     * back to the video after the uplink network condition improves.
     *
     * @note Once the remote media stream switches to the low stream due to
     * poor network conditions, you can monitor the stream switch between a
     * high and low stream in the RemoteVideoStats callback.
     *
     * @param uid ID of the remote user sending the stream.
     * @param isFallbackOrRecover Whether the remotely subscribed media stream
     * falls back to audio-only or switches back to the video:
     * - true: The remotely subscribed media stream falls back to audio-only
     * due to poor network conditions.
     * - false: The remotely subscribed media stream switches back to the
     * video stream after the network conditions improved.
     */
    virtual void onRemoteSubscribeFallbackToAudioOnly(uid_t uid, bool isFallbackOrRecover) {
        (void)uid;
        (void)isFallbackOrRecover;
    }

    /** Reports the transport-layer statistics of each remote audio stream.
     *
     * @deprecated
     * This callback is deprecated and replaced by the
     * \ref onRemoteAudioStats() "onRemoteAudioStats" callback.
     *
     * This callback reports the transport-layer statistics, such as the
     * packet loss rate and network time delay, once every two seconds after
     * the local user receives an audio packet from a remote user.
     *
     * @param uid  User ID of the remote user sending the audio packet.
     * @param delay Network time delay (ms) from the remote user sending the
     * audio packet to the local user.
     * @param lost Packet loss rate (%) of the audio packet sent from the
     * remote user.
     * @param rxKBitRate  Received bitrate (Kbps) of the audio packet sent
     * from the remote user.
     */
    virtual void onRemoteAudioTransportStats(
        uid_t uid, unsigned short delay, unsigned short lost,
        unsigned short rxKBitRate) {
        (void)uid;
        (void)delay;
        (void)lost;
        (void)rxKBitRate;
    }

    /** Reports the transport-layer statistics of each remote video stream.
     *
     * @deprecated
     * This callback is deprecated and replaced by the
     * \ref onRemoteVideoStats() "onRemoteVideoStats" callback.
     *
     * This callback reports the transport-layer statistics, such as the
     * packet loss rate and network time delay, once every two seconds after
     * the local user receives a video packet from a remote user.
     *
     * @param uid User ID of the remote user sending the video packet.
     * @param delay Network time delay (ms) from the remote user sending the
     * video packet to the local user.
     * @param lost Packet loss rate (%) of the video packet sent from the
     * remote user.
     * @param rxKBitRate Received bitrate (Kbps) of the video packet sent
     * from the remote user.
     */
    virtual void onRemoteVideoTransportStats(
        uid_t uid, unsigned short delay, unsigned short lost,
        unsigned short rxKBitRate) {
        (void)uid;
        (void)delay;
        (void)lost;
        (void)rxKBitRate;
    }

    /** **DEPRECATED** Occurs when the microphone is enabled/disabled.
     *
     * The \ref onMicrophoneEnabled() "onMicrophoneEnabled" callback is
     * deprecated. Use #LOCAL_AUDIO_STREAM_STATE_STOPPED (0) or
     * #LOCAL_AUDIO_STREAM_STATE_RECORDING (1) in the
     * \ref onLocalAudioStateChanged() "onLocalAudioStateChanged" callback
     * instead.
     *
     * The SDK triggers this callback when the local user resumes or stops
     * capturing the local audio stream by calling the
     * \ref ar::rtc::IRtcEngine::enableLocalAudio "enbaleLocalAudio" method.
     *
     * @param enabled Whether the microphone is enabled/disabled:
     * - true: Enabled.
     * - false: Disabled.
     */
    virtual void onMicrophoneEnabled(bool enabled) {
        (void)enabled;
    }
    /** Occurs when the connection state between the SDK and the server changes.

     @param state See #CONNECTION_STATE_TYPE.
     @param reason See #CONNECTION_CHANGED_REASON_TYPE.
     */
    virtual void onConnectionStateChanged(
        CONNECTION_STATE_TYPE state, CONNECTION_CHANGED_REASON_TYPE reason) {
      (void)state;
      (void)reason;
    }

    /** Occurs when the local network type changes.

	When the network connection is interrupted, this callback indicates whether the interruption is caused by a network type change or poor network conditions.

     @param type See #NETWORK_TYPE.
     */
    virtual void onNetworkTypeChanged(NETWORK_TYPE type) {
      (void)type;
    }
    /** Occurs when the local user successfully registers a user account by calling the \ref ar::rtc::IRtcEngine::registerLocalUserAccount "registerLocalUserAccount" method or joins a channel by calling the \ref ar::rtc::IRtcEngine::joinChannelWithUserAccount "joinChannelWithUserAccount" method.This callback reports the user ID and user account of the local user.

     @param uid The ID of the local user.
     @param userAccount The user account of the local user.
     */
    virtual void onLocalUserRegistered(uid_t uid, const char* userAccount) {
      (void)uid;
      (void)userAccount;
    }
    /** Occurs when the SDK gets the user ID and user account of the remote user.

     After a remote user joins the channel, the SDK gets the UID and user account of the remote user,
     caches them in a mapping table object (`userInfo`), and triggers this callback on the local client.

     @param uid The ID of the remote user.
     @param info The `UserInfo` object that contains the user ID and user account of the remote user.
     */
    virtual void onUserInfoUpdated(uid_t uid, const UserInfo& info) {
      (void)uid;
      (void)info;
    }
};

/**
* Video device collection methods.

 The IVideoDeviceCollection interface class retrieves the video device information.
*/
class IVideoDeviceCollection
{
protected:
    virtual ~IVideoDeviceCollection(){}
public:
    /** Retrieves the total number of the indexed video devices in the system.

    @return Total number of the indexed video devices:
    */
    virtual int getCount() = 0;

    /** Retrieves a specified piece of information about an indexed video device.

     @param index The specified index of the video device that must be less than the return value of \ref IVideoDeviceCollection::getCount "getCount".
     @param deviceName Pointer to the video device name.
     @param deviceId Pointer to the video device ID.
     @return
     - 0: Success.
     - < 0: Failure.
     */
    virtual int getDevice(int index, char deviceName[MAX_DEVICE_ID_LENGTH], char deviceId[MAX_DEVICE_ID_LENGTH]) = 0;

    /** Sets the device with the device ID.

     @param deviceId Device ID of the device.
     @return
     - 0: Success.
     - < 0: Failure.
     */
    virtual int setDevice(const char deviceId[MAX_DEVICE_ID_LENGTH]) = 0;

    /** Releases all IVideoDeviceCollection resources.
    */
    virtual void release() = 0;
};

/** Video device management methods.

 The IVideoDeviceManager interface class tests the video device interfaces. Instantiate an AVideoDeviceManager class to retrieve an IVideoDeviceManager interface.
*/
class IVideoDeviceManager
{
protected:
    virtual ~IVideoDeviceManager(){}
public:

    /** Enumerates the video devices.

     This method returns an IVideoDeviceCollection object including all video devices in the system. With the IVideoDeviceCollection object, the application can enumerate the video devices. The application must call the \ref IVideoDeviceCollection::release "release" method to release the returned object after using it.

     @return
     - An IVideoDeviceCollection object including all video devices in the system: Success.
     - NULL: Failure.
     */
    virtual IVideoDeviceCollection* enumerateVideoDevices() = 0;

    /** Starts the video-capture device test.

     This method tests whether the video-capture device works properly. Before calling this method, ensure that you have already called the \ref IRtcEngine::enableVideo "enableVideo" method, and the window handle (*hwnd*) parameter is valid.

     @param hwnd The window handle used to display the screen.

     @return
     - 0: Success.
     - < 0: Failure.
     */
    virtual int startDeviceTest(view_t hwnd) = 0;

    /** Stops the video-capture device test.

     @return
     - 0: Success.
     - < 0: Failure.
     */
    virtual int stopDeviceTest() = 0;

    /** Sets a device with the device ID.

     @param deviceId Pointer to the video-capture device ID. Call the \ref IVideoDeviceManager::enumerateVideoDevices "enumerateVideoDevices" method to retrieve it.

     @note Plugging or unplugging the device does not change the device ID.

     @return
     - 0: Success.
     - < 0: Failure.
     */
    virtual int setDevice(const char deviceId[MAX_DEVICE_ID_LENGTH]) = 0;

    /** Retrieves the video-capture device that is in use.

     @param deviceId Pointer to the video-capture device ID.
     @return
     - 0: Success.
     - < 0: Failure.
     */
    virtual int getDevice(char deviceId[MAX_DEVICE_ID_LENGTH]) = 0;

	/** Select video device for Screen cast.

	 @param deviceId Pointer to the video-capture device ID.
	 @note: deviceId - scree0 for default desktop
	 @return
	 - 0: Success.
	 - < 0: Failure.
	 */
	virtual int selectScreenCastDevice(char deviceId[MAX_DEVICE_ID_LENGTH]) = 0;

    /** Releases all IVideoDeviceManager resources.
    */
    virtual void release() = 0;
};

/** Audio device collection methods.

The IAudioDeviceCollection interface class retrieves device-related information.
*/
class IAudioDeviceCollection
{
protected:
    virtual ~IAudioDeviceCollection(){}
public:

    /** Retrieves the total number of audio playback or audio recording devices.

     @note You must first call the \ref IAudioDeviceManager::enumeratePlaybackDevices "enumeratePlaybackDevices" or \ref IAudioDeviceManager::enumerateRecordingDevices "enumerateRecordingDevices" method before calling this method to return the number of  audio playback or audio recording devices.

     @return Number of audio playback or audio recording devices.
     */
    virtual int getCount() = 0;

    /** Retrieves a specified piece of information about an indexed audio device.

     @param index The specified index that must be less than the return value of \ref IAudioDeviceCollection::getCount "getCount".
     @param deviceName Pointer to the audio device name.
     @param deviceId Pointer to the audio device ID.
     @return
     - 0: Success.
     - < 0: Failure.
     */
    virtual int getDevice(int index, char deviceName[MAX_DEVICE_ID_LENGTH], char deviceId[MAX_DEVICE_ID_LENGTH]) = 0;

    /** Specifies a device with the device ID.

     @param deviceId Pointer to the device ID of the device.
     @return
     - 0: Success.
     - < 0: Failure.
     */
    virtual int setDevice(const char deviceId[MAX_DEVICE_ID_LENGTH]) = 0;

    /** Sets the volume of the application.

    @param volume Application volume. The value ranges between 0 (lowest volume) and 255 (highest volume).
    @return
    - 0: Success.
    - < 0: Failure.
    */
    virtual int setApplicationVolume(int volume) = 0;

    /** Retrieves the volume of the application.

     @param volume Pointer to the application volume. The volume value ranges between 0 (lowest volume) and 255 (highest volume).

     @return
     - 0: Success.
     - < 0: Failure.
     */
    virtual int getApplicationVolume(int& volume) = 0;

    /** Mutes the application.

     @param mute Sets whether to mute/unmute the application:
     - true: Mute the application.
     - false: Unmute the application.

     @return
     - 0: Success.
     - < 0: Failure.
     */
    virtual int setApplicationMute(bool mute) = 0;
    /** Gets the mute state of the application.

     @param mute Pointer to whether the application is muted/unmuted.
     - true: The application is muted.
     - false: The application is not muted.

     @return
     - 0: Success.
     - < 0: Failure.
     */
    virtual int isApplicationMute(bool& mute) = 0;

    /** Releases all IAudioDeviceCollection resources.
    */
    virtual void release() = 0;
};
/** Audio device management methods.

 The IAudioDeviceManager interface class allows for audio device interface testing. Instantiate an AAudioDeviceManager class to retrieve the IAudioDeviceManager interface.
*/
class IAudioDeviceManager
{
protected:
    virtual ~IAudioDeviceManager(){}
public:

    /** Enumerates the audio playback devices.

     This method returns an IAudioDeviceCollection object that includes all audio playback devices in the system. With the IAudioDeviceCollection object, the application can enumerate the audio playback devices.

     @note The application must call the \ref IAudioDeviceCollection::release "release" method to release the returned object after using it.

     @return
     - Success: Returns an IAudioDeviceCollection object that includes all audio playback devices in the system. For wireless Bluetooth headset devices with master and slave headsets, the master headset is the playback device.
     - Returns NULL: Failure.
     */
    virtual IAudioDeviceCollection* enumeratePlaybackDevices() = 0;

    /** Enumerates the audio recording devices.

     This method returns an IAudioDeviceCollection object that includes all audio recording devices in the system. With the IAudioDeviceCollection object, the application can enumerate the audio recording devices.

     @note The application needs to call the \ref IAudioDeviceCollection::release "release" method to release the returned object after using it.

     @return
     - Returns an IAudioDeviceCollection object that includes all audio recording devices in the system: Success.
     - Returns NULL: Failure.
     */
    virtual IAudioDeviceCollection* enumerateRecordingDevices() = 0;

    /** Sets the audio playback device using the device ID.

     @note Plugging or unplugging the audio device does not change the device ID.

     @param deviceId Device ID of the audio playback device, retrieved by calling the \ref IAudioDeviceManager::enumeratePlaybackDevices "enumeratePlaybackDevices" method.

     @return
     - 0: Success.
     - < 0: Failure.
     */
    virtual int setPlaybackDevice(const char deviceId[MAX_DEVICE_ID_LENGTH]) = 0;

    /** Sets the audio recording device using the device ID.

     @param deviceId Device ID of the audio recording device, retrieved by calling the \ref IAudioDeviceManager::enumerateRecordingDevices "enumerateRecordingDevices" method.

     @note Plugging or unplugging the audio device does not change the device ID.

     @return
     - 0: Success.
     - < 0: Failure.
     */
    virtual int setRecordingDevice(const char deviceId[MAX_DEVICE_ID_LENGTH]) = 0;

    /** Starts the audio playback device test.

     This method tests if the playback device works properly. In the test, the SDK plays an audio file specified by the user. If the user can hear the audio, the playback device works properly.

     @param testAudioFilePath Pointer to the path of the audio file for the audio playback device test in UTF-8:
     - Supported file formats: wav, mp3, m4a, and aac.
     - Supported file sample rates: 8000, 16000, 32000, 44100, and 48000 Hz.

     @return
     - 0: Success, and you can hear the sound of the specified audio file.
     - < 0: Failure.
     */
    virtual int startPlaybackDeviceTest(const char* testAudioFilePath) = 0;

    /** Stops the audio playback device test.

     This method stops testing the audio playback device. You must call this method to stop the test after calling the \ref IAudioDeviceManager::startPlaybackDeviceTest "startPlaybackDeviceTest" method.

     @return
     - 0: Success.
     - < 0: Failure.
     */
    virtual int stopPlaybackDeviceTest() = 0;

    /** Sets the volume of the audio playback device.

     @param volume Sets the volume of the audio playback device. The value ranges between 0 (lowest volume) and 255 (highest volume).
     @return
     - 0: Success.
     - < 0: Failure.
     */
    virtual int setPlaybackDeviceVolume(int volume) = 0;

    /** Retrieves the volume of the audio playback device.

     @param volume Pointer to the audio playback device volume. The volume value ranges between 0 (lowest volume) and 255 (highest volume).
     @return
     - 0: Success.
     - < 0: Failure.
     */
    virtual int getPlaybackDeviceVolume(int *volume) = 0;

    /** Sets the volume of the microphone.

     @param volume Sets the volume of the microphone. The value ranges between 0 (lowest volume) and 255 (highest volume).
     @return
     - 0: Success.
     - < 0: Failure.
     */
    virtual int setRecordingDeviceVolume(int volume) = 0;

    /** Retrieves the volume of the microphone.

     @param volume Pointer to the microphone volume. The volume value ranges between 0 (lowest volume) and 255 (highest volume).
     @return
     - 0: Success.
     - < 0: Failure.
     */
    virtual int getRecordingDeviceVolume(int *volume) = 0;

    /** Mutes the audio playback device.

     @param mute Sets whether to mute/unmute the audio playback device:
     - true: Mutes.
     - false: Unmutes.

     @return
     - 0: Success.
     - < 0: Failure.
     */
    virtual int setPlaybackDeviceMute(bool mute) = 0;
    /** Retrieves the mute status of the audio playback device.

     @param mute Pointer to whether the audio playback device is muted/unmuted.
     - true: Muted.
     - false: Unmuted.

     @return
     - 0: Success.
     - < 0: Failure.
     */
    virtual int getPlaybackDeviceMute(bool *mute) = 0;
    /** Mutes/Unmutes the microphone.

     @param mute Sets whether to mute/unmute the microphone:
     - true: Mutes.
     - false: Unmutes.

     @return
     - 0: Success.
     - < 0: Failure.
     */
    virtual int setRecordingDeviceMute(bool mute) = 0;

    /** Retrieves the microphone's mute status.

     @param mute Pointer to whether the microphone is muted/unmuted.
     - true: Muted.
     - false: Unmuted.

     @return
     - 0: Success.
     - < 0: Failure.
     */
    virtual int getRecordingDeviceMute(bool *mute) = 0;

    /** Starts the microphone test.

     This method tests whether the microphone works properly. Once the test starts, the SDK uses the \ref IRtcEngineEventHandler::onAudioVolumeIndication "onAudioVolumeIndication" callback to notify the application with the volume information.

     @param indicationInterval Interval period (ms) of the \ref IRtcEngineEventHandler::onAudioVolumeIndication "onAudioVolumeIndication" callback cycle.

     @return
     - 0: Success.
     - < 0: Failure.
     */
    virtual int startRecordingDeviceTest(int indicationInterval) = 0;

    /** Stops the microphone test.

     This method stops the microphone test. You must call this method to stop the test after calling the \ref IAudioDeviceManager::startRecordingDeviceTest "startRecordingDeviceTest" method.

     @return
     - 0: Success.
     - < 0: Failure.
     */
    virtual int stopRecordingDeviceTest() = 0;

    /** Retrieves the audio playback device associated with the device ID.

     @param deviceId Pointer to the ID of the audio playback device.
     @return
     - 0: Success.
     - < 0: Failure.
     */
    virtual int getPlaybackDevice(char deviceId[MAX_DEVICE_ID_LENGTH]) = 0;

    /** Retrieves the audio playback device information associated with the device ID and device name.

     @param deviceId Pointer to the device ID of the audio playback device.
     @param deviceName Pointer to the device name of the audio playback device.
     @return
     - 0: Success.
     - < 0: Failure.
     */
    virtual int getPlaybackDeviceInfo(char deviceId[MAX_DEVICE_ID_LENGTH], char deviceName[MAX_DEVICE_ID_LENGTH]) = 0;

    /** Retrieves the audio recording device associated with the device ID.

     @param deviceId Pointer to the device ID of the audio recording device.
     @return
     - 0: Success.
     - < 0: Failure.
     */
    virtual int getRecordingDevice(char deviceId[MAX_DEVICE_ID_LENGTH]) = 0;

    /** Retrieves the audio recording device information associated with the device ID and device name.

     @param deviceId Pointer to the device ID of the recording audio device.
     @param deviceName Pointer to the device name of the recording audio device.
     @return
     - 0: Success.
     - < 0: Failure.
     */
   virtual int getRecordingDeviceInfo(char deviceId[MAX_DEVICE_ID_LENGTH], char deviceName[MAX_DEVICE_ID_LENGTH]) = 0;

   /** Starts the audio device loopback test.

   This method tests whether the local audio devices are working properly. After calling this method, the microphone captures the local audio and plays it through the speaker. The \ref IRtcEngineEventHandler::onAudioVolumeIndication "onAudioVolumeIndication" callback returns the local audio volume information at the set interval.

   @note This method tests the local audio devices and does not report the network conditions.

   @param indicationInterval The time interval (ms) at which the \ref IRtcEngineEventHandler::onAudioVolumeIndication "onAudioVolumeIndication" callback returns.

   @return
   - 0: Success.
   - < 0: Failure.
   */
   virtual int startAudioDeviceLoopbackTest(int indicationInterval) = 0;

   /** Stops the audio device loopback test.

   @note Ensure that you call this method to stop the loopback test after calling the \ref IAudioDeviceManager::startAudioDeviceLoopbackTest "startAudioDeviceLoopbackTest" method.

   @return
   - 0: Success.
   - < 0: Failure.
   */
   virtual int stopAudioDeviceLoopbackTest() = 0;

   /** Releases all IAudioDeviceManager resources.
   */
    virtual void release() = 0;
};

/** Definition of RtcEngineContext.
*/
struct RtcEngineContext
{
    /** The IRtcEngineEventHandler object.
     */
    IRtcEngineEventHandler* eventHandler;
    /** App ID issued to you by AR. Apply for a new App ID from AR if
     * it is missing from your kit.
     */
    const char* appId;
    // For android, it the context(Activity or Application
	// for windows,Video hot plug device
    /** The video window handle. Once set, this parameter enables you to plug
     * or unplug the video devices while they are powered.
     */
    void* context;
    /**
     * The region for connection. This advanced feature applies to scenarios that have regional restrictions.
     *
     * For the regions that AR supports, see #AREA_CODE. After specifying the region, the app that integrates the AR SDK connects to the AR servers within that region.
     */
	unsigned int areaCode;
    RtcEngineContext()
    :eventHandler(NULL)
    ,appId(NULL)
    ,context(NULL)
    ,areaCode(rtc::AREA_CODE_GLOB)
    {}
};

/** Definition of IMetadataObserver
*/
class IMetadataObserver
{
public:
    /** Metadata type of the observer.
     @note We only support video metadata for now.
     */
    enum METADATA_TYPE
    {
        /** -1: the metadata type is unknown.
         */
        UNKNOWN_METADATA = -1,
        /** 0: the metadata type is video.
         */
        VIDEO_METADATA = 0,
    };

    struct Metadata
    {
        /** The User ID.

         - For the receiver: the ID of the user who sent the metadata.
         - For the sender: ignore it.
         */
        char* uid;
        /** Buffer size of the sent or received Metadata.
         */
        unsigned int size;
        /** Buffer address of the sent or received Metadata.
         */
        unsigned char *buffer;
        /** Time statmp of the frame following the metadata.
         */
        long long timeStampMs;
    };

    virtual ~IMetadataObserver() {};

    /** Occurs when the SDK requests the maximum size of the Metadata.

     The metadata includes the following parameters:
     - `uid`: ID of the user who sends the metadata.
     - `size`: The size of the sent or received metadata.
     - `buffer`: The sent or received metadata.
     - `timeStampMs`: The timestamp of the metadata.

     The SDK triggers this callback after you successfully call the \ref ar::rtc::IRtcEngine::registerMediaMetadataObserver "registerMediaMetadataObserver" method. You need to specify the maximum size of the metadata in the return value of this callback.

     @return The maximum size of the buffer of the metadata that you want to use. The highest value is 1024 bytes. Ensure that you set the return value.
     */
    virtual int getMaxMetadataSize() = 0;

    /** Occurs when the SDK is ready to receive and send metadata.

     @note Ensure that the size of the metadata does not exceed the value set in the \ref ar::rtc::IMetadataObserver::getMaxMetadataSize "getMaxMetadataSize" callback.

     @param metadata The Metadata to be sent.
     @return
     - true:  Send.
     - false: Do not send.
     */
    virtual bool onReadyToSendMetadata(Metadata &metadata) = 0;

    /** Occurs when the local user receives the metadata.

     @param metadata The received Metadata.
     */
    virtual void onMetadataReceived(const Metadata &metadata) = 0;
};

/** Encryption mode.
*/
enum ENCRYPTION_MODE
{
    /** 1: (Default) 128-bit AES encryption, XTS mode.
     */
    AES_128_XTS = 1,
    /** 2: 128-bit AES encryption, ECB mode.
     */
    AES_128_ECB = 2,
    /** 3: 256-bit AES encryption, XTS mode.
     */
    AES_256_XTS = 3,
    /** 4: 128-bit SM4 encryption, ECB mode.
     */
    SM4_128_ECB = 4,
    /** Enumerator boundary.
     */
    MODE_END,
};

/** Configurations of built-in encryption schemas. */
struct EncryptionConfig{
    /**
     * Encryption mode. The default encryption mode is `AES_128_XTS`. See #ENCRYPTION_MODE.
     */
    ENCRYPTION_MODE encryptionMode;
    /**
     * Encryption key in string type.
     *
     * @note If you do not set an encryption key or set it as NULL, you cannot use the built-in encryption, and the SDK returns #ERR_INVALID_ARGUMENT (-2).
     */
    const char* encryptionKey;

    EncryptionConfig() {
        encryptionMode = AES_128_XTS;
        encryptionKey = nullptr;
    }

    /// @cond
    const char* getEncryptionString() const {
        switch(encryptionMode)
        {
            case AES_128_XTS:
                return "aes-128-xts";
            case AES_128_ECB:
                return "aes-128-ecb";
            case AES_256_XTS:
                return "aes-256-xts";
            case SM4_128_ECB:
                return "sm4-128-ecb";
            default:
                return "aes-128-xts";
        }
        return "aes-128-xts";
    }
    /// @endcond
};

/** IRtcEngine is the base interface class of the AR SDK that provides the main AR SDK methods invoked by your application.

Enable the AR SDK's communication functionality through the creation of an IRtcEngine object, then call the methods of this object.
 */
class IRtcEngine
{
protected:
    virtual ~IRtcEngine() {}
public:

    /** Initializes the AR service.
     *
     * Ensure that you call the
     * \ref ar::rtc::IRtcEngine::createARRtcEngine
     * "createARRtcEngine" and \ref ar::rtc::IRtcEngine::initialize
     * "initialize" methods before calling any other API.
     *
     * @param context Pointer to the RTC engine context. See RtcEngineContext.
     *
     * @return
     * - 0: Success.
     * - < 0: Failure.
     *  - `ERR_INVALID_APP_ID (101)`: The app ID is invalid. Check if it is in the correct format.
     */
    virtual int initialize(const RtcEngineContext& context) = 0;

    /** Releases all IRtcEngine resources.
     *
     * Use this method for apps in which users occasionally make voice or video calls. When users do not make calls, you
     * can free up resources for other operations. Once you call `release` to destroy the created `IRtcEngine` instance,
     * you cannot use any method or callback in the SDK any more. If you want to use the real-time communication functions
     * again, you must call \ref createAgoraRtcEngine "createAgoraRtcEngine" and \ref ar::rtc::IRtcEngine::initialize "initialize"
     * to create a new `IRtcEngine` instance.
     *
     * @note If you want to create a new `IRtcEngine` instance after destroying the current one, ensure that you wait
     * till the `release` method completes executing.
     *
     * @param sync
     * - true: Synchronous call. AR suggests calling this method in a sub-thread to avoid congestion in the main thread
     * because the synchronous call and the app cannot move on to another task until the execution completes.
     * Besides, you **cannot** call this method in any method or callback of the SDK. Otherwise, the SDK cannot release the
     * resources occupied by the `IRtcEngine` instance until the callbacks return results, which may result in a deadlock.
     * The SDK automatically detects the deadlock and converts this method into an asynchronous call, causing the test to
     * take additional time.
     * - false: Asynchronous call. Do not immediately uninstall the SDK's dynamic library after the call, or it may cause
     * a crash due to the SDK clean-up thread not quitting.
     */
    virtual void release(bool sync=false) = 0;

    /** Sets the channel profile of the AR IRtcEngine.
     *
     * The AR IRtcEngine differentiates channel profiles and applies optimization algorithms accordingly.
     * For example, it prioritizes smoothness and low latency for a video call, and prioritizes video quality for the live interactive video streaming.
     *
     * @warning
     * - To ensure the quality of real-time communication, we recommend that all users in a channel use the same channel profile.
     * - Call this method before calling \ref IRtcEngine::joinChannel "joinChannel" . You cannot set the channel profile once you have joined the channel.
     * - The default audio route and video encoding bitrate are different in different channel profiles. For details, see
     * \ref IRtcEngine::setDefaultAudioRouteToSpeakerphone "setDefaultAudioRouteToSpeakerphone" and \ref IRtcEngine::setVideoEncoderConfiguration "setVideoEncoderConfiguration".
     *
     * @param profile The channel profile of the AR IRtcEngine. See #CHANNEL_PROFILE_TYPE
     * @return
     * - 0(ERR_OK): Success.
     * - < 0: Failure.
     *  - -2 (ERR_INVALID_ARGUMENT): The parameter is invalid.
     *  - -7(ERR_NOT_INITIALIZED): The SDK is not initialized.
     */
    virtual int setChannelProfile(CHANNEL_PROFILE_TYPE profile) = 0;

    /** Sets the role of the user, such as a host or an audience (default), before joining a channel in the live interactive streaming.
     *
     * This method can be used to switch the user role in the live interactive streaming after the user joins a channel.
     *
     * In the `LIVE_BROADCASTING` profile, when a user switches user roles after joining a channel, a successful \ref ar::rtc::IRtcEngine::setClientRole "setClientRole" method call triggers the following callbacks:
     * - The local client: \ref ar::rtc::IRtcEngineEventHandler::onClientRoleChanged "onClientRoleChanged"
     * - The remote client: \ref ar::rtc::IRtcEngineEventHandler::onUserJoined "onUserJoined" or \ref ar::rtc::IRtcEngineEventHandler::onUserOffline "onUserOffline" (BECOME_AUDIENCE)
     *
     * @note
     * This method applies only to the `LIVE_BROADCASTING` profile.
     *
     * @param role Sets the role of the user. See #CLIENT_ROLE_TYPE.
     *
     * @return
     * - 0(ERR_OK): Success.
     * - < 0: Failure.
     *  - -1(ERR_FAILED): A general error occurs (no specified reason).
     *  - -2(ERR_INALID_ARGUMENT): The parameter is invalid.
     *  - -7(ERR_NOT_INITIALIZED): The SDK is not initialized.
     */
    virtual int setClientRole(CLIENT_ROLE_TYPE role) = 0;
    /// @cond
    /** Sets the role of a user in a live interactive streaming.
     *
     * @since v3.2.0
     *
     * You can call this method either before or after joining the channel to set the user role as audience or host. If
     * you call this method to switch the user role after joining the channel, the SDK triggers the following callbacks:
     * - The local client: \ref IRtcEngineEventHandler::onClientRoleChanged "onClientRoleChanged".
     * - The remote client: \ref IRtcEngineEventHandler::onUserJoined "onUserJoined"
     * or \ref IRtcEngineEventHandler::onUserOffline "onUserOffline".
     *
     * @note
     * - This method applies to the `LIVE_BROADCASTING` profile only (when the `profile` parameter in
     * \ref IRtcEngine::setChannelProfile "setChannelProfile" is set as `CHANNEL_PROFILE_LIVE_BROADCASTING`).
     * - The difference between this method and \ref IRtcEngine::setClientRole(CLIENT_ROLE_TYPE) "setClientRole1" is that
     * this method can set the user level in addition to the user role.
     *  - The user role determines the permissions that the SDK grants to a user, such as permission to send local
     * streams, receive remote streams, and push streams to a CDN address.
     *  - The user level determines the level of services that a user can enjoy within the permissions of the user's
     * role. For example, an audience can choose to receive remote streams with low latency or ultra low latency. Levels
     * affect prices.
     *
     * **Example**
     * ```cpp
     * ClientRoleOptions options;
     * options.audienceLatencyLevel = AUDIENCE_LATENCY_LEVEL_ULTRA_LOW_LATENCY;
     * options.audienceLatencyLevel = AUDIENCE_LATENCY_LEVEL_LOW_LATENCY;
     * arEngine->setClientRole(role, options);
     * ```
     *
     * @param role The role of a user in a live interactive streaming. See #CLIENT_ROLE_TYPE.
     * @param options The detailed options of a user, including user level. See ClientRoleOptions.
     *
     * @return
     * - 0(ERR_OK): Success.
     * - < 0: Failure.
     *  - -1(ERR_FAILED): A general error occurs (no specified reason).
     *  - -2(ERR_INALID_ARGUMENT): The parameter is invalid.
     *  - -7(ERR_NOT_INITIALIZED): The SDK is not initialized.
     */
    virtual int setClientRole(CLIENT_ROLE_TYPE role, const ClientRoleOptions& options) = 0;
    /// @endcond
    /** Joins a channel with the user ID.

     Users in the same channel can talk to each other, and multiple users in the same channel can start a group chat. Users with different App IDs cannot call each other.


     You must call the \ref IRtcEngine::leaveChannel "leaveChannel" method to exit the current call before entering another channel.

     A successful \ref ar::rtc::IRtcEngine::joinChannel "joinChannel" method call triggers the following callbacks:
     - The local client: \ref ar::rtc::IRtcEngineEventHandler::onJoinChannelSuccess "onJoinChannelSuccess"
     - The remote client: \ref ar::rtc::IRtcEngineEventHandler::onUserJoined "onUserJoined" , if the user joining the channel is in the Communication profile, or is a BROADCASTER in the Live Broadcast profile.

     When the connection between the client and AR's server is interrupted due to poor network conditions, the SDK tries reconnecting to the server. When the local client successfully rejoins the channel, the SDK triggers the \ref ar::rtc::IRtcEngineEventHandler::onRejoinChannelSuccess "onRejoinChannelSuccess" callback on the local client.

     @note A channel does not accept duplicate uids, such as two users with the same @p uid. If you set @p uid as 0, the system automatically assigns a @p uid. If you want to join a channel from different devices, ensure that each device has a different uid.
     @warning Ensure that the App ID used for creating the token is the same App ID used by the \ref IRtcEngine::initialize "initialize" method for initializing the RTC engine. Otherwise, the CDN live streaming may fail.

     @param token Pointer to the token generated by the application server. In most circumstances, a static App ID suffices. For added security, use a Channel Key.
     - If the user uses a static App ID, *token* is optional and can be set as NULL.
     - If the user uses a Channel Key, AR issues an additional App Certificate for you to generate a user key based on the algorithm and App Certificate for user authentication on the server.
     @param channelId Pointer to the unique channel name for the AR RTC session in the string format smaller than 64 bytes. Supported characters:
     - The 26 lowercase English letters: a to z
     - The 26 uppercase English letters: A to Z
     - The 10 numbers: 0 to 9
     - The space
     - "!", "#", "$", "%", "&", "(", ")", "+", "-", ":", ";", "<", "=", ".", ">", "?", "@", "[", "]", "^", "_", " {", "}", "|", "~", ","
     @param info (Optional) Pointer to additional information about the channel. This parameter can be set to NULL or contain channel related information. Other users in the channel will not receive this message.
     @param uid (Optional) User ID. A 32-bit unsigned integer with a value ranging from 1 to 2<sup>32</sup>-1. The @p uid must be unique. If a @p uid is not assigned (or set to 0), the SDK assigns and returns a @p uid in the \ref IRtcEngineEventHandler::onJoinChannelSuccess "onJoinChannelSuccess" callback. Your application must record and maintain the returned *uid* since the SDK does not do so.

     @return
     - 0: Success.
     - < 0: Failure:
        - #ERR_INVALID_ARGUMENT (-2)
        - #ERR_NOT_READY (-3)
        - #ERR_REFUSED (-5)
     */
    virtual int joinChannel(const char* token, const char* channelId, const char* info, uid_t uid) = 0;
    /** Switches to a different channel.
     *
     * This method allows the audience of a Live-broadcast channel to switch
     * to a different channel.
     *
     * After the user successfully switches to another channel, the
     * \ref ar::rtc::IRtcEngineEventHandler::onLeaveChannel "onLeaveChannel"
     *  and \ref ar::rtc::IRtcEngineEventHandler::onJoinChannelSuccess
     * "onJoinChannelSuccess" callbacks are triggered to indicate that the
     * user has left the original channel and joined a new one.
     *
     * @note
     * This method applies to the audience role in a Live-broadcast channel
     * only.
     *
     * @param token The token generated at your server:
     * - For low-security requirements: You can use the temporary token
     * generated in Console. For details, see
     * [Get a temporary token](https://docs.ar.io/en/AR%20Platform/token?platfor%20*%20m=All%20Platforms#get-a-temporary-token).
     * - For high-security requirements: Use the token generated at your
     * server. For details, see
     * [Get a token](https://docs.ar.io/en/AR%20Platform/token?platfor%20*%20m=All%20Platforms#get-a-token).
     * @param channelId Unique channel name for the ARRTC session in the
     * string format. The string length must be less than 64 bytes. Supported
     * character scopes are:
     * - The 26 lowercase English letters: a to z.
     * - The 26 uppercase English letters: A to Z.
     * - The 10 numbers: 0 to 9.
     * - The space.
     * - "!", "#", "$", "%", "&", "(", ")", "+", "-", ":", ";", "<", "=", ".",
     * ">", "?", "@", "[", "]", "^", "_", " {", "}", "|", "~", ",".

     @return
     - 0: Success.
     - < 0: Failure.
        - #ERR_INVALID_ARGUMENT (-2)
        - #ERR_NOT_READY (-3)
        - #ERR_REFUSED (-5)
     */
    virtual int switchChannel(const char* token, const char* channelId) = 0;
    
    /** Allows a user to leave a channel, such as hanging up or exiting a call.

     After joining a channel, the user must call the *leaveChannel* method to end the call before joining another channel.

     This method returns 0 if the user leaves the channel and releases all resources related to the call.

     This method call is asynchronous, and the user has not left the channel when the method call returns. Once the user leaves the channel, the SDK triggers the \ref IRtcEngineEventHandler::onLeaveChannel "onLeaveChannel" callback.

     A successful \ref ar::rtc::IRtcEngine::leaveChannel "leaveChannel" method call triggers the following callbacks:
     - The local client: \ref ar::rtc::IRtcEngineEventHandler::onLeaveChannel "onLeaveChannel"
     - The remote client: \ref ar::rtc::IRtcEngineEventHandler::onUserOffline "onUserOffline" , if the user leaving the channel is in the Communication channel, or is a BROADCASTER in the Live Broadcast profile.

     @note
     - If you call the \ref IRtcEngine::release "release" method immediately after the *leaveChannel* method, the *leaveChannel* process interrupts, and the \ref IRtcEngineEventHandler::onLeaveChannel "onLeaveChannel" callback is not triggered.
     - If you call the *leaveChannel* method during a CDN live streaming, the SDK triggers the \ref IRtcEngine::removePublishStreamUrl "removePublishStreamUrl" method.

     @return
     - 0: Success.
     - < 0: Failure.
     */
    virtual int leaveChannel() = 0;
    
    /** Gets a new token when the current token expires after a period of time.

     The `token` expires after a period of time once the token schema is enabled when:

     - The SDK triggers the \ref IRtcEngineEventHandler::onTokenPrivilegeWillExpire "onTokenPrivilegeWillExpire" callback, or
     - The \ref IRtcEngineEventHandler::onConnectionStateChanged "onConnectionStateChanged" reports CONNECTION_CHANGED_TOKEN_EXPIRED(9).

     The application should call this method to get the new `token`. Failure to do so will result in the SDK disconnecting from the server.

     @param token Pointer to the new token.
     @return
     - 0: Success.
     - < 0: Failure.
     */
    virtual int renewToken(const char* token) = 0;

    /** Retrieves the pointer to the device manager object.

     @param iid ID of the interface.
     @param inter Pointer to the *DeviceManager* object.
     @return
     - 0: Success.
     - < 0: Failure.
     */
    virtual int queryInterface(INTERFACE_ID_TYPE iid, void** inter) = 0;

     /** Registers a user account.

     Once registered, the user account can be used to identify the local user when the user joins the channel.
     After the user successfully registers a user account, the SDK triggers the \ref ar::rtc::IRtcEngineEventHandler::onLocalUserRegistered "onLocalUserRegistered" callback on the local client,
     reporting the user ID and user account of the local user.

     To join a channel with a user account, you can choose either of the following:

     - Call the \ref ar::rtc::IRtcEngine::registerLocalUserAccount "registerLocalUserAccount" method to create a user account, and then the \ref ar::rtc::IRtcEngine::joinChannelWithUserAccount "joinChannelWithUserAccount" method to join the channel.
     - Call the \ref ar::rtc::IRtcEngine::joinChannelWithUserAccount "joinChannelWithUserAccount" method to join the channel.

     The difference between the two is that for the former, the time elapsed between calling the \ref ar::rtc::IRtcEngine::joinChannelWithUserAccount "joinChannelWithUserAccount" method
     and joining the channel is shorter than the latter.

     @note
     - Ensure that you set the `userAccount` parameter. Otherwise, this method does not take effect.
     - Ensure that the value of the `userAccount` parameter is unique in the channel.
     - To ensure smooth communication, use the same parameter type to identify the user. For example, if a user joins the channel with a user ID, then ensure all the other users use the user ID too. The same applies to the user account. If a user joins the channel with the AR Web SDK, ensure that the uid of the user is set to the same parameter type.

     @param appId The App ID of your project.
     @param userAccount The user account. The maximum length of this parameter is 255 bytes. Ensure that you set this parameter and do not set it as null. Supported character scopes are:
     - All lowercase English letters: a to z.
     - All uppercase English letters: A to Z.
     - All numeric characters: 0 to 9.
     - The space character.
     - Punctuation characters and other symbols, including: "!", "#", "$", "%", "&", "(", ")", "+", "-", ":", ";", "<", "=", ".", ">", "?", "@", "[", "]", "^", "_", " {", "}", "|", "~", ",".

     @return
     - 0: Success.
     - < 0: Failure.
    */
    virtual int registerLocalUserAccount(
        const char* appId, const char* userAccount) = 0;
    /** Joins the channel with a user account.

     After the user successfully joins the channel, the SDK triggers the following callbacks:

     - The local client: \ref ar::rtc::IRtcEngineEventHandler::onLocalUserRegistered "onLocalUserRegistered" and \ref ar::rtc::IRtcEngineEventHandler::onJoinChannelSuccess "onJoinChannelSuccess" .
     The remote client: \ref ar::rtc::IRtcEngineEventHandler::onUserJoined "onUserJoined" and \ref ar::rtc::IRtcEngineEventHandler::onUserInfoUpdated "onUserInfoUpdated" , if the user joining the channel is in the Communication profile, or is a BROADCASTER in the Live Broadcast profile.

     @note To ensure smooth communication, use the same parameter type to identify the user. For example, if a user joins the channel with a user ID, then ensure all the other users use the user ID too. The same applies to the user account.
     If a user joins the channel with the AR Web SDK, ensure that the uid of the user is set to the same parameter type.

     @param token The token generated at your server:
     - For low-security requirements: You can use the temporary token generated at Console. For details, see [Get a temporary toke](https://docs.ar.io/en/Voice/token?platform=All%20Platforms#get-a-temporary-token).
     - For high-security requirements: Set it as the token generated at your server. For details, see [Get a token](https://docs.ar.io/en/Voice/token?platform=All%20Platforms#get-a-token).
     @param channelId The channel name. The maximum length of this parameter is 64 bytes. Supported character scopes are:
      The 26 lowercase English letters: a to z.
     - The 26 uppercase English letters: A to Z.
     - The 10 numbers: 0 to 9.
     - The space.
     - "!", "#", "$", "%", "&", "(", ")", "+", "-", ":", ";", "<", "=", ".", ">", "?", "@", "[", "]", "^", "_", " {", "}", "|", "~", ",".
     @param userAccount The user account. The maximum length of this parameter is 255 bytes. Ensure that you set this parameter and do not set it as null. Supported character scopes are:
     - The 26 lowercase English letters: a to z.
     - The 26 uppercase English letters: A to Z.
     - The 10 numbers: 0 to 9.
     - The space.
     - "!", "#", "$", "%", "&", "(", ")", "+", "-", ":", ";", "<", "=", ".", ">", "?", "@", "[", "]", "^", "_", " {", "}", "|", "~", ",".

     @return
     - 0: Success.
     - < 0: Failure.
        - #ERR_INVALID_ARGUMENT (-2)
        - #ERR_NOT_READY (-3)
        - #ERR_REFUSED (-5) 
     */
    virtual int joinChannelWithUserAccount(const char* token,
                                           const char* channelId,
                                           const char* userAccount) = 0;
    
    /** Gets the user information by passing in the user account.

     After a remote user joins the channel, the SDK gets the user ID and user account of the remote user, caches them
     in a mapping table object (`userInfo`), and triggers the \ref ar::rtc::IRtcEngineEventHandler::onUserInfoUpdated "onUserInfoUpdated" callback on the local client.

     After receiving the o\ref ar::rtc::IRtcEngineEventHandler::onUserInfoUpdated "onUserInfoUpdated" callback, you can call this method to get the user ID of the
     remote user from the `userInfo` object by passing in the user account.

     @param userAccount The user account of the user. Ensure that you set this parameter.
     @param[in/out] userInfo A userInfo object that identifies the user:
     - Input: A userInfo object.
     - Output: A userInfo object that contains the user account and user ID of the user.

     @return
     - 0: Success.
     - < 0: Failure.
     */
    virtual int getUserInfoByUserAccount(const char* userAccount, UserInfo* userInfo) = 0;
    /** Gets the user information by passing in the user ID.

     After a remote user joins the channel, the SDK gets the user ID and user account of the remote user,
     caches them in a mapping table object (`userInfo`), and triggers the \ref ar::rtc::IRtcEngineEventHandler::onUserInfoUpdated "onUserInfoUpdated" callback on the local client.

     After receiving the \ref ar::rtc::IRtcEngineEventHandler::onUserInfoUpdated "onUserInfoUpdated" callback, you can call this method to get the user account of the remote user
     from the `userInfo` object by passing in the user ID.

     @param uid The user ID of the remote user. Ensure that you set this parameter.
     @param[in/out] userInfo A userInfo object that identifies the user:
     - Input: A userInfo object.
     - Output: A userInfo object that contains the user account and user ID of the user.

     @return
     - 0: Success.
     - < 0: Failure.
     */
    virtual int getUserInfoByUid(uid_t uid, UserInfo* userInfo) = 0;

    /** **DEPRECATED** Starts an audio call test.

     This method is deprecated as of v2.4.0.

     This method starts an audio call test to check whether the audio devices (for example, headset and speaker) and the network connection are working properly.

     To conduct the test:

     - The user speaks and the recording is played back within 10 seconds.
     - If the user can hear the recording within 10 seconds, the audio devices and network connection are working properly.

     @note
     - After calling this method, always call the \ref IRtcEngine::stopEchoTest "stopEchoTest" method to end the test. Otherwise, the application cannot run the next echo test.
     - In the Live-broadcast profile, only the hosts can call this method. If the user switches from the Communication to Live-broadcast profile, the user must call the \ref IRtcEngine::setClientRole "setClientRole" method to change the user role from the audience (default) to the host before calling this method.

     @return
     - 0: Success.
     - < 0: Failure.
     */
    virtual int startEchoTest() = 0;

    /** Starts an audio call test.

    This method starts an audio call test to determine whether the audio devices (for example, headset and speaker) and the network connection are working properly.

    In the audio call test, you record your voice. If the recording plays back within the set time interval, the audio devices and the network connection are working properly.

    @note
    - Call this method before joining a channel.
    - After calling this method, call the \ref IRtcEngine::stopEchoTest "stopEchoTest" method to end the test. Otherwise, the app cannot run the next echo test, or call the \ref IRtcEngine::joinChannel "joinChannel" method.
    - In the Live-broadcast profile, only a host can call this method.
    @param intervalInSeconds The time interval (s) between when you speak and when the recording plays back.

     @return
     - 0: Success.
     - < 0: Failure.
     */
  virtual int startEchoTest(int intervalInSeconds) = 0;

    /** Stops the audio call test.

     @return
     - 0: Success.
     - < 0: Failure.
     */
    virtual int stopEchoTest() = 0;

    /** Enables the video module.

     Call this method either before joining a channel or during a call. If this method is called before joining a channel, the call starts in the video mode. If this method is called during an audio call, the audio mode switches to the video mode. To disable the video module, call the \ref IRtcEngine::disableVideo "disableVideo" method.

     A successful \ref ar::rtc::IRtcEngine::enableVideo "enableVideo" method call triggers the \ref ar::rtc::IRtcEngineEventHandler::onUserEnableVideo "onUserEnableVideo" (true) callback on the remote client.
     @note
     - This method affects the internal engine and can be called after the \ref ar::rtc::IRtcEngine::leaveChannel "leaveChannel" method.
     - This method resets the internal engine and takes some time to take effect. We recommend using the following API methods to control the video engine modules separately:
         - \ref IRtcEngine::enableLocalVideo "enableLocalVideo": Whether to enable the camera to create the local video stream.
         - \ref IRtcEngine::muteLocalVideoStream "muteLocalVideoStream": Whether to publish the local video stream.
         - \ref IRtcEngine::muteRemoteVideoStream "muteRemoteVideoStream": Whether to subscribe to and play the remote video stream.
         - \ref IRtcEngine::muteAllRemoteVideoStreams "muteAllRemoteVideoStreams": Whether to subscribe to and play all remote video streams.

     @return
     - 0: Success.
     - < 0: Failure.
     */
    virtual int enableVideo() = 0;

    /** Disables the video module.

    This method can be called before joining a channel or during a call. If this method is called before joining a channel, the call starts in audio mode. If this method is called during a video call, the video mode switches to the audio mode. To enable the video module, call the \ref IRtcEngine::enableVideo "enableVideo" method.

    A successful \ref ar::rtc::IRtcEngine::disableVideo "disableVideo" method call triggers the \ref ar::rtc::IRtcEngineEventHandler::onUserEnableVideo "onUserEnableVideo" (false) callback on the remote client.
     @note
     - This method affects the internal engine and can be called after the \ref ar::rtc::IRtcEngine::leaveChannel "leaveChannel" method.
     - This method resets the internal engine and takes some time to take effect. We recommend using the following API methods to control the video engine modules separately:
         - \ref IRtcEngine::enableLocalVideo "enableLocalVideo": Whether to enable the camera to create the local video stream.
         - \ref IRtcEngine::muteLocalVideoStream "muteLocalVideoStream": Whether to publish the local video stream.
         - \ref IRtcEngine::muteRemoteVideoStream "muteRemoteVideoStream": Whether to subscribe to and play the remote video stream.
         - \ref IRtcEngine::muteAllRemoteVideoStreams "muteAllRemoteVideoStreams": Whether to subscribe to and play all remote video streams.

     @return
     - 0: Success.
     - < 0: Failure.
     */
    virtual int disableVideo() = 0;

    /** **DEPRECATED** Sets the video profile.

     This method is deprecated as of v2.3. Use the \ref IRtcEngine::setVideoEncoderConfiguration "setVideoEncoderConfiguration" method instead.

     Each video profile includes a set of parameters, such as the resolution, frame rate, and bitrate. If the camera device does not support the specified resolution, the SDK automatically chooses a suitable camera resolution, keeping the encoder resolution specified by the *setVideoProfile* method.

     @note
     - If you do not need to set the video profile after joining the channel, call this method before the \ref IRtcEngine::enableVideo "enableVideo" method to reduce the render time of the first video frame.
     - Always set the video profile before calling the \ref IRtcEngine::joinChannel "joinChannel" or \ref IRtcEngine::startPreview "startPreview" method.

     @param profile Sets the video profile. See #VIDEO_PROFILE_TYPE.
     @param swapWidthAndHeight Sets whether to swap the width and height of the video stream:
     - true: Swap the width and height.
     - false: (Default) Do not swap the width and height.
     The width and height of the output video are consistent with the set video profile.
     @note Since the landscape or portrait mode of the output video can be decided directly by the video profile, We recommend setting *swapWidthAndHeight* to *false* (default).

     @return
     - 0: Success.
     - < 0: Failure.
     */
    virtual int setVideoProfile(VIDEO_PROFILE_TYPE profile, bool swapWidthAndHeight) = 0;

    /** Sets the video encoder configuration.

     Each video encoder configuration corresponds to a set of video parameters, including the resolution, frame rate, bitrate, and video orientation.

     The parameters specified in this method are the maximum values under ideal network conditions. If the video engine cannot render the video using the specified parameters due to poor network conditions, the parameters further down the list are considered until a successful configuration is found.

     @note If you do not need to set the video encoder configuration after joining the channel, you can call this method before the \ref IRtcEngine::enableVideo "enableVideo" method to reduce the render time of the first video frame.

     @param config Sets the local video encoder configuration. See VideoEncoderConfiguration.
     @return
     - 0: Success.
     - < 0: Failure.
     */
    virtual int setVideoEncoderConfiguration(const VideoEncoderConfiguration& config) = 0;
    /** Sets the camera capture configuration.

     For a video call or live broadcast, generally the SDK controls the camera output parameters. When the default camera capturer settings do not meet special requirements or cause performance problems, we recommend using this method to set the camera capturer configuration:

     - If the resolution or frame rate of the captured raw video data are higher than those set by \ref IRtcEngine::setVideoEncoderConfiguration "setVideoEncoderConfiguration", processing video frames requires extra CPU and RAM usage and degrades performance. We recommend setting config as CAPTURER_OUTPUT_PREFERENCE_PERFORMANCE = 1 to avoid such problems.
     - If you do not need local video preview or are willing to sacrifice preview quality, we recommend setting config as CAPTURER_OUTPUT_PREFERENCE_PERFORMANCE = 1 to optimize CPU and RAM usage.
     - If you want better quality for the local video preview, we recommend setting config as CAPTURER_OUTPUT_PREFERENCE_PREVIEW = 2.

     @note Call this method before enabling the local camera. That said, you can call this method before calling \ref ar::rtc::IRtcEngine::joinChannel "joinChannel", \ref ar::rtc::IRtcEngine::enableVideo "enableVideo", or \ref IRtcEngine::enableLocalVideo "enableLocalVideo", depending on which method you use to turn on your local camera.

     @param config Sets the camera capturer configuration. See CameraCapturerConfiguration.

     @return
     - 0: Success.
     - < 0: Failure.
     */
    virtual int setCameraCapturerConfiguration(const CameraCapturerConfiguration& config) = 0;

    /** Initializes the local video view.

     This method initializes the video view of a local stream on the local device. It affects only the video view that the local user sees, not the published local video stream.
     
     Call this method to bind the local video stream to a video view and to set the rendering and mirror modes of the video view.
     The binding is still valid after the user leaves the channel, which means that the window still displays. To unbind the view, set the *view* in VideoCanvas to NULL.
     
     @note 
     - Call this method before joining a channel.
     - To update the rendering or mirror mode of the local video view during a call, use the \ref IRtcEngine::setLocalRenderMode "setLocalRenderMode" method.
     @param canvas Pointer to the local video view and settings. See VideoCanvas.
     @return
     - 0: Success.
     - < 0: Failure.
     */
    virtual int setupLocalVideo(const VideoCanvas& canvas) = 0;

    /** Initializes the video view of a remote user.

     This method initializes the video view of a remote stream on the local device. It affects only the video view that the local user sees.

     Call this method to bind the remote video stream to a video view and to set the rendering and mirror modes of the video view.

     The application specifies the uid of the remote video in this method before the remote user joins the channel. If the remote uid is unknown to the application, set it after the application receives the \ref IRtcEngineEventHandler::onUserJoined "onUserJoined" callback.
     If the Video Recording function is enabled, the Video Recording Service joins the channel as a dummy client, causing other clients to also receive the \ref IRtcEngineEventHandler::onUserJoined "onUserJoined" callback. Do not bind the dummy client to the application view because the dummy client does not send any video streams. If your application does not recognize the dummy client, bind the remote user to the view when the SDK triggers the \ref IRtcEngineEventHandler::onFirstRemoteVideoDecoded "onFirstRemoteVideoDecoded" callback.
     To unbind the remote user from the view, set the view in VideoCanvas to NULL. Once the remote user leaves the channel, the SDK unbinds the remote user.

     @note To update the rendering or mirror mode of the remote video view during a call, use the \ref IRtcEngine::setRemoteRenderMode "setRemoteRenderMode" method.

     @param canvas Pointer to the remote video view and settings. See VideoCanvas.
     @return
     - 0: Success.
     - < 0: Failure.
     */
    virtual int setupRemoteVideo(const VideoCanvas& canvas) = 0;

    /** Starts the local video preview before joining the channel.

     Before calling this method, you must:

     - Call the \ref IRtcEngine::setupLocalVideo "setupLocalVideo" method to set up the local preview window and configure the attributes.
     - Call the \ref IRtcEngine::enableVideo "enableVideo" method to enable video.

     @note Once the startPreview method is called to start the local video preview, if you leave the channel by calling the \ref IRtcEngine::leaveChannel "leaveChannel" method, the local video preview remains until you call the \ref IRtcEngine::stopPreview "stopPreview" method to disable it.

     @return
     - 0: Success.
     - < 0: Failure.
     */
    virtual int startPreview() = 0;

    /** Prioritizes a remote user's stream.

    Use this method with the \ref IRtcEngine::setRemoteSubscribeFallbackOption "setRemoteSubscribeFallbackOption" method. If the fallback function is enabled for a subscribed stream, the SDK ensures the high-priority user gets the best possible stream quality.

    @note The AR SDK supports setting @p userPriority as high for one user only.

    @param  uid  The ID of the remote user.
    @param  userPriority Sets the priority of the remote user. See #PRIORITY_TYPE.

     @return
     - 0: Success.
     - < 0: Failure.
     */
    virtual int setRemoteUserPriority(uid_t uid, PRIORITY_TYPE userPriority) = 0;

    /** Stops the local video preview and disables video.

     @return
     - 0: Success.
     - < 0: Failure.
     */
    virtual int stopPreview() = 0;

    /** Enables the audio module.

    The audio mode is enabled by default.

     @note
     - This method affects the internal engine and can be called after the \ref ar::rtc::IRtcEngine::leaveChannel "leaveChannel" method. You can call this method either before or after joining a channel.
     - This method resets the internal engine and takes some time to take effect. We recommend using the following API methods to control the audio engine modules separately:
         - \ref IRtcEngine::enableLocalAudio "enableLocalAudio": Whether to enable the microphone to create the local audio stream.
         - \ref IRtcEngine::muteLocalAudioStream "muteLocalAudioStream": Whether to publish the local audio stream.
         - \ref IRtcEngine::muteRemoteAudioStream "muteRemoteAudioStream": Whether to subscribe to and play the remote audio stream.
         - \ref IRtcEngine::muteAllRemoteAudioStreams "muteAllRemoteAudioStreams": Whether to subscribe to and play all remote audio streams.

    @return
    - 0: Success.
    - < 0: Failure.
    */
    virtual int enableAudio() = 0;

    /** Disables/Re-enables the local audio function.

    The audio function is enabled by default. This method disables or re-enables the local audio function, that is, to stop or restart local audio capturing.

    This method does not affect receiving or playing the remote audio streams,and enableLocalAudio(false) is applicable to scenarios where the user wants to 
    receive remote audio streams without sending any audio stream to other users in the channel.

    The SDK triggers the \ref IRtcEngineEventHandler::onMicrophoneEnabled "onMicrophoneEnabled" callback once the local audio function is disabled or enabled.

     @note
     - Call this method after the \ref IRtcEngine::joinChannel "joinChannel" method.
     - This method is different from the \ref ar::rtc::IRtcEngine::muteLocalAudioStream "muteLocalAudioStream" method:

        - \ref ar::rtc::IRtcEngine::enableLocalAudio "enableLocalAudio": Disables/Re-enables the local audio capturing and processing. 
        If you disable or re-enable local audio recording using the `enableLocalAudio` method, the local user may hear a pause in the remote audio playback.
        - \ref ar::rtc::IRtcEngine::muteLocalAudioStream "muteLocalAudioStream": Sends/Stops sending the local audio streams.
     
     @param enabled Sets whether to disable/re-enable the local audio function:
     - true: (Default) Re-enable the local audio function, that is, to start the local audio capturing device (for example, the microphone).
     - false: Disable the local audio function, that is, to stop local audio capturing.

     @return
     - 0: Success.
     - < 0: Failure.
     */
    virtual int enableLocalAudio(bool enabled) = 0;

    /** Disables the audio module.

     @note
     - This method affects the internal engine and can be called after the \ref ar::rtc::IRtcEngine::leaveChannel "leaveChannel" method. You can call this method either before or after joining a channel.
     - This method resets the internal engine and takes some time to take effect. We recommend using the \ref ar::rtc::IRtcEngine::enableLocalAudio "enableLocalAudio" and \ref ar::rtc::IRtcEngine::muteLocalAudioStream "muteLocalAudioStream" methods to capture, process, and send the local audio streams.

     @return
     - 0: Success.
     - < 0: Failure.
     */
    virtual int disableAudio() = 0;

	/** Sets the audio parameters and application scenarios.

     @note
     - The *setAudioProfile* method must be called before the \ref IRtcEngine::joinChannel "joinChannel" method.
     - In the Communication and Live-broadcast profiles, the bitrate may be different from your settings due to network self-adaptation.
     - In scenarios requiring high-quality audio, for example, a music teaching scenario, we recommend setting profile as AUDIO_PROFILE_MUSIC_HIGH_QUALITY (4) and  scenario as AUDIO_SCENARIO_GAME_STREAMING (3).

     @param  profile Sets the sample rate, bitrate, encoding mode, and the number of channels. See #AUDIO_PROFILE_TYPE.
     @param  scenario Sets the audio application scenario. See #AUDIO_SCENARIO_TYPE. Under different audio scenarios, the device uses different volume tracks, i.e. either the in-call volume or the media volume. For details, see [What is the difference between the in-call volume and the media volume?](https://docs.ar.io/en/faq/system_volume).

     @return
     - 0: Success.
     - < 0: Failure.
     */
    virtual int setAudioProfile(AUDIO_PROFILE_TYPE profile, AUDIO_SCENARIO_TYPE scenario) = 0;
    /** Stops/Resumes sending the local audio stream.

     A successful \ref ar::rtc::IRtcEngine::muteLocalAudioStream "muteLocalAudioStream" method call triggers the \ref ar::rtc::IRtcEngineEventHandler::onUserMuteAudio "onUserMuteAudio" callback on the remote client.
     @note 
     - When @p mute is set as @p true, this method does not disable the microphone, which does not affect any ongoing recording.
     - If you call \ref ar::rtc::IRtcEngine::setChannelProfile "setChannelProfile" after this method, the SDK resets whether or not to mute the local audio according to the channel profile and user role. Therefore, we recommend calling this method after the `setChannelProfile` method.

     @param mute Sets whether to send/stop sending the local audio stream:
     - true: Stops sending the local audio stream.
     - false: (Default) Sends the local audio stream.

     @return
     - 0: Success.
     - < 0: Failure.
     */
    virtual int muteLocalAudioStream(bool mute) = 0;
    /** Stops/Resumes receiving all remote users' audio streams.

     @param mute Sets whether to receive/stop receiving all remote users' audio streams.
     - true: Stops receiving all remote users' audio streams.
     - false: (Default) Receives all remote users' audio streams.

     @return
     - 0: Success.
     - < 0: Failure.
     */
	virtual int muteAllRemoteAudioStreams(bool mute) = 0;
    /** Stops/Resumes receiving all remote users' audio streams by default.

     You can call this method either before or after joining a channel. If you call `setDefaultMuteAllRemoteAudioStreams (true)` after joining a channel, the remote audio streams of all subsequent users are not received.

     @note If you want to resume receiving the audio stream, call \ref ar::rtc::IRtcEngine::muteRemoteAudioStream "muteRemoteAudioStream (false)",
     and specify the ID of the remote user whose audio stream you want to receive.
     To receive the audio streams of multiple remote users, call `muteRemoteAudioStream (false)` as many times.
     Calling `setDefaultMuteAllRemoteAudioStreams (false)` resumes receiving the audio streams of subsequent users only.

     @param mute Sets whether to receive/stop receiving all remote users' audio streams by default:
     - true:  Stops receiving all remote users' audio streams by default.
     - false: (Default) Receives all remote users' audio streams by default.

     @return
     - 0: Success.
     - < 0: Failure.
     */
	virtual int setDefaultMuteAllRemoteAudioStreams(bool mute) = 0;
    
    /** Adjusts the playback volume of a specified remote user.

     You can call this method as many times as necessary to adjust the playback volume of different remote users, or to repeatedly adjust the playback volume of the same remote user.
     
     @note
     - Call this method after joining a channel.
     - The playback volume here refers to the mixed volume of a specified remote user.
     - This method can only adjust the playback volume of one specified remote user at a time. To adjust the playback volume of different remote users, call the method as many times, once for each remote user.

     @param uid The ID of the remote user.
     @param volume The playback volume of the specified remote user. The value ranges from 0 to 100:
     - 0: Mute.
     - 100: Original volume.

     @return
     - 0: Success.
	 - < 0: Failure. 
     */
    virtual int adjustUserPlaybackSignalVolume(AR::uid_t uid, int volume) = 0;
	/** Stops/Resumes receiving a specified remote user's audio stream.

	 @note If you called the \ref ar::rtc::IRtcEngine::muteAllRemoteAudioStreams "muteAllRemoteAudioStreams" method and set @p mute as @p true to stop receiving all remote users' audio streams, call the *muteAllRemoteAudioStreams* method and set @p mute as @p false before calling this method. The *muteAllRemoteAudioStreams* method sets all remote audio streams, while the *muteRemoteAudioStream* method sets a specified remote audio stream.

	 @param userId User ID of the specified remote user sending the audio.
	 @param mute Sets whether to receive/stop receiving a specified remote user's audio stream:
	 - true: Stops receiving the specified remote user's audio stream.
	 - false: (Default) Receives the specified remote user's audio stream.

	 @return
	 - 0: Success.
	 - < 0: Failure.

	 */
	virtual int muteRemoteAudioStream(uid_t userId, bool mute) = 0;
    /** Stops/Resumes sending the local video stream.

     A successful \ref ar::rtc::IRtcEngine::muteLocalVideoStream "muteLocalVideoStream" method call triggers the \ref ar::rtc::IRtcEngineEventHandler::onUserMuteVideo "onUserMuteVideo" callback on the remote client.
     
     @note 
     - When set to *true*, this method does not disable the camera which does not affect the retrieval of the local video streams. This method executes faster than the \ref ar::rtc::IRtcEngine::enableLocalVideo "enableLocalVideo" method which controls the sending of the local video stream.
     - If you call \ref ar::rtc::IRtcEngine::setChannelProfile "setChannelProfile" after this method, the SDK resets whether or not to mute the local video according to the channel profile and user role. Therefore, we recommend calling this method after the `setChannelProfile` method.

     @param mute Sets whether to send/stop sending the local video stream:
     - true: Stop sending the local video stream.
     - false: (Default) Send the local video stream.

     @return
     - 0: Success.
     - < 0: Failure.
     */
	virtual int muteLocalVideoStream(bool mute) = 0;
    /** Enables/Disables the local video capture.

     This method disables or re-enables the local video capturer, and does not affect receiving the remote video stream.

     After you call the \ref ar::rtc::IRtcEngine::enableVideo "enableVideo" method, the local video capturer is enabled by default. You can call \ref ar::rtc::IRtcEngine::enableLocalVideo "enableLocalVideo(false)" to disable the local video capturer. If you want to re-enable it, call \ref ar::rtc::IRtcEngine::enableLocalVideo "enableLocalVideo(true)".

     After the local video capturer is successfully disabled or re-enabled, the SDK triggers the \ref ar::rtc::IRtcEngineEventHandler::onUserEnableLocalVideo "onUserEnableLocalVideo" callback on the remote client.
     
     @note This method affects the internal engine and can be called after the \ref ar::rtc::IRtcEngine::leaveChannel "leaveChannel" method.

     @param enabled Sets whether to disable/re-enable the local video, including the capturer, renderer, and sender:
     - true: (Default) Re-enable the local video.
     - false: Disable the local video. Once the local video is disabled, the remote users can no longer receive the video stream of this user, while this user can still receive the video streams of the other remote users.

     @return
     - 0: Success.
     - < 0: Failure.
     */
	virtual int enableLocalVideo(bool enabled) = 0;
    /** Stops/Resumes receiving all video stream from a specified remote user.

     @param  mute Sets whether to receive/stop receiving all remote users' video streams:
     - true: Stop receiving all remote users' video streams.
     - false: (Default) Receive all remote users' video streams.

     @return
     - 0: Success.
     - < 0: Failure.
     */
	virtual int muteAllRemoteVideoStreams(bool mute) = 0;
    /** Stops/Resumes receiving all remote users' video streams by default.

     @param mute Sets whether to receive/stop receiving all remote users' video streams by default:
     - true: Stop receiving all remote users' video streams by default.
     - false: (Default) Receive all remote users' video streams by default.

     @return
     - 0: Success.
     - < 0: Failure.
     */
	virtual int setDefaultMuteAllRemoteVideoStreams(bool mute) = 0;
    /** Stops/Resumes receiving the video stream from a specified remote user.

     @note If you called the \ref ar::rtc::IRtcEngine::muteAllRemoteVideoStreams "muteAllRemoteVideoStreams" method and set @p mute as @p true to stop receiving all remote video streams, call the *muteAllRemoteVideoStreams* method and set @p mute as @p false before calling this method.

     @param userId User ID of the specified remote user.
     @param mute Sets whether to stop/resume receiving the video stream from a specified remote user:
     - true: Stop receiving the specified remote user's video stream.
     - false: (Default) Receive the specified remote user's video stream.

     @return
     - 0: Success.
     - < 0: Failure.
     */
	virtual int muteRemoteVideoStream(uid_t userId, bool mute) = 0;
    /** Sets the remote user's video stream type received by the local user when the remote user sends dual streams.

     This method allows the application to adjust the corresponding video-stream type based on the size of the video window to reduce the bandwidth and resources.

     - If the remote user enables the dual-stream mode by calling the \ref ar::rtc::IRtcEngine::enableDualStreamMode "enableDualStreamMode" method, the SDK receives the high-stream video by default.
     - If the dual-stream mode is not enabled, the SDK receives the high-stream video by default.

     The method result returns in the \ref ar::rtc::IRtcEngineEventHandler::onApiCallExecuted "onApiCallExecuted" callback. The SDK receives the high-stream video by default to reduce the bandwidth. If needed, users may use this method to switch to the low-stream video.
     By default, the aspect ratio of the low-stream video is the same as the high-stream video. Once the resolution of the high-stream video is set, the system automatically sets the resolution, frame rate, and bitrate of the low-stream video.

     @param userId ID of the remote user sending the video stream.
     @param streamType  Sets the video-stream type. See #REMOTE_VIDEO_STREAM_TYPE.
     @return
     - 0: Success.
     - < 0: Failure.
     */
	virtual int setRemoteVideoStreamType(uid_t userId, REMOTE_VIDEO_STREAM_TYPE streamType) = 0;
    /** Sets the default video-stream type for the video received by the local user when the remote user sends dual streams.

     - If the dual-stream mode is enabled by calling the \ref ar::rtc::IRtcEngine::enableDualStreamMode "enableDualStreamMode" method, the user receives the high-stream video by default. The @p setRemoteDefaultVideoStreamType method allows the application to adjust the corresponding video-stream type according to the size of the video window, reducing the bandwidth and resources.
     - If the dual-stream mode is not enabled, the user receives the high-stream video by default.

     The result after calling this method is returned in the \ref ar::rtc::IRtcEngineEventHandler::onApiCallExecuted "onApiCallExecuted" callback. The AR SDK receives the high-stream video by default to reduce the bandwidth. If needed, users can switch to the low-stream video through this method.

     @param streamType Sets the default video-stream type. See #REMOTE_VIDEO_STREAM_TYPE.

     @return
     - 0: Success.
     - < 0: Failure.
     */
	virtual int setRemoteDefaultVideoStreamType(REMOTE_VIDEO_STREAM_TYPE streamType) = 0;

    /** Enables the \ref ar::rtc::IRtcEngineEventHandler::onAudioVolumeIndication "onAudioVolumeIndication" callback at a set time interval to report on which users are speaking and the speakers' volume.

     Once this method is enabled, the SDK returns the volume indication in the \ref ar::rtc::IRtcEngineEventHandler::onAudioVolumeIndication "onAudioVolumeIndication" callback at the set time interval, whether or not any user is speaking in the channel.

     @param interval Sets the time interval between two consecutive volume indications:
     - &le; 0: Disables the volume indication.
     - > 0: Time interval (ms) between two consecutive volume indications. We recommend setting @p interval &gt; 200 ms. Do not set @p interval &lt; 10 ms, or the *onAudioVolumeIndication* callback will not be triggered.
     @param smooth  Smoothing factor sets the sensitivity of the audio volume indicator. The value ranges between 0 and 10. The greater the value, the more sensitive the indicator. The recommended value is 3.
     @param report_vad
     
     - true: Enable the voice activity detection of the local user. Once it is enabled, the `vad` parameter of the `onAudioVolumeIndication` callback reports the voice activity status of the local user.
     - false: (Default) Disable the voice activity detection of the local user. Once it is disabled, the `vad` parameter of the `onAudioVolumeIndication` callback does not report the voice activity status of the local user, except for the scenario where the engine automatically detects the voice activity of the local user.
     @return
     - 0: Success.
     - < 0: Failure.
     */
	virtual int enableAudioVolumeIndication(int interval, int smooth, bool report_vad) = 0;
    /** **DEPRECATED** Starts an audio recording.
     * Use \ref IRtcEngine::startAudioRecording(const char* filePath, int sampleRate, AUDIO_RECORDING_QUALITY_TYPE quality) "startAudioRecording"2 instead.

     The SDK allows recording during a call. Supported formats:

     - .wav: Large file size with high fidelity.
     - .aac: Small file size with low fidelity.

     This method has a fixed sample rate of 32 kHz.

     Ensure that the directory to save the recording file exists and is writable.
     This method is usually called after the \ref ar::rtc::IRtcEngine::joinChannel "joinChannel" method.
     The recording automatically stops when the \ref ar::rtc::IRtcEngine::leaveChannel "leaveChannel" method is called.

     @param filePath Pointer to the absolute file path of the recording file. The string of the file name is in UTF-8.
     @param quality Sets the audio recording quality. See #AUDIO_RECORDING_QUALITY_TYPE.

     @return
     - 0: Success.
     - < 0: Failure.
     */
	virtual int startAudioRecording(const char* filePath, AUDIO_RECORDING_QUALITY_TYPE quality) = 0;

    /** Starts an audio recording on the client.
     * 
     * The SDK allows recording during a call. After successfully calling this method, you can record the audio of all the users in the channel and get an audio recording file. 
     * Supported formats of the recording file are as follows:
     * - .wav: Large file size with high fidelity.
     * - .aac: Small file size with low fidelity.
     * 
     * @note
     * - Ensure that the directory you use to save the recording file exists and is writable.
     * - This method is usually called after the `joinChannel` method. The recording automatically stops when you call the `leaveChannel` method.
     * - For better recording effects, set quality as #AUDIO_RECORDING_QUALITY_MEDIUM or #AUDIO_RECORDING_QUALITY_HIGH when `sampleRate` is 44.1 kHz or 48 kHz.
     * 
     * @param filePath Pointer to the absolute file path of the recording file. The string of the file name is in UTF-8.
     * @param sampleRate Sample rate (kHz) of the recording file. Supported values are as follows:
     * - 16
     * - (Default) 32
     * - 44.1
     * - 48
     * @param quality Sets the audio recording quality. See #AUDIO_RECORDING_QUALITY_TYPE.
     * 
     * @return
     * - 0: Success.
     * - < 0: Failure.
     */
	virtual int startAudioRecording(const char* filePath, int sampleRate, AUDIO_RECORDING_QUALITY_TYPE quality) = 0;
    /** Stops an audio recording on the client.

     You can call this method before calling the \ref ar::rtc::IRtcEngine::leaveChannel "leaveChannel" method else, the recording automatically stops when the \ref ar::rtc::IRtcEngine::leaveChannel "leaveChannel" method is called.

     @return
     - 0: Success
     - < 0: Failure.
     */
	virtual int stopAudioRecording() = 0;
    /** Starts playing and mixing the music file.

     This method mixes the specified local audio file with the audio stream from the microphone, or replaces the microphone's audio stream with the specified local audio file. You can choose whether the other user can hear the local audio playback and specify the number of playback loops. This method also supports online music playback.

     When the audio mixing file playback finishes after calling this method, the SDK triggers the \ref ar::rtc::IRtcEngineEventHandler::onAudioMixingFinished "onAudioMixingFinished" callback.

     A successful \ref ar::rtc::IRtcEngine::startAudioMixing "startAudioMixing" method call triggers the \ref ar::rtc::IRtcEngineEventHandler::onAudioMixingStateChanged "onAudioMixingStateChanged" (PLAY) callback on the local client.

     When the audio mixing file playback finishes, the SDK triggers the \ref ar::rtc::IRtcEngineEventHandler::onAudioMixingStateChanged "onAudioMixingStateChanged" (STOPPED) callback on the local client.
     @note
     - Call this method when you are in a channel.
     - If the local audio mixing file does not exist, or if the SDK does not support the file format or cannot access the music file URL, the SDK returns WARN_AUDIO_MIXING_OPEN_ERROR = 701.

     @param filePath Pointer to the absolute path of the local or online audio file to mix. Supported audio formats: 3GP, ASF, ADTS, AVI, MP3, MPEG-4, SAMI, and WAVE. For more information, see [Supported Media Formats in Media Foundation](https://docs.microsoft.com/en-us/windows/desktop/medfound/supported-media-formats-in-media-foundation).
     @param loopback Sets which user can hear the audio mixing:
     - true: Only the local user can hear the audio mixing.
     - false: Both users can hear the audio mixing.
     @param replace Sets the audio mixing content:
     - true: Only the specified audio file is published; the audio stream received by the microphone is not published.
     - false: The local audio file is mixed with the audio stream from the microphone.
     @param cycle Sets the number of playback loops:
     - Positive integer: Number of playback loops.
     - -1: Infinite playback loops.

     @return
     - 0: Success.
     - < 0: Failure.
     */
	virtual int startAudioMixing(const char* filePath, bool loopback, bool replace, int cycle) = 0;
    /** Stops playing and mixing the music file.

     Call this method when you are in a channel.

     @return
     - 0: Success.
     - < 0: Failure.
     */
	virtual int stopAudioMixing() = 0;
    /** Pauses playing and mixing the music file.

     Call this method when you are in a channel.

     @return
     - 0: Success.
     - < 0: Failure.
     */
	virtual int pauseAudioMixing() = 0;
    /** Resumes playing and mixing the music file.

     Call this method when you are in a channel.

     @return
     - 0: Success.
     - < 0: Failure.
     */
	virtual int resumeAudioMixing() = 0;
    /** **DEPRECATED** AR does not recommend using this method.

     Sets the high-quality audio preferences. Call this method and set all parameters before joining a channel.

     Do not call this method again after joining a channel.

     @param fullband Sets whether to enable/disable full-band codec (48-kHz sample rate). Not compatible with SDK versions before v1.7.4:
     - true: Enable full-band codec.
     - false: Disable full-band codec.
     @param  stereo Sets whether to enable/disable stereo codec. Not compatible with SDK versions before v1.7.4:
     - true: Enable stereo codec.
     - false: Disable stereo codec.
     @param fullBitrate Sets whether to enable/disable high-bitrate mode. Recommended in voice-only mode:
     - true: Enable high-bitrate mode.
     - false: Disable high-bitrate mode.

     @return
     - 0: Success.
     - < 0: Failure.
     */
    virtual int setHighQualityAudioParameters(bool fullband, bool stereo, bool fullBitrate) = 0;
    /** Adjusts the volume during audio mixing.

     Call this method when you are in a channel.

     @note Calling this method does not affect the volume of audio effect file playback invoked by the \ref ar::rtc::IRtcEngine::playEffect "playEffect" method.

     @param volume Audio mixing volume. The value ranges between 0 and 100 (default).

     @return
     - 0: Success.
     - < 0: Failure.
     */
	virtual int adjustAudioMixingVolume(int volume) = 0;
    /** Adjusts the audio mixing volume for local playback.

     @note Call this method when you are in a channel.

     @param volume Audio mixing volume for local playback. The value ranges between 0 and 100 (default).

     @return
     - 0: Success.
     - < 0: Failure.
     */
    virtual int adjustAudioMixingPlayoutVolume(int volume) = 0;
    /** Retrieves the audio mixing volume for local playback.

     This method helps troubleshoot audio volume related issues.

     @note Call this method when you are in a channel.

     @return
     - &ge; 0: The audio mixing volume, if this method call succeeds. The value range is [0,100].
     - < 0: Failure.
     */
    virtual int getAudioMixingPlayoutVolume() = 0;
    /** Adjusts the audio mixing volume for publishing (for remote users).

     @note Call this method when you are in a channel.

     @param volume Audio mixing volume for publishing. The value ranges between 0 and 100 (default).

     @return
     - 0: Success.
     - < 0: Failure.
     */
    virtual int adjustAudioMixingPublishVolume(int volume) = 0;
    /** Retrieves the audio mixing volume for publishing.

     This method helps troubleshoot audio volume related issues.

     @note Call this method when you are in a channel.

     @return
     - &ge; 0: The audio mixing volume for publishing, if this method call succeeds. The value range is [0,100].
     - < 0: Failure.
     */
    virtual int getAudioMixingPublishVolume() = 0;

    /** Retrieves the duration (ms) of the music file.

     Call this method when you are in a channel.

     @return
     - &ge; 0: The audio mixing duration, if this method call succeeds.
     - < 0: Failure.
     */
	virtual int getAudioMixingDuration() = 0;
    /** Retrieves the playback position (ms) of the music file.

     Call this method when you are in a channel.

     @return
     - &ge; 0: The current playback position of the audio mixing, if this method call succeeds.
     - < 0: Failure.
     */
	virtual int getAudioMixingCurrentPosition() = 0;
    /** Sets the playback position of the music file to a different starting position (the default plays from the beginning).

     @param pos The playback starting position (ms) of the music file.

     @return
     - 0: Success.
     - < 0: Failure.
     */
    virtual int setAudioMixingPosition(int pos /*in ms*/) = 0;
    /** Sets the pitch of the local music file.
     * @since v3.0.1
     *
     * When a local music file is mixed with a local human voice, call this method to set the pitch of the local music file only.
     *
     * @note
     * Call this method after calling `startAudioMixing`.
     *
     * @param pitch Sets the pitch of the local music file by chromatic scale. The default value is 0,
     * which means keeping the original pitch. The value ranges from -12 to 12, and the pitch value between
     * consecutive values is a chromatic value. The greater the absolute value of this parameter, the
     * higher or lower the pitch of the local music file.
     *
     * @return
     * - 0: Success.
     * - < 0: Failure.
     */
    virtual int setAudioMixingPitch(int pitch) = 0;
    /** Retrieves the volume of the audio effects.

     The value ranges between 0.0 and 100.0.

     @return
     - &ge; 0: Volume of the audio effects, if this method call succeeds.

     - < 0: Failure.
     */
	virtual int getEffectsVolume() = 0;
    /** Sets the volume of the audio effects.

     @param volume Sets the volume of the audio effects. The value ranges between 0 and 100 (default).

     @return
     - 0: Success.
     - < 0: Failure.
     */
	virtual int setEffectsVolume(int volume) = 0;
    /** Sets the volume of a specified audio effect.

     @param soundId ID of the audio effect. Each audio effect has a unique ID.
     @param volume Sets the volume of the specified audio effect. The value ranges between 0 and 100 (default).

     @return
     - 0: Success.
     - < 0: Failure.
     */
	virtual int setVolumeOfEffect(int soundId, int volume) = 0;

#if defined(__ANDROID__) || (defined(__APPLE__) && TARGET_OS_IOS)
    /**
     * Enables/Disables face detection for the local user. Applies to Android and iOS only.
     * @since v3.0.1
     *
     * Once face detection is enabled, the SDK triggers the \ref IRtcEngineEventHandler::onFacePositionChanged "onFacePositionChanged" callback
     * to report the face information of the local user, which includes the following aspects:
     * - The width and height of the local video.
     * - The position of the human face in the local video.
     * - The distance between the human face and the device screen.
     *
     * @param enable Determines whether to enable the face detection function for the local user:
     * - true: Enable face detection.
     * - false: (Default) Disable face detection.
     * @return
     * - 0: Success.
     * - < 0: Failure.
     */
    virtual int enableFaceDetection(bool enable) = 0;
#endif
    /** Plays a specified local or online audio effect file.

     This method allows you to set the loop count, pitch, pan, and gain of the audio effect file, as well as whether the remote user can hear the audio effect.

     To play multiple audio effect files simultaneously, call this method multiple times with different soundIds and filePaths. We recommend playing no more than three audio effect files at the same time.

     @param soundId ID of the specified audio effect. Each audio effect has a unique ID.

     @note
     - If the audio effect is preloaded into the memory through the \ref IRtcEngine::preloadEffect "preloadEffect" method, the value of @p soundID must be the same as that in the *preloadEffect* method.
     - Playing multiple online audio effect files simultaneously is not supported on macOS and Windows.

     @param filePath The absolute path to the local audio effect file or the URL of the online audio effect file.
     @param loopCount Sets the number of times the audio effect loops:
     - 0: Play the audio effect once.
     - 1: Play the audio effect twice.
     - -1: Play the audio effect in an indefinite loop until the \ref IRtcEngine::stopEffect "stopEffect" or \ref IRtcEngine::stopAllEffects "stopAllEffects" method is called.
     @param pitch Sets the pitch of the audio effect. The value ranges between 0.5 and 2. The default value is 1 (no change to the pitch). The lower the value, the lower the pitch.
     @param pan Sets the spatial position of the audio effect. The value ranges between -1.0 and 1.0:
     - 0.0: The audio effect displays ahead.
     - 1.0: The audio effect displays to the right.
     - -1.0: The audio effect displays to the left.
     @param gain  Sets the volume of the audio effect. The value ranges between 0 and 100 (default). The lower the value, the lower the volume of the audio effect.
     @param publish Sets whether or not to publish the specified audio effect to the remote stream:
     - true: The locally played audio effect is published to the AR Cloud and the remote users can hear it.
     - false: The locally played audio effect is not published to the AR Cloud and the remote users cannot hear it.

     @return
     - 0: Success.
     - < 0: Failure.
     */
	virtual int playEffect(int soundId, const char* filePath, int loopCount, double pitch, double pan, int gain, bool publish = false) = 0;
    /** Stops playing a specified audio effect.

     @param soundId ID of the audio effect to stop playing. Each audio effect has a unique ID.

     @return
     - 0: Success.
     - < 0: Failure.
     */
	virtual int stopEffect(int soundId) = 0;
    /** Stops playing all audio effects.

     @return
     - 0: Success.
     - < 0: Failure.
     */
	virtual int stopAllEffects() = 0;

    /** Preloads a specified audio effect file into the memory.

     @note This method does not support online audio effect files.

     To ensure smooth communication, limit the size of the audio effect file. We recommend using this method to preload the audio effect before calling the \ref IRtcEngine::joinChannel "joinChannel" method.

     Supported audio formats: mp3, aac, m4a, 3gp, and wav.

     @param soundId ID of the audio effect. Each audio effect has a unique ID.
     @param filePath Pointer to the absolute path of the audio effect file.

     @return
     - 0: Success.
     - < 0: Failure.
     */
	virtual int preloadEffect(int soundId, const char* filePath) = 0;
    /** Releases a specified preloaded audio effect from the memory.

     @param soundId ID of the audio effect. Each audio effect has a unique ID.
     @return
     - 0: Success.
     - < 0: Failure.
     */
	virtual int unloadEffect(int soundId) = 0;
    /** Pauses a specified audio effect.

     @param soundId ID of the audio effect. Each audio effect has a unique ID.
     @return
     - 0: Success.
     - < 0: Failure.
     */
	virtual int pauseEffect(int soundId) = 0;
    /** Pauses all audio effects.

     @return
     - 0: Success.
     - < 0: Failure.
     */
	virtual int pauseAllEffects() = 0;
    /** Resumes playing a specified audio effect.

     @param soundId ID of the audio effect. Each audio effect has a unique ID.
     @return
     - 0: Success.
     - < 0: Failure.
     */
	virtual int resumeEffect(int soundId) = 0;
    /** Resumes playing all audio effects.

     @return
     - 0: Success.
     - < 0: Failure.
     */
	virtual int resumeAllEffects() = 0;
    /** Enables/Disables stereo panning for remote users.

     Ensure that you call this method before joinChannel to enable stereo panning for remote users so that the local user can track the position of a remote user by calling \ref ar::rtc::IRtcEngine::setRemoteVoicePosition "setRemoteVoicePosition".

     @param enabled Sets whether or not to enable stereo panning for remote users:
     - true: enables stereo panning.
     - false: disables stereo panning.

     @return
     - 0: Success.
     - < 0: Failure.
     */
    virtual int enableSoundPositionIndication(bool enabled) = 0;
    /** Sets the sound position and gain of a remote user.

     When the local user calls this method to set the sound position of a remote user, the sound difference between the left and right channels allows the local user to track the real-time position of the remote user, creating a real sense of space. This method applies to massively multiplayer online games, such as Battle Royale games.

     @note
     - For this method to work, enable stereo panning for remote users by calling the \ref ar::rtc::IRtcEngine::enableSoundPositionIndication "enableSoundPositionIndication" method before joining a channel.
     - This method requires hardware support. For the best sound positioning, we recommend using a stereo speaker.

     @param uid The ID of the remote user.
     @param pan The sound position of the remote user. The value ranges from -1.0 to 1.0:
     - 0.0: the remote sound comes from the front.
     - -1.0: the remote sound comes from the left.
     - 1.0: the remote sound comes from the right.
     @param gain Gain of the remote user. The value ranges from 0.0 to 100.0. The default value is 100.0 (the original gain of the remote user). The smaller the value, the less the gain.

     @return
     - 0: Success.
     - < 0: Failure.
     */
    virtual int setRemoteVoicePosition(uid_t uid, double pan, double gain) = 0;

    /** Changes the voice pitch of the local speaker.

     @param pitch Sets the voice pitch. The value ranges between 0.5 and 2.0. The lower the value, the lower the voice pitch. The default value is 1.0 (no change to the local voice pitch).
     @return
     - 0: Success.
     - < 0: Failure.
     */
	virtual int setLocalVoicePitch(double pitch) = 0;
    /** Sets the local voice equalization effect.

     @param bandFrequency Sets the band frequency. The value ranges between 0 and 9, representing the respective 10-band center frequencies of the voice effects, including 31, 62, 125, 500, 1k, 2k, 4k, 8k, and 16k Hz. See #AUDIO_EQUALIZATION_BAND_FREQUENCY.
     @param bandGain  Sets the gain of each band in dB. The value ranges between -15 and 15.

     @return
     - 0: Success.
     - < 0: Failure.
     */
	virtual int setLocalVoiceEqualization(AUDIO_EQUALIZATION_BAND_FREQUENCY bandFrequency, int bandGain) = 0;
    /**  Sets the local voice reverberation.

     v2.4.0 adds the \ref ar::rtc::IRtcEngine::setLocalVoiceReverbPreset "setLocalVoiceReverbPreset" method, a more user-friendly method for setting the local voice reverberation. You can use this method to set the local reverberation effect, such as pop music, R&B, rock music, and hip-hop.

     @param reverbKey Sets the reverberation key. See #AUDIO_REVERB_TYPE.
     @param value Sets the value of the reverberation key.

     @return
     - 0: Success.
     - < 0: Failure.
     */
	virtual int setLocalVoiceReverb(AUDIO_REVERB_TYPE reverbKey, int value) = 0;
    /** Sets the local voice changer option.

     @note Do not use this method together with the \ref ar::rtc::IRtcEngine::setLocalVoiceReverbPreset "setLocalVoiceReverbPreset" method, because the method called later overrides the one called earlier.

     @param voiceChanger Sets the local voice changer option. See #VOICE_CHANGER_PRESET.

     @return
     - 0: Success.
     - < 0: Failure.
     */
    virtual int setLocalVoiceChanger(VOICE_CHANGER_PRESET voiceChanger) = 0;
    /** Sets the preset local voice reverberation effect.

     @note
     - Do not use this method together with \ref ar::rtc::IRtcEngine::setLocalVoiceReverb "setLocalVoiceReverb".
     - Do not use this method together with the \ref ar::rtc::IRtcEngine::setLocalVoiceChanger "setLocalVoiceChanger" method, because the method called later overrides the one called earlier.

     @param reverbPreset Sets the preset audio reverberation configuration. See #AUDIO_REVERB_PRESET.

     @return
     - 0: Success.
     - < 0: Failure.
     */
    virtual int setLocalVoiceReverbPreset(AUDIO_REVERB_PRESET reverbPreset) = 0;
    /** Sets an SDK preset voice beautifier effect.
     *
     * @since v3.2.0
     *
     * Call this method to set an SDK preset voice beautifier effect for the local user who sends an audio stream. After
     * setting a voice beautifier effect, all users in the channel can hear the effect.
     *
     * You can set different voice beautifier effects for different scenarios. See *Set the Voice Beautifier and Audio Effects*.
     *
     * To achieve better audio effect quality, AR recommends calling \ref IRtcEngine::setAudioProfile "setAudioProfile" and
     * setting the `scenario` parameter to `AUDIO_SCENARIO_GAME_STREAMING(3)` and the `profile` parameter to
     * `AUDIO_PROFILE_MUSIC_HIGH_QUALITY(4)` or `AUDIO_PROFILE_MUSIC_HIGH_QUALITY_STEREO(5)` before calling this method.
     *
     * @note
     * - You can call this method either before or after joining a channel.
     * - Do not set the `profile` parameter of \ref IRtcEngine::setAudioProfile "setAudioProfile" to `AUDIO_PROFILE_SPEECH_STANDARD(1)`
     * or `AUDIO_PROFILE_IOT(6)`; otherwise, this method call fails.
     * - This method works best with the human voice. AR does not recommend using this method for audio containing music.
     * - After calling this method, AR recommends not calling the following methods, because they can override \ref IRtcEngine::setAudioEffectParameters "setAudioEffectParameters":
     *  - \ref IRtcEngine::setAudioEffectPreset "setAudioEffectPreset"
     *  - \ref IRtcEngine::setVoiceBeautifierPreset "setVoiceBeautifierPreset"
     *  - \ref IRtcEngine::setLocalVoiceReverbPreset "setLocalVoiceReverbPreset"
     *  - \ref IRtcEngine::setLocalVoiceChanger "setLocalVoiceChanger"
     *  - \ref IRtcEngine::setLocalVoicePitch "setLocalVoicePitch"
     *  - \ref IRtcEngine::setLocalVoiceEqualization "setLocalVoiceEqualization"
     *  - \ref IRtcEngine::setLocalVoiceReverb "setLocalVoiceReverb"
     *
     * @param preset The options for SDK preset voice beautifier effects: #VOICE_BEAUTIFIER_PRESET.
     *
     * @return
     * - 0: Success.
     * - < 0: Failure.
     */
    virtual int setVoiceBeautifierPreset(VOICE_BEAUTIFIER_PRESET preset) = 0;
    /** Sets an SDK preset audio effect.
     *
     * @since v3.2.0
     *
     * Call this method to set an SDK preset audio effect for the local user who sends an audio stream. This audio effect
     * does not change the gender characteristics of the original voice. After setting an audio effect, all users in the
     * channel can hear the effect.
     *
     * You can set different audio effects for different scenarios. See *Set the Voice Beautifier and Audio Effects*.
     *
     * To achieve better audio effect quality, AR recommends calling \ref IRtcEngine::setAudioProfile "setAudioProfile"
     * and setting the `scenario` parameter to `AUDIO_SCENARIO_GAME_STREAMING(3)` before calling this method.
     *
     * @note
     * - You can call this method either before or after joining a channel.
     * - Do not set the profile `parameter` of `setAudioProfile` to `AUDIO_PROFILE_SPEECH_STANDARD(1)` or `AUDIO_PROFILE_IOT(6)`;
     * otherwise, this method call fails.
     * - This method works best with the human voice. AR does not recommend using this method for audio containing music.
     * - If you call this method and set the `preset` parameter to enumerators except `ROOM_ACOUSTICS_3D_VOICE` or `PITCH_CORRECTION`,
     * do not call \ref IRtcEngine::setAudioEffectParameters "setAudioEffectParameters"; otherwise, `setAudioEffectParameters`
     * overrides this method.
     * - After calling this method, AR recommends not calling the following methods, because they can override `setAudioEffectPreset`:
     *  - \ref IRtcEngine::setVoiceBeautifierPreset "setVoiceBeautifierPreset"
     *  - \ref IRtcEngine::setLocalVoiceReverbPreset "setLocalVoiceReverbPreset"
     *  - \ref IRtcEngine::setLocalVoiceChanger "setLocalVoiceChanger"
     *  - \ref IRtcEngine::setLocalVoicePitch "setLocalVoicePitch"
     *  - \ref IRtcEngine::setLocalVoiceEqualization "setLocalVoiceEqualization"
     *  - \ref IRtcEngine::setLocalVoiceReverb "setLocalVoiceReverb"
     *
     * @param preset The options for SDK preset audio effects. See #AUDIO_EFFECT_PRESET.
     *
     * @return
     * - 0: Success.
     * - < 0: Failure.
     */
    virtual int setAudioEffectPreset(AUDIO_EFFECT_PRESET preset) = 0;
    /** Sets parameters for SDK preset audio effects.
     *
     * @since v3.2.0
     *
     * Call this method to set the following parameters for the local user who send an audio stream:
     * - 3D voice effect: Sets the cycle period of the 3D voice effect.
     * - Pitch correction effect: Sets the basic mode and tonic pitch of the pitch correction effect. Different songs
     * have different modes and tonic pitches. AR recommends bounding this method with interface elements to enable
     * users to adjust the pitch correction interactively.
     *
     * After setting parameters, all users in the channel can hear the relevant effect.
     *
     * You can call this method directly or after \ref IRtcEngine::setAudioEffectPreset "setAudioEffectPreset". If you
     * call this method after \ref IRtcEngine::setAudioEffectPreset "setAudioEffectPreset", ensure that you set the preset
     * parameter of `setAudioEffectPreset` to `ROOM_ACOUSTICS_3D_VOICE` or `PITCH_CORRECTION` and then call this method
     * to set the same enumerator; otherwise, this method overrides `setAudioEffectPreset`.
     *
     * @note
     * - You can call this method either before or after joining a channel.
     * - To achieve better audio effect quality, AR recommends calling \ref IRtcEngine::setAudioProfile "setAudioProfile"
     * and setting the `scenario` parameter to `AUDIO_SCENARIO_GAME_STREAMING(3)` before calling this method.
     * - Do not set the `profile` parameter of \ref IRtcEngine::setAudioProfile "setAudioProfile" to `AUDIO_PROFILE_SPEECH_STANDARD(1)` or
     * `AUDIO_PROFILE_IOT(6)`; otherwise, this method call fails.
     * - This method works best with the human voice. AR does not recommend using this method for audio containing music.
     * - After calling this method, AR recommends not calling the following methods, because they can override `setAudioEffectParameters`:
     *  - \ref IRtcEngine::setAudioEffectPreset "setAudioEffectPreset"
     *  - \ref IRtcEngine::setVoiceBeautifierPreset "setVoiceBeautifierPreset"
     *  - \ref IRtcEngine::setLocalVoiceReverbPreset "setLocalVoiceReverbPreset"
     *  - \ref IRtcEngine::setLocalVoiceChanger "setLocalVoiceChanger"
     *  - \ref IRtcEngine::setLocalVoicePitch "setLocalVoicePitch"
     *  - \ref IRtcEngine::setLocalVoiceEqualization "setLocalVoiceEqualization"
     *  - \ref IRtcEngine::setLocalVoiceReverb "setLocalVoiceReverb"
     *
     * @param preset The options for SDK preset audio effects:
     * - 3D voice effect: `ROOM_ACOUSTICS_3D_VOICE`.
     *  - Call \ref IRtcEngine::setAudioProfile "setAudioProfile" and set the `profile` parameter to `AUDIO_PROFILE_MUSIC_STANDARD_STEREO(3)`
     * or `AUDIO_PROFILE_MUSIC_HIGH_QUALITY_STEREO(5)` before setting this enumerator; otherwise, the enumerator setting does not take effect.
     *  - If the 3D voice effect is enabled, users need to use stereo audio playback devices to hear the anticipated voice effect.
     * - Pitch correction effect: `PITCH_CORRECTION`. To achieve better audio effect quality, AR recommends calling
     * \ref IRtcEngine::setAudioProfile "setAudioProfile" and setting the `profile` parameter to `AUDIO_PROFILE_MUSIC_HIGH_QUALITY(4)` or
     * `AUDIO_PROFILE_MUSIC_HIGH_QUALITY_STEREO(5)` before setting this enumerator.
     * @param param1
     * - If you set `preset` to `ROOM_ACOUSTICS_3D_VOICE`, the `param1` sets the cycle period of the 3D voice effect.
     * The value range is [1,60] and the unit is a second. The default value is 10 seconds, indicating that the voice moves
     * around you every 10 seconds.
     * - If you set `preset` to `PITCH_CORRECTION`, `param1` sets the basic mode of the pitch correction effect:
     *  - `1`: (Default) Natural major scale.
     *  - `2`: Natural minor scale.
     *  - `3`: Japanese pentatonic scale.
     * @param param2
     * - If you set `preset` to `ROOM_ACOUSTICS_3D_VOICE`, you do not need to set `param2`.
     * - If you set `preset` to `PITCH_CORRECTION`, `param2` sets the tonic pitch of the pitch correction effect:
     *  - `1`: A
     *  - `2`: A#
     *  - `3`: B
     *  - `4`: (Default) C
     *  - `5`: C#
     *  - `6`: D
     *  - `7`: D#
     *  - `8`: E
     *  - `9`: F
     *  - `10`: F#
     *  - `11`: G
     *  - `12`: G#
     *
     * @return
     * - 0: Success.
     * - < 0: Failure.
     */
    virtual int setAudioEffectParameters(AUDIO_EFFECT_PRESET preset, int param1, int param2) = 0;
    /** Sets the log files that the SDK outputs.
     *
     * By default, the SDK outputs five log files, `arsdk.log`, `arsdk_1.log`, `arsdk_2.log`, `arsdk_3.log`, `arsdk_4.log`, each with a default size of 1024 KB.
     * These log files are encoded in UTF-8. The SDK writes the latest logs in `arsdk.log`. When `arsdk.log` is full, the SDK deletes the log file with the earliest
     * modification time among the other four, renames `arsdk.log` to the name of the deleted log file, and create a new `arsdk.log` to record latest logs.
     *
     * @note Ensure that you call this method immediately after calling \ref ar::rtc::IRtcEngine::initialize "initialize" , otherwise the output logs may not be complete.
     *
     * @see \ref IRtcEngine::setLogFileSize "setLogFileSize"
     * @see \ref IRtcEngine::setLogFilter "setLogFilter"
     *
     * @param filePath The absolute path of log files. The default file path is `C: \Users\<user_name>\AppData\Local\AR\<process_name>\arsdk.log`.
     * Ensure that the directory for the log files exists and is writable. You can use this parameter to rename the log files.
     *
     * @return
     * - 0: Success.
     * - < 0: Failure.
     */
	virtual int setLogFile(const char* filePath) = 0;
    /** Sets the output log level of the SDK.

     You can use one or a combination of the log filter levels. The log level follows the sequence of OFF, CRITICAL, ERROR, WARNING, INFO, and DEBUG. Choose a level to see the logs preceding that level.

     If you set the log level to WARNING, you see the logs within levels CRITICAL, ERROR, and WARNING.

     @param filter Sets the log filter level. See #LOG_FILTER_TYPE.

     @return
     - 0: Success.
     - < 0: Failure.
     */
	virtual int setLogFilter(unsigned int filter) = 0;
    /** Sets the log file size (KB).

     The SDK has two log files, each with a default size of 512 KB. If you set @p fileSizeInBytes as 1024 KB, the SDK outputs log files with a total maximum size of 2 MB. If the total size of the log files exceed the set value, the new output log files overwrite the old output log files.

     @param fileSizeInKBytes The SDK log file size (KB).
     @return
     - 0: Success.
     - <0: Failure.
     */
    virtual int setLogFileSize(unsigned int fileSizeInKBytes) = 0;
    /** 
     @deprecated This method is deprecated, use the \ref IRtcEngine::setLocalRenderMode(RENDER_MODE_TYPE renderMode, VIDEO_MIRROR_MODE_TYPE mirrorMode) "setLocalRenderMode"2 method instead.
     Sets the local video display mode.

     This method can be called multiple times during a call to change the display mode.

     @param renderMode  Sets the local video display mode. See #RENDER_MODE_TYPE.
     @return
     - 0: Success.
     - < 0: Failure.
     */
	virtual int setLocalRenderMode(RENDER_MODE_TYPE renderMode) = 0;
    /** Updates the display mode of the local video view.

     After initializing the local video view, you can call this method to update its rendering and mirror modes. It affects only the video view that the local user sees, not the published local video stream.
     
     @note
     - Ensure that you have called the \ref IRtcEngine::setupLocalVideo "setupLocalVideo" method to initialize the local video view before calling this method.
     @param renderMode The rendering mode of the local video view. See #RENDER_MODE_TYPE.
     @param mirrorMode 
     - The mirror mode of the local video view. See #VIDEO_MIRROR_MODE_TYPE. 
     - **Note**: If you use a front camera, the SDK enables the mirror mode by default; if you use a rear camera, the SDK disables the mirror mode by default.
     @return
     - 0: Success.
     - < 0: Failure.
     */
    virtual int setLocalRenderMode(RENDER_MODE_TYPE renderMode, VIDEO_MIRROR_MODE_TYPE mirrorMode) = 0;
    /** 
     @deprecated This method is deprecated, use the \ref IRtcEngine::setRemoteRenderMode(uid_t userId, RENDER_MODE_TYPE renderMode, VIDEO_MIRROR_MODE_TYPE mirrorMode) "setRemoteRenderMode"2 method instead.
     Sets the video display mode of a specified remote user.

     This method can be called multiple times during a call to change the display mode.

     @param userId ID of the remote user.
     @param renderMode  Sets the video display mode. See #RENDER_MODE_TYPE.
     @return
     - 0: Success.
     - < 0: Failure.
     */
	virtual int setRemoteRenderMode(uid_t userId, RENDER_MODE_TYPE renderMode) = 0;
    /** Updates the display mode of the video view of a remote user.

     After initializing the video view of a remote user, you can call this method to update its rendering and mirror modes. This method affects only the video view that the local user sees.

     @note
     - Ensure that you have called the \ref IRtcEngine::setupRemoteVideo "setupRemoteVideo" method to initialize the remote video view before calling this method.
     - During a call, you can call this method as many times as necessary to update the display mode of the video view of a remote user.

     @param userId The ID of the remote user.
     @param renderMode The rendering mode of the remote video view. See #RENDER_MODE_TYPE.
     @param mirrorMode 
     - The mirror mode of the remote video view. See #VIDEO_MIRROR_MODE_TYPE.
     - **Note**: The SDK disables the mirror mode by default.
     
     @return
     - 0: Success.
     - < 0: Failure.
     */
    virtual int setRemoteRenderMode(uid_t userId, RENDER_MODE_TYPE renderMode, VIDEO_MIRROR_MODE_TYPE mirrorMode) = 0;
    /** 
     @deprecated This method is deprecated, use the \ref IRtcEngine::setupLocalVideo "setupLocalVideo" 
     or \ref IRtcEngine::setLocalRenderMode(RENDER_MODE_TYPE renderMode, VIDEO_MIRROR_MODE_TYPE mirrorMode) "setLocalRenderMode" method instead.
     Sets the local video mirror mode.

     You must call this method before calling the \ref ar::rtc::IRtcEngine::startPreview "startPreview" method, otherwise the mirror mode will not work.

     @warning
     - Call this method after calling the \ref ar::rtc::IRtcEngine::setupLocalVideo "setupLocalVideo" method to initialize the local video view.
     - During a call, you can call this method as many times as necessary to update the mirror mode of the local video view.

     @param mirrorMode Sets the local video mirror mode. See #VIDEO_MIRROR_MODE_TYPE.
     @return
     - 0: Success.
     - < 0: Failure.
     */
	virtual int setLocalVideoMirrorMode(VIDEO_MIRROR_MODE_TYPE mirrorMode) = 0;
    /** Sets the stream mode to the single-stream (default) or dual-stream mode. (Live broadcast only.)

     If the dual-stream mode is enabled, the receiver can choose to receive the high stream (high-resolution and high-bitrate video stream), or the low stream (low-resolution and low-bitrate video stream).

     @param enabled Sets the stream mode:
     - true: Dual-stream mode.
     - false: (Default) Single-stream mode.
     */
	virtual int enableDualStreamMode(bool enabled) = 0;
    /** Sets the external audio source. Please call this method before \ref ar::rtc::IRtcEngine::joinChannel "joinChannel".

     @param enabled Sets whether to enable/disable the external audio source:
     - true: Enables the external audio source.
     - false: (Default) Disables the external audio source.
     @param sampleRate Sets the sample rate (Hz) of the external audio source, which can be set as 8000, 16000, 32000, 44100, or 48000 Hz.
     @param channels Sets the number of audio channels of the external audio source:
     - 1: Mono.
     - 2: Stereo.
     
     @return
     - 0: Success.
     - < 0: Failure.
     */
	virtual int setExternalAudioSource(bool enabled, int sampleRate, int channels) = 0;
    /** Sets the external audio sink.
     * This method applies to scenarios where you want to use external audio
     * data for playback. After enabling the external audio sink, you can call
     * the \ref AM::IMediaEngine::pullAudioFrame "pullAudioFrame" method to pull the remote audio data, process
     * it, and play it with the audio effects that you want.
     *
     * @note
     * Once you enable the external audio sink, the app will not retrieve any
     * audio data from the
     * \ref AM::IAudioFrameObserver::onPlaybackAudioFrame "onPlaybackAudioFrame" callback.
     *
     * @param enabled
     * - true: Enables the external audio sink.
     * - false: (Default) Disables the external audio sink.
     * @param sampleRate Sets the sample rate (Hz) of the external audio sink, which can be set as 16000, 32000, 44100 or 48000.
     * @param channels Sets the number of audio channels of the external
     * audio sink:
     * - 1: Mono.
     * - 2: Stereo.
     *
     * @return
     * - 0: Success.
     * - < 0: Failure.
     */
    virtual int setExternalAudioSink(bool enabled, int sampleRate, int channels) = 0;
    /** Sets the audio recording format for the \ref AM::IAudioFrameObserver::onRecordAudioFrame "onRecordAudioFrame" callback. 
    

     @param sampleRate Sets the sample rate (@p samplesPerSec) returned in the *onRecordAudioFrame* callback, which can be set as 8000, 16000, 32000, 44100, or 48000 Hz.
     @param channel Sets the number of audio channels (@p channels) returned in the *onRecordAudioFrame* callback:
     - 1: Mono
     - 2: Stereo
     @param mode Sets the use mode (see #RAW_AUDIO_FRAME_OP_MODE_TYPE) of the *onRecordAudioFrame* callback.
     @param samplesPerCall Sets the number of samples returned in the *onRecordAudioFrame* callback. `samplesPerCall` is usually set as 1024 for RTMP streaming.


     @note The SDK triggers the `onRecordAudioFrame` callback according to the sample interval. Ensure that the sample interval >= 0.01 (s). And, Sample interval (sec) = `samplePerCall`/(`sampleRate` x `channel`). 

     @return
     - 0: Success.
     - < 0: Failure.
     */
	virtual int setRecordingAudioFrameParameters(int sampleRate, int channel, RAW_AUDIO_FRAME_OP_MODE_TYPE mode, int samplesPerCall) = 0;
    /** Sets the audio playback format for the \ref AM::IAudioFrameObserver::onPlaybackAudioFrame "onPlaybackAudioFrame" callback.
     
     
     @param sampleRate Sets the sample rate (@p samplesPerSec) returned in the *onPlaybackAudioFrame* callback, which can be set as 8000, 16000, 32000, 44100, or 48000 Hz.
     @param channel Sets the number of channels (@p channels) returned in the *onPlaybackAudioFrame* callback:
     - 1: Mono
     - 2: Stereo
     @param mode Sets the use mode (see #RAW_AUDIO_FRAME_OP_MODE_TYPE) of the *onPlaybackAudioFrame* callback.
     @param samplesPerCall Sets the number of samples returned in the *onPlaybackAudioFrame* callback. `samplesPerCall` is usually set as 1024 for RTMP streaming.
     
     @note The SDK triggers the `onPlaybackAudioFrame` callback according to the sample interval. Ensure that the sample interval >= 0.01 (s). And, Sample interval (sec) = `samplePerCall`/(`sampleRate` x `channel`).
     
     @return
     - 0: Success.
     - < 0: Failure.
     */
	virtual int setPlaybackAudioFrameParameters(int sampleRate, int channel, RAW_AUDIO_FRAME_OP_MODE_TYPE mode, int samplesPerCall) = 0;
    /** Sets the mixed audio format for the \ref AM::IAudioFrameObserver::onMixedAudioFrame "onMixedAudioFrame" callback.
     
    
     @param sampleRate Sets the sample rate (@p samplesPerSec) returned in the *onMixedAudioFrame* callback, which can be set as 8000, 16000, 32000, 44100, or 48000 Hz.
     @param samplesPerCall Sets the number of samples (`samples`) returned in the *onMixedAudioFrame* callback. `samplesPerCall` is usually set as 1024 for RTMP streaming.
     
     @note The SDK triggers the `onMixedAudioFrame` callback according to the sample interval. Ensure that the sample interval >= 0.01 (s). And, Sample interval (sec) = `samplePerCall`/(`sampleRate` x `channels`).
    
     @return
     - 0: Success.
     - < 0: Failure.
     */
	virtual int setMixedAudioFrameParameters(int sampleRate, int samplesPerCall) = 0;
    /** Adjusts the recording volume.

     @param volume Recording volume. The value ranges between 0 and 400:
     - 0: Mute.
     - 100: Original volume.
     - 400: (Maximum) Four times the original volume with signal clipping protection.

     @return
     - 0: Success.
     - < 0: Failure.
     */
	virtual int adjustRecordingSignalVolume(int volume) = 0;
    /** Adjusts the playback volume of all remote users.
     
     @note 
     - This method adjusts the playback volume that is the mixed volume of all remote users.
     - (Since v2.3.2) To mute the local audio playback, call both the `adjustPlaybackSignalVolume` and \ref IRtcEngine::adjustAudioMixingVolume "adjustAudioMixingVolume" methods and set the volume as `0`.

     @param volume The playback volume of all remote users. The value ranges from 0 to 400:
     - 0: Mute.
     - 100: Original volume.
     - 400: (Maximum) Four times the original volume with signal clipping protection.

     @return
     - 0: Success.
     - < 0: Failure.
     */
	virtual int adjustPlaybackSignalVolume(int volume) = 0;

    /** 
     @deprecated This method is deprecated. As of v3.0.0, the Native SDK automatically enables interoperability with the Web SDK, so you no longer need to call this method.
     Enables interoperability with the AR Web SDK.

     @note 
     - This method applies only to the Live-broadcast profile. In the Communication profile, interoperability with the AR Web SDK is enabled by default.
     - If the channel has Web SDK users, ensure that you call this method, or the video of the Native user will be a black screen for the Web user.

     @param enabled Sets whether to enable/disable interoperability with the AR Web SDK:
     - true: Enable.
     - false: (Default) Disable.

     @return
     - 0: Success.
     - < 0: Failure.
     */
	virtual int enableWebSdkInteroperability(bool enabled) = 0;
    //only for live broadcast
    /** **DEPRECATED** Sets the preferences for the high-quality video. (Live broadcast only).

     This method is deprecated as of v2.4.0.

     @param preferFrameRateOverImageQuality Sets the video quality preference:
     - true: Frame rate over image quality.
     - false: (Default) Image quality over frame rate.

     @return
     - 0: Success.
     - < 0: Failure.
     */
    virtual int setVideoQualityParameters(bool preferFrameRateOverImageQuality) = 0;
    /** Sets the fallback option for the locally published video stream based on the network conditions.

     If `option` is set as #STREAM_FALLBACK_OPTION_AUDIO_ONLY (2), the SDK will:

     - Disable the upstream video but enable audio only when the network conditions deteriorate and cannot support both video and audio.
     - Re-enable the video when the network conditions improve.
     
     When the locally published video stream falls back to audio only or when the audio-only stream switches back to the video, the SDK triggers the \ref ar::rtc::IRtcEngineEventHandler::onLocalPublishFallbackToAudioOnly "onLocalPublishFallbackToAudioOnly" callback.

     @note AR does not recommend using this method for CDN live streaming, because the remote CDN live user will have a noticeable lag when the locally published video stream falls back to audio only.

     @param option Sets the fallback option for the locally published video stream:
     - #STREAM_FALLBACK_OPTION_DISABLED (0): (Default) No fallback behavior for the locally published video stream when the uplink network condition is poor. The stream quality is not guaranteed.
     - #STREAM_FALLBACK_OPTION_AUDIO_ONLY (2): The locally published video stream falls back to audio only when the uplink network condition is poor.

     @return
     - 0: Success.
     - < 0: Failure.
     */
    virtual int setLocalPublishFallbackOption(STREAM_FALLBACK_OPTIONS option) = 0;
    /** Sets the fallback option for the remotely subscribed video stream based on the network conditions.

     The default setting for `option` is #STREAM_FALLBACK_OPTION_VIDEO_STREAM_LOW (1), where the remotely subscribed video stream falls back to the low-stream video (low resolution and low bitrate) under poor downlink network conditions.

     If `option` is set as #STREAM_FALLBACK_OPTION_AUDIO_ONLY (2), the SDK automatically switches the video from a high-stream to a low-stream, or disables the video when the downlink network conditions cannot support both audio and video to guarantee the quality of the audio. The SDK monitors the network quality and restores the video stream when the network conditions improve.

     When the remotely subscribed video stream falls back to audio only or when the audio-only stream switches back to the video stream, the SDK triggers the \ref ar::rtc::IRtcEngineEventHandler::onRemoteSubscribeFallbackToAudioOnly "onRemoteSubscribeFallbackToAudioOnly" callback.

     @param  option  Sets the fallback option for the remotely subscribed video stream. See #STREAM_FALLBACK_OPTIONS.
     @return
     - 0: Success.
     - < 0: Failure.
     */
    virtual int setRemoteSubscribeFallbackOption(STREAM_FALLBACK_OPTIONS option) = 0;

#if defined(__ANDROID__) || (defined(__APPLE__) && TARGET_OS_IOS)
	/** Switches between front and rear cameras.

     @note This method is for Android and iOS only.

     @return
     - 0: Success.
     - < 0: Failure.
     */
    virtual int switchCamera() = 0;
    /** Switches between front and rear cameras.

     @note This method is for Android and iOS only, and it is private.
     
     @param direction Sets the camera to be used:
     - CAMERA_DIRECTION.CAMERA_REAR: Use the rear camera.
     - CAMERA_DIRECTION.CAMERA_FRONT: Use the front camera.
     
     @return
     - 0: Success.
     - < 0: Failure.
     */
    virtual int switchCamera(CAMERA_DIRECTION direction) = 0;
    /** Sets the default audio playback route.

     This method sets whether the received audio is routed to the earpiece or speakerphone by default before joining a channel.
     If a user does not call this method, the audio is routed to the earpiece by default. If you need to change the default audio route after joining a channel, call the \ref IRtcEngine::setEnableSpeakerphone "setEnableSpeakerphone" method.

     The default setting for each profile:
     - `COMMUNICATION`: In a voice call, the default audio route is the earpiece. In a video call, the default audio route is the speakerphone. If a user who is in the `COMMUNICATION` profile calls
     the \ref IRtcEngine.disableVideo "disableVideo" method or if the user calls
     the \ref IRtcEngine.muteLocalVideoStream "muteLocalVideoStream" and
     \ref IRtcEngine.muteAllRemoteVideoStreams "muteAllRemoteVideoStreams" methods, the
     default audio route switches back to the earpiece automatically.
     - `LIVE_BROADCASTING`: Speakerphone.

     @note
     - This method is for Android and iOS only.
     - This method only works in audio mode.
     - Call this method before calling the \ref IRtcEngine::joinChannel "joinChannel" method.
     - Regardless of whether the audio is routed to the speakerphone or earpiece by default, once a headset is plugged in or Bluetooth device is connected, the default audio route changes. The default audio route switches to the earpiece once removing the headset or disconnecting the Bluetooth device.

     @param defaultToSpeaker Sets the default audio route:
     - true: Route the audio to the speakerphone. If the playback device connects to the earpiece or Bluetooth, the audio cannot be routed to the speakerphone.
     - false: (Default) Route the audio to the earpiece. If a headset is plugged in, the audio is routed to the headset.

     @return
     - 0: Success.
     - < 0: Failure.
     */
	virtual int setDefaultAudioRouteToSpeakerphone(bool defaultToSpeaker) = 0;
    /** Enables/Disables the audio playback route to the speakerphone.

     This method sets whether the audio is routed to the speakerphone or earpiece.

     See the default audio route explanation in the \ref IRtcEngine::setDefaultAudioRouteToSpeakerphone "setDefaultAudioRouteToSpeakerphone" method and check whether it is necessary to call this method.

     @note
     - This method is for Android and iOS only.
     - Ensure that you have successfully called the \ref IRtcEngine::joinChannel "joinChannel" method before calling this method.
     - After calling this method, the SDK returns the \ref IRtcEngineEventHandler::onAudioRouteChanged "onAudioRouteChanged" callback to indicate the changes.
     - This method does not take effect if a headset is used.

     @param speakerOn Sets whether to route the audio to the speakerphone or earpiece:
     - true: Route the audio to the speakerphone.
     - false: Route the audio to the earpiece.

     @return
     - 0: Success.
     - < 0: Failure.
     */
	virtual int setEnableSpeakerphone(bool speakerOn) = 0;
    /** Enables in-ear monitoring (for Android and iOS only).
     @param enabled Sets whether to enable/disable in-ear monitoring:
     - true: Enable.
     - false: (Default) Disable.
     
     * @return
     - 0: Success.
     - < 0: Failure.
     */
    virtual int enableInEarMonitoring(bool enabled) = 0;
    /** Sets the volume of the in-ear monitor.

     @param volume Sets the volume of the in-ear monitor. The value ranges between 0 and 100 (default).

     @note This method is for Android and iOS only.

     @return
     - 0: Success.
     - < 0: Failure.
     */
	virtual int setInEarMonitoringVolume(int volume) = 0;
    /** Checks whether the speakerphone is enabled.

     @note This method is for Android and iOS only.

     @return
     - 0: Success.
     - < 0: Failure.
     */
    virtual bool isSpeakerphoneEnabled() = 0;
    
    virtual bool isCameraZoomSupported() = 0;
    virtual bool isCameraTorchSupported() = 0;
    virtual bool isCameraFocusPositionInPreviewSupported() = 0;
    virtual bool isCameraExposurePositionSupported() = 0;
    virtual bool isCameraAutoFocusFaceModeSupported() = 0;
    virtual float setCameraZoomFactor(float zoomFactor) = 0;
    virtual bool setCameraFocusPositionInPreview(float x, float y) = 0;
    virtual bool setCameraExposurePosition(float x, float y) = 0;
    virtual bool setCameraTorchOn(bool enabled) = 0;
    virtual bool setCameraAutoFocusFaceModeEnabled(bool enabled) = 0;
    
#endif

#if (defined(__APPLE__) && TARGET_OS_IOS)
    /** Sets the audio session’s operational restriction.

     The SDK and the app can both configure the audio session by default. The app may occasionally use other apps or third-party components to manipulate the audio session and restrict the SDK from doing so. This method allows the app to restrict the SDK’s manipulation of the audio session.

     You can call this method at any time to return the control of the audio sessions to the SDK.

     @note
     - This method is for iOS only.
     - This method restricts the SDK’s manipulation of the audio session. Any operation to the audio session relies solely on the app, other apps, or third-party components.

     @param restriction The operational restriction (bit mask) of the SDK on the audio session. See #AUDIO_SESSION_OPERATION_RESTRICTION.

     @return
     - 0: Success.
     - < 0: Failure.
     */
    virtual int setAudioSessionOperationRestriction(AUDIO_SESSION_OPERATION_RESTRICTION restriction) = 0;
#endif

#if (defined(__APPLE__) && TARGET_OS_MAC && !TARGET_OS_IPHONE) || defined(_WIN32)
    /** Enables loopback recording.

     If you enable loopback recording, the output of the sound card is mixed into the audio stream sent to the other end.

     @param enabled Sets whether to enable/disable loopback recording.
     - true: Enable loopback recording.
     - false: (Default) Disable loopback recording.
     @param deviceName Pointer to the device name of the sound card. The default value is NULL (the default sound card).

     @note
     - This method is for macOS and Windows only.
     - macOS does not support loopback recording of the default sound card. If you need to use this method, please use a virtual sound card and pass its name to the deviceName parameter. AR has tested and recommends using soundflower.

     */
    virtual int enableLoopbackRecording(bool enabled, const char* deviceName = NULL) = 0;

#if (defined(__APPLE__) && TARGET_OS_MAC && !TARGET_OS_IPHONE)
    /** Shares the whole or part of a screen by specifying the display ID.

     @note This method is for macOS only.

     @param  displayId The display ID of the screen to be shared. This parameter specifies which screen you want to share.
     @param  regionRect (Optional) Sets the relative location of the region to the screen. NIL means sharing the whole screen. See Rectangle. If the specified region overruns the screen, the SDK shares only the region within it; if you set width or height as 0, the SDK shares the whole screen.
     @param  captureParams Sets the screen sharing encoding parameters. See ScreenCaptureParameters.


     @return
     - 0: Success.
     - < 0: Failure:
        - #ERR_INVALID_ARGUMENT: the argument is invalid.
     */
    virtual int startScreenCaptureByDisplayId(unsigned int displayId, const Rectangle& regionRect, const ScreenCaptureParameters& captureParams) = 0;
#endif

#if defined(_WIN32)
    /** Shares the whole or part of a screen by specifying the screen rect.

     @param  screenRect Sets the relative location of the screen to the virtual screen. For information on how to get screenRect, see [Share the Screen](https://docs.ar.io/en/Video/screensharing_windows?platform=Windows).
     @param  regionRect (Optional) Sets the relative location of the region to the screen. NULL means sharing the whole screen. See Rectangle. If the specified region overruns the screen, the SDK shares only the region within it; if you set width or height as 0, the SDK shares the whole screen.
     @param  captureParams Sets the screen sharing encoding parameters. See ScreenCaptureParameters.

     @return
     - 0: Success.
     - < 0: Failure:
        - ERR_INVALID_STATE: the screen sharing state is invalid, probably because another screen or window is being shared. Call \ref ar::rtc::IRtcEngine::stopScreenCapture "stopScreenCapture" to stop the current screen sharing.
        - ERR_INVALID_ARGUMENT: the argument is invalid.
     */
    virtual int startScreenCaptureByScreenRect(const Rectangle& screenRect, const Rectangle& regionRect, const ScreenCaptureParameters& captureParams) = 0;
#endif

    /** Shares the whole or part of a window by specifying the window ID.

     Since v3.0.0, this method supports sharing with common Windows platforms. AR tests the mainstream Windows applications, see details as follows:

     <table>
         <tr>
             <td><b>OS version</b></td>
             <td><b>Software</b></td>
             <td><b>Software name</b></td>
             <td><b>Whether support</b></td>
         </tr>
         <tr>
             <td rowspan="8">win10</td>
             <td >Chrome</td>
             <td>76.0.3809.100</td>
             <td>No</td>
         </tr>
         <tr>
             <td>Office Word</td>
             <td rowspan="3">18.1903.1152.0</td>
             <td>Yes</td>
         </tr>
             <tr>
             <td>Office Excel</td>
             <td>No</td>
         </tr>
         <tr>
             <td>Office PPT</td>
             <td>No</td>
         </tr>
      <tr>
             <td>WPS Word</td>
             <td rowspan="3">11.1.0.9145</td>
             <td rowspan="3">Yes</td>
         </tr>
             <tr>
             <td>WPS Excel</td>
         </tr>
         <tr>
             <td>WPS PPT</td>
         </tr>
             <tr>
             <td>Media Player (come with the system)</td>
             <td>All</td>
             <td>Yes</td>
         </tr>
          <tr>
             <td rowspan="8">win8</td>
             <td >Chrome</td>
             <td>All</td>
             <td>Yes</td>
         </tr>
         <tr>
             <td>Office Word</td>
             <td rowspan="3">All</td>
             <td rowspan="3">Yes</td>
         </tr>
             <tr>
             <td>Office Excel</td>
         </tr>
         <tr>
             <td>Office PPT</td>
         </tr>
      <tr>
             <td>WPS Word</td>
             <td rowspan="3">11.1.0.9098</td>
             <td rowspan="3">Yes</td>
         </tr>
             <tr>
             <td>WPS Excel</td>
         </tr>
         <tr>
             <td>WPS PPT</td>
         </tr>
             <tr>
             <td>Media Player(come with the system)</td>
             <td>All</td>
             <td>Yes</td>
         </tr>
       <tr>
             <td rowspan="8">win7</td>
             <td >Chrome</td>
             <td>73.0.3683.103</td>
             <td>No</td>
         </tr>
         <tr>
             <td>Office Word</td>
             <td rowspan="3">All</td>
             <td rowspan="3">Yes</td>
         </tr>
             <tr>
             <td>Office Excel</td>
         </tr>
         <tr>
             <td>Office PPT</td>
         </tr>
      <tr>
             <td>WPS Word</td>
             <td rowspan="3">11.1.0.9098</td>
             <td rowspan="3">No</td>
         </tr>
             <tr>
             <td>WPS Excel</td>
         </tr>
         <tr>
             <td>WPS PPT</td>
         </tr>
             <tr>
             <td>Media Player(come with the system)</td>
             <td>All</td>
             <td>No</td>
         </tr>
     </table>

     @param  windowId The ID of the window to be shared. For information on how to get the windowId, see the advanced guide *Share Screen*.
     @param  regionRect (Optional) The relative location of the region to the window. NULL/NIL means sharing the whole window. See Rectangle. If the specified region overruns the window, the SDK shares only the region within it; if you set width or height as 0, the SDK shares the whole window.
     @param  captureParams Window sharing encoding parameters. See ScreenCaptureParameters.

     @return
     - 0: Success.
     - < 0: Failure:
        - #ERR_INVALID_ARGUMENT: the argument is invalid.
     */
    virtual int startScreenCaptureByWindowId(view_t windowId, const Rectangle& regionRect, const ScreenCaptureParameters& captureParams) = 0;

    /** Sets the content hint for screen sharing.

    A content hint suggests the type of the content being shared, so that the SDK applies different optimization algorithm to different types of content.

     @param  contentHint Sets the content hint for screen sharing. See VideoContentHint.

     @return
     - 0: Success.
     - < 0: Failure.
     */
    virtual int setScreenCaptureContentHint(VideoContentHint contentHint) = 0;

    /** Updates the screen sharing parameters.

     @param  captureParams Sets the screen sharing encoding parameters. See ScreenCaptureParameters.

     @return
     - 0: Success.
     - < 0: Failure:
        - #ERR_NOT_READY: no screen or windows is being shared.
     */
    virtual int updateScreenCaptureParameters(const ScreenCaptureParameters& captureParams) = 0;

    /** Updates the screen sharing region.

     @param  regionRect Sets the relative location of the region to the screen or window. NULL means sharing the whole screen or window. See Rectangle. If the specified region overruns the screen or window, the SDK shares only the region within it; if you set width or height as 0, the SDK shares the whole screen or window.

     @return
     - 0: Success.
     - < 0: Failure:
        - #ERR_NOT_READY: no screen or window is being shared.
     */
    virtual int updateScreenCaptureRegion(const Rectangle& regionRect) = 0;

    /** Stop screen sharing.

     @return
     - 0: Success.
     - < 0: Failure.
    */
    virtual int stopScreenCapture() = 0;

#if defined(__APPLE__)
    typedef unsigned int WindowIDType;
#elif defined(_WIN32)
    typedef HWND WindowIDType;
#endif

    /** **DEPRECATED** Starts screen sharing.

     This method is deprecated as of v2.4.0. See the following methods instead:

     - \ref ar::rtc::IRtcEngine::startScreenCaptureByDisplayId "startScreenCaptureByDisplayId"
     - \ref ar::rtc::IRtcEngine::startScreenCaptureByScreenRect "startScreenCaptureByScreenRect"
     - \ref ar::rtc::IRtcEngine::startScreenCaptureByWindowId "startScreenCaptureByWindowId"

     This method shares the whole screen, specified window, or specified region:

     - Whole screen: Set @p windowId as 0 and @p rect as NULL.
     - Specified window: Set @p windowId as a value other than 0. Each window has a @p windowId that is not 0.
     - Specified region: Set @p windowId as 0 and @p rect not as NULL. In this case, you can share the specified region, for example by dragging the mouse or implementing your own logic.

     @note The specified region is a region on the whole screen. Currently, sharing a specified region in a specific window is not supported.
     *captureFreq* is the captured frame rate once the screen-sharing function is enabled. The mandatory value ranges between 1 fps and 15 fps.

     @param windowId Sets the screen sharing area. See WindowIDType.
     @param captureFreq (Mandatory) The captured frame rate. The value ranges between 1 fps and 15 fps.
     @param rect Specifies the screen-sharing region. @p rect is valid when @p windowsId is set as 0. When @p rect is set as NULL, the whole screen is shared.
     @param bitrate The captured bitrate.

     @return
     - 0: Success.
     - < 0: Failure.
     */
    virtual int startScreenCapture(WindowIDType windowId, int captureFreq, const Rect *rect, int bitrate) = 0;

    /** **DEPRECATED** Updates the screen capture region.

     @param rect Specifies the required region inside the screen or window.
     @return
     - 0: Success.
     - < 0: Failure.
     */
    virtual int updateScreenCaptureRegion(const Rect *rect) = 0;
#endif

#if defined(_WIN32)
    /** Sets a custom video source.
     *
     * During real-time communication, the AR SDK enables the default video input device, that is, the built-in camera to
     * capture video. If you need a custom video source, implement the IVideoSource class first, and call this method to add
     * the custom video source to the SDK.
     *
     * @param source The custom video source. See IVideoSource.
     *
     * @return
     * - true: The custom video source is added to the SDK.
     * - false: The custom video source is not added to the SDK.
     */
    virtual bool setVideoSource(IVideoSource *source) = 0;
#endif

    /** Retrieves the current call ID.

     When a user joins a channel on a client, a @p callId is generated to identify the call from the client. Feedback methods, such as \ref IRtcEngine::rate "rate" and \ref IRtcEngine::complain "complain", must be called after the call ends to submit feedback to the SDK.

     The \ref IRtcEngine::rate "rate" and \ref IRtcEngine::complain "complain" methods require the @p callId parameter retrieved from the *getCallId* method during a call. @p callId is passed as an argument into the \ref IRtcEngine::rate "rate" and \ref IRtcEngine::complain "complain" methods after the call ends.

     @param callId Pointer to the current call ID.

     @return
     - 0: Success.
     - < 0: Failure.
     */
    virtual int getCallId(ar::util::AString& callId) = 0;

    /** Allows a user to rate a call after the call ends.

     @param callId Pointer to the ID of the call, retrieved from the \ref IRtcEngine::getCallId "getCallId" method.
     @param rating  Rating of the call. The value is between 1 (lowest score) and 5 (highest score). If you set a value out of this range, the #ERR_INVALID_ARGUMENT (2) error returns.
     @param description (Optional) Pointer to the description of the rating, with a string length of less than 800 bytes.

     @return
     - 0: Success.
     - < 0: Failure.
     */
    virtual int rate(const char* callId, int rating, const char* description) = 0;

    /** Allows a user to complain about the call quality after a call ends.

    @param callId Pointer to the ID of the call, retrieved from the \ref IRtcEngine::getCallId "getCallId" method.
    @param description (Optional) Pointer to the description of the complaint, with a string length of less than 800 bytes.

    @return
    - 0: Success.
    - < 0: Failure.

    */
    virtual int complain(const char* callId, const char* description) = 0;

    /** Retrieves the SDK version number.

     @param build Pointer to the build number.
     @return The version of the current SDK in the string format. For example, 2.3.1.
     */
    virtual const char* getVersion(int* build) = 0;

    /**  Enables the network connection quality test.

     This method tests the quality of the users' network connections and is disabled by default.

     Before a user joins a channel or before an audience switches to a host, call this method to check the uplink network quality.

     This method consumes additional network traffic, and hence may affect communication quality.

     Call the \ref IRtcEngine::disableLastmileTest "disableLastmileTest" method to disable this test after receiving the \ref IRtcEngineEventHandler::onLastmileQuality "onLastmileQuality" callback, and before joining a channel.

     @note
     - Do not call any other methods before receiving the \ref IRtcEngineEventHandler::onLastmileQuality "onLastmileQuality" callback. Otherwise, the callback may be interrupted by other methods, and hence may not be triggered.
     - A host should not call this method after joining a channel (when in a call).
     - If you call this method to test the last-mile quality, the SDK consumes the bandwidth of a video stream, whose bitrate corresponds to the bitrate you set in the \ref ar::rtc::IRtcEngine::setVideoEncoderConfiguration "setVideoEncoderConfiguration" method. After you join the channel, whether you have called the `disableLastmileTest` method or not, the SDK automatically stops consuming the bandwidth.

     @return
     - 0: Success.
     - < 0: Failure.
     */
    virtual int enableLastmileTest() = 0;

    /** Disables the network connection quality test.

     @return
     - 0: Success.
     - < 0: Failure.
     */
    virtual int disableLastmileTest() = 0;

    /** Starts the last-mile network probe test.

    This method starts the last-mile network probe test before joining a channel to get the uplink and downlink last-mile network statistics, including the bandwidth, packet loss, jitter, and round-trip time (RTT).

    Call this method to check the uplink network quality before users join a channel or before an audience switches to a host.
    Once this method is enabled, the SDK returns the following callbacks:
    - \ref IRtcEngineEventHandler::onLastmileQuality "onLastmileQuality": the SDK triggers this callback within two seconds depending on the network conditions. This callback rates the network conditions and is more closely linked to the user experience.
    - \ref IRtcEngineEventHandler::onLastmileProbeResult "onLastmileProbeResult": the SDK triggers this callback within 30 seconds depending on the network conditions. This callback returns the real-time statistics of the network conditions and is more objective.

    @note
    - This method consumes extra network traffic and may affect communication quality. We do not recommend calling this method together with enableLastmileTest.
    - Do not call other methods before receiving the \ref IRtcEngineEventHandler::onLastmileQuality "onLastmileQuality" and \ref IRtcEngineEventHandler::onLastmileProbeResult "onLastmileProbeResult" callbacks. Otherwise, the callbacks may be interrupted.
    - In the Live-broadcast profile, a host should not call this method after joining a channel.

    @param config Sets the configurations of the last-mile network probe test. See LastmileProbeConfig.

     @return
     - 0: Success.
     - < 0: Failure.
     */
    virtual int startLastmileProbeTest(const LastmileProbeConfig& config) = 0;

    /** Stops the last-mile network probe test. */
    virtual int stopLastmileProbeTest() = 0;

    /** Retrieves the warning or error description.

     @param code Warning code or error code returned in the \ref ar::rtc::IRtcEngineEventHandler::onWarning "onWarning" or \ref ar::rtc::IRtcEngineEventHandler::onError "onError" callback.
     
     @return #WARN_CODE_TYPE or #ERROR_CODE_TYPE.
     */
    virtual const char* getErrorDescription(int code) = 0;

    /** Enables built-in encryption with an encryption password before users join a channel.

     All users in a channel must use the same encryption password. The encryption password is automatically cleared once a user leaves the channel.

     If an encryption password is not specified, the encryption functionality will be disabled.

     @note
     - Do not use this method for CDN live streaming.
     - For optimal transmission, ensure that the encrypted data size does not exceed the original data size + 16 bytes. 16 bytes is the maximum padding size for AES encryption.

     @param secret Pointer to the encryption password.

     @return
     - 0: Success.
     - < 0: Failure.
     */
    virtual int setEncryptionSecret(const char* secret) = 0;

    /** Sets the built-in encryption mode.

     The AR SDK supports built-in encryption, which is set to the @p aes-128-xts mode by default. Call this method to use other encryption modes.

     All users in the same channel must use the same encryption mode and password.

     Refer to the information related to the AES encryption algorithm on the differences between the encryption modes.

     @note Call the \ref IRtcEngine::setEncryptionSecret "setEncryptionSecret" method to enable the built-in encryption function before calling this method.

     @param encryptionMode Pointer to the set encryption mode:
     - "aes-128-xts": (Default) 128-bit AES encryption, XTS mode.
     - "aes-128-ecb": 128-bit AES encryption, ECB mode.
     - "aes-256-xts": 256-bit AES encryption, XTS mode.
     - "": When encryptionMode is set as NULL, the encryption mode is set as "aes-128-xts" by default.

     @return
     - 0: Success.
     - < 0: Failure.
     */
    virtual int setEncryptionMode(const char* encryptionMode) = 0;

    /** Enables/Disables the built-in encryption.
     *
     * @since v3.1.0
     *
     * In scenarios requiring high security, AR recommends calling this method to enable the built-in encryption before joining a channel.
     *
     * All users in the same channel must use the same encryption mode and encryption key. Once all users leave the channel, the encryption key of this channel is automatically cleared.
     *
     * @note
     * - If you enable the built-in encryption, you cannot use the RTMP streaming function.
     * - AR supports four encryption modes. If you choose an encryption mode (excepting `SM4_128_ECB` mode), you need to add an external encryption library when integrating the SDK. See the advanced guide *Channel Encryption*.
     *
     * @param enabled Whether to enable the built-in encryption:
     * - true: Enable the built-in encryption.
     * - false: Disable the built-in encryption.
     * @param config Configurations of built-in encryption schemas. See EncryptionConfig.
     *
     * @return
     * - 0: Success.
     * - < 0: Failure.
     *  - -2(ERR_INVALID_ARGUMENT): An invalid parameter is used. Set the parameter with a valid value.
     *  - -4(ERR_NOT_SUPPORTED): The encryption mode is incorrect or the SDK fails to load the external encryption library. Check the enumeration or reload the external encryption library.
     *  - -7(ERR_NOT_INITIALIZED): The SDK is not initialized. Initialize the `IRtcEngine` instance before calling this method.
     */
    virtual int enableEncryption(bool enabled, const EncryptionConfig& config) = 0;

    /** Registers a packet observer.

     The AR SDK allows your application to register a packet observer to receive callbacks for voice or video packet transmission.
     
     @note
     - The size of the packet sent to the network after processing should not exceed 1200 bytes, otherwise, the packet may fail to be sent.
     - Ensure that both receivers and senders call this method, otherwise, you may meet undefined behaviors such as no voice and black screen.
     - When you use CDN live streaming, recording or storage functions, AR doesn't recommend calling this method.

     @param observer Pointer to the registered packet observer. See IPacketObserver.

     @return
     - 0: Success.
     - < 0: Failure.
     */
    virtual int registerPacketObserver(IPacketObserver* observer) = 0;

    /** Creates a data stream.

     Each user can create up to five data streams during the lifecycle of the IRtcEngine.

     @note Set both the @p reliable and @p ordered parameters to true or false. Do not set one as true and the other as false.

     @param streamId Pointer to the ID of the created data stream.
     @param reliable Sets whether or not the recipients are guaranteed to receive the data stream from the sender within five seconds:
     - true: The recipients receive the data stream from the sender within five seconds. If the recipient does not receive the data stream within five seconds, an error is reported to the application.
     - false: There is no guarantee that the recipients receive the data stream within five seconds and no error message is reported for any delay or missing data stream.
     @param ordered Sets whether or not the recipients receive the data stream in the sent order:
     - true: The recipients receive the data stream in the sent order.
     - false: The recipients do not receive the data stream in the sent order.

     @return
     - Returns 0: Success.
     - < 0: Failure.
     */
    virtual int createDataStream(int* streamId, bool reliable, bool ordered) = 0;

    /** Sends data stream messages to all users in a channel.

     The SDK has the following restrictions on this method:
     - Up to 30 packets can be sent per second in a channel with each packet having a maximum size of 1 kB.
     - Each client can send up to 6 kB of data per second.
     - Each user can have up to five data streams simultaneously.

     A successful \ref ar::rtc::IRtcEngine::sendStreamMessage "sendStreamMessage" method call triggers the \ref ar::rtc::IRtcEngineEventHandler::onStreamMessage "onStreamMessage" callback on the remote client, from which the remote user gets the stream message.

     A failed \ref ar::rtc::IRtcEngine::sendStreamMessage "sendStreamMessage" method call triggers the \ref ar::rtc::IRtcEngineEventHandler::onStreamMessageError "onStreamMessage" callback on the remote client.
     @note This method applies only to the Communication profile or to the hosts in the Live-broadcast profile. If an audience in the Live-broadcast profile calls this method, the audience may be switched to a host.

     @param  streamId  ID of the sent data stream, returned in the \ref IRtcEngine::createDataStream "createDataStream" method.
     @param data Pointer to the sent data.
     @param length Length of the sent data.

     @return
     - 0: Success.
     - < 0: Failure.
     */
    virtual int sendStreamMessage(int streamId, const char* data, size_t length) = 0;

    /** Publishes the local stream to a specified CDN live RTMP address.  (CDN live only.)

     The SDK returns the result of this method call in the \ref IRtcEngineEventHandler::onStreamPublished "onStreamPublished" callback.

     The \ref ar::rtc::IRtcEngine::addPublishStreamUrl "addPublishStreamUrl" method call triggers the \ref ar::rtc::IRtcEngineEventHandler::onRtmpStreamingStateChanged "onRtmpStreamingStateChanged" callback on the local client to report the state of adding a local stream to the CDN.
     @note
     - Ensure that the user joins the channel before calling this method.
     - Ensure that you enable the RTMP Converter service before using this function. See [Prerequisites](https://docs.ar.io/en/Interactive%20Broadcast/cdn_streaming_windows?platform=Windows#prerequisites).
     - This method adds only one stream RTMP URL address each time it is called.
     - This method applies to Live Broadcast only.

     @param url The CDN streaming URL in the RTMP format. The maximum length of this parameter is 1024 bytes. The RTMP URL address must not contain special characters, such as Chinese language characters.
     @param  transcodingEnabled Sets whether transcoding is enabled/disabled:
     - true: Enable transcoding. To [transcode](https://docs.ar.io/en/AR%20Platform/terms?platform=All%20Platforms#transcoding) the audio or video streams when publishing them to CDN live, often used for combining the audio and video streams of multiple hosts in CDN live. If you set this parameter as `true`, ensure that you call the \ref IRtcEngine::setLiveTranscoding "setLiveTranscoding" method before this method.
     - false: Disable transcoding.

     @return
     - 0: Success.
     - < 0: Failure.
          - #ERR_INVALID_ARGUMENT (2): The RTMP URL address is NULL or has a string length of 0.
          - #ERR_NOT_INITIALIZED (7): You have not initialized the RTC engine when publishing the stream.
     */
    virtual int addPublishStreamUrl(const char *url, bool transcodingEnabled) = 0;

    /** Removes an RTMP stream from the CDN. (CDN live only.)

     This method removes the RTMP URL address (added by the \ref IRtcEngine::addPublishStreamUrl "addPublishStreamUrl" method) from a CDN live stream. The SDK returns the result of this method call in the \ref IRtcEngineEventHandler::onStreamUnpublished "onStreamUnpublished" callback.

     The \ref ar::rtc::IRtcEngine::removePublishStreamUrl "removePublishStreamUrl" method call triggers the \ref ar::rtc::IRtcEngineEventHandler::onRtmpStreamingStateChanged "onRtmpStreamingStateChanged" callback on the local client to report the state of removing an RTMP stream from the CDN.
     @note
     - This method removes only one RTMP URL address each time it is called.
     - The RTMP URL address must not contain special characters, such as Chinese language characters.
     - This method applies to Live Broadcast only.

     @param url The RTMP URL address to be removed. The maximum length of this parameter is 1024 bytes.

     @return
     - 0: Success.
     - < 0: Failure.
     */
    virtual int removePublishStreamUrl(const char *url) = 0;

    /** Sets the video layout and audio settings for CDN live. (CDN live only.)
     
     The SDK triggers the \ref ar::rtc::IRtcEngineEventHandler::onTranscodingUpdated "onTranscodingUpdated" callback when you call the `setLiveTranscoding` method to update the transcoding setting.
     
     @note
     - This method applies to Live Broadcast only.
     - Ensure that you enable the RTMP Converter service before using this function. See [Prerequisites](https://docs.ar.io/en/Interactive%20Broadcast/cdn_streaming_windows?platform=Windows#prerequisites).
     - If you call the `setLiveTranscoding` method to update the transcoding setting for the first time, the SDK does not trigger the `onTranscodingUpdated` callback.
  
     @param transcoding Sets the CDN live audio/video transcoding settings. See LiveTranscoding.

     @return
     - 0: Success.
     - < 0: Failure.
     */
    virtual int setLiveTranscoding(const LiveTranscoding &transcoding) = 0;

    /** **DEPRECATED** Adds a watermark image to the local video or CDN live stream.

     This method is deprecated from v2.9.1. Use \ref ar::rtc::IRtcEngine::addVideoWatermark(const char* watermarkUrl, const WatermarkOptions& options) "addVideoWatermark"2 instead.

     This method adds a PNG watermark image to the local video stream for the recording device, channel audience, and CDN live audience to view and capture.

     To add the PNG file to the CDN live publishing stream, see the \ref IRtcEngine::setLiveTranscoding "setLiveTranscoding" method.

     @param watermark Pointer to the watermark image to be added to the local video stream. See RtcImage.

     @note
     - The URL descriptions are different for the local video and CDN live streams:
        - In a local video stream, @p url in RtcImage refers to the absolute path of the added watermark image file in the local video stream.
        - In a CDN live stream, @p url in RtcImage refers to the URL address of the added watermark image in the CDN live broadcast.
     - The source file of the watermark image must be in the PNG file format. If the width and height of the PNG file differ from your settings in this method, the PNG file will be cropped to conform to your settings.
     - The AR SDK supports adding only one watermark image onto a local video or CDN live stream. The newly added watermark image replaces the previous one.

     @return
     - 0: Success.
     - < 0: Failure.
     */
    virtual int addVideoWatermark(const RtcImage& watermark) = 0;

    /** Adds a watermark image to the local video.

     This method adds a PNG watermark image to the local video in a live broadcast. Once the watermark image is added, all the audience in the channel (CDN audience included), 
     and the recording device can see and capture it. AR supports adding only one watermark image onto the local video, and the newly watermark image replaces the previous one.

     The watermark position depends on the settings in the \ref IRtcEngine::setVideoEncoderConfiguration "setVideoEncoderConfiguration" method:
     - If the orientation mode of the encoding video is #ORIENTATION_MODE_FIXED_LANDSCAPE, or the landscape mode in #ORIENTATION_MODE_ADAPTIVE, the watermark uses the landscape orientation.
     - If the orientation mode of the encoding video is #ORIENTATION_MODE_FIXED_PORTRAIT, or the portrait mode in #ORIENTATION_MODE_ADAPTIVE, the watermark uses the portrait orientation.
     - When setting the watermark position, the region must be less than the dimensions set in the `setVideoEncoderConfiguration` method. Otherwise, the watermark image will be cropped.

     @note
     - Ensure that you have called the \ref ar::rtc::IRtcEngine::enableVideo "enableVideo" method to enable the video module before calling this method.
     - If you only want to add a watermark image to the local video for the audience in the CDN live broadcast channel to see and capture, you can call this method or the \ref ar::rtc::IRtcEngine::setLiveTranscoding "setLiveTranscoding" method.
     - This method supports adding a watermark image in the PNG file format only. Supported pixel formats of the PNG image are RGBA, RGB, Palette, Gray, and Alpha_gray.
     - If the dimensions of the PNG image differ from your settings in this method, the image will be cropped or zoomed to conform to your settings.
     - If you have enabled the local video preview by calling the \ref ar::rtc::IRtcEngine::startPreview "startPreview" method, you can use the `visibleInPreview` member in the WatermarkOptions class to set whether or not the watermark is visible in preview.
     - If you have mirrored the local video by calling the \ref ar::rtc::IRtcEngine::setupLocalVideo "setupLocalVideo" or \ref ar::rtc::IRtcEngine::setLocalRenderMode(RENDER_MODE_TYPE renderMode, VIDEO_MIRROR_MODE_TYPE mirrorMode) "setLocalRenderMode" method, the watermark image in preview is also mirrored.
     
     @param watermarkUrl The local file path of the watermark image to be added. This method supports adding a watermark image from the local absolute or relative file path.
     @param options Pointer to the watermark's options to be added. See WatermarkOptions for more infomation.

     @return
     - 0: Success.
     - < 0: Failure.
     */
    virtual int addVideoWatermark(const char* watermarkUrl, const WatermarkOptions& options) = 0;

    /** Removes the watermark image from the video stream added by the \ref ar::rtc::IRtcEngine::addVideoWatermark(const char* watermarkUrl, const WatermarkOptions& options) "addVideoWatermark" method.

     @return
     - 0: Success.
     - < 0: Failure.
     */
    virtual int clearVideoWatermarks() = 0;

    /** Enables/Disables image enhancement and sets the options.

    @note 
    - Call this method after calling the enableVideo method.
    - Currently this method does not apply for macOS.

    @param enabled Sets whether or not to enable image enhancement:
    - true: enables image enhancement.
    - false: disables image enhancement.
    @param options Sets the image enhancement option. See BeautyOptions.
    */
    virtual int setBeautyEffectOptions(bool enabled, BeautyOptions options) = 0;

    /** Adds a voice or video stream URL address to a live broadcast.

    The \ref IRtcEngineEventHandler::onStreamPublished "onStreamPublished" callback returns the inject status. If this method call is successful, the server pulls the voice or video stream and injects it into a live channel. This is applicable to scenarios where all audience members in the channel can watch a live show and interact with each other.

     The \ref ar::rtc::IRtcEngine::addInjectStreamUrl "addInjectStreamUrl" method call triggers the following callbacks:
    - The local client:
      - \ref ar::rtc::IRtcEngineEventHandler::onStreamInjectedStatus "onStreamInjectedStatus" , with the state of the injecting the online stream.
      - \ref ar::rtc::IRtcEngineEventHandler::onUserJoined "onUserJoined" (uid: 666), if the method call is successful and the online media stream is injected into the channel.
    - The remote client:
      - \ref ar::rtc::IRtcEngineEventHandler::onUserJoined "onUserJoined" (uid: 666), if the method call is successful and the online media stream is injected into the channel.

     @note
     - Ensure that you enable the RTMP Converter service before using this function. See [Prerequisites](https://docs.ar.io/en/Interactive%20Broadcast/cdn_streaming_windows?platform=Windows#prerequisites).
     - This method applies to the Native SDK v2.4.1 and later.

     @param url Pointer to the URL address to be added to the ongoing live broadcast. Valid protocols are RTMP, HLS, and FLV.
     - Supported FLV audio codec type: AAC.
     - Supported FLV video codec type: H264 (AVC).
     @param config Pointer to the InjectStreamConfig object that contains the configuration of the added voice or video stream.

     @return
     - 0: Success.
     - < 0: Failure.
        - #ERR_INVALID_ARGUMENT (2): The injected URL does not exist. Call this method again to inject the stream and ensure that the URL is valid.
        - #ERR_NOT_READY (3): The user is not in the channel.
        - #ERR_NOT_SUPPORTED (4): The channel profile is not live broadcast. Call the \ref ar::rtc::IRtcEngine::setChannelProfile "setChannelProfile" method and set the channel profile to live broadcast before calling this method.
        - #ERR_NOT_INITIALIZED (7): The SDK is not initialized. Ensure that the IRtcEngine object is initialized before calling this method.
     */
    virtual int addInjectStreamUrl(const char* url, const InjectStreamConfig& config) = 0;
    /** Starts to relay media streams across channels.
     *
     * After a successful method call, the SDK triggers the
     * \ref ar::rtc::IRtcEngineEventHandler::onChannelMediaRelayStateChanged
     *  "onChannelMediaRelayStateChanged" and
     * \ref ar::rtc::IRtcEngineEventHandler::onChannelMediaRelayEvent
     * "onChannelMediaRelayEvent" callbacks, and these callbacks return the
     * state and events of the media stream relay.
     * - If the
     * \ref ar::rtc::IRtcEngineEventHandler::onChannelMediaRelayStateChanged
     *  "onChannelMediaRelayStateChanged" callback returns
     * #RELAY_STATE_RUNNING (2) and #RELAY_OK (0), and the
     * \ref ar::rtc::IRtcEngineEventHandler::onChannelMediaRelayEvent
     * "onChannelMediaRelayEvent" callback returns
     * #RELAY_EVENT_PACKET_SENT_TO_DEST_CHANNEL (4), the broadcaster starts
     * sending data to the destination channel.
     * - If the
     * \ref ar::rtc::IRtcEngineEventHandler::onChannelMediaRelayStateChanged
     *  "onChannelMediaRelayStateChanged" callback returns
     * #RELAY_STATE_FAILURE (3), an exception occurs during the media stream
     * relay.
     *
     * @note
     * - Call this method after the \ref joinChannel() "joinChannel" method.
     * - This method takes effect only when you are a broadcaster in a
     * Live-broadcast channel.
     * - After a successful method call, if you want to call this method
     * again, ensure that you call the
     * \ref stopChannelMediaRelay() "stopChannelMediaRelay" method to quit the
     * current relay.
     * - Contact sales-us@ar.io before implementing this function.
     * - We do not support string user accounts in this API.
     *
     * @param configuration The configuration of the media stream relay:
     * ChannelMediaRelayConfiguration.
     *
     * @return
     * - 0: Success.
     * - < 0: Failure.
     */
	virtual int startChannelMediaRelay(const ChannelMediaRelayConfiguration &configuration) = 0;
    /** Updates the channels for media stream relay. After a successful
     * \ref startChannelMediaRelay() "startChannelMediaRelay" method call, if
     * you want to relay the media stream to more channels, or leave the
     * current relay channel, you can call the
     * \ref updateChannelMediaRelay() "updateChannelMediaRelay" method.
     *
     * After a successful method call, the SDK triggers the
     * \ref ar::rtc::IRtcEngineEventHandler::onChannelMediaRelayEvent
     *  "onChannelMediaRelayEvent" callback with the
     * #RELAY_EVENT_PACKET_UPDATE_DEST_CHANNEL (7) state code.
     *
     * @note
     * Call this method after the
     * \ref startChannelMediaRelay() "startChannelMediaRelay" method to update
     * the destination channel.
     *
     * @param configuration The media stream relay configuration:
     * ChannelMediaRelayConfiguration.
     *
     * @return
     * - 0: Success.
     * - < 0: Failure.
     */
	virtual int updateChannelMediaRelay(const ChannelMediaRelayConfiguration &configuration) = 0;
    /** Stops the media stream relay.
     *
     * Once the relay stops, the broadcaster quits all the destination
     * channels.
     *
     * After a successful method call, the SDK triggers the
     * \ref ar::rtc::IRtcEngineEventHandler::onChannelMediaRelayStateChanged
     *  "onChannelMediaRelayStateChanged" callback. If the callback returns
     * #RELAY_STATE_IDLE (0) and #RELAY_OK (0), the broadcaster successfully
     * stops the relay.
     *
     * @note
     * If the method call fails, the SDK triggers the
     * \ref ar::rtc::IRtcEngineEventHandler::onChannelMediaRelayStateChanged
     *  "onChannelMediaRelayStateChanged" callback with the
     * #RELAY_ERROR_SERVER_NO_RESPONSE (2) or
     * #RELAY_ERROR_SERVER_CONNECTION_LOST (8) state code. You can leave the
     * channel by calling the \ref leaveChannel() "leaveChannel" method, and
     * the media stream relay automatically stops.
     *
     * @return
     * - 0: Success.
     * - < 0: Failure.
     */
	virtual int stopChannelMediaRelay() = 0;

    /** Removes the voice or video stream URL address from a live broadcast.

     This method removes the URL address (added by the \ref IRtcEngine::addInjectStreamUrl "addInjectStreamUrl" method) from the live broadcast.

     @note If this method is called successfully, the SDK triggers the \ref IRtcEngineEventHandler::onUserOffline "onUserOffline" callback and returns a stream uid of 666.

     @param url Pointer to the URL address of the added stream to be removed.

     @return
     - 0: Success.
     - < 0: Failure.
     */
    virtual int removeInjectStreamUrl(const char* url) = 0;
    virtual bool registerEventHandler(IRtcEngineEventHandler *eventHandler) = 0;
    virtual bool unregisterEventHandler(IRtcEngineEventHandler *eventHandler) = 0;
    /** AR supports reporting and analyzing customized messages.
     *
     * @since v3.1.0
     *
     * This function is in the beta stage with a free trial. The ability provided in its beta test version is reporting a maximum of 10 message pieces within 6 seconds, with each message piece not exceeding 256 bytes.
     * To try out this function, contact [support@ar.io](mailto:support@ar.io) and discuss the format of customized messages with us.
     */
    virtual int sendCustomReportMessage(const char *id, const char* category, const char* event, const char* label, int value) = 0;
    /** Gets the current connection state of the SDK.

     @return #CONNECTION_STATE_TYPE.
     */
    virtual CONNECTION_STATE_TYPE getConnectionState() = 0;
    /// @cond
    /** Enables/Disables the super-resolution algorithm for a remote user's video stream.
     *
     * @since v3.2.0
     *
     * The algorithm effectively improves the resolution of the specified remote user's video stream. When the original
     * resolution of the remote video stream is axb pixels, you can receive and render the stream at a higher
     * resolution (2ax2b pixels) by enabling the algorithm.
     *
     * After calling this method, the SDK triggers the
     * \ref IRtcEngineEventHandler::onUserSuperResolutionEnabled "onUserSuperResolutionEnabled" callback to report
     * whether you have successfully enabled the super-resolution algorithm.
     *
     * @warning The super-resolution algorithm requires extra system resources.
     * To balance the visual experience and system usage, the SDK poses the following restrictions:
     * - The algorithm can only be used for a single user at a time.
     * - On the Android platform, the original resolution of the remote video must not exceed 640x360 pixels.
     * - On the iOS platform, the original resolution of the remote video must not exceed 640x480 pixels.
     * If you exceed these limitations, the SDK triggers the \ref IRtcEngineEventHandler::onWarning "onWarning"
     * callback with the corresponding warning codes:
     * - #WARN_SUPER_RESOLUTION_STREAM_OVER_LIMITATION (1610): The origin resolution of the remote video is beyond the range where the super-resolution algorithm can be applied.
     * - #WARN_SUPER_RESOLUTION_USER_COUNT_OVER_LIMITATION (1611): Another user is already using the super-resolution algorithm.
     * - #WARN_SUPER_RESOLUTION_DEVICE_NOT_SUPPORTED (1612): The device does not support the super-resolution algorithm.
     *
     * @note
     * - This method applies to Android and iOS only.
     * - Requirements for the user's device:
     *  - Android: The following devices are known to support the method:
     *    - VIVO: V1821A, NEX S, 1914A, 1916A, and 1824BA
     *    - OPPO: PCCM00
     *    - OnePlus: A6000
     *    - Xiaomi: Mi 8, Mi 9, MIX3, and Redmi K20 Pro
     *    - SAMSUNG: SM-G9600, SM-G9650, SM-N9600, SM-G9708, SM-G960U, and SM-G9750
     *    - HUAWEI: SEA-AL00, ELE-AL00, VOG-AL00, YAL-AL10, HMA-AL00, and EVR-AN00
     *  - iOS: This method is supported on devices running iOS 12.0 or later. The following
     * device models are known to support the method:
     *      - iPhone XR
     *      - iPhone XS
     *      - iPhone XS Max
     *      - iPhone 11
     *      - iPhone 11 Pro
     *      - iPhone 11 Pro Max
     *      - iPad Pro 11-inch (3rd Generation)
     *      - iPad Pro 12.9-inch (3rd Generation)
     *      - iPad Air 3 (3rd Generation)
     *
     * @param userId The ID of the remote user.
     * @param enable Whether to enable the super-resolution algorithm:
     * - true: Enable the super-resolution algorithm.
     * - false: Disable the super-resolution algorithm.
     *
     * @return
     * - 0: Success.
     * - < 0: Failure.
     */
    virtual int enableRemoteSuperResolution(uid_t userId, bool enable) = 0;
    /// @endcond

    /** Registers the metadata observer.

     Registers the metadata observer. You need to implement the IMetadataObserver class and specify the metadata type in this method. A successful call of this method triggers the \ref ar::rtc::IMetadataObserver::getMaxMetadataSize "getMaxMetadataSize" callback.
     This method enables you to add synchronized metadata in the video stream for more diversified live broadcast interactions, such as sending shopping links, digital coupons, and online quizzes.

     @note
     - Call this method before the joinChannel method.
     - This method applies to the Live-broadcast channel profile.

     @param observer The IMetadataObserver class. See the definition of IMetadataObserver for details.
     @param type See \ref IMetadataObserver::METADATA_TYPE "METADATA_TYPE". The SDK supports VIDEO_METADATA (0) only for now.

     @return
     - 0: Success.
     - < 0: Failure.
     */
    virtual int registerMediaMetadataObserver(IMetadataObserver *observer, IMetadataObserver::METADATA_TYPE type) = 0;
    /** Provides technical preview functionalities or special customizations by configuring the SDK with JSON options.

     The JSON options are not public by default. AR is working on making commonly used JSON options public in a standard way.

     @param parameters Sets the parameter as a JSON string in the specified format.

     @return
     - 0: Success.
     - < 0: Failure.
     */
    virtual int setParameters(const char* parameters) = 0;
};


class IRtcEngineParameter
{
public:
    virtual ~IRtcEngineParameter () {}
    /**
    * Releases all IRtcEngineParameter resources.
    */
    virtual void release() = 0;

    /** Sets the bool value of a specified key in the JSON format.

     @param key Pointer to the name of the key.
     @param value Sets the value.
     @return
     - 0: Success.
     - < 0: Failure.
     */
    virtual int setBool(const char* key, bool value) = 0;

    /** Sets the int value of a specified key in the JSON format.

     @param key Pointer to the name of the key.
     @param value Sets the value.

     @return
     - 0: Success.
     - < 0: Failure.
     */
    virtual int setInt(const char* key, int value) = 0;

    /** Sets the unsigned int value of a specified key in the JSON format.

     @param key Pointer to the name of the key.
     @param value Sets the value.

     @return
     - 0: Success.
     - < 0: Failure.
     */
    virtual int setUInt(const char* key, unsigned int value) = 0;

    /** Sets the double value of a specified key in the JSON format.

     @param key Pointer to the name of the key.
     @param value Sets the value.

     @return
     - 0: Success.
     - < 0: Failure.
     */
    virtual int setNumber(const char* key, double value) = 0;

    /** Sets the string value of a specified key in the JSON format.

     @param key Pointer to the name of the key.
     @param value Pointer to the set value.

     @return
     - 0: Success.
     - < 0: Failure.
     */
    virtual int setString(const char* key, const char* value) = 0;

    /** Sets the object value of a specified key in the JSON format.

     @param key Pointer to the name of the key.
     @param value Pointer to the set value.

     @return
     - 0: Success.
     - < 0: Failure.
     */
    virtual int setObject(const char* key, const char* value) = 0;

    /** Retrieves the bool value of a specified key in the JSON format.

     @param key Pointer to the name of the key.
     @param value Pointer to the retrieved value.

     @return
     - 0: Success.
     - < 0: Failure.
     */
    virtual int getBool(const char* key, bool& value) = 0;

    /** Retrieves the int value of the JSON format.

     @param key Pointer to the name of the key.
     @param value Pointer to the retrieved value.

     @return
     - 0: Success.
     - < 0: Failure.
     */
    virtual int getInt(const char* key, int& value) = 0;

    /** Retrieves the unsigned int value of a specified key in the JSON format.

     @param key Pointer to the name of the key.
     @param value Pointer to the retrieved value.
     @return
     - 0: Success.
     - < 0: Failure.
     */
    virtual int getUInt(const char* key, unsigned int& value) = 0;

    /** Retrieves the double value of a specified key in the JSON format.

     @param key Pointer to the name of the key.
     @param value Pointer to the retrieved value.

     @return
     - 0: Success.
     - < 0: Failure.
     */
    virtual int getNumber(const char* key, double& value) = 0;

    /** Retrieves the string value of a specified key in the JSON format.

     @param key Pointer to the name of the key.
     @param value Pointer to the retrieved value.

     @return
     - 0: Success.
     - < 0: Failure.
    */
    virtual int getString(const char* key, ar::util::AString& value) = 0;

    /** Retrieves a child object value of a specified key in the JSON format.

     @param key Pointer to the name of the key.
     @param value Pointer to the retrieved value.
     @return
     - 0: Success.
     - < 0: Failure.
     */
    virtual int getObject(const char* key, ar::util::AString& value) = 0;

    /** Retrieves the array value of a specified key in the JSON format.

     @param key Pointer to the name of the key.
     @param value Pointer to the retrieved value.
     @return
     - 0: Success.
     - < 0: Failure.
     */
    virtual int getArray(const char* key, ar::util::AString& value) = 0;

    /** Provides the technical preview functionalities or special customizations by configuring the SDK with JSON options.

     @param parameters Pointer to the set parameters in a JSON string.

     @return
     - 0: Success.
     - < 0: Failure.
     */
    virtual int setParameters(const char* parameters) = 0;

    /** Sets the profile to control the RTC engine.

     @param profile Pointer to the set profile.
     @param merge Sets whether to merge the profile data with the original value:
     - true: Merge the profile data with the original value.
     - false: Do not merge the profile data with the original value.

     @return
     - 0: Success.
     - < 0: Failure.
     */
    virtual int setProfile(const char* profile, bool merge) = 0;

	virtual int convertPath(const char* filePath, ar::util::AString& value) = 0;
};

class AAudioDeviceManager : public ar::util::AutoPtr<IAudioDeviceManager>
{
public:
    AAudioDeviceManager(IRtcEngine* engine)
    {
		queryInterface(engine, AR_IID_AUDIO_DEVICE_MANAGER);
    }
};

class AVideoDeviceManager : public ar::util::AutoPtr<IVideoDeviceManager>
{
public:
    AVideoDeviceManager(IRtcEngine* engine)
    {
		queryInterface(engine, AR_IID_VIDEO_DEVICE_MANAGER);
    }
};

class AParameter : public ar::util::AutoPtr<IRtcEngineParameter>
{
public:
    AParameter(IRtcEngine& engine) { initialize(&engine); }
    AParameter(IRtcEngine* engine) { initialize(engine); }
    AParameter(IRtcEngineParameter* p) :ar::util::AutoPtr<IRtcEngineParameter>(p) {}
private:
    bool initialize(IRtcEngine* engine)
    {
        IRtcEngineParameter* p = NULL;
        if (engine && !engine->queryInterface(AR_IID_RTC_ENGINE_PARAMETER, (void**)&p))
            reset(p);
        return p != NULL;
    }
};
/** **DEPRECATED** The RtcEngineParameters class is deprecated, use the IRtcEngine class instead.
*/
class RtcEngineParameters
{
public:
    RtcEngineParameters(IRtcEngine& engine)
        :m_parameter(&engine){}
    RtcEngineParameters(IRtcEngine* engine)
        :m_parameter(engine){}

    
    int enableLocalVideo(bool enabled) {
        return setParameters("{\"rtc.video.capture\":%s,\"che.video.local.capture\":%s,\"che.video.local.render\":%s,\"che.video.local.send\":%s}", enabled ? "true" : "false", enabled ? "true" : "false", enabled ? "true" : "false", enabled ? "true" : "false");
    }


   
    int muteLocalVideoStream(bool mute) {
        return setParameters("{\"rtc.video.mute_me\":%s,\"che.video.local.send\":%s}", mute ? "true" : "false", mute ? "false" : "true");
    }

    
    int muteAllRemoteVideoStreams(bool mute) {
        return m_parameter ? m_parameter->setBool("rtc.video.mute_peers", mute) : -ERR_NOT_INITIALIZED;
    }


    
    int setDefaultMuteAllRemoteVideoStreams(bool mute) {
        return m_parameter ? m_parameter->setBool("rtc.video.set_default_mute_peers", mute) : -ERR_NOT_INITIALIZED;
    }

    
    int muteRemoteVideoStream(uid_t uid, bool mute) {
        return setObject("rtc.video.mute_peer", "{\"uid\":%u,\"mute\":%s}", uid, mute ? "true" : "false");
    }

   
    int setPlaybackDeviceVolume(int volume) {// [0,255]
        return m_parameter ? m_parameter->setInt("che.audio.output.volume", volume) : -ERR_NOT_INITIALIZED;
    }

    
    int startAudioRecording(const char* filePath, AUDIO_RECORDING_QUALITY_TYPE quality) {
        return startAudioRecording(filePath, 32000, quality);
    }

    int startAudioRecording(const char* filePath, int sampleRate, AUDIO_RECORDING_QUALITY_TYPE quality) {
        if (!m_parameter) return -ERR_NOT_INITIALIZED;
#if defined(_WIN32)
        util::AString path;
        if (!m_parameter->convertPath(filePath, path))
            filePath = path->c_str();
        else
            return -ERR_INVALID_ARGUMENT;
#endif
        return setObject("che.audio.start_recording", "{\"filePath\":\"%s\",\"sampleRate\":%d,\"quality\":%d}", filePath, sampleRate, quality);
    }

    
    int stopAudioRecording() {
        return m_parameter ? m_parameter->setBool("che.audio.stop_recording", true) : -ERR_NOT_INITIALIZED;
    }

    
    int startAudioMixing(const char* filePath, bool loopback, bool replace, int cycle) {
        if (!m_parameter) return -ERR_NOT_INITIALIZED;
#if defined(_WIN32)
        util::AString path;
        if (!m_parameter->convertPath(filePath, path))
            filePath = path->c_str();
        else
            return -ERR_INVALID_ARGUMENT;
#endif
        return setObject("che.audio.start_file_as_playout", "{\"filePath\":\"%s\",\"loopback\":%s,\"replace\":%s,\"cycle\":%d}",
                         filePath,
                         loopback?"true":"false",
                         replace?"true":"false",
                         cycle);
    }

   
    int stopAudioMixing() {
        return m_parameter ? m_parameter->setBool("che.audio.stop_file_as_playout", true) : -ERR_NOT_INITIALIZED;
    }

    
    int pauseAudioMixing() {
        return m_parameter ? m_parameter->setBool("che.audio.pause_file_as_playout", true) : -ERR_NOT_INITIALIZED;
    }

    
    int resumeAudioMixing() {
        return m_parameter ? m_parameter->setBool("che.audio.pause_file_as_playout", false) : -ERR_NOT_INITIALIZED;
    }

    
    int adjustAudioMixingVolume(int volume) {
        int ret = adjustAudioMixingPlayoutVolume(volume);
        if (ret == 0) {
            adjustAudioMixingPublishVolume(volume);
        }
        return ret;
    }

    
    int adjustAudioMixingPlayoutVolume(int volume) {
        return m_parameter ? m_parameter->setInt("che.audio.set_file_as_playout_volume", volume) : -ERR_NOT_INITIALIZED;
    }

    
    int getAudioMixingPlayoutVolume() {
        int volume = 0;
        int r = m_parameter ? m_parameter->getInt("che.audio.get_file_as_playout_volume", volume) : -ERR_NOT_INITIALIZED;
        if (r == 0)
            r = volume;
        return r;
    }

    
    int adjustAudioMixingPublishVolume(int volume) {
        return m_parameter ? m_parameter->setInt("che.audio.set_file_as_playout_publish_volume", volume) : -ERR_NOT_INITIALIZED;
    }

   
    int getAudioMixingPublishVolume() {
        int volume = 0;
        int r = m_parameter ? m_parameter->getInt("che.audio.get_file_as_playout_publish_volume", volume) : -ERR_NOT_INITIALIZED;
        if (r == 0)
            r = volume;
        return r;
    }

    
    int getAudioMixingDuration() {
        int duration = 0;
        int r = m_parameter ? m_parameter->getInt("che.audio.get_mixing_file_length_ms", duration) : -ERR_NOT_INITIALIZED;
        if (r == 0)
            r = duration;
        return r;
    }

    
    int getAudioMixingCurrentPosition() {
        if (!m_parameter) return -ERR_NOT_INITIALIZED;
        int pos = 0;
        int r = m_parameter->getInt("che.audio.get_mixing_file_played_ms", pos);
        if (r == 0)
            r = pos;
        return r;
    }
    
    int setAudioMixingPosition(int pos /*in ms*/) {
        return m_parameter ? m_parameter->setInt("che.audio.mixing.file.position", pos) : -ERR_NOT_INITIALIZED;
    }

    int setAudioMixingPitch(int pitch) {
        if (!m_parameter) {
            return -ERR_NOT_INITIALIZED;
        }
        if (pitch > 12 || pitch < -12) {
            return -ERR_INVALID_ARGUMENT;
        }
        return m_parameter->setInt("che.audio.set_playout_file_pitch_semitones", pitch);
    }

    int getEffectsVolume() {
        if (!m_parameter) return -ERR_NOT_INITIALIZED;
        int volume = 0;
        int r = m_parameter->getInt("che.audio.game_get_effects_volume", volume);
        if (r == 0)
            r = volume;
        return r;
    }

    
    int setEffectsVolume(int volume) {
        return m_parameter ? m_parameter->setInt("che.audio.game_set_effects_volume", volume) : -ERR_NOT_INITIALIZED;
    }

   
    int setVolumeOfEffect(int soundId, int volume) {
        return setObject(
                         "che.audio.game_adjust_effect_volume",
                         "{\"soundId\":%d,\"gain\":%d}",
                         soundId, volume);
    }

    
    int playEffect(int soundId, const char* filePath, int loopCount, double pitch, double pan, int gain, bool publish = false) {
#if defined(_WIN32)
        util::AString path;
        if (!m_parameter->convertPath(filePath, path))
            filePath = path->c_str();
        else if (!filePath)
            filePath = "";
#endif
        return setObject(
                         "che.audio.game_play_effect",
                         "{\"soundId\":%d,\"filePath\":\"%s\",\"loopCount\":%d, \"pitch\":%lf,\"pan\":%lf,\"gain\":%d, \"send2far\":%d}",
                         soundId, filePath, loopCount, pitch, pan, gain, publish);
    }

    
    int stopEffect(int soundId) {
        return m_parameter ? m_parameter->setInt(
                                                 "che.audio.game_stop_effect", soundId) : -ERR_NOT_INITIALIZED;
    }

   
    int stopAllEffects() {
        return m_parameter ? m_parameter->setBool(
                                                  "che.audio.game_stop_all_effects", true) : -ERR_NOT_INITIALIZED;
    }

    
    int preloadEffect(int soundId, char* filePath) {
        return setObject(
                         "che.audio.game_preload_effect",
                         "{\"soundId\":%d,\"filePath\":\"%s\"}",
                         soundId, filePath);
    }

    
    int unloadEffect(int soundId) {
        return m_parameter ? m_parameter->setInt(
                                                 "che.audio.game_unload_effect", soundId) : -ERR_NOT_INITIALIZED;
    }

    
    int pauseEffect(int soundId) {
        return m_parameter ? m_parameter->setInt(
                                                 "che.audio.game_pause_effect", soundId) : -ERR_NOT_INITIALIZED;
    }

    
    int pauseAllEffects() {
        return m_parameter ? m_parameter->setBool(
                                                  "che.audio.game_pause_all_effects", true) : -ERR_NOT_INITIALIZED;
    }

    
    int resumeEffect(int soundId) {
        return m_parameter ? m_parameter->setInt(
                                                 "che.audio.game_resume_effect", soundId) : -ERR_NOT_INITIALIZED;
    }

    
    int resumeAllEffects() {
        return m_parameter ? m_parameter->setBool(
                                                  "che.audio.game_resume_all_effects", true) : -ERR_NOT_INITIALIZED;
    }


    int enableSoundPositionIndication(bool enabled) {
        return m_parameter ? m_parameter->setBool(
                                                  "che.audio.enable_sound_position", enabled) : -ERR_NOT_INITIALIZED;
    }

    
    int setRemoteVoicePosition(uid_t uid, double pan, double gain) {
        return setObject("che.audio.game_place_sound_position", "{\"uid\":%u,\"pan\":%lf,\"gain\":%lf}", uid, pan, gain);
    }

    
    int setLocalVoicePitch(double pitch) {
        return m_parameter ? m_parameter->setInt(
                                                 "che.audio.morph.pitch_shift",
                                                 static_cast<int>(pitch * 100)) : -ERR_NOT_INITIALIZED;
    }
    
    int setLocalVoiceEqualization(AUDIO_EQUALIZATION_BAND_FREQUENCY bandFrequency, int bandGain) {
        return setObject(
                         "che.audio.morph.equalization",
                         "{\"index\":%d,\"gain\":%d}",
                         static_cast<int>(bandFrequency), bandGain);
    }
    
    int setLocalVoiceReverb(AUDIO_REVERB_TYPE reverbKey, int value) {
        return setObject(
                         "che.audio.morph.reverb",
                         "{\"key\":%d,\"value\":%d}",
                         static_cast<int>(reverbKey), value);
    }

    
    int setLocalVoiceChanger(VOICE_CHANGER_PRESET voiceChanger) {
        if(!m_parameter)
            return -ERR_NOT_INITIALIZED;
        if(voiceChanger == 0x00000000) {
            return m_parameter->setInt("che.audio.morph.voice_changer", static_cast<int>(voiceChanger));
        }
        else if(voiceChanger > 0x00000000 && voiceChanger < 0x00100000) {
            return m_parameter->setInt("che.audio.morph.voice_changer", static_cast<int>(voiceChanger));
        }
        else if(voiceChanger > 0x00100000 && voiceChanger < 0x00200000) {
            return m_parameter->setInt("che.audio.morph.voice_changer", static_cast<int>(voiceChanger - 0x00100000 + 6));
        }
        else if(voiceChanger > 0x00200000 && voiceChanger < 0x00300000) {
            return m_parameter->setInt("che.audio.morph.beauty_voice", static_cast<int>(voiceChanger - 0x00200000));
        }
        else {
            return -ERR_INVALID_ARGUMENT;
        }
    }

    
    int setLocalVoiceReverbPreset(AUDIO_REVERB_PRESET reverbPreset) {
        if(!m_parameter)
            return -ERR_NOT_INITIALIZED;
        if(reverbPreset == 0x00000000) {
            return m_parameter->setInt("che.audio.morph.reverb_preset", static_cast<int>(reverbPreset));
        }
        else if(reverbPreset > 0x00000000 && reverbPreset < 0x00100000) {
            return m_parameter->setInt("che.audio.morph.reverb_preset", static_cast<int>(reverbPreset + 8));
        }
        else if(reverbPreset > 0x00100000 && reverbPreset < 0x00200000) {
            return m_parameter->setInt("che.audio.morph.reverb_preset", static_cast<int>(reverbPreset - 0x00100000));
        }
        else if(reverbPreset > 0x00200000 && reverbPreset < 0x00200002) {
            return m_parameter->setInt("che.audio.morph.virtual_stereo", static_cast<int>(reverbPreset - 0x00200000));
        }
        else if (reverbPreset > (AUDIO_REVERB_PRESET) 0x00300000 && reverbPreset < (AUDIO_REVERB_PRESET) 0x00300002)
            return setObject( "che.audio.morph.electronic_voice", "{\"key\":%d,\"value\":%d}", 1, 4);
        else if (reverbPreset > (AUDIO_REVERB_PRESET) 0x00400000 && reverbPreset < (AUDIO_REVERB_PRESET) 0x00400002)
            return m_parameter->setInt("che.audio.morph.threedim_voice", 10);
        else {
            return -ERR_INVALID_ARGUMENT;
        }
    }

    int setAudioEffectPreset(AUDIO_EFFECT_PRESET preset){
        if(!m_parameter)
            return -ERR_NOT_INITIALIZED;
        if(preset == AUDIO_EFFECT_OFF) {
            return m_parameter->setInt("che.audio.morph.voice_changer", 0);
        }
        if(preset == ROOM_ACOUSTICS_KTV){
            return m_parameter->setInt("che.audio.morph.reverb_preset", 1);
        }
        if(preset == ROOM_ACOUSTICS_VOCAL_CONCERT) {
            return m_parameter->setInt("che.audio.morph.reverb_preset", 2);
        }
        if(preset == ROOM_ACOUSTICS_STUDIO) {
            return m_parameter->setInt("che.audio.morph.reverb_preset", 5);
        }
        if(preset == ROOM_ACOUSTICS_PHONOGRAPH) {
            return m_parameter->setInt("che.audio.morph.reverb_preset", 8);
        }
        if(preset == ROOM_ACOUSTICS_VIRTUAL_STEREO) {
            return m_parameter->setInt("che.audio.morph.virtual_stereo", 1);
        }
        if(preset == ROOM_ACOUSTICS_SPACIAL) {
            return m_parameter->setInt("che.audio.morph.voice_changer", 15);
        }
        if(preset == ROOM_ACOUSTICS_ETHEREAL) {
            return m_parameter->setInt("che.audio.morph.voice_changer", 5);
        }
        if(preset == ROOM_ACOUSTICS_3D_VOICE) {
            return m_parameter->setInt("che.audio.morph.threedim_voice", 10);
        }
        if(preset == VOICE_CHANGER_EFFECT_UNCLE) {
            return m_parameter->setInt("che.audio.morph.reverb_preset", 3);
        }
        if(preset == VOICE_CHANGER_EFFECT_OLDMAN) {
            return m_parameter->setInt("che.audio.morph.voice_changer", 1);
        }
        if(preset == VOICE_CHANGER_EFFECT_BOY) {
            return m_parameter->setInt("che.audio.morph.voice_changer", 2);
        }
        if(preset == VOICE_CHANGER_EFFECT_SISTER) {
            return m_parameter->setInt("che.audio.morph.reverb_preset", 4);
        }
        if(preset == VOICE_CHANGER_EFFECT_GIRL) {
            return m_parameter->setInt("che.audio.morph.voice_changer", 3);
        }
        if(preset == VOICE_CHANGER_EFFECT_PIGKING) {
            return m_parameter->setInt("che.audio.morph.voice_changer", 4);
        }
        if(preset == VOICE_CHANGER_EFFECT_HULK) {
            return m_parameter->setInt("che.audio.morph.voice_changer", 6);
        }
        if(preset == STYLE_TRANSFORMATION_RNB) {
            return m_parameter->setInt("che.audio.morph.reverb_preset", 7);
        }
        if(preset == STYLE_TRANSFORMATION_POPULAR) {
            return m_parameter->setInt("che.audio.morph.reverb_preset", 6);
        }
        if(preset == PITCH_CORRECTION) {
            return setObject( "che.audio.morph.electronic_voice", "{\"key\":%d,\"value\":%d}", 1, 4);
        }
        return -ERR_INVALID_ARGUMENT;
    }

    int setVoiceBeautifierPreset(VOICE_BEAUTIFIER_PRESET preset) {
        if(!m_parameter)
            return -ERR_NOT_INITIALIZED;
        if(preset == VOICE_BEAUTIFIER_OFF) {
            return m_parameter->setInt("che.audio.morph.voice_changer", 0);
        }
        if(preset == CHAT_BEAUTIFIER_MAGNETIC) {
            return m_parameter->setInt("che.audio.morph.beauty_voice", 1);
        }
        if(preset == CHAT_BEAUTIFIER_FRESH) {
            return m_parameter->setInt("che.audio.morph.beauty_voice", 2);
        }
        if(preset == CHAT_BEAUTIFIER_VITALITY) {
            return m_parameter->setInt("che.audio.morph.beauty_voice", 3);
        }
        /*if(preset == SINGING_BEAUTIFICATION_MAN) {
            return m_parameter->setInt("che.audio.morph.beauty_sing", 1);
        }
        if(preset == SINGING_BEAUTIFICATION_WOMAN) {
            return m_parameter->setInt("che.audio.morph.beauty_sing", 2);
        }*/
        if(preset == TIMBRE_TRANSFORMATION_VIGOROUS) {
            return m_parameter->setInt("che.audio.morph.voice_changer", 7);
        }
        if(preset == TIMBRE_TRANSFORMATION_DEEP) {
            return m_parameter->setInt("che.audio.morph.voice_changer", 8);
        }
        if(preset == TIMBRE_TRANSFORMATION_MELLOW) {
            return m_parameter->setInt("che.audio.morph.voice_changer", 9);
        }
        if(preset == TIMBRE_TRANSFORMATION_FALSETTO) {
            return m_parameter->setInt("che.audio.morph.voice_changer", 10);
        }
        if(preset == TIMBRE_TRANSFORMATION_FULL) {
            return m_parameter->setInt("che.audio.morph.voice_changer", 11);
        }
        if(preset == TIMBRE_TRANSFORMATION_CLEAR) {
            return m_parameter->setInt("che.audio.morph.voice_changer", 12);
        }
        if(preset == TIMBRE_TRANSFORMATION_RESOUNDING) {
            return m_parameter->setInt("che.audio.morph.voice_changer", 13);
        }
        if(preset == TIMBRE_TRANSFORMATION_RINGING) {
            return m_parameter->setInt("che.audio.morph.voice_changer", 14);
        }
        return -ERR_INVALID_ARGUMENT;
    }

    int setAudioEffectParameters(AUDIO_EFFECT_PRESET preset, int param1, int param2){
        if(!m_parameter)
            return -ERR_NOT_INITIALIZED;
        if(preset == PITCH_CORRECTION){
            return setObject( "che.audio.morph.electronic_voice", "{\"key\":%d,\"value\":%d}", param1, param2);
        }
        if(preset == ROOM_ACOUSTICS_3D_VOICE){
            return m_parameter->setInt("che.audio.morph.threedim_voice", param1);
        }
        return -ERR_INVALID_ARGUMENT;
    }

    /** **DEPRECATED** Use \ref IRtcEngine::disableAudio "disableAudio" instead. Disables the audio function in the channel.

     @return
     - 0: Success.
     - < 0: Failure.
     */
    int pauseAudio() {
        return m_parameter ? m_parameter->setBool("che.pause.audio", true) : -ERR_NOT_INITIALIZED;
    }

    
    int resumeAudio() {
        return m_parameter ? m_parameter->setBool("che.pause.audio", false) : -ERR_NOT_INITIALIZED;
    }

    
    int setHighQualityAudioParameters(bool fullband, bool stereo, bool fullBitrate) {
        return setObject("che.audio.codec.hq", "{\"fullband\":%s,\"stereo\":%s,\"fullBitrate\":%s}", fullband ? "true" : "false", stereo ? "true" : "false", fullBitrate ? "true" : "false");
    }

    
    int adjustRecordingSignalVolume(int volume) {//[0, 400]: e.g. 50~0.5x 100~1x 400~4x
        if (volume < 0)
            volume = 0;
        else if (volume > 400)
            volume = 400;
        return m_parameter ? m_parameter->setInt("che.audio.record.signal.volume", volume) : -ERR_NOT_INITIALIZED;
    }

    
    int adjustPlaybackSignalVolume(int volume) {//[0, 400]
        if (volume < 0)
            volume = 0;
        else if (volume > 400)
            volume = 400;
        return m_parameter ? m_parameter->setInt("che.audio.playout.signal.volume", volume) : -ERR_NOT_INITIALIZED;
    }

    
    int enableAudioVolumeIndication(int interval, int smooth, bool report_vad) { // in ms: <= 0: disable, > 0: enable, interval in ms
        if (interval < 0)
            interval = 0;
        return setObject("che.audio.volume_indication", "{\"interval\":%d,\"smooth\":%d,\"vad\":%d}", interval, smooth, report_vad);
    }

    
    int muteLocalAudioStream(bool mute) {
        return setParameters("{\"rtc.audio.mute_me\":%s,\"che.audio.mute_me\":%s}", mute ? "true" : "false", mute ? "true" : "false");
    }
    // mute/unmute all peers. unmute will clear all muted peers specified mutePeer() interface

    
    int muteRemoteAudioStream(uid_t uid, bool mute) {
        return setObject("rtc.audio.mute_peer", "{\"uid\":%u,\"mute\":%s}", uid, mute?"true":"false");
    }

    
    int muteAllRemoteAudioStreams(bool mute) {
        return m_parameter ? m_parameter->setBool("rtc.audio.mute_peers", mute) : -ERR_NOT_INITIALIZED;
    }

    
    int setDefaultMuteAllRemoteAudioStreams(bool mute) {
        return m_parameter ? m_parameter->setBool("rtc.audio.set_default_mute_peers", mute) : -ERR_NOT_INITIALIZED;
    }

    
    int setExternalAudioSource(bool enabled, int sampleRate, int channels) {
        if (enabled)
            return setParameters("{\"che.audio.external_capture\":true,\"che.audio.external_capture.push\":true,\"che.audio.set_capture_raw_audio_format\":{\"sampleRate\":%d,\"channelCnt\":%d,\"mode\":%d}}", sampleRate, channels, RAW_AUDIO_FRAME_OP_MODE_TYPE::RAW_AUDIO_FRAME_OP_MODE_READ_WRITE);
        else
            return setParameters("{\"che.audio.external_capture\":false,\"che.audio.external_capture.push\":false}");
    }

    
    int setExternalAudioSink(bool enabled, int sampleRate, int channels) {
        if (enabled)
            return setParameters("{\"che.audio.external_render\":true,\"che.audio.external_render.pull\":true,\"che.audio.set_render_raw_audio_format\":{\"sampleRate\":%d,\"channelCnt\":%d,\"mode\":%d}}", sampleRate, channels, RAW_AUDIO_FRAME_OP_MODE_TYPE::RAW_AUDIO_FRAME_OP_MODE_READ_ONLY);
        else
            return setParameters("{\"che.audio.external_render\":false,\"che.audio.external_render.pull\":false}");
    }

    
    int setLogFile(const char* filePath) {
        if (!m_parameter) return -ERR_NOT_INITIALIZED;
#if defined(_WIN32)
        util::AString path;
        if (!m_parameter->convertPath(filePath, path))
            filePath = path->c_str();
        else if (!filePath)
            filePath = "";
#endif
        return m_parameter->setString("rtc.log_file", filePath);
    }

    
    int setLogFilter(unsigned int filter) {
        return m_parameter ? m_parameter->setUInt("rtc.log_filter", filter&LOG_FILTER_MASK) : -ERR_NOT_INITIALIZED;
    }

    
    int setLogFileSize(unsigned int fileSizeInKBytes) {
        return m_parameter ? m_parameter->setUInt("rtc.log_size", fileSizeInKBytes) : -ERR_NOT_INITIALIZED;
    }

   
    int setLocalRenderMode(RENDER_MODE_TYPE renderMode) {
        return setRemoteRenderMode(0, renderMode);
    }

    
    int setRemoteRenderMode(uid_t uid, RENDER_MODE_TYPE renderMode) {
        return setParameters("{\"che.video.render_mode\":[{\"uid\":%u,\"renderMode\":%d}]}", uid, renderMode);
    }

    
    int setCameraCapturerConfiguration(const CameraCapturerConfiguration& config) {
        if (!m_parameter) return -ERR_NOT_INITIALIZED;
        return m_parameter->setInt("che.video.camera_capture_mode", (int)config.preference);
    }

    
    int enableDualStreamMode(bool enabled) {
        return setParameters("{\"rtc.dual_stream_mode\":%s,\"che.video.enableLowBitRateStream\":%d}", enabled ? "true" : "false", enabled ? 1 : 0);
    }

    
    int setRemoteVideoStreamType(uid_t uid, REMOTE_VIDEO_STREAM_TYPE streamType) {
        return setParameters("{\"rtc.video.set_remote_video_stream\":{\"uid\":%u,\"stream\":%d}, \"che.video.setstream\":{\"uid\":%u,\"stream\":%d}}", uid, streamType, uid, streamType);
//        return setObject("rtc.video.set_remote_video_stream", "{\"uid\":%u,\"stream\":%d}", uid, streamType);
    }

    
    int setRemoteDefaultVideoStreamType(REMOTE_VIDEO_STREAM_TYPE streamType) {
        return m_parameter ? m_parameter->setInt("rtc.video.set_remote_default_video_stream_type", streamType) : -ERR_NOT_INITIALIZED;
    }

    
    int setRecordingAudioFrameParameters(int sampleRate, int channel, RAW_AUDIO_FRAME_OP_MODE_TYPE mode, int samplesPerCall) {
        return setObject("che.audio.set_capture_raw_audio_format", "{\"sampleRate\":%d,\"channelCnt\":%d,\"mode\":%d,\"samplesPerCall\":%d}", sampleRate, channel, mode, samplesPerCall);
    }
    
    int setPlaybackAudioFrameParameters(int sampleRate, int channel, RAW_AUDIO_FRAME_OP_MODE_TYPE mode, int samplesPerCall) {
        return setObject("che.audio.set_render_raw_audio_format", "{\"sampleRate\":%d,\"channelCnt\":%d,\"mode\":%d,\"samplesPerCall\":%d}", sampleRate, channel, mode, samplesPerCall);
    }
    
    int setMixedAudioFrameParameters(int sampleRate, int samplesPerCall) {
        return setObject("che.audio.set_mixed_raw_audio_format", "{\"sampleRate\":%d,\"samplesPerCall\":%d}", sampleRate, samplesPerCall);
    }

    
    int enableWebSdkInteroperability(bool enabled) {//enable interoperability with zero-plugin web sdk
        return setParameters("{\"rtc.video.web_h264_interop_enable\":%s,\"che.video.web_h264_interop_enable\":%s}", enabled ? "true" : "false", enabled ? "true" : "false");
    }

    //only for live broadcast
    
    int setVideoQualityParameters(bool preferFrameRateOverImageQuality) {
        return setParameters("{\"rtc.video.prefer_frame_rate\":%s,\"che.video.prefer_frame_rate\":%s}", preferFrameRateOverImageQuality ? "true" : "false", preferFrameRateOverImageQuality ? "true" : "false");
    }

    
    int setLocalVideoMirrorMode(VIDEO_MIRROR_MODE_TYPE mirrorMode) {
        if (!m_parameter) return -ERR_NOT_INITIALIZED;
        const char *value;
        switch (mirrorMode) {
            case VIDEO_MIRROR_MODE_AUTO:
                value = "default";
                break;
            case VIDEO_MIRROR_MODE_ENABLED:
                value = "forceMirror";
                break;
            case VIDEO_MIRROR_MODE_DISABLED:
                value = "disableMirror";
                break;
            default:
                return -ERR_INVALID_ARGUMENT;
        }
        return m_parameter->setString("che.video.localViewMirrorSetting", value);
    }

   
    int setLocalPublishFallbackOption(STREAM_FALLBACK_OPTIONS option) {
        return m_parameter ? m_parameter->setInt("rtc.local_publish_fallback_option", option) : -ERR_NOT_INITIALIZED;
    }

    
    int setRemoteSubscribeFallbackOption(STREAM_FALLBACK_OPTIONS option) {
        return m_parameter ? m_parameter->setInt("rtc.remote_subscribe_fallback_option", option) : -ERR_NOT_INITIALIZED;
    }

#if (defined(__APPLE__) && TARGET_OS_MAC && !TARGET_OS_IPHONE) || defined(_WIN32)
    
    int enableLoopbackRecording(bool enabled, const char* deviceName = NULL) {
        if (!deviceName) {
            return setParameters("{\"che.audio.loopback.recording\":%s}", enabled ? "true" : "false");
        }
        else {
            return setParameters("{\"che.audio.loopback.deviceName\":\"%s\",\"che.audio.loopback.recording\":%s}", deviceName, enabled ? "true" : "false");
        }
    }
#endif

    
    int setInEarMonitoringVolume(int volume) {
        return m_parameter ? m_parameter->setInt("che.audio.headset.monitoring.parameter", volume) : -ERR_NOT_INITIALIZED;
    }

protected:
    AParameter& parameter() {
        return m_parameter;
    }
    int setParameters(const char* format, ...) {
        char buf[512];
        va_list args;
        va_start(args, format);
        vsnprintf(buf, sizeof(buf)-1, format, args);
        va_end(args);
        return m_parameter ? m_parameter->setParameters(buf) : -ERR_NOT_INITIALIZED;
    }
    int setObject(const char* key, const char* format, ...) {
        char buf[512];
        va_list args;
        va_start(args, format);
        vsnprintf(buf, sizeof(buf)-1, format, args);
        va_end(args);
        return m_parameter ? m_parameter->setObject(key, buf) : -ERR_NOT_INITIALIZED;
    }
    int stopAllRemoteVideo() {
        return m_parameter ? m_parameter->setBool("che.video.peer.stop_render", true) : -ERR_NOT_INITIALIZED;
    }
private:
    AParameter m_parameter;
};

} //namespace rtc
} // namespace ar


#define getARRtcEngineVersion getARSdkVersion

////////////////////////////////////////////////////////
/** \addtogroup createARRtcEngine
 @{
 */
////////////////////////////////////////////////////////

/** Creates the IRtcEngine object and returns the pointer.
 * 
 * @return Pointer to the IRtcEngine object.
 */ 
AR_API ar::rtc::IRtcEngine* AR_CALL createARRtcEngine();

////////////////////////////////////////////////////////
/** @} */
////////////////////////////////////////////////////////

#define getARRtcEngineErrorDescription getARSdkErrorDescription
#define setARRtcEngineExternalSymbolLoader setARSdkExternalSymbolLoader

#endif

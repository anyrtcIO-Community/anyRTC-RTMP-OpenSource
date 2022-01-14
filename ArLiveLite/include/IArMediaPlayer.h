//
//  anyrtc Rtc Engine SDK
//
//  Copyright (c) 2019 anyrtc.io. All rights reserved.
//

#ifndef __I_AR_MEDIA_PLAYER_H__
#define __I_AR_MEDIA_PLAYER_H__

#include "ArMediaBase.h"

#ifndef AR
#define AR ar::rtc
#endif

// external key
/**
 * set analyze duration for real time stream
 * @example "setPlayerOption(KEY_PLAYER_REAL_TIME_STREAM_ANALYZE_DURATION,1000000)"
 */
#define KEY_PLAYER_REAL_TIME_STREAM_ANALYZE_DURATION    "analyzeduration"

/**
 * set the player disable to play audio
 * @example  "setPlayerOption(KEY_PLAYER_DISABLE_AUDIO,0)"
 */
#define KEY_PLAYER_DISABLE_AUDIO                  "audio_disable"

/**
 * set the player disable to play video
 * @example  "setPlayerOption(KEY_PLAYER_DISABLE_VIDEO,0)"
 */
#define KEY_PLAYER_DISABLE_VIDEO                  "video_disable"

namespace ar {
namespace rtc {

/** Definition of MediaPlayerContext.
 */
struct MediaPlayerContext {
  /** User Context, i.e., activity context in Android.
   */
  void* context;

  MediaPlayerContext()
      : context(nullptr) {}
};

class IMediaPlayerObserver;

/**
 * @brief Player interface
 *
 */
class IMediaPlayer {
 public:

  virtual int initialize(const MediaPlayerContext& context) = 0;

  /**
   * @brief Open media file
   *
   * @param src Media path, local path or network path
   * @param startPos Set the starting position for playback, in seconds
   * @return int <= 0 On behalf of an error, the value corresponds to one of PLAYER_ERROR
   */
  virtual int open(const char* src, int64_t startPos) = 0;

  /**
   * @brief Play
   *
   * @return int <= 0 On behalf of an error, the value corresponds to one of PLAYER_ERROR
   */
  virtual int play() = 0;

  /**
   * @brief pause
   *
   * @return int <= 0 On behalf of an error, the value corresponds to one of PLAYER_ERROR
   */
  virtual int pause() = 0;

  /**
   * @brief stop
   *
   * @return int <= 0 On behalf of an error, the value corresponds to one of PLAYER_ERROR
   */
  virtual int stop() = 0;

  /**
   * @brief Play to a specified position
   *
   * @param pos The position to play, in seconds
   * @return int <= 0 On behalf of an error, the value corresponds to one of PLAYER_ERROR
   */
  virtual int seek(int64_t pos) = 0;

  /**
   * @brief Turn mute on or off
   *
   * @param mute Whether to mute on
   * @return int <= 0 On behalf of an error, the value corresponds to one of PLAYER_ERROR
   */
  virtual int mute(bool mute) = 0;

  /**
   * @brief Get mute state
   *
   * @param[out] mute Whether is mute on
   * @return int <= 0 On behalf of an error, the value corresponds to one of PLAYER_ERROR
   */
  virtual int getMute(bool& mute) = 0;

  /**
   * @brief Adjust playback volume
   *
   * @param volume The volume value to be adjusted
   * The volume can be adjusted from 0 to 400:
   * 0: mute;
   * 100: original volume;
   * 400: Up to 4 times the original volume (with built-in overflow protection).
   * @return int <= 0 On behalf of an error, the value corresponds to one of PLAYER_ERROR
   */
  virtual int adjustPlayoutVolume(int volume) = 0;

  /**
   * @brief Get the current playback volume
   *
   * @param[out] volume
   * @return int <= 0 On behalf of an error, the value corresponds to one of PLAYER_ERROR
   */
  virtual int getPlayoutVolume(int& volume) = 0;

  /**
   * @brief Get the current playback progress
   *
   * @param[out] pos Progress in seconds
   * @return int <= 0 On behalf of an error, the value corresponds to one of PLAYER_ERROR
   */
  virtual int getPlayPosition(int64_t& pos) = 0;

  /**
   * @brief Get the current playback progress
   *
   * @param[out] pos Progress in millisecond
   * @return int <= 0 On behalf of an error, the value corresponds to one of PLAYER_ERROR
   */
  virtual int getPlayPositionInMillisecond(int64_t& positionInMs) = 0;

  /**
   * @brief Get media duration
   *
   * @param[out] duration Duration in seconds
   * @return int <= 0 On behalf of an error, the value corresponds to one of PLAYER_ERROR
   */
  virtual int getDuration(int64_t& duration) = 0;

  /**
   * @brief Get player state
   *
   * @return PLAYER_STATE
   */
  virtual ar::media::MEDIA_PLAYER_STATE getState() = 0;

  /**
   * @brief Get the streams info count in the media
   *
   * @param[out] count
   * @return int <= 0 On behalf of an error, the value corresponds to one of PLAYER_ERROR
   */
  virtual int getStreamCount(int& count) = 0;

  /**
   * @brief Get the streams info by index
   *
   * @param[in] index, index
   * @param[out] info, stream info for return
   */
  virtual int getStreamInfo(int index, ar::media::MediaStreamInfo* info) = 0;

  /**
   * @brief Set video rendering view
   *
   * @param view view object, windows platform is HWND
   * @return int <= 0 On behalf of an error, the value corresponds to one of PLAYER_ERROR
   */
  virtual int setView(ar::media::base::view_t view) = 0;

  /**
   * @brief Set video display mode
   *
   * @param renderMode Video display mode
   * @return int <= 0 On behalf of an error, the value corresponds to one of PLAYER_ERROR
   */
  virtual int setRenderMode(ar::media::base::RENDER_MODE_TYPE renderMode) = 0;

  /**
   * @brief Register the player observer
   *
   * @param observer observer object
   * @return int <= 0 On behalf of an error, the value corresponds to one of PLAYER_ERROR
   */
  virtual int registerPlayerObserver(IMediaPlayerObserver* observer) = 0;

  /**
   * @brief Unregister the player observer
   *
   * @param observer observer object
   * @return int <= 0 On behalf of an error, the value corresponds to one of PLAYER_ERROR
   */
  virtual int unregisterPlayerObserver(IMediaPlayerObserver* observer) = 0;

  /**
   * @brief Register the player video observer
   *
   * @param observer observer object
   * @return int <= 0 On behalf of an error, the value corresponds to one of PLAYER_ERROR
   */
  virtual int registerVideoFrameObserver(ar::media::base::IVideoFrameObserver* observer) = 0;

  /**
   * @brief UnRegister the player video observer
   *
   * @param observer observer object
   * @return int <= 0 On behalf of an error, the value corresponds to one of PLAYER_ERROR
   */
  virtual int unregisterVideoFrameObserver(ar::media::base::IVideoFrameObserver* observer) = 0;

  /**
   * @brief register the player audio observer
   *
   * @param observer observer object
   * @return int <= 0 On behalf of an error, the value corresponds to one of PLAYER_ERROR
   */
  virtual int registerAudioFrameObserver(ar::media::base::IAudioFrameObserver* observer) = 0;

  /**
   * @brief Unregister the player audio observer
   *
   * @param observer observer object
   * @return int <= 0 On behalf of an error, the value corresponds to one of PLAYER_ERROR
   */
  virtual int unregisterAudioFrameObserver(ar::media::base::IAudioFrameObserver* observer) = 0;

  /**
   * @brief change log file position
   *
   * @return int <= 0 On behalf of an error, the value corresponds to one of PLAYER_ERROR
   */
  virtual int setLogFile(const char* filePath) = 0;

  /**
   * Sets the output log level of the SDK.
   *
   * You can use one or a combination of the filters. The log level follows the
   * sequence of `OFF`, `CRITICAL`, `ERROR`, `WARNING`, `INFO`, and `DEBUG`. Choose a level
   * and you will see logs preceding that level. For example, if you set the log level to
   * `WARNING`, you see the logs within levels `CRITICAL`, `ERROR`, and `WARNING`.
   *
   * @param filter Sets the log filter level:
   * - LOG_FILTER_DEBUG (0x80f): Output all API logs. Set your log filter as DEBUG
   * if you want to get the most complete log file.
   * - LOG_FILTER_INFO (0x0f): Output logs of the CRITICAL, ERROR, WARNING, and INFO
   * level. We recommend setting your log filter as this level.
   * - LOG_FILTER_WARNING (0x0e): Output logs of the CRITICAL, ERROR, and WARNING level.
   * - LOG_FILTER_ERROR (0x0c): Output logs of the CRITICAL and ERROR level.
   * - LOG_FILTER_CRITICAL (0x08): Output logs of the CRITICAL level.
   * - LOG_FILTER_OFF (0): Do not output any log.
   *
   * @return
   * - 0: Success.
   * - < 0: Failure.
   */
  virtual int setLogFilter(unsigned int filter) = 0;

   /**
   * @brief modify player option before play,
   * @param [in] key
   *        the option key name
   * @param [in] value
   *        the option value
   * @return int <= 0 On behalf of an error, the value corresponds to one of PLAYER_ERROR
   */
  virtual int setPlayerOption(const char *key ,int value) = 0;

  /**
   * @brief change playback speed
   *
   * @return int <= 0 On behalf of an error, the value corresponds to one of PLAYER_ERROR
   */
  virtual int changePlaybackSpeed(const ar::media::MEDIA_PLAYER_PLAY_SPEED speed) = 0;

  /**
   * @brief change audio track
   *
   * @return int <= 0 On behalf of an error, the value corresponds to one of PLAYER_ERROR
   */
  virtual int selectAudioTrack(int index) = 0;

  /**
   * @brief release IMediaPlayer object.
   *
   */
  virtual void release(bool sync = true) = 0;

  virtual ~IMediaPlayer() {}
};

class IMediaPlayerObserver {
 public:
  /**
   * @brief Triggered when the player state changes
   *
   * @param state New player state
   * @param ec Player error message
   */
  virtual void onPlayerStateChanged(ar::media::MEDIA_PLAYER_STATE state,
                                    ar::media::MEDIA_PLAYER_ERROR ec) = 0;

  /**
   * @brief Triggered when the player progress changes, once every 1 second
   *
   * @param position Current playback progress, in seconds
   */
  virtual void onPositionChanged(const int64_t position) = 0;

  /**
   * @brief Triggered when the player have some event
   *
   * @param event
   */
  virtual void onPlayerEvent(ar::media::MEDIA_PLAYER_EVENT event) = 0;

  /**
   * @brief Triggered when metadata is obtained
   *
   * @param type Metadata type
   * @param data data
   * @param length  data length
   */
  virtual void onMetadata(ar::media::MEDIA_PLAYER_METADATA_TYPE type, const uint8_t* data,
                                  uint32_t length) = 0;

  virtual ~IMediaPlayerObserver() {}
};

}  // namespace rtc
}  // namespace ar


#if defined(_WIN32)
#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
#include <windows.h>
#define AGORA_PLAYER_API extern "C" __declspec(dllexport)
#define AGORA_PLAYER_CALL
#elif defined(__APPLE__)
#include <TargetConditionals.h>
#define AGORA_PLAYER_API __attribute__((visibility("default"))) extern "C"
#define AGORA_PLAYER_CALL
#elif defined(__ANDROID__) || defined(__linux__)
#define AGORA_PLAYER_API extern "C" __attribute__((visibility("default")))
#define AGORA_PLAYER_CALL
#else
#define AGORA_PLAYER_API extern "C"
#define AGORA_PLAYER_CALL
#endif

/**
 * Creates an anyrtc media player object and returns the pointer.
 * @return
 * - The pointer to \ref ar::rtc::IMediaPlayer "IMediaPlayer", if the method call succeeds.
 * - The empty pointer NULL, if the method call fails.
 */
AGORA_PLAYER_API ar::rtc::IMediaPlayer* AGORA_PLAYER_CALL createArMediaPlayer();

#endif  // __I_AR_MEDIA_PLAYER_H__

//
//  anyrtc Engine SDK
//
//  Created by Sting Feng in 2017-11.
//  Copyright (c) 2017 anyrtc.io. All rights reserved.

#ifndef __AR_MEDIA_BASE_H__  // NOLINT(build/header_guard)
#define __AR_MEDIA_BASE_H__

#include <stdint.h>
#include <stddef.h>

namespace ar {
namespace media {
namespace base {

typedef void* view_t;

typedef const char* user_id_t;


/** The video pixel format.
 */
enum VIDEO_PIXEL_FORMAT {
  /** 0: The video pixel format is unknown.
   */
  VIDEO_PIXEL_UNKNOWN = 0,
  /** 1: The video pixel format is I420.
   */
  VIDEO_PIXEL_I420 = 1,
  /** 2: The video pixel format is BGRA.
   */
  VIDEO_PIXEL_BGRA = 2,
  /** 3: Planar YUV 4:2:2 format.
   */
  VIDEO_PIXEL_I422 = 3,
  /** 2: The video pixel format is RGBA.
   */
  VIDEO_PIXEL_RGBA = 4,
  /** 8: The video pixel format is NV12.
   */
  VIDEO_PIXEL_NV12 = 8,
};

/** 
 * The video display mode. 
 */
enum RENDER_MODE_TYPE {
  /**
   * 1: Uniformly scale the video until it fills the visible boundaries
   * (cropped). One dimension of the video may have clipped contents.
   */
  RENDER_MODE_HIDDEN = 1,
  /**
   * 2: Uniformly scale the video until one of its dimension fits the boundary
   * (zoomed to fit). Areas that are not filled due to the disparity in the
   * aspect ratio will be filled with black.
   */
  RENDER_MODE_FIT = 2,
  /**
   * @deprecated
   * 3: This mode is deprecated.
   */
  RENDER_MODE_ADAPTIVE = 3,
};

/** Definition of VideoFrame.

The video data format is in YUV420. The buffer provides a pointer to a pointer. However, the
interface cannot modify the pointer of the buffer, but can only modify the content of the buffer.

*/
struct VideoFrame {
  VIDEO_PIXEL_FORMAT type;
  /** Video pixel width.
   */
  int width;  // width of video frame
  /** Video pixel height.
   */
  int height;  // height of video frame
  /** Line span of Y buffer in YUV data.
   */
  int yStride;  // stride of Y data buffer
  /** Line span of U buffer in YUV data.
   */
  int uStride;  // stride of U data buffer
  /** Line span of V buffer in YUV data.
   */
  int vStride;  // stride of V data buffer
  /** Pointer to the Y buffer pointer in the YUV data.
   */
  uint8_t* yBuffer;  // Y data buffer
  /** Pointer to the U buffer pointer in the YUV data.
   */
  uint8_t* uBuffer;  // U data buffer
  /** Pointer to the V buffer pointer in the YUV data
   */
  uint8_t* vBuffer;  // V data buffer
  /** Set the rotation of this frame before rendering the video, and it supports 0, 90, 180, 270
   * degrees clockwise.
   */
  int rotation;  // rotation of this frame (0, 90, 180, 270)
  /** Timestamp to render the video stream. It instructs the users to use this timestamp to
  synchronize the video stream render while rendering the video streams.

  Note: This timestamp is for rendering the video stream, not for capturing the video stream.
  */
  int64_t renderTimeMs;
  int avsync_type;
};

/**
 * The struct of AudioPcmFrame.
 */
struct AudioPcmFrame {
  /**
   * The buffer size of the PCM audio frame.
   */
  enum : size_t {
    // Stereo, 32 kHz, 60 ms (2 * 32 * 60)
    kMaxDataSizeSamples = 3840,
    kMaxDataSizeBytes = kMaxDataSizeSamples * sizeof(int16_t),
  };

  AudioPcmFrame() {}

  uint32_t capture_timestamp = 0;
  size_t samples_per_channel_ = 0;
  int sample_rate_hz_ = 0;
  size_t num_channels_ = 0;
  size_t bytes_per_sample = 0;
  int16_t data_[kMaxDataSizeSamples] = {0};

  AudioPcmFrame(const AudioPcmFrame& frame) = delete;
  void operator=(const AudioPcmFrame& frame) = delete;
};

class IVideoFrameObserver {
 public:
  virtual void onFrame(const VideoFrame* frame) = 0;
  virtual ~IVideoFrameObserver() {}
};

class IAudioFrameObserver {
 public:
  virtual void onFrame(const AudioPcmFrame* frame) = 0;
  virtual ~IAudioFrameObserver() {}
};

}  // namespace base
}  // namespace media
}  // namespace ar

namespace ar {
namespace media {

/**
 * @brief Player state
 *
 */
enum MEDIA_PLAYER_STATE {
  /** Default state
   */
  PLAYER_STATE_IDLE = 0,
  /** Opening media file
   */
  PLAYER_STATE_OPENING = 1,
  /** Media file opened successfully
   *
   */
  PLAYER_STATE_OPEN_COMPLETED = 2,
  /** Player playing
   */
  PLAYER_STATE_PLAYING = 3,
  /** Player paused
   */
  PLAYER_STATE_PAUSED = 4,
  /** Player playback complete
   */
  PLAYER_STATE_PLAYBACK_COMPLETED = 5,
  /** Player stopped
   */
  PLAYER_STATE_STOPPED = 6,
  /** Player failed
   */
  PLAYER_STATE_FAILED = 100,
};

/**
 * @brief Player error code
 *
 */
enum MEDIA_PLAYER_ERROR {
  /** No error
   */
  PLAYER_ERROR_NONE = 0,
  /** The parameter is incorrect
   */
  PLAYER_ERROR_INVALID_ARGUMENTS = -1,
  /** Internel error
   */
  PLAYER_ERROR_INTERNAL = -2,
  /** No resource error
   */
  PLAYER_ERROR_NO_RESOURCE = -3,
  /** Media source is invalid
   */
  PLAYER_ERROR_INVALID_MEDIA_SOURCE = -4,
  /** Unknown stream type
   */
  PLAYER_ERROR_UNKNOWN_STREAM_TYPE = -5,
  /** Object is not initialized
   */
  PLAYER_ERROR_OBJ_NOT_INITIALIZED = -6,
  /** Decoder codec not supported
   */
  PLAYER_ERROR_CODEC_NOT_SUPPORTED = -7,
  /** Video renderer is invalid
   */
  PLAYER_ERROR_VIDEO_RENDER_FAILED = -8,
  /** Internal state error
   */
  PLAYER_ERROR_INVALID_STATE = -9,
  /** Url not found
   */
  PLAYER_ERROR_URL_NOT_FOUND = -10,
  /** Invalid connection state
   */
  PLAYER_ERROR_INVALID_CONNECTION_STATE = -11,
  /** Insufficient buffer data
   */
  PLAY_ERROR_SRC_BUFFER_UNDERFLOW = -12,
};

/**
 * @brief Media stream type
 *
 */
enum MEDIA_STREAM_TYPE {
  /** Unknown stream type
   */
  STREAM_TYPE_UNKNOWN = 0,
  /** Video stream
   */
  STREAM_TYPE_VIDEO = 1,
  /** Audio stream
   */
  STREAM_TYPE_AUDIO = 2,
  /** Subtitle stream
   */
  STREAM_TYPE_SUBTITLE = 3,
};

/**
 * @brief Playback speed type
 *
 */
enum MEDIA_PLAYER_PLAY_SPEED {
  /** origin playback speed
   */
  ORIGIN_PLAYBACK_SPEED = 100,
  /** playback speed slow down to 0.75
   */
  PLAYBACK_SPEED_75_PERCENT = 75,
  /** playback speed slow down to 0.5
   */
  PLAYBACK_SPEED_50_PERCENT = 50,
  /** playback speed speed up to 1.25
   */
  PLAYBACK_SPEED_125_PERCENT = 125,
  /** playback speed speed up to 1.5
   */
  PLAYBACK_SPEED_150_PERCENT = 150,
    /** playback speed speed up to 2.0
   */
  PLAYBACK_SPEED_200_PERCENT = 200,
};

/**
 * @brief Player event
 *
 */
enum MEDIA_PLAYER_EVENT {
  /** seek complete
   */
  PLAYER_EVENT_SEEK_BEGIN = 0,
  /** seek complete
   */
  PLAYER_EVENT_SEEK_COMPLETE = 1,
  /** seek failed
   */
  PLAYER_EVENT_SEEK_ERROR = 2,
  /** player video published
   */
  PLAYER_EVENT_VIDEO_PUBLISHED = 3,
  /** player audio published
   */
  PLAYER_EVENT_AUDIO_PUBLISHED = 4,
  /** player audio track changed
   */
  PLAYER_EVENT_AUDIO_TRACK_CHANGED = 5,
};

/**
 * @brief Media stream object
 *
 */
static const uint8_t kMaxCodecNameLength = 50;
struct MediaStreamInfo { /* the index of the stream in the media file */
  int streamIndex;

  /* stream type */
  MEDIA_STREAM_TYPE streamType;

  /* stream encoding name */
  char codecName[kMaxCodecNameLength];

  /* streaming language */
  char language[kMaxCodecNameLength];

  /* If it is a video stream, video frames rate */
  int videoFrameRate;

  /* If it is a video stream, video bit rate */
  int videoBitRate;

  /* If it is a video stream, video width */
  int videoWidth;

  /* If it is a video stream, video height */
  int videoHeight;

  /* If it is a video stream, video rotation */
  int videoRotation;

  /* If it is an audio stream, audio bit rate */
  int audioSampleRate;

  /* If it is an audio stream, the number of audio channels */
  int audioChannels;

  /* stream duration in second */
  int64_t duration;};

/**
 * @brief Player Metadata type
 *
 */
enum MEDIA_PLAYER_METADATA_TYPE {
  /** data type unknown
   */
  PLAYER_METADATA_TYPE_UNKNOWN = 0,
  /** sei data
   */
  PLAYER_METADATA_TYPE_SEI = 1,
};

}  // namespace media

namespace rtc {
/**
 * The audio route.
 */
enum AudioRoute
{
  /**
   * -1: The default audio route.
   */
  ROUTE_DEFAULT = -1,
  /**
   * The headset.
   */
  ROUTE_HEADSET,
  /**
   * The earpiece.
   */
  ROUTE_EARPIECE,
  /**
   * The headset with no microphone.
   */
  ROUTE_HEADSETNOMIC,
  /**
   * The speakerphone.
   */
  ROUTE_SPEAKERPHONE,
  /**
   * The loudspeaker.
   */
  ROUTE_LOUDSPEAKER,
  /**
   * The Bluetooth headset.
   */
  ROUTE_HEADSETBLUETOOTH
};
} // namespace rtc


}  // namespace ar

#endif // __AR_MEDIA_BASE_H__

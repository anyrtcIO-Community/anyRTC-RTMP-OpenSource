/*
 *  Copyright (c) 2012 The WebRTC project authors. All Rights Reserved.
 *
 *  Use of this source code is governed by a BSD-style license
 *  that can be found in the LICENSE file in the root of the source
 *  tree. An additional intellectual property rights grant can be found
 *  in the file PATENTS.  All contributing project authors may
 *  be found in the AUTHORS file in the root of the source tree.
 */

#ifndef AUDIO_DEVICE_AUDIO_DEVICE_DUMMY_H_
#define AUDIO_DEVICE_AUDIO_DEVICE_DUMMY_H_

#include <stdint.h>
#include "api/call/audio_sink.h"
#include "rtc_base/deprecated/recursive_critical_section.h"
#include "modules/audio_device/audio_device_buffer.h"
#include "modules/audio_device/audio_device_generic.h"
#include "modules/audio_device/include/audio_device.h"
#include "modules/audio_device/include/audio_device_defines.h"

namespace webrtc {

class AudioDeviceDummy : public AudioDeviceGeneric {
 public:
  AudioDeviceDummy()
      : _ptrAudioBuffer(NULL),
        _playing(false),
        _playIsInitialized(false),
        _playout_index(0),
        _playoutFramesIn10MS(0),
        _playoutFramesLeft(0),
        _recording(false), 
        _recIsInitialized(false),
        _record_index(0),
        _recordingFramesIn10MS(0),
        _recordingBufferSizeIn10MS(0),
        _recordingFramesLeft(0),
        total_delay_in_milliseconds_(20) {}
  virtual ~AudioDeviceDummy() {}
  //* Play audio
  int GetNeedPlayAudio(void* audioSamples, uint32_t& samplesPerSec, size_t& nChannels);
  //* Recod audio
  void SetRecordAudioData(const AudioSinkInterface::Data& audio);

  // Retrieve the currently utilized audio layer
  int32_t ActiveAudioLayer(
      AudioDeviceModule::AudioLayer& audioLayer) const override;

  // Main initializaton and termination
  InitStatus Init() override;
  int32_t Terminate() override;
  bool Initialized() const override;

  // Device enumeration
  int16_t PlayoutDevices() override;
  int16_t RecordingDevices() override;
  int32_t PlayoutDeviceName(uint16_t index,
                            char name[kAdmMaxDeviceNameSize],
                            char guid[kAdmMaxGuidSize]) override;
  int32_t RecordingDeviceName(uint16_t index,
                              char name[kAdmMaxDeviceNameSize],
                              char guid[kAdmMaxGuidSize]) override;

  // Device selection
  int32_t SetPlayoutDevice(uint16_t index) override;
  int32_t SetPlayoutDevice(
      AudioDeviceModule::WindowsDeviceType device) override;
  int32_t SetRecordingDevice(uint16_t index) override;
  int32_t SetRecordingDevice(
      AudioDeviceModule::WindowsDeviceType device) override;

  // Audio transport initialization
  int32_t PlayoutIsAvailable(bool& available) override;
  int32_t InitPlayout() override;
  bool PlayoutIsInitialized() const override;
  int32_t RecordingIsAvailable(bool& available) override;
  int32_t InitRecording() override;
  bool RecordingIsInitialized() const override;

  // Audio transport control
  int32_t StartPlayout() override;
  int32_t StopPlayout() override;
  bool Playing() const override;
  int32_t StartRecording() override;
  int32_t StopRecording() override;
  bool Recording() const override;

  // Audio mixer initialization
  int32_t InitSpeaker() override;
  bool SpeakerIsInitialized() const override;
  int32_t InitMicrophone() override;
  bool MicrophoneIsInitialized() const override;

  // Speaker volume controls
  int32_t SpeakerVolumeIsAvailable(bool& available) override;
  int32_t SetSpeakerVolume(uint32_t volume) override;
  int32_t SpeakerVolume(uint32_t& volume) const override;
  int32_t MaxSpeakerVolume(uint32_t& maxVolume) const override;
  int32_t MinSpeakerVolume(uint32_t& minVolume) const override;

  // Microphone volume controls
  int32_t MicrophoneVolumeIsAvailable(bool& available) override;
  int32_t SetMicrophoneVolume(uint32_t volume) override;
  int32_t MicrophoneVolume(uint32_t& volume) const override;
  int32_t MaxMicrophoneVolume(uint32_t& maxVolume) const override;
  int32_t MinMicrophoneVolume(uint32_t& minVolume) const override;

  // Speaker mute control
  int32_t SpeakerMuteIsAvailable(bool& available) override;
  int32_t SetSpeakerMute(bool enable) override;
  int32_t SpeakerMute(bool& enabled) const override;

  // Microphone mute control
  int32_t MicrophoneMuteIsAvailable(bool& available) override;
  int32_t SetMicrophoneMute(bool enable) override;
  int32_t MicrophoneMute(bool& enabled) const override;

  // Stereo support
  int32_t StereoPlayoutIsAvailable(bool& available) override;
  int32_t SetStereoPlayout(bool enable) override;
  int32_t StereoPlayout(bool& enabled) const override;
  int32_t StereoRecordingIsAvailable(bool& available) override;
  int32_t SetStereoRecording(bool enable) override;
  int32_t StereoRecording(bool& enabled) const override;

  // Delay information and control
  int32_t PlayoutDelay(uint16_t& delayMS) const override;

  void AttachAudioBuffer(AudioDeviceBuffer* audioBuffer) override;
 private:
  AudioDeviceBuffer* _ptrAudioBuffer;
  bool _playing;
  bool _playIsInitialized;
  int32_t _playout_index;
  size_t _playoutFramesIn10MS;
  uint32_t _playoutFramesLeft;

  bool _recording;
  bool _recIsInitialized;
  int32_t _record_index;
  size_t _recordingFramesIn10MS;
  size_t _recordingBufferSizeIn10MS;
  uint32_t _recordingFramesLeft;

   rtc::RecursiveCriticalSection cs_lock_;


   // Delay estimate of the total round-trip delay (input + output).
   // Fixed value set once in AttachAudioBuffer() and it can take one out of two
   // possible values. See audio_common.h for details.
   int total_delay_in_milliseconds_;  //@Eric - Just for Android
};

}  // namespace webrtc

#endif  // AUDIO_DEVICE_AUDIO_DEVICE_DUMMY_H_

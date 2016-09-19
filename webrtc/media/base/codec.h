/*
 *  Copyright (c) 2004 The WebRTC@AnyRTC project authors. All Rights Reserved.
 *
 *  Use of this source code is governed by a BSD-style license
 *  that can be found in the LICENSE file in the root of the source
 *  tree. An additional intellectual property rights grant can be found
 *  in the file PATENTS.  All contributing project authors may
 *  be found in the AUTHORS file in the root of the source tree.
 */

#ifndef WEBRTC_MEDIA_BASE_CODEC_H_
#define WEBRTC_MEDIA_BASE_CODEC_H_

#include <map>
#include <set>
#include <string>
#include <vector>

#include "webrtc/api/rtpparameters.h"
#include "webrtc/media/base/mediaconstants.h"

namespace cricket {

typedef std::map<std::string, std::string> CodecParameterMap;

extern const int kMaxPayloadId;

class FeedbackParam {
 public:
  FeedbackParam(const std::string& id, const std::string& param)
      : id_(id),
        param_(param) {
  }
  explicit FeedbackParam(const std::string& id)
      : id_(id),
        param_(kParamValueEmpty) {
  }
  bool operator==(const FeedbackParam& other) const;

  const std::string& id() const { return id_; }
  const std::string& param() const { return param_; }

 private:
  std::string id_;  // e.g. "nack", "ccm"
  std::string param_;  // e.g. "", "rpsi", "fir"
};

class FeedbackParams {
 public:
  bool operator==(const FeedbackParams& other) const;

  bool Has(const FeedbackParam& param) const;
  void Add(const FeedbackParam& param);

  void Intersect(const FeedbackParams& from);

  const std::vector<FeedbackParam>& params() const { return params_; }
 private:
  bool HasDuplicateEntries() const;

  std::vector<FeedbackParam> params_;
};

struct Codec {
  int id;
  std::string name;
  int clockrate;
  CodecParameterMap params;
  FeedbackParams feedback_params;

  // Creates a codec with the given parameters.
  Codec(int id, const std::string& name, int clockrate);
  // Creates an empty codec.
  Codec();
  Codec(const Codec& c);
  virtual ~Codec();

  // Indicates if this codec is compatible with the specified codec.
  bool Matches(const Codec& codec) const;

  // Find the parameter for |name| and write the value to |out|.
  bool GetParam(const std::string& name, std::string* out) const;
  bool GetParam(const std::string& name, int* out) const;

  void SetParam(const std::string& name, const std::string& value);
  void SetParam(const std::string& name, int value);

  // It is safe to input a non-existent parameter.
  // Returns true if the parameter existed, false if it did not exist.
  bool RemoveParam(const std::string& name);

  bool HasFeedbackParam(const FeedbackParam& param) const;
  void AddFeedbackParam(const FeedbackParam& param);

  // Filter |this| feedbacks params such that only those shared by both |this|
  // and |other| are kept.
  void IntersectFeedbackParams(const Codec& other);

  virtual webrtc::RtpCodecParameters ToCodecParameters() const;

  Codec& operator=(const Codec& c);

  bool operator==(const Codec& c) const;

  bool operator!=(const Codec& c) const {
    return !(*this == c);
  }
};

struct AudioCodec : public Codec {
  int bitrate;
  size_t channels;

  // Creates a codec with the given parameters.
  AudioCodec(int id,
             const std::string& name,
             int clockrate,
             int bitrate,
             size_t channels);
  // Creates an empty codec.
  AudioCodec();
  AudioCodec(const AudioCodec& c);
  virtual ~AudioCodec() = default;

  // Indicates if this codec is compatible with the specified codec.
  bool Matches(const AudioCodec& codec) const;

  std::string ToString() const;

  webrtc::RtpCodecParameters ToCodecParameters() const override;

  AudioCodec& operator=(const AudioCodec& c);

  bool operator==(const AudioCodec& c) const;

  bool operator!=(const AudioCodec& c) const {
    return !(*this == c);
  }
};

struct VideoCodec : public Codec {
  int width;
  int height;
  int framerate;

  // Creates a codec with the given parameters.
  VideoCodec(int id,
             const std::string& name,
             int width,
             int height,
             int framerate);
  VideoCodec(int id, const std::string& name);
  // Creates an empty codec.
  VideoCodec();
  VideoCodec(const VideoCodec& c);
  virtual ~VideoCodec() = default;

  std::string ToString() const;

  VideoCodec& operator=(const VideoCodec& c);

  bool operator==(const VideoCodec& c) const;

  bool operator!=(const VideoCodec& c) const {
    return !(*this == c);
  }

  static VideoCodec CreateRtxCodec(int rtx_payload_type,
                                   int associated_payload_type);

  enum CodecType {
    CODEC_VIDEO,
    CODEC_RED,
    CODEC_ULPFEC,
    CODEC_RTX,
  };

  CodecType GetCodecType() const;
  // Validates a VideoCodec's payload type, dimensions and bitrates etc. If they
  // don't make sense (such as max < min bitrate), and error is logged and
  // ValidateCodecFormat returns false.
  bool ValidateCodecFormat() const;
};

struct DataCodec : public Codec {
  DataCodec(int id, const std::string& name);
  DataCodec();
  DataCodec(const DataCodec& c);
  virtual ~DataCodec() = default;

  DataCodec& operator=(const DataCodec& c);

  std::string ToString() const;
};

// Get the codec setting associated with |payload_type|. If there
// is no codec associated with that payload type it returns false.
template <class Codec>
bool FindCodecById(const std::vector<Codec>& codecs,
                   int payload_type,
                   Codec* codec_out) {
  for (const auto& codec : codecs) {
    if (codec.id == payload_type) {
      *codec_out = codec;
      return true;
    }
  }
  return false;
}

bool CodecNamesEq(const std::string& name1, const std::string& name2);
bool HasNack(const Codec& codec);
bool HasRemb(const Codec& codec);
bool HasTransportCc(const Codec& codec);

}  // namespace cricket

#endif  // WEBRTC_MEDIA_BASE_CODEC_H_

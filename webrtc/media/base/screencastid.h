/*
 *  Copyright (c) 2012 The WebRTC project authors. All Rights Reserved.
 *
 *  Use of this source code is governed by a BSD-style license
 *  that can be found in the LICENSE file in the root of the source
 *  tree. An additional intellectual property rights grant can be found
 *  in the file PATENTS.  All contributing project authors may
 *  be found in the AUTHORS file in the root of the source tree.
 */

// Author: thorcarpenter@google.com (Thor Carpenter)
//
// Defines variant class ScreencastId that combines WindowId and DesktopId.

#ifndef WEBRTC_MEDIA_BASE_SCREENCASTID_H_
#define WEBRTC_MEDIA_BASE_SCREENCASTID_H_

#include <string>
#include <vector>

#include "webrtc/base/window.h"
#include "webrtc/base/windowpicker.h"

namespace cricket {

class ScreencastId;
typedef std::vector<ScreencastId> ScreencastIdList;

// Used for identifying a window or desktop to be screencast.
class ScreencastId {
 public:
  enum Type { INVALID, WINDOW, DESKTOP };

  // Default constructor indicates invalid ScreencastId.
  ScreencastId() : type_(INVALID) {}
  explicit ScreencastId(const rtc::WindowId& id)
      : type_(WINDOW), window_(id) {
  }
  explicit ScreencastId(const rtc::DesktopId& id)
      : type_(DESKTOP), desktop_(id) {
  }

  Type type() const { return type_; }
  const rtc::WindowId& window() const { return window_; }
  const rtc::DesktopId& desktop() const { return desktop_; }

  // Title is an optional parameter.
  const std::string& title() const { return title_; }
  void set_title(const std::string& desc) { title_ = desc; }

  bool IsValid() const {
    if (type_ == INVALID) {
      return false;
    } else if (type_ == WINDOW) {
      return window_.IsValid();
    } else {
      return desktop_.IsValid();
    }
  }
  bool IsWindow() const { return type_ == WINDOW; }
  bool IsDesktop() const { return type_ == DESKTOP; }
  bool EqualsId(const ScreencastId& other) const {
    if (type_ != other.type_) {
      return false;
    }
    if (type_ == INVALID) {
      return true;
    } else if (type_ == WINDOW) {
      return window_.Equals(other.window());
    }
    return desktop_.Equals(other.desktop());
  }

  // T is assumed to be WindowDescription or DesktopDescription.
  template<class T>
  static cricket::ScreencastIdList Convert(const std::vector<T>& list) {
    ScreencastIdList screencast_list;
    screencast_list.reserve(list.size());
    for (typename std::vector<T>::const_iterator it = list.begin();
         it != list.end(); ++it) {
      ScreencastId id(it->id());
      id.set_title(it->title());
      screencast_list.push_back(id);
    }
    return screencast_list;
  }

 private:
  Type type_;
  rtc::WindowId window_;
  rtc::DesktopId desktop_;
  std::string title_;  // Optional.
};

}  // namespace cricket

#endif  // WEBRTC_MEDIA_BASE_SCREENCASTID_H_

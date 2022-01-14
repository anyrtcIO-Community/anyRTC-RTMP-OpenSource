/*
 *  Copyright (c) 2013 The WebRTC@AnyRTC project authors. All Rights Reserved.
 *
 *  Use of this source code is governed by a BSD-style license
 *  that can be found in the LICENSE file in the root of the source
 *  tree. An additional intellectual property rights grant can be found
 *  in the file PATENTS.  All contributing project authors may
 *  be found in the AUTHORS file in the root of the source tree.
 *
 *  This is a part of the AR RTC Service SDK.
 *  Copyright (C) 2021 anRTC IO
 *  All rights reserved.
 *  Author - Eric.Mao
 *  Email - maozongwu@dync.cc
 *  Website - https://www.anyrtc.io
 *
 */
#ifndef ANYRTC_NATIVE_HANDLE_BUFFER_H_
#define ANYRTC_NATIVE_HANDLE_BUFFER_H_
#include "api/video/video_frame_buffer.h"
#include "rtc_base/checks.h"
#include "api/scoped_refptr.h"

namespace webrtc {
class NativeHandleBuffer : public VideoFrameBuffer{
 public:
   NativeHandleBuffer(void* native_handle, int width, int height)
    :native_handle_(native_handle)
    ,width_(width)
    ,height_(height) {}
   Type type() const override { return Type::kNative; }
   int width() const override { return width_; }
   int height() const override { return height_; }
   rtc::scoped_refptr<I420BufferInterface> ToI420() override {
     RTC_NOTREACHED();
     return nullptr;
   }

   void* native_handle() { return native_handle_; }
 private:
   void* native_handle_;
   const int width_;
   const int height_;
};
}  // namespace webrtc
#endif  // ANYRTC_NATIVE_HANDLE_BUFFER_H_

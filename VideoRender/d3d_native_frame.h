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
#ifndef ANYRTC_WIN_D3DNATIVEFRAME_H
#define ANYRTC_WIN_D3DNATIVEFRAME_H
#include <d3d9.h>
#include <dxva2api.h>
#include "common_video/include/video_frame_buffer.h"
namespace webrtc {
// structure that containts d3d9 device manager
// and d3d9 surface that contains decoded frame.
struct NativeD3DSurfaceHandle {
  IDirect3DSurface9* surface_;
  IDirect3DDeviceManager9* dev_manager_;
  UINT dev_manager_reset_token_;
  int width_;   // width of the frame passing from decoder
  int height_;  // height of the frame passing from decoder
};

// We keep D3D11ImageHandle definition in videorendererinterface,
// as we may allow external renderer to accept the same handle.
}  // namespace webrtc
#endif  // ANYRTC_WIN_D3DNATIVEFRAME_H

/*
 *  Copyright (c) 2013 The WebRTC@AnyRTC project authors. All Rights Reserved.
 *
 *  Use of this source code is governed by a BSD-style license
 *  that can be found in the LICENSE file in the root of the source
 *  tree. An additional intellectual property rights grant can be found
 *  in the file PATENTS.  All contributing project authors may
 *  be found in the AUTHORS file in the root of the source tree.
 */
#ifndef WEBRTC_TEST_WIN_D3D_RENDERER_H_
#define WEBRTC_TEST_WIN_D3D_RENDERER_H_

#include <WinSock2.h>
#include <d3d9.h>
#pragma comment(lib, "d3d9.lib")       // located in DirectX SDK

#include "video_renderer.h"
#include "webrtc/base/scoped_ref_ptr.h"
#include "webrtc/typedefs.h"

namespace webrtc {

class D3dRenderer : public VideoRenderer {
 public:
  static D3dRenderer* Create(const void* hwnd, size_t width,
                             size_t height);
  virtual ~D3dRenderer();

  void OnFrame(const cricket::VideoFrame& frame) override;

 private:
  D3dRenderer(size_t width, size_t height);

  bool Init(const void* hwnd);
  void Resize(size_t width, size_t height);
  void Destroy();

  size_t width_, height_;
  int fps_;

  HWND hwnd_;
  rtc::scoped_refptr<IDirect3D9> d3d_;
  rtc::scoped_refptr<IDirect3DDevice9> d3d_device_;

  rtc::scoped_refptr<IDirect3DTexture9> texture_;
  rtc::scoped_refptr<IDirect3DVertexBuffer9> vertex_buffer_;
};
}  // namespace webrtc

#endif  // WEBRTC_TEST_WIN_D3D_RENDERER_H_

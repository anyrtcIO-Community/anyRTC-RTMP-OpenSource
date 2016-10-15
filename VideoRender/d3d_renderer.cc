/*
 *  Copyright (c) 2013 The WebRTC@AnyRTC project authors. All Rights Reserved.
 *
 *  Use of this source code is governed by a BSD-style license
 *  that can be found in the LICENSE file in the root of the source
 *  tree. An additional intellectual property rights grant can be found
 *  in the file PATENTS.  All contributing project authors may
 *  be found in the AUTHORS file in the root of the source tree.
 */
#include "d3d_renderer.h"

#include "webrtc/common_video/libyuv/include/webrtc_libyuv.h"

namespace webrtc {

#define D3DFVF_CUSTOMVERTEX (D3DFVF_XYZ|D3DFVF_TEX1)

struct D3dCustomVertex {
  float x, y, z;
  float u, v;
};

const char kD3DClassName[] = "d3d_renderer";

#if 1
VideoRenderer* VideoRenderer::CreatePlatformRenderer(const void* hwnd,
                                                     size_t width,
                                                     size_t height) {
  return D3dRenderer::Create(hwnd, width, height);
}
#endif

D3dRenderer::D3dRenderer(size_t width, size_t height)
    : width_(width),
      height_(height),
	  fps_(0),
      hwnd_(NULL),
      d3d_(NULL),
      d3d_device_(NULL),
      texture_(NULL),
      vertex_buffer_(NULL) {
  assert(width > 0);
  assert(height > 0);
}

D3dRenderer::~D3dRenderer() { Destroy(); }

void D3dRenderer::Destroy() {
  texture_ = NULL;
  vertex_buffer_ = NULL;
  d3d_device_ = NULL;
  d3d_ = NULL;

  if (hwnd_ != NULL) {
    hwnd_ = NULL;
  }
}

bool D3dRenderer::Init(const void* hwnd) { 
  hwnd_ = (HWND)hwnd;

  if (hwnd_ == NULL) {
    return false;
  }

  d3d_ = Direct3DCreate9(D3D_SDK_VERSION);
  if (d3d_ == NULL) {
    Destroy();
    return false;
  }

  D3DPRESENT_PARAMETERS d3d_params = {};

  d3d_params.Windowed = TRUE;
  d3d_params.SwapEffect = D3DSWAPEFFECT_COPY;

  IDirect3DDevice9* d3d_device;
  if (d3d_->CreateDevice(D3DADAPTER_DEFAULT,
                         D3DDEVTYPE_HAL,
                         hwnd_,
                         D3DCREATE_SOFTWARE_VERTEXPROCESSING,
                         &d3d_params,
                         &d3d_device) != D3D_OK) {
    Destroy();
    return false;
  }
  d3d_device_ = d3d_device;
  d3d_device->Release();

  IDirect3DVertexBuffer9* vertex_buffer;
  const int kRectVertices = 4;
  if (d3d_device_->CreateVertexBuffer(kRectVertices * sizeof(D3dCustomVertex),
                                      0,
                                      D3DFVF_CUSTOMVERTEX,
                                      D3DPOOL_MANAGED,
                                      &vertex_buffer,
                                      NULL) != D3D_OK) {
    Destroy();
    return false;
  }
  vertex_buffer_ = vertex_buffer;
  vertex_buffer->Release();

  d3d_device_->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);
  d3d_device_->SetRenderState(D3DRS_LIGHTING, FALSE);
  Resize(width_, height_);

  d3d_device_->Present(NULL, NULL, NULL, NULL);

  return true;
}

D3dRenderer* D3dRenderer::Create(const void*hwnd, 
                                 size_t width,
                                 size_t height) {
  D3dRenderer* d3d_renderer = new D3dRenderer(width, height);
  if (!d3d_renderer->Init(hwnd)) {
    delete d3d_renderer;
    return NULL;
  }

  return d3d_renderer;
}

void D3dRenderer::Resize(size_t width, size_t height) {
  width_ = width;
  height_ = height;
  IDirect3DTexture9* texture;

  d3d_device_->CreateTexture(static_cast<UINT>(width_),
                             static_cast<UINT>(height_),
                             1,
                             0,
                             D3DFMT_A8R8G8B8,
                             D3DPOOL_MANAGED,
                             &texture,
                             NULL);
  texture_ = texture;
  texture->Release();

  // Vertices for the video frame to be rendered to.
  static const D3dCustomVertex rect[] = {
    {-1.0f, -1.0f, 0.0f, 0.0f, 1.0f},
    {-1.0f, 1.0f, 0.0f, 0.0f, 0.0f},
    {1.0f, -1.0f, 0.0f, 1.0f, 1.0f},
    {1.0f, 1.0f, 0.0f, 1.0f, 0.0f},
  };

  void* buf_data;
  if (vertex_buffer_->Lock(0, 0, &buf_data, 0) != D3D_OK)
    return;

  memcpy(buf_data, &rect, sizeof(rect));
  vertex_buffer_->Unlock();
}

void D3dRenderer::OnFrame(const cricket::VideoFrame& frame) {
	if (fps_++ % 2 == 0) {
		return;
	}
	const cricket::VideoFrame*videoFrame = &frame;
  if (static_cast<size_t>(videoFrame->width()) != width_ ||
      static_cast<size_t>(videoFrame->height()) != height_) {
    Resize(static_cast<size_t>(videoFrame->width()),
           static_cast<size_t>(videoFrame->height()));
  }

  D3DLOCKED_RECT lock_rect;
  if (texture_->LockRect(0, &lock_rect, NULL, 0) != D3D_OK)
    return;

  webrtc::VideoFrame video_frame(videoFrame->video_frame_buffer(), 0, 0, videoFrame->rotation());
  ConvertFromI420(video_frame, kARGB, 0, static_cast<uint8_t*>(lock_rect.pBits));
  texture_->UnlockRect(0);

  d3d_device_->BeginScene();
  d3d_device_->SetFVF(D3DFVF_CUSTOMVERTEX);
  d3d_device_->SetStreamSource(0, vertex_buffer_, 0, sizeof(D3dCustomVertex));
  d3d_device_->SetTexture(0, texture_);
  d3d_device_->DrawPrimitive(D3DPT_TRIANGLESTRIP, 0, 2);
  d3d_device_->EndScene();

  d3d_device_->Present(NULL, NULL, NULL, NULL);
}
}  // namespace webrtc

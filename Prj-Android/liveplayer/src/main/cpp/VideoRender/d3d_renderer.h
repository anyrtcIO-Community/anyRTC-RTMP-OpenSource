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
 *  Copyright (C) 2020 anRTC IO
 *  All rights reserved.
 *  Author - Eric.Mao
 *  Email - maozongwu@dync.cc
 *  Website - https://www.anyrtc.io
 *
 */
#ifndef WEBRTC_TEST_WIN_D3D_RENDERER_H_
#define WEBRTC_TEST_WIN_D3D_RENDERER_H_

#include <WinSock2.h>
#include <d3d9.h>
#pragma comment(lib, "d3d9.lib")       // located in DirectX SDK

#include "video_renderer.h"

namespace webrtc {

class D3dRenderer : public VideoRenderer {
 public:
  static D3dRenderer* Create(const void* hwnd, size_t width,
                             size_t height);
public:
	D3dRenderer(HWND wnd)
		: wnd_(wnd),
		inited_for_raw_(false),
		width_(0),
		height_(0) {}
	virtual void OnFrame(const webrtc::VideoFrame& frame) override;
	virtual ~D3dRenderer() { Destroy(); }
private:
	void Destroy();
	void Resize(size_t width, size_t height);
	HWND wnd_;
	bool inited_for_raw_;
	size_t width_;
	size_t height_;
	rtc::scoped_refptr<IDirect3D9> m_d3d_;
	rtc::scoped_refptr<IDirect3DDevice9> m_d3d_device_;
	rtc::scoped_refptr<IDirect3DTexture9> m_texture_;
	rtc::scoped_refptr<IDirect3DVertexBuffer9> m_vertex_buffer_;
};
}  // namespace webrtc

#endif  // WEBRTC_TEST_WIN_D3D_RENDERER_H_

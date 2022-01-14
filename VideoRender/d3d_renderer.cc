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
#include "d3d_renderer.h"
#include "d3d_native_frame.h"
#include "native_handle_buffer.h"
#include "common_video/libyuv/include/webrtc_libyuv.h"
#include "rtc_base/logging.h"
#include <d3d9.h>
#include <d3d9types.h>
#include <atlbase.h>
#include <codecapi.h>
#include <combaseapi.h>
#include <dxva2api.h>
#include <Windows.h>

namespace webrtc {


VideoRenderer* VideoRenderer::CreatePlatformRenderer(const void* hwnd,
                                                     size_t width,
                                                     size_t height) {
  return D3dRenderer::Create(hwnd, width, height);
}

#define D3DFVF_CUSTOMVERTEX (D3DFVF_XYZ | D3DFVF_TEX1)
struct D3dCustomVertex {
	float x, y, z;
	float u, v;
};

D3dRenderer* D3dRenderer::Create(const void*hwnd,
	size_t width,
	size_t height) {
	D3dRenderer* d3d_renderer = new D3dRenderer((HWND)hwnd);

	return d3d_renderer;
}

void D3dRenderer::Destroy() {
	m_texture_ = nullptr;
	m_vertex_buffer_ = nullptr;
	m_d3d_device_ = nullptr;
	m_d3d_ = nullptr;
	inited_for_raw_ = false;
}

void D3dRenderer::Resize(size_t width, size_t height) {
	width_ = width;
	height_ = height;
	IDirect3DTexture9* texture;

	m_d3d_device_->CreateTexture(static_cast<UINT>(width),
		static_cast<UINT>(height), 1, 0, D3DFMT_A8R8G8B8,
		D3DPOOL_MANAGED, &texture, nullptr);
	m_texture_ = texture;
	texture->Release();

	// Vertices for the video frame to be rendered to.
	static const D3dCustomVertex rect[] = {
		{-1.0f, -1.0f, 0.0f, 0.0f, 1.0f},
		{-1.0f, 1.0f, 0.0f, 0.0f, 0.0f},
		{1.0f, -1.0f, 0.0f, 1.0f, 1.0f},
		{1.0f, 1.0f, 0.0f, 1.0f, 0.0f},
	};

	void* buf_data = nullptr;
	if (m_vertex_buffer_->Lock(0, 0, &buf_data, 0) != D3D_OK)
		return;

	CopyMemory(buf_data, &rect, sizeof(rect));
	m_vertex_buffer_->Unlock();
}

void D3dRenderer::OnFrame(
	const webrtc::VideoFrame& video_frame) {
	// Do we need to Lock the renderframe call? since we have the device lock here
	// it seems no neccessary.
	if (video_frame.video_frame_buffer()->type() ==
		webrtc::VideoFrameBuffer::Type::kNative) {  // We're handling DXVA buffer
		NativeD3DSurfaceHandle* nativeHandle =
			reinterpret_cast<NativeD3DSurfaceHandle*>(
				reinterpret_cast<webrtc::NativeHandleBuffer*>(video_frame.video_frame_buffer().get())->native_handle());

		IDirect3DDeviceManager9* dev_manager = nativeHandle->dev_manager_;
		IDirect3DSurface9* surface = nativeHandle->surface_;

		IDirect3DDevice9* pDevice;
		HANDLE hDevice = 0;
		HRESULT hr = dev_manager->OpenDeviceHandle(&hDevice);
		if (FAILED(hr)) {
			RTC_LOG(LS_ERROR) << "Failed to open the d3d device handle";
			return;
		}

		// Our renderer does not need to lock the device.
		hr = dev_manager->LockDevice(hDevice, &pDevice, FALSE);
		if (FAILED(hr)) {
			RTC_LOG(LS_ERROR) << "Failed to lock device";
			dev_manager->CloseDeviceHandle(hDevice);
			return;
		}
		CComPtr<IDirect3DSurface9> back_buffer;
		hr = pDevice->GetBackBuffer(0, 0, D3DBACKBUFFER_TYPE_MONO, &back_buffer);
		if (FAILED(hr)) {
			RTC_LOG(LS_ERROR) << "Failed to get back buffer";
			return;
		}

		if (surface != nullptr) {
			hr = pDevice->StretchRect(surface, nullptr, back_buffer, nullptr,
				D3DTEXF_NONE);
			if (FAILED(hr)) {
				RTC_LOG(LS_ERROR) << "Failed to stretch the rectangle";
				return;
			}
		}

		hr = pDevice->Present(nullptr, nullptr, wnd_, nullptr);

		if (hr == D3DERR_DEVICELOST) {
			RTC_LOG(LS_WARNING) << "Device lost for present.";
		}
		dev_manager->UnlockDevice(hDevice, FALSE);
		dev_manager->CloseDeviceHandle(hDevice);

		// Done with the native handle.
		delete nativeHandle;
		nativeHandle = nullptr;
	}
	else {  // I420 frame passed.
		if (!inited_for_raw_) {
			m_d3d_ = Direct3DCreate9(D3D_SDK_VERSION);
			if (!m_d3d_)
				return;

			D3DPRESENT_PARAMETERS d3d_params = {};

			d3d_params.Windowed = true;
			d3d_params.SwapEffect = D3DSWAPEFFECT_COPY;

			IDirect3DDevice9* d3d_device;
			if (m_d3d_->CreateDevice(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, wnd_,
				D3DCREATE_SOFTWARE_VERTEXPROCESSING, &d3d_params,
				&d3d_device) != D3D_OK) {
				Destroy();
				return;
			}
			m_d3d_device_ = d3d_device;
			d3d_device->Release();

			IDirect3DVertexBuffer9* vertex_buffer;
			const int kRectVertices = 4;
			if (m_d3d_device_->CreateVertexBuffer(
				kRectVertices * sizeof(D3dCustomVertex), 0, D3DFVF_CUSTOMVERTEX,
				D3DPOOL_MANAGED, &vertex_buffer, nullptr) != D3D_OK) {
				Destroy();
				return;
			}
			m_vertex_buffer_ = vertex_buffer;
			vertex_buffer->Release();

			m_d3d_device_->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);
			m_d3d_device_->SetRenderState(D3DRS_LIGHTING, FALSE);
			Resize(video_frame.width(), video_frame.height());
			inited_for_raw_ = true;
		}
		else {
			HRESULT hr = m_d3d_device_->TestCooperativeLevel();
			if (FAILED(hr)) {
				if (hr == D3DERR_DEVICELOST) {
					RTC_LOG(LS_WARNING) << "Device lost.";
				}
				else if (hr == D3DERR_DEVICENOTRESET) {
					Destroy();
					RTC_LOG(LS_WARNING) << "Device try to reinit.";
				}
				else {
					RTC_LOG(LS_WARNING) << "Device driver internal error.";
				}

				return;
			}

			if (static_cast<size_t>(video_frame.width()) != width_ ||
				static_cast<size_t>(video_frame.height()) != height_) {
				Resize(static_cast<size_t>(video_frame.width()),
					static_cast<size_t>(video_frame.height()));
			}
			D3DLOCKED_RECT lock_rect;
			if (m_texture_->LockRect(0, &lock_rect, nullptr, 0) != D3D_OK)
				return;

			ConvertFromI420(video_frame, webrtc::VideoType::kARGB, 0,
				static_cast<uint8_t*>(lock_rect.pBits));
			m_texture_->UnlockRect(0);

			m_d3d_device_->BeginScene();
			m_d3d_device_->SetFVF(D3DFVF_CUSTOMVERTEX);
			m_d3d_device_->SetStreamSource(0, m_vertex_buffer_, 0,
				sizeof(D3dCustomVertex));
			m_d3d_device_->SetTexture(0, m_texture_);
			m_d3d_device_->DrawPrimitive(D3DPT_TRIANGLESTRIP, 0, 2);
			m_d3d_device_->EndScene();

			m_d3d_device_->Present(nullptr, nullptr, wnd_, nullptr);
		}
	}

	return;
}
}  // namespace webrtc

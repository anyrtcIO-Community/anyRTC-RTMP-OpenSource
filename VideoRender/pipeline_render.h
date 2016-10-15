//* ////////////////////////////////////////////////////////////////////////////// */
//*
//
//              INTEL CORPORATION PROPRIETARY INFORMATION
//  This software is supplied under the terms of a license  agreement or
//  nondisclosure agreement with Intel Corporation and may not be copied
//  or disclosed except in  accordance  with the terms of that agreement.
//        Copyright (c) 2005-2015 Intel Corporation. All Rights Reserved.
//
//
//*/

#ifndef __PIPELINE_RENDER_H__
#define __PIPELINE_RENDER_H__


#include "sample_defs.h"
#include "hw_device.h"

#ifdef D3D_SURFACES_SUPPORT
#pragma warning(disable : 4201)
#endif

#include "sample_utils.h"
#include "sample_params.h"
#include "base_allocator.h"

#include "mfxmvc.h"
#include "mfxvideo.h"
//#include "mfxvp8.h"
#include "mfxvideo++.h"
#include "mfxplugin.h"
#include "mfxplugin++.h"

#include <vector>
#include <memory>

#include "plugin_loader.h"

msdk_tick time_get_tick(void);
msdk_tick time_get_frequency(void);

enum {
    MVC_DISABLED          = 0x0,
    MVC_ENABLED           = 0x1,
    MVC_VIEWOUTPUT        = 0x2,    // 2 output bitstreams
};

enum MemType {
    SYSTEM_MEMORY = 0x00,
    D3D9_MEMORY   = 0x01,
    D3D11_MEMORY  = 0x02,
};


/* This class implements a pipeline with 2 mfx components: vpp (video preprocessing) and encode */
class CRendererPipeline
{
public:
	CRendererPipeline(void);
    virtual ~CRendererPipeline();

	virtual mfxStatus Init(mfxU16 nWidth, mfxU16 nHeight, MemType memType, HWND hParentWnd);
	virtual mfxStatus Run(unsigned char*pData, int nLen);
    virtual void Close();

	virtual mfxStatus ResetDevice();

	mfxStatus LoadNextFrame(mfxFrameSurface1* pSurface, unsigned char*pEnData, int nLen);

protected:
    MFXVideoSession m_mfxSession;

    MFXFrameAllocator* m_pMFXAllocator;
    mfxAllocatorParams* m_pmfxAllocatorParams;
    MemType m_memType;
	HWND	m_hParentWnd;

	mfxFrameSurface1* m_pEncSurfaces; // frames array for encoder input (vpp output)
	mfxFrameAllocResponse m_EncResponse;  // memory allocation response for encoder

	int					m_nWidth;
	int					m_nHeight;
	int					m_nY;
	int					m_nUV;
	bool				m_bUsedNV12;

    CHWDevice *			m_hwdev;

    virtual mfxStatus CreateAllocator();
    virtual void DeleteAllocator();

	mfxStatus RenderFrame(mfxFrameSurface1 *pSurface, mfxFrameAllocator *pmfxAlloc);
    virtual mfxStatus CreateHWDevice();
    virtual void DeleteHWDevice();
	virtual mfxStatus AllocFrames();
	virtual void DeleteFrames();

    virtual MFXVideoSession& GetFirstSession(){return m_mfxSession;}
};

#endif // __PIPELINE_ENCODE_H__

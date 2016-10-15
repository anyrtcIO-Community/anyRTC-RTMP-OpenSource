/*********************************************************************************

INTEL CORPORATION PROPRIETARY INFORMATION
This software is supplied under the terms of a license agreement or nondisclosure
agreement with Intel Corporation and may not be copied or disclosed except in
accordance with the terms of that agreement
Copyright(c) 2005-2015 Intel Corporation. All Rights Reserved.

**********************************************************************************/

#include "mfx_samples_config.h"

#include "pipeline_render.h"
#include "sysmem_allocator.h"

#if D3D_SURFACES_SUPPORT
#include "d3d_allocator.h"
#include "d3d11_allocator.h"

#include "d3d_device.h"
#include "d3d11_device.h"
#endif

#define USED_NV12 0

mfxStatus CRendererPipeline::RenderFrame(mfxFrameSurface1 *pSurface, mfxFrameAllocator *pmfxAlloc)
{
	if (m_hwdev==NULL)
	{
		return MFX_ERR_UNKNOWN;
	}

	RECT rect;
	GetClientRect(m_hParentWnd, &rect);
	if (IsRectEmpty(&rect))
		return MFX_ERR_UNKNOWN;

	mfxStatus sts = m_hwdev->RenderFrame(pSurface, pmfxAlloc);
	MSDK_CHECK_RESULT(sts, MFX_ERR_NONE, sts);

	return sts;
}

mfxStatus CRendererPipeline::CreateHWDevice()
{
    mfxStatus sts = MFX_ERR_NONE;
#if D3D_SURFACES_SUPPORT
    POINT point = {0, 0};
	HWND window = m_hParentWnd;// WindowFromPoint(point);

#if MFX_D3D11_SUPPORT
    if (D3D11_MEMORY == m_memType)
        m_hwdev = new CD3D11Device();
    else
#endif // #if MFX_D3D11_SUPPORT
        m_hwdev = new CD3D9Device();

    if (NULL == m_hwdev)
        return MFX_ERR_MEMORY_ALLOC;

    sts = m_hwdev->Init(
        window,
        1,
		MSDKAdapter::GetNumber(m_mfxSession)/* MSDKAdapter::GetNumber(GetFirstSession())*/);
    MSDK_CHECK_RESULT(sts, MFX_ERR_NONE, sts);

#elif LIBVA_SUPPORT
    m_hwdev = CreateVAAPIDevice();
    if (NULL == m_hwdev)
    {
        return MFX_ERR_MEMORY_ALLOC;
    }
    sts = m_hwdev->Init(NULL, 0, MSDKAdapter::GetNumber(GetFirstSession()));
    MSDK_CHECK_RESULT(sts, MFX_ERR_NONE, sts);
#endif
    return MFX_ERR_NONE;
}

mfxStatus CRendererPipeline::ResetDevice()
{
    if (D3D9_MEMORY == m_memType || D3D11_MEMORY == m_memType)
    {
        return m_hwdev->Reset();
    }
    return MFX_ERR_NONE;
}


mfxStatus CRendererPipeline::AllocFrames()
{
	mfxStatus sts = MFX_ERR_NONE;
	mfxFrameAllocRequest EncRequest;

	mfxU16 nEncSurfNum = 0; // number of surfaces for encoder

	MSDK_ZERO_MEMORY(EncRequest);

	MFXVideoENCODE*pmfxENC = new MFXVideoENCODE(m_mfxSession);

	mfxVideoParam mfxEncParams;
	MSDK_ZERO_MEMORY(mfxEncParams);
	mfxEncParams.mfx.CodecId = MFX_CODEC_AVC;
	mfxEncParams.mfx.TargetUsage = MFX_TARGETUSAGE_BALANCED;
	mfxEncParams.mfx.TargetKbps = 1024; // in Kbps
	mfxEncParams.mfx.RateControlMethod = MFX_RATECONTROL_CBR;
	mfxEncParams.mfx.NumSlice = 0;
	ConvertFrameRate(60, &mfxEncParams.mfx.FrameInfo.FrameRateExtN, &mfxEncParams.mfx.FrameInfo.FrameRateExtD);
	mfxEncParams.mfx.EncodedOrder = 0; // binary flag, 0 signals encoder to take frames in display order
	mfxEncParams.IOPattern = MFX_IOPATTERN_IN_VIDEO_MEMORY;

	// frame info parameters
	mfxEncParams.mfx.FrameInfo.FourCC = (m_bUsedNV12 ? MFX_FOURCC_NV12 : MFX_FOURCC_YV12);
	mfxEncParams.mfx.FrameInfo.ChromaFormat = MFX_CHROMAFORMAT_YUV420;
	mfxEncParams.mfx.FrameInfo.PicStruct = MFX_PICSTRUCT_PROGRESSIVE;

	// set frame size and crops
	// width must be a multiple of 16
	// height must be a multiple of 16 in case of frame picture and a multiple of 32 in case of field picture
	mfxEncParams.mfx.FrameInfo.Width = (m_nWidth);
	mfxEncParams.mfx.FrameInfo.Height = (m_nHeight);

	mfxEncParams.mfx.FrameInfo.CropX = 0;
	mfxEncParams.mfx.FrameInfo.CropY = 0;
	mfxEncParams.mfx.FrameInfo.CropW = (m_nWidth);
	mfxEncParams.mfx.FrameInfo.CropH = (m_nHeight);

	//mfxEncParams.mfx.GopRefDist = 1;
	//mfxEncParams.mfx.GopPicSize = m_mfxEncParams.mfx.GopPicSize;

	mfxEncParams.AsyncDepth = 1;
	sts = pmfxENC->QueryIOSurf(&mfxEncParams, &EncRequest);
	MSDK_SAFE_DELETE(pmfxENC);

	// Calculate the number of surfaces for components.
	// QueryIOSurf functions tell how many surfaces are required to produce at least 1 output.
	// To achieve better performance we provide extra surfaces.
	// 1 extra surface at input allows to get 1 extra output.

	if (EncRequest.NumFrameSuggested < mfxEncParams.AsyncDepth)
		return MFX_ERR_MEMORY_ALLOC;

	// The number of surfaces shared by vpp output and encode input.
	nEncSurfNum = EncRequest.NumFrameSuggested;

	// prepare allocation requests
	EncRequest.NumFrameSuggested = EncRequest.NumFrameMin = nEncSurfNum;
	MSDK_MEMCPY_VAR(EncRequest.Info, &(mfxEncParams.mfx.FrameInfo), sizeof(mfxFrameInfo));
	// alloc frames for encoder
	sts = m_pMFXAllocator->Alloc(m_pMFXAllocator->pthis, &EncRequest, &m_EncResponse);
	MSDK_CHECK_RESULT(sts, MFX_ERR_NONE, sts);

	// prepare mfxFrameSurface1 array for encoder
	m_pEncSurfaces = new mfxFrameSurface1[m_EncResponse.NumFrameActual];
	MSDK_CHECK_POINTER(m_pEncSurfaces, MFX_ERR_MEMORY_ALLOC);

	for (int i = 0; i < m_EncResponse.NumFrameActual; i++)
	{
		memset(&(m_pEncSurfaces[i]), 0, sizeof(mfxFrameSurface1));
		MSDK_MEMCPY_VAR(m_pEncSurfaces[i].Info, &(mfxEncParams.mfx.FrameInfo), sizeof(mfxFrameInfo));

		m_pEncSurfaces[i].Data.MemId = m_EncResponse.mids[i];
	}

	return MFX_ERR_NONE;
}


mfxStatus CRendererPipeline::CreateAllocator()
{
    mfxStatus sts = MFX_ERR_NONE;

	if (D3D11_MEMORY == m_memType)
	{
       sts = CreateHWDevice();
        MSDK_CHECK_RESULT(sts, MFX_ERR_NONE, sts);

		// create system memory allocator
		m_pMFXAllocator = new SysMemFrameAllocator;
		MSDK_CHECK_POINTER(m_pMFXAllocator, MFX_ERR_MEMORY_ALLOC);

		// initialize memory allocator
		sts = m_pMFXAllocator->Init(m_pmfxAllocatorParams);
		MSDK_CHECK_RESULT(sts, MFX_ERR_NONE, sts);
		return sts;
	}

    if (D3D9_MEMORY == m_memType || D3D11_MEMORY == m_memType)
    {
#if D3D_SURFACES_SUPPORT
        sts = CreateHWDevice();
        MSDK_CHECK_RESULT(sts, MFX_ERR_NONE, sts);

        mfxHDL hdl = NULL;
        mfxHandleType hdl_t =
        #if MFX_D3D11_SUPPORT
            D3D11_MEMORY == m_memType ? MFX_HANDLE_D3D11_DEVICE :
        #endif // #if MFX_D3D11_SUPPORT
            MFX_HANDLE_D3D9_DEVICE_MANAGER;

        sts = m_hwdev->GetHandle(hdl_t, &hdl);
        MSDK_CHECK_RESULT(sts, MFX_ERR_NONE, sts);

        // handle is needed for HW library only
        mfxIMPL impl = 0;
        m_mfxSession.QueryIMPL(&impl);
        if (impl != MFX_IMPL_SOFTWARE)
        {
            sts = m_mfxSession.SetHandle(hdl_t, hdl);
            MSDK_CHECK_RESULT(sts, MFX_ERR_NONE, sts);
        }

        // create D3D allocator
#if MFX_D3D11_SUPPORT
        if (D3D11_MEMORY == m_memType)
        {
            m_pMFXAllocator = new D3D11FrameAllocator;
            MSDK_CHECK_POINTER(m_pMFXAllocator, MFX_ERR_MEMORY_ALLOC);

            D3D11AllocatorParams *pd3dAllocParams = new D3D11AllocatorParams;
            MSDK_CHECK_POINTER(pd3dAllocParams, MFX_ERR_MEMORY_ALLOC);
            pd3dAllocParams->pDevice = reinterpret_cast<ID3D11Device *>(hdl);

            m_pmfxAllocatorParams = pd3dAllocParams;
        }
        else
#endif // #if MFX_D3D11_SUPPORT
        {
            m_pMFXAllocator = new D3DFrameAllocator;
            MSDK_CHECK_POINTER(m_pMFXAllocator, MFX_ERR_MEMORY_ALLOC);

            D3DAllocatorParams *pd3dAllocParams = new D3DAllocatorParams;
            MSDK_CHECK_POINTER(pd3dAllocParams, MFX_ERR_MEMORY_ALLOC);
            pd3dAllocParams->pManager = reinterpret_cast<IDirect3DDeviceManager9 *>(hdl);

            m_pmfxAllocatorParams = pd3dAllocParams;
        }

        /* In case of video memory we must provide MediaSDK with external allocator
        thus we demonstrate "external allocator" usage model.
        Call SetAllocator to pass allocator to Media SDK */
        sts = m_mfxSession.SetFrameAllocator(m_pMFXAllocator);
        MSDK_CHECK_RESULT(sts, MFX_ERR_NONE, sts);

#endif
#ifdef LIBVA_SUPPORT
        sts = CreateHWDevice();
        MSDK_CHECK_RESULT(sts, MFX_ERR_NONE, sts);
        /* It's possible to skip failed result here and switch to SW implementation,
        but we don't process this way */

        mfxHDL hdl = NULL;
        sts = m_hwdev->GetHandle(MFX_HANDLE_VA_DISPLAY, &hdl);
        // provide device manager to MediaSDK
        sts = m_mfxSession.SetHandle(MFX_HANDLE_VA_DISPLAY, hdl);
        MSDK_CHECK_RESULT(sts, MFX_ERR_NONE, sts);

        // create VAAPI allocator
        m_pMFXAllocator = new vaapiFrameAllocator;
        MSDK_CHECK_POINTER(m_pMFXAllocator, MFX_ERR_MEMORY_ALLOC);

        vaapiAllocatorParams *p_vaapiAllocParams = new vaapiAllocatorParams;
        MSDK_CHECK_POINTER(p_vaapiAllocParams, MFX_ERR_MEMORY_ALLOC);

        p_vaapiAllocParams->m_dpy = (VADisplay)hdl;
        m_pmfxAllocatorParams = p_vaapiAllocParams;

        /* In case of video memory we must provide MediaSDK with external allocator
        thus we demonstrate "external allocator" usage model.
        Call SetAllocator to pass allocator to mediasdk */
        sts = m_mfxSession.SetFrameAllocator(m_pMFXAllocator);
        MSDK_CHECK_RESULT(sts, MFX_ERR_NONE, sts);

#endif
    }
    else
    {
#ifdef LIBVA_SUPPORT
        //in case of system memory allocator we also have to pass MFX_HANDLE_VA_DISPLAY to HW library
        mfxIMPL impl;
        m_mfxSession.QueryIMPL(&impl);

        if(MFX_IMPL_HARDWARE == MFX_IMPL_BASETYPE(impl))
        {
            sts = CreateHWDevice();
            MSDK_CHECK_RESULT(sts, MFX_ERR_NONE, sts);

            mfxHDL hdl = NULL;
            sts = m_hwdev->GetHandle(MFX_HANDLE_VA_DISPLAY, &hdl);
            // provide device manager to MediaSDK
            sts = m_mfxSession.SetHandle(MFX_HANDLE_VA_DISPLAY, hdl);
            MSDK_CHECK_RESULT(sts, MFX_ERR_NONE, sts);
        }
#endif

        // create system memory allocator
        m_pMFXAllocator = new SysMemFrameAllocator;
        MSDK_CHECK_POINTER(m_pMFXAllocator, MFX_ERR_MEMORY_ALLOC);

        /* In case of system memory we demonstrate "no external allocator" usage model.
        We don't call SetAllocator, Media SDK uses internal allocator.
        We use system memory allocator simply as a memory manager for application*/
    }

    // initialize memory allocator
    sts = m_pMFXAllocator->Init(m_pmfxAllocatorParams);
    MSDK_CHECK_RESULT(sts, MFX_ERR_NONE, sts);

    return MFX_ERR_NONE;
}


void CRendererPipeline::DeleteFrames()
{
	// delete surfaces array
	MSDK_SAFE_DELETE_ARRAY(m_pEncSurfaces);

	// delete frames
	if (m_pMFXAllocator)
	{
		m_pMFXAllocator->Free(m_pMFXAllocator->pthis, &m_EncResponse);
	}
}

void CRendererPipeline::DeleteHWDevice()
{
    MSDK_SAFE_DELETE(m_hwdev);
}

void CRendererPipeline::DeleteAllocator()
{
    // delete allocator
    MSDK_SAFE_DELETE(m_pMFXAllocator);
    MSDK_SAFE_DELETE(m_pmfxAllocatorParams);

    DeleteHWDevice();
}

CRendererPipeline::CRendererPipeline(void)
{
    m_pMFXAllocator = NULL;
    m_pmfxAllocatorParams = NULL;
    m_memType = SYSTEM_MEMORY;
	m_hParentWnd = NULL;

	m_nWidth = 0;
	m_nWidth = 0;
	m_nY = m_nWidth*m_nWidth;
	m_nUV = (m_nY / 4);

	m_bUsedNV12 = false;

	m_pEncSurfaces = NULL;
	MSDK_ZERO_MEMORY(m_EncResponse);

#if D3D_SURFACES_SUPPORT
    m_hwdev = NULL;
#endif
}

CRendererPipeline::~CRendererPipeline()
{
}

mfxStatus CRendererPipeline::Init(mfxU16 nWidth, mfxU16 nHeight, MemType memType, HWND hParentWnd)
{
    mfxStatus sts = MFX_ERR_NONE;

    mfxVersion min_version;
    mfxVersion version;     // real API version with which library is initialized

    // we set version to 1.0 and later we will query actual version of the library which will got leaded
    min_version.Major = 1;
    min_version.Minor = 0;

    //sts = m_mfxSession.Init(MFX_IMPL_SOFTWARE, &min_version);
    //sts = m_mfxSession.Init(MFX_IMPL_HARDWARE, &min_version);
    sts = m_mfxSession.Init(MFX_IMPL_RUNTIME, &min_version);

    MSDK_CHECK_RESULT(sts, MFX_ERR_NONE, sts);

    sts = MFXQueryVersion(m_mfxSession , &version); // get real API version of the loaded library
    MSDK_CHECK_RESULT(sts, MFX_ERR_NONE, sts);

    // set memory type
	m_nHeight = nHeight;
	m_nWidth = nWidth;
	m_memType = memType;
	m_hParentWnd=hParentWnd;

	m_nY = m_nWidth*m_nHeight;
	m_nUV = (m_nY / 4);

    // create and init frame allocator
    sts = CreateAllocator();
    MSDK_CHECK_RESULT(sts, MFX_ERR_NONE, sts);

	sts = AllocFrames();
	MSDK_CHECK_RESULT(sts, MFX_ERR_NONE, sts);


    return MFX_ERR_NONE;
}

void CRendererPipeline::Close()
{
	DeleteFrames();

    m_mfxSession.Close();
	
    // allocator if used as external for MediaSDK must be deleted after SDK components
    DeleteAllocator();
}


mfxStatus CRendererPipeline::Run(unsigned char*pData, int nLen)
{
	mfxStatus sts = MFX_ERR_NONE;
	mfxFrameSurface1* pSurf = NULL; // dispatching pointer
	mfxU16 nEncSurfIdx = 0;     // index of free surface for encoder input (vpp output)
	sts = MFX_ERR_NONE;

	nEncSurfIdx = GetFreeSurface(m_pEncSurfaces, m_EncResponse.NumFrameActual);
	MSDK_CHECK_ERROR(nEncSurfIdx, MSDK_INVALID_SURF_IDX, MFX_ERR_MEMORY_ALLOC);

	// point pSurf to encoder surface
	pSurf = &m_pEncSurfaces[nEncSurfIdx];


	{
		// get YUV pointers
		sts = m_pMFXAllocator->Lock(m_pMFXAllocator->pthis, pSurf->Data.MemId, &(pSurf->Data));
		MSDK_CHECK_RESULT(sts, MFX_ERR_NONE, sts);
	}

	pSurf->Info.FrameId.ViewId = 0;
	sts = LoadNextFrame(pSurf, pData, nLen);
	MSDK_CHECK_RESULT(sts, MFX_ERR_NONE, sts);

	// ... after we're done call Unlock
	{
		sts = m_pMFXAllocator->Unlock(m_pMFXAllocator->pthis, pSurf->Data.MemId, &(pSurf->Data));
		MSDK_CHECK_RESULT(sts, MFX_ERR_NONE, sts);
	}

	if (D3D11_MEMORY != m_memType)
	{
		RenderFrame(pSurf, m_pMFXAllocator);
	}

	return sts;
}

mfxStatus CRendererPipeline::LoadNextFrame(mfxFrameSurface1* pSurface, unsigned char*pData, int nLen)
{
	MSDK_CHECK_POINTER(pSurface, MFX_ERR_NULL_PTR);

	mfxFrameInfo* pInfo = &pSurface->Info;
	mfxFrameData* pFrameData = &pSurface->Data;
	UINT32 nPitch = pFrameData->Pitch;

	UINT8*pDstU = pFrameData->U;
	UINT8*pDstV = pFrameData->V;

	unsigned char*pSrcU = pData + m_nY;
	unsigned char*pSrcV = pSrcU + m_nUV;

	int nSrcStrideUV = m_nWidth >> 1;

	UINT8*pDstY = pFrameData->Y;
	UINT8*pSrcY = pData;

	int nHeight = m_nHeight >> 1;

	if (m_bUsedNV12)
	{
		for (int i = 0; i < nHeight; i++)
		{
			memcpy(pDstY, pSrcY, m_nWidth);
			pSrcY += m_nWidth;
			pDstY += nPitch;

			memcpy(pDstY, pSrcY, m_nWidth);
			pSrcY += m_nWidth;
			pDstY += nPitch;


			for (int j = 0; j < nSrcStrideUV; j++)
			{
				pDstU[j << 1] = pSrcU[j];
				pDstV[j << 1] = pSrcV[j];
			}
			pDstU += nPitch;;
			pDstV += nPitch;;


			pSrcU += nSrcStrideUV;
			pSrcV += nSrcStrideUV;
		}
	}
	else
	{
		for (int i = 0; i < nHeight; i++)
		{
			memcpy(pDstY, pSrcY, m_nWidth);
			pSrcY += m_nWidth;
			pDstY += nPitch;

			memcpy(pDstY, pSrcY, m_nWidth);
			pSrcY += m_nWidth;
			pDstY += nPitch;


			memcpy(pDstU, pSrcU, (m_nWidth >> 1));
			memcpy(pDstV, pSrcV, (m_nWidth >> 1));

			pDstU += (nPitch >> 1);
			pDstV += (nPitch >> 1);

			pSrcU += nSrcStrideUV;
			pSrcV += nSrcStrideUV;
		}
	}


	return MFX_ERR_NONE;
}

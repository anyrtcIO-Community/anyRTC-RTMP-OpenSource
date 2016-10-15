
#include "VideoRenderer.h"
#include "mfx_samples_config.h"
#include "d3d_renderer.h"


namespace webrtc {
namespace test {
#if 0
	VideoRenderer* VideoRenderer::CreatePlatformRenderer(const void* hwnd,size_t width,	size_t height)
	{
		D3DVideoRenderer*pD3DVideoRenderer = new D3DVideoRenderer();
		if (0 != pD3DVideoRenderer->Create(hwnd, width, height, 60))
		{
			delete pD3DVideoRenderer;
			pD3DVideoRenderer = NULL;
		}
		if (pD3DVideoRenderer == NULL) {
			return D3dRenderer::Create(hwnd, width, height);
		}
		return pD3DVideoRenderer;
	}
#endif
//---------------------------------------------------------------------------------------
//构造函数
D3DVideoRenderer::D3DVideoRenderer(void)
	: m_pEncodingPipeline(NULL)
	, m_hParentWnd(NULL)
	, m_nWidth(0)
	, m_nHeight(0)
{
}

//---------------------------------------------------------------------------------------
//析构函数
D3DVideoRenderer::~D3DVideoRenderer(void)
{
	Destroy();
}

int D3DVideoRenderer::Create(const void* hParentWnd/*HWND*/, int nWidth, int nHeight, int nFrameRate)
{
	mfxStatus sts = MFX_ERR_NONE; // return value check   
	m_pEncodingPipeline = new CRendererPipeline();
	if (m_pEncodingPipeline)
	{
		sts = m_pEncodingPipeline->Init((mfxU16)nWidth, (mfxU16)nHeight, D3D9_MEMORY, (HWND)hParentWnd);
	}
	if (sts != MFX_ERR_NONE)
		return -1;

	m_nWidth = nWidth;
	m_nHeight = nHeight;
	m_hParentWnd = hParentWnd;

	return 0;
}

//关闭
void D3DVideoRenderer::Destroy(void)
{
	CRendererPipeline* pEncodingPipeline = m_pEncodingPipeline;
	m_pEncodingPipeline = NULL;
	if (pEncodingPipeline)
	{
		pEncodingPipeline->Close();
		delete pEncodingPipeline;
		pEncodingPipeline = NULL;
	}
}

//编码&显示
int	D3DVideoRenderer::Render(unsigned char*pData, int nLen)
{
	if (m_pEncodingPipeline)
	{
		return m_pEncodingPipeline->Run(pData, nLen);
	}
	return -1;
}

void D3DVideoRenderer::OnFrame(const cricket::VideoFrame& frame)
{
	const cricket::VideoFrame*videoFrame = &frame;
	if (static_cast<size_t>(videoFrame->width()) != m_nWidth ||
		static_cast<size_t>(videoFrame->height()) != m_nHeight ||
		m_pEncodingPipeline==NULL)
	{
		mfxStatus sts = MFX_ERR_NONE; // return value check   
		int nWidth = videoFrame->width();
		int nHeight = videoFrame->height();

		CRendererPipeline* pEncodingPipeline = m_pEncodingPipeline;
		m_pEncodingPipeline = NULL;
		if (pEncodingPipeline)
		{
			pEncodingPipeline->Close();
			delete pEncodingPipeline;
			pEncodingPipeline = NULL;
		}

		m_pEncodingPipeline = new CRendererPipeline();
		if (m_pEncodingPipeline)
		{
			sts = m_pEncodingPipeline->Init((mfxU16)nWidth, (mfxU16)nHeight, D3D9_MEMORY, (HWND)m_hParentWnd);
		}
		if (sts != MFX_ERR_NONE)
			return ;

		m_nWidth = nWidth;
		m_nHeight = nHeight;
	}

	Render((uint8_t*)(videoFrame->video_frame_buffer()->DataY()), 0);
}

}  // namespace test
}  // namespace webrtc
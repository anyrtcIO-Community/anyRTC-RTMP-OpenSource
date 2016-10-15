#pragma once

#include "video_renderer.h"
#include "webrtc/base/scoped_ref_ptr.h"
#include "webrtc/typedefs.h"
#include "pipeline_render.h"

namespace webrtc {
namespace test {
//---------------------------------------------------------------------------------------
// TCP流类定义
class  D3DVideoRenderer : public VideoRenderer
{
public:
	D3DVideoRenderer(void);
	virtual ~D3DVideoRenderer(void);

	virtual int Create(const void* hParentWnd/*HWND*/, int nWidth, int nHeight, int nFrameRate);
	//关闭
	virtual void Destroy(void);
	//编码&显示
	virtual int	Render(unsigned char*pData, int nLen);

	void OnFrame(const cricket::VideoFrame& frame) override;
private:
	CRendererPipeline*		m_pEncodingPipeline;
	int						m_nWidth;
	int						m_nHeight;
	const void*				m_hParentWnd;
};
}  // namespace test
}  // namespace webrtc

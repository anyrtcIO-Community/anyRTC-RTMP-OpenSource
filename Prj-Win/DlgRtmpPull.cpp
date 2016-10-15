/*
*  Copyright (c) 2016 The AnyRTC project authors. All Rights Reserved.
*
*  Please visit https://www.anyrtc.io for detail.
*
* The GNU General Public License is a free, copyleft license for
* software and other kinds of works.
*
* The licenses for most software and other practical works are designed
* to take away your freedom to share and change the works.  By contrast,
* the GNU General Public License is intended to guarantee your freedom to
* share and change all versions of a program--to make sure it remains free
* software for all its users.  We, the Free Software Foundation, use the
* GNU General Public License for most of our software; it applies also to
* any other work released this way by its authors.  You can apply it to
* your programs, too.
* See the GNU LICENSE file for more info.
*/
#include "stdafx.h"
#include "DlgRtmpPull.h"
// DlgRtmpPull 对话框

IMPLEMENT_DYNAMIC(DlgRtmpPull, CDialog)

DlgRtmpPull::DlgRtmpPull()
	: CDialog(DlgRtmpPull::IDD)
	, m_strUrl(_T("rtmp://live.hkstv.hk.lxdns.com/live/hks"))
	, m_pAVRtmplayer(NULL)
	, m_pDlgVideoMain(NULL)
{
}

DlgRtmpPull::~DlgRtmpPull()
{
}

void DlgRtmpPull::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_EDIT_URL, m_editUrl);
	DDX_Control(pDX, IDC_BTN_PULL, m_btnRtmp);
	DDX_Text(pDX, IDC_EDIT_URL, m_strUrl);
	DDX_Control(pDX, IDC_STATIC_CAPTRUE, m_staticCaptrue);
}


BEGIN_MESSAGE_MAP(DlgRtmpPull, CDialog)
	ON_WM_CLOSE()
	ON_WM_ERASEBKGND()
	ON_WM_LBUTTONDBLCLK()
	ON_MESSAGE(WM_MY_PULL_MESSAGE, OnMyMessage)
	ON_BN_CLICKED(IDC_BTN_PULL, &DlgRtmpPull::OnBnClickedBtnPull)
END_MESSAGE_MAP()


// DlgRtmpPull 消息处理程序

void DlgRtmpPull::OnOK()
{
}

void DlgRtmpPull::OnCancel()
{
	CDialog::EndDialog(0);
}

void DlgRtmpPull::OnClose()
{
	CDialog::EndDialog(0);
}

BOOL DlgRtmpPull::OnInitDialog()
{
	CDialog::OnInitDialog();

	{// Video player
		m_pDlgVideoMain = new DlgVideo(this);
		m_pDlgVideoMain->Create(DlgVideo::IDD, this);
		CRect rc;
		m_staticCaptrue.GetWindowRect(rc);
		m_staticCaptrue.ShowWindow(SW_HIDE);
		ScreenToClient(rc);
		m_pDlgVideoMain->SetWindowPos(NULL, rc.left, rc.top, rc.Width(), rc.Height(), SWP_SHOWWINDOW);
	}

	return TRUE;  // return TRUE unless you set the focus to a control
	// 异常: OCX 属性页应返回 FALSE
}

BOOL DlgRtmpPull::DestroyWindow()
{
	if (m_pAVRtmplayer) {
		m_pAVRtmplayer->StopRtmpPlay();
		RTMPGuester::Destory(m_pAVRtmplayer);
		m_pAVRtmplayer = NULL;
	}
	if (m_pDlgVideoMain) {
		m_pDlgVideoMain->DestroyWindow();
		delete m_pDlgVideoMain;
		m_pDlgVideoMain = NULL;
	}

	return CDialog::DestroyWindow();
}

BOOL DlgRtmpPull::OnEraseBkgnd(CDC* pDC)
{
	return FALSE;
}

LRESULT DlgRtmpPull::OnMyMessage(WPARAM wParam, LPARAM lParam)
{
	CString *pstrGet = (CString *)lParam;
	char ss[128];
	memset(ss, 0, 128);
	int fnlen = pstrGet->GetLength();
	for (int i = 0; i <= fnlen; i++) {
		ss[i] = pstrGet->GetAt(i);
	}
	DlgVideo* ptrDlg = NULL;
	delete pstrGet;
	return 0;
}

void DlgRtmpPull::OnLButtonDblClk(UINT nFlags, CPoint point)
{
	CDialog::OnLButtonDblClk(nFlags, point);
}

void DlgRtmpPull::OnBnClickedBtnPull()
{
	// TODO:  在此添加控件通知处理程序代码
	if (m_pAVRtmplayer == NULL) {
		m_pAVRtmplayer = RTMPGuester::Create(*this);
		UpdateData(TRUE);
		char ss[128];
		memset(ss, 0, 128);
		int fnlen = m_strUrl.GetLength();
		for (int i = 0; i <= fnlen; i++) {
			ss[i] = m_strUrl.GetAt(i);
		}
		m_pAVRtmplayer->StartRtmpPlay(ss, m_pDlgVideoMain->m_hWnd);
		m_btnRtmp.SetWindowTextW(L"结束");
	}
	else {
		m_btnRtmp.SetWindowTextW(L"拉流");
		m_pAVRtmplayer->StopRtmpPlay();
		RTMPGuester::Destory(m_pAVRtmplayer);
		m_pAVRtmplayer = NULL;
	}
}

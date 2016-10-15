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
#pragma once
#include "Resource.h"
#include "RtmpGuester.h"
#include "DlgVideo.h"
#define WM_MY_PULL_MESSAGE (WM_USER + 102)

// DlgVideo 对话框
class CLiveWin32Dlg;
class DlgRtmpPull : public CDialog, public RTMPGuesterEvent
{
	DECLARE_DYNAMIC(DlgRtmpPull)

public:
	DlgRtmpPull();   // 标准构造函数
	virtual ~DlgRtmpPull();

// 对话框数据
	enum { IDD = IDD_DIALOG_PULL };

public:
	//* For RTMPCGuesterEvent
	virtual void OnRtmplayerOK() {};
	virtual void OnRtmplayerStatus(int cacheTime, int curBitrate) {};
	virtual void OnRtmplayerCache(int time) {};
	virtual void OnRtmplayerClosed(int errcode) {};


protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
	virtual void OnOK();
	virtual void OnCancel();
public:
	afx_msg void OnClose();
	virtual BOOL OnInitDialog();
	virtual BOOL DestroyWindow();
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnLButtonDblClk(UINT nFlags, CPoint point);
	afx_msg LRESULT OnMyMessage(WPARAM wParam, LPARAM lParam);

	CEdit	m_editUrl;
	CButton m_btnRtmp;
	CString m_strUrl;
	CStatic m_staticCaptrue;

	afx_msg void OnBnClickedBtnPull();

private:
	DlgVideo		*m_pDlgVideoMain;
	RTMPGuester		*m_pAVRtmplayer;
	
};

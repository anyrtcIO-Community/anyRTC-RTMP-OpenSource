/*
CFAAC - set of classes to import/export .aac/.mp4 files
Copyright (C) 2004 Antonio Foranna

This program is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation.
	
This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.
		
You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software
Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
			
The author can be contacted at:
ntnfrn_email-temp@yahoo.it
*/

#include <windows.h>
#include "resource.h"
#include <Defines.h>	// my defines
#include "Cfaad.h"
#include "DecDialog.h"
#include "EncDialog.h"

// *********************************************************************************************

extern HINSTANCE hInstance;
extern HBITMAP hBmBrowse;

// -----------------------------------------------------------------------------------------------

#ifndef FAAD_FMT_64BIT
#define FAAD_FMT_64BIT 5
#endif

// *********************************************************************************************

int ShowDlg4RawAAC()
{
	return DialogBoxParam((HINSTANCE)hInstance,(LPCSTR)MAKEINTRESOURCE(IDD_DECODER),(HWND)NULL, (DLGPROC)DialogMsgProcDec, 0);
//	return DialogBoxParam((HINSTANCE)hInstance,(LPCSTR)MAKEINTRESOURCE(IDD_DECODER),(HWND)hWnd, (DLGPROC)DialogMsgProcDec, (DWORD)&cfg);
}

// *********************************************************************************************

#define INIT_CB(hWnd,nID,list,IdSelected) \
{ \
	for(int i=0; list[i]; i++) \
		SendMessage(GetDlgItem(hWnd, nID), CB_ADDSTRING, 0, (LPARAM)list[i]); \
	SendMessage(GetDlgItem(hWnd, nID), CB_SETCURSEL, IdSelected, 0); \
}
// -----------------------------------------------------------------------------------------------

//	EnableWindow(GetDlgItem(hWndDlg, IDC_RADIO_SSR), Enabled);
#define DISABLE_CTRLS_DEC(Enabled) \
{ \
	EnableWindow(GetDlgItem(hWndDlg, IDC_RADIO_MAIN), Enabled); \
	EnableWindow(GetDlgItem(hWndDlg, IDC_RADIO_LOW), Enabled); \
	EnableWindow(GetDlgItem(hWndDlg, IDC_RADIO_LTP), Enabled); \
	EnableWindow(GetDlgItem(hWndDlg, IDC_CHK_DOWNMATRIX), Enabled); \
	EnableWindow(GetDlgItem(hWndDlg, IDC_CHK_OLDADTS), Enabled); \
	EnableWindow(GetDlgItem(hWndDlg, IDC_CB_SAMPLERATE), Enabled); \
	EnableWindow(GetDlgItem(hWndDlg, IDC_CB_BITSPERSAMPLE), Enabled); \
}
// -----------------------------------------------------------------------------------------------

BOOL DialogMsgProcDec(HWND hWndDlg, UINT Message, WPARAM wParam, LPARAM lParam)
{
	switch(Message)
	{
	case WM_INITDIALOG:
		{
/*			if(!lParam)
			{
				MessageBox(hWndDlg,"Pointer==NULL",0,MB_OK|MB_ICONSTOP);
				EndDialog(hWndDlg, 0);
				return TRUE;
			}
*/
		char buf[50];
		char	*SampleRate[]={"6000","8000","16000","22050","32000","44100","48000","64000","88200","96000","192000",0},
				*BitsPerSample[]={"16","24","32","32 bit FLOAT","64 bit FLOAT",0};
		CMyDecCfg cfg(false);

			SetWindowPos(GetDlgItem(hWndDlg,IDC_CHK_DEFAULTCFG),GetDlgItem(hWndDlg,IDC_GRP_DEFAULT),0,0,0,0,SWP_NOMOVE | SWP_NOSIZE);

			INIT_CB(hWndDlg,IDC_CB_BITSPERSAMPLE,BitsPerSample,0);
			INIT_CB(hWndDlg,IDC_CB_SAMPLERATE,SampleRate,5);
			sprintf(buf,"%lu",cfg.DecCfg.defSampleRate);
			SetDlgItemText(hWndDlg, IDC_CB_SAMPLERATE, buf);

			switch(cfg.DecCfg.defObjectType)
			{
			case MAIN:
				CheckDlgButton(hWndDlg,IDC_RADIO_MAIN,TRUE);
				break;
			case LC:
				CheckDlgButton(hWndDlg,IDC_RADIO_LOW,TRUE);
				break;
			case SSR:
				CheckDlgButton(hWndDlg,IDC_RADIO_SSR,TRUE);
				break;
			case LTP:
				CheckDlgButton(hWndDlg,IDC_RADIO_LTP,TRUE);
				break;
			case HE_AAC:
				CheckDlgButton(hWndDlg,IDC_RADIO_HE,TRUE);
				break;
			}

			switch(cfg.DecCfg.outputFormat)
			{
			case FAAD_FMT_16BIT:
				SendMessage(GetDlgItem(hWndDlg, IDC_CB_BITSPERSAMPLE), CB_SETCURSEL, 0, 0);
				break;
			case FAAD_FMT_24BIT:
				SendMessage(GetDlgItem(hWndDlg, IDC_CB_BITSPERSAMPLE), CB_SETCURSEL, 1, 0);
				break;
			case FAAD_FMT_32BIT:
				SendMessage(GetDlgItem(hWndDlg, IDC_CB_BITSPERSAMPLE), CB_SETCURSEL, 2, 0);
				break;
			case FAAD_FMT_FLOAT:
				SendMessage(GetDlgItem(hWndDlg, IDC_CB_BITSPERSAMPLE), CB_SETCURSEL, 3, 0);
				break;
			case FAAD_FMT_64BIT:
				SendMessage(GetDlgItem(hWndDlg, IDC_CB_BITSPERSAMPLE), CB_SETCURSEL, 4, 0);
				break;
			}

			CheckDlgButton(hWndDlg,IDC_CHK_DOWNMATRIX, cfg.DecCfg.downMatrix);
			CheckDlgButton(hWndDlg,IDC_CHK_OLDADTS, cfg.DecCfg.useOldADTSFormat);

			CheckDlgButton(hWndDlg,IDC_CHK_DEFAULTCFG, cfg.DefaultCfg);
			DISABLE_CTRLS_DEC(!cfg.DefaultCfg);
		}
		break; // End of WM_INITDIALOG                                 

	case WM_CLOSE:
		// Closing the Dialog behaves the same as Cancel               
		PostMessage(hWndDlg, WM_COMMAND, IDCANCEL, 0);
		break; // End of WM_CLOSE                                      

	case WM_COMMAND:
		switch(LOWORD(wParam))
		{
		case IDC_CHK_DEFAULTCFG:
			{
			char Enabled=!IsDlgButtonChecked(hWndDlg,IDC_CHK_DEFAULTCFG);
				DISABLE_CTRLS_DEC(Enabled);
			}
			break;

		case IDOK:
			{
		CMyDecCfg cfg;
				if(IsDlgButtonChecked(hWndDlg,IDC_RADIO_MAIN))
					cfg.DecCfg.defObjectType=MAIN;
				if(IsDlgButtonChecked(hWndDlg,IDC_RADIO_LOW))
					cfg.DecCfg.defObjectType=LC;
				if(IsDlgButtonChecked(hWndDlg,IDC_RADIO_SSR))
					cfg.DecCfg.defObjectType=SSR;
				if(IsDlgButtonChecked(hWndDlg,IDC_RADIO_LTP))
					cfg.DecCfg.defObjectType=LTP;
				if(IsDlgButtonChecked(hWndDlg,IDC_RADIO_HE))
					cfg.DecCfg.defObjectType=HE_AAC;
				switch(SendMessage(GetDlgItem(hWndDlg, IDC_CB_BITSPERSAMPLE), CB_GETCURSEL, 0, 0))
				{
				case 0:
					cfg.DecCfg.outputFormat=FAAD_FMT_16BIT;
					break;
				case 1:
					cfg.DecCfg.outputFormat=FAAD_FMT_24BIT;
					break;
				case 2:
					cfg.DecCfg.outputFormat=FAAD_FMT_32BIT;
					break;
				case 3:
					cfg.DecCfg.outputFormat=FAAD_FMT_FLOAT;
					break;
				case 4:
					cfg.DecCfg.outputFormat=FAAD_FMT_64BIT;
					break;
				}

				cfg.DecCfg.defSampleRate=GetDlgItemInt(hWndDlg, IDC_CB_SAMPLERATE, 0, FALSE);
				cfg.DecCfg.downMatrix=IsDlgButtonChecked(hWndDlg,IDC_CHK_DOWNMATRIX) ? TRUE : FALSE;
				cfg.DecCfg.useOldADTSFormat=IsDlgButtonChecked(hWndDlg,IDC_CHK_OLDADTS) ? TRUE : FALSE;
				cfg.DefaultCfg=IsDlgButtonChecked(hWndDlg,IDC_CHK_DEFAULTCFG) ? TRUE : FALSE;

				EndDialog(hWndDlg, (DWORD)TRUE);
			}
			break;

        case IDCANCEL:
			// Ignore data values entered into the controls        
			// and dismiss the dialog window returning FALSE
			EndDialog(hWndDlg, (DWORD)FALSE);
			break;

		case IDC_BTN_ABOUT:
				DialogBox((HINSTANCE)hInstance,(LPCSTR)MAKEINTRESOURCE(IDD_ABOUT), (HWND)hWndDlg, (DLGPROC)DialogMsgProcAbout);
			break;
		}
		break; // End of WM_COMMAND
	default: 
		return FALSE;
	}
 
	return TRUE;
}

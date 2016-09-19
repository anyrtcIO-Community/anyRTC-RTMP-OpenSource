/*
FAAC - codec plugin for Cooledit
Copyright (C) 2002-2004 Antonio Foranna

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
#include "filters.h"	// CoolEdit
#include "Defines.h"	// my defines
#include "EncDialog.h"
#include "Cfaac.h"



// *********************************************************************************************



inline DWORD ERROR_FGO(char *msg)
{
	if(msg)
	{
	char buf[100];
		sprintf(buf,"FilterGetOptions: %s", msg);
		MessageBox(0, buf, APP_NAME " plugin", MB_OK|MB_ICONSTOP);
	}
	return 0;
}
// -----------------------------------------------------------------------------------------------

DWORD FAR PASCAL FilterGetOptions(HWND hWnd, HINSTANCE hInst, long lSamprate, WORD wChannels, WORD wBitsPerSample, DWORD dwOptions)
{
long retVal=DialogBoxParam((HINSTANCE)hInst,(LPCSTR)MAKEINTRESOURCE(IDD_ENCODER), (HWND)hWnd, (DLGPROC)DIALOGMsgProcEnc, dwOptions);

	if(retVal==-1)
		/*return */ERROR_FGO("DialogBoxParam");

	return !retVal ? dwOptions : retVal;
}
// *********************************************************************************************

// GetSuggestedSampleType() is called if OpenFilterOutput() returns NULL
void FAR PASCAL GetSuggestedSampleType(LONG *lplSamprate, WORD *lpwBitsPerSample, WORD *wChannels)
{
	*lplSamprate=0; // don't care
	*lpwBitsPerSample= *lpwBitsPerSample<=16 ? 0 : 16;
	*wChannels= *wChannels<49 ? 0 : 48;
}
// *********************************************************************************************

void FAR PASCAL CloseFilterOutput(HANDLE hOutput)
{
	if(!hOutput)
		return;

Cfaac tmp(hOutput); // this line frees memory
}              
// *********************************************************************************************

HANDLE FAR PASCAL OpenFilterOutput(LPSTR lpstrFilename,long lSamprate,WORD wBitsPerSample,WORD wChannels,long lSize, long far *lpChunkSize, DWORD dwOptions)
{
HANDLE	hOutput;
Cfaac	tmp;
CMyEncCfg	cfg(false);
char	*srcFilename=tmp.getSourceFilename(cfg.TagSrcPath,lpstrFilename,cfg.TagSrcExt);

	if(hOutput=tmp.Init(srcFilename,lpstrFilename,lSamprate,wBitsPerSample,wChannels,lSize))
	{
	MYOUTPUT *mo;
		GLOBALLOCK(mo,hOutput,MYOUTPUT,return NULL);
		*lpChunkSize=mo->samplesInput*(wBitsPerSample>>3); // size of samplesInput

		GlobalUnlock(hOutput);
		tmp.hOutput=NULL;
	}

	FREE_ARRAY(srcFilename);
	return hOutput;
}
// *********************************************************************************************

DWORD FAR PASCAL WriteFilterOutput(HANDLE hOutput, unsigned char far *bufIn, long lBytes)
{
	if(!hOutput)
		return 0;

Cfaac tmp;
DWORD bytesWritten;

	bytesWritten=tmp.processData(hOutput,bufIn,lBytes);
	return bytesWritten ? bytesWritten : 0x7fffffff; // bytesWritten<=0 stops CoolEdit
}

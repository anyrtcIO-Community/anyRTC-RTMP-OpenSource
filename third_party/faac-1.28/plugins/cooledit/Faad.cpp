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
//#include "resource.h"
#include "filters.h"	// CoolEdit
#include "DecDialog.h"
#include "Cfaad.h"

// *********************************************************************************************

BOOL FAR PASCAL FilterUnderstandsFormat(LPSTR filename)
{
WORD len;

	if((len=lstrlen(filename))>4 && 
		(!strcmpi(filename+len-4,".aac") ||
		!strcmpi(filename+len-4,".mp4") ||
		!strcmpi(filename+len-4,".m4a")))
		return TRUE;
	return FALSE;
}
// *********************************************************************************************

long FAR PASCAL FilterGetFileSize(HANDLE hInput)
{
	if(!hInput)
		return 0;

DWORD	dst_size;
MYINPUT	*mi;

	GLOBALLOCK(mi,hInput,MYINPUT,return 0);
	dst_size=mi->dst_size;
	
	GlobalUnlock(hInput);

	return dst_size;
}
// *********************************************************************************************

DWORD FAR PASCAL FilterOptionsString(HANDLE hInput, LPSTR szString)
{
	if(!hInput)
	{
		lstrcpy(szString,"");
		return 0;
	}

MYINPUT	*mi;

	GLOBALLOCK(mi,hInput,MYINPUT,return 0);

	sprintf(szString,"MPEG%d - %lu bps\n", mi->file_info.version ? 4 : 2, mi->file_info.bitrate);
	
	if(mi->IsMP4)  // MP4 file --------------------------------------------------------------------
		lstrcat(szString,mpeg4AudioNames[mi->type]);
	else  // AAC file -----------------------------------------------------------------------------
	{
		switch(mi->file_info.headertype)
		{
		case RAW:
			sprintf(szString,"MPEG%d\nRaw\n", mi->file_info.version ? 4 : 2);
			lstrcat(szString,mpeg4AudioNames[mi->file_info.object_type]);
			GlobalUnlock(hInput);
			return 1;//0; // call FilterGetOptions()
		case ADIF:
			lstrcat(szString,"ADIF\n");
			break;
		case ADTS:
			lstrcat(szString,"ADTS\n");
			break;
		}
		
		lstrcat(szString,mpeg4AudioNames[mi->file_info.object_type]);
/*		switch(mi->file_info.object_type)
		{
		case MAIN:
			lstrcat(szString,"Main");
			break;
		case LC:
			lstrcat(szString,"LC (Low Complexity)");
			break;
		case SSR:
			lstrcat(szString,"SSR (unsupported)");
			break;
		case LTP:
			lstrcat(szString,"LTP (Long Term Prediction)");
			break;
		case HE_AAC:
			lstrcat(szString,"HE (High Efficiency)");
			break;
		}*/
	}
	
	GlobalUnlock(hInput);
	return 1; // don't call FilterGetOptions()
}
// *********************************************************************************************
/*
DWORD FAR PASCAL FilterOptions(HANDLE hInput)
{
//	FilterGetOptions() is called if this function and FilterSetOptions() are exported and FilterOptionsString() returns 0
//	FilterSetOptions() is called only if this function is exported and and it returns 0

	return 1;
}
// ---------------------------------------------------------------------------------------------

DWORD FAR PASCAL FilterSetOptions(HANDLE hInput, DWORD dwOptions, LONG lSamprate, WORD wChannels, WORD wBPS)
{
	return dwOptions;
}*/
// *********************************************************************************************

void FAR PASCAL CloseFilterInput(HANDLE hInput)
{
	if(!hInput)
		return;

/*	if(mi->file_info.headertype==RAW)
	{
	CRegistry	reg;

		if(reg.openCreateReg(HKEY_CURRENT_USER,REGISTRY_PROGRAM_NAME  "\\FAAD"))
			reg.setRegBool("OpenDialog",FALSE);
		else
			MessageBox(0,"Can't open registry!",0,MB_OK|MB_ICONSTOP);
	}*/

Cfaad tmp(hInput);
}
// *********************************************************************************************

#define ERROR_OFI(msg) \
{ \
	if(msg) \
		MessageBox(0, msg, APP_NAME " plugin", MB_OK|MB_ICONSTOP); \
	if(hInput) \
	{ \
		GlobalUnlock(hInput); \
		CloseFilterInput(hInput); \
	} \
	return 0; \
}
// -----------------------------------------------------------------------------------------------

// return handle that will be passed in to close, and write routines
HANDLE FAR PASCAL OpenFilterInput(LPSTR lpstrFilename, long far *lSamprate, WORD far *wBitsPerSample, WORD far *wChannels, HWND hWnd, long far *lChunkSize)
{
HANDLE	hInput=NULL;
Cfaad	tmp;
CMyDecCfg cfg(false);

	if(!*lSamprate && !tmp.IsMP4(lpstrFilename))
	{
/*	aac_buffer	b;
	float		fLength;
	int			bitrate;
	DWORD		headertype;
		tmp.GetAACInfos(lpstrFilename,&b,&headertype,&fLength,&bitrate);
		if(headertype==RAW)
			tmp.ShowDlg4RawAAC=ShowDlg4RawAAC;*/
    DWORD           *seek_table;
    int             seek_table_length;
	faadAACInfo     file_info;
		if(!get_AAC_format(lpstrFilename, &file_info, &seek_table, &seek_table_length, 0))
			if(file_info.headertype==RAW)
				tmp.ShowDlg4RawAAC=ShowDlg4RawAAC;
	}

	if(hInput=tmp.getInfos(lpstrFilename))
	{
	MYINPUT	*mi;
		GLOBALLOCK(mi,hInput,MYINPUT,return NULL);
	
		*wChannels=(WORD)mi->Channels;
		*lSamprate=mi->Samprate;
		*wBitsPerSample=mi->BitsPerSample;
		*lChunkSize=(*wBitsPerSample/8)*1024**wChannels*2;

		GlobalUnlock(hInput);
		tmp.hInput=NULL;
	}

	return hInput;
}
// *********************************************************************************************

DWORD FAR PASCAL ReadFilterInput(HANDLE hInput, unsigned char far *bufout, long lBytes)
{
	if(!hInput)
		return 0;

Cfaad tmp;

	return tmp.processData(hInput,bufout,lBytes);
}

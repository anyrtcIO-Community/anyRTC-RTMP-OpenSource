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
#include "filters.h"	//CoolEdit
//#include "faac.h"
#include "Defines.h"	// my defines

// Plugins of CoolEdit can be unloaded between each call of its exported funcs,
// that's why no global variables can be used

HINSTANCE hInstance=NULL;
HBITMAP hBmBrowse=NULL;

BOOL WINAPI DllMain (HANDLE hModule, DWORD fdwReason, LPVOID lpReserved)
{
	switch (fdwReason)
	{
	case DLL_PROCESS_ATTACH:
		hInstance=(HINSTANCE)hModule;
		if(!hBmBrowse)
			hBmBrowse=(HBITMAP)LoadImage(hInstance,MAKEINTRESOURCE(IDB_BROWSE),IMAGE_BITMAP,0,0,/*LR_CREATEDIBSECTION|*/LR_LOADTRANSPARENT|LR_LOADMAP3DCOLORS);
        /*	Code from LibMain inserted here.  Return TRUE to keep the
			DLL loaded or return FALSE to fail loading the DLL.

			You may have to modify the code in your original LibMain to
			account for the fact that it may be called more than once.
			You will get one DLL_PROCESS_ATTACH for each process that
			loads the DLL. This is different from LibMain which gets
			called only once when the DLL is loaded. The only time this
			is critical is when you are using shared data sections.
			If you are using shared data sections for statically
			allocated data, you will need to be careful to initialize it
			only once. Check your code carefully.

			Certain one-time initializations may now need to be done for
			each process that attaches. You may also not need code from
			your original LibMain because the operating system may now
			be doing it for you.
         */
         break;
		
	case DLL_THREAD_ATTACH:
         /*	Called each time a thread is created in a process that has
			already loaded (attached to) this DLL. Does not get called
			for each thread that exists in the process before it loaded
			the DLL.
			Do thread-specific initialization here.
         */
		break;
		
	case DLL_THREAD_DETACH:
         /*	Same as above, but called when a thread in the process
            exits.
            Do thread-specific cleanup here.
         */
		break;
		
	case DLL_PROCESS_DETACH:
		/*	Code from _WEP inserted here.  This code may (like the
			LibMain) not be necessary.  Check to make certain that the
			operating system is not doing it for you.
			*/
		hInstance=NULL;
		if(hBmBrowse)
		{
            DeleteObject(hBmBrowse);
            hBmBrowse=NULL;
		}
		break;
	}
 
	/*	The return value is only used for DLL_PROCESS_ATTACH; all other
		conditions are ignored.
	*/
	return TRUE;   // successful DLL_PROCESS_ATTACH
}

// Fill COOLQUERY structure with information regarding this file filter
short FAR PASCAL QueryCoolFilter(COOLQUERY far * cq)
{
	lstrcpy(cq->szName, APP_NAME);		
	lstrcpy(cq->szCopyright, APP_NAME " codec");
	lstrcpy(cq->szExt,"AAC");
	lstrcpy(cq->szExt2,"MP4"); 
	lstrcpy(cq->szExt3,"M4A"); 
	cq->lChunkSize=16384; 
	cq->dwFlags=QF_RATEADJUSTABLE|QF_CANLOAD|QF_CANSAVE|QF_HASOPTIONSBOX|QF_CANDO32BITFLOATS;
 	cq->Mono8=R_5500|R_11025|R_22050|R_32075|R_44100|R_48000;
 	cq->Mono12=0;		// no support
 	cq->Mono16=R_5500|R_11025|R_22050|R_32075|R_44100|R_48000;
 	cq->Mono24=0;
 	cq->Mono32=R_5500|R_11025|R_22050|R_32075|R_44100|R_48000;
 	cq->Stereo8=R_5500|R_11025|R_22050|R_32075|R_44100|R_48000;
 	cq->Stereo12=0;
 	cq->Stereo16=R_5500|R_11025|R_22050|R_32075|R_44100|R_48000;
 	cq->Stereo24=0;
 	cq->Stereo32=R_5500|R_11025|R_22050|R_32075|R_44100|R_48000;
 	cq->Quad8=0;
 	cq->Quad16=0;
 	cq->Quad32=0;
 	return C_VALIDLIBRARY;
}

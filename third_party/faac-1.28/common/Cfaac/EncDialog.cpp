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
#include <shlobj.h>		// Browse
#include <shellapi.h>	// ShellExecute
#include <Commdlg.h>
#include "resource.h"
#include "Defines.h"	// my defines
#include "CTag.h"
#include "Cfaac.h"
#include "EncDialog.h"

#include <commctrl.h>
#include <id3v2tag.h>
#include <..\..\common\id3lib\win32\config.h> // ID3LIB_FULLNAME

// *********************************************************************************************

#ifdef USE_OUTPUT_FOLDER
extern char	config_AACoutdir[MAX_PATH];
#endif

extern HINSTANCE hInstance;
extern HBITMAP hBmBrowse;

// *********************************************************************************************

/*
DWORD PackCfg(MY_ENC_CFG *cfg)
{
DWORD dwOptions=0;

	if(cfg->AutoCfg)
		dwOptions=1<<31;
	dwOptions|=(DWORD)cfg->EncCfg.mpegVersion<<30;
	dwOptions|=(DWORD)cfg->EncCfg.aacObjectType<<28;
	dwOptions|=(DWORD)cfg->EncCfg.allowMidside<<27;
	dwOptions|=(DWORD)cfg->EncCfg.useTns<<26;
	dwOptions|=(DWORD)cfg->EncCfg.useLfe<<25;
	dwOptions|=(DWORD)cfg->EncCfg.outputFormat<<24;
	if(cfg->UseQuality)
		dwOptions|=(((DWORD)cfg->EncCfg.quantqual>>1)&0xff)<<16; // [2,512]
	else
		dwOptions|=(((DWORD)cfg->EncCfg.bitRate>>1)&0xff)<<16; // [2,512]
	if(cfg->UseQuality)
		dwOptions|=1<<15;
	dwOptions|=((DWORD)cfg->EncCfg.bandWidth>>1)&&0x7fff; // [0,65536]

	return dwOptions;
}
// -----------------------------------------------------------------------------------------------

void UnpackCfg(MY_ENC_CFG *cfg, DWORD dwOptions)
{
	cfg->AutoCfg=dwOptions>>31;
	cfg->EncCfg.mpegVersion=(dwOptions>>30)&1;
	cfg->EncCfg.aacObjectType=(dwOptions>>28)&3;
	cfg->EncCfg.allowMidside=(dwOptions>>27)&1;
	cfg->EncCfg.useTns=(dwOptions>>26)&1;
	cfg->EncCfg.useLfe=(dwOptions>>25)&1;
	cfg->EncCfg.outputFormat=(dwOptions>>24)&1;
	cfg->EncCfg.bitRate=((dwOptions>>16)&0xff)<<1;
	cfg->UseQuality=(dwOptions>>15)&1;
	cfg->EncCfg.bandWidth=(dwOptions&0x7fff)<<1;
}*/
// -----------------------------------------------------------------------------------------------

#define INIT_CB(hWnd,nID,list,FillList,IdSelected) \
{ \
	if(FillList) \
		for(int i=0; list[i]; i++) \
			SendMessage(GetDlgItem(hWnd, nID), CB_ADDSTRING, 0, (LPARAM)list[i]); \
	SendMessage(GetDlgItem(hWnd, nID), CB_SETCURSEL, IdSelected, 0); \
}
// -----------------------------------------------------------------------------------------------

#define INIT_CB_GENRES(hWnd,nID,ID3Genres,IdSelected) \
{ \
	for(int i=0; i<(sizeof(ID3Genres)/sizeof(ID3Genres[0])); i++) \
		SendMessage(GetDlgItem(hWnd, nID), CB_ADDSTRING, 0, (LPARAM)ID3Genres[i].name); \
	SendMessage(GetDlgItem(hWnd, nID), CB_SETCURSEL, IdSelected, 0); \
}
// -----------------------------------------------------------------------------------------------

#define DISABLE_LTP \
{ \
	if(IsDlgButtonChecked(hWndDlg,IDC_RADIO_MPEG2) && \
	   IsDlgButtonChecked(hWndDlg,IDC_RADIO_LTP)) \
	{ \
		CheckDlgButton(hWndDlg,IDC_RADIO_LTP,FALSE); \
		CheckDlgButton(hWndDlg,IDC_RADIO_MAIN,TRUE); \
	} \
    EnableWindow(GetDlgItem(hWndDlg, IDC_RADIO_LTP), FALSE); \
}
// -----------------------------------------------------------------------------------------------

//        EnableWindow(GetDlgItem(hWndDlg, IDC_RADIO_SSR), Enabled);
//        EnableWindow(GetDlgItem(hWndDlg, IDC_CHK_USELFE), Enabled);
#define DISABLE_CTRLS_ENC(Enabled) \
{ \
	CheckDlgButton(hWndDlg,IDC_CHK_AUTOCFG, !Enabled); \
	EnableWindow(GetDlgItem(hWndDlg, IDC_RADIO_MPEG4), Enabled); \
	EnableWindow(GetDlgItem(hWndDlg, IDC_RADIO_MPEG2), Enabled); \
	EnableWindow(GetDlgItem(hWndDlg, IDC_RADIO_RAW), Enabled); \
	EnableWindow(GetDlgItem(hWndDlg, IDC_RADIO_ADTS), Enabled); \
	EnableWindow(GetDlgItem(hWndDlg, IDC_CHK_ALLOWMIDSIDE), Enabled); \
	EnableWindow(GetDlgItem(hWndDlg, IDC_CHK_USETNS), Enabled); \
	EnableWindow(GetDlgItem(hWndDlg, IDC_CHK_USELFE), Enabled); \
	EnableWindow(GetDlgItem(hWndDlg, IDC_CB_QUALITY), Enabled); \
	EnableWindow(GetDlgItem(hWndDlg, IDC_CB_BITRATE), Enabled); \
	EnableWindow(GetDlgItem(hWndDlg, IDC_CB_BANDWIDTH), Enabled); \
	EnableWindow(GetDlgItem(hWndDlg, IDC_RADIO_QUALITY), Enabled); \
	EnableWindow(GetDlgItem(hWndDlg, IDC_RADIO_BITRATE), Enabled); \
    EnableWindow(GetDlgItem(hWndDlg, IDC_RADIO_MAIN), Enabled); \
    EnableWindow(GetDlgItem(hWndDlg, IDC_RADIO_LOW), Enabled); \
    EnableWindow(GetDlgItem(hWndDlg, IDC_RADIO_LTP), Enabled); \
	if(IsDlgButtonChecked(hWndDlg,IDC_RADIO_MPEG4)) \
		EnableWindow(GetDlgItem(hWndDlg, IDC_RADIO_LTP), Enabled); \
	else \
		DISABLE_LTP \
}
// -----------------------------------------------------------------------------------------------

#define ENABLE_TAG(Enabled) \
{ \
	EnableWindow(GetDlgItem(hWndDlg, IDC_E_ARTIST), Enabled); \
	EnableWindow(GetDlgItem(hWndDlg, IDC_E_TITLE), Enabled); \
	EnableWindow(GetDlgItem(hWndDlg, IDC_E_ALBUM), Enabled); \
	EnableWindow(GetDlgItem(hWndDlg, IDC_E_YEAR), Enabled); \
	EnableWindow(GetDlgItem(hWndDlg, IDC_CB_GENRE), Enabled); \
	EnableWindow(GetDlgItem(hWndDlg, IDC_E_WRITER), Enabled); \
	EnableWindow(GetDlgItem(hWndDlg, IDC_E_COMMENT), Enabled); \
	EnableWindow(GetDlgItem(hWndDlg, IDC_E_COMPILATION), Enabled); \
	EnableWindow(GetDlgItem(hWndDlg, IDC_CHK_COMPILATION), Enabled); \
	EnableWindow(GetDlgItem(hWndDlg, IDC_E_TRACK), Enabled); \
	EnableWindow(GetDlgItem(hWndDlg, IDC_E_NTRACKS), Enabled); \
	EnableWindow(GetDlgItem(hWndDlg, IDC_E_DISK), Enabled); \
	EnableWindow(GetDlgItem(hWndDlg, IDC_E_NDISKS), Enabled); \
	EnableWindow(GetDlgItem(hWndDlg, IDC_E_ARTFILE), Enabled); \
	EnableWindow(GetDlgItem(hWndDlg, IDC_BTN_ARTFILE), Enabled); \
}
// -----------------------------------------------------------------------------------------------

#define ENABLE_AACTAGS(Enabled) \
{ \
	EnableWindow(GetDlgItem(hWndDlg, IDC_E_COMPILATION), Enabled); \
	EnableWindow(GetDlgItem(hWndDlg, IDC_CHK_COMPILATION), Enabled); \
	EnableWindow(GetDlgItem(hWndDlg, IDC_E_NTRACKS), Enabled); \
	EnableWindow(GetDlgItem(hWndDlg, IDC_E_DISK), Enabled); \
	EnableWindow(GetDlgItem(hWndDlg, IDC_E_NDISKS), Enabled); \
}
// -----------------------------------------------------------------------------------------------

#ifdef USE_OUTPUT_FOLDER
static int CALLBACK BrowseCallbackProc(HWND hwnd, UINT uMsg, LPARAM lParam, LPARAM lpData)
{
	if (uMsg == BFFM_INITIALIZED)
	{
		SetWindowText(hwnd,"Select folder");
		SendMessage(hwnd,BFFM_SETSELECTION,(WPARAM)TRUE,(LPARAM)lpData);
	}
	return 0;
}
#else
	#ifdef USE_IMPORT_TAG
	static int CALLBACK BrowseCallbackProc(HWND hwnd, UINT uMsg, LPARAM lParam, LPARAM lpData)
	{
		if (uMsg == BFFM_INITIALIZED)
		{
			SetWindowText(hwnd,"Select folder");
			SendMessage(hwnd,BFFM_SETSELECTION,(WPARAM)TRUE,(LPARAM)lpData);
		}
		return 0;
	}
	#endif
#endif

// -----------------------------------------------------------------------------------------------

BOOL DialogMsgProcAbout(HWND hWndDlg, UINT Message, WPARAM wParam, LPARAM lParam)
{
	switch(Message)
	{
	case WM_INITDIALOG:
		{
		char buf[512];
		char *faac_id_string, *faac_copyright_string;

		sprintf(buf,
					APP_NAME " plugin " APP_VER " by Antonio Foranna\n\n"
					"Libraries used:\n"
					"\tlibfaac v%s\n"
					"\tFAAD2 v" FAAD2_VERSION "\n"
					"\t" MPEG4IP_PACKAGE " v" MPEG4IP_VERSION "\n"
					"\t" ID3LIB_FULLNAME "\n\n"
					"This code is given with FAAC package and does not contain executables.\n"
					"This program is free software and can be distributed/modifyed under the terms of the GNU General Public License.\n"
					"This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY.\n\n"
					"Compiled on %s\n",
				(faacEncGetVersion(&faac_id_string, &faac_copyright_string)==FAAC_CFG_VERSION) ? faac_id_string : " wrong libfaac version",
					__DATE__
					);
			SetDlgItemText(hWndDlg, IDC_L_ABOUT, buf);
		}
		break;
	case WM_COMMAND:
		switch(LOWORD(wParam))
		{
		case IDOK:
			EndDialog(hWndDlg, TRUE);
			break;
        case IDCANCEL:
			// Ignore data values entered into the controls and dismiss the dialog window returning FALSE
			EndDialog(hWndDlg, FALSE);
			break;
		case IDC_AUDIOCODING:
			ShellExecute(hWndDlg, NULL, "http://www.audiocoding.com", NULL, NULL, SW_SHOW);
			break;
		case IDC_MPEG4IP:
			ShellExecute(hWndDlg, NULL, "http://www.mpeg4ip.net", NULL, NULL, SW_SHOW);
			break;
		case IDC_ID3:
			ShellExecute(hWndDlg, NULL, "http://id3lib.sourceforge.net", NULL, NULL, SW_SHOW);
			break;
		case IDC_EMAIL:
			ShellExecute(hWndDlg, NULL, "mailto:ntnfrn_email-temp@yahoo.it", NULL, NULL, SW_SHOW);
			break;
		}
		break;
	default: 
		return FALSE;
	}

	return TRUE;
}
// -----------------------------------------------------------------------------------------------

// ripped from id3v2tag.c
ID3GENRES ID3Genres[]=
{
    123,    "Acapella",
    34,     "Acid",
    74,     "Acid Jazz",
    73,     "Acid Punk",
    99,     "Acoustic",
    20,     "Alternative",
    40,     "AlternRock",
    26,     "Ambient",
    90,     "Avantgarde",
    116,    "Ballad",
    41,     "Bass",
    85,     "Bebob",
    96,     "Big Band",
    89,     "Bluegrass",
    0,      "Blues",
    107,    "Booty Bass",
    65,     "Cabaret",
    88,     "Celtic",
    104,    "Chamber Music",
    102,    "Chanson",
    97,     "Chorus",
    61,     "Christian Rap",
    1,      "Classic Rock",
    32,     "Classical",
    112,    "Club",
    57,     "Comedy",
    2,      "Country",
    58,     "Cult",
    3,      "Dance",
    125,    "Dance Hall",
    50,     "Darkwave",
    254,    "Data",
    22,     "Death Metal",
    4,      "Disco",
    55,     "Dream",
    122,    "Drum Solo",
    120,    "Duet",
    98,     "Easy Listening",
    52,     "Electronic",
    48,     "Ethnic",
    124,    "Euro-House",
    25,     "Euro-Techno",
    54,     "Eurodance",
    84,     "Fast Fusion",
    80,     "Folk",
    81,     "Folk-Rock",
    115,    "Folklore",
    119,    "Freestyle",
    5,      "Funk",
    30,     "Fusion",
    36,     "Game",
    59,     "Gangsta",
    38,     "Gospel",
    49,     "Gothic",
    91,     "Gothic Rock",
    6,      "Grunge",
    79,     "Hard Rock",
    7,      "Hip-Hop",
    35,     "House",
    100,    "Humour",
    19,     "Industrial",
    33,     "Instrumental",
    46,     "Instrumental Pop",
    47,     "Instrumental Rock",
    8,      "Jazz",
    29,     "Jazz+Funk",
    63,     "Jungle",
    86,     "Latin",
    71,     "Lo-Fi",
    45,     "Meditative",
    9,      "Metal",
    77,     "Musical",
    82,     "National Folk",
    64,     "Native American",
    10,     "New Age",
    66,     "New Wave",
    39,     "Noise",
    255,    "Not Set",
    11,     "Oldies",
    103,    "Opera",
    12,     "Other",
    75,     "Polka",
    13,     "Pop",
    62,     "Pop/Funk",
    53,     "Pop-Folk",
    109,    "Porn Groove",
    117,    "Power Ballad",
    23,     "Pranks",
    108,    "Primus",
    92,     "Progressive Rock",
    67,     "Psychadelic",
    93,     "Psychedelic Rock",
    43,     "Punk",
    121,    "Punk Rock",
    14,     "R&B",
    15,     "Rap",
    68,     "Rave",
    16,     "Reggae",
    76,     "Retro",
    87,     "Revival",
    118,    "Rhythmic Soul",
    17,     "Rock",
    78,     "Rock & Roll",
    114,    "Samba",
    110,    "Satire",
    69,     "Showtunes",
    21,     "Ska",
    111,    "Slow Jam",
    95,     "Slow Rock",
    105,    "Sonata",
    42,     "Soul",
    37,     "Sound Clip",
    24,     "Soundtrack",
    56,     "Southern Rock",
    44,     "Space",
    101,    "Speech",
    83,     "Swing",
    94,     "Symphonic Rock",
    106,    "Symphony",
    113,    "Tango",
    18,     "Techno",
    51,     "Techno-Industrial",
    60,     "Top 40",
    70,     "Trailer",
    31,     "Trance",
    72,     "Tribal",
    27,     "Trip-Hop",
    28,     "Vocal"
};

BOOL CALLBACK DIALOGMsgProcEnc(HWND hWndDlg, UINT Message, WPARAM wParam, LPARAM lParam)
{
	switch(Message)
	{
	case WM_INITDIALOG:
		{
		char buf[50];
		char FillList=wParam||lParam ? 1 : 0;
		char *Quality[]={"Default (100)","10","20","30","40","50","60","70","80","90","100","110","120","130","140","150","200","300","400","500",0};
		char *BitRate[]={"Auto","8","16","18","20","24","32","40","48","56","64","80","96","112","128","160","192","224","256","320","384",0};
		char *BandWidth[]={"Auto","Full","4000","8000","11025","16000","22050","24000","32000","44100","48000",0};
		char *Ext[]={".aac",".mp4",".m4a",".m4b",0};
		CMyEncCfg cfg(false);
			
//			sprintf(Quality[0]+8,"%3d",3);

			SetWindowPos(GetDlgItem(hWndDlg,IDC_CHK_TAG),GetDlgItem(hWndDlg,IDC_GRP_TAG),0,0,0,0,SWP_NOMOVE | SWP_NOSIZE);

			INIT_CB(hWndDlg,IDC_CB_QUALITY,Quality,FillList,0);
			INIT_CB(hWndDlg,IDC_CB_BITRATE,BitRate,FillList,0);
			INIT_CB(hWndDlg,IDC_CB_BANDWIDTH,BandWidth,FillList,0);
			INIT_CB(hWndDlg,IDC_CB_EXT,Ext,FillList,0);

			INIT_CB_GENRES(hWndDlg,IDC_CB_GENRE,ID3Genres,0);

			SendMessage(GetDlgItem(hWndDlg, IDC_BTN_ARTFILE), BM_SETIMAGE, IMAGE_BITMAP, (LPARAM) hBmBrowse);
#ifdef USE_OUTPUT_FOLDER			
			SendMessage(GetDlgItem(hWndDlg, IDC_BTN_BROWSE), BM_SETIMAGE, IMAGE_BITMAP, (LPARAM) hBmBrowse);
			if(!cfg.OutDir || !*cfg.OutDir)
			{
				GetCurrentDirectory(MAX_PATH,config_AACoutdir);
				FREE_ARRAY(cfg.OutDir);
				cfg.OutDir=strdup(config_AACoutdir);
			}
			else
				strcpy(config_AACoutdir,cfg.OutDir);
			SetDlgItemText(hWndDlg, IDC_E_BROWSE, cfg.OutDir);
#else
			ShowWindow(GetDlgItem(hWndDlg, IDC_BTN_BROWSE),SW_HIDE);
			ShowWindow(GetDlgItem(hWndDlg, IDC_E_BROWSE),SW_HIDE);
			ShowWindow(GetDlgItem(hWndDlg, IDC_L_BROWSE),SW_HIDE);
#endif
			CheckDlgButton(hWndDlg,IDC_RADIO_MPEG4,FALSE);
			CheckDlgButton(hWndDlg,IDC_RADIO_MPEG2,FALSE);
			if(cfg.EncCfg.mpegVersion==MPEG4)
				CheckDlgButton(hWndDlg,IDC_RADIO_MPEG4,TRUE);
			else
				CheckDlgButton(hWndDlg,IDC_RADIO_MPEG2,TRUE);
			
			CheckDlgButton(hWndDlg,IDC_RADIO_MAIN,FALSE);
			CheckDlgButton(hWndDlg,IDC_RADIO_LOW,FALSE);
			CheckDlgButton(hWndDlg,IDC_RADIO_SSR,FALSE);
			CheckDlgButton(hWndDlg,IDC_RADIO_LTP,FALSE);
			switch(cfg.EncCfg.aacObjectType)
			{
			case MAIN:
				CheckDlgButton(hWndDlg,IDC_RADIO_MAIN,TRUE);
				break;
			case LOW:
				CheckDlgButton(hWndDlg,IDC_RADIO_LOW,TRUE);
				break;
			case SSR:
				CheckDlgButton(hWndDlg,IDC_RADIO_SSR,TRUE);
				break;
			case LTP:
				CheckDlgButton(hWndDlg,IDC_RADIO_LTP,TRUE);
				DISABLE_LTP
				break;
			}
			
			CheckDlgButton(hWndDlg,IDC_RADIO_RAW,FALSE);
			CheckDlgButton(hWndDlg,IDC_RADIO_ADTS,FALSE);
			switch(cfg.EncCfg.outputFormat)
			{
			case RAW:
				CheckDlgButton(hWndDlg,IDC_RADIO_RAW,TRUE);
				break;
			case ADTS:
				CheckDlgButton(hWndDlg,IDC_RADIO_ADTS,TRUE);
				break;
			}
			
			CheckDlgButton(hWndDlg, IDC_CHK_ALLOWMIDSIDE, cfg.EncCfg.allowMidside);
			CheckDlgButton(hWndDlg, IDC_CHK_USETNS, cfg.EncCfg.useTns);
			CheckDlgButton(hWndDlg, IDC_CHK_USELFE, cfg.EncCfg.useLfe);

			if(cfg.UseQuality)
				CheckDlgButton(hWndDlg,IDC_RADIO_QUALITY,TRUE);
			else
				CheckDlgButton(hWndDlg,IDC_RADIO_BITRATE,TRUE);

			switch(cfg.EncCfg.quantqual)
			{
			case 100:
				SendMessage(GetDlgItem(hWndDlg, IDC_CB_QUALITY), CB_SETCURSEL, 0, 0);
				break;
			default:
				if(cfg.EncCfg.quantqual<10)
					cfg.EncCfg.quantqual=10;
				if(cfg.EncCfg.quantqual>500)
					cfg.EncCfg.quantqual=500;
				sprintf(buf,"%lu",cfg.EncCfg.quantqual);
				SetDlgItemText(hWndDlg, IDC_CB_QUALITY, buf);
				break;
			}
			switch(cfg.EncCfg.bitRate)
			{
			case 0:
				SendMessage(GetDlgItem(hWndDlg, IDC_CB_BITRATE), CB_SETCURSEL, 0, 0);
				break;
			default:
				sprintf(buf,"%lu",cfg.EncCfg.bitRate);
				SetDlgItemText(hWndDlg, IDC_CB_BITRATE, buf);
				break;
			}
			switch(cfg.EncCfg.bandWidth)
			{
			case 0:
				SendMessage(GetDlgItem(hWndDlg, IDC_CB_BANDWIDTH), CB_SETCURSEL, 0, 0);
				break;
			case 0xffffffff:
				SendMessage(GetDlgItem(hWndDlg, IDC_CB_BANDWIDTH), CB_SETCURSEL, 1, 0);
				break;
			default:
				sprintf(buf,"%lu",cfg.EncCfg.bandWidth);
				SetDlgItemText(hWndDlg, IDC_CB_BANDWIDTH, buf);
				break;
			}
			
			SendMessage(GetDlgItem(hWndDlg, IDC_CB_EXT), CB_SETCURSEL, cfg.SaveMP4, 0);

			if(wParam|lParam)
			{
				CheckDlgButton(hWndDlg,IDC_CHK_AUTOCFG, cfg.AutoCfg);
				DISABLE_CTRLS_ENC(!cfg.AutoCfg);
			}
#ifdef USE_IMPORT_TAG
			CheckDlgButton(hWndDlg,IDC_CHK_IMPORTTAG, cfg.TagImport);
			cfg.TagImport=IsDlgButtonChecked(hWndDlg,IDC_CHK_IMPORTTAG) ? 1 : 0;
			ShowWindow(GetDlgItem(hWndDlg, IDC_CHK_IMPORTTAG),SW_SHOW);
	#ifdef USE_PATHEXT
			SetDlgItemText(hWndDlg, IDC_E_SOURCEPATH, cfg.TagSrcPath);
			SetDlgItemText(hWndDlg, IDC_E_SOURCEEXT, cfg.TagSrcExt);
			SendMessage(GetDlgItem(hWndDlg, IDC_BTN_BROWSEIMPORT), BM_SETIMAGE, IMAGE_BITMAP, (LPARAM) hBmBrowse);
			ShowWindow(GetDlgItem(hWndDlg, IDC_E_SOURCEEXT),SW_SHOW);
			ShowWindow(GetDlgItem(hWndDlg, IDC_E_SOURCEPATH),SW_SHOW);
			ShowWindow(GetDlgItem(hWndDlg, IDC_BTN_BROWSEIMPORT),SW_SHOW);
	#endif
#endif
			CheckDlgButton(hWndDlg,IDC_CHK_TAG, cfg.TagOn);
			ENABLE_TAG(cfg.TagOn && !cfg.TagImport);
			ENABLE_AACTAGS(cfg.TagOn && !cfg.TagImport && cfg.SaveMP4);
			SetDlgItemText(hWndDlg, IDC_E_ARTIST, cfg.Tag.artist);
			SetDlgItemText(hWndDlg, IDC_E_TITLE, cfg.Tag.title);
			SetDlgItemText(hWndDlg, IDC_E_ALBUM, cfg.Tag.album);
			SetDlgItemText(hWndDlg, IDC_E_YEAR, cfg.Tag.year);
			SetDlgItemText(hWndDlg, IDC_CB_GENRE, cfg.Tag.genre);
			SetDlgItemText(hWndDlg, IDC_E_WRITER, cfg.Tag.writer);
			SetDlgItemText(hWndDlg, IDC_E_COMMENT, cfg.Tag.comment);
			SetDlgItemText(hWndDlg, IDC_E_ARTFILE, cfg.Tag.artFilename);
			SetDlgItemInt(hWndDlg, IDC_E_TRACK, cfg.Tag.trackno, FALSE);
			SetDlgItemInt(hWndDlg, IDC_E_NTRACKS, cfg.Tag.ntracks, FALSE);
			SetDlgItemInt(hWndDlg, IDC_E_DISK, cfg.Tag.discno, FALSE);
			SetDlgItemInt(hWndDlg, IDC_E_NDISKS, cfg.Tag.ndiscs, FALSE);
			SetDlgItemInt(hWndDlg, IDC_E_COMPILATION, cfg.Tag.compilation, FALSE);
			CheckDlgButton(hWndDlg, IDC_CHK_COMPILATION, cfg.Tag.compilation);
		}
		break; // End of WM_INITDIALOG                                 
		
	case WM_CLOSE:
		// Closing the Dialog behaves the same as Cancel               
		PostMessage(hWndDlg, WM_COMMAND, IDCANCEL, 0L);
		break; // End of WM_CLOSE                                      
		
	case WM_COMMAND:
		switch(LOWORD(wParam))
		{
		case IDOK:
			{
//			HANDLE hCfg=(HANDLE)lParam;
			char buf[50];
			CMyEncCfg cfg;

				cfg.AutoCfg=IsDlgButtonChecked(hWndDlg,IDC_CHK_AUTOCFG) ? TRUE : FALSE;
				if(!cfg.AutoCfg)
				{
					cfg.EncCfg.mpegVersion=IsDlgButtonChecked(hWndDlg,IDC_RADIO_MPEG4) ? MPEG4 : MPEG2;
					if(IsDlgButtonChecked(hWndDlg,IDC_RADIO_MAIN))
						cfg.EncCfg.aacObjectType=MAIN;
					if(IsDlgButtonChecked(hWndDlg,IDC_RADIO_LOW))
						cfg.EncCfg.aacObjectType=LOW;
					if(IsDlgButtonChecked(hWndDlg,IDC_RADIO_SSR))
						cfg.EncCfg.aacObjectType=SSR;
					if(IsDlgButtonChecked(hWndDlg,IDC_RADIO_LTP))
						cfg.EncCfg.aacObjectType=LTP;
					cfg.EncCfg.allowMidside=IsDlgButtonChecked(hWndDlg, IDC_CHK_ALLOWMIDSIDE);
					cfg.EncCfg.useTns=IsDlgButtonChecked(hWndDlg, IDC_CHK_USETNS);
					cfg.EncCfg.useLfe=IsDlgButtonChecked(hWndDlg, IDC_CHK_USELFE);
					
					GetDlgItemText(hWndDlg, IDC_CB_BITRATE, buf, 50);
					switch(*buf)
					{
					case 'A': // Auto
						cfg.EncCfg.bitRate=0;
						break;
					default:
						cfg.EncCfg.bitRate=GetDlgItemInt(hWndDlg, IDC_CB_BITRATE, 0, FALSE);
					}
					GetDlgItemText(hWndDlg, IDC_CB_BANDWIDTH, buf, 50);
					switch(*buf)
					{
					case 'A': // Auto
						cfg.EncCfg.bandWidth=0;
						break;
					case 'F': // Full
						cfg.EncCfg.bandWidth=0xffffffff;
						break;
					default:
						cfg.EncCfg.bandWidth=GetDlgItemInt(hWndDlg, IDC_CB_BANDWIDTH, 0, FALSE);
					}
					cfg.UseQuality=IsDlgButtonChecked(hWndDlg,IDC_RADIO_QUALITY) ? TRUE : FALSE;
					GetDlgItemText(hWndDlg, IDC_CB_QUALITY, buf, 50);
					switch(*buf)
					{
					case 'D': // Default
						cfg.EncCfg.quantqual=100;
						break;
					default:
						cfg.EncCfg.quantqual=GetDlgItemInt(hWndDlg, IDC_CB_QUALITY, 0, FALSE);
					}
				}

				cfg.EncCfg.outputFormat=IsDlgButtonChecked(hWndDlg,IDC_RADIO_RAW) ? RAW : ADTS;
#ifdef USE_OUTPUT_FOLDER
				GetDlgItemText(hWndDlg, IDC_E_BROWSE, config_AACoutdir, MAX_PATH);
				FREE_ARRAY(cfg.OutDir);
				cfg.OutDir=strdup(config_AACoutdir);
#endif
				cfg.SaveMP4=(BYTE)SendMessage(GetDlgItem(hWndDlg, IDC_CB_EXT), CB_GETCURSEL, 0, 0);

				cfg.TagOn=IsDlgButtonChecked(hWndDlg,IDC_CHK_TAG) ? 1 : 0;
			char buffer[MAX_PATH];
#ifdef USE_IMPORT_TAG
				cfg.TagImport=IsDlgButtonChecked(hWndDlg,IDC_CHK_IMPORTTAG) ? 1 : 0;
	#ifdef USE_PATHEXT
				GetDlgItemText(hWndDlg, IDC_E_SOURCEPATH, buffer, MAX_PATH);
				cfg.TagSrcPath=strdup(buffer);
				GetDlgItemText(hWndDlg, IDC_E_SOURCEEXT, buffer, MAX_PATH);
				cfg.TagSrcExt=strdup(buffer);
	#endif
#endif
				GetDlgItemText(hWndDlg, IDC_E_ARTIST, buffer, MAX_PATH);
				cfg.Tag.artist=strdup(buffer);
				GetDlgItemText(hWndDlg, IDC_E_TITLE, buffer, MAX_PATH);
				cfg.Tag.title=strdup(buffer);
				GetDlgItemText(hWndDlg, IDC_E_ALBUM, buffer, MAX_PATH);
				cfg.Tag.album=strdup(buffer);
				GetDlgItemText(hWndDlg, IDC_E_YEAR, buffer, MAX_PATH);
				cfg.Tag.year=strdup(buffer);
				GetDlgItemText(hWndDlg, IDC_CB_GENRE, buffer, MAX_PATH);
				cfg.Tag.genre=strdup(buffer);
				GetDlgItemText(hWndDlg, IDC_E_WRITER, buffer, MAX_PATH);
				cfg.Tag.writer=strdup(buffer);
				GetDlgItemText(hWndDlg, IDC_E_COMMENT, buffer, MAX_PATH);
				cfg.Tag.comment=strdup(buffer);
				GetDlgItemText(hWndDlg, IDC_E_ARTFILE, buffer, MAX_PATH);
				cfg.Tag.artFilename=strdup(buffer);
				cfg.Tag.trackno=GetDlgItemInt(hWndDlg, IDC_E_TRACK, 0, FALSE);
				cfg.Tag.ntracks=GetDlgItemInt(hWndDlg, IDC_E_NTRACKS, 0, FALSE);
				cfg.Tag.discno=GetDlgItemInt(hWndDlg, IDC_E_DISK, 0, FALSE);
				cfg.Tag.ndiscs=GetDlgItemInt(hWndDlg, IDC_E_NDISKS, 0, FALSE);
				cfg.Tag.compilation=(BYTE)GetDlgItemInt(hWndDlg, IDC_E_COMPILATION, 0, FALSE);
				cfg.Tag.compilation=IsDlgButtonChecked(hWndDlg, IDC_CHK_COMPILATION) ? 1 : 0;
				EndDialog(hWndDlg, TRUE);//(DWORD)hCfg);
			}
			break;
			
        case IDCANCEL:
			// Ignore data values entered into the controls        
			// and dismiss the dialog window returning FALSE
			EndDialog(hWndDlg, FALSE);
			break;

		case IDC_BTN_ABOUT:
			DialogBox((HINSTANCE)hInstance,(LPCSTR)MAKEINTRESOURCE(IDD_ABOUT), (HWND)hWndDlg, (DLGPROC)DialogMsgProcAbout);
			break;

		case IDC_BTN_LICENSE:
			{
			char *license =
				"\nPlease note that the use of this software may require the payment of patent royalties.\n"
				"You need to consider this issue before you start building derivative works.\n"
				"We are not warranting or indemnifying you in any way for patent royalities!\n"
				"YOU ARE SOLELY RESPONSIBLE FOR YOUR OWN ACTIONS!\n"
				"\n"
				"FAAC is based on the ISO MPEG-4 reference code. For this code base the\n"
				"following license applies:\n"
				"\n"
/*				"This software module was originally developed by\n"
				"\n"
				"FirstName LastName (CompanyName)\n"
				"\n"
				"and edited by\n"
				"\n"
				"FirstName LastName (CompanyName)\n"
				"FirstName LastName (CompanyName)\n"
				"\n"
*/				"in the course of development of the MPEG-2 NBC/MPEG-4 Audio standard\n"
				"ISO/IEC 13818-7, 14496-1,2 and 3. This software module is an\n"
				"implementation of a part of one or more MPEG-2 NBC/MPEG-4 Audio tools\n"
				"as specified by the MPEG-2 NBC/MPEG-4 Audio standard. ISO/IEC gives\n"
				"users of the MPEG-2 NBC/MPEG-4 Audio standards free license to this\n"
				"software module or modifications thereof for use in hardware or\n"
				"software products claiming conformance to the MPEG-2 NBC/ MPEG-4 Audio\n"
				"standards. Those intending to use this software module in hardware or\n"
				"software products are advised that this use may infringe existing\n"
				"patents. The original developer of this software module and his/her\n"
				"company, the subsequent editors and their companies, and ISO/IEC have\n"
				"no liability for use of this software module or modifications thereof\n"
				"in an implementation. Copyright is not released for non MPEG-2\n"
				"NBC/MPEG-4 Audio conforming products. The original developer retains\n"
				"full right to use the code for his/her own purpose, assign or donate\n"
				"the code to a third party and to inhibit third party from using the\n"
				"code for non MPEG-2 NBC/MPEG-4 Audio conforming products. This\n"
				"copyright notice must be included in all copies or derivative works.\n"
				"\n"
				"Copyright (c) 1997.\n"
				"\n"
				"For the changes made for the FAAC project the GNU Lesser General Public\n"
				"License (LGPL), version 2 1991 applies:\n"
				"\n"
				"FAAC - Freeware Advanced Audio Coder\n"
				"Copyright (C) 2001-2004 The individual contributors\n"
				"\n"
				"This library is free software; you can redistribute it and/or modify it under the terms of\n"
				"the GNU Lesser General Public License as published by the Free Software Foundation;\n"
				"either version 2.1 of the License, or (at your option) any later version.\n"
				"\n"
				"This library is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY;\n"
				"without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.\n"
				"See the GNU Lesser General Public License for more details.\n"
				"\n"
				"You should have received a copy of the GNU Lesser General Public\n"
				"License along with this library; if not, write to the Free Software\n"
				"Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA\n";

				MessageBox(hWndDlg,license,"FAAC libray License",MB_OK|MB_ICONINFORMATION);
			}
			break;

#ifdef USE_OUTPUT_FOLDER
		case IDC_BTN_BROWSE:
			{
			BROWSEINFO bi;
			ITEMIDLIST *idlist;

				GetDlgItemText(hWndDlg, IDC_E_BROWSE, config_AACoutdir, MAX_PATH);
				bi.hwndOwner = hWndDlg;
				bi.pidlRoot = 0;
				bi.pszDisplayName = config_AACoutdir;
				bi.lpszTitle = "Select a directory for aac/mp4 file output:";
				bi.ulFlags = BIF_RETURNONLYFSDIRS;
				bi.lpfn = BrowseCallbackProc;
				bi.lParam = (LPARAM)config_AACoutdir;
				
				idlist = SHBrowseForFolder(&bi);
				if(idlist)
				{
					SHGetPathFromIDList(idlist, config_AACoutdir);
					SetDlgItemText(hWndDlg, IDC_E_BROWSE, config_AACoutdir);
				}
			}
			break;
#endif			
		case IDC_BTN_ARTFILE:
			{
			OPENFILENAME ofn;
			char ArtFilename[MAX_PATH]="";

				GetDlgItemText(hWndDlg, IDC_E_ARTFILE, ArtFilename, MAX_PATH);
				ofn.lStructSize			= sizeof(OPENFILENAME);
				ofn.hwndOwner			= (HWND)hWndDlg;
				ofn.lpstrFilter			= "Cover art files (*.gif,*jpg,*.png)\0*.gif;*.jpg;*.png\0";
				ofn.lpstrCustomFilter	= NULL;
				ofn.nFilterIndex		= 1;
				ofn.lpstrFile			= ArtFilename;
				ofn.nMaxFile			= MAX_PATH; //sizeof ArtFilename;
				ofn.lpstrFileTitle		= NULL;
				ofn.nMaxFileTitle		= 0;
				ofn.lpstrInitialDir		= NULL;
				ofn.lpstrTitle			= "Select cover art file";
				ofn.Flags				= OFN_EXPLORER | OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST | OFN_HIDEREADONLY | OFN_ENABLESIZING;
				ofn.lpstrDefExt			= NULL;//"jpg";
				ofn.hInstance			= hInstance;

				if(GetOpenFileName(&ofn))
					SetDlgItemText(hWndDlg, IDC_E_ARTFILE, ArtFilename);
			}
			break;

#ifdef USE_IMPORT_TAG
		case IDC_BTN_BROWSEIMPORT:
			{
			char path[MAX_PATH];
			BROWSEINFO bi;
			ITEMIDLIST *idlist;

				GetDlgItemText(hWndDlg, IDC_E_SOURCEPATH, path, MAX_PATH);
				bi.hwndOwner = hWndDlg;
				bi.pidlRoot = 0;
				bi.pszDisplayName = path;
				bi.lpszTitle = "Select the folder where source files are stored:";
				bi.ulFlags = BIF_RETURNONLYFSDIRS;
				bi.lpfn = BrowseCallbackProc;
				bi.lParam = (LPARAM)path;
				
				idlist=SHBrowseForFolder(&bi);
				if(idlist)
				{
					SHGetPathFromIDList(idlist, path);
					SetDlgItemText(hWndDlg, IDC_E_SOURCEPATH, path);
				}
			}
			break;
#endif			
		case IDC_RADIO_MPEG4:
			EnableWindow(GetDlgItem(hWndDlg, IDC_RADIO_LTP), !IsDlgButtonChecked(hWndDlg,IDC_CHK_AUTOCFG));
			break;
			
		case IDC_RADIO_MPEG2:
			EnableWindow(GetDlgItem(hWndDlg, IDC_RADIO_LTP), FALSE);
			DISABLE_LTP
			break;

		case IDC_CHK_AUTOCFG:
			{
			BYTE Enabled=IsDlgButtonChecked(hWndDlg,IDC_CHK_AUTOCFG);
				DISABLE_CTRLS_ENC(!Enabled);
				if(!Enabled)
					SendMessage(hWndDlg, WM_INITDIALOG, 0L, 0L);
				else
				{
					SetDlgItemInt(hWndDlg, IDC_CB_QUALITY, DEF_QUALITY, FALSE);
					SetDlgItemInt(hWndDlg, IDC_CB_BITRATE, DEF_BITRATE, FALSE);
					SetDlgItemInt(hWndDlg, IDC_CB_BANDWIDTH, DEF_BANDWIDTH, FALSE);

					CheckDlgButton(hWndDlg,IDC_RADIO_MPEG4,FALSE);
					CheckDlgButton(hWndDlg,IDC_RADIO_MPEG2,FALSE);
					if(DEF_MPEGVER==MPEG4)
						CheckDlgButton(hWndDlg,IDC_RADIO_MPEG4,TRUE);
					else
						CheckDlgButton(hWndDlg,IDC_RADIO_MPEG2,TRUE);
					
					CheckDlgButton(hWndDlg,IDC_RADIO_MAIN,FALSE);
					CheckDlgButton(hWndDlg,IDC_RADIO_LOW,FALSE);
					CheckDlgButton(hWndDlg,IDC_RADIO_SSR,FALSE);
					CheckDlgButton(hWndDlg,IDC_RADIO_LTP,FALSE);
					switch(DEF_PROFILE)
					{
					case MAIN:
						CheckDlgButton(hWndDlg,IDC_RADIO_MAIN,TRUE);
						break;
					case LOW:
						CheckDlgButton(hWndDlg,IDC_RADIO_LOW,TRUE);
						break;
					case SSR:
						CheckDlgButton(hWndDlg,IDC_RADIO_SSR,TRUE);
						break;
					case LTP:
						CheckDlgButton(hWndDlg,IDC_RADIO_LTP,TRUE);
						DISABLE_LTP
						break;
					}
					
					CheckDlgButton(hWndDlg,IDC_RADIO_RAW,FALSE);
					CheckDlgButton(hWndDlg,IDC_RADIO_ADTS,FALSE);
					switch(DEF_HEADER)
					{
					case RAW:
						CheckDlgButton(hWndDlg,IDC_RADIO_RAW,TRUE);
						break;
					case ADTS:
						CheckDlgButton(hWndDlg,IDC_RADIO_ADTS,TRUE);
						break;
					}
					
					CheckDlgButton(hWndDlg, IDC_CHK_ALLOWMIDSIDE, DEF_MIDSIDE);
					CheckDlgButton(hWndDlg, IDC_CHK_USETNS, DEF_TNS);
					CheckDlgButton(hWndDlg, IDC_CHK_USELFE, DEF_LFE);

					if(DEF_USEQUALTY)
						CheckDlgButton(hWndDlg,IDC_RADIO_QUALITY,true);
					else
						CheckDlgButton(hWndDlg,IDC_RADIO_BITRATE,true);

					if(DEF_QUALITY<10)
						SetDlgItemInt(hWndDlg, IDC_CB_QUALITY, 10, FALSE);
					else
						if(DEF_QUALITY>500)
							SetDlgItemInt(hWndDlg, IDC_CB_QUALITY, 500, FALSE);
						else
							SetDlgItemInt(hWndDlg, IDC_CB_QUALITY, DEF_QUALITY, FALSE);

					switch(DEF_BITRATE)
					{
					case 0:
						SendMessage(GetDlgItem(hWndDlg, IDC_CB_BITRATE), CB_SETCURSEL, 0, 0);
//						SetDlgItemInt(hWndDlg, IDC_CB_BITRATE, 128, FALSE);
						break;
					default:
						SetDlgItemInt(hWndDlg, IDC_CB_BITRATE, DEF_BITRATE, FALSE);
						break;
					}
					switch(DEF_BANDWIDTH)
					{
					case 0:
						SendMessage(GetDlgItem(hWndDlg, IDC_CB_BANDWIDTH), CB_SETCURSEL, 0, 0);
						break;
					case 0xffffffff:
						SendMessage(GetDlgItem(hWndDlg, IDC_CB_BANDWIDTH), CB_SETCURSEL, 1, 0);
						break;
					default:
						SetDlgItemInt(hWndDlg, IDC_CB_BANDWIDTH, DEF_BANDWIDTH, FALSE);
						break;
					}
					
					SendMessage(GetDlgItem(hWndDlg, IDC_CB_EXT), CB_SETCURSEL, DEF_WRITEMP4, 0);
//					DISABLE_CTRLS_ENC(!Enabled);
				}
			}
			break;

		case IDC_CHK_TAG:
		case IDC_CB_EXT:
			{
			char TagImport=IsDlgButtonChecked(hWndDlg,IDC_CHK_IMPORTTAG);
			char TagEnabled=IsDlgButtonChecked(hWndDlg,IDC_CHK_TAG);
				ENABLE_TAG(TagEnabled && !TagImport);
			char Enabled=SendMessage(GetDlgItem(hWndDlg, IDC_CB_EXT), CB_GETCURSEL, 0, 0)!=0;
				ENABLE_AACTAGS(TagEnabled && !TagImport && Enabled);
			}
			break;
#ifdef USE_IMPORT_TAG
		case IDC_CHK_IMPORTTAG:
			SendMessage(hWndDlg, WM_COMMAND, IDC_CHK_TAG, 0);
			break;
#endif
		}
		break; // End of WM_COMMAND
	default: 
		return FALSE;
	}
	
	return TRUE;
} // End of DIALOGSMsgProc                                      

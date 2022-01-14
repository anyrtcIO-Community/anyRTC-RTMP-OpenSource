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

#ifndef _Cfaad_H
#define _Cfaad_H

#include <mp4.h>
#define HAVE_INT32_T
#include <faac.h>

#ifdef MAIN
	#undef MAIN
#endif
#ifdef SSR
	#undef SSR
#endif
#ifdef LTP
	#undef LTP
#endif

#include <neaacdec.h>
//#include <faad.h>
extern "C" {
	#include <aacinfo.h>    // get_AAC_format()
}
#include "Defines.h"
#include <CRegistry.h>
#include <CTag.h>



// *********************************************************************************************

// make this higher to support files with more channels
#define MAX_CHANNELS 6
#if FAAD_MIN_STREAMSIZE<2048
#undef FAAD_MIN_STREAMSIZE
// 960 for LD or else 1024 (expanded to 2048 for HE-AAC)
#define FAAD_MIN_STREAMSIZE 2048
#endif

#define	FAAD_STREAMSIZE	(FAAD_MIN_STREAMSIZE*MAX_CHANNELS)

// -----------------------------------------------------------------------------------------------

#define REG_DEFAULT "Default"
#define REG_PROFILE "Profile"
#define REG_SAMPLERATE "SampleRate"
#define REG_BPS "Bps"
#define REG_DOWNMATRIX "Downmatrix"
#define REG_OLDADTS "Old ADTS"
#define REG_DONTUPSAMPLESBR "Don\'t upsample implicit SBR"

// *********************************************************************************************



/* FAAD file buffering routines */
/*typedef struct {
    long bytes_into_buffer;
    long bytes_consumed;
    long file_offset;
    unsigned char *buffer;
    int at_eof;
    FILE *infile;
} aac_buffer;
// -----------------------------------------------------------------------------------------------

typedef struct {
    int version;
    int channels;
    int sampling_rate;
    int bitrate;
    int length;
    int object_type;
    int headertype;
} faadAACInfo;
*/

// *********************************************************************************************

class CMyDecCfg
{
private:

	bool SaveCfgOnDestroy;

public:

	CMyDecCfg(bool SaveOnDestroy=true) { getCfg(this); SaveCfgOnDestroy=SaveOnDestroy; }
	virtual ~CMyDecCfg() { if(SaveCfgOnDestroy) setCfg(this); FreeCfg(this); }

	void FreeCfg(CMyDecCfg *cfg) { cfg->Tag.FreeTag(); }
	void FreeCfg() { this->Tag.FreeTag(); }
	void getCfg(CMyDecCfg *cfg);
	void getCfg() { getCfg(this); }
	void setCfg(CMyDecCfg *cfg);
	void setCfg() { setCfg(this); }

	bool					DefaultCfg;
	BYTE					Channels;
	DWORD					BitRate;
	faacDecConfiguration	DecCfg;
	CMP4Tag					Tag;
};
// -----------------------------------------------------------------------------------------------

typedef struct input_tag // any special vars associated with input file
{
//MP4
MP4FileHandle	mp4File;
MP4SampleId		sampleId,
				numSamples;
int				track;
BYTE			type;

//AAC
FILE			*aacFile;
DWORD			src_size;		// size of compressed file
long			tagsize;
DWORD			bytes_read;		// from file
long			bytes_consumed;	// from buffer by faadDecDecode
long			bytes_into_buffer;
unsigned char	*buffer;
DWORD			*seek_table;
int				seek_table_length;

// Raw AAC
BOOL			FindBitrate;

// GLOBAL
faacDecHandle	hDecoder;
faadAACInfo		file_info;
faacDecFrameInfo	frameInfo;
DWORD			len_ms;			// length of file in milliseconds
BYTE			Channels;
DWORD			Samprate;
WORD			BitsPerSample;
DWORD			dst_size;		// size of decoded file. Cooledit needs it to update its progress bar
//char			*src_name;		// name of compressed file
int				IsMP4;
bool			LockSeeking,
				IsSeekable;
} MYINPUT;
// -----------------------------------------------------------------------------------------------

class Cfaad
{
private:
	virtual int GetAACTrack(MP4FileHandle infile);
	long id3v2_TagSize(unsigned char *buffer);
/*	long id3v2_TagSize(aac_buffer *b);
	int fill_buffer(aac_buffer *b);
	void advance_buffer(aac_buffer *b, int bytes);
	int adts_parse(aac_buffer *b, int *bitrate, float *length);
	void GetAACInfos(aac_buffer *b, DWORD *header_type, float *song_length, int *pbitrate, long filesize);
*/
	virtual void DisplayError(char *ProcName, char *str);
	virtual HANDLE ERROR_getInfos(char *str) { DisplayError("getInfos", str); return NULL; }
	virtual int ERROR_processData(char *str) { DisplayError("processData", str); return 0; }
	virtual void showInfo(MYINPUT *mi) {}
	virtual void showProgress(MYINPUT *mi) {}
	virtual void setDefaultFaadCfg(faacDecHandle hDecoder, BOOL showDlg);
	virtual void setFaadCfg(faacDecHandle hDecoder, CMyDecCfg Cfg);
	virtual int seek(int newpos_bytes) { return 0; } // !=0 => error
	virtual void NoSeek() {} // put here the code to block seeking of player

public:
    Cfaad(HANDLE hInput=NULL);
    virtual ~Cfaad();

	int (*ShowDlg4RawAAC)(); // set this to show your dialog (to decode raw aac files)
	int IsMP4(LPSTR lpstrFilename);
	inline bool CanSeek() { MYINPUT	*mi; GLOBALLOCK(mi,hInput,MYINPUT,return 0); bool IsSeekable=mi->IsSeekable; GlobalUnlock(hInput); return IsSeekable; }
//	virtual void GetAACInfos(char *Filename, aac_buffer *b, DWORD *header_type, float *song_length, int *pbitrate);
    virtual HANDLE getInfos(LPSTR lpstrFilename);
    virtual int processData(HANDLE hInput, unsigned char far *bufout, long lBytes);

// GLOBAL
	long            newpos_ms; // set this to change position
	HANDLE			hInput;
	CMyDecCfg		*pCfg; // set this to use your cfg (to decode raw aac files)
};
#endif

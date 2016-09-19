/*
CTag - Class to read/write id3v2/mp4 tags
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

#ifndef _CTag_H
#define _CTag_H

// *********************************************************************************************

#include <mp4.h>
#include <id3/tag.h>	// id3 tag
#include "CRegistry.h"
#include "Defines.h"

// *********************************************************************************************

#define REG_TAGON "Tag On"
#define REG_TAGIMPORT "Tag import"
#define REG_ARTIST "Tag Artist"
#define REG_TITLE "Tag Title"
#define REG_ALBUM "Tag Album"
#define REG_YEAR "Tag Year"
#define REG_GENRE "Tag Genre"
#define REG_WRITER "Tag Writer"
#define REG_COMMENT "Tag Comment"
#define REG_TRACK "Tag Track"
#define REG_NTRACKS "Tag Tracks"
#define REG_DISK "Tag Disk"
#define REG_NDISKS "Tag Disks"
#define REG_COMPILATION "Tag Compilation"
#define REG_ARTFILE "Tag Art file"

// *********************************************************************************************

typedef struct
{
	char	*data;
	DWORD	size;
	DWORD	pictureType; // front, back, icon, ...
	char	*mimeType, // jpg, png, gif
			*format, // ???
			*description; // text description
} id3Picture;

class CMP4Tag
{
private:
	int check_image_header(const char *buf);
	int ReadCoverArtFile(char *pCoverArtFile, char **artBuf);

public:
	CMP4Tag();
	virtual ~CMP4Tag() { FreeTag(); }

	virtual void FreeTag();
	virtual int WriteMP4Tag(MP4FileHandle MP4File);
	virtual int WriteAacTag(char *Filename);
	virtual int ReadMp4Tag(char *Filename);
	virtual int ReadAacTag(char *Filename);

	char	*copyright; // used in Cfaad
	char	*artist, *title, *album, *year, *genre, *writer, *comment;
	WORD	trackno,ntracks, discno,ndiscs;
	BYTE	compilation;
	char	*artFilename;
	id3Picture art; // used in ReadAacTag(). Remark: field not stored into registry
};

#endif
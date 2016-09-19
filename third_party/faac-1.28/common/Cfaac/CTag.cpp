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

#include <stdlib.h>
#include <mp4.h>
#define HAVE_INT32_T
#include <faac.h>
#include "CTag.h"



// *********************************************************************************************
//										CMP4Tag
// *********************************************************************************************

CMP4Tag::CMP4Tag()
{
//	memset(this,0,sizeof(*this));
	copyright=NULL;
	artist=title=album=year=genre=writer=comment=NULL;
	trackno=ntracks=discno=ndiscs=0;
	compilation=0;
	artFilename=NULL;
	art.pictureType=0; // = other
	memset(&art,0,sizeof(art));
}
// *********************************************************************************************

void CMP4Tag::FreeTag()
{
	FREE_ARRAY(artist);
	FREE_ARRAY(title);
	FREE_ARRAY(album);
	FREE_ARRAY(year);
	FREE_ARRAY(genre);
	FREE_ARRAY(writer);
	FREE_ARRAY(comment);
	FREE_ARRAY(artFilename);
	FREE_ARRAY(art.data);
	FREE_ARRAY(art.description);
	FREE_ARRAY(art.mimeType);
	FREE_ARRAY(art.format);
}
// ***********************************************************************************************

int CMP4Tag::check_image_header(const char *buf)
{
	if(!strncmp(buf, "\x89\x50\x4E\x47\x0D\x0A\x1A\x0A", 8))
		return 1; /* PNG */
	if(!strncmp(buf, "\xFF\xD8\xFF\xE0", 4) &&
		!strncmp(buf + 6, "JFIF\0",	5))
		return 2; /* JPEG */
	if(!strncmp(buf, "GIF87a", 6)	|| !strncmp(buf, "GIF89a", 6))
		return 3; /* GIF */

	return 0;
}
// -----------------------------------------------------------------------------------------------

int CMP4Tag::ReadCoverArtFile(char *pCoverArtFile, char **artData)
{
FILE *artFile;

	if(!pCoverArtFile || !*pCoverArtFile)
		return 0;

	if(!(artFile=fopen(pCoverArtFile, "rb")))
	{
	char buf[25+MAX_PATH+1];
		sprintf(buf,"ReadCoverArtFile: can't open \"%s\"",pCoverArtFile);
		MessageBox(NULL,buf,NULL,MB_OK);
		return 0;
	}

int r;
char *art;
int	artSize=0;

	fseek(artFile, 0, SEEK_END);
	artSize=ftell(artFile);
	fseek(artFile, 0, SEEK_SET);

	if(!(art=(char *)malloc(artSize)))
	{
		fclose(artFile);
		MessageBox(NULL,"ReadCoverArtFile: Memory allocation error!", NULL, MB_OK);
		return 0;
	}

	r=fread(art, 1, artSize, artFile);
	if(r!=artSize)
	{
		free(art);
		fclose(artFile);
		MessageBox(NULL,"ReadCoverArtFile: Error reading cover art file!", NULL, MB_OK);
		return 0;
	}
	else
		if(artSize<12 || !check_image_header(art))
		{
			// the above expression checks the image signature
			free(art);
			fclose(artFile);
			MessageBox(NULL,"ReadCoverArtFile: Unsupported cover image file format!", NULL, MB_OK);
			return 0;
		}

	FREE_ARRAY(*artData);
	*artData=art;
	fclose(artFile);
	return artSize;
}
// *********************************************************************************************

int CMP4Tag::WriteMP4Tag(MP4FileHandle MP4File)
{
char	buf[512], *faac_id_string, *faac_copyright_string;

	if(MP4File==NULL)
	{
		MessageBox(NULL,"WriteMp4Tag: can't open file!",NULL,MB_OK);
		return 1;
	}

	sprintf(buf, "FAAC v%s", (faacEncGetVersion(&faac_id_string, &faac_copyright_string)==FAAC_CFG_VERSION) ? faac_id_string : " wrong libfaac version");
	MP4SetMetadataTool(MP4File, buf);

	if(artist) MP4SetMetadataArtist(MP4File, artist);
	if(writer) MP4SetMetadataWriter(MP4File, writer);
	if(title) MP4SetMetadataName(MP4File, title);
	if(album) MP4SetMetadataAlbum(MP4File, album);
	if(trackno>0) MP4SetMetadataTrack(MP4File, trackno, ntracks);
	if(discno>0) MP4SetMetadataDisk(MP4File, discno, ndiscs);
	if(compilation) MP4SetMetadataCompilation(MP4File, compilation);
	if(year) MP4SetMetadataYear(MP4File, year);
	if(genre) MP4SetMetadataGenre(MP4File, genre);
	if(comment) MP4SetMetadataComment(MP4File, comment);
	if(art.size=ReadCoverArtFile(artFilename,&art.data))
	{
		MP4SetMetadataCoverArt(MP4File, (unsigned __int8 *)art.data, art.size);
		FREE_ARRAY(art.data);
	}
	return 0;
}
// *********************************************************************************************

int CMP4Tag::ReadMp4Tag(char *Filename)						 
{
MP4FileHandle MP4File;

	if(!(MP4File=MP4Read(Filename, 0)))
	{
	char buf[25+MAX_PATH+1];
		sprintf(buf,"ReadMp4Tag: can't open \"%s\"",Filename);
		MessageBox(NULL,buf,NULL,MB_OK);
		return 1;
	}

	FREE_ARRAY(copyright);
	MP4GetMetadataTool(MP4File, &copyright);

	FREE_ARRAY(artist);
	MP4GetMetadataArtist(MP4File, &artist);
	FREE_ARRAY(writer);
	MP4GetMetadataWriter(MP4File, &writer);
	FREE_ARRAY(title);
	MP4GetMetadataName(MP4File, &title);
	FREE_ARRAY(album);
	MP4GetMetadataAlbum(MP4File, &album);
	MP4GetMetadataTrack(MP4File, (unsigned __int16 *)&trackno, (unsigned __int16 *)&ntracks);
	MP4GetMetadataDisk(MP4File, (unsigned __int16 *)&discno, (unsigned __int16 *)&ndiscs);
	MP4GetMetadataCompilation(MP4File, (unsigned __int8 *)&compilation);
	FREE_ARRAY(year);
	MP4GetMetadataYear(MP4File, &year);
	FREE_ARRAY(genre);
	MP4GetMetadataGenre(MP4File, &genre);
	FREE_ARRAY(comment);
	MP4GetMetadataComment(MP4File, &comment);
	FREE_ARRAY(art.data);
	MP4GetMetadataCoverArt(MP4File, (unsigned __int8 **)&art.data, (u_int32_t *)&art.size);

	MP4Close(MP4File);
/*
	FILE *f=fopen("D:\\prova.jpg","wb");
		fwrite(artFile,1,artSize,f);
		fclose(f);*/
	return 0;
}
// *********************************************************************************************

#define DEL_FIELD(id3Tag,ID3FID) \
{ \
ID3_Frame *Frame=id3Tag.Find(ID3FID); \
	if(Frame!=NULL) \
		id3Tag.RemoveFrame(Frame); \
}
// -----------------------------------------------------------------------------------------------

#define ADD_FIELD(id3Tag,ID3FID,ID3FN,data) \
{ \
ID3_Frame *NewFrame=new ID3_Frame(ID3FID); \
	NewFrame->Field(ID3FN)=data; \
	DEL_FIELD(id3Tag,ID3FID); \
	id3Tag.AttachFrame(NewFrame); \
}
// -----------------------------------------------------------------------------------------------

int CMP4Tag::WriteAacTag(char *Filename)
{
char	buf[512], *faac_id_string, *faac_copyright_string;
ID3_Tag id3Tag;
FILE *file;

	if((file=fopen(Filename,"r"))==NULL)
	{
	char buf[25+MAX_PATH+1];
		sprintf(buf,"WriteAacTag: can't open \"%s\"",Filename);
		MessageBox(NULL,buf,NULL,MB_OK);
		return 1;
	}
	else
		fclose(file);
	id3Tag.Link(Filename);

	sprintf(buf, "FAAC v%s", (faacEncGetVersion(&faac_id_string, &faac_copyright_string)==FAAC_CFG_VERSION) ? faac_id_string : " wrong libfaac version");
	ADD_FIELD(id3Tag,ID3FID_ENCODEDBY,ID3FN_TEXT,buf);

	ADD_FIELD(id3Tag,ID3FID_LEADARTIST,ID3FN_TEXT,artist);
	ADD_FIELD(id3Tag,ID3FID_COMPOSER,ID3FN_TEXT,writer);
	ADD_FIELD(id3Tag,ID3FID_TITLE,ID3FN_TEXT,title);
	ADD_FIELD(id3Tag,ID3FID_ALBUM,ID3FN_TEXT,album);
	sprintf(buf,"%d",trackno);
	ADD_FIELD(id3Tag,ID3FID_TRACKNUM,ID3FN_TEXT,buf);
	ADD_FIELD(id3Tag,ID3FID_YEAR,ID3FN_TEXT,year);
	ADD_FIELD(id3Tag,ID3FID_CONTENTTYPE,ID3FN_TEXT,genre);
	ADD_FIELD(id3Tag,ID3FID_COMMENT,ID3FN_TEXT,comment);
	art.size=ReadCoverArtFile(artFilename,&art.data);
	if(art.size)
	{
	ID3_Frame *NewFrame=new ID3_Frame(ID3FID_PICTURE);
	char name[_MAX_FNAME], ext[_MAX_EXT];
		_splitpath(artFilename,NULL,NULL,name,ext);

		NewFrame->Field(ID3FN_DESCRIPTION)=name;
	char buf[15];
		sprintf(buf,"image/%s",check_image_header(art.data)==2 ? "jpeg" : strlwr(ext+1));
		NewFrame->Field(ID3FN_MIMETYPE)=buf;
//		NewFrame->Field(ID3FN_IMAGEFORMAT)=;
		NewFrame->Field(ID3FN_PICTURETYPE)=(DWORD)art.pictureType;
		NewFrame->Field(ID3FN_DATA).Set((BYTE *)art.data,art.size);
		id3Tag.AttachFrame(NewFrame);
	}

	// setup all our rendering parameters
    id3Tag.SetUnsync(false);
    id3Tag.SetExtendedHeader(true);
    id3Tag.SetCompression(true);
    id3Tag.SetPadding(true);
 
	// write any changes to the file
    id3Tag.Update();

	FREE_ARRAY(art.data);
	return 0;
}
// *********************************************************************************************

#define GET_FIELD_STR(id3Tag,ID3FID,ID3FN,data) \
{ \
	Frame=id3Tag.Find(ID3FID); \
	if(Frame!=NULL) \
	{ \
	DWORD size=Frame->Field(ID3FN).Size(); \
		FREE_ARRAY(data); \
		if(data=(char *)malloc(size+1)) \
			Frame->Field(ID3FN).Get(data,size+1); \
	} \
	else \
		FREE_ARRAY(data); \
}
// -----------------------------------------------------------------------------------------------

int CMP4Tag::ReadAacTag(char *Filename)
{
char	*buf=NULL;
ID3_Tag id3Tag;
ID3_Frame *Frame;

	if(!id3Tag.Link(Filename))
	{
	char buf[25+MAX_PATH+1];
		sprintf(buf,"ReadAacTag: can't open \"%s\"",Filename);
		MessageBox(NULL,buf,NULL,MB_OK);
		return 1;
	}

	GET_FIELD_STR(id3Tag,ID3FID_ENCODEDBY,ID3FN_TEXT,copyright);

	GET_FIELD_STR(id3Tag,ID3FID_LEADARTIST,ID3FN_TEXT,artist);
	GET_FIELD_STR(id3Tag,ID3FID_COMPOSER,ID3FN_TEXT,writer);
	GET_FIELD_STR(id3Tag,ID3FID_TITLE,ID3FN_TEXT,title);
	GET_FIELD_STR(id3Tag,ID3FID_ALBUM,ID3FN_TEXT,album);

	GET_FIELD_STR(id3Tag,ID3FID_TRACKNUM,ID3FN_TEXT,buf);
	if(buf)
		trackno=atoi(buf);
	FREE_ARRAY(buf);
	GET_FIELD_STR(id3Tag,ID3FID_YEAR,ID3FN_TEXT,year);
	GET_FIELD_STR(id3Tag,ID3FID_CONTENTTYPE,ID3FN_TEXT,genre);
	GET_FIELD_STR(id3Tag,ID3FID_COMMENT,ID3FN_TEXT,comment);

	if(Frame=id3Tag.Find(ID3FID_PICTURE))
	{
		art.size=Frame->Field(ID3FN_DATA).Size();
		FREE_ARRAY(art.data);
		if(art.data=(char *)malloc(art.size))
			memcpy(art.data,Frame->Field(ID3FN_DATA).GetBinary(),art.size);

		GET_FIELD_STR(id3Tag,ID3FID_PICTURE,ID3FN_MIMETYPE,art.mimeType);
		GET_FIELD_STR(id3Tag,ID3FID_PICTURE,ID3FN_DESCRIPTION,art.description);
		GET_FIELD_STR(id3Tag,ID3FID_PICTURE,ID3FN_IMAGEFORMAT,art.format);
		art.pictureType=Frame->Field(ID3FN_PICTURETYPE).Get();
/*
	FILE *f=fopen("D:\\prova.jpg","wb");
		fwrite(artFile,1,artSize,f);
		fclose(f);*/
	}
	return 0;
}
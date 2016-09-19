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

#include "Cfaac.h"



// *********************************************************************************************
//										CMyEncCfg
// *********************************************************************************************

void CMyEncCfg::getCfg(CMyEncCfg *cfg)
{ 
CRegistry reg;

	if(reg.OpenCreate(HKEY_CURRENT_USER, REGISTRY_PROGRAM_NAME "\\FAAC"))
	{
		cfg->AutoCfg=reg.GetSetBool(REG_AUTO,DEF_AUTO);
		cfg->SaveMP4=reg.GetSetByte(REG_WRITEMP4,DEF_WRITEMP4);
		cfg->EncCfg.mpegVersion=reg.GetSetDword(REG_MPEGVER,DEF_MPEGVER); 
		cfg->EncCfg.aacObjectType=reg.GetSetDword(REG_PROFILE,DEF_PROFILE); 
		cfg->EncCfg.allowMidside=reg.GetSetDword(REG_MIDSIDE,DEF_MIDSIDE); 
		cfg->EncCfg.useTns=reg.GetSetDword(REG_TNS,DEF_TNS); 
		cfg->EncCfg.useLfe=reg.GetSetDword(REG_LFE,DEF_LFE);
		cfg->UseQuality=reg.GetSetBool(REG_USEQUALTY,DEF_USEQUALTY);
		cfg->EncCfg.quantqual=reg.GetSetDword(REG_QUALITY,DEF_QUALITY); 
		cfg->EncCfg.bitRate=reg.GetSetDword(REG_BITRATE,DEF_BITRATE); 
		cfg->EncCfg.bandWidth=reg.GetSetDword(REG_BANDWIDTH,DEF_BANDWIDTH); 
		cfg->EncCfg.outputFormat=reg.GetSetDword(REG_HEADER,DEF_HEADER); 

		FREE_ARRAY(cfg->OutDir);
		cfg->OutDir=reg.GetSetStr(REG_OutFolder,"");

		cfg->TagOn=reg.GetSetByte(REG_TAGON,0);
#ifdef USE_IMPORT_TAG
		cfg->TagImport=reg.GetSetByte(REG_TAGIMPORT,0);
	#ifdef USE_PATHEXT
		FREE_ARRAY(cfg->TagSrcPath);
		FREE_ARRAY(cfg->TagSrcExt);
		cfg->TagSrcPath=reg.GetSetStr(REG_InFolder,"");
		cfg->TagSrcExt=reg.GetSetStr(REG_SrcExt,"");
	#endif
#endif
		cfg->Tag.artist=reg.GetSetStr(REG_ARTIST,"");
		cfg->Tag.title=reg.GetSetStr(REG_TITLE,"");
		cfg->Tag.album=reg.GetSetStr(REG_ALBUM,"");
		cfg->Tag.year=reg.GetSetStr(REG_YEAR,"");
		cfg->Tag.genre=reg.GetSetStr(REG_GENRE,"");
		cfg->Tag.writer=reg.GetSetStr(REG_WRITER,"");
		cfg->Tag.comment=reg.GetSetStr(REG_COMMENT,"");
		cfg->Tag.trackno=reg.GetSetWord(REG_TRACK,0);
		cfg->Tag.ntracks=reg.GetSetWord(REG_NTRACKS,0);
		cfg->Tag.discno=reg.GetSetWord(REG_DISK,0);
		cfg->Tag.ndiscs=reg.GetSetWord(REG_NDISKS,0);
		cfg->Tag.compilation=reg.GetSetByte(REG_COMPILATION,0);
		cfg->Tag.artFilename=reg.GetSetStr(REG_ARTFILE,"");
	}
	else
		MessageBox(0,"Can't open registry!",0,MB_OK|MB_ICONSTOP);
}
// -----------------------------------------------------------------------------------------------

void CMyEncCfg::setCfg(CMyEncCfg *cfg)
{ 
CRegistry reg;

	if(reg.OpenCreate(HKEY_CURRENT_USER, REGISTRY_PROGRAM_NAME "\\FAAC"))
	{
		reg.SetBool(REG_AUTO,cfg->AutoCfg);
		reg.SetByte(REG_WRITEMP4,cfg->SaveMP4);
		reg.SetDword(REG_MPEGVER,cfg->EncCfg.mpegVersion);
		reg.SetDword(REG_PROFILE,cfg->EncCfg.aacObjectType);
		reg.SetDword(REG_MIDSIDE,cfg->EncCfg.allowMidside);
		reg.SetDword(REG_TNS,cfg->EncCfg.useTns);
		reg.SetDword(REG_LFE,cfg->EncCfg.useLfe);
		reg.SetBool(REG_USEQUALTY,cfg->UseQuality);
		reg.SetDword(REG_QUALITY,cfg->EncCfg.quantqual);
		reg.SetDword(REG_BITRATE,cfg->EncCfg.bitRate);
		reg.SetDword(REG_BANDWIDTH,cfg->EncCfg.bandWidth);
		reg.SetDword(REG_HEADER,cfg->EncCfg.outputFormat);

		reg.SetStr(REG_OutFolder,cfg->OutDir);

		reg.SetByte(REG_TAGON,cfg->TagOn);
#ifdef USE_IMPORT_TAG
		reg.SetByte(REG_TAGIMPORT,cfg->TagImport);
	#ifdef USE_PATHEXT
		reg.SetStr(REG_InFolder,cfg->TagSrcPath);
		reg.SetStr(REG_SrcExt,cfg->TagSrcExt);
	#endif
#endif
		reg.SetStr(REG_ARTIST,cfg->Tag.artist);
		reg.SetStr(REG_TITLE,cfg->Tag.title);
		reg.SetStr(REG_ALBUM,cfg->Tag.album);
		reg.SetStr(REG_YEAR,cfg->Tag.year);
		reg.SetStr(REG_GENRE,cfg->Tag.genre);
		reg.SetStr(REG_WRITER,cfg->Tag.writer);
		reg.SetStr(REG_COMMENT,cfg->Tag.comment);
		reg.SetWord(REG_TRACK,cfg->Tag.trackno);
		reg.SetWord(REG_NTRACKS,cfg->Tag.ntracks);
		reg.SetWord(REG_DISK,cfg->Tag.discno);
		reg.SetWord(REG_NDISKS,cfg->Tag.ndiscs);
		reg.SetByte(REG_COMPILATION,cfg->Tag.compilation);
		reg.SetStr(REG_ARTFILE,cfg->Tag.artFilename);
	}
	else
		MessageBox(0,"Can't open registry!",0,MB_OK|MB_ICONSTOP);
}

// *********************************************************************************************
//											Cfaac
// *********************************************************************************************



Cfaac::Cfaac(HANDLE hOut)
{
	if(hOut)
	{
		hOutput=hOut;
		return;
	}

    if(!(hOutput=GlobalAlloc(GMEM_MOVEABLE|GMEM_SHARE|GMEM_ZEROINIT,sizeof(MYOUTPUT))))
	{
		MessageBox(0, "Memory allocation error: hOutput", APP_NAME " plugin", MB_OK|MB_ICONSTOP); \
		return;
	}
//	dontAskGetFilename=false;
}
// -----------------------------------------------------------------------------------------------

Cfaac::~Cfaac()
{
	if(!hOutput)
		return;

MYOUTPUT *mo;

	GLOBALLOCK(mo,hOutput,MYOUTPUT,return);
	
	if(mo->WrittenSamples)
	{
	int	BytesWritten;
		if(mo->bytes_into_buffer>0)
			memset(mo->bufIn+mo->bytes_into_buffer, 0, (mo->samplesInput*(mo->BitsPerSample>>3))-mo->bytes_into_buffer);
		do
		{
			if((BytesWritten=processData(hOutput,mo->bufIn,mo->bytes_into_buffer))<0)
				MessageBox(0, "~Cfaac: processData", APP_NAME " plugin", MB_OK|MB_ICONSTOP);
			mo->bytes_into_buffer=0;
		}while(BytesWritten>0);
	}

	if(mo->aacFile)
	{
		fclose(mo->aacFile);
		mo->aacFile=0;

	CMyEncCfg cfg(false);
		if(cfg.TagOn && mo->OutFilename)
		{
		int error=0;
#ifdef USE_IMPORT_TAG
			if(cfg.TagImport && mo->InFilename)
			{
			int l=strlen(mo->InFilename);
				if(	!strcmpi(mo->InFilename+l-4,".mp4") ||
					!strcmpi(mo->InFilename+l-4,".m4a") ||
					!strcmpi(mo->InFilename+l-4,".m4b"))
					error=cfg.Tag.ReadMp4Tag(mo->InFilename);
				else
					error=cfg.Tag.ReadAacTag(mo->InFilename);
			}
#endif
			if(!error)
				cfg.Tag.WriteAacTag(mo->OutFilename);
		}
	}
	else
	{
		MP4Close(mo->MP4File);
		mo->MP4File=0;
	}
	
	if(mo->hEncoder)
		faacEncClose(mo->hEncoder);
	
	FREE_ARRAY(mo->bitbuf)
	FREE_ARRAY(mo->buf32bit)
	FREE_ARRAY(mo->bufIn)

	FREE_ARRAY(mo->OutFilename)
	FREE_ARRAY(mo->InFilename)

	GlobalUnlock(hOutput);
	GlobalFree(hOutput);
}

// *********************************************************************************************
//									Utilities
// *********************************************************************************************

char *Cfaac::getSourceFilename(char *path, char *src, char *ext)
{
char	*dst, chExt,
		*pname, *pext;


	if(!src)
		return NULL;
	if(!path)
		path="";
	if(!ext)
		ext="";
	pname=pext=src+strlen(src);

	while(pname!=src && *pname!='\\' && *pname!='/')
		pname--;
	if(*pname=='\\' || *pname=='/')
		pname++;
	while(pext!=src && *pext!='.')
		pext--;
	chExt=*pext;
	if(chExt=='.')
		*pext='\0';

	if(*path)
	{
	int l;
		if(!(dst=(char *)malloc(strlen(path)+strlen(pname)+strlen(ext)+3))) // '\0','\\','.'
			return dst;
		l=strlen(path);
		if(l>0 && path[l-1]=='\\')
			sprintf(dst,"%s%s.%s",path,pname,ext);
		else
			sprintf(dst,"%s\\%s.%s",path,pname,ext);
	}
	else
	{
		if(!(dst=(char *)malloc(strlen(pname)+strlen(ext)+2)))
			return dst;
		sprintf(dst,"%s.%s",pname,ext);
	}
	*pext=chExt;

	if(!strcmpi(src,dst))
	{
//		if(!dontAskGetFilename)
		{
		char buf[MAX_PATH+100];
			sprintf(buf,"%s\nSource file=Destination file...aborting!\n\n\tSuppress this warning?",dst);
//			if(MessageBox(NULL,buf,NULL,MB_YESNO)==IDYES)
//				dontAskGetFilename=true;
		}
		FREE_ARRAY(dst);
		return NULL;
	}

	return dst;
}
// *********************************************************************************************

#define SWAP32(x) (((x & 0xff) << 24) | ((x & 0xff00) << 8) \
	| ((x & 0xff0000) >> 8) | ((x & 0xff000000) >> 24))
#define SWAP16(x) (((x & 0xff) << 8) | ((x & 0xff00) >> 8))

void Cfaac::To32bit(int32_t *buf, BYTE *bufi, int size, BYTE samplebytes, BYTE bigendian)
{
int i;

	switch(samplebytes)
	{
	case 1:
		// this is endian clean
		for (i = 0; i < size; i++)
			buf[i] = (bufi[i] - 128) * 65536;
		break;
		
	case 2:
#ifdef WORDS_BIGENDIAN
		if (!bigendian)
#else
			if (bigendian)
#endif
			{
				// swap bytes
				for (i = 0; i < size; i++)
				{
					int16_t s = ((int16_t *)bufi)[i];
					
					s = SWAP16(s);
					
					buf[i] = ((u_int32_t)s) << 8;
				}
			}
			else
			{
				// no swap
				for (i = 0; i < size; i++)
				{
					int s = ((int16_t *)bufi)[i];
					
					buf[i] = s << 8;
				}
			}
			break;
			
	case 3:
		if (!bigendian)
		{
			for (i = 0; i < size; i++)
			{
				int s = bufi[3 * i] | (bufi[3 * i + 1] << 8) | (bufi[3 * i + 2] << 16);
				
				// fix sign
				if (s & 0x800000)
					s |= 0xff000000;
				
				buf[i] = s;
			}
		}
		else // big endian input
		{
			for (i = 0; i < size; i++)
			{
				int s = (bufi[3 * i] << 16) | (bufi[3 * i + 1] << 8) | bufi[3 * i + 2];
				
				// fix sign
				if (s & 0x800000)
					s |= 0xff000000;
				
				buf[i] = s;
			}
		}
		break;
		
	case 4:		
#ifdef WORDS_BIGENDIAN
		if (!bigendian)
#else
			if (bigendian)
#endif
			{
				// swap bytes
				for (i = 0; i < size; i++)
				{
					int s = bufi[i];
					
					buf[i] = SWAP32(s);
				}
			}
			else
				memcpy(buf,bufi,size*sizeof(u_int32_t));
		/*
		int exponent, mantissa;
		float *bufo=(float *)buf;
			
			for (i = 0; i < size; i++)
			{
				exponent=bufi[(i<<2)+3]<<1;
				if(bufi[i*4+2] & 0x80)
					exponent|=0x01;
				exponent-=126;
				mantissa=(DWORD)bufi[(i<<2)+2]<<16;
				mantissa|=(DWORD)bufi[(i<<2)+1]<<8;
				mantissa|=bufi[(i<<2)];
				bufo[i]=(float)ldexp(mantissa,exponent);
			}*/
			break;
	}
}

// *********************************************************************************************
//									Main functions
// *********************************************************************************************

void Cfaac::DisplayError(char *ProcName, char *str)
{
char buf[100]="";

	if(str && *str)
	{
		if(ProcName && *ProcName)
			sprintf(buf,"%s: ", ProcName);
		strcat(buf,str);
		MessageBox(0, buf, APP_NAME " plugin", MB_OK|MB_ICONSTOP);
	}

MYOUTPUT *mo;
	GLOBALLOCK(mo,hOutput,MYOUTPUT,return);
	mo->bytes_into_buffer=-1;
	GlobalUnlock(hOutput);
	GlobalUnlock(hOutput);
}
// *********************************************************************************************

HANDLE Cfaac::Init(LPSTR InFileName, LPSTR OutFileName,long lSamprate,WORD wBitsPerSample,WORD wChannels,long FileSize)
{
MYOUTPUT	*mo;
CMyEncCfg	cfg(false);
DWORD		samplesInput,
			maxBytesOutput;

//	if(wBitsPerSample!=8 && wBitsPerSample!=16) // 32 bit audio from cooledit is in unsupported format
//		return 0;
	if(wChannels>48)	// FAAC supports max 48 tracks!
		return NULL;

	GLOBALLOCK(mo,hOutput,MYOUTPUT,return NULL);

	// open the encoder library
	if(!(mo->hEncoder=faacEncOpen(lSamprate, wChannels, &samplesInput, &maxBytesOutput)))
		return ERROR_Init("Can't open library");

	if(!(mo->bitbuf=(unsigned char *)malloc(maxBytesOutput*sizeof(unsigned char))))
		return ERROR_Init("Memory allocation error: output buffer");

	if(!(mo->bufIn=(BYTE *)malloc(samplesInput*sizeof(int32_t))))
		return ERROR_Init("Memory allocation error: input buffer");

	if(!(mo->buf32bit=(int32_t *)malloc(samplesInput*sizeof(int32_t))))
		return ERROR_Init("Memory allocation error: 32 bit buffer");

	if(cfg.SaveMP4)// || cfg.Tag.On)
	{
	int ExtPos=0, // append new ext
		fnLen=lstrlen(OutFileName);
//		if(OutFileName[lstrlen(OutFileName)-4]=='.')
		if(	fnLen>=4 &&
			(!strcmpi(OutFileName+fnLen-4,".aac") ||
			!strcmpi(OutFileName+fnLen-4,".mp4") ||
			!strcmpi(OutFileName+fnLen-4,".m4a") ||
			!strcmpi(OutFileName+fnLen-4,".m4b"))) // no aac/mp4 ext => append new ext
			if(	(cfg.SaveMP4==1 && strcmpi(OutFileName+fnLen-4,".mp4")) ||
				(cfg.SaveMP4==2 && strcmpi(OutFileName+fnLen-4,".m4a")) ||
				(cfg.SaveMP4==3 && strcmpi(OutFileName+fnLen-4,".m4b")))
				ExtPos=4; // wrong ext => replace it
			else
				ExtPos=-1; // correct ext => no action
		if(ExtPos!=-1)
		{
			switch(cfg.SaveMP4)
			{
			case 1:	strcpy(OutFileName+fnLen-ExtPos,".mp4"); break;
			case 2: strcpy(OutFileName+fnLen-ExtPos,".m4a"); break;
			case 3: strcpy(OutFileName+fnLen-ExtPos,".m4b"); break;
			}
		FILE *f=fopen(OutFileName,"rb");
			if(f)
			{
			char buf[MAX_PATH+20];
				sprintf(buf,"Overwrite \"%s\" ?",OutFileName);
				fclose(f);
				if(MessageBox(NULL,buf,"File already exists!",MB_YESNO|MB_ICONQUESTION)==IDNO)
					return ERROR_Init(0);//"User abort");
			}
		}
	}
	mo->WriteMP4=	!strcmpi(OutFileName+lstrlen(OutFileName)-4,".mp4") ||
					!strcmpi(OutFileName+lstrlen(OutFileName)-4,".m4a") ||
					!strcmpi(OutFileName+lstrlen(OutFileName)-4,".m4b");

faacEncConfigurationPtr CurFormat=faacEncGetCurrentConfiguration(mo->hEncoder);
	CurFormat->inputFormat=FAAC_INPUT_32BIT;
/*	switch(wBitsPerSample)
	{
	case 16:
		CurFormat->inputFormat=FAAC_INPUT_16BIT;
		break;
	case 24:
		CurFormat->inputFormat=FAAC_INPUT_24BIT;
		break;
	case 32:
		CurFormat->inputFormat=FAAC_INPUT_32BIT;
		break;
	default:
		CurFormat->inputFormat=FAAC_INPUT_NULL;
		break;
	}*/
	if(!cfg.AutoCfg)
	{
	faacEncConfigurationPtr myFormat=&cfg.EncCfg;

		if(cfg.UseQuality)
		{
			CurFormat->quantqual=myFormat->quantqual;
			CurFormat->bitRate=0;//myFormat->bitRate;
		}
		else
		{
			CurFormat->bitRate=(myFormat->bitRate*1000)/wChannels;
			CurFormat->quantqual=100;
		}

		switch(CurFormat->bandWidth)
		{
		case 0: // Auto
			break;
		case 0xffffffff: // Full
			CurFormat->bandWidth=lSamprate/2;
			break;
		default:
			CurFormat->bandWidth=myFormat->bandWidth;
			break;
		}
		CurFormat->mpegVersion=myFormat->mpegVersion;
		CurFormat->outputFormat=myFormat->outputFormat;
		CurFormat->mpegVersion=myFormat->mpegVersion;
		CurFormat->aacObjectType=myFormat->aacObjectType;
		CurFormat->allowMidside=myFormat->allowMidside;
//		CurFormat->useTns=myFormat->useTns;
		CurFormat->useTns=false;
	}
	else
	{
		CurFormat->mpegVersion=DEF_MPEGVER;
		CurFormat->aacObjectType=DEF_PROFILE;
		CurFormat->allowMidside=DEF_MIDSIDE;
//		CurFormat->useTns=DEF_TNS;
		CurFormat->useTns=false;
		CurFormat->useLfe=DEF_LFE;
		CurFormat->quantqual=DEF_QUALITY;
		CurFormat->bitRate=DEF_BITRATE;
		CurFormat->bandWidth=DEF_BANDWIDTH;
		CurFormat->outputFormat=DEF_HEADER;
	}
	if(mo->WriteMP4)
		CurFormat->outputFormat=RAW;
	CurFormat->useLfe=wChannels>=6 ? 1 : 0;
	if(!faacEncSetConfiguration(mo->hEncoder, CurFormat))
		return ERROR_Init("Unsupported parameters!");

//	mo->src_size=lSize;
//	mi->dst_name=strdup(OutFileName);
	mo->Samprate=lSamprate;
	mo->BitsPerSample=wBitsPerSample;
	mo->Channels=wChannels;
	mo->samplesInput=samplesInput;
	mo->samplesInputSize=samplesInput*(mo->BitsPerSample>>3);

	mo->maxBytesOutput=maxBytesOutput;

    if(mo->WriteMP4) // Create MP4 file --------------------------------------------------------------------------
	{
    BYTE *ASC=0;
    DWORD ASCLength=0;

        if((mo->MP4File=MP4Create(OutFileName, 0, 0))==MP4_INVALID_FILE_HANDLE)
			return ERROR_Init("Can't create file");
        MP4SetTimeScale(mo->MP4File, 90000);
        mo->MP4track=MP4AddAudioTrack(mo->MP4File, lSamprate, MP4_INVALID_DURATION, MP4_MPEG4_AUDIO_TYPE);
        MP4SetAudioProfileLevel(mo->MP4File, 0x0F);
        faacEncGetDecoderSpecificInfo(mo->hEncoder, &ASC, &ASCLength);
        MP4SetTrackESConfiguration(mo->MP4File, mo->MP4track, (unsigned __int8 *)ASC, ASCLength);
		mo->frameSize=samplesInput/wChannels;
		mo->ofs=mo->frameSize;

		if(cfg.TagOn)
		{
		int error=0;
#ifdef USE_IMPORT_TAG
			if(cfg.TagImport && InFileName)
			{
			int l=strlen(InFileName);
				if(	!strcmpi(InFileName+l-4,".mp4") ||
					!strcmpi(InFileName+l-4,".m4a") ||
					!strcmpi(InFileName+l-4,".m4b"))
					error=cfg.Tag.ReadMp4Tag(InFileName);
				else
					error=cfg.Tag.ReadAacTag(InFileName);
			}
#endif
			if(!error)
				cfg.Tag.WriteMP4Tag(mo->MP4File);
		}
	}
	else // Create AAC file -----------------------------------------------------------------------------
	{
		// open the aac output file 
		if(!(mo->aacFile=fopen(OutFileName, "wb")))
			return ERROR_Init("Can't create file");

		// use bufferized stream
		setvbuf(mo->aacFile,NULL,_IOFBF,32767);

		mo->InFilename=strdup(InFileName);
		mo->OutFilename=strdup(OutFileName);
	}

	showInfo(mo);

	GlobalUnlock(hOutput);
    return hOutput;
}
// *********************************************************************************************

int Cfaac::processData(HANDLE hOutput, BYTE *bufIn, DWORD len)
{
	if(!hOutput)
		return -1;

int bytesWritten=0;
int bytesEncoded;
MYOUTPUT far *mo;

	GLOBALLOCK(mo,hOutput,MYOUTPUT,return 0);

int32_t *buf=mo->buf32bit;

	if((int)len<mo->samplesInputSize)
	{
		mo->samplesInput=(len<<3)/mo->BitsPerSample;
		mo->samplesInputSize=mo->samplesInput*(mo->BitsPerSample>>3);
	}
//	if(mo->BitsPerSample==8 || mo->BitsPerSample==32)
		To32bit(buf,bufIn,mo->samplesInput,mo->BitsPerSample>>3,false);

	// call the actual encoding routine
	if((bytesEncoded=faacEncEncode(mo->hEncoder, (int32_t *)buf, mo->samplesInput, mo->bitbuf, mo->maxBytesOutput))<0)
		return ERROR_processData("faacEncEncode()");

	// write bitstream to aac file 
	if(mo->aacFile)
	{
		if(bytesEncoded>0)
		{
			if((bytesWritten=fwrite(mo->bitbuf, 1, bytesEncoded, mo->aacFile))!=bytesEncoded)
				return ERROR_processData("Write failed!");
			mo->WrittenSamples=1; // needed into destructor
		}
	}
	else
	// write bitstream to mp4 file
	{
	MP4Duration dur,
				SamplesLeft;
		if(len>0)
		{
			mo->srcSize+=len;
			dur=mo->frameSize;
		}
		else
		{
			mo->TotalSamples=(mo->srcSize<<3)/(mo->BitsPerSample*mo->Channels);
			SamplesLeft=(mo->TotalSamples-mo->WrittenSamples)+mo->frameSize;
			dur=SamplesLeft>mo->frameSize ? mo->frameSize : SamplesLeft;
		}
		if(bytesEncoded>0)
		{
			if(!(bytesWritten=MP4WriteSample(mo->MP4File, mo->MP4track, (unsigned __int8 *)mo->bitbuf, (DWORD)bytesEncoded, dur, mo->ofs, true) ? bytesEncoded : -1))
				return ERROR_processData("MP4WriteSample()");
			mo->ofs=0;
			mo->WrittenSamples+=dur;
		}
	}

	showProgress(mo);

	GlobalUnlock(hOutput);
	return bytesWritten;
}
// -----------------------------------------------------------------------------------------------

int Cfaac::processDataBufferized(HANDLE hOutput, BYTE *bufIn, long lBytes)
{
	if(!hOutput)
		return -1;

int	bytesWritten=0, tot=0;
MYOUTPUT far *mo;

	GLOBALLOCK(mo,hOutput,MYOUTPUT,return 0);

	if(mo->bytes_into_buffer>=0)
		do
		{
			if(mo->bytes_into_buffer+lBytes<mo->samplesInputSize)
			{
				memmove(mo->bufIn+mo->bytes_into_buffer, bufIn, lBytes);
				mo->bytes_into_buffer+=lBytes;
				lBytes=0;
			}
			else
			{
			int	shift=mo->samplesInputSize-mo->bytes_into_buffer;
				memmove(mo->bufIn+mo->bytes_into_buffer, bufIn, shift);
				mo->bytes_into_buffer+=shift;
				bufIn+=shift;
				lBytes-=shift;

				tot+=bytesWritten=processData(hOutput,mo->bufIn,mo->bytes_into_buffer);
				if(bytesWritten<0)
					return ERROR_processData(0);
				mo->bytes_into_buffer=0;
			}
		}while(lBytes);

	GlobalUnlock(hOutput);
	return tot;
}

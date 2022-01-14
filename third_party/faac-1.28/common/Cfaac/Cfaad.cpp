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

#include "Cfaad.h"



// *********************************************************************************************



Cfaad::Cfaad(HANDLE hIn)
{
	ShowDlg4RawAAC=NULL;
	pCfg=NULL;

	if(hIn)
	{
		hInput=hIn;
		return;
	}

MYINPUT *mi;

	if(!(hInput=GlobalAlloc(GMEM_MOVEABLE|GMEM_SHARE|GMEM_ZEROINIT,sizeof(MYINPUT))))
		MessageBox(0, "Memory allocation error: hInput", APP_NAME " plugin", MB_OK|MB_ICONSTOP); \
	if(!(mi=(MYINPUT *)GlobalLock(hInput)))
		MessageBox(0, "GlobalLock(hInput)", APP_NAME " plugin", MB_OK|MB_ICONSTOP); \
/*
	mi->mp4File=0;
	mi->aacFile=0;
    mi->hDecoder=0;
    mi->buffer=0;
	mi->bytes_read=0;*/
	mi->BitsPerSample=16;
	mi->FindBitrate=FALSE;
	newpos_ms=-1;
	mi->seek_table=NULL;
	mi->seek_table_length=0;
	mi->LockSeeking=false;
	GlobalUnlock(hInput);
}
// -----------------------------------------------------------------------------------------------

Cfaad::~Cfaad()
{
MYINPUT *mi;

	if(!hInput)
		return;

	GLOBALLOCK(mi,hInput,MYINPUT,return);

	if(mi->mp4File)
		MP4Close(mi->mp4File);
	if(mi->aacFile)
		fclose(mi->aacFile);
	if(mi->hDecoder)
		faacDecClose(mi->hDecoder);
	FREE_ARRAY(mi->buffer);
	FREE_ARRAY(mi->seek_table);

	GlobalUnlock(hInput);
	GlobalFree(hInput);
}

// *********************************************************************************************
//									Utilities
// *********************************************************************************************

int Cfaad::GetAACTrack(MP4FileHandle infile)
{
// find AAC track
int i, rc;
int numTracks = MP4GetNumberOfTracks(infile, NULL, 0);

	for (i = 0; i < numTracks; i++)
    {
    MP4TrackId trackId = MP4FindTrackId(infile, i, NULL, 0);
    const char* trackType = MP4GetTrackType(infile, trackId);

        if (!strcmp(trackType, MP4_AUDIO_TRACK_TYPE))
        {
        unsigned char *buff = NULL;
        unsigned __int32 buff_size = 0;
        mp4AudioSpecificConfig mp4ASC;

			MP4GetTrackESConfiguration(infile, trackId, (unsigned __int8 **)&buff, &buff_size);

            if (buff)
            {
                rc = AudioSpecificConfig(buff, buff_size, &mp4ASC);
                free(buff);

                if (rc < 0)
                    return -1;
                return trackId;
            }
        }
    }

    // can't decode this
    return -1;
}
// *********************************************************************************************

int Cfaad::IsMP4(LPSTR lpstrFilename)
{
DWORD	mp4file = 0;
FILE	*hMP4File = fopen(lpstrFilename, "rb");
BYTE	header[8];
    if(!hMP4File)
		return -1;
    fread(header, 1, 8, hMP4File);
    fclose(hMP4File);
    if(header[4]=='f' && header[5]=='t' && header[6]=='y' && header[7]=='p')
		return 1;

	return 0;
}
// *********************************************************************************************
long Cfaad::id3v2_TagSize(unsigned char *buffer)
{
	if(StringComp((const char *)buffer, "ID3", 3) == 0)
	{
	unsigned long tagsize;

	// high bit is not used
		tagsize =	(buffer[6] << 21) | (buffer[7] << 14) |
					(buffer[8] <<  7) | (buffer[9] <<  0);
		tagsize += 10;
		return tagsize;
	}
	return 0;
}
/*
long Cfaad::id3v2_TagSize(aac_buffer *b)
{
DWORD    tagsize = 0;
    if (!memcmp(b->buffer, "ID3", 3))
    {
        // high bit is not used
        tagsize = (b->buffer[6] << 21) | (b->buffer[7] << 14) |
            (b->buffer[8] <<  7) | (b->buffer[9] <<  0);

        tagsize += 10;
        advance_buffer(b, tagsize);
        fill_buffer(b);
    }
	return tagsize;
}
// *********************************************************************************************

int Cfaad::fill_buffer(aac_buffer *b)
{
    int bread;

    if (b->bytes_consumed > 0)
    {
        if (b->bytes_into_buffer)
        {
            memmove((void*)b->buffer, (void*)(b->buffer + b->bytes_consumed),
                b->bytes_into_buffer*sizeof(unsigned char));
        }

        if (!b->at_eof)
        {
            bread = fread((void*)(b->buffer + b->bytes_into_buffer), 1,
                b->bytes_consumed, b->infile);

            if (bread != b->bytes_consumed)
                b->at_eof = 1;

            b->bytes_into_buffer += bread;
        }

        b->bytes_consumed = 0;

        if (b->bytes_into_buffer > 3)
        {
            if (memcmp(b->buffer, "TAG", 3) == 0)
                b->bytes_into_buffer = 0;
        }
        if (b->bytes_into_buffer > 11)
        {
            if (memcmp(b->buffer, "LYRICSBEGIN", 11) == 0)
                b->bytes_into_buffer = 0;
        }
        if (b->bytes_into_buffer > 8)
        {
            if (memcmp(b->buffer, "APETAGEX", 8) == 0)
                b->bytes_into_buffer = 0;
        }
    }

    return 1;
}
// *********************************************************************************************

void Cfaad::advance_buffer(aac_buffer *b, int bytes)
{
    b->file_offset += bytes;
    b->bytes_consumed = bytes;
    b->bytes_into_buffer -= bytes;
}
// *********************************************************************************************

static int adts_sample_rates[] = {96000,88200,64000,48000,44100,32000,24000,22050,16000,12000,11025,8000,7350,0,0,0};

int Cfaad::adts_parse(aac_buffer *b, int *bitrate, float *length)
{
    int frames, frame_length;
    int t_framelength = 0;
    int samplerate;
    float frames_per_sec, bytes_per_frame;

    // Read all frames to ensure correct time and bitrate
    for (frames = 0; ; frames++)
    {
        fill_buffer(b);

        if (b->bytes_into_buffer > 7)
        {
            // check syncword 
            if (!((b->buffer[0] == 0xFF)&&((b->buffer[1] & 0xF6) == 0xF0)))
                break;

            if (frames == 0)
                samplerate = adts_sample_rates[(b->buffer[2]&0x3c)>>2];

            frame_length = ((((unsigned int)b->buffer[3] & 0x3)) << 11)
                | (((unsigned int)b->buffer[4]) << 3) | (b->buffer[5] >> 5);

            t_framelength += frame_length;

            if (frame_length > b->bytes_into_buffer)
                break;

            advance_buffer(b, frame_length);
        } else {
            break;
        }
    }

    frames_per_sec = (float)samplerate/1024.0f;
    if (frames != 0)
        bytes_per_frame = (float)t_framelength/(float)(frames*1000);
    else
        bytes_per_frame = 0;
    *bitrate = (int)(8. * bytes_per_frame * frames_per_sec + 0.5);
    if (frames_per_sec != 0)
        *length = (float)frames/frames_per_sec;
    else
        *length = 1;

    return 1;
}
// *********************************************************************************************

// get AAC infos for printing 
void Cfaad::GetAACInfos(aac_buffer *b, DWORD *header_type, float *song_length, int *pbitrate, long filesize)
{
int		bitrate=0;
float	length=0;
int		bread;
long	tagsize=id3v2_TagSize(b);

	*header_type = 0;
	b->file_offset=tagsize;

    if ((b->buffer[0] == 0xFF) && ((b->buffer[1] & 0xF6) == 0xF0))
    {
        adts_parse(b, &bitrate, &length);
        fseek(b->infile, tagsize, SEEK_SET);

        bread = fread(b->buffer, 1, FAAD_MIN_STREAMSIZE*MAX_CHANNELS, b->infile);
        if (bread != FAAD_MIN_STREAMSIZE*MAX_CHANNELS)
            b->at_eof = 1;
        else
            b->at_eof = 0;
        b->bytes_into_buffer = bread;
        b->bytes_consumed = 0;
        b->file_offset = tagsize;

        *header_type = 1;
    } else if (memcmp(b->buffer, "ADIF", 4) == 0) {
        int skip_size = (b->buffer[4] & 0x80) ? 9 : 0;
        bitrate = ((unsigned int)(b->buffer[4 + skip_size] & 0x0F)<<19) |
            ((unsigned int)b->buffer[5 + skip_size]<<11) |
            ((unsigned int)b->buffer[6 + skip_size]<<3) |
            ((unsigned int)b->buffer[7 + skip_size] & 0xE0);

        length = (float)filesize;
        if (length != 0)
        {
            length = ((float)length*8.f)/((float)bitrate) + 0.5f;
        }

        bitrate = (int)((float)bitrate/1000.0f + 0.5f);

        *header_type = 2;
    }

    *song_length = length;
	*pbitrate=bitrate;
}

void Cfaad::GetAACInfos(char *Filename, aac_buffer *b, DWORD *header_type, float *song_length, int *pbitrate)
{
	if(!(b->infile=fopen(Filename,"rb")))
	{
		MessageBox(NULL,"Error opening file",NULL,MB_OK);
		return;
	}
	fseek(b->infile, 0, SEEK_END);
long	src_size=ftell(b->infile);
	fseek(b->infile, 0, SEEK_SET);
	if(!(b->buffer=(BYTE *)malloc(FAAD_STREAMSIZE)))
	{
		MessageBox(NULL,"Memory allocation error: b->buffer",NULL,MB_OK);
		return;
	}

int	tread=src_size<FAAD_STREAMSIZE ? src_size : FAAD_STREAMSIZE;
	b->bytes_into_buffer=fread(b->buffer, 1, tread, b->infile);
	if(b->bytes_into_buffer!=tread)
	{
		MessageBox(NULL,"Read failed!",NULL,MB_OK);
		return;
	}
	b->bytes_consumed=0;
	b->file_offset=0;
	b->at_eof=(b->bytes_into_buffer!=tread) ? 1 : 0;

	*header_type = 0;
	b->file_offset=0;

	GetAACInfos(b,header_type,song_length,pbitrate,src_size);

	free(b->buffer);
	fclose(b->infile);
}
*/
// *********************************************************************************************
//									Main functions
// *********************************************************************************************

void CMyDecCfg::getCfg(CMyDecCfg *cfg) 
{ 
CRegistry reg;

if(reg.OpenCreate(HKEY_CURRENT_USER, REGISTRY_PROGRAM_NAME "\\FAAD"))
	{
		cfg->DefaultCfg=reg.GetSetBool(REG_DEFAULT,true);
		cfg->DecCfg.defObjectType=reg.GetSetByte(REG_PROFILE,LC);
		cfg->DecCfg.defSampleRate=reg.GetSetDword(REG_SAMPLERATE,44100);
		cfg->DecCfg.outputFormat=reg.GetSetByte(REG_BPS,FAAD_FMT_16BIT);
		cfg->DecCfg.downMatrix=reg.GetSetByte(REG_DOWNMATRIX,0);
		cfg->DecCfg.useOldADTSFormat=reg.GetSetByte(REG_OLDADTS,0);
		cfg->DecCfg.dontUpSampleImplicitSBR=reg.GetSetByte(REG_DONTUPSAMPLESBR,1);
//		cfg->Channels=reg.GetSetByte("Channels",2);
	}
	else
		MessageBox(0,"Can't open registry!",0,MB_OK|MB_ICONSTOP);
}
// -----------------------------------------------------------------------------------------------

void CMyDecCfg::setCfg(CMyDecCfg *cfg)
{ 
CRegistry reg;

	if(reg.OpenCreate(HKEY_CURRENT_USER, REGISTRY_PROGRAM_NAME "\\FAAD"))
	{
		reg.SetBool(REG_DEFAULT,cfg->DefaultCfg);
		reg.SetByte(REG_PROFILE,cfg->DecCfg.defObjectType);
		reg.SetDword(REG_SAMPLERATE,cfg->DecCfg.defSampleRate);
		reg.SetByte(REG_BPS,cfg->DecCfg.outputFormat);
		reg.SetByte(REG_DOWNMATRIX,cfg->DecCfg.downMatrix);
		reg.SetByte(REG_OLDADTS,cfg->DecCfg.useOldADTSFormat);
		reg.SetByte(REG_DONTUPSAMPLESBR,cfg->DecCfg.dontUpSampleImplicitSBR);
//		reg.SetByte("Channels",cfg->Channels);
	}
	else
		MessageBox(0,"Can't open registry!",0,MB_OK|MB_ICONSTOP);
}
// *********************************************************************************************

void Cfaad::setFaadCfg(faacDecHandle hDecoder, CMyDecCfg Cfg)
{
faacDecConfigurationPtr	config=faacDecGetCurrentConfiguration(hDecoder);

	if(!Cfg.DefaultCfg)
	{
		config->defObjectType=Cfg.DecCfg.defObjectType;
		config->outputFormat=Cfg.DecCfg.outputFormat;
		config->defSampleRate=Cfg.DecCfg.defSampleRate;
		config->downMatrix=Cfg.DecCfg.downMatrix;
		config->useOldADTSFormat=Cfg.DecCfg.useOldADTSFormat;
		config->dontUpSampleImplicitSBR=1;
	}
	else
	{
		config->defObjectType=LC;
		config->outputFormat=FAAD_FMT_16BIT;
		config->defSampleRate=44100;
		config->downMatrix=0;
		config->useOldADTSFormat=0;
		config->dontUpSampleImplicitSBR=1;
	}
	faacDecSetConfiguration(hDecoder, config);
}
// -----------------------------------------------------------------------------------------------

void Cfaad::setDefaultFaadCfg(faacDecHandle hDecoder, BOOL showDlg)
{
	if(showDlg && ShowDlg4RawAAC)
		ShowDlg4RawAAC();

CMyDecCfg Cfg(false);
	setFaadCfg(hDecoder,Cfg);
}
// -----------------------------------------------------------------------------------------------

void Cfaad::DisplayError(char *ProcName, char *str)
{
MYINPUT *mi;
char buf[100]="";

	GlobalUnlock(hInput); // it wasn't done in getInfos()
	GLOBALLOCK(mi,hInput,MYINPUT,return);

	if(ProcName && *ProcName)
		sprintf(buf,"%s: ", ProcName);
	if(str && *str)
		strcat(buf,str);
	if(*buf && str)
		MessageBox(0, buf, APP_NAME " plugin", MB_OK|MB_ICONSTOP);

	mi->bytes_into_buffer=-1;
	GlobalUnlock(hInput);
}
// *********************************************************************************************

HANDLE Cfaad::getInfos(LPSTR lpstrFilename)
{
MYINPUT *mi;

// test tags
//CMyDecCfg cfg;	cfg.Tag.ReadMp4Tag(lpstrFilename);
//CMyDecCfg cfg;	cfg.Tag.ReadAacTag(lpstrFilename);

	GLOBALLOCK(mi,hInput,MYINPUT,return NULL);

//	mi->IsAAC=strcmpi(lpstrFilename+lstrlen(lpstrFilename)-4,".aac")==0;
	if((mi->IsMP4=IsMP4(lpstrFilename))==-1)
		return ERROR_getInfos("Error opening file");

	if(mi->IsMP4) // MP4 file ---------------------------------------------------------------------
	{
	MP4Duration			length;
	unsigned __int32	buffer_size;
	DWORD				timeScale;
	BYTE				sf;
    mp4AudioSpecificConfig mp4ASC;

		if(!(mi->mp4File=MP4Read(lpstrFilename, 0)))
			return ERROR_getInfos("Error opening file");

		if((mi->track=GetAACTrack(mi->mp4File))<0)
			return ERROR_getInfos(0); //"Unable to find correct AAC sound track");

		if(!(mi->hDecoder=faacDecOpen()))
			return ERROR_getInfos("Error initializing decoder library");

		MP4GetTrackESConfiguration(mi->mp4File, mi->track, (unsigned __int8 **)&mi->buffer, &buffer_size);
		if(!mi->buffer)
			return ERROR_getInfos("MP4GetTrackESConfiguration()");
		AudioSpecificConfig(mi->buffer, buffer_size, &mp4ASC);

        timeScale = mp4ASC.samplingFrequency;
        mi->Channels=mp4ASC.channelsConfiguration;
        sf = mp4ASC.samplingFrequencyIndex;
        mi->type = mp4ASC.objectTypeIndex;
//        mi->SBR=mp4ASC.sbr_present_flag;

		if(faacDecInit2(mi->hDecoder, mi->buffer, buffer_size, &mi->Samprate, &mi->Channels) < 0)
			return ERROR_getInfos("Error initializing decoder library");
		FREE_ARRAY(mi->buffer);

		length=MP4GetTrackDuration(mi->mp4File, mi->track);
		mi->len_ms=(DWORD)MP4ConvertFromTrackDuration(mi->mp4File, mi->track, length, MP4_MSECS_TIME_SCALE);
		mi->file_info.bitrate=MP4GetTrackBitRate(mi->mp4File, mi->track);
		mi->file_info.version=MP4GetTrackAudioType(mi->mp4File, mi->track)==MP4_MPEG4_AUDIO_TYPE ? 4 : 2;
		mi->numSamples=MP4GetTrackNumberOfSamples(mi->mp4File, mi->track);
		mi->sampleId=1;

		mi->IsSeekable=true;
		mi->LockSeeking=!mi->IsSeekable;
	}
	else // AAC file ------------------------------------------------------------------------------
	{   
	DWORD			read,
					tmp;
	BYTE			Channels4Raw=0;

		if(!(mi->aacFile=fopen(lpstrFilename,"rb")))
			return ERROR_getInfos("Error opening file"); 

		// use bufferized stream
		setvbuf(mi->aacFile,NULL,_IOFBF,32767);

		// get size of file
		fseek(mi->aacFile, 0, SEEK_END);
		mi->src_size=ftell(mi->aacFile);
		fseek(mi->aacFile, 0, SEEK_SET);

		if(!(mi->buffer=(BYTE *)malloc(FAAD_STREAMSIZE)))
			return ERROR_getInfos("Memory allocation error: mi->buffer");

		tmp=mi->src_size<FAAD_STREAMSIZE ? mi->src_size : FAAD_STREAMSIZE;
		read=fread(mi->buffer, 1, tmp, mi->aacFile);
		if(read==tmp)
		{
			mi->bytes_read=read;
			mi->bytes_into_buffer=read;
		}
		else
			return ERROR_getInfos("Read failed!");

		// skip Tag
	long tagsize;
		if(tagsize=id3v2_TagSize(mi->buffer))
		{
			if(tagsize>(long)mi->src_size)
				ERROR_getInfos("Corrupt stream!");
			if(tagsize<mi->bytes_into_buffer)
			{
				mi->bytes_into_buffer-=tagsize;
				memcpy(mi->buffer,mi->buffer+tagsize,mi->bytes_into_buffer);
			}
			else
			{
				mi->bytes_read=tagsize;
				mi->bytes_into_buffer=0;
				if(tagsize>mi->bytes_into_buffer)
					fseek(mi->aacFile, tagsize, SEEK_SET);
			}
			if(mi->src_size<mi->bytes_read+FAAD_STREAMSIZE-mi->bytes_into_buffer)
				tmp=mi->src_size-mi->bytes_read;
			else
				tmp=FAAD_STREAMSIZE-mi->bytes_into_buffer;
			read=fread(mi->buffer+mi->bytes_into_buffer, 1, tmp, mi->aacFile);
			if(read==tmp)
			{
				mi->bytes_read+=read;
				mi->bytes_into_buffer+=read;
			}
			else
				ERROR_getInfos("Read failed!");
		}

		if(get_AAC_format(lpstrFilename, &mi->file_info, &mi->seek_table, &mi->seek_table_length, 0))
			ERROR_getInfos("get_AAC_format()");
		mi->IsSeekable=mi->file_info.headertype==ADTS && mi->seek_table && mi->seek_table_length>0;
		mi->LockSeeking=!mi->IsSeekable;
/*
	aac_buffer	b;
	float		fLength;
	DWORD		headertype;
		b.infile=mi->aacFile;
		b.buffer=mi->buffer;
	    b.bytes_into_buffer=read;
		b.bytes_consumed=mi->bytes_consumed;
		b.file_offset=0;
		b.at_eof=(read!=tmp) ? 1 : 0;
		GetAACInfos(&b,&headertype,&fLength,&mi->file_info.bitrate,mi->src_size);
		mi->file_info.bitrate*=1024;
		mi->file_info.headertype=headertype;
        mi->bytes_into_buffer=b.bytes_into_buffer;
        mi->bytes_consumed=b.bytes_consumed;
		IsSeekable=false; // only mp4 can be seeked
*/
		if(!mi->FindBitrate) // open a new instance to get info from decoder
		{
		MYINPUT *miTmp;
		Cfaad	*NewInst;
			if(!(NewInst=new Cfaad()))
				return ERROR_getInfos("Memory allocation error: NewInst");

			GLOBALLOCK(miTmp,NewInst->hInput,MYINPUT,return 0);
			miTmp->FindBitrate=TRUE;
			NewInst->ShowDlg4RawAAC=ShowDlg4RawAAC;
			NewInst->pCfg=pCfg;
			if(!NewInst->getInfos(lpstrFilename))
				return ERROR_getInfos(0);
			mi->Channels=miTmp->frameInfo.channels;
			if(mi->file_info.headertype==RAW)
				mi->file_info.bitrate=miTmp->file_info.bitrate;//*mi->Channels;
			mi->Samprate=miTmp->Samprate;
			mi->file_info.headertype=miTmp->file_info.headertype;
			mi->file_info.object_type=miTmp->file_info.object_type;
			mi->file_info.version=miTmp->file_info.version;
			GlobalUnlock(NewInst->hInput);
			delete NewInst;
		}

		if(!(mi->hDecoder=faacDecOpen()))
			return ERROR_getInfos("Can't open library");
		if(mi->file_info.headertype==RAW)
			if(pCfg)
				setFaadCfg(mi->hDecoder,*pCfg);
			else
				setDefaultFaadCfg(mi->hDecoder,mi->FindBitrate);
	BYTE Channels; // faacDecInit doesn't report correctly the number of channels in raw aac files
		if((mi->bytes_consumed=faacDecInit(mi->hDecoder, mi->buffer, mi->bytes_into_buffer, &mi->Samprate, &Channels))<0)
			return ERROR_getInfos("faacDecInit()");
		mi->bytes_into_buffer-=mi->bytes_consumed;

		if(mi->FindBitrate) // get info from decoder
		{
		DWORD	Samples,
				BytesConsumed;

			if(!processData(hInput,0,0))
				return ERROR_getInfos(0);
			Samples=mi->frameInfo.samples/sizeof(short);
			BytesConsumed=mi->frameInfo.bytesconsumed;
			if(mi->file_info.headertype==RAW || !mi->file_info.bitrate)
				mi->file_info.bitrate=(BytesConsumed*8*mi->Samprate)/(Samples*2);
			if(!mi->file_info.bitrate)
				return ERROR_getInfos("Can't determine the bitrate");
		}

		mi->len_ms=(DWORD)((mi->src_size<<3)/(mi->file_info.bitrate>>10));
//		mi->len_ms=(DWORD)((1000*((float)mi->src_size*8))/mi->file_info.bitrate);
	}

	if(mi->len_ms)
		mi->dst_size=(DWORD)(mi->len_ms*((float)mi->Samprate/1000)*mi->Channels*(mi->BitsPerSample/8));
	else
		return ERROR_getInfos("Can't determine the length");

	showInfo(mi);

	GlobalUnlock(hInput);
    return hInput;
}
// *********************************************************************************************

int Cfaad::processData(HANDLE hInput, unsigned char far *bufout, long lBytes)
{
BYTE	*buffer;
DWORD	BytesDecoded=0;
char	*sample_buffer=0;
int		read;
MYINPUT	*mi;

	GLOBALLOCK(mi,hInput,MYINPUT,return 0);

	if(mi->LockSeeking)
	{
		NoSeek();
		mi->LockSeeking=false;
	}

	if(mi->IsMP4) // MP4 file --------------------------------------------------------------------------
	{   
	unsigned __int32 buffer_size=0;
    int rc;

		if(newpos_ms>-1)
		{
		MP4Duration duration=MP4ConvertToTrackDuration(mi->mp4File,mi->track,newpos_ms,MP4_MSECS_TIME_SCALE);
		MP4SampleId sampleId=MP4GetSampleIdFromTime(mi->mp4File,mi->track,duration,0);
			mi->bytes_read=(DWORD)(((float)newpos_ms*mi->file_info.bitrate)/(8*1000));
			if(seek(mi->bytes_read))  // update the slider
				return ERROR_processData(0);
			newpos_ms=-1;
		}
		do
		{
			buffer=NULL;
			if(mi->sampleId>=mi->numSamples)
				return ERROR_processData(0);

			rc=MP4ReadSample(mi->mp4File, mi->track, mi->sampleId++, (unsigned __int8 **)&buffer, &buffer_size, NULL, NULL, NULL, NULL);
			if(rc==0 || buffer==NULL)
			{
				FREE_ARRAY(buffer);
				return ERROR_processData("MP4ReadSample()");
			}

			sample_buffer=(char *)faacDecDecode(mi->hDecoder,&mi->frameInfo,buffer,buffer_size);
			BytesDecoded=mi->frameInfo.samples*sizeof(short);
			if(BytesDecoded>(DWORD)lBytes)
				BytesDecoded=lBytes;
			memmove(bufout,sample_buffer,BytesDecoded);
			FREE_ARRAY(buffer);
			// to update the slider
			mi->bytes_read+=buffer_size;
			if(seek(mi->bytes_read))
				return ERROR_processData(0);
		}while(!BytesDecoded && !mi->frameInfo.error);
	}
	else // AAC file --------------------------------------------------------------------------
	{   
		if(newpos_ms>-1)
		{
			if(mi->IsSeekable)
			{
			DWORD normalized=mi->len_ms/(mi->seek_table_length-1);
				if(normalized<1000)
					normalized=1000;
				mi->bytes_read=mi->seek_table[newpos_ms/normalized];
				fseek(mi->aacFile, mi->bytes_read, SEEK_SET);
				if(seek(mi->bytes_read))  // update the slider
					return ERROR_processData(0);
				mi->bytes_into_buffer=0;
				mi->bytes_consumed=FAAD_STREAMSIZE;
			}
			newpos_ms=-1;
		}
		buffer=mi->buffer;
		do
		{
			if(mi->bytes_consumed>0)
			{
				if(mi->bytes_into_buffer)
					memmove(buffer,buffer+mi->bytes_consumed,mi->bytes_into_buffer);

				if(mi->bytes_read<mi->src_size)
				{
				int tmp;
					if(mi->bytes_read+mi->bytes_consumed<mi->src_size)
						tmp=mi->bytes_consumed;
					else
						tmp=mi->src_size-mi->bytes_read;
					read=fread(buffer+mi->bytes_into_buffer, 1, tmp, mi->aacFile);
					if(read==tmp)
					{
						mi->bytes_read+=read;
						mi->bytes_into_buffer+=read;
					}
				}
				else
					if(mi->bytes_into_buffer)
						memset(buffer+mi->bytes_into_buffer, 0, mi->bytes_consumed);

				mi->bytes_consumed=0;

				if(	(mi->bytes_into_buffer>3 && !memcmp(mi->buffer, "TAG", 3)) ||
					(mi->bytes_into_buffer>11 && !memcmp(mi->buffer, "LYRICSBEGIN", 11)) ||
					(mi->bytes_into_buffer>8 && !memcmp(mi->buffer, "APETAGEX", 8)))
					return ERROR_processData(0);
			}

			if(mi->bytes_into_buffer<1)
				if(mi->bytes_read<mi->src_size)
					return ERROR_processData("Buffer empty!");
				else
					return ERROR_processData(0);

			sample_buffer=(char *)faacDecDecode(mi->hDecoder,&mi->frameInfo,buffer,mi->bytes_into_buffer);
			BytesDecoded=mi->frameInfo.samples*sizeof(short);
			if(bufout)
			{
				if(BytesDecoded>(DWORD)lBytes)
					BytesDecoded=lBytes;
				if(sample_buffer && BytesDecoded && !mi->frameInfo.error)
					memmove(bufout,sample_buffer,BytesDecoded);
			}
			else // Data needed to decode Raw files
			{
				mi->Channels=mi->frameInfo.channels;
				mi->file_info.object_type=mi->frameInfo.object_type;
			}
		    mi->bytes_consumed+=mi->frameInfo.bytesconsumed;
			mi->bytes_into_buffer-=mi->bytes_consumed;
		}while(!BytesDecoded && !mi->frameInfo.error);
	} // END AAC file --------------------------------------------------------------------------

	if(mi->frameInfo.error)
		return ERROR_processData((char *)faacDecGetErrorMessage(mi->frameInfo.error));

	showProgress(mi);

	GlobalUnlock(hInput);
    return BytesDecoded;
}

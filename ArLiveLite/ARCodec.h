#ifndef __AR_CODEC_H__
#define __AR_CODEC_H__

//qsv cuvid d3d11va dxva2
typedef enum CodecMethod
{
	CM_Adjust= 0,
	CM_Soft = 1,
	CM_Qsv = 2,
	CM_Cuvid = 3,
	CM_D3d11va = 4,
	CM_Dxva1 = 5,
}CodecMethod;

typedef enum CodecType
{
	CT_None = 0,
	CT_YUV420 = 1,
	CT_H264 = 2,
	CT_H265 = 3,
	CT_AV1 = 10,
	CT_MJpg = 20,

	CT_PCM = 100,
	CT_AAC,
	CT_MP3,
	CT_OPUS,
	CT_G711A,
	CT_G711U,
	CT_G719A_32 = 110,
	CT_G719A_48,
	CT_G719A_64,
	CT_G719A = CT_G719A_48,

	CT_G722_48 = 120,//16khz
	CT_G722_56,//16khz
	CT_G722_64,//16khz

	CT_G7221_7_24 = 130,	//16khz
	CT_G7221_7_32,		//16khz
	CT_G7221_14_24,		//32khz
	CT_G7221_14_32,		//32khz
	CT_G7221_14_48,		//32khz
}CodecType;

#endif	// __AR_CODEC_H__

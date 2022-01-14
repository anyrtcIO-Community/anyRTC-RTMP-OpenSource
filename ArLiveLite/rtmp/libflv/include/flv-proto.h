#ifndef _flv_proto_h_
#define _flv_proto_h_

// FLV Tag Type
#define FLV_TYPE_AUDIO		8
#define FLV_TYPE_VIDEO		9
#define FLV_TYPE_SCRIPT		18

// FLV Audio Type
#define FLV_AUDIO_ADPCM		(1 << 4)
#define FLV_AUDIO_MP3		(2 << 4)
#define FLV_AUDIO_G711A		(7 << 4) // G711 A-law
#define FLV_AUDIO_G711U     (8 << 4) // G711 mu-law
#define FLV_AUDIO_AAC		(10 << 4)
#define FLV_AUDIO_OPUS		(13 << 4)
#define FLV_AUDIO_MP3_8K	(14 << 4)
#define FLV_AUDIO_ASC		0x100 // AudioSpecificConfig(ISO-14496-3)
#define FLV_AUDIO_OPUS_HEAD	0x101 // opus-codec.org

// FLV Video Type
#define FLV_VIDEO_H263		2
#define FLV_VIDEO_VP6		4
#define FLV_VIDEO_H264		7
#define FLV_VIDEO_H265		12 // https://github.com/CDN-Union/H265
#define FLV_VIDEO_AV1		13 // https://aomediacodec.github.io/av1-isobmff
#define FLV_VIDEO_AVCC		0x200 // AVCDecoderConfigurationRecord(ISO-14496-15)
#define FLV_VIDEO_HVCC		0x201 // HEVCDecoderConfigurationRecord(ISO-14496-15)
#define FLV_VIDEO_AV1C		0x202 // AV1CodecConfigurationRecord(av1-isobmff)

#define FLV_SCRIPT_METADATA	0x300 // onMetaData

enum
{
	FLV_SEQUENCE_HEADER = 0, // AVC/AAC sequence header
	FLV_AVPACKET		= 1, // AVC NALU / AAC raw
	FLV_END_OF_SEQUENCE = 2, // AVC end of sequence (lower level NALU sequence ender is not required or supported)
};

enum
{
	FLV_VIDEO_KEY_FRAME					= 1, // key frame (for AVC, a seekable frame)
	FLV_VIDEO_INTER_FRAME				= 2, // inter frame (for AVC, a non-seekable frame)
	FLV_VIDEO_DISPOSABLE_INTER_FRAME	= 3, // H.263 only
	FLV_VIDEO_GENERATED_KEY_FRAME		= 4, // generated key frame (reserved for server use only)
	FLV_VIDEO_COMMAND_FRAME				= 5, // video info/command frame
};

#endif /* !_flv_proto_h_ */

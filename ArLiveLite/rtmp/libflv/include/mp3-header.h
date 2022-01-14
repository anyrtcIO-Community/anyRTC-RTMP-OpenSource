#ifndef _mp3_header_h_
#define _mp3_header_h_

// https://en.wikipedia.org/wiki/MP3

#if defined(__cplusplus)
extern "C" {
#endif

/*
ISO/IEC 11172-3 
2.4.1.3 Header
unsigned int sync: 12
unsigned int version: 1
unsigned int layer: 2
unsigned int error protection: 1
unsigned int bitrate_index: 4
unsigned int sampling_frequency: 2
unsigned int padding: 1
unsigned int private: 1
unsigned int mode: 2
unsigned int mode extension: 2
unsigned int copyright: 1
unsigned int original: 1
unsigned int emphasis: 2

bit_rate_index	Layer I		Layer II	Layer III
	'0000'		free format free format free format
	'0001'		32 kbit/s	32 kbit/s	32 kbit/s
	'0010'		64 kbit/s	48 kbit/s	40 kbit/s
	'0011'		96 kbit/s	56 kbit/s	48 kbit/s
	'0100'		128 kbit/s	64 kbit/s	56 kbit/s
	'0101'		160 kbit/s	80 kbit/s	64 kbit/s
	'0110'		192 kbit/s	96 kbit/s	80 kbit/s
	'0111'		224 kbit/s	112 kbit/s	96 kbit/s
	'1000'		256 kbit/s	128 kbit/s	112 kbit/s
	'1001'		288 kbit/s	160 kbit/s	128 kbit/s
	'1010'		320 kbit/s	192 kbit/s	160 kbit/s
	'1011'		352 kbit/s	224 kbit/s	192 kbit/s
	'1100'		384 kbit/s	256 kbit/s	224 kbit/s
	'1101'		416 kbit/s	320 kbit/s	256 kbit/s
	'1110'		448 kbit/s	384 kbit/s	320 kbit/s

sampling_frequency
'00' 44.1	kHz
'01' 48		kHz
'10' 32		kHz
'11' reserved

mode
'00' stereo
'01' joint_stereo (intensity_stereo and/or ms_stereo)
'10' dual_channel
'11' single_channel

mode_extension
'00' subbands 4-31 in intensity_stereo, bound==4
'01' subbands 8-31 in intensity_stereo, bound==8
'10' subbands 12-31 in intensity_stereo, bound==12
'11' subbands 16-31 in intensity_stereo, bound==16

emphasis
'00' no emphasis
'01' 50/15 microsec. emphasis
'10' reserved
'11' CCITT J.17
*/

struct mp3_header_t
{
	unsigned int version : 2; // 0-MPEG 2.5, 1-undefined, 2-MPEG-2, 3-MPEG-1
	unsigned int layer : 2; // 3-Layer I, 2-Layer II, 1-Layer III, 0-reserved
	unsigned int protection : 1;
	unsigned int bitrate_index : 4; //0-free, 
	unsigned int sampling_frequency : 2;
	unsigned int priviate : 1;
	unsigned int mode : 2;
	unsigned int mode_extension : 2;
	unsigned int copyright : 1;
	unsigned int original : 1;
	unsigned int emphasis : 2;
};

// version
#define MP3_MPEG1	3
#define MP3_MPEG2	2
#define MP3_MPEG2_5	0

// layer
#define MP3_LAYER1	3
#define MP3_LAYER2	2
#define MP3_LAYER3	1

#define MP3_BITS_PER_SAMPLE 16

///MP3 Header size: 4
int mp3_header_load(struct mp3_header_t* mp3, const void* data, int bytes);
int mp3_header_save(const struct mp3_header_t* mp3, void* data, int bytes);

int mp3_get_channel(const struct mp3_header_t* mp3);
int mp3_get_bitrate(const struct mp3_header_t* mp3);
int mp3_set_bitrate(struct mp3_header_t* mp3, int bitrate);
int mp3_get_frequency(const struct mp3_header_t* mp3);
int mp3_set_frequency(struct mp3_header_t* mp3, int frequency);

#if defined(__cplusplus)
}
#endif
#endif /* !_mp3_header_h_ */

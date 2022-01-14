#include "mpeg4-aac.h"
#include <assert.h>
#include <string.h>

int mpeg4_aac_adts_pce_load(const uint8_t* data, size_t bytes, struct mpeg4_aac_t* aac);
int mpeg4_aac_adts_pce_save(uint8_t* data, size_t bytes, const struct mpeg4_aac_t* aac);
int mpeg4_aac_audio_specific_config_load2(const uint8_t* data, size_t bytes, struct mpeg4_aac_t* aac);
int mpeg4_aac_audio_specific_config_save2(const struct mpeg4_aac_t* aac, uint8_t* data, size_t bytes);
int mpeg4_aac_stream_mux_config_load2(const uint8_t* data, size_t bytes, struct mpeg4_aac_t* aac);

/*
// ISO-14496-3 adts_frame (p122)

adts_fixed_header()
{
	syncword;					12 bslbf
	ID;							1 bslbf
	layer;						2 uimsbf
	protection_absent;			1 bslbf
	profile_ObjectType;			2 uimsbf
	sampling_frequency_index;	4 uimsbf
	private_bit;				1 bslbf
	channel_configuration;		3 uimsbf
	original_copy;				1 bslbf
	home;						1 bslbf
}

adts_variable_header()
{
	copyright_identification_bit;		1 bslbf
	copyright_identification_start;		1 bslbf
	aac_frame_length;					13 bslbf
	adts_buffer_fullness;				11 bslbf
	number_of_raw_data_blocks_in_frame; 2 uimsbf
}
*/
/// @return >=0-adts header length, <0-error
int mpeg4_aac_adts_load(const uint8_t* data, size_t bytes, struct mpeg4_aac_t* aac)
{
	if (bytes < 7) return -1;

	memset(aac, 0, sizeof(struct mpeg4_aac_t));
	assert(0xFF == data[0] && 0xF0 == (data[1] & 0xF0)); /* syncword */
	aac->profile = ((data[2] >> 6) & 0x03) + 1; // 2 bits: the MPEG-2 Audio Object Type add 1
	aac->sampling_frequency_index = (data[2] >> 2) & 0x0F; // 4 bits: MPEG-4 Sampling Frequency Index (15 is forbidden)
	aac->channel_configuration = ((data[2] & 0x01) << 2) | ((data[3] >> 6) & 0x03); // 3 bits: MPEG-4 Channel Configuration 
	assert(aac->profile > 0 && aac->profile < 31);
	assert(aac->channel_configuration >= 0 && aac->channel_configuration <= 7);
	assert(aac->sampling_frequency_index >= 0 && aac->sampling_frequency_index <= 0xc);
	aac->channels = mpeg4_aac_channel_count(aac->channel_configuration);
	aac->sampling_frequency = mpeg4_aac_audio_frequency_to(aac->sampling_frequency_index);
	aac->extension_frequency = aac->sampling_frequency;

	if (0 == aac->channel_configuration)
		return mpeg4_aac_adts_pce_load(data, bytes, aac);
	return 7;
}

/// @return >=0-adts header length, <0-error
int mpeg4_aac_adts_save(const struct mpeg4_aac_t* aac, size_t payload, uint8_t* data, size_t bytes)
{
	const uint8_t ID = 0; // 0-MPEG4/1-MPEG2
	size_t len = payload + 7;
	if (bytes < 7 || len >= (1 << 12)) return -1;

	if (0 == aac->channel_configuration && aac->npce > 0)
		len += mpeg4_aac_adts_pce_save(data, bytes, aac);

	assert(aac->profile > 0 && aac->profile < 31);
	assert(aac->channel_configuration >= 0 && aac->channel_configuration <= 7);
	assert(aac->sampling_frequency_index >= 0 && aac->sampling_frequency_index <= 0xc);
	data[0] = 0xFF; /* 12-syncword */
	data[1] = 0xF0 /* 12-syncword */ | (ID << 3)/*1-ID*/ | (0x00 << 2) /*2-layer*/ | 0x01 /*1-protection_absent*/;
	data[2] = ((aac->profile - 1) << 6) | ((aac->sampling_frequency_index & 0x0F) << 2) | ((aac->channel_configuration >> 2) & 0x01);
	data[3] = ((aac->channel_configuration & 0x03) << 6) | ((len >> 11) & 0x03); /*0-original_copy*/ /*0-home*/ /*0-copyright_identification_bit*/ /*0-copyright_identification_start*/
	data[4] = (uint8_t)(len >> 3);
	data[5] = ((len & 0x07) << 5) | 0x1F;
	data[6] = 0xFC /*| ((len / (1024 * aac->channels)) & 0x03)*/;
	return (int)(len - payload);
}

int mpeg4_aac_adts_frame_length(const uint8_t* data, size_t bytes)
{
	uint16_t len;
	if (bytes < 7) return -1;
	assert(0xFF == data[0] && 0xF0 == (data[1] & 0xF0)); /* syncword */
	len = ((uint16_t)(data[3] & 0x03) << 11) | ((uint16_t)data[4] << 3) | ((uint16_t)(data[5] >> 5) & 0x07);
	return len;
}

// ISO-14496-3 AudioSpecificConfig (p52)
/*
audioObjectType;								5 uimsbf
if (audioObjectType == 31) {
	audioObjectType = 32 + audioObjectTypeExt;	6 uimsbf
}
samplingFrequencyIndex;							4 bslbf
if ( samplingFrequencyIndex == 0xf ) {
	samplingFrequency;							24 uimsbf
}
channelConfiguration;							4 bslbf
*/
/// @return >=0-adts header length, <0-error
int mpeg4_aac_audio_specific_config_load(const uint8_t* data, size_t bytes, struct mpeg4_aac_t* aac)
{
	if (bytes < 2) return -1;

	memset(aac, 0, sizeof(struct mpeg4_aac_t));
	aac->profile = (data[0] >> 3) & 0x1F;
	aac->sampling_frequency_index = ((data[0] & 0x7) << 1) | ((data[1] >> 7) & 0x01);
	aac->channel_configuration = (data[1] >> 3) & 0x0F;
	assert(aac->profile > 0 && aac->profile < 31);
	assert(aac->channel_configuration >= 0 && aac->channel_configuration <= 7);
	assert(aac->sampling_frequency_index >= 0 && aac->sampling_frequency_index <= 0xc);
	aac->channels = mpeg4_aac_channel_count(aac->channel_configuration);
	aac->sampling_frequency = mpeg4_aac_audio_frequency_to(aac->sampling_frequency_index);
	aac->extension_frequency = aac->sampling_frequency;

	if (bytes > 2)
		return mpeg4_aac_audio_specific_config_load2(data, bytes, aac);
	return 2;
}

// ISO-14496-3 AudioSpecificConfig
int mpeg4_aac_audio_specific_config_save(const struct mpeg4_aac_t* aac, uint8_t* data, size_t bytes)
{
	uint8_t channel_configuration;
	if (bytes < 2+ (size_t)aac->npce) return -1;

	channel_configuration = aac->npce > 0 ? 0 : aac->channel_configuration;
	assert(aac->profile > 0 && aac->profile < 31);
	assert(aac->channel_configuration >= 0 && aac->channel_configuration <= 7);
	assert(aac->sampling_frequency_index >= 0 && aac->sampling_frequency_index <= 0xc);
	data[0] = (aac->profile << 3) | ((aac->sampling_frequency_index >> 1) & 0x07);
	data[1] = ((aac->sampling_frequency_index & 0x01) << 7) | ((channel_configuration & 0xF) << 3) | (0 << 2) /* frame length-1024 samples*/ | (0 << 1) /* don't depend on core */ | 0 /* not extension */;

	if (0 == aac->channel_configuration && aac->npce > 0)
		return mpeg4_aac_audio_specific_config_save2(aac, data, bytes);
	return 2;
}

// ISO/IEC 14496-3:2009(E) Table 1.42 - Syntax of StreamMuxConfig() (p83)
int mpeg4_aac_stream_mux_config_load(const uint8_t* data, size_t bytes, struct mpeg4_aac_t* aac)
{
	if (bytes < 6) return -1;

	memset(aac, 0, sizeof(*aac));
	if (6 == bytes && 0x40 == data[0] && 0 == (data[1] & 0xFE))
	{
		// fast path
		// [0] 0-audioMuxVersion(1), 1-allStreamsSameTimeFraming(1), 0-numSubFrames(6)
		assert(0 == (0x80 & data[0])); // audioMuxVersion: 0
		aac->profile = ((data[1] & 0x01) << 4) | (data[2] >> 4); // 0-numProgram(4), 0-numLayer(3), 1-ASC(1)
		aac->sampling_frequency_index = data[2] & 0x0F;
		aac->channel_configuration = data[3] >> 4;
		assert(aac->profile > 0 && aac->profile < 31);
		assert(aac->channel_configuration >= 0 && aac->channel_configuration <= 7);
		assert(aac->sampling_frequency_index >= 0 && aac->sampling_frequency_index <= 0xc);
		aac->channels = mpeg4_aac_channel_count(aac->channel_configuration);
		aac->sampling_frequency = mpeg4_aac_audio_frequency_to(aac->sampling_frequency_index);
		aac->extension_frequency = aac->sampling_frequency;
		return 6;
	}

	return mpeg4_aac_stream_mux_config_load2(data, bytes, aac);
}

// ISO/IEC 14496-3:2009(E) Table 1.42 - Syntax of StreamMuxConfig() (p83)
int mpeg4_aac_stream_mux_config_save(const struct mpeg4_aac_t* aac, uint8_t* data, size_t bytes)
{
	if (bytes < 6) return -1;

	assert(aac->profile > 0 && aac->profile < 31);
	assert(aac->channel_configuration >= 0 && aac->channel_configuration <= 7);
	assert(aac->sampling_frequency_index >= 0 && aac->sampling_frequency_index <= 0xc);
	data[0] = 0x40; // 0-audioMuxVersion(1), 1-allStreamsSameTimeFraming(1), 0-numSubFrames(6)
	//data[1] = 0x00 | ((aac->profile >> 4) & 0x01); // 0-numProgram(4), 0-numLayer(3)
	//data[2] = ((aac->profile & 0x0F) << 4) | (aac->sampling_frequency_index & 0x0F);
	data[1] = 0x00;
	data[2] = 0x20 | (aac->sampling_frequency_index & 0x0F); // AAC_LC profile
	data[3] = ((aac->channel_configuration & 0x0F) << 4) | 0; // 0-GASpecificConfig(3), 0-frameLengthType(1)
	data[4] = 0x3F; // 0-frameLengthType(2), 111111-latmBufferFullness(6)
	data[5] = 0xC0; // 11-latmBufferFullness(2), 0-otherDataPresent, 0-crcCheckPresent
	return 6;
}

// Table 1.6 ¨C Levels for the High Quality Audio Profile
static int mpeg4_aac_high_quality_level(const struct mpeg4_aac_t* aac)
{
	if (aac->sampling_frequency <= 22050)
	{
		if (aac->channel_configuration <= 2)
			return 1; // Level 1/5
	}
	else if (aac->sampling_frequency <= 48000)
	{
		if (aac->channel_configuration <= 2)
			return 2; // Level 2/6
		else if (aac->channel_configuration <= 5)
			return 3; // Level 3/4/7/8
	}

	return 8;
}

// Table 1.10 ¨C Levels for the AAC Profile
static int mpeg4_aac_level(const struct mpeg4_aac_t* aac)
{
	if (aac->sampling_frequency <= 24000)
	{
		if (aac->channel_configuration <= 2)
			return 1; // AAC Profile, Level 1
	}
	else if (aac->sampling_frequency <= 48000)
	{
		if (aac->channel_configuration <= 2)
			return 2; // Level 2
		else if (aac->channel_configuration <= 5)
			return 4; // Level 4
	}
	else if (aac->sampling_frequency <= 96000)
	{
		if (aac->channel_configuration <= 5)
			return 5; // Level 5
	}

	return 5;
}

static int mpeg4_aac_he_level(const struct mpeg4_aac_t* aac)
{
	if (aac->sampling_frequency <= 48000)
	{
		if (aac->channel_configuration <= 2)
			return aac->sbr ? 3 : 2; // Level 2/3
		else if (aac->channel_configuration <= 5)
			return 4; // Level 4
	}
	else if (aac->sampling_frequency <= 96000)
	{
		if (aac->channel_configuration <= 5)
			return 5; // Level 5
	}

	return 5;
}

// ISO/IEC 14496-3:2009(E)  Table 1.14 - audioProfileLevelIndication values (p51)
int mpeg4_aac_profile_level(const struct mpeg4_aac_t* aac)
{
	// Table 1.10 - Levels for the AAC Profile (p49)
	// Table 1.14 - audioProfileLevelIndication values (p51)
	switch (aac->profile)
	{
	case MPEG4_AAC_LC:
		return mpeg4_aac_level(aac) - 1 + 0x28; // AAC Profile
	case MPEG4_AAC_SBR:
		return mpeg4_aac_he_level(aac) - 2 + 0x2C; // High Efficiency AAC Profile
	case MPEG4_AAC_PS:
		return mpeg4_aac_he_level(aac) - 2 + 0x30; // High Efficiency AAC v2 Profile
	case MPEG4_AAC_CELP:
		return mpeg4_aac_high_quality_level(aac) - 1 + 0x0E; // High Quality Audio Profile
	default:
		return 1; // Main Audio Profile, Level 1
	}
}

#define ARRAYOF(arr) sizeof(arr)/sizeof(arr[0])

static const int s_frequency[] = { 96000, 88200, 64000, 48000, 44100, 32000, 24000, 22050, 16000, 12000, 11025, 8000, 7350 };

int mpeg4_aac_audio_frequency_to(enum mpeg4_aac_frequency index)
{
	if (index < 0 || index >= ARRAYOF(s_frequency))
		return 0;
	return s_frequency[index];
}

int mpeg4_aac_audio_frequency_from(int frequence)
{
	int i = 0;
	while (i < ARRAYOF(s_frequency) && s_frequency[i] != frequence) i++;
	return i >= ARRAYOF(s_frequency) ? -1 : i;
}

uint8_t mpeg4_aac_channel_count(uint8_t channel_configuration)
{
	static const uint8_t s_channels[] = { 0, 1, 2, 3, 4, 5, 6, 8 };
	if (channel_configuration < 0 || channel_configuration >= ARRAYOF(s_channels))
		return 0;
	return s_channels[channel_configuration];
}
#undef ARRAYOF

#if defined(_DEBUG) || defined(DEBUG)
void mpeg4_aac_test(void)
{
	struct mpeg4_aac_t aac, aac2;
	const unsigned char asc[] = { 0x13, 0x88 };
	const unsigned char adts[] = { 0xFF, 0xF1, 0x5C, 0x40, 0x01, 0x1F, 0xFC };
//	const unsigned char ascsbr[] = { 0x13, 0x10, 0x56, 0xe5, 0x9d, 0x48, 0x00 };
	const unsigned char ascsbr[] = { 0x2b, 0x92, 0x08, 0x00 };
	// https://datatracker.ietf.org/doc/html/rfc6416#page-25
	const unsigned char mux1[] = { 0x40, 0x00, 0x8B, 0x18, 0x38, 0x83, 0x80 }; // 6 kbit/s CELP
	const unsigned char mux2[] = { 0x40, 0x00, 0x26, 0x20, 0x3f, 0xc0 }; // 64 kbit/s AAC LC Stereo
	const unsigned char mux3[] = { 0x40, 0x00, 0x56, 0x23, 0x10, 0x1f, 0xe0 }; // Hierarchical Signaling of SBR
	const unsigned char mux4[] = { 0x40, 0x00, 0x26, 0x10, 0x3f, 0xc0 }; // HE AAC v2 Signaling
	const unsigned char mux5[] = { 0x40, 0x01, 0xd6, 0x13, 0x10, 0x1f, 0xe0 }; // Hierarchical Signaling of PS
	const unsigned char mux6[] = { 0x8F, 0xF8, 0x00, 0x41, 0x92, 0xB1, 0x18, 0x80, 0xFF, 0x0D, 0xDE, 0x36, 0x99, 0xF2, 0x40, 0x8C, 0x00, 0x53, 0x6C, 0x02, 0x31, 0x3C, 0xF3, 0xCE, 0x0F, 0xF0 }; // MPEG Surround
	const unsigned char mux7[] = { 0x40, 0x00, 0x56, 0x23, 0x10, 0x1f, 0xe0 }; // MPEG Surround with Extended SDP Parameters
	const unsigned char mux8[] = { 0x8F, 0xF8, 0x00, 0x06, 0x52, 0xB9, 0x20, 0x87, 0x6A, 0x83, 0xA1, 0xF4, 0x40, 0x88, 0x40, 0x53, 0x62, 0x0F, 0xF0 }; // MPEG Surround with Single-Layer Configuration
	
	unsigned char data[32];

	assert(sizeof(ascsbr) == mpeg4_aac_audio_specific_config_load(ascsbr, sizeof(ascsbr), &aac));
	assert(2 == aac.profile && 7 == aac.sampling_frequency_index && 2 == aac.channel_configuration);
	//assert(sizeof(ascsbr) == mpeg4_aac_audio_specific_config_save(&aac, data, sizeof(data)));
	//assert(0 == memcmp(ascsbr, data, sizeof(ascsbr)));

	assert(sizeof(asc) == mpeg4_aac_audio_specific_config_load(asc, sizeof(asc), &aac));
	assert(2 == aac.profile && 7 == aac.sampling_frequency_index && 1 == aac.channel_configuration);
	assert(sizeof(asc) == mpeg4_aac_audio_specific_config_save(&aac, data, sizeof(data)));
	assert(0 == memcmp(asc, data, sizeof(asc)));

	assert(sizeof(adts) == mpeg4_aac_adts_save(&aac, 1, data, sizeof(data)));
	assert(0 == memcmp(adts, data, sizeof(adts)));
	assert(7 == mpeg4_aac_adts_load(data, sizeof(adts), &aac2));
	assert(0 == memcmp(&aac, &aac2, sizeof(aac)));

	assert(22050 == mpeg4_aac_audio_frequency_to(aac.sampling_frequency_index));
	assert(aac.sampling_frequency_index == mpeg4_aac_audio_frequency_from(22050));

	//assert(sizeof(ascsbr) == mpeg4_aac_audio_specific_config_load(ascsbr, sizeof(ascsbr), &aac));
	//assert(2 == aac.profile && 6 == aac.sampling_frequency_index && 1 == aac.channel_configuration);

	memset(&aac, 0, sizeof(aac));
	mpeg4_aac_stream_mux_config_load(mux1, sizeof(mux1), &aac);
	mpeg4_aac_stream_mux_config_load(mux2, sizeof(mux2), &aac);
	mpeg4_aac_stream_mux_config_load(mux3, sizeof(mux3), &aac);
	mpeg4_aac_stream_mux_config_load(mux4, sizeof(mux4), &aac);
	mpeg4_aac_stream_mux_config_load(mux5, sizeof(mux5), &aac);
	//mpeg4_aac_stream_mux_config_load(mux6, sizeof(mux6), &aac);
	//mpeg4_aac_stream_mux_config_load(mux7, sizeof(mux7), &aac);
	//mpeg4_aac_stream_mux_config_load(mux8, sizeof(mux8), &aac);
	mpeg4_aac_stream_mux_config_save(&aac, data, sizeof(data));
	//assert(0 == memcmp(data, mux1, sizeof(mux1)));
}
#endif

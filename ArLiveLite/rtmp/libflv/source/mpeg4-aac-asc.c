#include "mpeg4-aac.h"
#include "mpeg4-bits.h"
#include <assert.h>
#include <stdlib.h>
#include <string.h>

// Table 4.85 - Syntactic elements (p533)
enum {
	ID_SCE = 0x0, // single channel element()
	ID_CPE = 0x1, // channel_pair_element()
	ID_CCE = 0x2, // coupling_channel_element()
	ID_LFE = 0x3, // lfe_channel_element()
	ID_DSE = 0x4, // data_stream_element()
	ID_PCE = 0x5, // program_config_element()
	ID_FIL = 0x6, // fill_element()
	ID_END = 0x7,
};

// ISO-14496-3 4.4.1.1 Program config element (p488)
// There may be up to 16 such elements per raw data block, each one must have a unique element_instance_tag
// PCEs must come before all other syntactic elements in a raw_data_block().
/*
program_config_element()
{
	element_instance_tag;                       4 uimsbf
	object_type;                                2 uimsbf
	sampling_frequency_index;                   4 uimsbf
	num_front_channel_elements;                 4 uimsbf
	num_side_channel_elements;                  4 uimsbf
	num_back_channel_elements;                  4 uimsbf
	num_lfe_channel_elements;                   2 uimsbf
	num_assoc_data_elements;                    3 uimsbf
	num_valid_cc_elements;                      4 uimsbf
	mono_mixdown_present;                       1 uimsbf
	if (mono_mixdown_present == 1 )
		mono_mixdown_element_number;            4 uimsbf
	stereo_mixdown_present;                     1 uimsbf
	if (stereo_mixdown_present == 1 )
		stereo_mixdown_element_number;          4 uimsbf
	matrix_mixdown_idx_present;                 1 uimsbf
	if (matrix_mixdown_idx_present == 1 ) {
		matrix_mixdown_idx ;                    2 uimsbf
		pseudo_surround_enable;                 1 uimsbf
	}

	for (i = 0; i < num_front_channel_elements; i++) {
		front_element_is_cpe[i];                1 bslbf
		front_element_tag_select[i];            4 uimsbf
	}
	for (i = 0; i < num_side_channel_elements; i++) {
		side_element_is_cpe[i];                 1 bslbf
		side_element_tag_select[i];             4 uimsbf
	}
	for (i = 0; i < num_back_channel_elements; i++) {
		back_element_is_cpe[i];                 1 bslbf
		back_element_tag_select[i];             4 uimsbf
	}
	for (i = 0; i < num_lfe_channel_elements; i++)
		lfe_element_tag_select[i];              4 uimsbf
	for ( i = 0; i < num_assoc_data_elements; i++)
		assoc_data_element_tag_select[i];       4 uimsbf
	for (i = 0; i < num_valid_cc_elements; i++) {
		cc_element_is_ind_sw[i];                1 uimsbf
		valid_cc_element_tag_select[i];         4 uimsbf
	}
	byte_alignment();                           Note 1
	comment_field_bytes;                        8 uimsbf
	for (i = 0; i < comment_field_bytes; i++)
		comment_field_data[i];                  8 uimsbf
}
*/
static inline uint64_t mpeg4_bits_copy(struct mpeg4_bits_t* dst, struct mpeg4_bits_t* src, int n)
{
	uint64_t v;
	v = mpeg4_bits_read_n(src, n);
	mpeg4_bits_write_n(dst, v, n);
	return v;
}

static int mpeg4_aac_pce_load(struct mpeg4_bits_t* bits, struct mpeg4_aac_t* aac, struct mpeg4_bits_t* pce)
{
	uint64_t i, cpe, tag;
	uint64_t element_instance_tag;
	uint64_t object_type;
	uint64_t sampling_frequency_index;
	uint64_t num_front_channel_elements;
	uint64_t num_side_channel_elements;
	uint64_t num_back_channel_elements;
	uint64_t num_lfe_channel_elements;
	uint64_t num_assoc_data_elements;
	uint64_t num_valid_cc_elements;
	uint64_t comment_field_bytes;

	aac->channels = 0;
	element_instance_tag = mpeg4_bits_copy(pce, bits, 4);
	object_type = mpeg4_bits_copy(pce, bits, 2);
	sampling_frequency_index = mpeg4_bits_copy(pce, bits, 4);
	num_front_channel_elements = mpeg4_bits_copy(pce, bits, 4);
	num_side_channel_elements = mpeg4_bits_copy(pce, bits, 4);
	num_back_channel_elements = mpeg4_bits_copy(pce, bits, 4);
	num_lfe_channel_elements = mpeg4_bits_copy(pce, bits, 2);
	num_assoc_data_elements = mpeg4_bits_copy(pce, bits, 3);
	num_valid_cc_elements = mpeg4_bits_copy(pce, bits, 4);

	if (mpeg4_bits_copy(pce, bits, 1))
		mpeg4_bits_copy(pce, bits, 4);		// MONO
	if (mpeg4_bits_copy(pce, bits, 1))
		mpeg4_bits_copy(pce, bits, 4);		// STEREO
	if (mpeg4_bits_copy(pce, bits, 1))
		mpeg4_bits_copy(pce, bits, 3);		// Matrix, Pseudo surround

	for (i = 0; i < num_front_channel_elements; i++)
	{
		cpe = mpeg4_bits_copy(pce, bits, 1); // front_element_is_cpe
		tag = mpeg4_bits_copy(pce, bits, 4); // front_element_tag_select
		aac->channels += (cpe || aac->ps) ? 2 : 1;
	}

	for (i = 0; i < num_side_channel_elements; i++)
	{
		cpe = mpeg4_bits_copy(pce, bits, 1); // side_element_is_cpe
		tag = mpeg4_bits_copy(pce, bits, 4); // side_element_tag_select
		aac->channels += (cpe || aac->ps) ? 2 : 1;
	}

	for (i = 0; i < num_back_channel_elements; i++)
	{
		cpe = mpeg4_bits_copy(pce, bits, 1); // back_element_is_cpe
		tag = mpeg4_bits_copy(pce, bits, 4); // back_element_tag_select
		aac->channels += (cpe || aac->ps) ? 2 : 1;
	}

	for (i = 0; i < num_lfe_channel_elements; i++)
	{
		tag = mpeg4_bits_copy(pce, bits, 4); // lfe_element_tag_select
		aac->channels += 1;
	}

	for (i = 0; i < num_assoc_data_elements; i++)
	{
		tag = mpeg4_bits_copy(pce, bits, 4); // assoc_data_element_tag_select
	}

	for (i = 0; i < num_valid_cc_elements; i++)
	{
		cpe = mpeg4_bits_copy(pce, bits, 1); // cc_element_is_ind_sw
		tag = mpeg4_bits_copy(pce, bits, 4); // valid_cc_element_tag_select
	}

	mpeg4_bits_aligment(bits, 8); // byte_alignment();
	mpeg4_bits_aligment(pce, 8);

	comment_field_bytes = mpeg4_bits_copy(pce, bits, 8);
	for (i = 0; i < comment_field_bytes; i++)
		mpeg4_bits_copy(pce, bits, 8); // comment_field_data

	assert(aac->sampling_frequency_index == sampling_frequency_index);
	assert(aac->profile == object_type + 1);
	return (int)((pce->bits + 7) / 8);
}

// 4.4.1 Decoder configuration (GASpecificConfig) (p487)
/*
GASpecificConfig (samplingFrequencyIndex, channelConfiguration, audioObjectType)
{
	frameLengthFlag;											1 bslbf
	dependsOnCoreCoder;											1 bslbf
	if (dependsOnCoreCoder) {
		coreCoderDelay;											14 uimsbf
	}
	extensionFlag;												1 bslbf
	if (! channelConfiguration) {
		program_config_element ();
	}
	if ((audioObjectType == 6) || (audioObjectType == 20)) {
		layerNr;												3 uimsbf
	}
	if (extensionFlag) {
		if (audioObjectType == 22) {
			numOfSubFrame;										5 bslbf
			layer_length;										11 bslbf
		}
		if (audioObjectType == 17 || audioObjectType == 19 || audioObjectType == 20 || audioObjectType == 23) {
			aacSectionDataResilienceFlag;						1 bslbf
			aacScalefactorDataResilienceFlag;					1 bslbf
			aacSpectralDataResilienceFlag;						1 bslbf
		}
		extensionFlag3;											1 bslbf
		if (extensionFlag3) {
			// tbd in version 3
		}
	}
}
*/
static int mpeg4_aac_ga_specific_config_load(struct mpeg4_bits_t* bits, struct mpeg4_aac_t* aac)
{
	int extensionFlag;
	struct mpeg4_bits_t pce;

	mpeg4_bits_read(bits); // frameLengthFlag
	if (mpeg4_bits_read(bits)) // dependsOnCoreCoder
		mpeg4_bits_read_uint16(bits, 14); // coreCoderDelay
	extensionFlag = mpeg4_bits_read(bits); // extensionFlag

	if (0 == aac->channel_configuration)
	{
		mpeg4_bits_init(&pce, aac->pce, sizeof(aac->pce));
		aac->npce = mpeg4_aac_pce_load(bits, aac, &pce); // update channel count
	}

	if (6 == aac->profile || 20 == aac->profile)
		mpeg4_bits_read_uint8(bits, 3); // layerNr

	if (extensionFlag)
	{
		if (22 == aac->profile)
		{
			mpeg4_bits_read_uint8(bits, 5); // numOfSubFrame
			mpeg4_bits_read_uint16(bits, 11); // layer_length
		}

		if (17 == aac->profile || 19 == aac->profile || 20 == aac->profile || 23 == aac->profile)
		{
			mpeg4_bits_read(bits); // aacSectionDataResilienceFlag
			mpeg4_bits_read(bits); // aacScalefactorDataResilienceFlag
			mpeg4_bits_read(bits); // aacSpectralDataResilienceFlag
		}

		if (mpeg4_bits_read(bits)) // extensionFlag3
		{
			// tbd in version 3
			assert(0);
		}
	}

	return mpeg4_bits_error(bits);
}

static int mpeg4_aac_celp_specific_config_load(struct mpeg4_bits_t* bits, struct mpeg4_aac_t* aac)
{
	int ExcitationMode;
	if (mpeg4_bits_read(bits)) // isBaseLayer
	{
		// CelpHeader

		ExcitationMode = mpeg4_bits_read(bits);
		mpeg4_bits_read(bits); // SampleRateMode
		mpeg4_bits_read(bits); // FineRateControl

		// Table 3.50 - Description of ExcitationMode
		if (ExcitationMode == 1 /*RPE*/)
		{
			mpeg4_bits_read_n(bits, 3); // RPE_Configuration
		}
		if (ExcitationMode == 0 /*MPE*/)
		{
			mpeg4_bits_read_n(bits, 5); // MPE_Configuration
			mpeg4_bits_read_n(bits, 2); // NumEnhLayers
			mpeg4_bits_read(bits); // BandwidthScalabilityMode
		}
	}
	else
	{
		if (mpeg4_bits_read(bits)) // isBWSLayer
			mpeg4_bits_read_n(bits, 2); // BWS_configuration
		else
			mpeg4_bits_read_n(bits, 2); // CELP-BRS-id
	}

	(void)aac;
	return mpeg4_bits_error(bits);
}

// ISO/IEC 23003-1 Table 9.1 ¡ª Syntax of SpatialSpecificConfig()
/*
SpatialSpecificConfig()
{
	bsSamplingFrequencyIndex;	4	uimsbf
	if ( bsSamplingFrequencyIndex == 0xf ) {
		bsSamplingFrequency;	24	uimsbf
	}
	bsFrameLength;	7	uimsbf
	bsFreqRes;	3	uimsbf
	bsTreeConfig;	4	uimsbf
	if (bsTreeConfig == ¡®0111¡¯) {
		bsNumInCh;	4	uimsbf
		bsNumLFE	2	uimsbf
		bsHasSpeakerConfig	1	uimsbf
		if ( bsHasSpeakerConfig == 1 ) {
			audioChannelLayout = SpeakerConfig3d();		Note 1
		}
	}

	bsQuantMode;	2	uimsbf
	bsOneIcc;	1	uimsbf
	bsArbitraryDownmix;	1	uimsbf
	bsFixedGainSur;	3	uimsbf
	bsFixedGainLFE;	3	uimsbf
	bsFixedGainDMX;	3	uimsbf
	bsMatrixMode;	1	uimsbf
	bsTempShapeConfig;	2	uimsbf
	bsDecorrConfig;	2	uimsbf
	bs3DaudioMode;	1	uimsbf

	if ( bsTreeConfig == ¡®0111¡¯ ) {
		for (i=0; i< NumInCh - NumLfe; i++) {
			defaultCld[i] = 1;
			ottModelfe[i] = 0;
		}
		for (i= NumInCh - NumLfe; i< NumInCh; i++) {
			defaultCld[i] = 1;
			ottModelfe[i] = 1;
		}
	}

	for (i=0; i<numOttBoxes; i++) {		Note 2
		OttConfig(i);
	}
	for (i=0; i<numTttBoxes; i++) {		Note 2
		TttConfig(i);
	}
	if (bsTempShapeConfig == 2) {
		bsEnvQuantMode	1	uimsbf
	}
	if (bs3DaudioMode) {
		bs3DaudioHRTFset;	2	uimsbf
		if (bs3DaudioHRTFset==0) {
			ParamHRTFset();
		}
	}
	ByteAlign();
	SpatialExtensionConfig();
}
*/
static int SpatialSpecificConfig(struct mpeg4_bits_t* bits, struct mpeg4_aac_t* aac)
{
    return 0;
}

static inline uint8_t mpeg4_aac_get_audio_object_type(struct mpeg4_bits_t* bits)
{
	uint8_t audioObjectType;
	audioObjectType = mpeg4_bits_read_uint8(bits, 5);
	if (31 == audioObjectType)
		audioObjectType = 32 + mpeg4_bits_read_uint8(bits, 6);
	return audioObjectType;
}

static inline uint8_t mpeg4_aac_get_sampling_frequency(struct mpeg4_bits_t* bits)
{
	uint8_t samplingFrequencyIndex;
	uint32_t samplingFrequency;
	samplingFrequencyIndex = mpeg4_bits_read_uint8(bits, 4);
	if (0x0F == samplingFrequencyIndex)
		samplingFrequency = mpeg4_bits_read_uint32(bits, 24);
	return samplingFrequencyIndex;
}

/// @return asc bits
static size_t mpeg4_aac_audio_specific_config_load3(struct mpeg4_bits_t* bits, struct mpeg4_aac_t* aac)
{
	uint16_t syncExtensionType;
//	uint8_t audioObjectType;
	uint8_t extensionAudioObjectType = 0;
//	uint8_t samplingFrequencyIndex = 0;
	uint8_t extensionSamplingFrequencyIndex = 0;
//	uint8_t channelConfiguration = 0;
	uint8_t extensionChannelConfiguration = 0;
	uint8_t epConfig;
	size_t offset;

	offset = bits->bits;
	aac->profile = mpeg4_aac_get_audio_object_type(bits);
	aac->sampling_frequency_index = mpeg4_aac_get_sampling_frequency(bits);
	aac->channel_configuration = mpeg4_bits_read_uint8(bits, 4);
	aac->channels = mpeg4_aac_channel_count(aac->channel_configuration);
	aac->sampling_frequency = mpeg4_aac_audio_frequency_to(aac->sampling_frequency_index);
	aac->extension_frequency = aac->sampling_frequency;

	if (5 == aac->profile || 29 == aac->profile)
	{
		extensionAudioObjectType = 5;
		aac->sbr = 1;
		if (29 == aac->profile)
			aac->ps = 1;
		extensionSamplingFrequencyIndex = mpeg4_aac_get_sampling_frequency(bits);
		aac->extension_frequency = mpeg4_aac_audio_frequency_to(extensionSamplingFrequencyIndex);
		aac->profile = mpeg4_aac_get_audio_object_type(bits);
		if (22 == aac->profile)
			extensionChannelConfiguration = mpeg4_bits_read_uint8(bits, 4);
	}
	else
	{
		extensionAudioObjectType = 0;
	}

	switch (aac->profile)
	{
	case 1: case 2: case 3: case 4: case 6: case 7:
	case 17: case 19: case 20: case 21: case 22: case 23:
		mpeg4_aac_ga_specific_config_load(bits, aac);
		break;

	case 8:
		mpeg4_aac_celp_specific_config_load(bits, aac);
		break;

	case 30:
		/*sacPayloadEmbedding=*/ mpeg4_bits_read(bits);
		break;

	default:
		assert(0);
		return bits->bits - offset;
	}

	switch (aac->profile)
	{
	case 17: case 19: case 20: case 21: case 22:
	case 23: case 24: case 25: case 26: case 27: case 39:
		epConfig = mpeg4_bits_read_uint8(bits, 2);
		if (2 == epConfig || 3 == epConfig)
		{
			// 1.8.2.1 Error protection specific configuration (p96)
			// TODO: ErrorProtectionSpecificConfig();
			assert(0);
		}
		if (3 == epConfig)
		{
			if (mpeg4_bits_read(bits)) // directMapping
			{
				// tbd
				assert(0);
			}
		}
		break;

	default:
		break; // do nothing;
	}

	if (5 != extensionAudioObjectType && mpeg4_bits_remain(bits) >= 16)
	{
		syncExtensionType = mpeg4_bits_read_uint16(bits, 11);
		if (0x2b7 == syncExtensionType)
		{
			extensionAudioObjectType = mpeg4_aac_get_audio_object_type(bits);
			if (5 == extensionAudioObjectType)
			{
				aac->sbr = mpeg4_bits_read(bits);
				if (aac->sbr)
				{
					extensionSamplingFrequencyIndex = mpeg4_aac_get_sampling_frequency(bits);
					aac->extension_frequency = mpeg4_aac_audio_frequency_to(extensionSamplingFrequencyIndex);
					if (mpeg4_bits_remain(bits) >= 12)
					{
						syncExtensionType = mpeg4_bits_read_uint16(bits, 11);
						if (0x548 == syncExtensionType)
							aac->ps = mpeg4_bits_read(bits);
					}
				}
			}
			if (22 == extensionAudioObjectType)
			{
				aac->sbr = mpeg4_bits_read(bits);
				if (aac->sbr)
				{
					extensionSamplingFrequencyIndex = mpeg4_aac_get_sampling_frequency(bits);
					aac->extension_frequency = mpeg4_aac_audio_frequency_to(extensionSamplingFrequencyIndex);
				}
				extensionChannelConfiguration = mpeg4_bits_read_uint8(bits, 4);
			}
		}
	}

	return bits->bits - offset;
}

int mpeg4_aac_audio_specific_config_load2(const uint8_t* data, size_t bytes, struct mpeg4_aac_t* aac)
{
	struct mpeg4_bits_t bits;
	mpeg4_bits_init(&bits, (void*)data, bytes);
	mpeg4_aac_audio_specific_config_load3(&bits, aac);
	mpeg4_bits_aligment(&bits, 8);
	return mpeg4_bits_error(&bits) ? -1 : (int)(bits.bits / 8);
}

int mpeg4_aac_audio_specific_config_save2(const struct mpeg4_aac_t* aac, uint8_t* data, size_t bytes)
{
	if (bytes < 2 + (size_t)aac->npce)
		return -1;

	memcpy(data + 2, aac->pce, aac->npce);
	return 2 + aac->npce;
	//data[2 + aac->npce] = 0x56;
	//data[2 + aac->npce + 1] = 0xe5;
	//data[2 + aac->npce + 2] = 0x00;
	//return 2 + aac->npce + 3;
}

int mpeg4_aac_adts_pce_load(const uint8_t* data, size_t bytes, struct mpeg4_aac_t* aac)
{
	uint8_t i;
	size_t offset = 7;
	struct mpeg4_bits_t bits, pce;
	
	if (0 == (data[1] & 0x01)) // protection_absent
	{
		// number_of_raw_data_blocks_in_frame
		for (i = 1; i <= (data[6] & 0x03); i++)
			offset += 2; // raw_data_block_position 16-bits
		offset += 2; // crc_check 16-bits
	}

	if (bytes <= offset)
		return (int)offset;

	mpeg4_bits_init(&bits, (uint8_t*)data + offset, bytes - offset);
	if (ID_PCE == mpeg4_bits_read_uint8(&bits, 3))
	{
		mpeg4_bits_init(&pce, aac->pce, sizeof(aac->pce));
		aac->npce = mpeg4_aac_pce_load(&bits, aac, &pce);
		return mpeg4_bits_error(&bits) ? -1 : (int)(7 + (pce.bits + 7) / 8);
	}
	return 7;
}

int mpeg4_aac_adts_pce_save(uint8_t* data, size_t bytes, const struct mpeg4_aac_t* aac)
{
	struct mpeg4_aac_t src;
	struct mpeg4_bits_t pce, adts;
	if ((size_t)aac->npce + 7 > bytes)
		return 0;
	memcpy(&src, aac, sizeof(src));
//	assert(data[1] & 0x01); // disable protection_absent
	mpeg4_bits_init(&pce, (uint8_t*)aac->pce, aac->npce);
	mpeg4_bits_init(&adts, (uint8_t*)data + 7, bytes - 7);
	mpeg4_bits_write_uint8(&adts, ID_PCE, 3);
	mpeg4_aac_pce_load(&pce, &src, &adts);
	assert(src.channels == aac->channels);
	return mpeg4_bits_error(&pce) ? 0 : (int)((7 + (adts.bits+7) / 8));
}

static size_t mpeg4_aac_stream_mux_config_load3(struct mpeg4_bits_t* bits, struct mpeg4_aac_t* aac)
{
	uint8_t audioMuxVersion = 0;
	uint8_t numSubFrames;
	uint8_t numProgram;
	uint8_t numLayer;
	uint8_t allStreamsSameTimeFraming;
	uint8_t profile = 0;
	uint64_t ascLen;
	size_t offset;
	int streamCnt, prog, lay;

	offset = bits->bits;
	audioMuxVersion = (uint8_t)mpeg4_bits_read(bits);
	if (!audioMuxVersion || 0 == mpeg4_bits_read(bits))
	{
		if (1 == audioMuxVersion)
			/*taraBufferFullness =*/ mpeg4_bits_read_latm(bits);

		streamCnt = 0;
		allStreamsSameTimeFraming = (uint8_t)mpeg4_bits_read(bits);
		numSubFrames = (uint8_t)mpeg4_bits_read_n(bits, 6);
		numProgram = (uint8_t)mpeg4_bits_read_n(bits, 4);
		for (prog = 0; prog <= numProgram; prog++)
		{
			numLayer = (uint8_t)mpeg4_bits_read_n(bits, 3);
			for (lay = 0; lay <= numLayer; lay++)
			{
				//progSIndx[streamCnt] = prog;
				//laySIndx[streamCnt] = lay;
				//streamID[prog][lay] = streamCnt++;
				if ( (prog == 0 && lay == 0) || 0 == (uint8_t)mpeg4_bits_read(bits)) 
				{
					profile = aac->profile; // previous profile
					if (audioMuxVersion == 0) {
						mpeg4_aac_audio_specific_config_load3(bits, aac);
					} else {
						ascLen = mpeg4_bits_read_latm(bits);
						ascLen -= mpeg4_aac_audio_specific_config_load3(bits, aac);
						mpeg4_bits_skip(bits, (size_t)ascLen);
					}
				}

				//frameLengthType[streamID[prog][lay]] = (uint8_t)mpeg4_bits_read_n(bits, 3);
				//switch (frameLengthType[streamID[prog][lay]])
				switch (mpeg4_bits_read_n(bits, 3))
				{
				case 0:
					/*latmBufferFullness[streamID[prog][lay]] =*/ (uint8_t)mpeg4_bits_read_n(bits, 8);
					if (!allStreamsSameTimeFraming)
					{
						// fixme
						//if ((AudioObjectType[lay] == 6 || AudioObjectType[lay] == 20) &&
						//	(AudioObjectType[lay - 1] == 8 || AudioObjectType[lay - 1] == 24))
						if( (aac->profile == 6 || aac->profile == 20) && (profile == 8 || profile == 24) )
						{
							/*coreFrameOffset =*/ (uint8_t)mpeg4_bits_read_n(bits, 6);
						}
					}
					break;

				case 1:
					/*frameLength[streamID[prog][lay]] =*/ (uint16_t)mpeg4_bits_read_n(bits, 9);
					break;

				case 3:
				case 4:
				case 5:
					/*CELPframeLengthTableIndex[streamID[prog][lay]] =*/ (uint16_t)mpeg4_bits_read_n(bits, 6);
					break;

				case 6:
				case 7:
					/*HVXCframeLengthTableIndex[streamID[prog][lay]] =*/ (uint16_t)mpeg4_bits_read_n(bits, 1);
					break;

				default:
					// nothing to do
					break;
				}
			}
		}

		// otherDataPresent
		if (mpeg4_bits_read(bits))
		{
			if (audioMuxVersion == 1)
			{
				/*otherDataLenBits =*/ mpeg4_bits_read_latm(bits);
			}
			else
			{
				/*otherDataLenBits =*/ mpeg4_bits_read_n(bits, 8); /* helper variable 32bit */
				while(mpeg4_bits_read(bits))
				{
					/*otherDataLenBits <<= 8;
					otherDataLenBits +=*/ mpeg4_bits_read_n(bits, 8);
				}
			}
		}

		// crcCheckPresent
		if (mpeg4_bits_read(bits))
			/*crcCheckSum =*/ mpeg4_bits_read_n(bits, 8);
	}
	else
	{
		/*tbd*/
	}

	return bits->bits - offset;
}

int mpeg4_aac_stream_mux_config_load2(const uint8_t* data, size_t bytes, struct mpeg4_aac_t* aac)
{
	struct mpeg4_bits_t bits;
	mpeg4_bits_init(&bits, (void*)data, bytes);
	mpeg4_aac_stream_mux_config_load3(&bits, aac);
	mpeg4_bits_aligment(&bits, 8);
	return mpeg4_bits_error(&bits) ? -1 : (int)(bits.bits / 8);
}

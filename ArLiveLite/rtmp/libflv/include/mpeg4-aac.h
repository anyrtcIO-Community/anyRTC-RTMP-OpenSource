#ifndef _mpeg4_aac_h_
#define _mpeg4_aac_h_

#include <stddef.h>
#include <stdint.h>

#if defined(__cplusplus)
extern "C" {
#endif

struct mpeg4_aac_t
{
	uint8_t profile; // 0-NULL, 1-AAC Main, 2-AAC LC, 2-AAC SSR, 3-AAC LTP
	uint8_t sampling_frequency_index; // 0-96000, 1-88200, 2-64000, 3-48000, 4-44100, 5-32000, 6-24000, 7-22050, 8-16000, 9-12000, 10-11025, 11-8000, 12-7350, 13/14-reserved, 15-frequency is written explictly
	uint8_t channel_configuration; // 0-AOT, 1-1channel,front-center, 2-2channels, front-left/right, 3-3channels: front center/left/right, 4-4channels: front-center/left/right, back-center, 5-5channels: front center/left/right, back-left/right, 6-6channels: front center/left/right, back left/right LFE-channel, 7-8channels

	uint32_t extension_frequency; // play frequency(AAC-HE v1/v2 sbr/ps)
	uint32_t sampling_frequency;  // codec frequency, valid only in decode
	uint8_t channels; // valid only in decode
	int sbr; // sbr flag, valid only in decode
	int ps; // ps flag, valid only in decode
	uint8_t pce[64];
	int  npce; // pce bytes
};

enum mpeg2_aac_profile
{
	MPEG2_AAC_MAIN = 0,
	MPEG2_AAC_LC,
	MPEG2_AAC_SSR,
};

// ISO/IEC 14496-3:2009(E) Table 1.3 - Audio Profiles definition (p41)
// https://en.wikipedia.org/wiki/MPEG-4_Part_3#Audio_Profiles
enum mpeg4_aac_object_type
{
	MPEG4_AAC_MAIN = 1,
	MPEG4_AAC_LC,
	MPEG4_AAC_SSR,
	MPEG4_AAC_LTP,
	MPEG4_AAC_SBR, // (used with AAC LC in the "High Efficiency AAC Profile" (HE-AAC v1))
	MPEG4_AAC_SCALABLE,
	MPEG4_AAC_TWINVQ,
	MPEG4_AAC_CELP,
	MPEG4_AAC_HVXC,
	MPEG4_AAC_TTSI = 12,
	MPEG4_AAC_MAIN_SYNTHETIC,
	MPEG4_AAC_WAVETABLE_SYNTHETIC,
	MPEG4_AAC_GENERAL_MIDI,
	MPEG4_AAC_ALGORITHMIC_SYNTHESIS, // Algorithmic Synthesis and Audio FX object type
	MPEG4_AAC_ER_LC, // Error Resilient (ER) AAC Low Complexity (LC) object type
	MPEG4_AAC_ER_LTP = 19, // Error Resilient (ER) AAC Long Term Predictor (LTP) object type
	MPEG4_AAC_ER_SCALABLE, // Error Resilient (ER) AAC scalable object type
	MPEG4_AAC_ER_TWINVQ, // Error Resilient (ER) TwinVQ object type
	MPEG4_AAC_ER_BSAC, // Error Resilient (ER) BSAC object type
	MPEG4_AAC_ER_AAC_LD, // Error Resilient (ER) AAC LD object type(used with CELP, ER CELP, HVXC, ER HVXC and TTSI in the "Low Delay Profile")
	MPEG4_AAC_ER_CELP, // Error Resilient (ER) CELP object type
	MPEG4_AAC_ER_HVXC, // Error Resilient (ER) HVXC object type
	MPEG4_AAC_ER_HILN, // Error Resilient (ER) HILN object type
	MPEG4_AAC_ER_PARAMTRIC, // Error Resilient (ER) Parametric object type
	MPEG4_AAC_SSC, // SSC Audio object type
	MPEG4_AAC_PS, // PS object type(used with AAC LC and SBR in the "HE-AAC v2 Profile")
	MPEG4_AAC_MPEG_SURROUND, // MPEG Surround object type
	MPEG4_AAC_LAYER_1 = 32, // Layer-1 Audio object type
	MPEG4_AAC_LAYER_2, // Layer-2 Audio object type
	MPEG4_AAC_LAYER_3, // Layer-3 Audio object type
	MPEG4_AAC_DST,
	MPEG4_AAC_ALS, // ALS Audio object type
	MPEG4_AAC_SLS, // SLS Audio object type
	MPEG4_AAC_SLS_NON_CORE, // SLS Non-Core Audio object type
	MPEG4_AAC_ER_AAC_ELD, // Error Resilient (ER) AAC ELD object type (uses AAC-LD, AAC-ELD and AAC-ELDv2, "Low Delay AAC v2")
	MPEG4_AAC_SMR_SIMPLE, // SMR Simple object type: MPEG-4 Part 23 standard (ISO/IEC 14496-23:2008)
	MPEG4_AAC_SMR_MAIN, // SMR Main object type
	MPEG4_AAC_USAC_NO_SBR, // Unified Speech and Audio Coding (no SBR)
	MPEG4_AAC_SAOC, // Spatial Audio Object Coding: MPEG-D Part 2 standard (ISO/IEC 23003-2:2010)
	MPEG4_AAC_LD_MEPG_SURROUND, // MPEG-D Part 2 - ISO/IEC 23003-2
	MPEG4_AAC_USAC, // MPEG-D Part 3 - ISO/IEC 23003-3
};

enum mpeg4_audio_profile
{
	MPEG4_AAC_PROFILE, // AAC LC
	MPEG4_HIGH_EFFICIENCY_AAC_PROFILE, // AAC LC, SBR (<=128 kbps)
	MPEG4_HE_AAC_V2_PROFILE, // AAC LC, SBR, PS (approx. 16 - 48 kbit/s)
	MPEG4_MAIN_AUDIO_PROFILE, // AAC Main, AAC LC, AAC SSR, AAC LTP, AAC Scalable, TwinVQ, CELP, HVXC, TTSI, Main synthesis
	MPEG4_SCALABLE_AUDIO_PROFILE, // AAC LC, AAC LTP, AAC Scalable, TwinVQ, CELP, HVXC, TTSI
	MPEG4_SPEECH_AUDIO_PROFILE, // CELP, HVXC, TTSI
	MPEG4_SYNTHETIC_AUDIO_PRIFILE, // TTSI, Main synthesis
	MPEG4_HIGH_QUALITY_AUDIO_PROFILE, // AAC LC, AAC LTP, AAC Scalable, CELP, ER AAC LC, ER AAC LTP, ER AAC Scalable, ER CELP
	MPEG4_LOW_DELAY_AUDIO_PROFILE, // CELP, HVXC, TTSI, ER AAC LD, ER CELP, ER HVXC
	MPEG4_NATURAL_AUDIO_PRIFILE, // AAC Main, AAC LC, AAC SSR, AAC LTP, AAC Scalable, TwinVQ, CELP, HVXC, TTSI, ER AAC LC, ER AAC LTP, ER AAC Scalable, ER TwinVQ, ER BSAC, ER AAC LD, ER CELP, ER HVXC, ER HILN, ER Parametric
	MPEG4_MOBILE_AUDIO_INTERNETWORKING_PROFILE, // ER AAC LC, ER AAC Scalable, ER TwinVQ, ER BSAC, ER AAC LD
	MPEG4_HD_AAC_PROFILE, // AAC LC, SLS
	MPEG4_ALS_SIMPLE_PROFILE, // ALS
};

enum mpeg4_aac_frequency
{
	MPEG4_AAC_96000 = 0,
	MPEG4_AAC_88200,	// 0x1
	MPEG4_AAC_64000,	// 0x2
	MPEG4_AAC_48000,	// 0x3
	MPEG4_AAC_44100,	// 0x4
	MPEG4_AAC_32000,	// 0x5
	MPEG4_AAC_24000,	// 0x6
	MPEG4_AAC_22050,	// 0x7
	MPEG4_AAC_16000,	// 0x8
	MPEG4_AAC_12000,	// 0x9
	MPEG4_AAC_11025,	// 0xa
	MPEG4_AAC_8000,		// 0xb
	MPEG4_AAC_7350,		// 0xc
						// reserved
						// reserved
						// escape value

};

/// @return >=0-adts header length, <0-error
int mpeg4_aac_adts_save(const struct mpeg4_aac_t* aac, size_t payload, uint8_t* data, size_t bytes);
/// @return >=0-adts header length, <0-error
int mpeg4_aac_adts_load(const uint8_t* data, size_t bytes, struct mpeg4_aac_t* aac);

/// @return >=0-audio specific config length, <0-error
int mpeg4_aac_audio_specific_config_load(const uint8_t* data, size_t bytes, struct mpeg4_aac_t* aac);
/// @return >=0-audio specific config length, <0-error
int mpeg4_aac_audio_specific_config_save(const struct mpeg4_aac_t* aac, uint8_t* data, size_t bytes);

/// @return >=0-stream mux config length, <0-error
int mpeg4_aac_stream_mux_config_load(const uint8_t* data, size_t bytes, struct mpeg4_aac_t* aac);
/// @return >=0-stream mux config length, <0-error
int mpeg4_aac_stream_mux_config_save(const struct mpeg4_aac_t* aac, uint8_t* data, size_t bytes);

/// get AAC profile level indication value
int mpeg4_aac_profile_level(const struct mpeg4_aac_t* aac);

/// MPEG4_AAC_96000 => 96000
/// @return -1-error, other-frequency value
int mpeg4_aac_audio_frequency_to(enum mpeg4_aac_frequency index);
/// 96000 => MPEG4_AAC_96000
/// @return -1-error, other-frequency index
int mpeg4_aac_audio_frequency_from(int frequency);

/// @return aac channel count
uint8_t mpeg4_aac_channel_count(uint8_t channel_configuration);

int mpeg4_aac_adts_frame_length(const uint8_t* data, size_t bytes);

#if defined(__cplusplus)
}
#endif
#endif /* !_mpeg4_aac_h_ */

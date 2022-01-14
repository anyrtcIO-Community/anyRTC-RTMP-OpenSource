#ifndef _webm_vpx_h_
#define _webm_vpx_h_

#include <stdint.h>
#include <stddef.h>

#if defined(__cplusplus)
extern "C" {
#endif

// VP8/VP9/VP10
struct webm_vpx_t
{
    uint8_t profile;
    uint8_t level;
    uint8_t bit_depth;
    uint8_t chroma_subsampling; // 0-4:2:0 vertical, 1-4:2:0 colocated with luma (0,0), 2-4:2:2, 3-4:4:4
    uint8_t video_full_range_flag; // 0 = legal range (e.g. 16-235 for 8 bit sample depth); 1 = full range (e.g. 0-255 for 8-bit sample depth)
    uint8_t colour_primaries; // ISO/IEC 23001-8:2016
    uint8_t transfer_characteristics;
    uint8_t matrix_coefficients;
    uint16_t codec_intialization_data_size; // must be 0
    uint8_t codec_intialization_data[1]; // not used for VP8 and VP9
};

int webm_vpx_codec_configuration_record_load(const uint8_t* data, size_t bytes, struct webm_vpx_t* vpx);
int webm_vpx_codec_configuration_record_save(const struct webm_vpx_t* vpx, uint8_t* data, size_t bytes);

int webm_vpx_codec_configuration_record_from_vp8(struct webm_vpx_t* vpx, int* width, int* height, const void* keyframe, size_t bytes);
int webm_vpx_codec_configuration_record_from_vp9(struct webm_vpx_t* vpx, int* width, int* height, const void* keyframe, size_t bytes);

#if defined(__cplusplus)
}
#endif
#endif /* !_webm_vpx_h_ */

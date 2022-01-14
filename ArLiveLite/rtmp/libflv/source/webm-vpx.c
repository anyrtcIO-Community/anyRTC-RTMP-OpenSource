#include "webm-vpx.h"
#include "mpeg4-bits.h"
#include <stdlib.h>
#include <string.h>
#include <assert.h>

enum {
    WEBM_VP_LEVEL_1     = 10,
    WEBM_VP_LEVEL_1_1   = 11,
    WEBM_VP_LEVEL_2     = 20,
    WEBM_VP_LEVEL_2_1   = 21,
    WEBM_VP_LEVEL_3     = 30,
    WEBM_VP_LEVEL_3_1   = 31,
    WEBM_VP_LEVEL_4     = 40,
    WEBM_VP_LEVEL_4_1   = 41,
    WEBM_VP_LEVEL_5     = 50,
    WEBM_VP_LEVEL_5_1   = 51,
    WEBM_VP_LEVEL_5_2   = 52,
    WEBM_VP_LEVEL_6     = 60,
    WEBM_VP_LEVEL_6_1   = 61,
    WEBM_VP_LEVEL_6_2   = 62,
};

/*
aligned (8) class VPCodecConfigurationRecord {
    unsigned int (8)     profile;
    unsigned int (8)     level;
    unsigned int (4)     bitDepth;
    unsigned int (3)     chromaSubsampling;
    unsigned int (1)     videoFullRangeFlag;
    unsigned int (8)     colourPrimaries;
    unsigned int (8)     transferCharacteristics;
    unsigned int (8)     matrixCoefficients;
    unsigned int (16)    codecIntializationDataSize;
    unsigned int (8)[]   codecIntializationData;
}
*/

int webm_vpx_codec_configuration_record_load(const uint8_t* data, size_t bytes, struct webm_vpx_t* vpx)
{
    if (bytes < 8)
        return -1;

    vpx->profile = data[0];
    vpx->level = data[1];
    vpx->bit_depth = (data[2] >> 4) & 0x0F;
    vpx->chroma_subsampling = (data[2] >> 1) & 0x07;
    vpx->video_full_range_flag = data[2] & 0x01;
    vpx->colour_primaries = data[3];
    vpx->transfer_characteristics = data[4];
    vpx->matrix_coefficients = data[5];
    vpx->codec_intialization_data_size = (((uint16_t)data[6]) << 8) | data[7];
    assert(0 == vpx->codec_intialization_data_size);
    return 8;
}

int webm_vpx_codec_configuration_record_save(const struct webm_vpx_t* vpx, uint8_t* data, size_t bytes)
{
    if (bytes < 8 + (size_t)vpx->codec_intialization_data_size)
        return 0; // don't have enough memory

    data[0] = vpx->profile;
    data[1] = vpx->level;
    data[2] = (vpx->bit_depth << 4) | ((vpx->chroma_subsampling & 0x07) << 1) | (vpx->video_full_range_flag & 0x01);
    data[3] = vpx->colour_primaries;
    data[4] = vpx->transfer_characteristics;
    data[5] = vpx->matrix_coefficients;
    data[6] = (uint8_t)(vpx->codec_intialization_data_size >> 8);
    data[7] = (uint8_t)vpx->codec_intialization_data_size;

    if(vpx->codec_intialization_data_size > 0)
        memcpy(data + 8, vpx->codec_intialization_data, vpx->codec_intialization_data_size);
    return 8 + vpx->codec_intialization_data_size;
}

// https://www.webmproject.org/vp9/mp4/
// https://github.com/webmproject/vp9-dash
// https://www.rfc-editor.org/rfc/pdfrfc/rfc6386.txt.pdf
// https://storage.googleapis.com/downloads.webmproject.org/docs/vp9/vp9-bitstream-specification-v0.6-20160331-draft.pdf
int webm_vpx_codec_configuration_record_from_vp8(struct webm_vpx_t* vpx, int *width, int* height, const void* keyframe, size_t bytes)
{
    uint32_t tag;
    const uint8_t* p;
    const uint8_t startcode[] = { 0x9d, 0x01, 0x2a };

    if (bytes < 10)
        return -1;

    p = (const uint8_t*)keyframe;

    // 9.1.  Uncompressed Data Chunk
    tag = (uint32_t)p[0] | ((uint32_t)p[1] << 8) | ((uint32_t)p[2] << 16);
    //key_frame = tag & 0x01;
    //version = (tag >> 1) & 0x07;
    //show_frame = (tag >> 4) & 0x1;
    //first_part_size = (tag >> 5) & 0x7FFFF;

    if (0 != (tag & 0x01) || startcode[0] != p[3] || startcode[1] != p[4] || startcode[2] != p[5])
        return -1; // not key frame

    *width = ((uint16_t)(p[7] & 0x3F) << 8) | (uint16_t)(p[6]); // (2 bits Horizontal Scale << 14) | Width (14 bits)
    *height = ((uint16_t)(p[9] & 0x3F) << 8) | (uint16_t)(p[8]); // (2 bits Vertical Scale << 14) | Height (14 bits)

    memset(vpx, 0, sizeof(*vpx));
    vpx->profile = (tag >> 1) & 0x03;
    vpx->level = 31;
    vpx->bit_depth = 8;
    return 0;
}

// https://storage.googleapis.com/downloads.webmproject.org/docs/vp9/vp9-bitstream-specification-v0.6-20160331-draft.pdf
int webm_vpx_codec_configuration_record_from_vp9(struct webm_vpx_t* vpx, int* width, int* height, const void* keyframe, size_t bytes)
{
    const uint8_t* p;
    struct mpeg4_bits_t bits;
    const uint8_t frame_sync_code[] = { 0x49, 0x83, 0x42 };

    p = (const uint8_t*)keyframe;
    if (bytes < 4 || frame_sync_code[0] != p[1] || frame_sync_code[1] != p[2] || frame_sync_code[2] != p[3])
        return -1;
    
    memset(vpx, 0, sizeof(*vpx));
    vpx->level = 31;

    // 6.2 Uncompressed header syntax
    mpeg4_bits_init(&bits, (void*)keyframe, bytes);
    mpeg4_bits_read_n(&bits, 2); // 2-frame_marker
    vpx->profile = (uint8_t)(mpeg4_bits_read(&bits) | (mpeg4_bits_read(&bits) << 1)); // 2-profile_low_bit+profile_high_bit
    mpeg4_bits_read_n(&bits, 4 + 24); // skip 4-bits + frame_sync_code

    // color_config()
    if (vpx->profile >= 2)
        vpx->bit_depth = (uint8_t)mpeg4_bits_read(&bits) ? 12 : 10; // 1-ten_or_twelve_bit
    else
        vpx->bit_depth = 8;
    if (7 /*CS_RGB*/ != mpeg4_bits_read_n(&bits, 3)) // 3-color_space
    {
        vpx->video_full_range_flag = (uint8_t)mpeg4_bits_read(&bits); // color_range
        if (1 == vpx->profile || 3 == vpx->profile)
        {
            vpx->chroma_subsampling = 3 - (uint8_t)mpeg4_bits_read_n(&bits, 2); // subsampling_x/subsampling_y
            mpeg4_bits_read(&bits); // reserved_zero
        }
    }
    else
    {
        if (1 == vpx->profile || 3 == vpx->profile)
            mpeg4_bits_read(&bits); // reserved_zero
    }
   
    // frame_size()
    *width = (int)mpeg4_bits_read_n(&bits, 16) + 1;
    *height = (int)mpeg4_bits_read_n(&bits, 16) + 1;
    return mpeg4_bits_error(&bits) ? -1 : 0;
}

#if defined(_DEBUG) || defined(DEBUG)
void webm_vpx_test(void)
{
    const unsigned char src[] = {
        0x00, 0x1f, 0x80, 0x02, 0x02, 0x02, 0x00, 0x00
    };
    unsigned char data[sizeof(src)];

    struct webm_vpx_t vpx;
    assert(sizeof(src) == webm_vpx_codec_configuration_record_load(src, sizeof(src), &vpx));
    assert(0 == vpx.profile && 31 == vpx.level && 8 == vpx.bit_depth && 0 == vpx.chroma_subsampling && 0 == vpx.video_full_range_flag);
    assert(sizeof(src) == webm_vpx_codec_configuration_record_save(&vpx, data, sizeof(data)));
    assert(0 == memcmp(src, data, sizeof(src)));
}
#endif

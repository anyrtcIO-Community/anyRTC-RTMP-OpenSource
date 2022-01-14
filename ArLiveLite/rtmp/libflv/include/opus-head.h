#ifndef _opus_head_h_
#define _opus_head_h_

#include <stddef.h>
#include <stdint.h>

#if defined(__cplusplus)
extern "C" {
#endif

struct opus_head_t
{
    uint8_t version;
    uint8_t channels;
    uint16_t pre_skip;
    uint32_t input_sample_rate;
    int16_t output_gain;
    uint8_t channel_mapping_family;
    uint8_t stream_count;
    uint8_t coupled_count;
    uint8_t channel_mapping[8];
};

/// @return >0-ok, <=0-error
int opus_head_save(const struct opus_head_t* opus, uint8_t* data, size_t bytes);
/// @return >0-ok, <=0-error
int opus_head_load(const uint8_t* data, size_t bytes, struct opus_head_t* opus);

static inline int opus_head_channels(const struct opus_head_t* opus)
{
    return 0 == opus->channels ? 2 : opus->channels;
}

int opus_packet_getframes(const void* data, size_t len, int (*onframe)(uint8_t toc, const void* frame, size_t size), void* param);

#if defined(__cplusplus)
}
#endif
#endif /* !_opus_head_h_ */

#ifndef _flv_reader_h_
#define _flv_reader_h_

#include <stdint.h>
#include <stddef.h>

#if defined(__cplusplus)
extern "C" {
#endif

void* flv_reader_create(const char* file);
void* flv_reader_create2(int(*read)(void* param, void* buf, int len), void* param);
void flv_reader_destroy(void* flv);

///@param[out] tagtype 8-audio, 9-video, 18-script data
///@param[out] timestamp FLV timestamp
///@param[out] taglen flv tag length(0 is ok but should be silently discard)
///@param[out] buffer FLV stream
///@param[in] bytes buffer size
///@return 1-got a packet, 0-EOF, other-error
int flv_reader_read(void* flv, int* tagtype, uint32_t* timestamp, size_t* taglen, void* buffer, size_t bytes);

#if defined(__cplusplus)
}
#endif
#endif /* !_flv_reader_h_ */

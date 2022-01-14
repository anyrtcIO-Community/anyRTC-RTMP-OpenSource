#ifndef _rtmp_netstream_h_
#define _rtmp_netstream_h_

#include <stdint.h>
#include <stddef.h>

uint8_t* rtmp_netstream_play(uint8_t* out, size_t bytes, double transactionId, const char* stream_name, double start, double duration, int reset);
uint8_t* rtmp_netstream_pause(uint8_t* out, size_t bytes, double transactionId, int pause, double ms);
uint8_t* rtmp_netstream_seek(uint8_t* out, size_t bytes, double transactionId, double ms);

uint8_t* rtmp_netstream_receive_audio(uint8_t* out, size_t bytes, double transactionId, int enable);
uint8_t* rtmp_netstream_receive_video(uint8_t* out, size_t bytes, double transactionId, int enable);

uint8_t* rtmp_netstream_publish(uint8_t* out, size_t bytes, double transactionId, const char* stream_name, const char* stream_type);
uint8_t* rtmp_netstream_delete_stream(uint8_t* out, size_t bytes, double transactionId, double stream_id);
uint8_t* rtmp_netconnection_close_stream(uint8_t* out, size_t bytes, double transactionId, double stream_id);

uint8_t* rtmp_netstream_release_stream(uint8_t* out, size_t bytes, double transactionId, const char* stream_name);
uint8_t* rtmp_netstream_fcpublish(uint8_t* out, size_t bytes, double transactionId, const char* stream_name);
uint8_t* rtmp_netstream_fcunpublish(uint8_t* out, size_t bytes, double transactionId, const char* stream_name);
uint8_t* rtmp_netstream_fcsubscribe(uint8_t* out, size_t bytes, double transactionId, const char* stream_name);
uint8_t* rtmp_netstream_fcunsubscribe(uint8_t* out, size_t bytes, double transactionId, const char* stream_name);

uint8_t* rtmp_netstream_onstatus(uint8_t* out, size_t bytes, double transactionId, const char* level, const char* code, const char* description);

uint8_t* rtmp_netstream_rtmpsampleaccess(uint8_t* out, size_t bytes);

#endif /* !_rtmp_netstream_h_ */

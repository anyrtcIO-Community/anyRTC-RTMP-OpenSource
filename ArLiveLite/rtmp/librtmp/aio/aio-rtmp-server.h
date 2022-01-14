#ifndef _aio_rtmp_server_h_
#define _aio_rtmp_server_h_

#include <stdint.h>
#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct aio_rtmp_server_t aio_rtmp_server_t;
typedef struct aio_rtmp_session_t aio_rtmp_session_t;
typedef void* aio_rtmp_userptr_t;

struct aio_rtmp_server_handler_t
{
	/// aio transport close
	/// @param[in] ptr create by onpublish/onplay
	void (*onclose)(aio_rtmp_userptr_t ptr);

	///push(client -> server)
	///@param[in] type: live/record/append
	///@return user-defined pointer, for onvideo/onaudio first parameter
	aio_rtmp_userptr_t (*onpublish)(void* param, aio_rtmp_session_t* session, const char* app, const char* stream, const char* type);
	///@param[in] data FLV VideoTagHeader + AVCVIDEOPACKET: AVCDecoderConfigurationRecord(ISO 14496-15) / One or more NALUs(four-bytes length + NALU)
	int (*onvideo)(aio_rtmp_userptr_t ptr, const void* data, size_t bytes, uint32_t timestamp);
	///@param[in] data FLV AudioTagHeader + AACAUDIODATA: AudioSpecificConfig(14496-3) / Raw AAC frame data in UI8
	int (*onaudio)(aio_rtmp_userptr_t ptr, const void* data, size_t bytes, uint32_t timestamp);
	///@param[in] data AMF0/AMF3 script
	int (*onscript)(aio_rtmp_userptr_t ptr, const void* data, size_t bytes, uint32_t timestamp);

	///pull(server -> client)
	///@return user-defined pointer, for onpause/onseek/onsend first parameter
	aio_rtmp_userptr_t (*onplay)(void* param, aio_rtmp_session_t* session, const char* app, const char* stream, double start, double duration, uint8_t reset);
	int (*onpause)(aio_rtmp_userptr_t ptr, int pause, uint32_t ms);
	int (*onseek)(aio_rtmp_userptr_t ptr, uint32_t ms);

	/// aio_rtmp_server_send_audio/aio_rtmp_server_send_video callback
	void (*onsend)(aio_rtmp_userptr_t ptr, size_t bytes);

	///@param[in] param aio_rtmp_server_create param
	///@param[out] duration stream length in seconds
	///@return 0-ok, other-error
	int (*ongetduration)(void* param, const char* app, const char* stream, double* duration);
};

aio_rtmp_server_t* aio_rtmp_server_create(const char* ip, int port, struct aio_rtmp_server_handler_t* handler, void* param);
int aio_rtmp_server_destroy(aio_rtmp_server_t* server);

/// @param[in] session oncreate session parameter
int aio_rtmp_server_send_audio(aio_rtmp_session_t* session, const void* flv, size_t bytes, uint32_t timestamp);
int aio_rtmp_server_send_video(aio_rtmp_session_t* session, const void* flv, size_t bytes, uint32_t timestamp);
int aio_rtmp_server_send_script(aio_rtmp_session_t* session, const void* flv, size_t bytes, uint32_t timestamp);

size_t aio_rtmp_server_get_unsend(aio_rtmp_session_t* session);
int aio_rtmp_server_get_addr(aio_rtmp_session_t* session, char ip[65], unsigned short* port);

#ifdef __cplusplus
}
#endif
#endif /* !_aio_rtmp_server_h_ */

#ifndef _aio_rtmp_transport_h_
#define _aio_rtmp_transport_h_

#include "aio-transport.h"
#include <stdint.h>
#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct aio_rtmp_transport_t aio_rtmp_transport_t;

struct aio_rtmp_handler_t
{
	/// aio_rtmp_transport_t object destroy
	/// @param[in] param aio_rtmp_transport_create param
	void (*ondestroy)(void* param);

	/// @param[in] param aio_rtmp_transport_create param
	/// @param[in] data rtmp data
	/// @param[in] bytes >= 0
	void (*onrecv)(void* param, int code, const void* data, size_t bytes);

	/// @param[in] param aio_rtmp_transport_create param
	/// @param[in] bytes remain unsent bytes, 0-all sent
	void (*onsend)(void* param, int code, size_t bytes);
};

/// @param[in] socket CONNECTED aio socket handle
/// @param[in] handler tcp data send/recv handler
/// @param[in] param user-defined parameter
/// @return NULL-failed, other-transport
aio_rtmp_transport_t* aio_rtmp_transport_create(aio_socket_t socket, struct aio_rtmp_handler_t* handler, void* param);
int aio_rtmp_transport_destroy(aio_rtmp_transport_t* transport);

/// start/stop rtmp recv/send (CALL one-time only)
int aio_rtmp_transport_start(aio_rtmp_transport_t* transport);

int aio_rtmp_transport_send(aio_rtmp_transport_t* transport, const void* header, size_t len, const void* payload, size_t bytes);

size_t aio_rtmp_transport_get_unsend(aio_rtmp_transport_t* transport);

/// set recv/send timeout in ms(default 2min, 0-infinite)
void aio_rtmp_transport_set_timeout(aio_rtmp_transport_t* transport, int recv, int send);

#endif /* !_aio_rtmp_transport_h_ */

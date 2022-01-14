#include "aio-rtmp-server.h"
#include "aio-rtmp-transport.h"
#include "sockutil.h"
#include "sys/atomic.h"
#include "sys/locker.h"
#include "rtmp-server.h"
#include "aio-accept.h"
#include <stdlib.h>
#include <assert.h>
#include <stdio.h>

#define TIMEOUT_RECV 20000
#define TIMEOUT_SEND 20000

struct aio_rtmp_server_t
{
	void* listen; // listen aio socket

	struct aio_rtmp_server_handler_t handle;
	void* param;
};

struct aio_rtmp_session_t
{
	locker_t locker;
	socklen_t salen;
	struct sockaddr_storage sa;
	
	aio_rtmp_transport_t* aio; // aio rtmp transport
	aio_rtmp_userptr_t usr; // user-defined parameter(return by oncreate)
	rtmp_server_t* rtmp; // create by rtmp_server_create
	struct aio_rtmp_server_t* server;
};

static void aio_rtmp_server_onaccept(void* param, int code, socket_t socket, const struct sockaddr* sa, socklen_t salen);

static void rtmp_session_ondestroy(void* param);
static void rtmp_session_onsend(void* param, int code, size_t bytes);
static void rtmp_session_onrecv(void* param, int code, const void* data, size_t bytes);

static int rtmp_handler_send(void* param, const void* header, size_t len, const void* payload, size_t bytes);
static int rtmp_handler_onpublish(void* param, const char* app, const char* stream, const char* type);
static int rtmp_handler_onscript(void* param, const void* data, size_t bytes, uint32_t timestamp);
static int rtmp_handler_onaudio(void* param, const void* data, size_t bytes, uint32_t timestamp);
static int rtmp_handler_onvideo(void* param, const void* data, size_t bytes, uint32_t timestamp);
static int rtmp_handler_onplay(void* param, const char* app, const char* stream, double start, double duration, uint8_t reset);
static int rtmp_handler_onpause(void* param, int pause, uint32_t ms);
static int rtmp_handler_onseek(void* param, uint32_t ms);
static int rtmp_handler_ongetduration(void* param, const char* app, const char* stream, double* duration);

static void aio_rtmp_server_ondestroy(void* param)
{
	struct aio_rtmp_server_t *server;
	server = (struct aio_rtmp_server_t*)param;
	free(server);
}

struct aio_rtmp_server_t* aio_rtmp_server_create(const char* ip, int port, struct aio_rtmp_server_handler_t* handler, void* param)
{
	socket_t socket;
	struct aio_rtmp_server_t *ctx;

	// create server socket
	socket = socket_tcp_listen(0 /*AF_UNSPEC*/, ip, (u_short)port, SOMAXCONN, 0, 1);
	if (socket_invalid == socket)
	{
		printf("%s(%s, %d): create socket error: %d\n", __FUNCTION__, ip, port, socket_geterror());
		return NULL;
	}

	ctx = (struct aio_rtmp_server_t*)malloc(sizeof(*ctx));
	ctx->param = param;
	memcpy(&ctx->handle, handler, sizeof(ctx->handle));
	ctx->listen = aio_accept_start(socket, aio_rtmp_server_onaccept, ctx);
	if (!ctx->listen)
	{
		printf("%s(%s, %d) create aio transport error.\n", __FUNCTION__, ip, port);
		aio_rtmp_server_destroy(ctx);
		socket_close(socket);
		return NULL;
	}

	return ctx;
}

int aio_rtmp_server_destroy(struct aio_rtmp_server_t *server)
{
	if (server->listen)
	{
		return aio_accept_stop(server->listen, aio_rtmp_server_ondestroy, server);
	}
	else
	{
		aio_rtmp_server_ondestroy(server);
		return 0;
	}
}

int aio_rtmp_server_send_audio(struct aio_rtmp_session_t* session, const void* data, size_t bytes, uint32_t timestamp)
{
	return rtmp_server_send_audio(session->rtmp, data, bytes, timestamp);
}

int aio_rtmp_server_send_video(struct aio_rtmp_session_t* session, const void* data, size_t bytes, uint32_t timestamp)
{
	return rtmp_server_send_video(session->rtmp, data, bytes, timestamp);
}

int aio_rtmp_server_send_script(struct aio_rtmp_session_t* session, const void* data, size_t bytes, uint32_t timestamp)
{
	return rtmp_server_send_script(session->rtmp, data, bytes, timestamp);
}

size_t aio_rtmp_server_get_unsend(struct aio_rtmp_session_t* session)
{
	return aio_rtmp_transport_get_unsend(session->aio);
}

int aio_rtmp_server_get_addr(struct aio_rtmp_session_t* session, char ip[65], unsigned short* port)
{
	return socket_addr_to((struct sockaddr*)&session->sa, session->salen, ip, port);
}

static void aio_rtmp_server_onaccept(void* param, int code, socket_t socket, const struct sockaddr* sa, socklen_t salen)
{
	struct aio_rtmp_server_t* server;
	struct aio_rtmp_session_t* session;
	struct aio_rtmp_handler_t aiohandler;
	struct rtmp_server_handler_t handler;
	server = (struct aio_rtmp_server_t*)param;

	if (0 != code)
	{
		printf("aio_rtmp_server_onaccept code: %d\n", code);
		return;
	}

	socket_setnonblock(socket, 1);
	socket_setnondelay(socket, 1);

	session = (struct aio_rtmp_session_t*)malloc(sizeof(*session));
	if (session)
	{
		session->usr = NULL;
		session->server = server;
		locker_create(&session->locker);
		session->salen = salen < sizeof(session->sa) ? salen : sizeof(session->sa);
		memcpy(&session->sa, sa, session->salen);

		handler.send = rtmp_handler_send;
		handler.onplay = rtmp_handler_onplay;
		handler.onseek = rtmp_handler_onseek;
		handler.onpause = rtmp_handler_onpause;
		handler.onaudio = rtmp_handler_onaudio;
		handler.onvideo = rtmp_handler_onvideo;
		handler.onscript = rtmp_handler_onscript;
		handler.onpublish = rtmp_handler_onpublish;
		handler.ongetduration = server->handle.ongetduration ? rtmp_handler_ongetduration : NULL;
		session->rtmp = rtmp_server_create(session, &handler);

		aiohandler.onrecv = rtmp_session_onrecv;
		aiohandler.onsend = rtmp_session_onsend;
		aiohandler.ondestroy = rtmp_session_ondestroy;
		session->aio = aio_rtmp_transport_create(aio_socket_create(socket, 1), &aiohandler, session);
		aio_rtmp_transport_set_timeout(session->aio, TIMEOUT_RECV, TIMEOUT_SEND);
		aio_rtmp_transport_start(session->aio);
	}
}

static void rtmp_session_ondestroy(void* param)
{
	struct aio_rtmp_session_t* session;
	session = (struct aio_rtmp_session_t*)param;
	
	// notify session stop
	if(session->server->handle.onclose && session->usr)
		session->server->handle.onclose(session->usr);

	if (session->rtmp)
	{
		rtmp_server_destroy(session->rtmp);
		session->rtmp = NULL;
	}

	locker_destroy(&session->locker);
	session->aio = NULL;

#if defined(DEBUG) || defined(_DEBUG)
	memset(session, 0xCC, sizeof(*session));
#endif
	free(session);
}

static void rtmp_session_onrecv(void* param, int code, const void* data, size_t bytes)
{
	struct aio_rtmp_session_t* session;
	session = (struct aio_rtmp_session_t*)param;

	if(0 == code)
		code = rtmp_server_input(session->rtmp, data, bytes);
	if (0 != code)
		aio_rtmp_transport_destroy(session->aio);
}

static void rtmp_session_onsend(void* param, int code, size_t bytes)
{
	struct aio_rtmp_session_t* session;
	session = (struct aio_rtmp_session_t*)param;
	if (0 == code)
	{
		if (session->server->handle.onsend && session->usr)
			session->server->handle.onsend(session->usr, bytes);
	}
	else
	{
		aio_rtmp_transport_destroy(session->aio);
	}
}

static int rtmp_handler_send(void* param, const void* header, size_t len, const void* payload, size_t bytes)
{
	struct aio_rtmp_session_t* session;
	session = (struct aio_rtmp_session_t*)param;
	return aio_rtmp_transport_send(session->aio, header, len, payload, bytes);
}

static int rtmp_handler_onplay(void* param, const char* app, const char* stream, double start, double duration, uint8_t reset)
{
	struct aio_rtmp_session_t* session;
	session = (struct aio_rtmp_session_t*)param;
	session->usr = session->server->handle.onplay(session->server->param, session, app, stream, start, duration, reset);
	return session->usr ? 0 : -1;
}

static int rtmp_handler_onpause(void* param, int pause, uint32_t ms)
{
	struct aio_rtmp_session_t* session;
	session = (struct aio_rtmp_session_t*)param;
	if(session->server->handle.onpause && session->usr)
		return session->server->handle.onpause(session->usr, pause, ms);
	return 0;
}

static int rtmp_handler_onseek(void* param, uint32_t ms)
{
	struct aio_rtmp_session_t* session;
	session = (struct aio_rtmp_session_t*)param;
	if(session->server->handle.onseek && session->usr)
		return session->server->handle.onseek(session->usr, ms);
	return 0;
}

static int rtmp_handler_onpublish(void* param, const char* app, const char* stream, const char* type)
{
	struct aio_rtmp_session_t* session;
	session = (struct aio_rtmp_session_t*)param;
	session->usr = session->server->handle.onpublish(session->server->param, session, app, stream, type);
	return session->usr ? 0 : -1;
}

static int rtmp_handler_onscript(void* param, const void* data, size_t bytes, uint32_t timestamp)
{
	struct aio_rtmp_session_t* session;
	session = (struct aio_rtmp_session_t*)param;
	if (session->server->handle.onscript && session->usr)
		return session->server->handle.onscript(session->usr, data, bytes, timestamp);
	return 0;
}

static int rtmp_handler_onvideo(void* param, const void* data, size_t bytes, uint32_t timestamp)
{
	struct aio_rtmp_session_t* session;
	session = (struct aio_rtmp_session_t*)param;
	if(session->server->handle.onvideo && session->usr)
		return session->server->handle.onvideo(session->usr, data, bytes, timestamp);
	return 0;
}

static int rtmp_handler_onaudio(void* param, const void* data, size_t bytes, uint32_t timestamp)
{
	struct aio_rtmp_session_t* session;
	session = (struct aio_rtmp_session_t*)param;
	if(session->server->handle.onaudio && session->usr)
		return session->server->handle.onaudio(session->usr, data, bytes, timestamp);
	return 0;
}

static int rtmp_handler_ongetduration(void* param, const char* app, const char* stream, double* duration)
{
	struct aio_rtmp_session_t* session;
	session = (struct aio_rtmp_session_t*)param;
	if (session->server->handle.ongetduration)
		return session->server->handle.ongetduration(session->server->param, app, stream, duration);
	return -1;
}

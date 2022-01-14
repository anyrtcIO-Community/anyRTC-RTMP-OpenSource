#include "aio-rtmp-client.h"
#include "aio-rtmp-transport.h"
#include "rtmp-client.h"
#include "sockutil.h"
#include <stdlib.h>
#include <string.h>
#include <assert.h>

#define TIMEOUT_RECV 5000
#define TIMEOUT_SEND 5000

struct aio_rtmp_client_t
{
	int ready;
	int publish;

	aio_rtmp_transport_t* aio;
	rtmp_client_t* rtmp;

	void* param;
	struct aio_rtmp_client_handler_t handler;
};

static void aio_rtmp_transport_ondestroy(void* param);
static void aio_rtmp_transport_onsend(void* param, int code, size_t bytes);
static void aio_rtmp_transport_onrecv(void* param, int code, const void* data, size_t bytes);
static int rtmp_client_send(void* param, const void* header, size_t len, const void* payload, size_t bytes);
static int rtmp_client_onaudio(void* param, const void* audio, size_t bytes, uint32_t timestamp);
static int rtmp_client_onvideo(void* param, const void* video, size_t bytes, uint32_t timestamp);
static int rtmp_client_onscript(void* param, const void* script, size_t bytes, uint32_t timestamp);

struct aio_rtmp_client_t* aio_rtmp_client_create(aio_socket_t aio, const char* app, const char* stream, const char* tcurl, struct aio_rtmp_client_handler_t* handler, void* param)
{
	struct aio_rtmp_client_t* c;
	struct aio_rtmp_handler_t h;
	struct rtmp_client_handler_t h2;
	c = (struct aio_rtmp_client_t*)calloc(1, sizeof(*c));
	if (c)
	{
		memcpy(&c->handler, handler, sizeof(c->handler));
		c->param = param;
		
		h2.send = rtmp_client_send;
		h2.onaudio = rtmp_client_onaudio;
		h2.onvideo = rtmp_client_onvideo;
		h2.onscript = rtmp_client_onscript;
		c->rtmp = rtmp_client_create(app, stream, tcurl, c, &h2);

		h.onrecv = aio_rtmp_transport_onrecv;
		h.onsend = aio_rtmp_transport_onsend;
		h.ondestroy = aio_rtmp_transport_ondestroy;
		c->aio = aio_rtmp_transport_create(aio, &h, c);

		aio_rtmp_transport_set_timeout(c->aio, TIMEOUT_RECV, TIMEOUT_SEND);
	}
	return c;
}

void aio_rtmp_client_destroy(struct aio_rtmp_client_t* client)
{
	aio_rtmp_transport_destroy(client->aio);
}

int aio_rtmp_client_start(struct aio_rtmp_client_t* client, int publish)
{
	aio_rtmp_transport_start(client->aio);

	client->publish = publish;
	return rtmp_client_start(client->rtmp, publish);
}

int aio_rtmp_client_stop(struct aio_rtmp_client_t* client)
{
	return rtmp_client_stop(client->rtmp);
}

int aio_rtmp_client_pause(struct aio_rtmp_client_t* client, int pause)
{
	return rtmp_client_pause(client->rtmp, pause);
}

int aio_rtmp_client_seek(struct aio_rtmp_client_t* client, double timestamp)
{
	return rtmp_client_seek(client->rtmp, timestamp);
}

int aio_rtmp_client_send_audio(struct aio_rtmp_client_t* client, const void* flv, size_t bytes, uint32_t timestamp)
{
	return rtmp_client_push_audio(client->rtmp, flv, bytes, timestamp);
}

int aio_rtmp_client_send_video(struct aio_rtmp_client_t* client, const void* flv, size_t bytes, uint32_t timestamp)
{
	return rtmp_client_push_video(client->rtmp, flv, bytes, timestamp);
}

int aio_rtmp_client_send_script(struct aio_rtmp_client_t* client, const void* flv, size_t bytes, uint32_t timestamp)
{
	return rtmp_client_push_script(client->rtmp, flv, bytes, timestamp);
}

size_t aio_rtmp_client_get_unsend(aio_rtmp_client_t* client)
{
	return aio_rtmp_transport_get_unsend(client->aio);
}

static int rtmp_client_send(void* param, const void* header, size_t len, const void* payload, size_t bytes)
{
	struct aio_rtmp_client_t* client;
	client = (struct aio_rtmp_client_t*)param;
	return aio_rtmp_transport_send(client->aio, header, len, payload, bytes);
}

static int rtmp_client_onaudio(void* param, const void* audio, size_t bytes, uint32_t timestamp)
{
	struct aio_rtmp_client_t* client;
	client = (struct aio_rtmp_client_t*)param;
	return client->handler.onaudio(client->param, audio, bytes, timestamp);
}

static int rtmp_client_onvideo(void* param, const void* video, size_t bytes, uint32_t timestamp)
{
	struct aio_rtmp_client_t* client;
	client = (struct aio_rtmp_client_t*)param;
	return client->handler.onvideo(client->param, video, bytes, timestamp);
}

static int rtmp_client_onscript(void* param, const void* script, size_t bytes, uint32_t timestamp)
{
	struct aio_rtmp_client_t* client;
	client = (struct aio_rtmp_client_t*)param;
	return client->handler.onscript(client->param, script, bytes, timestamp);
}

static void aio_rtmp_transport_onsend(void* param, int code, size_t bytes)
{
	struct aio_rtmp_client_t* client;
	client = (struct aio_rtmp_client_t*)param;
	if (0 == code)
	{
		if (client->handler.onsend)
			client->handler.onsend(client->param, bytes);
	}
	else
	{
		client->handler.onerror(client->param, code);
	}
}

static void aio_rtmp_transport_onrecv(void* param, int code, const void* data, size_t bytes)
{
	struct aio_rtmp_client_t* client;
	client = (struct aio_rtmp_client_t*)param;

	if (0 == code)
	{
		code = rtmp_client_input(client->rtmp, data, bytes);
		if (0 == client->publish && 0 == client->ready && 4/*RTMP_STATE_START*/ == rtmp_client_getstate(client->rtmp))
		{
			client->ready = 1;
			if (client->handler.onready)
				client->handler.onready(client->param);
		}
	}

	if (0 != code)
		client->handler.onerror(client->param, code);
}

static void aio_rtmp_transport_ondestroy(void* param)
{
	struct aio_rtmp_client_t* client;
	client = (struct aio_rtmp_client_t*)param;
	if (client->handler.ondestroy)
		client->handler.ondestroy(client->param);
	if (client->rtmp)
		rtmp_client_destroy(client->rtmp);
	free(client);
}

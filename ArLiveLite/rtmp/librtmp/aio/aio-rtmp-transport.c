#include "aio-rtmp-transport.h"
#include "aio-transport.h"
#include "sys/sock.h"
#include "sys/locker.h"
#include "sys/system.h"
#include "list.h"
#include <errno.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

#define VEC 8

struct aio_rtmp_chunk_t
{
	struct list_head node;
	uint8_t* data;
	size_t size;
};

struct aio_rtmp_transport_t
{
	int code;

	int vecsize;
	socket_bufvec_t vec[VEC];
	aio_transport_t* aio;
	char buffer[2 * 1024];

	locker_t locker;
	int count; // list size
	size_t bytes; // list total bytes
	struct list_head root;

	struct aio_rtmp_handler_t handler;
	void* param;
};

static int aio_rtmp_send(struct aio_rtmp_transport_t* t)
{
	struct list_head* p;
	struct aio_rtmp_chunk_t* c;

	locker_lock(&t->locker);
	if (0 != t->vecsize /*sending*/ || 0 == t->count /*no more data*/)
	{
		locker_unlock(&t->locker);
		return 0;
	}

	assert(0 == t->vecsize);
	for (p = t->root.next; t->vecsize < VEC && t->vecsize < t->count; t->vecsize++)
	{
		assert(p != &t->root);
		c = list_entry(p, struct aio_rtmp_chunk_t, node);
		socket_setbufvec(t->vec, t->vecsize, c->data, c->size);
		p = p->next;
	}

	assert(t->vecsize > 0);
	locker_unlock(&t->locker);

	return aio_transport_send_v(t->aio, t->vec, t->vecsize);
}

static void aio_rtmp_ondestroy(void* param)
{
	struct list_head *n, *p;
	struct aio_rtmp_chunk_t* c;
	struct aio_rtmp_transport_t* t;
	t = (struct aio_rtmp_transport_t*)param;

	if(t->handler.ondestroy)
		t->handler.ondestroy(t->param);

	list_for_each_safe(p, n, &t->root)
	{
		c = list_entry(p, struct aio_rtmp_chunk_t, node);
		free(c);
	}

	locker_destroy(&t->locker);
	t->aio = NULL;
	free(t);
}

static void aio_rtmp_onrecv(void* param, int code, size_t bytes)
{
	struct aio_rtmp_transport_t* t;
	t = (struct aio_rtmp_transport_t*)param;

	if (0 == code && 0 == bytes)
		code = ECONNRESET;

	t->handler.onrecv(t->param, code, t->buffer, bytes);
	if (0 == code)
	{
		code = aio_transport_recv(t->aio, t->buffer, sizeof(t->buffer));
		if (0 != code)
			t->handler.onrecv(t->param, code, t->buffer, 0);
	}
}

static void aio_rtmp_onsend(void* param, int code, size_t bytes)
{
	struct aio_rtmp_chunk_t* c;
	struct aio_rtmp_transport_t* t;
	t = (struct aio_rtmp_transport_t*)param;
	
	if (0 == code)
	{
		locker_lock(&t->locker);
		for (assert(t->vecsize > 0); t->vecsize > 0; --t->vecsize)
		{
			assert(!list_empty(&t->root));
			c = list_entry(t->root.next, struct aio_rtmp_chunk_t, node);
			list_remove(t->root.next);
			free(c);
			t->count -= 1;
		}
		t->bytes -= bytes;
		locker_unlock(&t->locker);
	}

	t->handler.onsend(t->param, code, t->bytes); // callback

	if (0 == code)
		code = aio_rtmp_send(t); // send next

	if (0 != code)
	{
		t->code = code;
		t->handler.onsend(t->param, code, 0);
	}
}

struct aio_rtmp_transport_t* aio_rtmp_transport_create(aio_socket_t socket, struct aio_rtmp_handler_t* handler, void* param)
{
	struct aio_rtmp_transport_t* t;
	struct aio_transport_handler_t h;
	h.ondestroy = aio_rtmp_ondestroy;
	h.onrecv = aio_rtmp_onrecv;
	h.onsend = aio_rtmp_onsend;

	t = (struct aio_rtmp_transport_t*)calloc(1, sizeof(*t));
	if (!t) return NULL;

	LIST_INIT_HEAD(&t->root);
	locker_create(&t->locker);
	memcpy(&t->handler, handler, sizeof(t->handler));
	t->param = param;
	t->aio = aio_transport_create2(socket, &h, t);
	return t;
}

int aio_rtmp_transport_destroy(struct aio_rtmp_transport_t* t)
{
	return aio_transport_destroy(t->aio);
}

int aio_rtmp_transport_start(struct aio_rtmp_transport_t* t)
{
	return aio_transport_recv(t->aio, t->buffer, sizeof(t->buffer));
}

int aio_rtmp_transport_send(struct aio_rtmp_transport_t* t, const void* header, size_t len, const void* payload, size_t bytes)
{
	struct aio_rtmp_chunk_t* c;
	if (0 != t->code)
		return -1;

	c = (struct aio_rtmp_chunk_t*)malloc(sizeof(*c) + len + bytes + 12);
	if (NULL == c)
		return -ENOMEM;

	c->data = (uint8_t*)(c + 1);
	c->size = len + bytes;
	if(len > 0) memcpy(c->data, header, len);
	if (bytes > 0) memcpy(c->data + len, payload, bytes);

	locker_lock(&t->locker);
	t->count += 1;
	t->bytes += len + bytes;
	list_insert_before(&c->node, &t->root); // link to end
	locker_unlock(&t->locker);

	return 0 == aio_rtmp_send(t) ? (int)(len + bytes) : -1;
}

size_t aio_rtmp_transport_get_unsend(struct aio_rtmp_transport_t* t)
{
	return t->bytes;
}

void aio_rtmp_transport_set_timeout(struct aio_rtmp_transport_t* t, int recv, int send)
{
	aio_transport_set_timeout(t->aio, recv, send);
}

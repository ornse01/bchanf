/*
 * http_transport.c
 *
 * Copyright (c) 2012 project bchan
 *
 * This software is provided 'as-is', without any express or implied
 * warranty. In no event will the authors be held liable for any damages
 * arising from the use of this software.
 *
 * Permission is granted to anyone to use this software for any purpose,
 * including commercial applications, and to alter it and redistribute it
 * freely, subject to the following restrictions:
 *
 * 1. The origin of this software must not be misrepresented; you must not
 *    claim that you wrote the original software. If you use this software
 *    in a product, an acknowledgment in the product documentation would be
 *    appreciated but is not required.
 *
 * 2. Altered source versions must be plainly marked as such, and must not be
 *    misrepresented as being the original software.
 *
 * 3. This notice may not be removed or altered from any source
 *    distribution.
 *
 */

#include "http_transport.h"

#include	<basic.h>
#include	<bstdio.h>
#include	<bstdlib.h>
#include	<bctype.h>
#include	<errcode.h>
#include	<btron/btron.h>
#include	<btron/bsocket.h>

#include    <coll/idtocb.h>

#ifdef BCHAN_CONFIG_DEBUG
# define DP(arg) printf arg
# define DP_ER(msg, err) printf("%s (%d/%x)\n", msg, err>>16, err)
#else
# define DP(arg) /**/
# define DP_ER(msg, err) /**/
#endif

#if 0
# define DP_STATE(state) printf("%s\n", state)
#else
# define DP_STATE(state) /**/
#endif

#define HTTP_TRANSPORT_CB_RCVBUF_SIZE 1024

struct http_transport_cb_t_ {
	idtocb_entry_t base;
	enum {
		DORMANT,
		HOLDED,
		CLOSED,
	} status;
	Bool reading;
	Bool read_ready;
	SOCKADDR addr;
	W s;
	struct {
		W rcv_len;
		W consume_len;
		UB buf[HTTP_TRANSPORT_CB_RCVBUF_SIZE];
	} buffer;
};
typedef struct http_transport_cb_t_ http_transport_cb_t;

struct http_transport_t_ {
	idtocb_t *base;
};

struct http_transport_iterator_t_ {
	idtocb_iterator_t base;
};
typedef struct http_transport_iterator_t_ http_transport_iterator_t;

LOCAL Bool http_transport_iterator_next(http_transport_iterator_t *iter, http_transport_cb_t **cb)
{
	return idtocb_iterator_next(&iter->base, (idtocb_entry_t**)cb);
}


LOCAL VOID http_transport_iterator_initialize(http_transport_iterator_t *iter, http_transport_t *transport)
{
	idtocb_iterator_initialize(&iter->base, transport->base);
}


LOCAL VOID http_transport_iterator_finalize(http_transport_iterator_t *iter) 
{
	idtocb_iterator_finalize(&iter->base);
}

LOCAL ID http_transport_allocatecb(http_transport_t *transport)
{
	return idtocb_allocate(transport->base);
}

LOCAL VOID http_transport_freecb(http_transport_t *transport, ID id)
{
	idtocb_free(transport->base, id);
}

LOCAL W http_transport_getcontrolblock(http_transport_t *transport, ID id, http_transport_cb_t **ptr)
{
	return idtocb_getcontrolblock(transport->base, id, (idtocb_entry_t**)ptr);
}

EXPORT ID http_transport_prepairendpoint(http_transport_t *transport, SOCKADDR *addr)
{
	ID id;
	W err;
	http_transport_cb_t *cb;

	id = http_transport_allocatecb(transport);
	if (id < 0) {
		DP_ER("http_transport_allocate", id);
		return id;
	}
	err = http_transport_getcontrolblock(transport, id, &cb);
	if (err < 0) {
		DP_ER("http_transport_getcontrollblock", err);
		http_transport_freecb(transport, id);
		return err;
	}
	cb->status = DORMANT;
	cb->reading = False;
	cb->addr = *addr;
	cb->buffer.rcv_len = 0;
	cb->buffer.consume_len = 0;

	cb->s = so_socket(AF_INET, SOCK_STREAM, 0);
	if (cb->s < 0) {
		DP_ER("so_socket", cb->s);
		http_transport_freecb(transport, id);
		return cb->s;
	}

	err = so_connect(cb->s, &(cb->addr), sizeof(SOCKADDR));
	if (err < 0) {
		DP_ER("so_connect", err);
		so_close(cb->s);
		http_transport_freecb(transport, id);
		return err;
	}

	return id;
}

EXPORT ID http_transport_searchendpoint(http_transport_t *transport, SOCKADDR *addr)
{
	http_transport_iterator_t iter;
	http_transport_cb_t *cb;
	Bool cont;
	struct sockaddr_in *ain0, *ain1;

	http_transport_iterator_initialize(&iter, transport);

	for (;;) {
		cont = http_transport_iterator_next(&iter, &cb);
		if (cont == False) {
			break;
		}
		if (cb->status != DORMANT) {
			continue;
		}

		ain0 = (struct sockaddr_in *)&(cb->addr);
		ain1 = (struct sockaddr_in *)addr;
		if (ain0->sin_family != ain1->sin_family) {
			continue;
		}
		if (ain0->sin_port != ain1->sin_port) {
			continue;
		}
		if (memcmp(&ain0->sin_addr, &ain1->sin_addr, sizeof(struct in_addr)) != 0) {
			continue;
		}

		http_transport_iterator_finalize(&iter);
		return cb->base.id;
	}

	http_transport_iterator_finalize(&iter);

	return ER_NOEXS; /* TODO: sub error code */
}

EXPORT VOID http_transport_releaseunusedendpoint(http_transport_t *transport)
{
	http_transport_iterator_t iter;
	http_transport_cb_t *cb;
	Bool cont;

	http_transport_iterator_initialize(&iter, transport);

	for (;;) {
		cont = http_transport_iterator_next(&iter, &cb);
		if (cont == False) {
			break;
		}
		if ((cb->status == DORMANT) || (cb->status == CLOSED)) {
			so_close(cb->s);
			http_transport_freecb(transport, cb->base.id);
		}
	}

	http_transport_iterator_finalize(&iter);
}

EXPORT W http_transport_holdendpoint(http_transport_t *transport, ID endpoint)
{
	http_transport_cb_t *cb;
	W err;

	err = http_transport_getcontrolblock(transport, endpoint, &cb);
	if (err < 0) {
		DP_ER("http_transport_getcontrolblock", err);
		return err;
	}
	if (cb->status != DORMANT) {
		return ER_BUSY; /* TODO: sub error code */
	}
	cb->status = HOLDED;

	return ER_OK;
}

EXPORT W http_transport_isholdedendpoint(http_transport_t *transport, ID endpoint, Bool *is_holded)
{
	http_transport_cb_t *cb;
	W err;

	err = http_transport_getcontrolblock(transport, endpoint, &cb);
	if (err < 0) {
		DP_ER("http_transport_getcontrolblock", err);
		return err;
	}
	if (cb->status != DORMANT) {
		return ER_BUSY; /* TODO: sub error code */
	}
	if (cb->status == HOLDED) {
		*is_holded = True;
	} else {
		*is_holded = False;
	}

	return ER_OK;
}

EXPORT VOID http_transport_releaseendpoint(http_transport_t *transport, ID endpoint)
{
	http_transport_cb_t *cb;
	W err;

	err = http_transport_getcontrolblock(transport, endpoint, &cb);
	if (err < 0) {
		DP_ER("http_transport_getcontrolblock", err);
		return;
	}
	if (cb->status != HOLDED) {
		return;
	}
	cb->status = DORMANT;
}

EXPORT W http_transport_setwaitingreceive(http_transport_t *transport, ID endpoint)
{
	http_transport_cb_t *cb;
	W err;

	err = http_transport_getcontrolblock(transport, endpoint, &cb);
	if (err < 0) {
		DP_ER("http_transport_getcontrolblock", err);
		return err;
	}
	if (cb->status != HOLDED) {
		return ER_BUSY; /* TODO: sub error code */
	}
	cb->reading = True;
	cb->read_ready = False;

	return ER_OK;
}

EXPORT W http_transport_waitreceive(http_transport_t *transport, TMOUT tmout)
{
	http_transport_iterator_t iter;
	http_transport_cb_t *cb;
	Bool cont;
	W err, s = -1;
	fd_set rfds;
	TIMEVAL *timep = NULL, time0;

	FD_ZERO(&rfds);

	http_transport_iterator_initialize(&iter, transport);
	for (;;) {
		cont = http_transport_iterator_next(&iter, &cb);
		if (cont == False) {
			break;
		}
		if ((cb->status == HOLDED) && (cb->reading != False)) {
			if (cb->s > s) {
				s = cb->s;
			}
			FD_SET(cb->s, &rfds);
		}
	}
	http_transport_iterator_finalize(&iter);

	if (s < 0) {
		return E_OK;
	}

	if (tmout != T_FOREVER) {
		if (tmout == T_NOWAIT) {
			time0.tv_sec = 0;
			time0.tv_usec = 0;
		} else {
			time0.tv_sec = tmout / 1000;
			time0.tv_usec = (tmout % 1000) * 1000;
		}
		timep = &time0;
	}
	err = so_select(s + 1, &rfds, NULL, NULL, timep);
	if (err == 0) {
		return ER_NONE;
	}
	if (err < 0) {
		return err;
	}

	http_transport_iterator_initialize(&iter, transport);
	for (;;) {
		cont = http_transport_iterator_next(&iter, &cb);
		if (cont == False) {
			break;
		}
		if ((cb->status == HOLDED) && (cb->reading != False)) {
			cb->reading = False;
			if (FD_ISSET(cb->s, &rfds)) {
				cb->read_ready = True;
			}
		}
	}
	http_transport_iterator_finalize(&iter);

	return err; /* number of ready */
}

EXPORT W http_transport_isreadready(http_transport_t *transport, ID endpoint, Bool *is_ready)
{
	http_transport_cb_t *cb;
	W err;

	err = http_transport_getcontrolblock(transport, endpoint, &cb);
	if (err < 0) {
		DP_ER("http_transport_getcontrolblock", err);
		return err;
	}

	*is_ready = cb->read_ready;
	return ER_OK;
}

EXPORT W http_transport_write(http_transport_t *transport, ID endpoint, CONST UB *bin, W len)
{
	http_transport_cb_t *cb;
	W err;

	err = http_transport_getcontrolblock(transport, endpoint, &cb);
	if (err < 0) {
		DP_ER("http_transport_getcontrolblock", err);
		return err;
	}
	if (cb->status != HOLDED) {
		return ER_BUSY; /* TODO: sub error code */
	}
	err = so_write(cb->s, (UB*)bin, len);
	if ((err & 0xFFFF0000) == EX_CONNABORTED) {
		cb->status = CLOSED;
	}
	if (err < 0) {
		DP_ER("so_write", err);
		return err;
	}

	return ER_OK;
}

EXPORT W http_transport_read(http_transport_t *transport, ID endpoint, UB **bin, W *len)
{
	http_transport_cb_t *cb;
	W err;

	err = http_transport_getcontrolblock(transport, endpoint, &cb);
	if (err < 0) {
		DP_ER("http_transport_getcontrolblock", err);
		return err;
	}
	if (cb->status != HOLDED) {
		return ER_BUSY; /* TODO: sub error code */
	}

	cb->read_ready = False; /* should be other timing? */
	if (cb->buffer.consume_len < cb->buffer.rcv_len) {
		*bin = cb->buffer.buf + cb->buffer.consume_len;
		*len = cb->buffer.rcv_len - cb->buffer.consume_len;
	} else {
		err = so_read(cb->s, cb->buffer.buf, HTTP_TRANSPORT_CB_RCVBUF_SIZE);
		if ((err & 0xFFFF0000) == EX_CONNABORTED) {
			cb->status = CLOSED;
		}
		if (err < 0) {
			DP_ER("so_read", err);
			return err;
		}
		cb->buffer.rcv_len = err;
		cb->buffer.consume_len = 0;
		*bin = cb->buffer.buf;
		*len = cb->buffer.rcv_len;
	}

	return ER_OK;
}

EXPORT W http_transport_consumereadbuffer(http_transport_t *transport, ID endpoint, W consumed_len)
{
	http_transport_cb_t *cb;
	W err;

	err = http_transport_getcontrolblock(transport, endpoint, &cb);
	if (err < 0) {
		DP_ER("http_transport_getcontrolblock", err);
		return err;
	}
	if (cb->status != HOLDED) {
		return ER_BUSY; /* TODO: sub error code */
	}

	cb->buffer.consume_len += consumed_len;

	return ER_OK;
}

EXPORT http_transport_t* http_transport_new(W max_endpoints)
{
	http_transport_t *ret;

	ret = malloc(sizeof(http_transport_t));
	if (ret == NULL) {
		return NULL;
	}
	ret->base = idtocb_new(sizeof(http_transport_cb_t), max_endpoints);
	if (ret->base == NULL) {
		free(ret);
		return NULL;
	}
	return ret;
}

EXPORT VOID http_transport_delete(http_transport_t *transport)
{
	http_transport_iterator_t iter;
	http_transport_cb_t *cb;
	Bool cont;

	http_transport_iterator_initialize(&iter, transport);
	for (;;) {
		cont = http_transport_iterator_next(&iter, &cb);
		if (cont == False) {
			break;
		}
		so_close(cb->s);
		http_transport_freecb(transport, cb->base.id);
	}
	http_transport_iterator_finalize(&iter);

	idtocb_delete(transport->base);
	free(transport);
}

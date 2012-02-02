/*
 * http_connector.c
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

#include "http_connector.h"

#include	<basic.h>
#include	<bstdio.h>
#include	<bstdlib.h>
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

struct http_reqentry_t_ {
	idtocb_entry_t base;
	enum {
		NON_EXISTENT,
		WAITING_TRANSPORT,
		SENDING_REQUEST,
		WAITING_RESPONSE,
		RECEIVING_RESPONSE,
		ABORTED_BY_TRANSPORT,
		COMPLETED
	} status;
	enum {
		SEND_REQUEST_LINE,
		SEND_HEADER_MINE,
		SEND_HEADER_USER,
		SEND_MESSAGE_BODY,
	} snd_state;
	enum {
		RECEIVE_STATUS_LINE,
		RECEIVE_HEADER,
		RECEIVE_HEADER_END,
		RECEIVE_MESSAGE_BODY,
		RECEIVE_MESSAGE_END,
	} rcv_state;
	Bool aborted_by_user;
	UB *host;
	W host_len;
	SOCKADDR addr;
};
typedef struct http_reqentry_t_ http_reqentry_t;

LOCAL VOID http_reqentry_attachendpoint(http_reqentry_t *entry)
{
	entry->status = SENDING_REQUEST;
	/* TODO */
	return;
}

LOCAL VOID http_reqentry_detachendpoint(http_reqentry_t *entry)
{
	entry->status = WAITING_TRANSPORT;
	/* TODO */
	return;
}

LOCAL W http_reqentry_initialize(http_reqentry_t *entry, UB *host, W host_len, UH port)
{
	W err;
	B buf[HBUFLEN];
	HOSTENT ent;
	struct sockaddr_in *addr_in;

	entry->host = malloc(sizeof(UB)*(host_len+1));
	if (entry->host == NULL) {
		err = ER_NOMEM; /* TODO: detail error code */
		goto error_host;
	}
	memcpy(entry->host, host, host_len);
	entry->host[host_len] = '\0';
	entry->host_len = host_len;

	err = so_gethostbyname(entry->host, &ent, buf);
	if (err < 0) {
		goto error_gethostbyname;
	}
	addr_in = (struct sockaddr_in *)&(entry->addr);
	addr_in->sin_family = AF_INET;
	addr_in->sin_port = htons( port );
	addr_in->sin_addr.s_addr = *(unsigned int *)(ent.h_addr_list[0]);

	entry->aborted_by_user = False;
	entry->status = WAITING_TRANSPORT;
	entry->snd_state = SEND_REQUEST_LINE;
	entry->rcv_state = RECEIVE_STATUS_LINE;

	return 0;

error_gethostbyname:
	free(entry->host);
error_host:
	entry->status = NON_EXISTENT;
	return err;
}

LOCAL VOID http_reqentry_finalize(http_reqentry_t *entry)
{
	if (entry->host != NULL) {
		free(entry->host);
	}
}

struct http_reqdict_t_ {
	idtocb_t *base;
	W entries;
	http_reqentry_t entry[1];
};
typedef struct http_reqdict_t_ http_reqdict_t;

typedef struct {
	idtocb_iterator_t base;
} http_recdictiterator_t;

LOCAL Bool http_reqdictiterator_next(http_recdictiterator_t *iter, http_reqentry_t **entry)
{
	return idtocb_iterator_next(&iter->base, (idtocb_entry_t**)entry);
}

LOCAL VOID http_reqdictiterator_initialize(http_recdictiterator_t *iter, http_reqdict_t *dict)
{
	idtocb_iterator_initialize(&iter->base, dict->base);
}

LOCAL VOID http_reqdictiterator_finalize(http_recdictiterator_t *iter)
{
	idtocb_iterator_finalize(&iter->base);
}

LOCAL ID http_reqdict_allocate(http_reqdict_t *dict, UB *host, W host_len, UH port)
{
	ID id;
	http_reqentry_t *cb;
	W err;

	id = idtocb_allocate(dict->base);
	if (id < 0) {
		return id;
	}
	err = idtocb_getcontrolblock(dict->base, id, (idtocb_entry_t**)&cb);
	if (err < 0) {
		idtocb_free(dict->base, id);
		return err;
	}

	err = http_reqentry_initialize(cb, host, host_len, port);
	if (err < 0) {
		idtocb_free(dict->base, id);
		return err;
	}

	return id;
}

LOCAL http_reqentry_t* http_reqdict_getentrybyID(http_reqdict_t *dict, ID id)
{
	http_reqentry_t *entry;
	W err;

	err = idtocb_getcontrolblock(dict->base, id, (idtocb_entry_t**)&entry);
	if (err < 0) {
		return NULL;
	}
	return entry;
}

LOCAL VOID http_reqdict_free(http_reqdict_t *dict, ID id)
{
	http_reqentry_t *entry;
	W err;

	err = idtocb_getcontrolblock(dict->base, id, (idtocb_entry_t**)&entry);
	if (err < 0) {
		return;
	}
	http_reqentry_finalize(entry);
	idtocb_free(dict->base, id);
}

LOCAL http_reqdict_t* http_reqdict_new(W max_entries)
{
	http_reqdict_t *dict;

	dict = (http_reqdict_t*)malloc(sizeof(http_reqdict_t));
	if (dict == NULL) {
		return NULL;
	}
	dict->base = idtocb_new(sizeof(http_reqentry_t), max_entries);
	if (dict->base == NULL) {
		free(dict);
		return NULL;
	}
	return dict;
}

LOCAL VOID http_reqdict_delete(http_reqdict_t *dict)
{
	http_recdictiterator_t iter;
	http_reqentry_t *entry;
	Bool cont;

	http_reqdictiterator_initialize(&iter, dict);
	for (;;) {
		cont = http_reqdictiterator_next(&iter, &entry);
		if (cont == False) {
			break;
		}
		http_reqentry_finalize(entry);
	}
	http_reqdictiterator_finalize(&iter);

	idtocb_delete(dict->base);
	free(dict);
}

struct http_connector_t_ {
	http_reqdict_t *dict;
	ID flg;
};

#define HTTP_CONNECTOR_FLAG_CREARMASK(flag) (~(flag))
#define HTTP_CONNECTOR_FLAG_REQUEST 0x00000001
#define HTTP_CONNECTOR_FLAG_EVENT   0x00000002
#define HTTP_CONNECTOR_FLAG_CLEARMASK_REQUEST HTTP_CONNECTOR_FLAG_CREARMASK(HTTP_CONNECTOR_FLAG_REQUEST)
#define HTTP_CONNECTOR_FLAG_CLEARMASK_EVENT HTTP_CONNECTOR_FLAG_CREARMASK(HTTP_CONNECTOR_FLAG_EVENT)

EXPORT ID http_connector_createendpoint(http_connector_t *connector, UB *host, W host_len, UH port)
{
	ID id;
	W err;

	id = http_reqdict_allocate(connector->dict, host, host_len, port);
	if (id < 0) {
		DP_ER("http_reqdict_allocate", id);
		return id; /* TODO */
	}

	err = set_flg(connector->flg, HTTP_CONNECTOR_FLAG_REQUEST);
	if (err < 0) {
		DP_ER("set_flg", err);
		http_reqdict_free(connector->dict, id);
		return err;
	}

	return id;
}

EXPORT VOID http_connector_deleteendpoint(http_connector_t *connector, ID endpoint)
{
	http_reqentry_t *entry;

	entry = http_reqdict_getentrybyID(connector->dict, endpoint);
	if (entry == NULL) {
		return;
	}
	entry->aborted_by_user = True;
}

LOCAL VOID http_connector_collect(http_connector_t *connector)
{
	http_reqentry_t *entry;
	http_recdictiterator_t iter;
	Bool cont;

	http_reqdictiterator_initialize(&iter, connector->dict);
	for (;;) {
		cont = http_reqdictiterator_next(&iter, &entry);
		if (cont == False) {
			break;
		}
		if (entry->status == ABORTED_BY_TRANSPORT) {
			http_reqdict_free(connector->dict, entry->base.id);
		} else if (entry->status == COMPLETED) {
			/* TODO: release transport endpoint */
			http_reqdict_free(connector->dict, entry->base.id);
		}
	}
	http_reqdictiterator_finalize(&iter);
}

LOCAL W http_connector_searchwaiting(http_connector_t *connector)
{
	W ret = 0;
	http_reqentry_t *entry;
	http_recdictiterator_t iter;
	Bool cont;

	http_reqdictiterator_initialize(&iter, connector->dict);
	for (;;) {
		cont = http_reqdictiterator_next(&iter, &entry);
		if (cont == False) {
			break;
		}
		if (entry->status == WAITING_TRANSPORT) {
			/* TODO: transport end point search */
			/* TODO: transport end point status change: HOLDED and sending */
			http_reqentry_attachendpoint(entry);
			ret++;
		}
	}
	http_reqdictiterator_finalize(&iter);

	return ret;
}

LOCAL W http_connector_waitreceiving(http_connector_t *connector)
{
	W ret = 0;
	http_reqentry_t *entry;
	http_recdictiterator_t iter;
	Bool cont;

	http_reqdictiterator_initialize(&iter, connector->dict);
	for (;;) {
		cont = http_reqdictiterator_next(&iter, &entry);
		if (cont == False) {
			break;
		}
		if (entry->status == WAITING_RESPONSE) {
			/* TODO: end point status change: readinging */
			/* TODO: select() for end point */
			entry->status = RECEIVING_RESPONSE;
			ret++;
		}
	}
	http_reqdictiterator_finalize(&iter);

	return ret;
}

EXPORT W http_connector_waitconnection(http_connector_t *connector, TMOUT tmout)
{
	W err;
	Bool evt = False;

	/**/
	http_connector_collect(connector);

	err = http_connector_searchwaiting(connector);
	if (err < 0) {
		return err;
	}
	if (err > 0) {
		evt = True;
	}

	err = http_connector_waitreceiving(connector);
	if (err < 0) {
		return err;
	}
	if (err > 0) {
		evt = True;
	}

	if (evt != False) {
		err = set_flg(connector->flg, HTTP_CONNECTOR_FLAG_EVENT);
		if (err < 0) {
			DP_ER("set_flg", err);
		}
	}

	return 0;
}

EXPORT Bool http_connector_searcheventtarget(http_connector_t *connector, http_connector_event *event)
{
	http_reqentry_t *entry;
	http_recdictiterator_t iter;
	Bool cont, found = False;

	http_reqdictiterator_initialize(&iter, connector->dict);
	for (;;) {
		cont = http_reqdictiterator_next(&iter, &entry);
		if (cont == False) {
			break;
		}
		if (entry->status == SENDING_REQUEST) {
			event->type = HTTP_CONNECTOR_EVENTTYPE_SEND;
			event->endpoint = entry->base.id;
			found = True;
			break;
		}
		if (entry->status == RECEIVING_RESPONSE) {
			event->type = HTTP_CONNECTOR_EVENTTYPE_RECEIVE_MESSAGEBODY_END;
			event->endpoint = entry->base.id;
			entry->status = COMPLETED;
			found = True;
			break;
		}
	}
	http_reqdictiterator_finalize(&iter);

	return found;
}

EXPORT W http_connector_getevent(http_connector_t *connector, http_connector_event *event)
{
	W err;
	Bool found;

	err = wai_flg(connector->flg, HTTP_CONNECTOR_FLAG_EVENT, WF_AND, T_NOWAIT);
	if ((err & 0xFFFF0000) == ER_NONE) {
		return err;
	}
	if (err < 0) {
		DP_ER("wai_flg", err);
		return err;
	}
	found = http_connector_searcheventtarget(connector, event);
	if (found == False) {
		err = clr_flg(connector->flg, HTTP_CONNECTOR_FLAG_CLEARMASK_EVENT);
		if (err < 0) {
			DP_ER("clr_flg", err);
			return err;
		}
		return ER_NONE; /* TODO: detail error code */
	}
	return 0;
}

#define HTTP_CONNECTOR_SENDXXX_GET_CHECK(connector, endpoint, state, entry) \
	(entry) = http_reqdict_getentrybyID((connector)->dict, (endpoint)); \
	if ((entry) == NULL) { \
		return ER_NOEXS; /* TODO: detail error code */ \
	} \
	if ((entry)->status != SENDING_REQUEST) { \
		return ER_PAR; /* TODO: suitable error code */ \
	} \
	if ((entry)->snd_state != (state)) { \
		return ER_PAR; /* TODO: suitable error code */ \
	} \

#define HTTP_CONNECTOR_SENDXXX_GET_CHECK_2(connector, endpoint, state1, state2, entry) \
	(entry) = http_reqdict_getentrybyID((connector)->dict, (endpoint)); \
	if ((entry) == NULL) { \
		return ER_NOEXS; /* TODO: detail error code */ \
	} \
	if ((entry)->status != SENDING_REQUEST) { \
		return ER_PAR; /* TODO: suitable error code */ \
	} \
	if (((entry)->snd_state != (state1))&&((entry)->snd_state != (state2))) { \
		return ER_PAR; /* TODO: suitable error code */ \
	} \

EXPORT W http_connector_sendrequestline(http_connector_t *connector, ID endpoint, UB *path, W path_len)
{
	http_reqentry_t *entry;

	HTTP_CONNECTOR_SENDXXX_GET_CHECK(connector, endpoint, SEND_REQUEST_LINE, entry);

	entry->snd_state = SEND_HEADER_MINE;

	return 0;
}

EXPORT W http_connector_sendheader(http_connector_t *connector, ID endpoint, UB *p, W len)
{
	http_reqentry_t *entry;

	HTTP_CONNECTOR_SENDXXX_GET_CHECK_2(connector, endpoint, SEND_HEADER_MINE, SEND_HEADER_USER, entry);

	if (entry->snd_state == SEND_HEADER_MINE) {
		entry->snd_state = SEND_HEADER_USER;
	}

	return 0;
}

EXPORT W http_connector_sendheaderend(http_connector_t *connector, ID endpoint)
{
	http_reqentry_t *entry;

	HTTP_CONNECTOR_SENDXXX_GET_CHECK_2(connector, endpoint, SEND_HEADER_MINE, SEND_HEADER_USER, entry);

	if (entry->snd_state == SEND_HEADER_MINE) {
	}

	entry->snd_state = SEND_MESSAGE_BODY;

	return 0;
}

EXPORT W http_connector_sendmessagebody(http_connector_t *connector, ID endpoint, UB *p, W len)
{
	http_reqentry_t *entry;

	HTTP_CONNECTOR_SENDXXX_GET_CHECK(connector, endpoint, SEND_MESSAGE_BODY, entry);

	return 0;
}

EXPORT W http_connector_sendmessagebodyend(http_connector_t *connector, ID endpoint)
{
	http_reqentry_t *entry;

	HTTP_CONNECTOR_SENDXXX_GET_CHECK(connector, endpoint, SEND_MESSAGE_BODY, entry);

	entry->status = WAITING_RESPONSE;

	return 0;
}

EXPORT http_connector_t* http_connector_new()
{
	http_connector_t *connector;

	connector = (http_connector_t*)malloc(sizeof(http_connector_t));
	if (connector == NULL) {
		DP_ER("malloc", 0);
		return NULL;
	}
	connector->dict = http_reqdict_new(10/*tmp*/);
	if (connector->dict == NULL) {
		DP_ER("http_recdict_new", 0);
		goto error_http_reqdict;
	}
	connector->flg = cre_flg(0, DELEXIT);
	if (connector->flg < 0) {
		DP_ER("cre_flg", connector->flg);
		goto error_flg;
	}

	return connector;

	del_flg(connector->flg);
error_flg:
	http_reqdict_delete(connector->dict);
error_http_reqdict:
	free(connector);
	return NULL;
}

EXPORT VOID http_connector_delete(http_connector_t *connector)
{
	del_flg(connector->flg);
	http_reqdict_delete(connector->dict);
	free(connector);
}

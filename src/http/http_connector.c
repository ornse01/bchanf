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

#ifdef BCHAN_CONFIG_DEBUG
# define DP(arg) printf arg
# define DP_ER(msg, err) printf("%s (%d/%x)\n", msg, err>>16, err)
#else
# define DP(arg) /**/
# define DP_ER(msg, err) /**/
#endif

struct http_reqentry_t_ {
	enum {
		NON_EXISTENT,
		WAITING_ENDPOINT,
		SENDING,
		RECEIVING,
		ABORTED_BY_TRANSPORT,
		ABORTED_BY_USER,
		COMPLETED
	} status;
	UB *host;
	W host_len;
	UB *path;
	W path_len;
	SOCKADDR addr;
	VP arg;
};
typedef struct http_reqentry_t_ http_reqentry_t;

LOCAL VOID http_reqentry_attachendpoint(http_reqentry_t *entry)
{
	entry->status = SENDING;
	/* TODO */
	return;
}

LOCAL VOID http_reqentry_detachendpoint(http_reqentry_t *entry)
{
	entry->status = WAITING_ENDPOINT;
	/* TODO */
	return;
}

LOCAL W http_reqentry_allocate(http_reqentry_t *entry, UB *host, W host_len, UH port, UB *path, W path_len, VP arg)
{
	W err;
	B buf[HBUFLEN];
	HOSTENT ent;
	struct sockaddr_in *addr_in;

	entry->status = WAITING_ENDPOINT;
	entry->host = malloc(sizeof(UB)*(host_len+1));
	if (entry->host == NULL) {
		err = ER_NOMEM; /* TODO: detail error code */
		goto error_host;
	}
	memcpy(entry->host, host, host_len);
	entry->host[host_len] = '\0';
	entry->host_len = host_len;
	entry->path = malloc(sizeof(UB)*(path_len+1));
	if (entry->path == NULL) {
		err = ER_NOMEM; /* TODO: detail error code */
		goto error_path;
	}
	memcpy(entry->path, path, path_len);
	entry->path[path_len] = '\0';
	entry->path_len = path_len;
	entry->arg = arg;

	err = so_gethostbyname(entry->host, &ent, buf);
	if (err < 0) {
		goto error_gethostbyname;
	}
	addr_in = (struct sockaddr_in *)&(entry->addr);
	addr_in->sin_family = AF_INET;
	addr_in->sin_port = htons( port );
	addr_in->sin_addr.s_addr = *(unsigned int *)(ent.h_addr_list[0]);

	return 0;

error_gethostbyname:
	free(entry->path);
error_path:
	free(entry->host);
error_host:
	entry->status = NON_EXISTENT;
	return err;
}

LOCAL VOID http_reqentry_free(http_reqentry_t *entry)
{
	entry->path_len = 0;
	if (entry->path != NULL) {
		free(entry->path);
	}
	entry->host_len = 0;
	if (entry->host != NULL) {
		free(entry->host);
	}
	entry->status = NON_EXISTENT;
}

LOCAL VOID http_reqentry_initialize(http_reqentry_t *entry)
{
	entry->status = NON_EXISTENT;
	entry->host = NULL;
	entry->host_len = 0;
	entry->path = NULL;
	entry->path_len = 0;
	entry->arg = NULL;
}

LOCAL VOID http_reqentry_finalize(http_reqentry_t *entry)
{
	if (entry->path != NULL) {
		free(entry->path);
	}
	if (entry->host != NULL) {
		free(entry->host);
	}
}

struct http_reqdict_t_ {
	W entries;
	http_reqentry_t entry[1];
};
typedef struct http_reqdict_t_ http_reqdict_t;

LOCAL http_reqentry_t* http_reqdict_allocate(http_reqdict_t *dict, UB *host, W host_len, UH port, UB *path, W path_len, VP arg)
{
	W i, err;

	for (i = 0; i < dict->entries; i++) {
		if (dict->entry[i].status == NON_EXISTENT) {
			break;
		}
	}
	if (i == dict->entries) {
		return NULL;
	}

	err = http_reqentry_allocate(dict->entry + i, host, host_len, port, path, path_len, arg);
	if (err < 0) {
		return NULL;
	}

	return dict->entry + i;
}

LOCAL http_reqentry_t* http_reqdict_getentrybyindex(http_reqdict_t *dict, W i)
{
	return dict->entry + i;
}

LOCAL W http_reqdict_length(http_reqdict_t *dict)
{
	return dict->entries;
}

LOCAL VOID http_reqdict_free(http_reqdict_t *dict, http_reqentry_t *entry)
{
	http_reqentry_free(entry);
}

LOCAL http_reqdict_t* http_reqdict_new(W max_entries)
{
	http_reqdict_t *dict;
	W i;

	dict = (http_reqdict_t*)malloc(sizeof(W)*sizeof(http_reqdict_t)*max_entries);
	if (dict == NULL) {
		return NULL;
	}
	dict->entries = max_entries;
	for (i = 0; i < max_entries; i++) {
		http_reqentry_initialize(dict->entry+i);
	}
	return dict;
}

LOCAL VOID http_reqdict_delete(http_reqdict_t *dict)
{
	W i;
	for (i = 0; i < dict->entries; i++) {
		http_reqentry_finalize(dict->entry+i);
	}
	free(dict);
}

struct http_requestsending_t_ {
	enum {
		SND_NONEXISTENT,
		SND_ALLOCATED,
	} status;
	http_reqentry_t *entry;
};

EXPORT W http_requestsending_sendheader(http_requestsending_t *request, UB *p, W len)
{
	/* TODO: transport */
	return 0;
}

EXPORT W http_requestsending_headerend(http_requestsending_t *request)
{
	/* TODO: transport */
	return 0;
}

EXPORT W http_requestsending_sendmessagebody(http_requestsending_t *request, UB *p, W len)
{
	/* TODO: transport */
	return 0;
}

EXPORT W http_requestsending_messageend(http_requestsending_t *request)
{
	/* TODO: transport */
	request->entry->status = RECEIVING; /* tmp */
	return 0;
}

LOCAL VOID http_requestsending_initialize(http_requestsending_t *snd)
{
	snd->status = SND_NONEXISTENT;
	snd->entry = NULL;
}

LOCAL VOID http_requestsending_finalize(http_requestsending_t *snd)
{
}

struct http_responsereceiving_t_ {
	enum {
		RCV_NONEXISTENT,
		RCV_ALLOCATED,
	} status;
	http_reqentry_t *entry;
};

EXPORT W http_responsereceiving_getmessage(http_responsereceiving_t *response, http_responsereceiving_result *result)
{
	/* TODO: transport */
	result->type = HTTP_RESPONSERECEIVING_RESULTTYPE_MESSAGEBODY_END;
	result->data = NULL;
	result->len = 0;
	return 0;
}

EXPORT W http_responsereceiving_messageend(http_responsereceiving_t *response)
{
	/* TODO: transport */
	response->entry->status = COMPLETED;
	return 0;
}

LOCAL VOID http_responsereceiving_initialize(http_responsereceiving_t *rcv)
{
	rcv->status = SND_NONEXISTENT;
	rcv->entry = NULL;
}

LOCAL VOID http_responsereceiving_finalize(http_responsereceiving_t *rcv)
{
}

struct http_ctxbuf_t_ {
	http_requestsending_t *snd;
	http_responsereceiving_t *rcv;
	W buflen;
};
typedef struct http_ctxbuf_t_ http_ctxbuf_t;

LOCAL http_requestsending_t* http_ctxbuf_allocatesend(http_ctxbuf_t *ctxbuf, http_reqentry_t *entry)
{
	W i;

	for (i = 0; i < ctxbuf->buflen; i++) {
		if (ctxbuf->snd[i].status == SND_NONEXISTENT) {
			ctxbuf->snd[i].status = SND_ALLOCATED;
			ctxbuf->snd[i].entry = entry;
			return ctxbuf->snd + i;
		}
	}

	return NULL;
}

LOCAL http_responsereceiving_t* http_ctxbuf_allocatereceive(http_ctxbuf_t *ctxbuf, http_reqentry_t *entry)
{
	W i;

	for (i = 0; i < ctxbuf->buflen; i++) {
		if (ctxbuf->rcv[i].status == RCV_NONEXISTENT) {
			ctxbuf->rcv[i].status = RCV_ALLOCATED;
			ctxbuf->rcv[i].entry = entry;
			return ctxbuf->rcv + i;
		}
	}

	return NULL;
}

LOCAL W http_ctxbuf_initialize(http_ctxbuf_t *ctxbuf, W buf_len)
{
	W i;

	ctxbuf->snd = (http_requestsending_t *)malloc(sizeof(http_requestsending_t)*buf_len);
	if (ctxbuf->snd == NULL) {
		return ER_NOMEM;
	}
	ctxbuf->rcv = (http_responsereceiving_t *)malloc(sizeof(http_responsereceiving_t)*buf_len);
	if (ctxbuf->rcv == NULL) {
		free(ctxbuf->snd);
		return ER_NOMEM;
	}
	ctxbuf->buflen = buf_len;

	for (i = 0; i < buf_len; i++) {
		http_requestsending_initialize(ctxbuf->snd + i);
		http_responsereceiving_initialize(ctxbuf->rcv + i);
	}

	return 0;
}

LOCAL VOID http_ctxbuf_finalize(http_ctxbuf_t *ctxbuf)
{
	W i = 0;

	for (i = 0; i < ctxbuf->buflen; i++) {
		http_responsereceiving_finalize(ctxbuf->rcv + i);
		http_requestsending_finalize(ctxbuf->snd + i);
	}
}

struct http_connector_t_ {
	http_reqdict_t *dict;
	http_ctxbuf_t ctxbuf;
	ID reqmbf;
	ID evtmbf;
};

EXPORT W http_connector_request(http_connector_t *connector, UB *host, W host_len, UH port, UB *path, W path_len, VP arg)
{
	http_reqentry_t *entry;

	entry = http_reqdict_allocate(connector->dict, host, host_len, port, path, path_len, arg);
	if (entry != NULL) {
		return -1; /* TODO */
	}

	return snd_mbf(connector->reqmbf, NULL, 0, T_FOREVER/* tmp */);
}

LOCAL VOID http_connector_collect(http_connector_t *connector)
{
	W i, len;
	http_reqentry_t *entry;

	len = http_reqdict_length(connector->dict);
	for (i = 0; i < len ; i++) {
		entry = http_reqdict_getentrybyindex(connector->dict, i);
		if (entry->status == ABORTED_BY_TRANSPORT) {
			http_reqdict_free(connector->dict, entry);
		} else if (entry->status == COMPLETED) {
			/* TODO: release endpoint */
			http_reqdict_free(connector->dict, entry);
		}
	}
}

LOCAL W http_connector_sendsendmessage(http_connector_t *connector, http_reqentry_t *entry)
{
	volatile http_connector_event evt;
	http_requestsending_t *snd;

	snd = http_ctxbuf_allocatesend(&connector->ctxbuf, entry);
	if (snd == NULL) {
		return ER_NONE;
	}

	evt.type = HTTP_CONNECTOR_EVENTTYPE_SEND;
	evt.ctx.request = snd;
	evt.arg = entry->arg;

	return snd_mbf(connector->evtmbf, (VP)&evt, sizeof(http_connector_event), T_FOREVER);
}

LOCAL W http_connector_searchwaiting(http_connector_t *connector)
{
	W i, len, err;
	http_reqentry_t *entry;

	len = http_reqdict_length(connector->dict);
	for (i = 0; i < len ; i++) {
		entry = http_reqdict_getentrybyindex(connector->dict, i);
		if (entry->status == WAITING_ENDPOINT) {
			/* TODO: end point search */
			/* TODO: end point status change: HOLDED and sending */
			http_reqentry_attachendpoint(entry);
			err = http_connector_sendsendmessage(connector, entry);
			if (err < 0) {
				http_reqentry_detachendpoint(entry);
			}
		}
	}

	return 0;
}

LOCAL W http_connector_sendreceivemessage(http_connector_t *connector, http_reqentry_t *entry)
{
	volatile http_connector_event evt;
	http_responsereceiving_t *rcv;

	rcv = http_ctxbuf_allocatereceive(&connector->ctxbuf, entry);
	if (rcv == NULL) {
		return ER_NONE;
	}

	evt.type = HTTP_CONNECTOR_EVENTTYPE_RECEIVE;
	evt.ctx.response = rcv;
	evt.arg = entry->arg;

	return snd_mbf(connector->evtmbf, (VP)&evt, sizeof(http_connector_event), T_FOREVER);
}

LOCAL W http_connector_waitreceiving(http_connector_t *connector)
{
	W i, len, err;
	http_reqentry_t *entry;

	len = http_reqdict_length(connector->dict);
	for (i = 0; i < len ; i++) {
		entry = http_reqdict_getentrybyindex(connector->dict, i);
		if (entry->status == RECEIVING) {
			/* TODO: end point status change: readinging */
			/* TODO: select() for end point */
			err = http_connector_sendreceivemessage(connector, entry);
			if (err < 0) {
				/* TODO */
			}
		}
	}

	return 0;
}

EXPORT W http_connector_waitconnection(http_connector_t *connector, TMOUT tmout)
{
	W err;

	/**/
	http_connector_collect(connector);

	err = http_connector_searchwaiting(connector);
	if (err < 0) {
		return err;
	}

	err = http_connector_waitreceiving(connector);
	if (err < 0) {
		return err;
	}

	return 0;
}

EXPORT W http_connector_getevent(http_connector_t *connector, http_connector_event *event)
{
	return rcv_mbf(connector->evtmbf, event, T_NOWAIT);
}

EXPORT http_connector_t* http_connector_new()
{
	http_connector_t *connector;
	W err;

	connector = (http_connector_t*)malloc(sizeof(http_connector_t));
	if (connector == NULL) {
		return NULL;
	}
	connector->dict = http_reqdict_new(10/*tmp*/);
	if (connector->dict == NULL) {
		goto error_http_reqdict;
		return NULL;
	}
	err = http_ctxbuf_initialize(&connector->ctxbuf, 10/*tmp*/);
	if (err < 0) {
		goto error_http_ctxbuf;
	}
	connector->reqmbf = cre_mbf(0, 0, DELEXIT);
	if (connector->reqmbf < 0) {
		goto error_reqmbf;
	}
	connector->evtmbf = cre_mbf(sizeof(http_connector_event)*10, sizeof(http_connector_event), DELEXIT);
	if (connector->evtmbf < 0) {
		goto error_evtmbf;
	}

	return connector;

	del_mbf(connector->evtmbf);
error_evtmbf:
	del_mbf(connector->reqmbf);
error_reqmbf:
	http_reqdict_delete(connector->dict);
error_http_ctxbuf:
	free(connector);
error_http_reqdict:
	return NULL;
}

EXPORT VOID http_connector_delete(http_connector_t *connector)
{
	del_mbf(connector->evtmbf);
	del_mbf(connector->reqmbf);
	http_ctxbuf_finalize(&connector->ctxbuf);
	http_reqdict_delete(connector->dict);
	free(connector);
}

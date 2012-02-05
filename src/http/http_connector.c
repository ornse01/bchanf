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
#include    "http_transport.h"
#include    "http_requestlinestream.h"
#include    "http_defaultheaderstream.h"
#include    "http_statuslineparser.h"
#include    "http_defaultheaderparser.h"
#include    "http_transferdecoder.h"
#include    "http_contentdecoder.h"

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
		SEND_COMPLETED,
	} snd_state;
	enum {
		RECEIVE_STATUS_LINE,
		RECEIVE_HEADER,
		RECEIVE_HEADER_END,
		RECEIVE_MESSAGE_BODY,
		RECEIVE_MESSAGE_BODY_TRANSFERDECODE,
		RECEIVE_MESSAGE_BODY_CONTENTDECODE,
		RECEIVE_MESSAGE_END,
		RECEIVE_COMPLETED,
	} rcv_state;
	Bool aborted_by_user;
	HTTP_METHOD method;
	UB *host;
	W host_len;
	SOCKADDR addr;
	ID transport;
	struct {
		http_statuslineparser_t sl;
		http_defaultheaderparser_t dh;
		http_transferdecoder_t tc;
		http_contentdecoder_t cc;
		struct {
			http_transferdecoder_result *transfer_result;
			W transfer_result_len;
			W transfer_result_consumed;
			http_contentdecoder_result *content_result;
			W content_result_len;
			W content_result_consumed;
		} body;
	} rcv_reader;
};
typedef struct http_reqentry_t_ http_reqentry_t;

LOCAL VOID http_reqentry_attachendpoint(http_reqentry_t *entry, http_transport_t *transport, ID endpoint)
{
	http_transport_holdendpoint(transport, endpoint);
	entry->status = SENDING_REQUEST;
	entry->transport = endpoint;
	return;
}

LOCAL VOID http_reqentry_detachendpoint(http_reqentry_t *entry, http_transport_t *transport, Bool transport_close)
{
	http_transport_releaseendpoint(transport, entry->transport, transport_close);
	entry->transport = -1;
	return;
}

LOCAL W http_reqentry_initialize(http_reqentry_t *entry, UB *host, W host_len, UH port, HTTP_METHOD method)
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
	entry->method = method;
	entry->transport = -1;

	http_statuslineparser_initialize(&entry->rcv_reader.sl);
	http_defaultheaderparser_initialize(&entry->rcv_reader.dh);

	return 0;

error_gethostbyname:
	free(entry->host);
error_host:
	entry->status = NON_EXISTENT;
	return err;
}

LOCAL VOID http_reqentry_finalize(http_reqentry_t *entry)
{
	switch (entry->rcv_state) {
	case RECEIVE_MESSAGE_BODY:
	case RECEIVE_MESSAGE_BODY_TRANSFERDECODE:
	case RECEIVE_MESSAGE_BODY_CONTENTDECODE:
	case RECEIVE_MESSAGE_END:
		http_contentdecoder_finalize(&entry->rcv_reader.cc);
		http_transferdecoder_finalize(&entry->rcv_reader.tc);
		break;
	case RECEIVE_STATUS_LINE:
	case RECEIVE_HEADER:
	case RECEIVE_HEADER_END:
	case RECEIVE_COMPLETED:
		break;
	}
	http_defaultheaderparser_finalize(&entry->rcv_reader.dh);
	http_statuslineparser_finalize(&entry->rcv_reader.sl);
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

LOCAL ID http_reqdict_allocate(http_reqdict_t *dict, UB *host, W host_len, UH port, HTTP_METHOD method)
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

	err = http_reqentry_initialize(cb, host, host_len, port, method);
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
	http_transport_t *transport;
	ID flg;
};

#define HTTP_CONNECTOR_FLAG_CREARMASK(flag) (~(flag))
#define HTTP_CONNECTOR_FLAG_REQUEST 0x00000001
#define HTTP_CONNECTOR_FLAG_EVENT   0x00000002
#define HTTP_CONNECTOR_FLAG_CLEARMASK_REQUEST HTTP_CONNECTOR_FLAG_CREARMASK(HTTP_CONNECTOR_FLAG_REQUEST)
#define HTTP_CONNECTOR_FLAG_CLEARMASK_EVENT HTTP_CONNECTOR_FLAG_CREARMASK(HTTP_CONNECTOR_FLAG_EVENT)

EXPORT ID http_connector_createendpoint(http_connector_t *connector, UB *host, W host_len, UH port, HTTP_METHOD method)
{
	ID id;
	W err;

	id = http_reqdict_allocate(connector->dict, host, host_len, port, method);
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
	Bool transport_close = True;

	entry = http_reqdict_getentrybyID(connector->dict, endpoint);
	if (entry == NULL) {
		return;
	}

	switch (entry->status) {
	case SENDING_REQUEST:
		if (entry->snd_state == SEND_REQUEST_LINE) {
			transport_close = False;
			break;
		}
		/* intentional */
	case WAITING_RESPONSE:
	case RECEIVING_RESPONSE:
		http_reqentry_detachendpoint(entry, connector->transport, transport_close);
		break;
	case NON_EXISTENT:
	case WAITING_TRANSPORT:
	case ABORTED_BY_TRANSPORT:
	case COMPLETED:
	}

	http_reqdict_free(connector->dict, endpoint);
}

LOCAL W http_connector_searchwaiting(http_connector_t *connector)
{
	W ret = 0;
	http_reqentry_t *entry;
	http_recdictiterator_t iter;
	Bool cont, holded;
	ID endpoint;

	http_reqdictiterator_initialize(&iter, connector->dict);
	for (;;) {
		cont = http_reqdictiterator_next(&iter, &entry);
		if (cont == False) {
			break;
		}
		if (entry->status == WAITING_TRANSPORT) {
			endpoint = http_transport_searchendpoint(connector->transport, &entry->addr);
			if ((endpoint & 0xFFFF0000) == ER_NOEXS) {
				endpoint = http_transport_prepairendpoint(connector->transport, &entry->addr);
				if (endpoint < 0) {
					continue;
				}
			} else if (endpoint < 0) {
				continue;
			}
			http_transport_isholdedendpoint(connector->transport, endpoint, &holded);
			if (holded != False) {
				continue;
			}
			http_reqentry_attachendpoint(entry, connector->transport, endpoint);
			ret++;
		}
	}
	http_reqdictiterator_finalize(&iter);

	return ret;
}

LOCAL W http_connector_waitreceiving(http_connector_t *connector, TMOUT tmout)
{
	W ret = 0;
	http_reqentry_t *entry;
	http_recdictiterator_t iter;
	Bool cont, is_ready;

	http_reqdictiterator_initialize(&iter, connector->dict);
	for (;;) {
		cont = http_reqdictiterator_next(&iter, &entry);
		if (cont == False) {
			break;
		}
		if (entry->status == WAITING_RESPONSE) {
			http_transport_setwaitingreceive(connector->transport, entry->transport);
		}
	}
	http_reqdictiterator_finalize(&iter);

	http_transport_waitreceive(connector->transport, tmout);

	http_reqdictiterator_initialize(&iter, connector->dict);
	for (;;) {
		cont = http_reqdictiterator_next(&iter, &entry);
		if (cont == False) {
			break;
		}
		if (entry->status == WAITING_RESPONSE) {
			http_transport_isreadready(connector->transport, entry->transport, &is_ready);
			if (is_ready != False) {
				entry->status = RECEIVING_RESPONSE;
				ret++;
			}
		}
	}
	http_reqdictiterator_finalize(&iter);

	return ret;
}

EXPORT W http_connector_waitconnection(http_connector_t *connector, TMOUT tmout)
{
	W err;
	Bool evt = False;

	err = http_connector_searchwaiting(connector);
	if (err < 0) {
		return err;
	}
	if (err > 0) {
		evt = True;
	}
	http_transport_releaseunusedendpoint(connector->transport);

	err = http_connector_waitreceiving(connector, tmout);
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

LOCAL W http_connector_rcv_status_line(http_connector_t *connector, http_reqentry_t *entry, http_connector_event *event)
{
	http_statuslineparser_result result;
	UB *bin;
	W i, len, err;
	Bool end = False;

	err = http_transport_read(connector->transport, entry->transport, &bin, &len);
	if (err < 0) {
		DP_ER("http_transport_read", err);
		return err;
	}

	for (i = 0; i < len; i++) {
		end = http_statuslineparser_inputchar(&entry->rcv_reader.sl, bin[i], &result);
		if (end != False) {
			i++;
			break;
		}
	}

	http_transport_consumereadbuffer(connector->transport, entry->transport, i);

	if (end != False) {
		if (result.error != HTTP_STATUSLINEPARSER_ERROR_NONE) {
			return -1; /* TODO */
		}
		entry->rcv_state = RECEIVE_HEADER;
		event->endpoint = entry->base.id;
		event->type = HTTP_CONNECTOR_EVENTTYPE_RECEIVE_STATUSLINE;
		event->data.receive_statusline.version = result.version;
		event->data.receive_statusline.statuscode = result.statuscode;
		return 1;
	}

	return 0;
}

LOCAL W http_connector_rcv_header(http_connector_t *connector, http_reqentry_t *entry, http_connector_event *event)
{
	HTTP_DEFAULTHEADERPARSER_RESULT result;
	UB *bin;
	W i, len, err;
	Bool end = False;

	err = http_transport_read(connector->transport, entry->transport, &bin, &len);
	if (err < 0) {
		DP_ER("http_transport_read", err);
		return err;
	}

	for (i = 0; i < len; i++) {
		http_defaultheaderparser_inputchar(&entry->rcv_reader.dh, bin[i], &result);
		if (result == HTTP_DEFAULTHEADERPARSER_RESULT_MESSAGE_HEADER_END) {
			end = True;
			i++;
			break;
		}
	}

	http_transport_consumereadbuffer(connector->transport, entry->transport, i);

	if (end != False) {
		entry->rcv_state = RECEIVE_HEADER_END;
	}

	event->endpoint = entry->base.id;
	event->type = HTTP_CONNECTOR_EVENTTYPE_RECEIVE_HEADER;
	event->data.receive_header.bin = bin;
	event->data.receive_header.len = i;

	return 1;
}

LOCAL W http_connector_rcv_header_end(http_connector_t *connector, http_reqentry_t *entry, http_connector_event *event)
{
	W err;
	W content_length;
	HTTP_TRANSFERCODING_TYPE transfer_coding;
	HTTP_CONTENTCODING_VALUE content_coding;

	entry->rcv_state = RECEIVE_MESSAGE_BODY;
	event->endpoint = entry->base.id;
	event->type = HTTP_CONNECTOR_EVENTTYPE_RECEIVE_HEADER_END;

	content_length = http_defaultheaderparsr_contentlength(&entry->rcv_reader.dh);
	transfer_coding = http_defaultheaderparsr_transferencoding(&entry->rcv_reader.dh);
	content_coding = http_defaultheaderparsr_contentencoding(&entry->rcv_reader.dh);

	http_transferdecoder_initialize(&entry->rcv_reader.tc, transfer_coding, content_length);
	err = http_contentdecoder_initialize(&entry->rcv_reader.cc, content_coding);
	if (err < 0) {
		return err;
	}
	entry->rcv_reader.body.transfer_result = NULL;
	entry->rcv_reader.body.transfer_result_len = 0;
	entry->rcv_reader.body.transfer_result_consumed = 0;
	entry->rcv_reader.body.content_result = NULL;
	entry->rcv_reader.body.content_result_len = 0;
	entry->rcv_reader.body.content_result_consumed = 0;

	return 1;
}

LOCAL W http_connector_rcv_message_body_contentdecode(http_connector_t *connector, http_reqentry_t *entry, http_connector_event *event)
{
	W err;
	http_contentdecoder_result *result;

	if (entry->rcv_reader.body.content_result_consumed == entry->rcv_reader.body.content_result_len) {
		err = http_contentdecoder_outputdata(&entry->rcv_reader.cc, &entry->rcv_reader.body.content_result, &entry->rcv_reader.body.content_result_len);
		if (err < 0) {
			DP_ER("http_contentdecoder_outputdata", err);
			return err;
		}
		entry->rcv_reader.body.content_result_consumed = 0;
	}
	result = entry->rcv_reader.body.content_result + entry->rcv_reader.body.content_result_consumed;
	entry->rcv_reader.body.content_result_consumed++;
	switch (result->type) {
	case HTTP_CONTENTDECODER_RESULTTYPE_DATA:
		event->type = HTTP_CONNECTOR_EVENTTYPE_RECEIVE_MESSAGEBODY;
		event->endpoint = entry->base.id;
		event->data.receive_messagebody.bin = result->data;
		event->data.receive_messagebody.len = result->len;
		return 1;
	case HTTP_CONTENTDECODER_RESULTTYPE_NEED_INPUT:
		entry->rcv_state = RECEIVE_MESSAGE_BODY_TRANSFERDECODE;
		break;
	case HTTP_CONTENTDECODER_RESULTTYPE_END:
		entry->rcv_state = RECEIVE_MESSAGE_END;
		break;
	}

	return 0;
}

LOCAL W http_connector_rcv_message_body_transferdecode(http_connector_t *connector, http_reqentry_t *entry, http_connector_event *event)
{
	http_transferdecoder_result *result;
	W err;

	if (entry->rcv_reader.body.transfer_result_consumed == entry->rcv_reader.body.transfer_result_len) {
		entry->rcv_state = RECEIVE_MESSAGE_BODY;
		return 0;
	}

	result = entry->rcv_reader.body.transfer_result + entry->rcv_reader.body.transfer_result_consumed;
	switch (result->type) {
	case HTTP_TRANSFERDECODER_RESULTTYPE_DATA:
		err = http_contentdecoder_inputentitybody(&entry->rcv_reader.cc, result->data, result->len);
		break;
	case HTTP_TRANSFERDECODER_RESULTTYPE_END:
		err = http_contentdecoder_inputendofdata(&entry->rcv_reader.cc);
		break;
	default:
		err = 0;
	}
	entry->rcv_reader.body.transfer_result_consumed++;
	if (err < 0) {
		DP_ER("http_contentdecoder_inputXXX", err);
		return err;
	}
	entry->rcv_state = RECEIVE_MESSAGE_BODY_CONTENTDECODE;

	return http_connector_rcv_message_body_contentdecode(connector, entry, event);
}

LOCAL W http_connector_rcv_message_body(http_connector_t *connector, http_reqentry_t *entry, http_connector_event *event)
{
	UB *bin;
	W len, err;

	err = http_transport_read(connector->transport, entry->transport, &bin, &len);
	if (err < 0) {
		DP_ER("http_transport_read", err);
		return err;
	}

	len = http_transferdecoder_inputentitybody(&entry->rcv_reader.tc, bin, len, &entry->rcv_reader.body.transfer_result, &entry->rcv_reader.body.transfer_result_len);
	entry->rcv_reader.body.transfer_result_consumed = 0;
	http_transport_consumereadbuffer(connector->transport, entry->transport, len);

	entry->rcv_state = RECEIVE_MESSAGE_BODY_TRANSFERDECODE;

	return http_connector_rcv_message_body_transferdecode(connector, entry, event);
}

LOCAL W http_connector_rcv_message_end(http_connector_t *connector, http_reqentry_t *entry, http_connector_event *event)
{
	Bool connection_closed;

	http_contentdecoder_finalize(&entry->rcv_reader.cc);
	http_transferdecoder_finalize(&entry->rcv_reader.tc);
	connection_closed = http_defaultheaderparser_connectionclosed(&entry->rcv_reader.dh);

	entry->status = COMPLETED;
	entry->rcv_state = RECEIVE_COMPLETED;
	event->endpoint = entry->base.id;
	event->type = HTTP_CONNECTOR_EVENTTYPE_RECEIVE_MESSAGEBODY_END;

	http_reqentry_detachendpoint(entry, connector->transport, connection_closed);

	return 1;
}

LOCAL W http_connector_handleevent_receiving_response(http_connector_t *connector, http_reqentry_t *entry, http_connector_event *event)
{
	W err = 0;

	switch (entry->rcv_state) {
	case RECEIVE_STATUS_LINE:
		DP_STATE("RECEIVE_STATUS_LINE");
		err = http_connector_rcv_status_line(connector, entry, event);
		break;
	case RECEIVE_HEADER:
		DP_STATE("RECEIVE_RECEIVE_HEADER");
		err = http_connector_rcv_header(connector, entry, event);
		break;
	case RECEIVE_HEADER_END:
		DP_STATE("RECEIVE_HEADER_END");
		err = http_connector_rcv_header_end(connector, entry, event);
		break;
	case RECEIVE_MESSAGE_BODY:
		DP_STATE("RECEIVE_MESSAGE_BODY");
		err = http_connector_rcv_message_body(connector, entry, event);
		break;
	case RECEIVE_MESSAGE_BODY_TRANSFERDECODE:
		DP_STATE("RECEIVE_MESSAGE_BODY_TRANSFERDECODE");
		err = http_connector_rcv_message_body_transferdecode(connector, entry, event);
		break;
	case RECEIVE_MESSAGE_BODY_CONTENTDECODE:
		DP_STATE("RECEIVE_MESSAGE_BODY_CONTENTDECDE");
		err = http_connector_rcv_message_body_contentdecode(connector, entry, event);
		break;
	case RECEIVE_MESSAGE_END:
		DP_STATE("RECEIVE_MESSAGE_END");
		err = http_connector_rcv_message_end(connector, entry, event);
		break;
	case RECEIVE_COMPLETED:
		DP_STATE("RECEIVE_COMPLETED");
		err = 0;
		break;
	}

	return err;
}

/* TODO: devide event found and need loop for state transition */
EXPORT Bool http_connector_searcheventtarget(http_connector_t *connector, http_connector_event *event)
{
	http_reqentry_t *entry;
	http_recdictiterator_t iter;
	Bool cont, found = False;
	W err;

	event->type = HTTP_CONNECTOR_EVENTTYPE_NONE;

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
			err = http_connector_handleevent_receiving_response(connector, entry, event);
			if ((err < 0)&&((err & 0xFFFF0000) != EX_WOULDBLOCK)) {
				event->type = HTTP_CONNECTOR_EVENTTYPE_ERROR;
				event->endpoint = entry->base.id;
				entry->status = ABORTED_BY_TRANSPORT;
				http_reqentry_detachendpoint(entry, connector->transport, True);
				break;
			}
			found = True;
			if (err > 0) {
				break;
			}
		}
	}
	http_reqdictiterator_finalize(&iter);

	return found;
}

EXPORT W http_connector_getevent(http_connector_t *connector, http_connector_event *event)
{
	W err;
	Bool found;

	err = wai_flg(connector->flg, HTTP_CONNECTOR_FLAG_EVENT, WF_AND|NOCLR, T_NOWAIT);
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
	if (event->type == HTTP_CONNECTOR_EVENTTYPE_NONE) {
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
	http_requestlinestream_t reqline;
	W err, len;
	Bool cont;
	CONST UB *str;

	HTTP_CONNECTOR_SENDXXX_GET_CHECK(connector, endpoint, SEND_REQUEST_LINE, entry);

	err = http_requestlinestream_initialize(&reqline, entry->method, path, path_len);
	if (err < 0) {
		return err;
	}
	for (;;) {
		cont = http_requestlinestream_make(&reqline, &str, &len);
		if (cont == False) {
			break;
		}
		err = http_transport_write(connector->transport, entry->transport, str, len);
		if (err < 0) {
			break;
		}
	}
	http_requestlinestream_finalize(&reqline);

	if (err < 0) {
		return err;
	}

	entry->snd_state = SEND_HEADER_MINE;

	return 0;
}

LOCAL W http_connector_writedefaultheader(http_connector_t *connector, ID transport, UB *host, W host_len)
{
	http_defaultheaderstream_t defaultheader;
	W err = 0, len;
	Bool cont;
	CONST UB *str;

	http_defaultheaderstream_initialize(&defaultheader, host, host_len);
	for (;;) {
		cont = http_defaultheaderstream_make(&defaultheader, &str, &len);
		if (cont == False) {
			break;
		}
		err = http_transport_write(connector->transport, transport, str, len);
		if (err < 0) {
			break;
		}
	}
	http_defaultheaderstream_finalize(&defaultheader);

	return err;
}

EXPORT W http_connector_sendheader(http_connector_t *connector, ID endpoint, UB *p, W len)
{
	http_reqentry_t *entry;
	W err;

	HTTP_CONNECTOR_SENDXXX_GET_CHECK_2(connector, endpoint, SEND_HEADER_MINE, SEND_HEADER_USER, entry);

	if (entry->snd_state == SEND_HEADER_MINE) {
		err = http_connector_writedefaultheader(connector, entry->transport, entry->host, entry->host_len);
		if (err < 0) {
			return err;
		}
		entry->snd_state = SEND_HEADER_USER;
	}

	return 0;
}

EXPORT W http_connector_sendheaderend(http_connector_t *connector, ID endpoint)
{
	http_reqentry_t *entry;
	W err;

	HTTP_CONNECTOR_SENDXXX_GET_CHECK_2(connector, endpoint, SEND_HEADER_MINE, SEND_HEADER_USER, entry);

	if (entry->snd_state == SEND_HEADER_MINE) {
		err = http_connector_writedefaultheader(connector, entry->transport, entry->host, entry->host_len);
		if (err < 0) {
			return err;
		}
	}
	err = http_transport_write(connector->transport, entry->transport, "\r\n", 2);
	if (err < 0) {
		return err;
	}

	entry->snd_state = SEND_MESSAGE_BODY;

	return 0;
}

EXPORT W http_connector_sendmessagebody(http_connector_t *connector, ID endpoint, UB *p, W len)
{
	http_reqentry_t *entry;
	W err;

	HTTP_CONNECTOR_SENDXXX_GET_CHECK(connector, endpoint, SEND_MESSAGE_BODY, entry);

	err = http_transport_write(connector->transport, entry->transport, p, len);
	if (err < 0) {
		return err;
	}

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
	connector->transport = http_transport_new(10/*tmp*/);
	if (connector->transport == NULL) {
		DP_ER("http_transport_new", -1);
		goto error_transport;
	}

	return connector;

	http_transport_delete(connector->transport);
error_transport:
	del_flg(connector->flg);
error_flg:
	http_reqdict_delete(connector->dict);
error_http_reqdict:
	free(connector);
	return NULL;
}

EXPORT VOID http_connector_delete(http_connector_t *connector)
{
	http_transport_delete(connector->transport);
	del_flg(connector->flg);
	http_reqdict_delete(connector->dict);
	free(connector);
}

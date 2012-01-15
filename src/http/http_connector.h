/*
 * http_connector.h
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

/* Vendor name: */
/* Functionality name: http */
/* Detail name: connector */

#include    <basic.h>
#include    <btron/taskcomm.h>

#ifndef __HTTP_CONNECTOR_H__
#define __HTTP_CONNECTOR_H__

/* Functionality name: http */
/* Detail name: requestsending */
typedef struct http_requestsending_t_ http_requestsending_t;

IMPORT W http_requestsending_sendheader(http_requestsending_t *request, UB *p, W len);
IMPORT W http_requestsending_headerend(http_requestsending_t *request);
IMPORT W http_requestsending_sendmessagebody(http_requestsending_t *request, UB *p, W len);
IMPORT W http_requestsending_messageend(http_requestsending_t *request);

/* Functionality name: http */
/* Detail name: responsereceiving */
/* Data structure identifier: result */
struct http_responsereceiving_result_ {
	enum {
		HTTP_RESPONSERECEIVING_RESULTTYPE_HEADER,
		HTTP_RESPONSERECEIVING_RESULTTYPE_HEADER_END,
		HTTP_RESPONSERECEIVING_RESULTTYPE_MESSAGEBODY,
		HTTP_RESPONSERECEIVING_RESULTTYPE_MESSAGEBODY_END,
	} type;
	UB *data;
	W len;
};
typedef struct http_responsereceiving_result_ http_responsereceiving_result;

/* Functionality name: http */
/* Detail name: responsereceiving */
typedef struct http_responsereceiving_t_ http_responsereceiving_t;

IMPORT W http_responsereceiving_getmessage(http_responsereceiving_t *response, http_responsereceiving_result *result);
IMPORT W http_responsereceiving_messageend(http_responsereceiving_t *response);

/* Functionality name: http */
/* Detail name: connector */
/* Data structure identifier: event */
struct http_connector_event_ {
	enum {
		HTTP_CONNECTOR_EVENTTYPE_SEND,
		HTTP_CONNECTOR_EVENTTYPE_RECEIVE,
	} type;
	union {
		http_requestsending_t *request;
		http_responsereceiving_t *response;
	} ctx;
	VP arg;
};
typedef struct http_connector_event_ http_connector_event;

/* Functionality name: http */
/* Detail name: connector */
typedef struct http_connector_t_ http_connector_t;

IMPORT http_connector_t* http_connector_new();
IMPORT VOID http_connector_delete(http_connector_t *connector);
IMPORT W http_connector_request(http_connector_t *connector, UB *host, W host_len, UH port, UB *path, W path_len, VP arg);
IMPORT W http_connector_waitconnection(http_connector_t *connector, TMOUT tmout);
IMPORT W http_connector_getevent(http_connector_t *connector, http_connector_event *event);

#endif

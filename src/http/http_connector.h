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
/* Detail name: connector */
/* Data structure identifier: event */
struct http_connector_event_ {
	enum {
		HTTP_CONNECTOR_EVENTTYPE_SEND,
		HTTP_CONNECTOR_EVENTTYPE_RECEIVE_STATUSLINE,
		HTTP_CONNECTOR_EVENTTYPE_RECEIVE_HEADER,
		HTTP_CONNECTOR_EVENTTYPE_RECEIVE_HEADER_END,
		HTTP_CONNECTOR_EVENTTYPE_RECEIVE_MESSAGEBODY,
		HTTP_CONNECTOR_EVENTTYPE_RECEIVE_MESSAGEBODY_END,
		HTTP_CONNECTOR_EVENTTYPE_ERROR,
	} type;
	ID endpoint;
	union {
		struct {
		} receive_statusline;
		struct {
			UB *bin;
			W len;
		} receive_header;
		struct {
			UB *bin;
			W len;
		} receive_messagebody;
	} data;
};
typedef struct http_connector_event_ http_connector_event;

/* Functionality name: http */
/* Detail name: connector */
typedef struct http_connector_t_ http_connector_t;

IMPORT http_connector_t* http_connector_new();
IMPORT VOID http_connector_delete(http_connector_t *connector);
IMPORT ID http_connector_createendpoint(http_connector_t *connector, UB *host, W host_len, UH port);
IMPORT VOID http_connector_deleteendpoint(http_connector_t *connector, ID endpoint);
IMPORT W http_connector_waitconnection(http_connector_t *connector, TMOUT tmout);
IMPORT W http_connector_getevent(http_connector_t *connector, http_connector_event *event);
IMPORT W http_connector_sendrequestline(http_connector_t *connector, ID endpoint, UB *path, W path_len);
IMPORT W http_connector_sendheader(http_connector_t *connector, ID endpoint, UB *p, W len);
IMPORT W http_connector_sendheaderend(http_connector_t *connector, ID endpoint);
IMPORT W http_connector_sendmessagebody(http_connector_t *connector, ID endpoint, UB *p, W len);
IMPORT W http_connector_sendmessagebodyend(http_connector_t *connector, ID endpoint, UB *p, W len);

#endif

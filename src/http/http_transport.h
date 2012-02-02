/*
 * http_transport.h
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
/* Detail name: transport */

#include    <basic.h>
#include	<btron/taskcomm.h>
#include	<btron/bsocket.h>

#ifndef __HTTP_TRANSPORT_H__
#define __HTTP_TRANSPORT_H__

/* Functionality name: http */
/* Detail name: transport */

typedef struct http_transport_t_ http_transport_t;

IMPORT http_transport_t* http_transport_new(W max_endpoints);
IMPORT VOID http_transport_delete(http_transport_t *transport);
IMPORT ID http_transport_prepairendpoint(http_transport_t *transport, SOCKADDR *addr);
IMPORT ID http_transport_searchendpoint(http_transport_t *transport, SOCKADDR *addr);
IMPORT VOID http_transport_releaseunusedendpoint(http_transport_t *transport);
IMPORT W http_transport_holdendpoint(http_transport_t *transport, ID endpoint);
IMPORT VOID http_transport_releaseendpoint(http_transport_t *transport, ID endpoint);
IMPORT W http_transport_setwaitingreceive(http_transport_t *transort, ID endpoint);
IMPORT W http_transport_waitreceive(http_transport_t *transport, TMOUT tmout);
IMPORT W http_transport_isreadready(http_transport_t *transport, ID endpoint, Bool *is_ready);
IMPORT W http_transport_write(http_transport_t *transport, ID endpoint, UB *bin, W len);
IMPORT W http_transport_read(http_transport_t *transport, ID endpoint, UB **bin, W *len);
IMPORT W http_transport_consumereadbuffer(http_transport_t *transport, ID endpoint, W consumed_len);

#endif

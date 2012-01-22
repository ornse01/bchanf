/*
 * http_requestlinestream.h
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
/* Detail name: requestlinestream */

#include    <basic.h>

#include    "http_typedef.h"

/* rfc2616 5.1 Request-Line */

#ifndef __HTTP_REQUESTLINESTREAM_H__
#define __HTTP_REQUESTLINESTREAM_H__

/* Functionality name: http */
/* Detail name: requestlinestream */

struct http_requestlinestream_t_ {
	enum {
		HTTP_REQUESTLINESTREAM_STATE_METHOD,
		HTTP_REQUESTLINESTREAM_STATE_URI,
		HTTP_REQUESTLINESTREAM_STATE_VERSION,
		HTTP_REQUESTLINESTREAM_STATE_END
	} state;
	HTTP_METHOD method;
	CONST UB *uri;
	W uri_len;
};
typedef struct http_requestlinestream_t_ http_requestlinestream_t;

IMPORT W http_requestlinestream_initialize(http_requestlinestream_t *stream, HTTP_METHOD method, CONST UB *uri, W uri_len);
IMPORT VOID http_requestlinestream_finalize(http_requestlinestream_t *stream);
IMPORT Bool http_requestlinestream_make(http_requestlinestream_t *stream, CONST UB **str, W *len);

#endif

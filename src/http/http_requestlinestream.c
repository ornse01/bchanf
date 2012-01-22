/*
 * http_requestlinestream.c
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

#include "http_requestlinestream.h"

#include	<basic.h>
#include	<bstdio.h>
#include	<bctype.h>
#include	<errcode.h>

#include    "http_typedef.h"

#ifdef BCHAN_CONFIG_DEBUG
# define DP(arg) printf arg
# define DP_ER(msg, err) printf("%s (%d/%x)\n", msg, err>>16, err)
#else
# define DP(arg) /**/
# define DP_ER(msg, err) /**/
#endif

LOCAL UB http_requestlinestream_method_get[] = "GET ";
LOCAL UB http_requestlinestream_method_post[] = "POST ";

LOCAL UB http_requestlinestream_http_version[] = " HTTP/1.1\r\n";

LOCAL VOID http_requestlinestream_makemethod(HTTP_METHOD method, CONST UB **str, W *len)
{
	if (method == HTTP_METHOD_GET) {
		*str = http_requestlinestream_method_get;
		*len = sizeof(http_requestlinestream_method_get) - 1;
	} else if (method == HTTP_METHOD_POST) {
		*str = http_requestlinestream_method_post;
		*len = sizeof(http_requestlinestream_method_post) - 1;
	}
}

EXPORT Bool http_requestlinestream_make(http_requestlinestream_t *stream, CONST UB **str, W *len)
{
	switch (stream->state) {
	case HTTP_REQUESTLINESTREAM_STATE_METHOD:
		http_requestlinestream_makemethod(stream->method, str, len);
		stream->state = HTTP_REQUESTLINESTREAM_STATE_URI;
		break;
	case HTTP_REQUESTLINESTREAM_STATE_URI:
		*str = stream->uri;
		*len = stream->uri_len;
		stream->state = HTTP_REQUESTLINESTREAM_STATE_VERSION;
		break;
	case HTTP_REQUESTLINESTREAM_STATE_VERSION:
		*str = http_requestlinestream_http_version;
		*len = sizeof(http_requestlinestream_http_version) - 1;
		stream->state = HTTP_REQUESTLINESTREAM_STATE_END;
		break;
	case HTTP_REQUESTLINESTREAM_STATE_END:
		return False;
	}
	return True;
}

EXPORT W http_requestlinestream_initialize(http_requestlinestream_t *stream, HTTP_METHOD method, CONST UB *uri, W uri_len)
{
	if ((method != HTTP_METHOD_GET)&&(method != HTTP_METHOD_POST)) {
		return ER_NOSPT; /* TODO: detail error code */
	}
	stream->state = HTTP_REQUESTLINESTREAM_STATE_METHOD;
	stream->method = method;
	stream->uri = uri;
	stream->uri_len = uri_len;

	return ER_OK;
}

EXPORT VOID http_requestlinestream_finalize(http_requestlinestream_t *stream)
{
}

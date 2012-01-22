/*
 * http_defaultheaderstream.c
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

#include "http_defaultheaderstream.h"

#include	<basic.h>
#include	<bstdio.h>
#include	<bctype.h>
#include	<errcode.h>

#ifdef BCHAN_CONFIG_DEBUG
# define DP(arg) printf arg
# define DP_ER(msg, err) printf("%s (%d/%x)\n", msg, err>>16, err)
#else
# define DP(arg) /**/
# define DP_ER(msg, err) /**/
#endif

LOCAL UB http_defaultheaderstream_before_host[] = "Accept-Encoding: gzip\r\nHost: ";
LOCAL UB http_defaultheaderstream_after_host[] = "\r\n";

EXPORT Bool http_defaultheaderstream_make(http_defaultheaderstream_t *stream, CONST UB **str, W *len)
{
	switch (stream->state) {
	case HTTP_DEFAULTHEADERSTREAM_STATE_BEFORE_HOST:
		*str = http_defaultheaderstream_before_host;
		*len = sizeof(http_defaultheaderstream_before_host) - 1;
		stream->state = HTTP_DEFAULTHEADERSTREAM_STATE_HOST;
		break;
	case HTTP_DEFAULTHEADERSTREAM_STATE_HOST:
		*str = stream->host;
		*len = stream->host_len;
		stream->state = HTTP_DEFAULTHEADERSTREAM_STATE_AFTER_HOST;
		break;
	case HTTP_DEFAULTHEADERSTREAM_STATE_AFTER_HOST:
		*str = http_defaultheaderstream_after_host;
		*len = sizeof(http_defaultheaderstream_after_host) - 1;
		stream->state = HTTP_DEFAULTHEADERSTREAM_STATE_END;
		break;
	case HTTP_DEFAULTHEADERSTREAM_STATE_END:
		return False;
	}
	return True;
}

EXPORT VOID http_defaultheaderstream_initialize(http_defaultheaderstream_t *stream, CONST UB *host, W host_len)
{
	stream->state = HTTP_DEFAULTHEADERSTREAM_STATE_BEFORE_HOST;
	stream->host = host;
	stream->host_len = host_len;
}

EXPORT VOID http_defaultheaderstream_finalize(http_defaultheaderstream_t *stream)
{
}

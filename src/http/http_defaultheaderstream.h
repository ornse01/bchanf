/*
 * http_defaultheaderstream.h
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
/* Detail name: defaultheaderstream */

#include    <basic.h>

/* HTTP header generator for using in http_connector_t. */
/*   Accept-Encoding, Host, Connection. */

#ifndef __HTTP_DEFAULTHEADERSTREAM_H__
#define __HTTP_DEFAULTHEADERSTREAM_H__

/* Functionality name: http */
/* Detail name: defaultheaderstream */

struct http_defaultheaderstream_t_ {
	enum {
		HTTP_DEFAULTHEADERSTREAM_STATE_BEFORE_HOST,
		HTTP_DEFAULTHEADERSTREAM_STATE_HOST,
		HTTP_DEFAULTHEADERSTREAM_STATE_AFTER_HOST,
		HTTP_DEFAULTHEADERSTREAM_STATE_END
	} state;
	CONST UB *host;
	W host_len;
};
typedef struct http_defaultheaderstream_t_ http_defaultheaderstream_t;

IMPORT VOID http_defaultheaderstream_initialize(http_defaultheaderstream_t *stream, CONST UB *host, W host_len);
IMPORT VOID http_defaultheaderstream_finalize(http_defaultheaderstream_t *stream);
IMPORT Bool http_defaultheaderstream_make(http_defaultheaderstream_t *stream, CONST UB **str, W *len);

#endif

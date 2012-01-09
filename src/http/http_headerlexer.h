/*
 * http_headerlexer.h
 *
 * Copyright (c) 2011-2012 project bchan
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
/* Detail name: headerlexer */

#include    <basic.h>

/* see rfc2616 4.2 Message Headers. */

#ifndef __HTTP_HEADERLEXER_H__
#define __HTTP_HEADERLEXER_H__

/* Functionality name: http */
/* Detail name: headerlexer */
struct http_headerlexer_t_ {
	enum {
		HTTP_HEADERLEXER_STATE_SEARCH_HEADER,
		HTTP_HEADERLEXER_STATE_SEARCH_HEADER_CR,
		HTTP_HEADERLEXER_STATE_READ_HEADER_FIELDNAME,
		HTTP_HEADERLEXER_STATE_READ_HEADER_FIELDVALUE,
		HTTP_HEADERLEXER_STATE_READ_HEADER_FIELDVALUE_LWS,
		HTTP_HEADERLEXER_STATE_HERDER_CR,
		HTTP_HEADERLEXER_STATE_READ_VALUE_MESSAGE,
	} state;
};
typedef struct http_headerlexer_t_ http_headerlexer_t;

/* Functionality name: http */
/* Detail name: headerlexer */
/* Data structure identifier: RESULT */

/* should implement field-value? but difficult. */
enum HTTP_HEADERLEXER_RESULT_ {
	HTTP_HEADERLEXER_RESULT_NONE,
	HTTP_HEADERLEXER_RESULT_FIELDNAME,
	HTTP_HEADERLEXER_RESULT_FIELDNAME_END,
	HTTP_HEADERLEXER_RESULT_FIELDCONTENT,
	HTTP_HEADERLEXER_RESULT_LWS,
	HTTP_HEADERLEXER_RESULT_MESSAGEHEADER_END,
};
typedef enum HTTP_HEADERLEXER_RESULT_ HTTP_HEADERLEXER_RESULT;

IMPORT W http_headerlexer_initialize(http_headerlexer_t *lexer);
IMPORT VOID http_headerlexer_finalize(http_headerlexer_t *lexer);
IMPORT VOID http_headerlexer_inputchar(http_headerlexer_t *lexer, UB ch, HTTP_HEADERLEXER_RESULT *result);

#endif

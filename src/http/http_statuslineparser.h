/*
 * http_transferdecoder.h
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
/* Detail name: statuslineparser */

#include    <basic.h>

#include    <parse/tokenchecker.h>
#include    "http_typedef.h"

/* rfc2616 6.1 Status-Line */

#ifndef __HTTP_STATUSLINEPARSER_H__
#define __HTTP_STATUSLINEPARSER_H__

/* Functionality name: http */
/* Detail name: statuslineparser */
/* Data structure identifier: statuscode */
struct http_statuslineparser_result_ {
	enum {
		HTTP_STATUSLINEPARSER_ERROR_NONE,
		HTTP_STATUSLINEPARSER_ERROR_UNSUPPORTED_HTTP_VERSION,
		HTTP_STATUSLINEPARSER_ERROR_UNSUPPORTED_STATUS_CODE,
		HTTP_STATUSLINEPARSER_ERROR_INVALID_FORMAT,
	} error;
	HTTP_HTTPVERSION version;
	HTTP_STATUSCODE statuscode;
};
typedef struct http_statuslineparser_result_ http_statuslineparser_result;

/* Functionality name: http */
/* Detail name: statuslineparser */
struct http_statuslineparser_t_ {
	enum {
		HTTP_STATUSLINEPARSER_STATE_HTTP_VERSION,
		HTTP_STATUSLINEPARSER_STATE_STATUS_CODE,
		HTTP_STATUSLINEPARSER_STATE_SKIP,
		HTTP_STATUSLINEPARSER_STATE_SKIP_CR,
	} state;
	struct {
		tokenchecker_t version;
		tokenchecker_t status;
	} checker;
	http_statuslineparser_result result;
};
typedef struct http_statuslineparser_t_ http_statuslineparser_t;

IMPORT VOID http_statuslineparser_initialize(http_statuslineparser_t *parser);
IMPORT VOID http_statuslineparser_finalize(http_statuslineparser_t *parser);
IMPORT Bool http_statuslineparser_inputchar(http_statuslineparser_t *parser, UB ch, http_statuslineparser_result *result);

#endif

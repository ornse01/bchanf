/*
 * http_defaultheaderparser.h
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
/* Detail name: defaultheaderparser */

#include    <basic.h>

#include    "http_typedef.h"
#include    "http_headerlexer.h"
#include    <parse/tokenchecker.h>

/* HTTP header parser for using in http_connector_t. */
/*   Content-Encoding, Content-Length, Transfer-Encoding, Connection */

#ifndef __HTTP_DEFAULTHEADERPARSER_H__
#define __HTTP_DEFAULTHEADERPARSER_H__

/* Functionality name: http */
/* Detail name: defaultheaderparser */
struct http_defaultheaderparser_t_ {
	enum {
		HTTP_DEFAULTHEADERPARSER_STATE_FIELDNAME,
		HTTP_DEFAULTHEADERPARSER_STATE_SKIP_FIELDNAME,
		HTTP_DEFAULTHEADERPARSER_STATE_SKIP_FIELDVALUE,
		HTTP_DEFAULTHEADERPARSER_STATE_CONTENT_ENCODING_LWS,
		HTTP_DEFAULTHEADERPARSER_STATE_CONTENT_ENCODING,
		HTTP_DEFAULTHEADERPARSER_STATE_CONTENT_LENGTH_LWS,
		HTTP_DEFAULTHEADERPARSER_STATE_CONTENT_LENGTH,
		HTTP_DEFAULTHEADERPARSER_STATE_TRANSFER_ENCODING_LWS,
		HTTP_DEFAULTHEADERPARSER_STATE_TRANSFER_ENCODING,
		HTTP_DEFAULTHEADERPARSER_STATE_CONNECTION_LWS,
		HTTP_DEFAULTHEADERPARSER_STATE_CONNECTION,
	} state;
	http_headerlexer_t header;
	struct {
		tokenchecker_t fieldname;
		tokenchecker_t transfercoding;
		tokenchecker_t contentcoding;
		tokenchecker_t connection;
	} checker;
	struct {
		W contentlength;
		HTTP_TRANSFERCODING_TYPE transfercoding;
		HTTP_CONTENTCODING_VALUE contentcoding;
		Bool connectionclosed;
	} result;
};
typedef struct http_defaultheaderparser_t_ http_defaultheaderparser_t;

/* Functionality name: http */
/* Detail name: defaultheaderparser */
/* Data structure identifier: RESULT */
enum HTTP_DEFAULTHEADERPARSER_RESULT_ {
	HTTP_DEFAULTHEADERPARSER_RESULT_NONE,
	/*HTTP_DEFAULTHEADERPARSER_RESULT_FIELD_VALUE_END,*/
	HTTP_DEFAULTHEADERPARSER_RESULT_MESSAGE_HEADER_END
};
typedef enum HTTP_DEFAULTHEADERPARSER_RESULT_ HTTP_DEFAULTHEADERPARSER_RESULT;

IMPORT VOID http_defaultheaderparser_initialize(http_defaultheaderparser_t *parser);
IMPORT VOID http_defaultheaderparser_finalize(http_defaultheaderparser_t *parser);
IMPORT VOID http_defaultheaderparser_inputchar(http_defaultheaderparser_t *parser, UB ch, HTTP_DEFAULTHEADERPARSER_RESULT *result);
IMPORT W http_defaultheaderparsr_contentlength(http_defaultheaderparser_t *parser);
IMPORT HTTP_TRANSFERCODING_TYPE http_defaultheaderparsr_transferencoding(http_defaultheaderparser_t *parser);
IMPORT HTTP_CONTENTCODING_VALUE http_defaultheaderparsr_contentencoding(http_defaultheaderparser_t *parser);
IMPORT Bool http_defaultheaderparser_connectionclosed(http_defaultheaderparser_t *parser);

#endif

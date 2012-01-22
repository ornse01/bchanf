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
/* Detail name: transferdecoder */

#include    <basic.h>

#include    "http_typedef.h"

/* rfc2616 3.6 Transfer Codings */

#ifndef __HTTP_TRANSFERDECODER_H__
#define __HTTP_TRANSFERDECODER_H__

/* Functionality name: http */
/* Detail name: transferdecoder */
/* Data structure identifier: result */
struct http_transferdecoder_result_ {
	enum {
		HTTP_TRANSFERDECODER_RESULTTYPE_DATA,
		HTTP_TRANSFERDECODER_RESULTTYPE_END,
	} type;
	UB *data;
	W len;
};
typedef struct http_transferdecoder_result_ http_transferdecoder_result;

/* Functionality name: http */
/* Detail name: transferdecoderidentity */
struct http_transferdecoderidentity_t_ {
	http_transferdecoder_result result[2];
	W content_length;
	W pushed_len;
};
typedef struct http_transferdecoderidentity_t_ http_transferdecoderidentity_t;

/* Functionality name: http */
/* Detail name: chuhkedbodyparser */
struct http_chunkedbodyparser_t_ {
	enum {
		HTTP_CHUNKEDBODYPARSER_STATE_CHUNK_SIZE,
		HTTP_CHUNKEDBODYPARSER_STATE_CHUNK_SIZE_CR,
		HTTP_CHUNKEDBODYPARSER_STATE_CHUNK_DATA,
		HTTP_CHUNKEDBODYPARSER_STATE_CHUNK_DATA_END,
		HTTP_CHUNKEDBODYPARSER_STATE_CHUNK_DATA_END_CR,
		HTTP_CHUNKEDBODYPARSER_STATE_CHUNK_EXTENTION,
		HTTP_CHUNKEDBODYPARSER_STATE_CHUNK_EXTENTION_CR,
		HTTP_CHUNKEDBODYPARSER_STATE_LAST_CHUNK_EXTENTION,
		HTTP_CHUNKEDBODYPARSER_STATE_LAST_CHUNK_EXTENTION_CR,
		HTTP_CHUNKEDBODYPARSER_STATE_AFTER_CHUNK,
		HTTP_CHUNKEDBODYPARSER_STATE_AFTER_CHUNK_CR,
		HTTP_CHUNKEDBODYPARSER_STATE_TRAILER,
		HTTP_CHUNKEDBODYPARSER_STATE_TRAILER_CR,
		HTTP_CHUNKEDBODYPARSER_STATE_END,
	} state;
	W chunk_size;
	W chunk_pushed_len;
};
typedef struct http_chunkedbodyparser_t_ http_chunkedbodyparser_t;

/* Functionality name: http */
/* Detail name: transferdecoderchunked */
#define HTTP_TRANSFERDECODER_CHUNKED_RESULTBUFLEN 4
struct http_transferdecoderchunked_t_ {
	http_transferdecoder_result result[HTTP_TRANSFERDECODER_CHUNKED_RESULTBUFLEN];
	http_chunkedbodyparser_t parser;
};
typedef struct http_transferdecoderchunked_t_ http_transferdecoderchunked_t;

/* Functionality name: http */
/* Detail name: transferdecoder */
struct http_transferdecoder_t_ {
	HTTP_TRANSFERCODING_TYPE type;
	union {
		http_transferdecoderidentity_t identity;
		http_transferdecoderchunked_t chunked;
	} d;
};
typedef struct http_transferdecoder_t_ http_transferdecoder_t;

IMPORT VOID http_transferdecoder_initialize(http_transferdecoder_t *decoder, HTTP_TRANSFERCODING_TYPE type, W content_length);
IMPORT VOID http_transferdecoder_finalize(http_transferdecoder_t *decoder);
IMPORT W http_transferdecoder_inputentitybody(http_transferdecoder_t *decoder, UB *data, W data_len, http_transferdecoder_result **result, W *result_len);

#endif

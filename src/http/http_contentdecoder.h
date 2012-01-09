/*
 * http_contentdecoder.h
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
/* Detail name: contentdecoder */

#include    <basic.h>
#include	<util/zlib.h>

/* rfc2616 3.5 Content Codings */

#ifndef __HTTP_CONTENTDECODER_H__
#define __HTTP_CONTENTDECODER_H__

/* Functionality name: http */
/* Detail name: contentcoding */
/* Data structure identifier: value */
typedef enum {
	HTTP_CONTENTCODING_VALUE_IDENTITY,
	HTTP_CONTENTCODING_VALUE_GZIP,
	HTTP_CONTENTCODING_VALUE_COMPRESS, /* unsupported yet */
	HTTP_CONTENTCODING_VALUE_DEFLATE, /* unsupported yet */
} HTTP_CONTENTCODING_VALUE;

/* Functionality name: http */
/* Detail name: contentdecoder */
/* Data structure identifier: result */
struct http_contentdecoder_result_ {
	enum {
		HTTP_CONTENTDECODER_RESULTTYPE_DATA,
		HTTP_CONTENTDECODER_RESULTTYPE_END,
	} type;
	UB *data;
	W len;
};
typedef struct http_contentdecoder_result_ http_contentdecoder_result;

/* Functionality name: http */
/* Detail name: contentdecoderidentity */
struct http_contentdecoderidentity_t_ {
	http_contentdecoder_result result;
	enum {
		HTTP_CONTENTDECODERIDENTITY_STATE_DATA,
		HTTP_CONTENTDECODERIDENTITY_STATE_END,
	} state;
	UB *data;
	W data_len;
};
typedef struct http_contentdecoderidentity_t_ http_contentdecoderidentity_t;

/* Functionality name: gzip */
/* Detail name: headerskip */
struct gzip_headerskip_t_ {
	enum {
		GZIP_HEADERSKIP_STATE_MAGIC_NUMBER_FIRST,
		GZIP_HEADERSKIP_STATE_MAGIC_NUMBER_SECOND,
		GZIP_HEADERSKIP_STATE_METHOD,
		GZIP_HEADERSKIP_STATE_FLAGS,
		GZIP_HEADERSKIP_STATE_SKIP_UNUSED_FIELD,
		GZIP_HEADERSKIP_STATE_EXTRA_FIELD_LEN_FIRST,
		GZIP_HEADERSKIP_STATE_EXTRA_FIELD_LEN_SECOND,
		GZIP_HEADERSKIP_STATE_SKIP_EXTRA_FIELD,
		GZIP_HEADERSKIP_STATE_SKIP_FILE_NAME,
		GZIP_HEADERSKIP_STATE_SKIP_FILE_COMMENT,
		GZIP_HEADERSKIP_STATE_SKIP_HEADER_CRC_FIRST,
		GZIP_HEADERSKIP_STATE_SKIP_HEADER_CRC_SECOND,
	} state;
	UB flags;
	W extra_len;
	W unused;
};
typedef struct gzip_headerskip_t_ gzip_headerskip_t;

/* Functionality name: http */
/* Detail name: contentdecodergzip */
#define HTTP_CONTENTDECODERGZIP_BUFLEN 512
struct http_contentdecodergzip_t_ {
	http_contentdecoder_result result[1];
	enum {
		HTTP_CONTENTDECODERGZIP_STATE_SKIP_HEADER,
		HTTP_CONTENTDECODERGZIP_STATE_DECODE,
		HTTP_CONTENTDECODERGZIP_STATE_END_OF_DATA, /* tmp */
	} state;
	gzip_headerskip_t gzipheader;
	z_stream z;
	UB buffer[HTTP_CONTENTDECODERGZIP_BUFLEN];
};
typedef struct http_contentdecodergzip_t_ http_contentdecodergzip_t;

/* Functionality name: http */
/* Detail name: contentdecoder */
struct http_contentdecoder_t_ {
	HTTP_CONTENTCODING_VALUE type;
	union {
		http_contentdecoderidentity_t identity;
		http_contentdecodergzip_t gzip;
	} d;
};
typedef struct http_contentdecoder_t_ http_contentdecoder_t;

IMPORT W http_contentdecoder_initialize(http_contentdecoder_t *decoder, HTTP_CONTENTCODING_VALUE type);
IMPORT VOID http_contentdecoder_finalize(http_contentdecoder_t *decoder);
IMPORT W http_contentdecoder_inputentitybody(http_contentdecoder_t *decoder, UB *data, W data_len);
IMPORT W http_contentdecoder_inputendofdata(http_contentdecoder_t *decoder);
IMPORT W http_contentdecoder_outputdata(http_contentdecoder_t *decoder, http_contentdecoder_result **result, W *result_len, Bool *need_input);

#endif

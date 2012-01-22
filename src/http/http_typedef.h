/*
 * http_typedef.h
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
/* Detail name: typedef */

#include    <basic.h>

#ifndef __HTTP_TYPEDEF_H__
#define __HTTP_TYPEDEF_H__

/* Functionality name: http */
/* Detail name: */
/* Data structure identifier: method */
/*  rfc2616 5.1.1 Method */
typedef enum {
	HTTP_METHOD_OPTIONS,
	HTTP_METHOD_GET,
	HTTP_METHOD_HEAD,
	HTTP_METHOD_POST,
	HTTP_METHOD_PUT,
	HTTP_METHOD_DELETE,
	HTTP_METHOD_TRACE,
	HTTP_METHOD_CONNECT,
} HTTP_METHOD;

/* Functionality name: http */
/* Detail name: transfercoding */
/* Data structure identifier: type */
/*  rfc2616 3.6 Transfer Codings */
typedef enum {
	HTTP_TRANSFERCODING_TYPE_IDENTITY,
	HTTP_TRANSFERCODING_TYPE_CHUNKED,
} HTTP_TRANSFERCODING_TYPE;

/* Functionality name: http */
/* Detail name: contentcoding */
/* Data structure identifier: value */
/*  rfc2616 3.5 Content Codings */
typedef enum {
	HTTP_CONTENTCODING_VALUE_IDENTITY,
	HTTP_CONTENTCODING_VALUE_GZIP,
	HTTP_CONTENTCODING_VALUE_COMPRESS, /* unsupported yet */
	HTTP_CONTENTCODING_VALUE_DEFLATE, /* unsupported yet */
} HTTP_CONTENTCODING_VALUE;

#endif

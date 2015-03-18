/*
 * htmlform_urlencoder.h
 *
 * Copyright (c) 2015 project bchan
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

/* http://tools.ietf.org/html/rfc1866#section-8.2.1 */

/* Vendor name: */
/* Functionality name: htmlform */
/* Detail name: urnencoder */

#include    <basic.h>

#ifndef __HTMLFORM_URLENCODER_H__
#define __HTMLFORM_URLENCODER_H__

/* Functionality name: htmlform */
/* Detail name: */
/* Data structure identifier: field */
struct htmlform_field_ {
	UB *name;
	W name_len;
	UB *value;
	W value_len;
};
typedef struct htmlform_field_ htmlform_field;

/* Functionality name: htmlform */
/* Detail name: urlencoder */
struct htmlform_urlencoder_t_ {
	htmlform_field *fields;
	W fields_len;
	enum {
		HTMLFORM_URLENCODER_STATE_NAME,
		HTMLFORM_URLENCODER_STATE_EQUAL,
		HTMLFORM_URLENCODER_STATE_VALUE,
		HTMLFORM_URLENCODER_STATE_AMP
	} state;
	W field_index;
	W src_pos;
	UB buf[3];
};
typedef struct htmlform_urlencoder_t_ htmlform_urlencoder_t;

IMPORT VOID htmlform_urlencoder_initialize(htmlform_urlencoder_t *encoder, htmlform_field *fields, W fields_len);
IMPORT VOID htmlform_urlencoder_finalize(htmlform_urlencoder_t *encoder);
IMPORT Bool htmlform_urlencoder_next(htmlform_urlencoder_t *encoder, UB **str, W *len);

#endif

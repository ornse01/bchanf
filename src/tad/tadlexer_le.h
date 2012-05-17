/*
 * tadlexer_le.h
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
/* Functionality name: tadlexer */
/* Detail name: le */

#include    <basic.h>
#include	<btron/dp.h>
#include    <tad.h>

#ifndef __TADLEXER_LE_H__
#define __TADLEXER_LE_H__

/* Functionality name: tadlexer */
/* Detail name: le */
/* Data structure identifier: result */
struct tadlexer_le_result_ {
	enum {
		TADLEXER_LE_RESULTTYPE_READING_SEGMENT,
		TADLEXER_LE_RESULTTYPE_SEGMENT_END,
	} type;
	UW flag;
	union {
		TC ch;
		struct {
			UB id;
			UW len;
		} seg;
	} val;
};
typedef struct tadlexer_le_result_ tadlexer_le_result;

IMPORT Bool tadlexer_le_result_is_fixedsegment(tadlexer_le_result *result);
IMPORT Bool tadlexer_le_result_is_variablesegment(tadlexer_le_result *result);
IMPORT Bool tadlexer_le_result_is_segmentid_determined(tadlexer_le_result *result);
IMPORT Bool tadlexer_le_result_is_lengthdetermined(tadlexer_le_result *result);
IMPORT Bool tadlexer_le_result_is_readingdata(tadlexer_le_result *result);

/* Functionality name: tadlexer */
/* Detail name: le */
struct tadlexer_le_t_ {
	enum {
		TADLEXER_LE_STATE_START,
		TADLEXER_LE_STATE_FIRST_BYTE,
		TADLEXER_LE_STATE_SEGLEN_FIRST,
		TADLEXER_LE_STATE_SEGLEN_SECOND,
		TADLEXER_LE_STATE_LSEGLEN_FIRST,
		TADLEXER_LE_STATE_LSEGLEN_SECOND,
		TADLEXER_LE_STATE_LSEGLEN_THIRD,
		TADLEXER_LE_STATE_LSEGLEN_FORTH,
		TADLEXER_LE_STATE_DATA,
	} state;
	UB segid;
	UW len;
	UW len_read;
	tadlexer_le_result buf;
};
typedef struct tadlexer_le_t_ tadlexer_le_t;

IMPORT VOID tadlexer_le_initialize(tadlexer_le_t *lexer);
IMPORT VOID tadlexer_le_finalize(tadlexer_le_t *lexer);
IMPORT VOID tadlexer_le_inputbyte(tadlexer_le_t *lexer, UB b, tadlexer_le_result **result, W *result_len);

#endif __TADLEXER_LE_H__

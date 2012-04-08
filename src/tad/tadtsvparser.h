/*
 * tadtsvparser.h
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
/* Functionality name: tadtsvarser */
/* Detail name: */

#include    <basic.h>
#include	<btron/dp.h>
#include    <tad.h>

#include    "tadstack.h"

/* parser for "Tab-Separated Values" format in TAD text. */

#ifndef __TADTSVPARSER_H__
#define __TADTSVPARSER_H__

/* Functionality name: tadtsvparser */
/* Detail name: result */
enum TADTSVPARSER_RESULT_ {
	TADTSVPARSER_RESULT_IGNORE_SEGMENT,
	TADTSVPARSER_RESULT_FIELD,
	TADTSVPARSER_RESULT_FIELD_END,
	TADTSVPARSER_RESULT_RECORD_END,
	TADTSVPARSER_RESULT_FORMAT_ERROR,
};
typedef enum TADTSVPARSER_RESULT_ TADTSVPARSER_RESULT;

/* Functionality name: tadtsvparser */
/* Detail name: */
struct tadtsvparser_t_ {
	enum {
		TADTSVPARSER_STATE_START,
		TADTSVPARSER_STATE_RECORD_START,
		TADTSVPARSER_STATE_READING_VALUE,
		TADTSVPARSER_STATE_READING_VALUE_NESTED_SEGMENT,
	} state;
	tadstack_t tadstack;
};
typedef struct tadtsvparser_t_ tadtsvparser_t;

IMPORT VOID tadtsvparser_initialize(tadtsvparser_t *parser);
IMPORT VOID tadtsvparser_finalize(tadtsvparser_t *parser);
IMPORT TADTSVPARSER_RESULT tadtsvparser_inputcharactor(tadtsvparser_t *parser, TC ch);
IMPORT TADTSVPARSER_RESULT tadtsvparser_inputvsegment(tadtsvparser_t *parser, UH segid, UB *bin, W len);
IMPORT TADTSVPARSER_RESULT tadtsvparser_inputendofdata(tadtsvparser_t *parser);

#endif

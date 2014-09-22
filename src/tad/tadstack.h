/*
 * tadstack.h
 *
 * Copyright (c) 2012-2014 project bchan
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
/* Functionality name: tadstack */
/* Detail name: */

#include    <basic.h>
#include	<btron/dp.h>
#include    <tad.h>

#include	"tadsegment.h"
#include	"tadlangcode.h"

#ifndef __TADSTACK_H__
#define __TADSTACK_H__

/* Functionality name: tadstack */
/* Detail name: datatype */
enum TADSTACK_DATATYPE_ {
	TADSTACK_DATATYPE_TEXT,
	TADSTACK_DATATYPE_FIG
};
typedef enum TADSTACK_DATATYPE_ TADSTACK_DATATYPE;

/* Functionality name: tadstack */
/* Detail name: result */
enum TADSTACK_RESULT_ {
	TADSTACK_RESULT_OK,
	TADSTACK_RESULT_PUSH_STACK,
	TADSTACK_RESULT_POP_STACK,
	TADSTACK_RESULT_LANGUAGE_CHANGE,
	TADSTACK_RESULT_FORMAT_ERROR,
};
typedef enum TADSTACK_RESULT_ TADSTACK_RESULT;

/* Functionality name: tadstack */
/* Detail name: */
struct tadstack_t_ {
	enum {
		TADSTACK_STATE_START,
		TADSTACK_STATE_TEXT,
		TADSTACK_STATE_TEXT_LANGCODE,
		TADSTACK_STATE_FIG,
	} state;
	W nestlevel;
	tadlangcode_parser_t langparser;
	struct {
		TADSTACK_DATATYPE type;
		RECT view;
		RECT draw;
		UNITS h_unit;
		UNITS v_unit;
		/* for text */
		UH lang;
		UH bgpat;
		tadlangcode currentlang;
		/* for fig */
		W ratio;
	} data[16];
};
typedef struct tadstack_t_ tadstack_t;

IMPORT VOID tadstack_initialize(tadstack_t *stack);
IMPORT VOID tadstack_finalize(tadstack_t *stack);
IMPORT TADSTACK_RESULT tadstack_inputcharactor(tadstack_t *stack, TC ch);
IMPORT TADSTACK_RESULT tadstack_inputvsegment(tadstack_t *stack, UH segid, UB *bin, W len);
IMPORT TADSTACK_RESULT tadstack_inputsegment(tadstack_t *stack, tadsegment *segment);
IMPORT W tadstack_nestlevel(tadstack_t *stack);
IMPORT W tadstack_currentlangcode(tadstack_t *stack, tadlangcode *langcode);
IMPORT W tadstack_currentdata(tadstack_t *stack, TADSTACK_DATATYPE *type);
IMPORT W tadstack_currentview(tadstack_t *stack, RECT *r);
IMPORT W tadstack_currentdraw(tadstack_t *stack, RECT *r);
IMPORT W tadstack_currenthunit(tadstack_t *stack, UNITS *units);
IMPORT W tadstack_currentvunit(tadstack_t *stack, UNITS *units);

#endif

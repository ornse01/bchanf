/*
 * texteditor_insertfilter.h
 *
 * Copyright (c) 2014 project bchan
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

#include    <basic.h>

#include	<tad/tadstack.h>
#include	<tad/taditerator.h>
#include	<tad/tadlangcode.h>

#ifndef __TEXTEDITOR_INSERTFILTER_H__
#define __TEXTEDITOR_INSERTFILTER_H__

#define TEXTEDITOR_INSERTFILTER_TEXTBUFFER_SIZE 10

/* Functionality name: texteditor */
/* Detail name: insertfilter */
struct texteditor_insertfilter_t_ {
	enum {
		TEXTEDITOR_INSTERTFILTER_STATE_START,
		TEXTEDITOR_INSTERTFILTER_STATE_FIRST_LANGCODE,
		TEXTEDITOR_INSTERTFILTER_STATE_RUNNING,
		TEXTEDITOR_INSTERTFILTER_STATE_LAST_LANGCODE,
		TEXTEDITOR_INSTERTFILTER_STATE_LAST,
	} state;
	struct {
		tadlangcode lang;
		RATIO w_ratio;
	} first;
	struct {
		tadlangcode lang;
		RATIO w_ratio;
	} current;
	tadstack_t tadstack;
	taditerator_t iterator;
	TC buffer[TEXTEDITOR_INSERTFILTER_TEXTBUFFER_SIZE];
};
typedef struct texteditor_insertfilter_t_ texteditor_insertfilter_t;

/* Functionality name: texteditor */
/* Detail name: insertfilter */
/* Data structure identifier: result */
struct texteditor_insertfilter_result_ {
	UB *data;
	W len;
};
typedef struct texteditor_insertfilter_result_ texteditor_insertfilter_result;

IMPORT VOID texteditor_insertfilter_initialize(texteditor_insertfilter_t *filter, tadlangcode *lang, RATIO w_ratio, UB *data, W len);
IMPORT VOID texteditor_insertfilter_finalize(texteditor_insertfilter_t *filter);

IMPORT Bool texteditor_insertfilter_next(texteditor_insertfilter_t *filter, texteditor_insertfilter_result *result);

#endif

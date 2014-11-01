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
#include	"texteditor_stackfilter.h"

#ifndef __TEXTEDITOR_INSERTFILTER_H__
#define __TEXTEDITOR_INSERTFILTER_H__

typedef struct texteditor_insertfilter_t_ texteditor_insertfilter_t;
typedef struct texteditor_insertfilterresult_t_ texteditor_insertfilterresult_t;

/* Functionality name: texteditor */
/* Detail name: insertfilterresult */
struct texteditor_insertfilterresult_t_ {
	texteditor_insertfilter_t *filter;
};

/* Functionality name: texteditor */
/* Detail name: insertfilter */
struct texteditor_insertfilter_t_ {
	texteditor_stackfilter_t stack;
	enum {
		TEXTEDITOR_INSERTFILTER_STATE_START,
		TEXTEDITOR_INSERTFILTER_STATE_RUNNING,
		TEXTEDITOR_INSERTFILTER_STATE_END,
	} state;
	struct {
		tadlangcode lang;
		RATIO w_ratio;
	} first;
	struct {
		tadlangcode lang;
		RATIO w_ratio;
	} current;
	struct {
#define TEXTEDITOR_INSERTFILTER_RESULT_BUFFER_LEN (3)
		tadsegment segs[TEXTEDITOR_INSERTFILTER_RESULT_BUFFER_LEN];
		W used;
		W consumed;
	} result_buffer;
	texteditor_insertfilterresult_t result;
};

IMPORT VOID texteditor_insertfilter_initialize(texteditor_insertfilter_t *filter, tadlangcode *lang, RATIO w_ratio);
IMPORT VOID texteditor_insertfilter_finalize(texteditor_insertfilter_t *filter);
IMPORT W texteditor_insertfilter_put(texteditor_insertfilter_t *filter, tadsegment *segment, texteditor_insertfilterresult_t **result);
#define TEXTEDITOR_INSERTFILTER_PUT_RESULT_OK (0)
#define TEXTEDITOR_INSERTFILTER_PUT_RESULT_FORMAT_ERROR (-1)
IMPORT VOID texteditor_insertfilter_endinput(texteditor_insertfilter_t *filter, texteditor_insertfilterresult_t **result);
IMPORT Bool texteditor_insertfilterresult_next(texteditor_insertfilterresult_t *filter, tadsegment **segment);

#endif

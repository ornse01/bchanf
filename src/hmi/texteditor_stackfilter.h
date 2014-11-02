/*
 * texteditor_stackfilter.h
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

#include	<tad/tadsegment.h>
#include	<tad/tadstack.h>

#ifndef __TEXTEDITOR_STACKFILTER_H__
#define __TEXTEDITOR_STACKFILTER_H__

/* Functionality name: texteditor */
/* Detail name: stackfilterresult */
struct texteditor_stackfilterresult_t_ {
	tadsegment segs[1];
	W used;
	W consumed;
};
typedef struct texteditor_stackfilterresult_t_ texteditor_stackfilterresult_t;

/* Functionality name: texteditor */
/* Detail name: stackfilter */
struct texteditor_stackfilter_t_ {
	enum {
		TEXTEDITOR_STACKFILTER_STATE_WAIT_INPUT,
		TEXTEDITOR_STACKFILTER_STATE_HAS_RESULT,
	} state;
	tadstack_t tadstack;
	texteditor_stackfilterresult_t result_buffer;
};
typedef struct texteditor_stackfilter_t_ texteditor_stackfilter_t;

IMPORT VOID texteditor_stackfilter_initialize(texteditor_stackfilter_t *filter);
IMPORT VOID texteditor_stackfilter_finalize(texteditor_stackfilter_t *filter);
IMPORT W texteditor_stackfilter_put(texteditor_stackfilter_t *filter, tadsegment *segment, texteditor_stackfilterresult_t **result);
#define TEXTEDITOR_STACKFILTER_PUT_RESULT_OK (0)
#define TEXTEDITOR_STACKFILTER_PUT_RESULT_FORMAT_ERROR (-1)
IMPORT Bool texteditor_stackfilterresult_next(texteditor_stackfilterresult_t *filter, tadsegment **segment);

#endif

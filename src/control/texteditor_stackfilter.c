/*
 * texteditor_stackfilter.c
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

#include	"texteditor_stackfilter.h"

#include	<bstdio.h>

#include	<tad/tadsegment.h>
#include	<tad/tadlangcode.h>

#ifdef BCHAN_CONFIG_DEBUG
# define DP(arg) printf arg
# define DP_ER(msg, err) printf("%s (%d/%x)\n", msg, err>>16, err)
#else
# define DP(arg) /**/
# define DP_ER(msg, err) /**/
#endif

IMPORT W texteditor_stackfilter_setinput(texteditor_stackfilter_t *filter, tadsegment *segment)
{
	W nestlevel, err;
	TADSTACK_DATATYPE type;
	TADSTACK_RESULT result;

	if (filter->state == TEXTEDITOR_STACKFILTER_STATE_HAS_RESULT) {
		return TEXTEDITOR_STACKFILTER_SETINPUT_RESULT_FULL;
	}

	result = tadstack_inputsegment(&filter->tadstack, segment);

	nestlevel = tadstack_nestlevel(&filter->tadstack);
	if (nestlevel < 0) {
		return TEXTEDITOR_STACKFILTER_SETINPUT_RESULT_FORMAT_ERROR;
	}
	if (nestlevel > 0) {
		return TEXTEDITOR_STACKFILTER_SETINPUT_RESULT_OK;
	}
	if (result == TADSTACK_RESULT_POP_STACK) {
		return TEXTEDITOR_STACKFILTER_SETINPUT_RESULT_OK;
	}
	err = tadstack_currentdata(&filter->tadstack, &type);
	if (err < 0) {
		return TEXTEDITOR_STACKFILTER_SETINPUT_RESULT_FORMAT_ERROR;
	}
	if (type != TADSTACK_DATATYPE_TEXT) {
		return TEXTEDITOR_STACKFILTER_SETINPUT_RESULT_OK;
	}

	filter->state = TEXTEDITOR_STACKFILTER_STATE_HAS_RESULT;
	filter->result_buffer.segs[0] = *segment;
	filter->result_buffer.used = 1;
	filter->result_buffer.consumed = 0;

	return TEXTEDITOR_STACKFILTER_SETINPUT_RESULT_OK;
}

IMPORT Bool texteditor_stackfilter_getoutput(texteditor_stackfilter_t *filter, tadsegment **segment)
{
	if (filter->state == TEXTEDITOR_STACKFILTER_STATE_WAIT_INPUT) {
		return False;
	}

	*segment = filter->result_buffer.segs + filter->result_buffer.consumed;
	filter->result_buffer.consumed++;

	if (filter->result_buffer.consumed >= filter->result_buffer.used) {
		filter->result_buffer.used = 0;
		filter->result_buffer.consumed = 0;
		filter->state = TEXTEDITOR_STACKFILTER_STATE_WAIT_INPUT;
	}

	return True;
}

IMPORT VOID texteditor_stackfilter_initialize(texteditor_stackfilter_t *filter)
{
	tadstack_initialize(&filter->tadstack);
	filter->state = TEXTEDITOR_STACKFILTER_STATE_WAIT_INPUT;
	filter->result_buffer.used = 0;
	filter->result_buffer.consumed = 0;
}

IMPORT VOID texteditor_stackfilter_finalize(texteditor_stackfilter_t *filter)
{
	tadstack_finalize(&filter->tadstack);
}

/*
 * texteditor_insertfilter.c
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

#include	"texteditor_insertfilter.h"

#include	<bstdio.h>
#include	<bassert.h>

#include	<tad/taditerator.h>
#include	<tad/tadstack.h>

#include	"texteditor_stackfilter.h"

#ifdef BCHAN_CONFIG_DEBUG
# define DP(arg) printf arg
# define DP_ER(msg, err) printf("%s (%d/%x)\n", msg, err>>16, err)
#else
# define DP(arg) /**/
# define DP_ER(msg, err) /**/
#endif

#if 0
#define DP_STATE(s) printf("%s\n", s)
#else
#define DP_STATE(s) /**/
#endif

/* */

LOCAL Bool texteditor_insertfilter_w_ratio_isZenkaku(RATIO w_ratio)
{
	UB a,b;

	a = w_ratio >> 8;
	b = w_ratio & 0xFF;

	if (b == 0) {
		return True;
	}
	if (a == b) {
		return True;
	}
	return False;
}

LOCAL Bool texteditor_insertfilter_w_ratio_isSystemScript(tadlangcode *lang)
{
	return tadlangcodecmpTC((TC[]){0xFE21}, 1, lang);
}

/* */

LOCAL TC textediter_insertfilter_zenkakufusen[] = { 0xFFA2, 0x0006, 0x0300, 0x0101, 0x0101 };
LOCAL TC textediter_insertfilter_hankakufusen[] = { 0xFFA2, 0x0006, 0x0300, 0x0101, 0x0102 };
#define TEXTEDITOR_INSERTFILTER_FUSEN_LENGTH 5

LOCAL VOID texteditor_insertfilter_pushback_segment(texteditor_insertfilter_t *filter, tadsegment *segment)
{
	assert(filter->result_buffer.used < TEXTEDITOR_INSERTFILTER_RESULT_BUFFER_LEN);
	filter->result_buffer.segs[filter->result_buffer.used] = *segment;
	filter->result_buffer.used++;
}

LOCAL VOID texteditor_insertfilter_pushback_zenkakufusen(texteditor_insertfilter_t *filter)
{
	tadsegment segment;
	segment.type = TADSEGMENT_TYPE_VARIABLE;
	segment.value.variable.raw = (UB*)textediter_insertfilter_zenkakufusen;
	segment.value.variable.rawlen = TEXTEDITOR_INSERTFILTER_FUSEN_LENGTH * sizeof(TC);
	texteditor_insertfilter_pushback_segment(filter, &segment);
}

LOCAL VOID texteditor_insertfilter_pushback_hankakufusen(texteditor_insertfilter_t *filter)
{
	tadsegment segment;
	segment.type = TADSEGMENT_TYPE_VARIABLE;
	segment.value.variable.raw = (UB*)textediter_insertfilter_hankakufusen;
	segment.value.variable.rawlen = TEXTEDITOR_INSERTFILTER_FUSEN_LENGTH * sizeof(TC);
	texteditor_insertfilter_pushback_segment(filter, &segment);
}

LOCAL VOID texteditor_insertfilter_pushback_systemlangcode(texteditor_insertfilter_t *filter)
{
	tadsegment segment;
	segment.type = TADSEGMENT_TYPE_LANGCODE;
	segment.value.lang = (tadlangcode){0, 0x21};
	texteditor_insertfilter_pushback_segment(filter, &segment);
}

LOCAL VOID texteditor_insertfilter_pushback_firstlangcode(texteditor_insertfilter_t *filter)
{
	tadsegment segment;
	segment.type = TADSEGMENT_TYPE_LANGCODE;
	segment.value.lang = filter->first.lang;
	texteditor_insertfilter_pushback_segment(filter, &segment);
}

LOCAL W texteditor_insertfilter_put_filtering(texteditor_insertfilter_t *filter, tadsegment *segment)
{
	W ret;
	Bool cont;
	tadsegment *result;
	UB segid, subid, *segdata;
	UW seglen;

	ret = texteditor_stackfilter_setinput(&filter->stack, segment);
	if (ret == TEXTEDITOR_STACKFILTER_SETINPUT_RESULT_FORMAT_ERROR) {
		return TEXTEDITOR_INSERTFILTER_PUT_RESULT_FORMAT_ERROR;
	}
	for(;;) {
		cont = texteditor_stackfilter_getoutput(&filter->stack, &result);
		if (cont == False) {
			break;
		}

		if (result->type == TADSEGMENT_TYPE_VARIABLE) {
			tadsegment_getvariable(result, &segid, &seglen, &segdata);
			if (segid != TS_TFONT) {
				continue;
			}
			if (seglen < 6) {
				continue;
			}
			subid = *(UH*)segdata >> 8;
			if (subid != 3) {
				continue;
			}

			filter->current.w_ratio = *((UH*)segdata + 5);
		} else if (result->type == TADSEGMENT_TYPE_LANGCODE) {
			filter->current.lang = result->value.lang;
		}

		texteditor_insertfilter_pushback_segment(filter, result);
	}

	return TEXTEDITOR_INSERTFILTER_PUT_RESULT_OK;
}

LOCAL W texteditor_insertfilter_put_state_start(texteditor_insertfilter_t *filter, tadsegment *segment, texteditor_insertfilterresult_t **result)
{
	Bool ok;

	ok = texteditor_insertfilter_w_ratio_isZenkaku(filter->first.w_ratio);
	if (ok == False) {
		texteditor_insertfilter_pushback_zenkakufusen(filter);
	}
	ok = texteditor_insertfilter_w_ratio_isSystemScript(&filter->first.lang);
	if (ok == False) {
		texteditor_insertfilter_pushback_systemlangcode(filter);
	}

	*result = &filter->result;

	filter->state = TEXTEDITOR_INSERTFILTER_STATE_RUNNING;

	return texteditor_insertfilter_put_filtering(filter, segment);
}

LOCAL W texteditor_insertfilter_put_state_running(texteditor_insertfilter_t *filter, tadsegment *segment, texteditor_insertfilterresult_t **result)
{
	filter->result_buffer.used = 0;
	filter->result_buffer.consumed = 0;

	*result = &filter->result;

	return texteditor_insertfilter_put_filtering(filter, segment);
}

EXPORT W texteditor_insertfilter_put(texteditor_insertfilter_t *filter, tadsegment *segment, texteditor_insertfilterresult_t **result)
{
	switch (filter->state) {
	case TEXTEDITOR_INSERTFILTER_STATE_START:
		DP_STATE("put:STATE_START");
		return texteditor_insertfilter_put_state_start(filter, segment, result);
	case TEXTEDITOR_INSERTFILTER_STATE_RUNNING:
		DP_STATE("put:STATE_RUNNING");
		return texteditor_insertfilter_put_state_running(filter, segment, result);
	case TEXTEDITOR_INSERTFILTER_STATE_END:
		DP_STATE("put:STATE_END");
		return TEXTEDITOR_INSERTFILTER_PUT_RESULT_FORMAT_ERROR;
	}
	return TEXTEDITOR_INSERTFILTER_PUT_RESULT_FORMAT_ERROR;
}

EXPORT VOID texteditor_insertfilter_endinput(texteditor_insertfilter_t *filter, texteditor_insertfilterresult_t **result)
{
	Bool ok, zen;

	filter->result_buffer.used = 0;
	filter->result_buffer.consumed = 0;
	*result = &filter->result;

	switch (filter->state) {
	case TEXTEDITOR_INSERTFILTER_STATE_START:
		DP_STATE("end:STATE_START");
		break;
	case TEXTEDITOR_INSERTFILTER_STATE_RUNNING:
		DP_STATE("end:STATE_RUNNING");
		if (filter->first.w_ratio != filter->current.w_ratio) {
			zen = texteditor_insertfilter_w_ratio_isZenkaku(filter->first.w_ratio);
			if (zen == False) {
				texteditor_insertfilter_pushback_hankakufusen(filter);
			} else {
				texteditor_insertfilter_pushback_zenkakufusen(filter);
			}
		}
		ok = tadlangcodecmp(&filter->first.lang, &filter->current.lang);
		if (ok == False) {
			texteditor_insertfilter_pushback_firstlangcode(filter);
		}
		break;
	case TEXTEDITOR_INSERTFILTER_STATE_END:
		DP_STATE("end:STATE_END");
		break;
	}
}

EXPORT Bool texteditor_insertfilterresult_next(texteditor_insertfilterresult_t *result, tadsegment **segment)
{
	texteditor_insertfilter_t *filter = result->filter;

	if (filter->result_buffer.consumed >= filter->result_buffer.used) {
		return False;
	}

	*segment = filter->result_buffer.segs + filter->result_buffer.consumed;
	filter->result_buffer.consumed++;

	if (filter->result_buffer.consumed >= filter->result_buffer.used) {
		filter->result_buffer.used = 0;
		filter->result_buffer.consumed = 0;
	}

	return True;
}

EXPORT VOID texteditor_insertfilter_initialize(texteditor_insertfilter_t *filter, tadlangcode *lang, RATIO w_ratio)
{
	texteditor_stackfilter_initialize(&filter->stack);
	filter->state = TEXTEDITOR_INSERTFILTER_STATE_START;
	filter->first.lang = *lang;
	filter->first.w_ratio = w_ratio;
	filter->current.lang = (tadlangcode){0, 0x21};
	filter->current.w_ratio = 0x0101;
	filter->result_buffer.used = 0;
	filter->result_buffer.consumed = 0;
	filter->result.filter = filter;
}

EXPORT VOID texteditor_insertfilter_finalize(texteditor_insertfilter_t *filter)
{
	texteditor_stackfilter_finalize(&filter->stack);
}

/*
 * tadstack.c
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

#include "tadstack.h"

#include	<basic.h>
#include	<bstdio.h>
#include	<tad.h>

#include    "tadlangcode.h"
#include    "tadsegment.h"

#ifdef BCHAN_CONFIG_DEBUG
# define DP(arg) printf arg
# define DP_ER(msg, err) printf("%s (%d/%x)\n", msg, err>>16, err)
#else
# define DP(arg) /**/
# define DP_ER(msg, err) /**/
#endif

#if 0
# define DP_STATE(state) printf("%s\n", state)
#else
# define DP_STATE(state) /**/
#endif

EXPORT W tadstack_nestlevel(tadstack_t *stack)
{
	return stack->nestlevel;
}

EXPORT W tadstack_currentlangcode(tadstack_t *stack, tadlangcode *langcode)
{
	if (stack->nestlevel < 0) {
		return -1;
	}
	if (stack->data[stack->nestlevel].type != TADSTACK_DATATYPE_TEXT) {
		return -1;
	}
	*langcode = stack->data[stack->nestlevel].currentlang;
	return 0;
}

EXPORT W tadstack_currentdata(tadstack_t *stack, TADSTACK_DATATYPE *type)
{
	if (stack->nestlevel < 0) {
		return -1;
	}
	*type = stack->data[stack->nestlevel].type;
	return 0;
}

EXPORT W tadstack_currentview(tadstack_t *stack, RECT *r)
{
	if (stack->nestlevel < 0) {
		return -1;
	}
	*r = stack->data[stack->nestlevel].view;
	return 0;
}

EXPORT W tadstack_currentdraw(tadstack_t *stack, RECT *r)
{
	if (stack->nestlevel < 0) {
		return -1;
	}
	*r = stack->data[stack->nestlevel].draw;
	return 0;
}

EXPORT W tadstack_currenthunit(tadstack_t *stack, UNITS *units)
{
	if (stack->nestlevel < 0) {
		return -1;
	}
	*units = stack->data[stack->nestlevel].h_unit;
	return 0;
}

EXPORT W tadstack_currentvunit(tadstack_t *stack, UNITS *units)
{
	if (stack->nestlevel < 0) {
		return -1;
	}
	*units = stack->data[stack->nestlevel].v_unit;
	return 0;
}

LOCAL VOID tadstack_pushTEXTstack(tadstack_t *stack, RECT view, RECT draw, UNITS h_unit, UNITS v_unit, UH lang, UH bgpat)
{
	TC ch = lang | 0xFE00;

	stack->nestlevel++;

	stack->data[stack->nestlevel].type = TADSTACK_DATATYPE_TEXT;
	stack->data[stack->nestlevel].view = view;
	stack->data[stack->nestlevel].draw = draw;
	stack->data[stack->nestlevel].h_unit = h_unit;
	stack->data[stack->nestlevel].v_unit = v_unit;
	stack->data[stack->nestlevel].lang = lang;
	stack->data[stack->nestlevel].bgpat = bgpat;
	TCtotadlangcode(&ch, 1, &(stack->data[stack->nestlevel].currentlang));
}

LOCAL VOID tadstack_pushFIGstack(tadstack_t *stack, RECT view, RECT draw, UNITS h_unit, UNITS v_unit, W ratio)
{
	stack->nestlevel++;

	stack->data[stack->nestlevel].type = TADSTACK_DATATYPE_FIG;
	stack->data[stack->nestlevel].view = view;
	stack->data[stack->nestlevel].draw = draw;
	stack->data[stack->nestlevel].h_unit = h_unit;
	stack->data[stack->nestlevel].v_unit = v_unit;
	stack->data[stack->nestlevel].ratio = ratio;
}

LOCAL VOID tadstack_popstack(tadstack_t *stack)
{
	stack->nestlevel--;
}

EXPORT TADSTACK_RESULT tadstack_inputcharactor(tadstack_t *stack, TC ch)
{
	TADSTACK_RESULT ret = TADSTACK_RESULT_OK;
	TADLANGCODE_PARSER_RESULT parser_result;

	switch (stack->state) {
	case TADSTACK_STATE_START:
		DP_STATE("START");
		tadstack_pushTEXTstack(stack, (RECT){{0, 0, 0, 0}}, (RECT){{0, 0, 0, 0}}, 0, 0, 0x21, 0);
		stack->state = TADSTACK_STATE_TEXT;
		ret = TADSTACK_RESULT_PUSH_STACK;
		break;
	case TADSTACK_STATE_TEXT:
		DP_STATE("TEXT");
		if ((ch & 0xFE00) == 0xFE00) {
			tadlangcode_parser_initialize(&stack->langparser);
			parser_result = tadlangcode_parser_inputchar(&stack->langparser, ch);
			if (parser_result == TADLANGCODE_PARSER_RESULT_DETERMINED) {
				tadlangcode_parser_getlangcode(&stack->langparser, &(stack->data[stack->nestlevel].currentlang));
				tadlangcode_parser_finalize(&stack->langparser);
				ret = TADSTACK_RESULT_LANGUAGE_CHANGE;
			} else if (parser_result == TADLANGCODE_PARSER_RESULT_CONTINUE) {
				ret = TADSTACK_RESULT_LANGUAGE_CHANGE;
				stack->state = TADSTACK_STATE_TEXT_LANGCODE;
			} else {
				ret = TADSTACK_RESULT_FORMAT_ERROR;
			}
			break;
		}
		ret = TADSTACK_RESULT_OK;
		break;
	case TADSTACK_STATE_TEXT_LANGCODE:
		DP_STATE("TEXT_LANGCODE");
		parser_result = tadlangcode_parser_inputchar(&stack->langparser, ch);
		if (parser_result == TADLANGCODE_PARSER_RESULT_DETERMINED) {
			tadlangcode_parser_getlangcode(&stack->langparser, &(stack->data[stack->nestlevel].currentlang));
			tadlangcode_parser_finalize(&stack->langparser);
			stack->state = TADSTACK_STATE_TEXT;
			ret = TADSTACK_RESULT_OK;
		} else if (parser_result == TADLANGCODE_PARSER_RESULT_CONTINUE) {
			ret = TADSTACK_RESULT_OK;
		} else {
			ret = TADSTACK_RESULT_FORMAT_ERROR;
		}
		break;
	case TADSTACK_STATE_FIG:
		DP_STATE("FIG");
		return TADSTACK_RESULT_FORMAT_ERROR;
	}

	return ret;
}

LOCAL TADSTACK_RESULT tadstack_handle_textsegment(tadstack_t *stack, UB *bin, W len)
{
	TEXTSEG *textseg;

	if (len < 24) {
		return TADSTACK_RESULT_FORMAT_ERROR;
	}
	textseg = (TEXTSEG*)bin;
	tadstack_pushTEXTstack(stack, textseg->view, textseg->draw, textseg->h_unit, textseg->v_unit, textseg->lang, textseg->bgpat);
	stack->state = TADSTACK_STATE_TEXT;

	return TADSTACK_RESULT_PUSH_STACK;
}

LOCAL TADSTACK_RESULT tadstack_handle_figsegment(tadstack_t *stack, UB *bin, W len)
{
	FIGSEG *figseg;

	if (len < 24) {
		return TADSTACK_RESULT_FORMAT_ERROR;
	}
	figseg = (FIGSEG*)bin;
	tadstack_pushFIGstack(stack, figseg->view, figseg->draw, figseg->h_unit, figseg->v_unit, figseg->ratio);
	stack->state = TADSTACK_STATE_FIG;

	return TADSTACK_RESULT_PUSH_STACK;
}

EXPORT TADSTACK_RESULT tadstack_inputvsegment(tadstack_t *stack, UH segid, UB *bin, W len)
{
	TADSTACK_RESULT ret = TADSTACK_RESULT_OK;

	switch (stack->state) {
	case TADSTACK_STATE_START:
		DP_STATE("START");
		if (segid == TS_TEXT) {
			ret = tadstack_handle_textsegment(stack, bin, len);
		} else if (segid == TS_FIG) {
			ret = tadstack_handle_figsegment(stack, bin, len);
		} else if ((TS_TPAGE <= segid)&&(segid <= TS_TAPPL)) {
			tadstack_pushTEXTstack(stack, (RECT){{0, 0, 0, 0}}, (RECT){{0, 0, 0, 0}}, 0, 0, 0x21, 0);
			stack->state = TADSTACK_STATE_TEXT;
			ret = TADSTACK_RESULT_PUSH_STACK;
		} else if ((TS_FPRIM <= segid)&&(segid <= TS_FAPPL)) {
			tadstack_pushFIGstack(stack, (RECT){{0, 0, 0, 0}}, (RECT){{0, 0, 0, 0}}, 0, 0, 0);
			stack->state = TADSTACK_STATE_FIG;
			ret = TADSTACK_RESULT_PUSH_STACK;
		} else {
			ret = TADSTACK_RESULT_OK;
		}
		break;
	case TADSTACK_STATE_TEXT:
		DP_STATE("TEXT");
		if ((TS_FPRIM <= segid)&&(segid <= TS_FAPPL)) {
			ret = TADSTACK_RESULT_FORMAT_ERROR;
			break;
		}
		if (segid == TS_TEXT) {
			ret = tadstack_handle_textsegment(stack, bin, len);
			break;
		}
		if (segid == TS_FIG) {
			ret = tadstack_handle_figsegment(stack, bin, len);
			break;
		}
		if (segid == TS_FIGEND) {
			ret = TADSTACK_RESULT_FORMAT_ERROR;
			break;
		}
		if (segid == TS_TEXTEND) {
			tadstack_popstack(stack);
			ret = TADSTACK_RESULT_POP_STACK;
			break;
		}
		ret = TADSTACK_RESULT_OK;
		break;
	case TADSTACK_STATE_TEXT_LANGCODE:
		DP_STATE("TEXT_LANGCODE");
		ret = TADSTACK_RESULT_FORMAT_ERROR;
		break;
	case TADSTACK_STATE_FIG:
		DP_STATE("FIG");
		if ((TS_TPAGE <= segid)&&(segid <= TS_TAPPL)) {
			ret = TADSTACK_RESULT_FORMAT_ERROR;
			break;
		}
		if (segid == TS_TEXT) {
			ret = tadstack_handle_textsegment(stack, bin, len);
			break;
		}
		if (segid == TS_FIG) {
			ret = tadstack_handle_figsegment(stack, bin, len);
			break;
		}
		if (segid == TS_TEXTEND) {
			ret = TADSTACK_RESULT_FORMAT_ERROR;
			break;
		}
		if (segid == TS_FIGEND) {
			tadstack_popstack(stack);
			ret = TADSTACK_RESULT_POP_STACK;
			break;
		}
		ret = TADSTACK_RESULT_OK;
		break;
	}

	return ret;
}

EXPORT TADSTACK_RESULT tadstack_inputlangcode(tadstack_t *stack, tadlangcode *langcode)
{
	TADSTACK_RESULT ret = TADSTACK_RESULT_OK;

	switch (stack->state) {
	case TADSTACK_STATE_START:
		DP_STATE("START");
		tadstack_pushTEXTstack(stack, (RECT){{0, 0, 0, 0}}, (RECT){{0, 0, 0, 0}}, 0, 0, 0x21, 0);
		stack->state = TADSTACK_STATE_TEXT;
		stack->data[stack->nestlevel].currentlang = *langcode;
		ret = TADSTACK_RESULT_PUSH_STACK;
		break;
	case TADSTACK_STATE_TEXT:
		DP_STATE("TEXT");
		stack->data[stack->nestlevel].currentlang = *langcode;
		ret = TADSTACK_RESULT_LANGUAGE_CHANGE;
		break;
	case TADSTACK_STATE_TEXT_LANGCODE:
		DP_STATE("TEXT_LANGCODE");
		ret = TADSTACK_RESULT_FORMAT_ERROR;
		break;
	case TADSTACK_STATE_FIG:
		DP_STATE("FIG");
		ret = TADSTACK_RESULT_FORMAT_ERROR;
		break;
	}

	return ret;
}

EXPORT TADSTACK_RESULT tadstack_inputsegment(tadstack_t *stack, tadsegment *segment)
{
	UB segid;
	UB *segdata;
	W seglen;

	switch (segment->type) {
	case TADSEGMENT_TYPE_VARIABLE:
		tadsegment_getvariable(segment, &segid, &seglen, &segdata);
		return tadstack_inputvsegment(stack, segid, segdata, seglen);
	case TADSEGMENT_TYPE_CHARACTOR:
		return tadstack_inputcharactor(stack, segment->value.ch);
	case TADSEGMENT_TYPE_LANGCODE:
		return tadstack_inputlangcode(stack, &segment->value.lang);
	}
	return TADSTACK_RESULT_FORMAT_ERROR;
}

EXPORT VOID tadstack_initialize(tadstack_t *stack)
{
	stack->state = TADSTACK_STATE_START;
	stack->nestlevel = -1;
}

EXPORT VOID tadstack_finalize(tadstack_t *stack)
{
}

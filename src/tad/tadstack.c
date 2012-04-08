/*
 * tadstack.c
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

#include "tadstack.h"

#include	<basic.h>
#include	<bstdio.h>
#include	<tad.h>

EXPORT W tadstack_nestlevel(tadstack_t *stack)
{
	return stack->nestlevel;
}

EXPORT VOID tadstack_currentlang(tadstack_t *stack, TC **lang, W *len)
{
	if (stack->data[stack->nestlevel].type != TADSTACK_DATATYPE_TEXT) {
		*lang = NULL;
		*len = 0;
		return;
	}
	*lang = stack->data[stack->nestlevel].currentlang;
	*len = 1;
}

EXPORT TADSTACK_DATATYPE tadstack_currentdata(tadstack_t *stack)
{
	return stack->data[stack->nestlevel].type;
}

EXPORT RECT tadstack_currentview(tadstack_t *stack)
{
	return stack->data[stack->nestlevel].view;
}

EXPORT RECT tadstack_currentdraw(tadstack_t *stack)
{
	return stack->data[stack->nestlevel].draw;
}

EXPORT UNITS tadstack_currenthunit(tadstack_t *stack)
{
	return stack->data[stack->nestlevel].h_unit;
}

EXPORT UNITS tadstack_currentvunit(tadstack_t *stack)
{
	return stack->data[stack->nestlevel].v_unit;
}

LOCAL VOID tadstack_pushTEXTstack(tadstack_t *stack, RECT view, RECT draw, UNITS h_unit, UNITS v_unit, UH lang, UH bgpat)
{
	stack->nestlevel++;

	stack->data[stack->nestlevel].type = TADSTACK_DATATYPE_TEXT;
	stack->data[stack->nestlevel].view = view;
	stack->data[stack->nestlevel].draw = draw;
	stack->data[stack->nestlevel].h_unit = h_unit;
	stack->data[stack->nestlevel].v_unit = v_unit;
	stack->data[stack->nestlevel].lang = lang;
	stack->data[stack->nestlevel].bgpat = bgpat;
	stack->data[stack->nestlevel].currentlang[0] = lang;
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

	switch (stack->state) {
	case TADSTACK_STATE_START:
		tadstack_pushTEXTstack(stack, (RECT){{0, 0, 0, 0}}, (RECT){{0, 0, 0, 0}}, 0, 0, 0x21, 0);
		stack->state = TADSTACK_STATE_TEXT;
		ret = TADSTACK_RESULT_PUSH_STACK;
		break;
	case TADSTACK_STATE_TEXT:
		if ((ch & 0xFE00) == 0xFE00) {
			stack->data[stack->nestlevel].currentlang[0] = ch;
			ret = TADSTACK_RESULT_LANGUAGE_CHANGE;
			if (ch == 0xFEFE) {
				stack->state = TADSTACK_STATE_TEXT_LANGCODE;
			}
			break;
		}
		ret = TADSTACK_RESULT_OK;
		break;
	case TADSTACK_STATE_TEXT_LANGCODE:
		if ((ch & 0xFE00) != 0xFE00) {
			ret = TADSTACK_RESULT_FORMAT_ERROR;
		}
		if (ch != 0xFEFE) {
			stack->state = TADSTACK_STATE_TEXT;
		}
		ret = TADSTACK_RESULT_OK;
		break;
	case TADSTACK_STATE_FIG:
		return TADSTACK_RESULT_FORMAT_ERROR;
	}

	return ret;
}

EXPORT TADSTACK_RESULT tadstack_inputvsegment(tadstack_t *stack, UH segid, UB *bin, W len)
{
	TADSTACK_RESULT ret = TADSTACK_RESULT_OK;
	TEXTSEG *textseg;
	FIGSEG *figseg;

	switch (stack->state) {
	  case TADSTACK_STATE_START:
		if (segid == TS_TEXT) {
			if (len < 24) {
				ret = TADSTACK_RESULT_FORMAT_ERROR;
				break;
			}
			textseg = (TEXTSEG*)bin;
			tadstack_pushTEXTstack(stack, textseg->view, textseg->draw, textseg->h_unit, textseg->v_unit, textseg->lang, textseg->bgpat);
			ret = TADSTACK_RESULT_PUSH_STACK;
		} else if (segid == TS_FIG) {
			if (len < 24) {
				ret = TADSTACK_RESULT_FORMAT_ERROR;
				break;
			}
			figseg = (FIGSEG*)bin;
			tadstack_pushFIGstack(stack, figseg->view, figseg->draw, figseg->h_unit, figseg->v_unit, figseg->ratio);
			ret = TADSTACK_RESULT_PUSH_STACK;
		} else if ((TS_FPRIM <= segid)&&(segid <= TS_FAPPL)) {
			tadstack_pushFIGstack(stack, (RECT){{0, 0, 0, 0}}, (RECT){{0, 0, 0, 0}}, 0, 0, 0);
			ret = TADSTACK_RESULT_PUSH_STACK;
		} else {
			ret = TADSTACK_RESULT_OK;
		}
		break;
	  case TADSTACK_STATE_TEXT:
		if ((TS_FPRIM <= segid)&&(segid <= TS_FAPPL)) {
			ret = TADSTACK_RESULT_FORMAT_ERROR;
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
		ret = TADSTACK_RESULT_FORMAT_ERROR;
		break;
	  case TADSTACK_STATE_FIG:
		if ((0xA0 <= segid)&&(segid <= 0xAF)) {
			ret = TADSTACK_RESULT_FORMAT_ERROR;
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

EXPORT VOID tadstack_initialize(tadstack_t *stack)
{
	stack->state = TADSTACK_STATE_START;
	stack->nestlevel = -1;
}

EXPORT VOID tadstack_finalize(tadstack_t *stack)
{
}

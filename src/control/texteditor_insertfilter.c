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

#include	<tad/taditerator.h>
#include	<tad/tadstack.h>

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

LOCAL Bool texteditor_insertfilter_stackfilter(texteditor_insertfilter_t *filter, taditerator_result *result)
{
	W nestlevel, err;
	TADSTACK_DATATYPE type;

	for (;;) {
		taditerator_next2(&filter->iterator, result);
		if (result->type == TADITERATOR_RESULTTYPE_END) {
			break;
		}

		if (result->type == TADITERATOR_RESULTTYPE_CHARCTOR) {
			tadstack_inputcharactor(&filter->tadstack, result->segment);
		} else if (result->type == TADITERATOR_RESULTTYPE_SEGMENT) {
			tadstack_inputvsegment(&filter->tadstack, result->segment, result->data, result->segsize);
		}

		nestlevel = tadstack_nestlevel(&filter->tadstack);
		if (nestlevel != 0) {
			continue;
		}
		err = tadstack_currentdata(&filter->tadstack, &type);
		if (type != TADSTACK_DATATYPE_TEXT) {
			continue;
		}

		return True;
	}

	return False;
}

struct texteditor_insertfilter_innerresult_ {
	enum {
		INNERRESULT_VARIABLE,
		INNERRESULT_STRING,
		INNERRESULT_LANGCODE,
	} type;
	UB *data;
	W len;
	struct {
		UB segid;
		UW len;
		UB *data;
	} variable;
};
typedef struct texteditor_insertfilter_innerresult_ texteditor_insertfilter_innerresult;

LOCAL Bool texteditor_insertfilter_langcode(texteditor_insertfilter_t *filter, texteditor_insertfilter_innerresult *result)
{
	taditerator_result iter_result;
	Bool reading_lang = False, cont;
	TC *lang_start = NULL;

	for (;;) {
		cont = texteditor_insertfilter_stackfilter(filter, &iter_result);
		if (cont == False) {
			break;
		}

		if (reading_lang != False) {
			if (iter_result.type != TADITERATOR_RESULTTYPE_CHARCTOR) {
				/* format error */
				break;
			}
			if (lang_start == NULL) {
				break;
			}
			if (iter_result.segment == 0xFEFE) {
				continue;
			}
			result->type = INNERRESULT_LANGCODE;
			result->data = (UB*)lang_start;
			result->len = (iter_result.pos - lang_start + 1) * sizeof(TC);
			TCtotadlangcode((TC*)result->data, result->len / sizeof(TC), &filter->current.lang);
			reading_lang = False;
			lang_start = NULL;
		}

		if (iter_result.type == TADITERATOR_RESULTTYPE_CHARCTOR) {
			if ((iter_result.segment & 0xFF00) == 0xFE00) {
				if (iter_result.segment == 0xFEFE) {
					reading_lang = True;
					lang_start = iter_result.pos;
				} else {
					result->type = INNERRESULT_LANGCODE;
					result->data = (UB*)iter_result.pos;
					result->len = sizeof(TC);
					TCtotadlangcode((TC*)result->data, result->len / sizeof(TC), &filter->current.lang);
					return True;
				}
			} else {
				result->type = INNERRESULT_STRING;
				result->data = (UB*)iter_result.pos;
				result->len = sizeof(TC);
				return True;
			}
		} else if (iter_result.type == TADITERATOR_RESULTTYPE_SEGMENT) {
			result->type = INNERRESULT_VARIABLE;
			result->data = (UB*)iter_result.pos;
			if (iter_result.vseg.l->len == 0xFFFF) {
				result->len = 2 + 2 + 4 + iter_result.vseg.l->llen;
			} else {
				result->len = 2 + 2 + iter_result.vseg.l->len;
			}
			result->variable.segid = iter_result.segment & 0xFF;
			result->variable.len = iter_result.segsize;
			result->variable.data = iter_result.data;
			return True;
		}
	}

	return False;
}

LOCAL Bool texteditor_insertfilter_chratio(texteditor_insertfilter_t *filter, texteditor_insertfilter_innerresult *result)
{
	Bool cont;
	UB subid;

	for (;;) {
		cont = texteditor_insertfilter_langcode(filter, result);
		if (cont == False) {
			break;
		}

		if (result->type != INNERRESULT_VARIABLE) {
			return True;
		}

		if (result->variable.segid != TS_TFONT) {
			continue;
		}
		if (result->variable.len < 6) {
			continue;
		}
		subid = *(UH*)result->variable.data >> 8;
		if (subid != 3) {
			continue;
		}

		filter->current.w_ratio = *((UH*)result->variable.data + 5);

		return True;
	}

	return False;
}

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

LOCAL TC texteditor_insertfilter_systemlangcode[] = { 0xFE21 };
#define TEXTEDITOR_INSERTFILTER_SYSTEMLANGCODE_LENGTH 1

LOCAL TC textediter_insertfilter_zenkakufusen[] = { 0xFFA2, 0x0006, 0x0300, 0x0101, 0x0101 };
LOCAL TC textediter_insertfilter_hankakufusen[] = { 0xFFA2, 0x0006, 0x0300, 0x0101, 0x0102 };
#define TEXTEDITOR_INSERTFILTER_FUSEN_LENGTH 5

LOCAL VOID texteditor_insertfilter_setresult_systemlangcode(texteditor_insertfilter_result *result)
{
	result->data = (UB*)texteditor_insertfilter_systemlangcode;
	result->len = TEXTEDITOR_INSERTFILTER_SYSTEMLANGCODE_LENGTH * sizeof(TC);
}

LOCAL VOID texteditor_insertfilter_setresult_firstlangcode(texteditor_insertfilter_t *filter, texteditor_insertfilter_result *result)
{
	result->len = tadlangcodetoTC(&filter->first.lang, filter->buffer, TEXTEDITOR_INSERTFILTER_TEXTBUFFER_SIZE) * sizeof(TC);
	result->data = (UB*)filter->buffer;
}

LOCAL VOID texteditor_insertfilter_setresult_zenkakufusen(texteditor_insertfilter_result *result)
{
	result->data = (UB*)textediter_insertfilter_zenkakufusen;
	result->len = TEXTEDITOR_INSERTFILTER_FUSEN_LENGTH * sizeof(TC);
}

LOCAL VOID texteditor_insertfilter_setresult_hankakufusen(texteditor_insertfilter_result *result)
{
	result->data = (UB*)textediter_insertfilter_hankakufusen;
	result->len = TEXTEDITOR_INSERTFILTER_FUSEN_LENGTH * sizeof(TC);
}

/* */

LOCAL Bool texteditor_insertfilter_next_terminate(texteditor_insertfilter_t *filter, texteditor_insertfilter_result *result)
{
	Bool ok, zen;

	if (filter->first.w_ratio != filter->current.w_ratio) {
		filter->state = TEXTEDITOR_INSTERTFILTER_STATE_LAST_LANGCODE;
		zen = texteditor_insertfilter_w_ratio_isZenkaku(filter->first.w_ratio);
		if (zen == False) {
			texteditor_insertfilter_setresult_hankakufusen(result);
		} else {
			texteditor_insertfilter_setresult_zenkakufusen(result);
		}
		return True;
	}
	ok = tadlangcodecmp(&filter->first.lang, &filter->current.lang);
	if (ok == False) {
		filter->state = TEXTEDITOR_INSTERTFILTER_STATE_LAST;
		texteditor_insertfilter_setresult_firstlangcode(filter, result);
		return True;
	}

	return False;
}

LOCAL Bool texteditor_insertfilter_next_state_start(texteditor_insertfilter_t *filter, texteditor_insertfilter_result *result)
{
	texteditor_insertfilter_innerresult inner_result;
	Bool cont, ok;

	ok = texteditor_insertfilter_w_ratio_isZenkaku(filter->first.w_ratio);
	if (ok == False) {
		filter->state = TEXTEDITOR_INSTERTFILTER_STATE_FIRST_LANGCODE;
		texteditor_insertfilter_setresult_zenkakufusen(result);
		return True;
	}
	ok = texteditor_insertfilter_w_ratio_isSystemScript(&filter->first.lang);
	if (ok == False) {
		filter->state = TEXTEDITOR_INSTERTFILTER_STATE_RUNNING;
		texteditor_insertfilter_setresult_systemlangcode(result);
		return True;
	}

	cont = texteditor_insertfilter_chratio(filter, &inner_result);
	if (cont == False) {
		return texteditor_insertfilter_next_terminate(filter, result);
	}

	filter->state = TEXTEDITOR_INSTERTFILTER_STATE_RUNNING;
	result->data = inner_result.data;
	result->len = inner_result.len;

	return True;
}

LOCAL Bool texteditor_insertfilter_next_state_langcode(texteditor_insertfilter_t *filter, texteditor_insertfilter_result *result)
{
	texteditor_insertfilter_innerresult inner_result;
	Bool cont, ok;

	ok = texteditor_insertfilter_w_ratio_isSystemScript(&filter->first.lang);
	if (ok == False) {
		filter->state = TEXTEDITOR_INSTERTFILTER_STATE_RUNNING;
		texteditor_insertfilter_setresult_systemlangcode(result);
		return True;
	}

	cont = texteditor_insertfilter_chratio(filter, &inner_result);
	if (cont == False) {
		return texteditor_insertfilter_next_terminate(filter, result);
	}

	filter->state = TEXTEDITOR_INSTERTFILTER_STATE_RUNNING;
	result->data = inner_result.data;
	result->len = inner_result.len;

	return True;
}

LOCAL Bool texteditor_insertfilter_next_state_running(texteditor_insertfilter_t *filter, texteditor_insertfilter_result *result)
{
	texteditor_insertfilter_innerresult inner_result;
	Bool cont;

	cont = texteditor_insertfilter_chratio(filter, &inner_result);
	if (cont == False) {
		return texteditor_insertfilter_next_terminate(filter, result);
	}

	filter->state = TEXTEDITOR_INSTERTFILTER_STATE_RUNNING;
	result->data = inner_result.data;
	result->len = inner_result.len;

	return True;
}

LOCAL Bool texteditor_insertfilter_next_state_last_langcode(texteditor_insertfilter_t *filter, texteditor_insertfilter_result *result)
{
	Bool ok;

	ok = tadlangcodecmp(&filter->first.lang, &filter->current.lang);
	if (ok == False) {
		filter->state = TEXTEDITOR_INSTERTFILTER_STATE_LAST;
		texteditor_insertfilter_setresult_firstlangcode(filter, result);
		return True;
	}

	return False;
}

LOCAL Bool texteditor_insertfilter_next_state_last(texteditor_insertfilter_t *filter, texteditor_insertfilter_result *result)
{
	return False;
}

EXPORT Bool texteditor_insertfilter_next(texteditor_insertfilter_t *filter, texteditor_insertfilter_result *result)
{
	Bool cont = False;

	switch (filter->state) {
	case TEXTEDITOR_INSTERTFILTER_STATE_START:
		DP_STATE("STATE_START");
		cont = texteditor_insertfilter_next_state_start(filter, result);
		break;
	case TEXTEDITOR_INSTERTFILTER_STATE_FIRST_LANGCODE:
		DP_STATE("STATE_FIRST_LANGCODE");
		cont = texteditor_insertfilter_next_state_langcode(filter, result);
		break;
	case TEXTEDITOR_INSTERTFILTER_STATE_RUNNING:
		DP_STATE("STATE_RUNNING");
		cont = texteditor_insertfilter_next_state_running(filter, result);
		break;
	case TEXTEDITOR_INSTERTFILTER_STATE_LAST_LANGCODE:
		DP_STATE("STATE_LAST_LANGCODE");
		cont = texteditor_insertfilter_next_state_last_langcode(filter, result);
		break;
	case TEXTEDITOR_INSTERTFILTER_STATE_LAST:
		DP_STATE("STATE_LAST");
		cont = texteditor_insertfilter_next_state_last(filter, result);
		break;
	}

	return cont;
}

EXPORT VOID texteditor_insertfilter_initialize(texteditor_insertfilter_t *filter, tadlangcode *lang, RATIO w_ratio, UB *data, W len)
{
	filter->state = TEXTEDITOR_INSTERTFILTER_STATE_START;
	filter->first.lang = *lang;
	filter->first.w_ratio = w_ratio;
	filter->current.lang = (tadlangcode){0, 0x21};
	filter->current.w_ratio = 0x0101;
	tadstack_initialize(&filter->tadstack);
	taditerator_initialize(&filter->iterator, (TC*)data, len / sizeof(TC));
}

EXPORT VOID texteditor_insertfilter_finalize(texteditor_insertfilter_t *filter)
{
	taditerator_finalize(&filter->iterator);
	tadstack_finalize(&filter->tadstack);
}

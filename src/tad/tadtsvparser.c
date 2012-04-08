/*
 * tadtsvparser.c
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

#include "tadtsvparser.h"

#include	<basic.h>
#include	<bstdio.h>
#include	<tad.h>
#include	<tcode.h>

#include    "tadstack.h"

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

EXPORT TADTSVPARSER_RESULT tadtsvparser_inputcharactor(tadtsvparser_t *parser, TC ch)
{
	TADTSVPARSER_RESULT ret = TADTSVPARSER_RESULT_IGNORE_SEGMENT;
	TADSTACK_RESULT stk_result;
	TADSTACK_DATATYPE type;
	TC *lang;
	W lang_len;

	stk_result = tadstack_inputcharactor(&parser->tadstack, ch);
	if (stk_result == TADSTACK_RESULT_FORMAT_ERROR) {
		return TADTSVPARSER_RESULT_FORMAT_ERROR;
	}
	//printf("inputchar = %04x\n", ch);

	switch (parser->state) {
	case TADTSVPARSER_STATE_START:
		DP_STATE("START inputcharactor");
		if (stk_result == TADSTACK_RESULT_PUSH_STACK) {
			type = tadstack_currentdata(&parser->tadstack);
			if (type == TADSTACK_DATATYPE_TEXT) {
				parser->state = TADTSVPARSER_STATE_READING_VALUE;
			} else {
				ret = TADTSVPARSER_RESULT_FORMAT_ERROR;
			}
		}
		break;
	case TADTSVPARSER_STATE_RECORD_START:
		DP_STATE("RECORD_START inputcharactor");
		parser->state = TADTSVPARSER_STATE_READING_VALUE;
		/* intentional */
	case TADTSVPARSER_STATE_READING_VALUE:
		DP_STATE("READING_VALUE inputcharactor");
		if (stk_result == TADSTACK_RESULT_PUSH_STACK) {
			parser->state = TADTSVPARSER_STATE_READING_VALUE_NESTED_SEGMENT;
			ret = TADTSVPARSER_RESULT_FIELD;
			break;
		}
		tadstack_currentlang(&parser->tadstack, &lang, &lang_len);
		//printf("lang %04x, %d, ch = %04x\n", lang[0], lang_len, ch);
		if ((lang_len == 1)&&((lang[0] & 0xFF) == 0x21)) {
			if (ch == TK_TAB) {
				ret = TADTSVPARSER_RESULT_FIELD_END;
			} else if (ch == TK_NL) {
				parser->state = TADTSVPARSER_STATE_RECORD_START;
				ret = TADTSVPARSER_RESULT_RECORD_END;
			} else {
				ret = TADTSVPARSER_RESULT_FIELD;
			}
			break;
		}
		ret = TADTSVPARSER_RESULT_FIELD;
		break;
	case TADTSVPARSER_STATE_READING_VALUE_NESTED_SEGMENT:
		DP_STATE("READING_VALUE_NESTED_SEGMENT inputcharactor");
		ret = TADTSVPARSER_RESULT_FIELD;
		break;
	}

	return ret;
}

EXPORT TADTSVPARSER_RESULT tadtsvparser_inputvsegment(tadtsvparser_t *parser, UH segid, UB *bin, W len)
{
	TADTSVPARSER_RESULT ret = TADTSVPARSER_RESULT_IGNORE_SEGMENT;
	TADSTACK_RESULT stk_result;
	TADSTACK_DATATYPE type;
	W nest;

	stk_result = tadstack_inputvsegment(&parser->tadstack, segid, bin, len);
	if (stk_result == TADSTACK_RESULT_FORMAT_ERROR) {
		return TADTSVPARSER_RESULT_FORMAT_ERROR;
	}

	switch (parser->state) {
	case TADTSVPARSER_STATE_START:
		DP_STATE("START inputvsegment");
		if (stk_result == TADSTACK_RESULT_PUSH_STACK) {
			type = tadstack_currentdata(&parser->tadstack);
			if (type == TADSTACK_DATATYPE_TEXT) {
				parser->state = TADTSVPARSER_STATE_READING_VALUE;
			} else {
				ret = TADTSVPARSER_RESULT_FORMAT_ERROR;
			}
		}
		break;
	case TADTSVPARSER_STATE_RECORD_START:
		DP_STATE("RECORD_START inputvsegment");
		if (stk_result == TADSTACK_RESULT_PUSH_STACK) {
			parser->state = TADTSVPARSER_STATE_READING_VALUE;
		}
		/* intentional */
	case TADTSVPARSER_STATE_READING_VALUE:
		DP_STATE("READING_VALUE inputvsegment");
		if (stk_result == TADSTACK_RESULT_POP_STACK) {
			ret = TADTSVPARSER_RESULT_IGNORE_SEGMENT;
			break;
		}
		if (stk_result == TADSTACK_RESULT_PUSH_STACK) {
			parser->state = TADTSVPARSER_STATE_READING_VALUE_NESTED_SEGMENT;
		}
		ret = TADTSVPARSER_RESULT_FIELD;
		break;
	case TADTSVPARSER_STATE_READING_VALUE_NESTED_SEGMENT:
		DP_STATE("READING_VALUE_NESTED_SEGMENT inputvsegment");
		if (stk_result == TADSTACK_RESULT_POP_STACK) {
			nest = tadstack_nestlevel(&parser->tadstack);
			if (nest == 0) {
				parser->state = TADTSVPARSER_STATE_READING_VALUE;
			}
		}
		ret = TADTSVPARSER_RESULT_FIELD;
		break;
	}

	return ret;
}

EXPORT TADTSVPARSER_RESULT tadtsvparser_inputendofdata(tadtsvparser_t *parser)
{
	switch (parser->state) {
	case TADTSVPARSER_STATE_RECORD_START:
		return TADTSVPARSER_RESULT_IGNORE_SEGMENT;
	case TADTSVPARSER_STATE_START:
	case TADTSVPARSER_STATE_READING_VALUE:
	case TADTSVPARSER_STATE_READING_VALUE_NESTED_SEGMENT:
		return TADTSVPARSER_RESULT_RECORD_END;
	}
}

EXPORT VOID tadtsvparser_initialize(tadtsvparser_t *parser)
{
	parser->state = TADTSVPARSER_STATE_START;
	tadstack_initialize(&parser->tadstack);
}

EXPORT VOID tadtsvparser_finalize(tadtsvparser_t *parser)
{
	tadstack_finalize(&parser->tadstack);
}

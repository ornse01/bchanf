/*
 * charreferparser.c
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

#include "charreferparser.h"

#include	<basic.h>
#include	<bstdlib.h>
#include	<bstdio.h>
#include	<bstring.h>
#include	<bctype.h>

#include    "tokenchecker.h"

#ifdef BCHAN_CONFIG_DEBUG
# define DP(arg) printf arg
# define DP_ER(msg, err) printf("%s (%d/%x)\n", msg, err>>16, err)
#else
# define DP(arg) /**/
# define DP_ER(msg, err) /**/
#endif

LOCAL tokenchecker_valuetuple_t nList_nameref[] = {
	{"amp", '&'},
	{"gt", '>'},
	{"lt", '<'},
	{"quot", '"'},
};
LOCAL B eToken_nameref[] = ";";

LOCAL W charreferparser_digitchartointeger(UB ch)
{
	return ch - '0';
}

LOCAL W charreferparser_hexchartointeger(UB ch)
{
	if(('a' <= ch)&&(ch <= 'h')){
		return ch - 'a' + 10;
	}
	if(('A' <= ch)&&(ch <= 'H')){
		return ch - 'A' + 10;
	}
	return charreferparser_digitchartointeger(ch);
}

EXPORT CHARREFERPARSER_RESULT charreferparser_parsechar(charreferparser_t *parser, UB ch)
{
	W ret, val;

	switch (parser->state) {
	case START:
		if (ch != '&') {
			return CHARREFERPARSER_RESULT_INVALID;
		}
		parser->state = RECIEVE_AMP;
		return CHARREFERPARSER_RESULT_CONTINUE;
	case RECIEVE_AMP:
		if (ch == '#') {
			parser->state = RECIEVE_NUMBER;
			return CHARREFERPARSER_RESULT_CONTINUE;
		}
		if (ch == ';') {
			return CHARREFERPARSER_RESULT_INVALID;
		}
		parser->state = NAMED;
		parser->charnumber = -1;
		ret = tokenchecker_inputchar(&parser->named, ch, &val);
		if (ret != TOKENCHECKER_RESULT_CONTINUE) {
			return CHARREFERPARSER_RESULT_INVALID;
		}
		return CHARREFERPARSER_RESULT_CONTINUE;
	case RECIEVE_NUMBER:
		if ((ch == 'x')||(ch == 'X')) {
			parser->state = NUMERIC_HEXADECIMAL;
			return CHARREFERPARSER_RESULT_CONTINUE;
		}
		if (isdigit(ch)) {
			parser->state = NUMERIC_DECIMAL;
			parser->charnumber = charreferparser_digitchartointeger(ch);
			return CHARREFERPARSER_RESULT_CONTINUE;
		}
		return CHARREFERPARSER_RESULT_INVALID;
	case NUMERIC_DECIMAL:
		if (isdigit(ch)) {
			parser->charnumber *= 10;
			parser->charnumber += charreferparser_digitchartointeger(ch);
			return CHARREFERPARSER_RESULT_CONTINUE;
		}
		if (ch == ';') {
			parser->state = DETERMINED;
			return CHARREFERPARSER_RESULT_DETERMINE;
		}
		return CHARREFERPARSER_RESULT_INVALID;
	case NUMERIC_HEXADECIMAL:
		if (isxdigit(ch)) {
			parser->charnumber *= 16;
			parser->charnumber += charreferparser_hexchartointeger(ch);
			return CHARREFERPARSER_RESULT_CONTINUE;
		}
		if (ch == ';') {
			parser->state = DETERMINED;
			return CHARREFERPARSER_RESULT_DETERMINE;
		}
		return CHARREFERPARSER_RESULT_INVALID;
	case NAMED:
		ret = tokenchecker_inputchar(&parser->named, ch, &val);
		if (ret == TOKENCHECKER_RESULT_DETERMINE) {
			parser->charnumber = val;
			parser->state = DETERMINED;
			return CHARREFERPARSER_RESULT_DETERMINE;
		} else if (ret != TOKENCHECKER_RESULT_CONTINUE) {
			return CHARREFERPARSER_RESULT_INVALID;
		}
		return CHARREFERPARSER_RESULT_CONTINUE;
	case INVALID:
		return CHARREFERPARSER_RESULT_INVALID;
	case DETERMINED:
		return CHARREFERPARSER_RESULT_INVALID;
	}

	return CHARREFERPARSER_RESULT_INVALID;
}

EXPORT W charreferparser_getcharnumber(charreferparser_t *parser)
{
	if (parser->state != DETERMINED) {
		return -1;
	}
	return parser->charnumber;
}

EXPORT VOID charreferparser_getlastmatchedstring(charreferparser_t *parser, UB **str, W *len)
{
	tokenchecker_getlastmatchedstring(&parser->named, str, len);
}

EXPORT VOID charreferparser_resetstate(charreferparser_t *parser)
{
	parser->state = START;
	parser->charnumber = 0;
	tokenchecker_clear(&(parser->named));
}

EXPORT W charreferparser_initialize(charreferparser_t *parser)
{
	parser->state = START;
	parser->charnumber = 0;
	tokenchecker_initialize(&(parser->named), nList_nameref, 4, eToken_nameref);
	return 0;
}

EXPORT VOID charreferparser_finalize(charreferparser_t *parser)
{
}

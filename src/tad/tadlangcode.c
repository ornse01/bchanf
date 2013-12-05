/*
 * tadlangcode.c
 *
 * Copyright (c) 2013 project bchan
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

#include	"tadlangcode.h"

#include	<bstdio.h>

#ifdef BCHAN_CONFIG_DEBUG
# define DP(arg) printf arg
# define DP_ER(msg, err) printf("%s (%d/%x)\n", msg, err>>16, err)
#else
# define DP(arg) /**/
# define DP_ER(msg, err) /**/
#endif

EXPORT TADLANGCODE_PARSER_RESULT tadlangcode_parser_inputchar(tadlangcode_parser_t *parser, TC ch)
{
	switch (parser->state) {
	case TADLANGCODE_PARSER_STATE_START:
		if ((ch & 0xFF00) != 0xFE00) {
			return TADLANGCODE_PARSER_RESULT_INVALID;
		}
		if (ch == 0xFEFE) {
			parser->state = TADLANGCODE_PARSER_STATE_LANGCODE;
			parser->iter = 1;
			return TADLANGCODE_PARSER_RESULT_CONTINUE;
		}
		parser->state = TADLANGCODE_PARSER_STATE_DETERMINED;
		parser->iter = 0;
		parser->code = ch & 0xFF;
		return TADLANGCODE_PARSER_RESULT_DETERMINED;
	case TADLANGCODE_PARSER_STATE_LANGCODE:
		if (ch == 0xFEFE) {
			parser->iter += 2;
			return TADLANGCODE_PARSER_RESULT_CONTINUE;
		}
		if ((ch & 0xFF00) == 0x0000) {
			parser->state = TADLANGCODE_PARSER_STATE_DETERMINED;
			parser->code = ch & 0xFF;
			return TADLANGCODE_PARSER_RESULT_DETERMINED;
		}
		if ((ch & 0xFF00) == 0xFE00) {
			parser->state = TADLANGCODE_PARSER_STATE_DETERMINED;
			parser->iter += 1;
			parser->code = ch & 0xFF;
			return TADLANGCODE_PARSER_RESULT_DETERMINED;
		}
		return TADLANGCODE_PARSER_RESULT_INVALID;
	case TADLANGCODE_PARSER_STATE_DETERMINED:
		return TADLANGCODE_PARSER_RESULT_INVALID;
	}
	return TADLANGCODE_PARSER_RESULT_INVALID;
}

EXPORT W tadlangcode_parser_getlangcode(tadlangcode_parser_t *parser, tadlangcode *langcode)
{
	if (parser->state != TADLANGCODE_PARSER_STATE_DETERMINED) {
		return -1; /* TODO */
	}
	langcode->iter = parser->iter;
	langcode->code = parser->code;
	return 0;
}

EXPORT VOID tadlangcode_parser_initialize(tadlangcode_parser_t *parser)
{
	parser->state = TADLANGCODE_PARSER_STATE_START;
	parser->iter = 0;
	parser->code = 0;
}

EXPORT VOID tadlangcode_parser_finalize(tadlangcode_parser_t *parser)
{
}

EXPORT W tadlangcodetoTC(tadlangcode *src, TC *dest, W dest_len)
{
	W i, req_len;

	req_len = 2 + (src->iter+1)/2*2;

	if (dest == NULL || dest_len < 0) {
		return req_len / sizeof(TC);
	}

	for (i = 0; i < (src->iter + 1) / 2; i++) {
		dest[i] = 0xFEFE;
	}
	if (src->iter % 2 == 1) {
		dest[i] = src->code;
	} else {
		dest[i] = 0xFE00 | src->code;
	}

	return req_len / sizeof(TC);
}

EXPORT W TCtotadlangcode(TC *src, W src_len, tadlangcode *dest)
{
	tadlangcode_parser_t parser;
	W i, err;
	TADLANGCODE_PARSER_RESULT result;

	tadlangcode_parser_initialize(&parser);

	for (i = 0; i < src_len; i++) {
		result = tadlangcode_parser_inputchar(&parser, src[i]);
		if (result == TADLANGCODE_PARSER_RESULT_DETERMINED) {
			break;
		}
	}

	err = tadlangcode_parser_getlangcode(&parser, dest);

	tadlangcode_parser_finalize(&parser);

	return err;
}

EXPORT Bool tadlangcodecmpTC(TC *str, W len, tadlangcode *langcode)
{
	W err;
	tadlangcode lang;

	err = TCtotadlangcode(str, len, &lang);
	if (err < 0) {
		return False;
	}
	return tadlangcodecmp(&lang, langcode);
}

EXPORT Bool tadlangcodecmp(tadlangcode *langcode1, tadlangcode *langcode2)
{
	if ((langcode1->iter == langcode2->iter)&&(langcode1->code == langcode2->code)) {
		return True;
	}
	return False;
}

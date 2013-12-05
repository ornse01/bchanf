/*
 * tadlangcode.h
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

/* Vendor name: */
/* Functionality name: tadlangcode */
/* Detail name: */

#include    <basic.h>

#ifndef __TADLANGCODE_H__
#define __TADLANGCODE_H__

struct tadlangcode_ {
	UW iter;
	UB code;
};
typedef struct tadlangcode_ tadlangcode;

IMPORT W tadlangcodetoTC(tadlangcode *src, TC *dest, W dest_len);
IMPORT W TCtotadlangcode(TC *src, W src_len, tadlangcode *dest);
IMPORT Bool tadlangcodecmpTC(TC *str, W len, tadlangcode *langcode);
IMPORT Bool tadlangcodecmp(tadlangcode *langcode1, tadlangcode *langcode2);

/* Functionality name: tadlangcode */
/* Detail name: parser */
struct tadlangcode_parser_t_ {
	enum {
		TADLANGCODE_PARSER_STATE_START,
		TADLANGCODE_PARSER_STATE_LANGCODE,
		TADLANGCODE_PARSER_STATE_DETERMINED,
	} state;
	UW iter;
	UB code;
};
typedef struct tadlangcode_parser_t_ tadlangcode_parser_t;

/* Functionality name: tadlangcode */
/* Detail name: parser */
/* Data structure identifier: result */
enum TADLANGCODE_PARSER_RESULT_ {
	TADLANGCODE_PARSER_RESULT_CONTINUE,
	TADLANGCODE_PARSER_RESULT_INVALID,
	TADLANGCODE_PARSER_RESULT_DETERMINED,
};
typedef enum TADLANGCODE_PARSER_RESULT_ TADLANGCODE_PARSER_RESULT;

IMPORT VOID tadlangcode_parser_initialize(tadlangcode_parser_t *parser);
IMPORT VOID tadlangcode_parser_finalize(tadlangcode_parser_t *parser);
IMPORT TADLANGCODE_PARSER_RESULT tadlangcode_parser_inputchar(tadlangcode_parser_t *parser, TC ch);
IMPORT W tadlangcode_parser_getlangcode(tadlangcode_parser_t *parser, tadlangcode *langcode);

#endif

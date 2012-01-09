/*
 * charreferparser.h
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

/* Vendor name: */
/* Functionality name: charreferparser */
/* Detail name: */

#include	<basic.h>

#include    "tokenchecker.h"

#ifndef __CHARREFERPARSER_H__
#define __CHARREFERPARSER_H__

/* Functionality name: charreferparser */
/* Detail name: */
struct charreferparser_t_ {
	enum {
		START,
		RECIEVE_AMP,
		RECIEVE_NUMBER,
		NUMERIC_DECIMAL,
		NUMERIC_HEXADECIMAL,
		NAMED,
		INVALID,
		DETERMINED
	} state;
	tokenchecker_t named;
	W charnumber;
};
typedef struct charreferparser_t_ charreferparser_t;

/* Functionality name: charreferparser */
/* Detail name: */
/* Data structure identifier: result */
typedef enum {
	CHARREFERPARSER_RESULT_CONTINUE,
	CHARREFERPARSER_RESULT_DETERMINE,
	CHARREFERPARSER_RESULT_INVALID
} CHARREFERPARSER_RESULT;

IMPORT W charreferparser_initialize(charreferparser_t *parser);
IMPORT VOID charreferparser_finalize(charreferparser_t *parser);
IMPORT CHARREFERPARSER_RESULT charreferparser_parsechar(charreferparser_t *parser, UB ch);
IMPORT W charreferparser_getcharnumber(charreferparser_t *parser);
IMPORT VOID charreferparser_getlastmatchedstring(charreferparser_t *parser, UB **str, W *len);
IMPORT W charreferparser_parsestr(charreferparser_t *parser, UB *str, W len);
IMPORT VOID charreferparser_resetstate(charreferparser_t *parser);

#endif

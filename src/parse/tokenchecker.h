/*
 * parselib.h
 *
 * Copyright (c) 2009-2011 project bchan
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
/* Functionality name: tokenchecker */
/* Detail name: */

#include	<basic.h>

#ifndef __TOKENCHECKER_H__
#define __TOKENCHECKER_H__

/* Functionality name: charreferparser */
/* Detail name: valuetuple */
struct tokenchecker_valuetuple_t_ {
	B *name;
	W val;
};
typedef struct tokenchecker_valuetuple_t_ tokenchecker_valuetuple_t;

/* Functionality name: charreferparser */
/* Detail name: */
struct tokenchecker_t_ {
	tokenchecker_valuetuple_t *namelist;
	W namelistnum;
	B *endtokens;
	W stringindex;
	W listindex_start;
	W listindex_end;
	W flag;
};
typedef struct  tokenchecker_t_ tokenchecker_t;

/* Functionality name: tokenchecker */
/* Detail name: */
/* Data structure identifier: result */
typedef enum {
	TOKENCHECKER_RESULT_CONTINUE,
	TOKENCHECKER_RESULT_CONTINUE_NOMATCH,
	TOKENCHECKER_RESULT_DETERMINE,
	TOKENCHECKER_RESULT_NOMATCH,
	TOKENCHECKER_RESULT_AFTER_END
} TOKENCHECKER_RESULT;

IMPORT VOID tokenchecker_initialize(tokenchecker_t *checker, tokenchecker_valuetuple_t *namelist, W namelistnum, B *endchars);
IMPORT VOID tokenchecker_finalize(tokenchecker_t *checker);
IMPORT VOID tokenchecker_clear(tokenchecker_t *checker);
IMPORT TOKENCHECKER_RESULT tokenchecker_inputchar(tokenchecker_t *checker, UB c, W *val);
IMPORT TOKENCHECKER_RESULT tokenchecker_endinput(tokenchecker_t *checker, W *val);
IMPORT VOID tokenchecker_getlastmatchedstring(tokenchecker_t *checker, UB **str, W *len);

#endif

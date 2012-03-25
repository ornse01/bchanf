/*
 * taditerator.h
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
/* Functionality name: taditerator */
/* Detail name: */

#include    <basic.h>
#include	<btron/dp.h>
#include    <tad.h>

#ifndef __TADITERATOR_H__
#define __TADITERATOR_H__

/* Functionality name: taditerator */
/* Detail name: */
struct taditerator_t_ {
	TC *bin;
	W len;
	W index;
};
typedef struct taditerator_t_ taditerator_t;

/* Functionality name: taditerator */
/* Detail name: */
/* Data structure identifier: resulttype */
enum TADITERATOR_RESULTTYPE_ {
	TADITERATOR_RESULTTYPE_CHARCTOR,
	TADITERATOR_RESULTTYPE_SEGMENT,
	TADITERATOR_RESULTTYPE_END,
};
typedef enum TADITERATOR_RESULTTYPE_ TADITERATOR_RESULTTYPE;

/* Functionality name: taditerator */
/* Detail name: */
/* Data structure identifier: resulttype */
struct taditerator_result_ {
	TADITERATOR_RESULTTYPE type;
	TC *pos;
	TC segment;
	union {
		TADSEG *s;
		LTADSEG *l;
	} vseg;
	W segsize;
	UB *data;
};
typedef struct taditerator_result_ taditerator_result;

IMPORT VOID taditerator_initialize(taditerator_t *iterator, TC *bin, W strlen);
IMPORT VOID taditerator_finalize(taditerator_t *iterator);
IMPORT TADITERATOR_RESULTTYPE taditerator_next(taditerator_t *iterator, TC **pos, TC *segment, LTADSEG **seg, W *segsize, UB **data);
IMPORT VOID taditerator_next2(taditerator_t *iterator, taditerator_result *result);

#endif

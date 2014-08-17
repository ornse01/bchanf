/*
 * tadfragment.h
 *
 * Copyright (c) 2013-2014 project bchan
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
/* Functionality name: tadfragment */
/* Detail name: */

#include    <basic.h>
#include	<btron/dp.h>
#include    <tad.h>

#include    <coll/bytearray.h>
#include    <coll/wordarray.h>
#include    "tadlangcode.h"
#include    "tadsegment.h"

#ifndef __TADFRAGMENT_H__
#define __TADFRAGMENT_H__

/* Functionality name: tadfragment */
/* Detail name: */
struct tadfragment_t_ {
	bytearray_t rawdata;
	wordarray_t pos;
};
typedef struct tadfragment_t_ tadfragment_t;

/* Functionality name: tadfragment */
/* Detail name: cursor */
struct tadfragment_cursor_t_ {
	tadfragment_t *target;
	wordarray_cursor_t base;
};
typedef struct tadfragment_cursor_t_ tadfragment_cursor_t;

IMPORT W tadfragment_initialize(tadfragment_t *fragment);
IMPORT VOID tadfragment_finalize(tadfragment_t *fragment);
IMPORT UB* tadfragment_getbuffer(tadfragment_t *fragment);
IMPORT W tadfragment_getsegmentlength(tadfragment_t *fragment);
IMPORT W tadfragment_getbufferlength(tadfragment_t *fragment);
IMPORT W tadfragment_pushback(tadfragment_t *fragment, UB *data, W len);
IMPORT W tadfragment_popback(tadfragment_t *fragment);
IMPORT VOID tadfragment_truncate(tadfragment_t *fragment, W size);

IMPORT VOID tadfragment_cursor_initialize(tadfragment_cursor_t *cursor, tadfragment_t *fragment);
IMPORT VOID tadfragment_cursor_finalize(tadfragment_cursor_t *cursor);
IMPORT W tadfragment_cursor_move(tadfragment_cursor_t *cursor, W diff);
IMPORT W tadfragment_cursor_erase(tadfragment_cursor_t *cursor, W len);
IMPORT W tadfragment_cursor_insert(tadfragment_cursor_t *cursor, UB *data, W len);
IMPORT W tadfragment_cursor_insertlang(tadfragment_cursor_t *cursor, tadlangcode *lang);
IMPORT W tadfragment_cursor_insertsegment(tadfragment_cursor_t *cursor, tadsegment *segment);
IMPORT Bool tadfragment_cursor_isend(tadfragment_cursor_t *cursor);
IMPORT W tadfragment_cursor_getdata(tadfragment_cursor_t *cursor, tadsegment *p);

#endif

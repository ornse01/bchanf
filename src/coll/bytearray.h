/*
 * bytearray.h
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
/* Functionality name: bytearray */
/* Detail name: */

#include	<basic.h>

#ifndef __BYTEARRAY_H__
#define __BYTEARRAY_H__

/* Vendor name: */
/* Functionality name: bytearray */
/* Detail name: */
struct bytearray_t_ {
	UB *ptr;
	W len;
};
typedef struct bytearray_t_ bytearray_t;

/* Vendor name: */
/* Functionality name: bytearray */
/* Detail name: cursor */
struct bytearray_cursor_t_ {
	bytearray_t *target;
	W index;
};
typedef struct bytearray_cursor_t_ bytearray_cursor_t;

IMPORT W bytearray_initialize(bytearray_t *bytearray);
IMPORT VOID bytearray_finalize(bytearray_t *bytearray);
IMPORT UB* bytearray_getbuffer(bytearray_t *bytearray);
IMPORT W bytearray_getlength(bytearray_t *bytearray);
IMPORT W bytearray_pushback(bytearray_t *bytearray, UB val);
IMPORT W bytearray_popback(bytearray_t *bytearray);

IMPORT VOID bytearray_cursor_initialize(bytearray_cursor_t *cursor, bytearray_t *bytearray);
IMPORT VOID bytearray_cursor_finalize(bytearray_cursor_t *cursor);
IMPORT W bytearray_cursor_move(bytearray_cursor_t *cursor, W diff);
IMPORT W bytearray_cursor_erase(bytearray_cursor_t *cursor, W len);
IMPORT W bytearray_cursor_insert(bytearray_cursor_t *cursor, UB *data, W len);
IMPORT Bool bytearray_cursor_isend(bytearray_cursor_t *cursor);
IMPORT W bytearray_cursor_getB(bytearray_cursor_t *cursor, B *p);
IMPORT W bytearray_cursor_getH(bytearray_cursor_t *cursor, H *p);
IMPORT W bytearray_cursor_getW(bytearray_cursor_t *cursor, W *p);
IMPORT W bytearray_cursor_getUB(bytearray_cursor_t *cursor, UB *p);
IMPORT W bytearray_cursor_getUH(bytearray_cursor_t *cursor, UH *p);
IMPORT W bytearray_cursor_getUW(bytearray_cursor_t *cursor, UW *p);

#endif

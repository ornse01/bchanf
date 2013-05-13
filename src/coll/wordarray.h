/*
 * wordarray.h
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
/* Functionality name: wordarray */
/* Detail name: */

#include	<basic.h>

#include    "bytearray.h"

#ifndef __WORDARRAY_H__
#define __WORDARRAY_H__

/* Functionality name: wordarray */
/* Detail name: */
struct wordarray_t_ {
	bytearray_t rawdata;
};
typedef struct wordarray_t_ wordarray_t;

/* Functionality name: wordarray */
/* Detail name: cursor */
struct wordarray_cursor_t_ {
	bytearray_cursor_t base;
};
typedef struct wordarray_cursor_t_ wordarray_cursor_t;

IMPORT W wordarray_initialize(wordarray_t *wordarray);
IMPORT VOID wordarray_finalize(wordarray_t *wordarray);
IMPORT UW* wordarray_getbuffer(wordarray_t *wordarray);
IMPORT W wordarray_getlength(wordarray_t *wordarray);
IMPORT W wordarray_pushback(wordarray_t *wordarray, UW val);
IMPORT W wordarray_popback(wordarray_t *wordarray);

IMPORT VOID wordarray_cursor_initialize(wordarray_cursor_t *cursor, wordarray_t *wordarray);
IMPORT VOID wordarray_cursor_finalize(wordarray_cursor_t *cursor);
IMPORT W wordarray_cursor_move(wordarray_cursor_t *cursor, W diff);
IMPORT W wordarray_cursor_erase(wordarray_cursor_t *cursor, W len);
IMPORT W wordarray_cursor_insert(wordarray_cursor_t *cursor, UW *data, W len);
IMPORT Bool wordarray_cursor_isend(wordarray_cursor_t *cursor);
IMPORT W wordarray_cursor_getW(wordarray_cursor_t *cursor, W *p);
IMPORT W wordarray_cursor_getUW(wordarray_cursor_t *cursor, UW *p);

#endif

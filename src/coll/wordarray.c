/*
 * bytearray.c
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

#include    "wordarray.h"

#include	<basic.h>
#include	<bstdio.h>

#include    "bytearray.h"

#ifdef BCHAN_CONFIG_DEBUG
# define DP(arg) printf arg
# define DP_ER(msg, err) printf("%s (%d/%x)\n", msg, err>>16, err)
#else
# define DP(arg) /**/
# define DP_ER(msg, err) /**/
#endif

EXPORT UW* wordarray_getbuffer(wordarray_t *wordarray)
{
	return (UW*)bytearray_getbuffer(&wordarray->rawdata);
}

EXPORT W wordarray_getlength(wordarray_t *wordarray)
{
	return bytearray_getlength(&wordarray->rawdata) / sizeof(UW);
}

EXPORT W wordarray_pushback(wordarray_t *wordarray, UW val)
{
	W i, j, err;
	UB *p;

	/* TODO: more efficient */
	p = (UB*)&val;
	for (i = 0; i < sizeof(UW); i++) {
		err = bytearray_pushback(&wordarray->rawdata, p[i]);
		if (err < 0) {
			DP_ER("bytearray_pushback error", err);
			goto error;
		}
	}

	return 0;

error:
	for (j = 0; j < i; j++) {
		bytearray_popback(&wordarray->rawdata);
	}
	return err;
}

EXPORT W wordarray_popback(wordarray_t *wordarray)
{
	W i, len;

	len = wordarray_getlength(wordarray);
	if (len == 0) {
		return -1; /* TODO */
	}

	for (i = 0; i < sizeof(UW); i++) {
		bytearray_popback(&wordarray->rawdata);
	}

	return 0;
}

EXPORT W wordarray_getat(wordarray_t *wordarray, W at, UW *p)
{
	bytearray_cursor_t cursor;
	W err;

	/* should be more efficient? */
	bytearray_cursor_initialize(&cursor, &wordarray->rawdata);
	err = bytearray_cursor_move(&cursor, at*sizeof(UW));
	if (err == 0) {
		err = bytearray_cursor_getUW(&cursor, p);
	}
	bytearray_cursor_finalize(&cursor);

	return err;
}

EXPORT W wordarray_setat(wordarray_t *wordarray, W at, UW val)
{
	bytearray_cursor_t cursor;
	W err;

	/* should be more efficient? */
	bytearray_cursor_initialize(&cursor, &wordarray->rawdata);
	err = bytearray_cursor_move(&cursor, at*sizeof(UW));
	if (err == 0) {
		err = bytearray_cursor_setUW(&cursor, val);
	}
	bytearray_cursor_finalize(&cursor);

	return err;
}

EXPORT W wordarray_initialize(wordarray_t *wordarray)
{
	return bytearray_initialize(&wordarray->rawdata);
}

EXPORT VOID wordarray_finalize(wordarray_t *wordarray)
{
	bytearray_finalize(&wordarray->rawdata);
}

EXPORT W wordarray_cursor_move(wordarray_cursor_t *cursor, W diff)
{
	return bytearray_cursor_move(&cursor->base, diff*sizeof(UW));
}

EXPORT W wordarray_cursor_erase(wordarray_cursor_t *cursor, W len)
{
	return bytearray_cursor_erase(&cursor->base, len*sizeof(UW));
}

EXPORT W wordarray_cursor_insert(wordarray_cursor_t *cursor, UW *data, W len)
{
	return bytearray_cursor_insert(&cursor->base, (UB*)data, len*sizeof(UW));
}

EXPORT Bool wordarray_cursor_isend(wordarray_cursor_t *cursor)
{
	return bytearray_cursor_isend(&cursor->base);
}

EXPORT W wordarray_cursor_getW(wordarray_cursor_t *cursor, W *p)
{
	return bytearray_cursor_getW(&cursor->base, p);
}

EXPORT W wordarray_cursor_getUW(wordarray_cursor_t *cursor, UW *p)
{
	return bytearray_cursor_getUW(&cursor->base, p);
}

EXPORT W wordarray_cursor_setW(wordarray_cursor_t *cursor, W val)
{
	return bytearray_cursor_setW(&cursor->base, val);
}

EXPORT W wordarray_cursor_setUW(wordarray_cursor_t *cursor, UW val)
{
	return bytearray_cursor_setUW(&cursor->base, val);
}

EXPORT VOID wordarray_cursor_initialize(wordarray_cursor_t *cursor, wordarray_t *wordarray)
{
	bytearray_cursor_initialize(&cursor->base, &wordarray->rawdata);
}

EXPORT VOID wordarray_cursor_finalize(wordarray_cursor_t *cursor)
{
	bytearray_cursor_finalize(&cursor->base);
}

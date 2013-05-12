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

#include    "bytearray.h"

#include	<basic.h>
#include	<bstdlib.h>
#include	<bstdio.h>
#include	<bstring.h>

#ifdef BCHAN_CONFIG_DEBUG
# define DP(arg) printf arg
# define DP_ER(msg, err) printf("%s (%d/%x)\n", msg, err>>16, err)
#else
# define DP(arg) /**/
# define DP_ER(msg, err) /**/
#endif

EXPORT UB* bytearray_getbuffer(bytearray_t *bytearray)
{
	return bytearray->ptr;
}

EXPORT W bytearray_getlength(bytearray_t *bytearray)
{
	return bytearray->len;
}

EXPORT W bytearray_pushback(bytearray_t *bytearray, UB val)
{
	UB *p;

	p = realloc(bytearray->ptr, bytearray->len+1);
	if (p == NULL) {
		return -1;
	}
	bytearray->ptr = p;
	bytearray->ptr[bytearray->len] = val;
	bytearray->len++;
	return 0;
}

EXPORT W bytearray_popback(bytearray_t *bytearray)
{
	if (bytearray->len == 0) {
		return -1; /* TODO */
	}
	bytearray->len--;
	return 0;
}

EXPORT W bytearray_initialize(bytearray_t *bytearray)
{
	bytearray->ptr = NULL;
	bytearray->len = 0;
	return 0;
}

EXPORT VOID bytearray_finalize(bytearray_t *bytearray)
{
	if (bytearray->ptr != NULL) {
		free(bytearray->ptr);
	}
}

EXPORT W bytearray_cursor_move(bytearray_cursor_t *cursor, W diff)
{
	W next = cursor->index + diff;

	if (next < 0) {
		return -1; /* TODO */
	}
	if (next > cursor->target->len) {
		return -1; /* TODO */
	}
	cursor->index = next;

	return 0;
}

EXPORT W bytearray_cursor_erase(bytearray_cursor_t *cursor, W len)
{
	Bool isend;
	UB *dest, *src;
	size_t move_len;

	if (len < 0) {
		return -1; /* TODO */
	}

	isend = bytearray_cursor_isend(cursor);
	if (isend != False) {
		return -1; /* TODO */
	}

	if (cursor->index + len > cursor->target->len) {
		cursor->target->len = cursor->index;
	} else {
		dest = cursor->target->ptr + cursor->index;
		src = dest + len;
		move_len = cursor->target->len - (cursor->index + len);
		memmove(dest, src, move_len);
		cursor->target->len -= len;
	}

	return 0;
}

EXPORT W bytearray_cursor_insert(bytearray_cursor_t *cursor, UB *data, W len)
{
	UB *p;
	UB *dest, *src;

	if (len < 0) {
		return -1; /* TODO */
	}
	if (len == 0) {
		return 0;
	}

	p = realloc(cursor->target->ptr, cursor->target->len + len);
	if (p == NULL) {
		return -1; /* TODO */
	}
	cursor->target->ptr = p;

	/* move previous data */
	src = cursor->target->ptr + cursor->index;
	dest = src + len;
	memmove(dest, src, cursor->target->len - cursor->index);

	/* insert new data */
	dest = src;
	src = data;
	memcpy(dest, src, len);

	cursor->target->len += len;

	return 0;
}

EXPORT Bool bytearray_cursor_isend(bytearray_cursor_t *cursor)
{
	if (cursor->index == cursor->target->len) {
		return True;
	}
	return False;
}

EXPORT W bytearray_cursor_getB(bytearray_cursor_t *cursor, B *p)
{
	if (cursor->index + sizeof(B) > cursor->target->len) {
		return -1; /* TODO */
	}
	*p = *(cursor->target->ptr + cursor->index);
	return 0;
}

EXPORT W bytearray_cursor_getH(bytearray_cursor_t *cursor, H *p)
{
	if (cursor->index + sizeof(H) > cursor->target->len) {
		return -1; /* TODO */
	}
	*p = *(H*)(cursor->target->ptr + cursor->index);
	return 0;
}

EXPORT W bytearray_cursor_getW(bytearray_cursor_t *cursor, W *p)
{
	if (cursor->index + sizeof(W) > cursor->target->len) {
		return -1; /* TODO */
	}
	*p = *(W*)(cursor->target->ptr + cursor->index);
	return 0;
}

EXPORT W bytearray_cursor_getUB(bytearray_cursor_t *cursor, UB *p)
{
	if (cursor->index + sizeof(UB) > cursor->target->len) {
		return -1; /* TODO */
	}
	*p = *(cursor->target->ptr + cursor->index);
	return 0;
}

EXPORT W bytearray_cursor_getUH(bytearray_cursor_t *cursor, UH *p)
{
	if (cursor->index + sizeof(UH) > cursor->target->len) {
		return -1; /* TODO */
	}
	*p = *(UH*)(cursor->target->ptr + cursor->index);
	return 0;
}

EXPORT W bytearray_cursor_getUW(bytearray_cursor_t *cursor, UW *p)
{
	if (cursor->index + sizeof(UW) > cursor->target->len) {
		return -1; /* TODO */
	}
	*p = *(UW*)(cursor->target->ptr + cursor->index);
	return 0;
}

EXPORT VOID bytearray_cursor_initialize(bytearray_cursor_t *cursor, bytearray_t *bytearray)
{
	cursor->target = bytearray;
	cursor->index = 0;
}

EXPORT VOID bytearray_cursor_finalize(bytearray_cursor_t *cursor)
{
}

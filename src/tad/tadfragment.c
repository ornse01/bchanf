/*
 * tadfragment.c
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

#include "tadfragment.h"

#include	<basic.h>
#include	<bstdio.h>

#include    <coll/bytearray.h>
#include    <coll/wordarray.h>
#include    "tadlexer_le.h"

#ifdef BCHAN_CONFIG_DEBUG
# define DP(arg) printf arg
# define DP_ER(msg, err) printf("%s (%d/%x)\n", msg, err>>16, err)
#else
# define DP(arg) /**/
# define DP_ER(msg, err) /**/
#endif

#if 0
# define DP_STATE(state) printf("%s\n", state)
#else
# define DP_STATE(state) /**/
#endif

struct tadfragment_pos_iterator_t_ {
	enum {
		TADFRAGMENT_POS_ITERATOR_STATE_BEFORE_SEGSTART,
		TADFRAGMENT_POS_ITERATOR_STATE_READING_TLANG,
		TADFRAGMENT_POS_ITERATOR_STATE_READING,
		TADFRAGMENT_POS_ITERATOR_STATE_COMPLETED,
		TADFRAGMENT_POS_ITERATOR_STATE_ERROR,
	} state;
	UB *data;
	W len;
	W pos;
	tadlexer_le_t lexer;
	struct {
		tadlexer_le_result *result;
		W len;
		W pos;
	} lexer_result_handling;
};
typedef struct tadfragment_pos_iterator_t_ tadfragment_pos_iterator_t;

struct tadfragment_pos_iterator_result_ {
	enum {
		TADFRAGMENT_POS_ITERATOR_RESULT_TYPE_SEGMENT,
		TADFRAGMENT_POS_ITERATOR_RESULT_TYPE_ERROR,
	} type;
	W index;
};
typedef struct tadfragment_pos_iterator_result_ tadfragment_pos_iterator_result;

LOCAL W tadfragment_pos_iterator_input(tadfragment_pos_iterator_t *iterator, tadlexer_le_result *result, Bool *segstart)
{
	Bool fixed;
	W err = 0;

	*segstart = False;

	switch (iterator->state) {
	case TADFRAGMENT_POS_ITERATOR_STATE_BEFORE_SEGSTART:
		DP_STATE("BEFORE_SEGSTART");
		iterator->state = TADFRAGMENT_POS_ITERATOR_STATE_READING;
		*segstart = True;
		break;
	case TADFRAGMENT_POS_ITERATOR_STATE_READING_TLANG:
		DP_STATE("READING_TLANG");
		if (result->type == TADLEXER_LE_RESULTTYPE_SEGMENT_END) {
			fixed = tadlexer_le_result_is_fixedsegment(result);
			if (fixed == False) {
				iterator->state = TADFRAGMENT_POS_ITERATOR_STATE_ERROR;
				err = -1; /* TODO */
				break;
			}
			if (result->val.ch == 0xFEFE) {
				break;
			}
			if ((result->val.ch & 0xFF00) != 0xFE00) {
				iterator->state = TADFRAGMENT_POS_ITERATOR_STATE_ERROR;
				err = -1; /* TODO */
				break;
			}
			iterator->state = TADFRAGMENT_POS_ITERATOR_STATE_BEFORE_SEGSTART;
		}
		break;
	case TADFRAGMENT_POS_ITERATOR_STATE_READING:
		DP_STATE("READING");
		if (result->type != TADLEXER_LE_RESULTTYPE_SEGMENT_END) {
			break;
		}
		fixed = tadlexer_le_result_is_fixedsegment(result);
		if ((fixed != False)&&(result->val.ch == 0xFEFE)) {
			iterator->state = TADFRAGMENT_POS_ITERATOR_STATE_READING_TLANG;
			break;
		}
		iterator->state = TADFRAGMENT_POS_ITERATOR_STATE_BEFORE_SEGSTART;
		break;
	case TADFRAGMENT_POS_ITERATOR_STATE_COMPLETED:
		DP_STATE("COMPLETED");
		break;
	case TADFRAGMENT_POS_ITERATOR_STATE_ERROR:
		DP_STATE("ERROR");
		err = -1; /* TODO */
		break;
	}

	return err;
}

LOCAL Bool tadfragment_pos_iterator_next(tadfragment_pos_iterator_t *iterator, tadfragment_pos_iterator_result *result)
{
	UB ch;
	W pos, len, err;
	Bool segstart;
	tadlexer_le_result *lexer_result;

	for (; iterator->lexer_result_handling.pos < iterator->lexer_result_handling.len; ) {
		pos = iterator->lexer_result_handling.pos++;
		lexer_result = iterator->lexer_result_handling.result + pos;
		err = tadfragment_pos_iterator_input(iterator, lexer_result, &segstart);
		if (err < 0) {
			result->type = TADFRAGMENT_POS_ITERATOR_RESULT_TYPE_ERROR;
			return False;
		}
		if (segstart != False) {
			result->type = TADFRAGMENT_POS_ITERATOR_RESULT_TYPE_SEGMENT;
			result->index = iterator->pos - 1;
			return True;
		}
	}

	for (; iterator->pos < iterator->len;) {
		ch = iterator->data[iterator->pos++];
		tadlexer_le_inputbyte(&iterator->lexer, ch, &lexer_result, &len);
		iterator->lexer_result_handling.result = lexer_result;
		iterator->lexer_result_handling.len = len;
		iterator->lexer_result_handling.pos = 0;

		for (; iterator->lexer_result_handling.pos < iterator->lexer_result_handling.len; ) {
			pos = iterator->lexer_result_handling.pos++;
			lexer_result = iterator->lexer_result_handling.result + pos;
			err = tadfragment_pos_iterator_input(iterator, lexer_result, &segstart);
			if (err < 0) {
				result->type = TADFRAGMENT_POS_ITERATOR_RESULT_TYPE_ERROR;
				return False;
			}
			if (segstart != False) {
				result->type = TADFRAGMENT_POS_ITERATOR_RESULT_TYPE_SEGMENT;
				result->index = iterator->pos - 1;
				return True;
			}
		}
	}

	if (iterator->state == TADFRAGMENT_POS_ITERATOR_STATE_BEFORE_SEGSTART) {
		iterator->state = TADFRAGMENT_POS_ITERATOR_STATE_COMPLETED;
		result->type = TADFRAGMENT_POS_ITERATOR_RESULT_TYPE_SEGMENT;
		result->index = iterator->len;
	} else {
		result->type = TADFRAGMENT_POS_ITERATOR_RESULT_TYPE_ERROR;
	}

	return False;
}

LOCAL VOID tadfragment_pos_iterator_initialize(tadfragment_pos_iterator_t *iterator, UB *data, W len)
{
	tadlexer_le_initialize(&iterator->lexer);
	iterator->state = TADFRAGMENT_POS_ITERATOR_STATE_BEFORE_SEGSTART;
	iterator->data = data;
	iterator->len = len;
	iterator->pos = 0;
	iterator->lexer_result_handling.result = NULL;
	iterator->lexer_result_handling.len = 0;
	iterator->lexer_result_handling.pos = 0;
}

LOCAL VOID tadfragment_pos_iterator_finalize(tadfragment_pos_iterator_t *iterator)
{
	tadlexer_le_finalize(&iterator->lexer);
}

LOCAL Bool tadfragment_verifydata(UB *data, W len)
{
	tadfragment_pos_iterator_t iterator;
	tadfragment_pos_iterator_result result;
	Bool cont, ret = True;

	tadfragment_pos_iterator_initialize(&iterator, data, len);
	for (;;) {
		cont = tadfragment_pos_iterator_next(&iterator, &result);
		if (cont == False) {
			break;
		}
		if (result.type == TADFRAGMENT_POS_ITERATOR_RESULT_TYPE_SEGMENT) {
		} else {
			ret = False;
			break;
		}
	}
	tadfragment_pos_iterator_finalize(&iterator);

	return True;
}

EXPORT UB* tadfragment_getbuffer(tadfragment_t *fragment)
{
	return bytearray_getbuffer(&fragment->rawdata);
}

EXPORT W tadfragment_getsegmentlength(tadfragment_t *fragment)
{
	return wordarray_getlength(&fragment->pos);
}

EXPORT W tadfragment_getbufferlength(tadfragment_t *fragment)
{
	return bytearray_getlength(&fragment->rawdata);
}

EXPORT W tadfragment_pushback(tadfragment_t *fragment, UB *data, W len)
{
	tadfragment_cursor_t cursor;
	W num, err;

	tadfragment_cursor_initialize(&cursor, fragment);

	num = tadfragment_getsegmentlength(fragment);
	tadfragment_cursor_move(&cursor, num);

	err = tadfragment_cursor_insert(&cursor, data, len);

	tadfragment_cursor_finalize(&cursor);

	return err;
}

EXPORT W tadfragment_popback(tadfragment_t *fragment)
{
	W len, val, i, size;

	len = wordarray_getlength(&fragment->pos);
	if (len == 0) {
		return -1; /* TODO */
	}

	wordarray_getat(&fragment->pos, len-1, &val);
	size = bytearray_getlength(&fragment->rawdata);

	for (i = 0; i < size - val; i++) {
		bytearray_popback(&fragment->rawdata);
	}
	wordarray_popback(&fragment->pos);

	return 0;
}

EXPORT W tadfragment_initialize(tadfragment_t *fragment)
{
	W err;

	err = bytearray_initialize(&fragment->rawdata);
	if (err < 0) {
		return err;
	}
	err = wordarray_initialize(&fragment->pos);
	if (err < 0) {
		bytearray_finalize(&fragment->rawdata);
		return err;
	}

	return 0;
}

EXPORT VOID tadfragment_finalize(tadfragment_t *fragment)
{
	wordarray_finalize(&fragment->pos);
	bytearray_finalize(&fragment->rawdata);
}

EXPORT W tadfragment_cursor_move(tadfragment_cursor_t *cursor, W diff)
{
	return wordarray_cursor_move(&cursor->base, diff);
}

EXPORT W tadfragment_cursor_erase(tadfragment_cursor_t *cursor, W len)
{
	bytearray_cursor_t bytecursor;
	Bool isend;
	W start_pos, end_pos, diff, val, count, err;

	if (len < 0) {
		return -1; /* TODO */
	}

	isend = tadfragment_cursor_isend(cursor);
	if (isend != False) {
		return -1; /* TODO */
	}

	wordarray_cursor_getW(&cursor->base, &start_pos);
	err = tadfragment_cursor_move(cursor, len);
	if (err < 0) {
		diff = bytearray_getlength(&cursor->target->rawdata) - start_pos;
	} else {
		wordarray_cursor_getW(&cursor->base, &end_pos);
		tadfragment_cursor_move(cursor, -len);

		diff = end_pos - start_pos;
	}

	wordarray_cursor_erase(&cursor->base, len);

	bytearray_cursor_initialize(&bytecursor, &cursor->target->rawdata);
	bytearray_cursor_move(&bytecursor, start_pos);
	bytearray_cursor_erase(&bytecursor, diff);
	bytearray_cursor_finalize(&bytecursor);

	count = 0;
	for (;;) {
		err = wordarray_cursor_getW(&cursor->base, &val);
		if (err < 0) {
			/* TODO: error or end position */
			break;
		}
		wordarray_cursor_setW(&cursor->base, val - diff);
		wordarray_cursor_move(&cursor->base, 1);
		count++;
	}
	wordarray_cursor_move(&cursor->base, -count);

	return 0;
}

LOCAL W tadfragment_cursor_getnextpos(tadfragment_cursor_t *cursor, W *pos)
{
	W err;
	Bool isend;

	err = tadfragment_cursor_move(cursor, 1);
	if (err < 0) {
		return err;
	}
	isend = tadfragment_cursor_isend(cursor);
	if (isend != False) {
		*pos = bytearray_getlength(&cursor->target->rawdata);
		tadfragment_cursor_move(cursor, -1);
		return 0;
	}

	err = wordarray_cursor_getW(&cursor->base, pos);
	tadfragment_cursor_move(cursor, -1);

	return err;
}

EXPORT W tadfragment_cursor_insert(tadfragment_cursor_t *cursor, UB *data, W len)
{
	bytearray_cursor_t bytecursor;
	tadfragment_pos_iterator_t iterator;
	tadfragment_pos_iterator_result result;
	Bool ok, cont, isend;
	W offset, val, err, err2;

	if (len < 0) {
		return -1; /* TODO */
	}
	if (len == 0) {
		return 0;
	}

	ok = tadfragment_verifydata(data, len);
	if (ok == False) {
		return -1; /* TODO */
	}

	isend = tadfragment_cursor_isend(cursor);
	if (isend == False) {
		err = wordarray_cursor_getW(&cursor->base, &offset);
		if (err < 0) {
			return err;
		}
	} else {
		offset = bytearray_getlength(&cursor->target->rawdata);
	}

	bytearray_cursor_initialize(&bytecursor, &cursor->target->rawdata);
	bytearray_cursor_move(&bytecursor, offset);
	err = bytearray_cursor_insert(&bytecursor, data, len);
	bytearray_cursor_finalize(&bytecursor);
	if (err < 0) {
		return err;
	}

	tadfragment_pos_iterator_initialize(&iterator, data, len);
	for (;;) {
		cont = tadfragment_pos_iterator_next(&iterator, &result);
		if (cont == False) {
			break;
		}
		if (result.type == TADFRAGMENT_POS_ITERATOR_RESULT_TYPE_SEGMENT) {
			val = result.index + offset;
			err = wordarray_cursor_insert(&cursor->base, &val, 1);
			if (err < 0) {
				break;
			}
			wordarray_cursor_move(&cursor->base, 1);
		}
	}
	tadfragment_pos_iterator_finalize(&iterator);

	for (;;) {
		err2 = wordarray_cursor_getW(&cursor->base, &val);
		if (err2 < 0) {
			/* TODO: error or end position */
			break;
		}
		wordarray_cursor_setW(&cursor->base, val + len);
		wordarray_cursor_move(&cursor->base, 1);
	}

	return err;
}

EXPORT Bool tadfragment_cursor_isend(tadfragment_cursor_t *cursor)
{
	return wordarray_cursor_isend(&cursor->base);
}

EXPORT W tadfragment_cursor_getdata(tadfragment_cursor_t *cursor, tadfragment_cursor_segment *p)
{
	Bool end;
	W offset, offset_next, err;
	UH ch;

	end = tadfragment_cursor_isend(cursor);
	if (end != False) {
		return -1; // TODO
	}

	err = wordarray_cursor_getW(&cursor->base, &offset);
	if (err < 0) {
		return err;
	}
	err = tadfragment_cursor_getnextpos(cursor, &offset_next);
	if (err < 0) {
		return err;
	}

	p->p = bytearray_getbuffer(&cursor->target->rawdata) + offset;
	p->len = offset_next - offset;

	ch = *(UH*)p->p;
	if ((ch & 0xFF80) == 0xFF80) {
		p->type = TADFRAGMENT_CURSOR_SEGMENTTYPE_VARIABLE;
	} else if ((ch & 0xFE00) == 0xFE00) {
		p->type = TADFRAGMENT_CURSOR_SEGMENTTYPE_LANGCODE;
	} else {
		p->type = TADFRAGMENT_CURSOR_SEGMENTTYPE_CHAR;
	}

	return 0;
}

EXPORT VOID tadfragment_cursor_initialize(tadfragment_cursor_t *cursor, tadfragment_t *fragment)
{
	cursor->target = fragment;
	wordarray_cursor_initialize(&cursor->base, &fragment->pos);
}

EXPORT VOID tadfragment_cursor_finalize(tadfragment_cursor_t *cursor)
{
	wordarray_cursor_finalize(&cursor->base);
}

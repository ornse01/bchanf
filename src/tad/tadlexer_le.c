/*
 * tadlexer_le.c
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

#include "tadlexer_le.h"

#include	<basic.h>
#include	<bstdio.h>

#define TADLEXER_LE_FLAG_FIXED_SEGMENT       0x00000001
#define TADLEXER_LE_FLAG_VARIABLE_SEGMENT    0x00000002
#define TADLEXER_LE_FLAG_SEGMENTID_DETERMINE 0x00000004
#define TADLEXER_LE_FLAG_LENGTH_DETERMINE    0x00000008
#define TADLEXER_LE_FLAG_READING_DATA        0x00000010

EXPORT Bool tadlexer_le_result_is_fixedsegment(tadlexer_le_result *result)
{
	return ((result->flag & TADLEXER_LE_FLAG_FIXED_SEGMENT) != 0) ? True : False;
}

EXPORT Bool tadlexer_le_result_is_variablesegment(tadlexer_le_result *result)
{
	return ((result->flag & TADLEXER_LE_FLAG_VARIABLE_SEGMENT) != 0) ? True : False;
}

EXPORT Bool tadlexer_le_result_is_segmentid_determined(tadlexer_le_result *result)
{
	return ((result->flag & TADLEXER_LE_FLAG_SEGMENTID_DETERMINE) != 0) ? True : False;
}

EXPORT Bool tadlexer_le_result_is_lengthdetermined(tadlexer_le_result *result)
{
	return ((result->flag & TADLEXER_LE_FLAG_LENGTH_DETERMINE) != 0) ? True : False;
}

EXPORT Bool tadlexer_le_result_is_readingdata(tadlexer_le_result *result)
{
	return ((result->flag & TADLEXER_LE_FLAG_READING_DATA) != 0) ? True : False;
}

EXPORT VOID tadlexer_le_inputbyte(tadlexer_le_t *lexer, UB b, tadlexer_le_result **result, W *result_len)
{
	*result = &lexer->buf;
	*result_len = 1;

	switch (lexer->state) {
	case TADLEXER_LE_STATE_START:
		lexer->segid = b;
		lexer->state = TADLEXER_LE_STATE_FIRST_BYTE;
		lexer->buf.type = TADLEXER_LE_RESULTTYPE_READING_SEGMENT;
		lexer->buf.flag = 0;
		break;
	case TADLEXER_LE_STATE_FIRST_BYTE:
		if ((b == 0xFF)&&(((lexer->segid) & 0x80) == 0x80)) {
			lexer->state = TADLEXER_LE_STATE_SEGLEN_FIRST;
			lexer->buf.type = TADLEXER_LE_RESULTTYPE_READING_SEGMENT;
			lexer->buf.flag = TADLEXER_LE_FLAG_VARIABLE_SEGMENT | TADLEXER_LE_FLAG_SEGMENTID_DETERMINE;
			lexer->buf.val.seg.id = lexer->segid;
		} else {
			lexer->state = TADLEXER_LE_STATE_START;
			lexer->buf.type = TADLEXER_LE_RESULTTYPE_SEGMENT_END;
			lexer->buf.flag = TADLEXER_LE_FLAG_FIXED_SEGMENT;
			lexer->buf.val.ch = lexer->segid | (b << 8);
		}
		break;
	case TADLEXER_LE_STATE_SEGLEN_FIRST:
		lexer->len = b;
		lexer->state = TADLEXER_LE_STATE_SEGLEN_SECOND;
		lexer->buf.type = TADLEXER_LE_RESULTTYPE_READING_SEGMENT;
		break;
	case TADLEXER_LE_STATE_SEGLEN_SECOND:
		lexer->len |= b << 8;
		if (lexer->len == 0) {
			lexer->state = TADLEXER_LE_STATE_START;
			lexer->buf.type = TADLEXER_LE_RESULTTYPE_SEGMENT_END;
			lexer->buf.flag |= TADLEXER_LE_FLAG_LENGTH_DETERMINE;
			lexer->buf.val.seg.id = lexer->segid;
			lexer->buf.val.seg.len = 0;
		} else if (lexer->len == 0xFFFF) {
			lexer->state = TADLEXER_LE_STATE_LSEGLEN_FIRST;
			lexer->buf.type = TADLEXER_LE_RESULTTYPE_READING_SEGMENT;
		} else {
			lexer->state = TADLEXER_LE_STATE_DATA;
			lexer->len_read = 0;
			lexer->buf.type = TADLEXER_LE_RESULTTYPE_READING_SEGMENT;
			lexer->buf.flag |= TADLEXER_LE_FLAG_LENGTH_DETERMINE;
			lexer->buf.val.seg.id = lexer->segid;
			lexer->buf.val.seg.len = lexer->len;
		}
		break;
	case TADLEXER_LE_STATE_LSEGLEN_FIRST:
		lexer->len = b;
		lexer->state = TADLEXER_LE_STATE_LSEGLEN_SECOND;
		lexer->buf.type = TADLEXER_LE_RESULTTYPE_READING_SEGMENT;
		break;
	case TADLEXER_LE_STATE_LSEGLEN_SECOND:
		lexer->len |= b << 8;
		lexer->state = TADLEXER_LE_STATE_LSEGLEN_THIRD;
		lexer->buf.type = TADLEXER_LE_RESULTTYPE_READING_SEGMENT;
		break;
	case TADLEXER_LE_STATE_LSEGLEN_THIRD:
		lexer->len |= b << 16;
		lexer->state = TADLEXER_LE_STATE_LSEGLEN_FORTH;
		lexer->buf.type = TADLEXER_LE_RESULTTYPE_READING_SEGMENT;
		break;
	case TADLEXER_LE_STATE_LSEGLEN_FORTH:
		lexer->len |= b << 24;
		lexer->state = TADLEXER_LE_STATE_DATA;
		lexer->len_read = 0;
		lexer->buf.type = TADLEXER_LE_RESULTTYPE_READING_SEGMENT;
		lexer->buf.flag |= TADLEXER_LE_FLAG_LENGTH_DETERMINE;
		lexer->buf.val.seg.id = lexer->segid;
		lexer->buf.val.seg.len = lexer->len;
		break;
	case TADLEXER_LE_STATE_DATA:
		lexer->buf.flag |= TADLEXER_LE_FLAG_READING_DATA;
		lexer->len_read++;
		if (lexer->len_read == lexer->len) {
			lexer->state = TADLEXER_LE_STATE_START;
			lexer->buf.type = TADLEXER_LE_RESULTTYPE_SEGMENT_END;
		} else {
			lexer->buf.type = TADLEXER_LE_RESULTTYPE_READING_SEGMENT;
		}
		break;
	}
}

EXPORT VOID tadlexer_le_initialize(tadlexer_le_t *lexer)
{
	lexer->state = TADLEXER_LE_STATE_START;
	lexer->segid = 0;
	lexer->len = 0;
}

EXPORT VOID tadlexer_le_finalize(tadlexer_le_t *lexer)
{
}

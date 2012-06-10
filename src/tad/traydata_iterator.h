/*
 * traydata_iterator.h
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
/* Functionality name: traydata */
/* Detail name: iterator */

#include    <basic.h>
#include    <btron/hmi.h>

#include    "tadlexer_le.h"

#ifndef __TRAYDATA_ITERATOR_H__
#define __TRAYDATA_ITERATOR_H__

/* Functionality name: traydata */
/* Detail name: iterator */
struct traydata_iterator_t_ {
	enum {
		TRAYDATA_ITERATOR_STATE_REC_START,
		TRAYDATA_ITERATOR_STATE_REC_UNITE,
		TRAYDATA_ITERATOR_STATE_REC_UNITE_VSEG_INFO,
		TRAYDATA_ITERATOR_STATE_REC_UNITE_VSEG_DATA,
		TRAYDATA_ITERATOR_STATE_REC_VOBJ_DEVIDE,
		TRAYDATA_ITERATOR_STATE_REC_DEVIDE,
	} state;
	TRAYREC *rec;
	W recnum;
	W i_rec;
	union {
		struct {
			W id;
			tadlexer_le_t lexer;
			W rec_read_len;
		} unite;
		struct {
			W read_len;
			UW whole_len;
		} devide_vobj;
		struct {
			W id;
			UW whole_len;
		} devide;
	} status;
};
typedef struct traydata_iterator_t_ traydata_iterator_t;

/* Functionality name: traydata */
/* Detail name: iterator */
/* Data structure identifier: result */
struct traydata_iterator_result_ {
	enum {
		TRAYDATA_ITERATOR_RESULTTYPE_FIXED_SEGMENT,
		TRAYDATA_ITERATOR_RESULTTYPE_VARIABLE_SEGMENT_CONT,
		TRAYDATA_ITERATOR_RESULTTYPE_VARIABLE_SEGMENT,
		TRAYDATA_ITERATOR_RESULTTYPE_VOBJREC,
		TRAYDATA_ITERATOR_RESULTTYPE_END,
	} type;
	union {
		TC ch;
		struct {
			UB id;
			UW seglen;
			UB *chunk_data;
			UW chunk_data_len;
		} seg;
		TR_VOBJREC vobjrec;
	} val;
};
typedef struct traydata_iterator_result_ traydata_iterator_result;

IMPORT VOID traydata_iterator_initialize(traydata_iterator_t *iterator, TRAYREC *rec, W recnum);
IMPORT VOID traydata_iterator_finalize(traydata_iterator_t *iterator);
IMPORT Bool traydata_iterator_next(traydata_iterator_t *iterator, traydata_iterator_result *result);

#endif

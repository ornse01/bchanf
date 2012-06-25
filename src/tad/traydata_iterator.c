/*
 * traydata_iterator.c
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

#include "traydata_iterator.h"

#include	<basic.h>
#include	<stdio.h>

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

/* return; -1:error, 0:continue, 1:break, 2:break increment */
LOCAL W traydata_iterator_inputTRAYREC(traydata_iterator_t *iterator, TRAYREC *rec, traydata_iterator_result *result)
{
	W i, lex_result_len, ret = -1;
	tadlexer_le_result *lex_result;
	UB ch;
	Bool ok, vseg, segend, fixed, segdata;

	switch (iterator->state) {
	case TRAYDATA_ITERATOR_STATE_REC_START:
		DP_STATE("TRAYDATA_ITERATOR_STATE_REC_START");
		if ((rec->id == TR_TEXT)||(rec->id == TR_FIG)||(rec->id == (TR_TEXT|TR_CONT))||(rec->id == (TR_FIG|TR_CONT))) {
			iterator->state = TRAYDATA_ITERATOR_STATE_REC_UNITE;
			iterator->status.unite.id = rec->id & 0xFF;
			tadlexer_le_initialize(&iterator->status.unite.lexer);
			iterator->status.unite.rec_read_len = 0;
		} else if (rec->id == TR_VOBJ) {
			result->type = TRAYDATA_ITERATOR_RESULTTYPE_VOBJREC;
			result->val.vobj.len = rec->len;
			result->val.vobj.chunk_data = rec->dt;	
			result->val.vobj.chunk_data_len = rec->len;
			ret = 2;
			break;
		} else if (rec->id == (TR_VOBJ|TR_CONT)) {
			iterator->status.devide_vobj.read_len = rec->len;
			iterator->state = TRAYDATA_ITERATOR_STATE_REC_VOBJ_DEVIDE;
			result->type = TRAYDATA_ITERATOR_RESULTTYPE_VOBJREC_CONT;
			result->val.vobj.len = 0; /* TODO */
			result->val.vobj.chunk_data = rec->dt;	
			result->val.vobj.chunk_data_len = rec->len;
			ret = 2;
			break;
		} else if ((rec->id & TR_CONT) != 0) {
			iterator->state = TRAYDATA_ITERATOR_STATE_REC_DEVIDE;
			result->type = TRAYDATA_ITERATOR_RESULTTYPE_VARIABLE_SEGMENT_CONT;
			result->val.seg.id = rec->id;
			result->val.seg.seglen = rec->len;
			result->val.seg.chunk_data = rec->dt;
			result->val.seg.chunk_data_len = rec->len;
			ret = 2;
			break;
		} else {
			result->type = TRAYDATA_ITERATOR_RESULTTYPE_VARIABLE_SEGMENT;
			result->val.seg.id = rec->id;
			result->val.seg.seglen = rec->len;
			result->val.seg.chunk_data = rec->dt;
			result->val.seg.chunk_data_len = rec->len;
			ret = 2;
			break;
		}
		/* intentional */
	case TRAYDATA_ITERATOR_STATE_REC_UNITE:
		DP_STATE("TRAYDATA_ITERATOR_STATE_REC_UNITE");
		i = iterator->status.unite.rec_read_len;
		vseg = False;
		fixed = False;
		for (;i < rec->len;) {
			ch = rec->dt[i];
			i++;
			tadlexer_le_inputbyte(&iterator->status.unite.lexer, ch, &lex_result, &lex_result_len);
			ok = tadlexer_le_result_is_fixedsegment(lex_result);
			if (ok != False) {
				fixed = True;
				break;
			}
			ok = tadlexer_le_result_is_variablesegment(lex_result);
			if (ok != False) {
				vseg = True;
				break;
			}
		}
		iterator->status.unite.rec_read_len = i;
		if (vseg != False) {
			iterator->state = TRAYDATA_ITERATOR_STATE_REC_UNITE_VSEG_INFO;
		} else if (fixed != False) {
			result->type = TRAYDATA_ITERATOR_RESULTTYPE_FIXED_SEGMENT;
			result->val.ch = lex_result->val.ch;
			ret = 1;
			break;
		} else if (i == rec->len) {
			iterator->status.unite.rec_read_len = 0;
			if (iterator->status.unite.id == rec->id) {
				iterator->state = TRAYDATA_ITERATOR_STATE_REC_START;
				tadlexer_le_finalize(&iterator->status.unite.lexer);
			}
			ret = 0;
			break;
		} else {
			DP(("error condition\n"));
		}
		/* intentional */
	case TRAYDATA_ITERATOR_STATE_REC_UNITE_VSEG_INFO:
		DP_STATE("TRAYDATA_ITERATOR_STATE_REC_UNITE_VSEG_INFO");
		i = iterator->status.unite.rec_read_len;
		segend = False;
		segdata = False;
		for (;i < rec->len;) {
			ch = rec->dt[i];
			i++;
			tadlexer_le_inputbyte(&iterator->status.unite.lexer, ch, &lex_result, &lex_result_len);
			if (lex_result->type == TADLEXER_LE_RESULTTYPE_SEGMENT_END) {
				segend = True;
				break;
			}
			ok = tadlexer_le_result_is_lengthdetermined(lex_result);
			if (ok != False) {
				segdata = True;
				break;
			}
		}
		iterator->status.unite.rec_read_len = i;
		if (segend != False) {
			iterator->state = TRAYDATA_ITERATOR_STATE_REC_UNITE;
			result->type = TRAYDATA_ITERATOR_RESULTTYPE_VARIABLE_SEGMENT;
			result->val.seg.id = lex_result->val.seg.id;
			result->val.seg.seglen = lex_result->val.seg.len;
			result->val.seg.chunk_data = NULL;
			result->val.seg.chunk_data_len = 0;
			ret = 1;
			break;
		} else if (segdata != False) {
			iterator->state = TRAYDATA_ITERATOR_STATE_REC_UNITE_VSEG_DATA;
		} else if (i == rec->len) {
			if (iterator->status.unite.id == rec->id) {
				iterator->state = TRAYDATA_ITERATOR_STATE_REC_START;
				tadlexer_le_finalize(&iterator->status.unite.lexer);
			}
			iterator->status.unite.rec_read_len = 0;
			ret = 0;
			break;
		} else {
			DP(("error condition\n"));
		}
		/* intentional */
	case TRAYDATA_ITERATOR_STATE_REC_UNITE_VSEG_DATA:
		DP_STATE("TRAYDATA_ITERATOR_STATE_REC_UNITE_VSEG_DATA");
		i = iterator->status.unite.rec_read_len;
		result->val.seg.chunk_data = rec->dt + i;
		for (;i < rec->len;) {
			ch = rec->dt[i];
			i++;
			tadlexer_le_inputbyte(&iterator->status.unite.lexer, ch, &lex_result, &lex_result_len);
			if (lex_result->type == TADLEXER_LE_RESULTTYPE_SEGMENT_END) {
				break;
			}
		}
		if (lex_result->type == TADLEXER_LE_RESULTTYPE_SEGMENT_END) {
			if (rec->len == i) {
				iterator->state = TRAYDATA_ITERATOR_STATE_REC_START;
			} else {
				iterator->state = TRAYDATA_ITERATOR_STATE_REC_UNITE;
			}
			result->type = TRAYDATA_ITERATOR_RESULTTYPE_VARIABLE_SEGMENT;
		} else {
			result->type = TRAYDATA_ITERATOR_RESULTTYPE_VARIABLE_SEGMENT_CONT;
		}
		result->val.seg.id = lex_result->val.seg.id;
		result->val.seg.seglen = lex_result->val.seg.len;
		result->val.seg.chunk_data_len = i - iterator->status.unite.rec_read_len;
		if (rec->len == i) {
			iterator->status.unite.rec_read_len = 0;
			ret = 2;
		} else {
			iterator->status.unite.rec_read_len = i;
			ret = 1;
		}
		break;
	case TRAYDATA_ITERATOR_STATE_REC_VOBJ_DEVIDE:
		DP_STATE("TRAYDATA_ITERATOR_STATE_REC_VOBJ_DEVIDE");
		if (rec->id == TR_VOBJ) {
			iterator->state = TRAYDATA_ITERATOR_STATE_REC_START;
			result->type = TRAYDATA_ITERATOR_RESULTTYPE_VOBJREC;
		} else {
			result->type = TRAYDATA_ITERATOR_RESULTTYPE_VOBJREC_CONT;
		}
		result->val.vobj.len = 0; /* TODO */
		result->val.vobj.chunk_data = rec->dt;	
		result->val.vobj.chunk_data_len = rec->len;
		iterator->status.devide_vobj.read_len = rec->len;
		ret = 2;
		break;
	case TRAYDATA_ITERATOR_STATE_REC_DEVIDE:
		DP_STATE("TRAYDATA_ITERATOR_STATE_REC_DEVIDE");
		if (rec->id == iterator->status.devide.id) {
			result->type = TRAYDATA_ITERATOR_RESULTTYPE_VARIABLE_SEGMENT;
		} else {
			result->type = TRAYDATA_ITERATOR_RESULTTYPE_VARIABLE_SEGMENT_CONT;
		}
		result->val.seg.id = iterator->status.devide.id;
		result->val.seg.seglen = -1; /* TODO */
		result->val.seg.chunk_data = rec->dt;
		result->val.seg.chunk_data_len = rec->len;
		ret = 1;
		break;
	}

	return ret;
}

EXPORT Bool traydata_iterator_next(traydata_iterator_t *iterator, traydata_iterator_result *result)
{
	TRAYREC *rec;
	W ret;

	for (; iterator->i_rec < iterator->recnum; iterator->i_rec++) {
		rec = iterator->rec + iterator->i_rec;
		ret = traydata_iterator_inputTRAYREC(iterator, rec, result);
		if (ret < 0) {
			return False; /* TODO: error*/
		} else if (ret == 1) {
			return True;
		} else if (ret == 2) {
			iterator->i_rec++;
			return True;
		}
	}

	return False;
}

EXPORT VOID traydata_iterator_initialize(traydata_iterator_t *iterator, TRAYREC *rec, W recnum)
{
	iterator->state = TRAYDATA_ITERATOR_STATE_REC_START;
	iterator->rec = rec;
	iterator->recnum = recnum;
	iterator->i_rec = 0;
}

EXPORT VOID traydata_iterator_finalize(traydata_iterator_t *iterator)
{
}

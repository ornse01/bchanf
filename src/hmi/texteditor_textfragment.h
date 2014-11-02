/*
 * texteditor_textfragment.h
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

#include    <basic.h>

#include	<tad/tadfragment.h>
#include	"texteditor_characterstate.h"
#include	"texteditor_insertfilter.h"

#ifndef __TEXTEDITOR_TEXTFRAGMENT_H__
#define __TEXTEDITOR_TEXTFRAGMENT_H__

/* Functionality name: texteditor */
/* Detail name: textfragment */
struct texteditor_textfragment_t_ {
	tadfragment_t base;
	W *pos;
	W pos_len;
};
typedef struct texteditor_textfragment_t_ texteditor_textfragment_t;

IMPORT W texteditor_textfragment_initialize(texteditor_textfragment_t *fragment);
IMPORT VOID texteditor_textfragment_finalize(texteditor_textfragment_t *fragment);
IMPORT UB* texteditor_textfragment_getbuffer(texteditor_textfragment_t *fragment);
IMPORT W texteditor_textfragment_getsegmentlength(texteditor_textfragment_t *fragment);
IMPORT W texteditor_textfragment_getbufferlength(texteditor_textfragment_t *fragment);
IMPORT W texteditor_textfragment_getcharlength(texteditor_textfragment_t *fragment);

/* Functionality name: texteditor */
/* Detail name: insertcontext */
struct texteditor_insertcontext_t_ {
	texteditor_textfragment_t *target;
	tadfragment_cursor_t target_cursor;
	struct {
		tadfragment_t fragment;
		tadfragment_cursor_t cursor;
	} dest;
	texteditor_characterstate_t state;
	struct {
		tadlangcode lang;
		Bool is_hankaku;
	} pos_state;
	texteditor_insertfilter_t filter;
	GID gid;
};
typedef struct texteditor_insertcontext_t_ texteditor_insertcontext_t;

IMPORT W texteditor_insertcontext_initialize(texteditor_insertcontext_t *ctx, texteditor_textfragment_t *target, GID gid, W pos);
IMPORT W texteditor_insertcontext_finalize(texteditor_insertcontext_t *ctx);
IMPORT W texteditor_insertcontext_insert(texteditor_insertcontext_t *ctx, tadsegment *segment);

#endif

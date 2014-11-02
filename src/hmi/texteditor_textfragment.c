/*
 * texteditor_textfragment.c
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

#include	"texteditor_textfragment.h"

#include	<bstdio.h>

#include	<tad/tadfragment.h>
#include	<tad/tadsegment.h>
#include	"texteditor_characterstate.h"
#include	"texteditor_insertfilter.h"

#ifdef BCHAN_CONFIG_DEBUG
# define DP(arg) printf arg
# define DP_ER(msg, err) printf("%s (%d/%x)\n", msg, err>>16, err)
#else
# define DP(arg) /**/
# define DP_ER(msg, err) /**/
#endif

EXPORT UB* texteditor_textfragment_getbuffer(texteditor_textfragment_t *fragment)
{
	return tadfragment_getbuffer(&fragment->base);
}

EXPORT W texteditor_textfragment_getsegmentlength(texteditor_textfragment_t *fragment)
{
	return tadfragment_getsegmentlength(&fragment->base);
}

EXPORT W texteditor_textfragment_getbufferlength(texteditor_textfragment_t *fragment)
{
	return tadfragment_getbufferlength(&fragment->base);
}

EXPORT W texteditor_textfragment_getcharlength(texteditor_textfragment_t *fragment)
{
	tadfragment_cursor_t cursor;
	tadsegment segment;
	W i = 0, err;

	tadfragment_cursor_initialize(&cursor, &fragment->base);

	for (;;) {
		err = tadfragment_cursor_getdata(&cursor, &segment);
		if (err < 0) {
			break;
		}

		if (segment.type == TADSEGMENT_TYPE_CHARACTOR) {
			i++;
		}

		err = tadfragment_cursor_move(&cursor, 1);
		if (err < 0) {
			break;
		}
	}

	tadfragment_cursor_finalize(&cursor);

	return i;
}

EXPORT W texteditor_textfragment_initialize(texteditor_textfragment_t *fragment)
{
	return tadfragment_initialize(&fragment->base);
}

EXPORT VOID texteditor_textfragment_finalize(texteditor_textfragment_t *fragment)
{
	tadfragment_finalize(&fragment->base);
}

/* texteditor_insertcontext */

LOCAL W texteditor_insertcontext_bufferprepair(tadfragment_cursor_t *src, texteditor_characterstate_t *state, tadfragment_cursor_t *dest, W pos)
{
	W i, err;
	tadsegment segment;

	i = 0;
	for (; i < pos;) {
		err = tadfragment_cursor_getdata(src, &segment);
		if (err < 0) {
			break;
		}
		texteditor_charactorstate_input(state, &segment);

		if (segment.type == TADSEGMENT_TYPE_CHARACTOR) {
			i++;
		}

		err = tadfragment_cursor_insertsegment(dest, &segment);
		if (err < 0) {
			break;
		}

		err = tadfragment_cursor_move(src, 1);
		if (err < 0) {
			break;
		}
	}

	if (i != pos) {
		return -1;
	}

	return 0;
}

LOCAL W texteditor_insertcontext_bufferappend(texteditor_characterstate_t *state, texteditor_insertfilterresult_t *result, tadfragment_cursor_t *dest)
{
	tadsegment *segment;
	W err = 0;
	Bool cont;

	for (;;) {
		cont = texteditor_insertfilterresult_next(result, &segment);
		if (cont == False) {
			break;
		}

		err = tadfragment_cursor_insertsegment(dest, segment);
		if (err < 0) {
			break;
		}

		tadfragment_cursor_getdata(dest, segment);
		texteditor_charactorstate_input(state, segment);
	}

	return err;
}

LOCAL W texteditor_insertcontext_bufferafterappend(texteditor_characterstate_t *state, tadfragment_cursor_t *src, tadfragment_cursor_t *dest)
{
	tadsegment segment;
	W err = 0;
	Bool is_end;

	for (;;) {
		is_end = tadfragment_cursor_isend(src);
		if (is_end != False) {
			break;
		}

		err = tadfragment_cursor_getdata(src, &segment);
		if (err < 0) {
			break;
		}
		texteditor_charactorstate_input(state, &segment);

		err = tadfragment_cursor_insertsegment(dest, &segment);
		if (err < 0) {
			break;
		}

		err = tadfragment_cursor_move(src, 1);
		if (err < 0) {
			break;
		}
	}

	return err;
}

EXPORT W texteditor_insertcontext_insert(texteditor_insertcontext_t *ctx, tadsegment *segment)
{
	texteditor_insertfilterresult_t *result;
	texteditor_insertfilter_put(&ctx->filter, segment, &result);
	return texteditor_insertcontext_bufferappend(&ctx->state, result, &ctx->dest.cursor);
}

EXPORT W texteditor_insertcontext_initialize(texteditor_insertcontext_t *ctx, texteditor_textfragment_t *target, GID gid, W pos)
{
	W err;

	ctx->target = target;
	ctx->gid = gid;

	tadfragment_cursor_initialize(&ctx->target_cursor, &ctx->target->base);
	err = tadfragment_initialize(&ctx->dest.fragment);
	if (err < 0) {
		goto error_dest_fragment;
	}
	tadfragment_cursor_initialize(&ctx->dest.cursor, &ctx->dest.fragment);

	texteditor_characterstate_initialize(&ctx->state);

	err = texteditor_insertcontext_bufferprepair(&ctx->target_cursor, &ctx->state, &ctx->dest.cursor, pos);
	if (err < 0) {
		goto error_prepair;
	}

	ctx->pos_state.is_hankaku = texteditor_characterstate_ishankaku(&ctx->state);
	texteditor_characterstate_getlangcode(&ctx->state, &ctx->pos_state.lang);

	texteditor_insertfilter_initialize(&ctx->filter, &ctx->pos_state.lang, (ctx->pos_state.is_hankaku != False) ? 0x0102 : 0x0101);

	return 0;

error_prepair:
	texteditor_characterstate_finalize(&ctx->state);
	tadfragment_cursor_finalize(&ctx->dest.cursor);
	tadfragment_finalize(&ctx->dest.fragment);
error_dest_fragment:
	tadfragment_cursor_finalize(&ctx->target_cursor);

	return err;
}

EXPORT W texteditor_insertcontext_finalize(texteditor_insertcontext_t *ctx)
{
	W len, err = 0;
	UB *data;
	texteditor_insertfilterresult_t *result;

	texteditor_insertfilter_endinput(&ctx->filter, &result);
	err = texteditor_insertcontext_bufferappend(&ctx->state, result, &ctx->dest.cursor);
	if (err < 0) {
		tadfragment_cursor_finalize(&ctx->dest.cursor);
		tadfragment_cursor_finalize(&ctx->target_cursor);
		goto finalize;
	}

	err = texteditor_insertcontext_bufferafterappend(&ctx->state, &ctx->target_cursor, &ctx->dest.cursor);
	if (err < 0) {
		tadfragment_cursor_finalize(&ctx->dest.cursor);
		tadfragment_cursor_finalize(&ctx->target_cursor);
		goto finalize;
	}

	tadfragment_cursor_finalize(&ctx->dest.cursor);
	tadfragment_cursor_finalize(&ctx->target_cursor);

	tadfragment_truncate(&ctx->target->base, 0);
	data = tadfragment_getbuffer(&ctx->dest.fragment);
	len = tadfragment_getbufferlength(&ctx->dest.fragment);
	err = tadfragment_pushback(&ctx->target->base, data, len);

finalize:

	texteditor_insertfilter_finalize(&ctx->filter);
	texteditor_characterstate_finalize(&ctx->state);
	tadfragment_finalize(&ctx->dest.fragment);

	return err;
}

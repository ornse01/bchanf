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
	tadfragment_cursor_segment segment;

	i = 0;
	for (; i < pos;) {
		err = tadfragment_cursor_getdata(src, &segment);
		if (err < 0) {
			break;
		}
		texteditor_charactorstate_input(state, &segment);

		if (segment.type == TADFRAGMENT_CURSOR_SEGMENTTYPE_CHAR) {
			i++;
		}

		err = tadfragment_cursor_insert(dest, segment.p, segment.len);
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

LOCAL W texteditor_insertcontext_bufferappend(texteditor_characterstate_t *state, tadlangcode *lang, Bool is_hankaku, UB *insert_data, W insert_len, tadfragment_cursor_t *dest)
{
	texteditor_insertfilter_t filter;
	texteditor_insertfilter_result filter_result;
	tadfragment_cursor_segment segment;
	W err = 0;
	Bool cont;

	texteditor_insertfilter_initialize(&filter, lang, (is_hankaku != False) ? 0x0102 : 0x0101, insert_data, insert_len);
	for (;;) {
		cont = texteditor_insertfilter_next(&filter, &filter_result);
		if (cont == False) {
			break;
		}

		err = tadfragment_cursor_insert(dest, filter_result.data, filter_result.len);
		if (err < 0) {
			break;
		}

		tadfragment_cursor_getdata(dest, &segment);
		texteditor_charactorstate_input(state, &segment);
	}
	texteditor_insertfilter_finalize(&filter);

	return err;
}

LOCAL W texteditor_insertcontext_bufferafterappend(texteditor_characterstate_t *state, tadfragment_cursor_t *src, tadfragment_cursor_t *dest)
{
	tadfragment_cursor_segment segment;
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

		err = tadfragment_cursor_insert(dest, segment.p, segment.len);
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

EXPORT W texteditor_insertcontext_insert(texteditor_insertcontext_t *ctx, UB *data, W len)
{
	tadlangcode lang;
	Bool is_hankaku;

	is_hankaku = texteditor_characterstate_ishankaku(&ctx->state);
	texteditor_characterstate_getlangcode(&ctx->state, &lang);

	return texteditor_insertcontext_bufferappend(&ctx->state, &lang, is_hankaku, data, len, &ctx->dest.cursor);
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

	texteditor_characterstate_finalize(&ctx->state);
	tadfragment_finalize(&ctx->dest.fragment);

	return err;
}

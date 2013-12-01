/*
 * texteditor_characterstate.h
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

#include	"texteditor_characterstate.h"

#include	<bstdio.h>

#ifdef BCHAN_CONFIG_DEBUG
# define DP(arg) printf arg
# define DP_ER(msg, err) printf("%s (%d/%x)\n", msg, err>>16, err)
#else
# define DP(arg) /**/
# define DP_ER(msg, err) /**/
#endif

EXPORT W texteditor_charactorstate_input(texteditor_characterstate_t *state, tadfragment_cursor_segment *segment)
{
	TC ch;
	W iter;
	UB segid, subid;

	if (segment->type == TADFRAGMENT_CURSOR_SEGMENTTYPE_VARIABLE) {
		if (segment->len != 10) {
			return 0;
		}
		segid = *(TC*)segment->p & 0xFF;
		if (segid != TS_TFONT) {
			return 0;
		}
		subid = *(TC*)(segment->p + 4) >> 8;
		if (subid != 3) {
			return 0;
		}
		state->w_ratio = *(RATIO*)(segment->p + 8);
	} else if (segment->type == TADFRAGMENT_CURSOR_SEGMENTTYPE_LANGCODE) {
		ch = *(TC*)(segment->p + segment->len - 2);
		iter = segment->len - 2;
		if ((ch & 0xFE00) == 0xFE00) {
			iter++;
		}
		state->lang.iter = iter - 1;
		state->lang.code = 0x00FF & ch;
	}
	return 0;
}

EXPORT Bool texteditor_characterstate_ishankaku(texteditor_characterstate_t *state)
{
	W ratio_a, ratio_b;

	ratio_a = state->w_ratio >> 8;
	ratio_b = state->w_ratio & 0xFF;
	if ((ratio_a * 2 > ratio_b)||(ratio_b == 0)) {
		return False;
	} else {
		return True;
	}
}

EXPORT W texteditor_characterstate_getlang(texteditor_characterstate_t *state, TC *str, W len)
{
	W i, req_len;

	req_len = 2 + (state->lang.iter+1)/2*2;

	if (str == NULL || len < 0) {
		return req_len / sizeof(TC);
	}

	for (i = 0; i < (state->lang.iter + 1) / 2; i++) {
		str[i] = 0xFEFE;
	}
	if (state->lang.iter % 2 == 1) {
		str[i] = state->lang.code;
	} else {
		str[i] = 0xFE00 | state->lang.code;
	}

	return req_len / sizeof(TC);
}

EXPORT Bool texteditor_characterstate_islang(texteditor_characterstate_t *state, TC *str, W len)
{
	TC ch;
	W iter;
	UH code;

	ch = str[len - 1];
	iter = (len - 1) * 2;
	if ((ch & 0xFE00) == 0xFE00) {
		iter++;
	}
	iter--;
	code = 0x00FF & ch;

	if ((state->lang.iter == iter)&&(state->lang.code == code)) {
		return True;
	}
	return False;
}

EXPORT VOID texteditor_characterstate_initialize(texteditor_characterstate_t *state)
{
	state->w_ratio = 0x0101;
	state->lang.iter = 0;
	state->lang.code = 0x21; // system script
}

EXPORT VOID texteditor_characterstate_finalize(texteditor_characterstate_t *state)
{
}

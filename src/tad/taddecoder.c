/*
 * taddecoder.c
 *
 * Copyright (c) 2014 project bchan
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

#include "taddecoder.h"

#include	<basic.h>
#include	<bstdio.h>
#include	<btron/dp.h>
#include    <tad.h>

#include    "tadlangcode.h"
#include    "tadsegment.h"

#ifdef BCHAN_CONFIG_DEBUG
# define DP(arg) printf arg
# define DP_ER(msg, err) printf("%s (%d/%x)\n", msg, err>>16, err)
#else
# define DP(arg) /**/
# define DP_ER(msg, err) /**/
#endif

EXPORT Bool taddecoder_next(taddecoder_t *iterator, tadsegment *result)
{
	TC *ch;
	W rawlen_by_TC;
	LTADSEG *seg0;
	tadlangcode_parser_t langparser;
	TADLANGCODE_PARSER_RESULT langparser_result;

	ch = iterator->bin + iterator->index;

	if (iterator->index >= iterator->len) {
		return False;
	}

	if ((*ch & 0xFF80) == 0xFF80) {
		result->type = TADSEGMENT_TYPE_VARIABLE;
		result->value.variable.raw = (UB*)ch;
		result->value.variable.segid = *ch & 0xFF;
		seg0 = (LTADSEG*)ch;
		if (seg0->len == 0xffff) {
			rawlen_by_TC = 1 + seg0->llen / 2 + 3;
			result->value.variable.seglen = seg0->llen;
			result->value.variable.segdata = ((UB*)seg0) + 8;
		} else {
			rawlen_by_TC = 1 + seg0->len / 2 + 1;
			result->value.variable.seglen = seg0->len;
			result->value.variable.segdata = ((UB*)seg0) + 4;
		}
		iterator->index += rawlen_by_TC;
		result->value.variable.rawlen = rawlen_by_TC * sizeof(TC);
	} else if ((*ch & 0xFF00) == 0xFE00) {
		result->type = TADSEGMENT_TYPE_LANGCODE;
		tadlangcode_parser_initialize(&langparser);
		for (;;) {
			langparser_result = tadlangcode_parser_inputchar(&langparser, *ch);
			iterator->index++;
			if (langparser_result == TADLANGCODE_PARSER_RESULT_DETERMINED) {
				break;
			} else if (langparser_result == TADLANGCODE_PARSER_RESULT_INVALID) {
				tadlangcode_parser_finalize(&langparser);
				return False;
			}
			ch = iterator->bin + iterator->index;
		}
		tadlangcode_parser_getlangcode(&langparser, &result->value.lang);
		tadlangcode_parser_finalize(&langparser);
	} else {
		result->type = TADSEGMENT_TYPE_CHARACTOR;
		result->value.ch = *ch;
		iterator->index++;
	}

	return True;
}

EXPORT VOID taddecoder_initialize(taddecoder_t *iterator, TC *bin, W strlen)
{
	iterator->bin = bin;
	iterator->len = strlen;
	iterator->index = 0;
}

EXPORT VOID taddecoder_finalize(taddecoder_t *iterator)
{
}

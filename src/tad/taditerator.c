/*
 * taditerator.c
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

#include "taditerator.h"

#include	<basic.h>
#include	<bstdio.h>

EXPORT TADITERATOR_RESULTTYPE taditerator_next(taditerator_t *iterator, TC **pos, TC *segment, LTADSEG **seg, W *segsize, UB **data)
{
	TC *ch, *dummy_pos;
	LTADSEG *seg0;

	ch = iterator->bin + iterator->index;
	if (pos == NULL) {
		pos = &dummy_pos;
	}

	if (iterator->index >= iterator->len) {
		*pos = ch;
		return TADITERATOR_RESULTTYPE_END;
	}

	if ((*ch & 0xFF80) == 0xFF80) {
		*pos = ch;
		*segment = *ch & 0xFF;
		seg0 = (LTADSEG*)ch;
		if (seg0->len == 0xffff) {
			iterator->index += 1 + seg0->llen / 2 + 3;
			*segsize = seg0->llen;
			*data = ((UB*)seg0) + 8;
			*seg = seg0;
		} else {
			iterator->index += 1 + seg0->len / 2 + 1;
			*segsize = seg0->len;
			*data = ((UB*)seg0) + 4;
			*seg = seg0;
		}
		return TADITERATOR_RESULTTYPE_SEGMENT;
	} else {
		*pos = ch;
		*segment = *ch;
		*seg = NULL;
		*segsize = 2;
		*data = NULL;
		iterator->index++;
		return TADITERATOR_RESULTTYPE_CHARCTOR;
	}
}

EXPORT VOID taditerator_next2(taditerator_t *iterator, taditerator_result *result)
{
	result->type = taditerator_next(iterator, &result->pos, &result->segment, &result->vseg.l, &result->segsize, &result->data);
}

EXPORT VOID taditerator_initialize(taditerator_t *iterator, TC *bin, W strlen)
{
	iterator->bin = bin;
	iterator->len = strlen;
	iterator->index = 0;
}

EXPORT VOID taditerator_finalize(taditerator_t *iterator)
{
}

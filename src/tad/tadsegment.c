/*
 * tadsegment.c
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

#include "tadsegment.h"

#include	<basic.h>
#include	<btron/dp.h>
#include    <tad.h>

EXPORT W tadsegment_getvariable(tadsegment *segment, UB *segid, UW *seglen, UB **segdata)
{
	LTADSEG *seg;
	TC *ch;

	if (segment->type != TADSEGMENT_TYPE_VARIABLE) {
		return -1; /* TODO */
	}

	ch = (TC*)segment->value.variable.raw;

	*segid = *ch & 0xFF;
	seg = (LTADSEG*)ch;
	if (seg->len == 0xffff) {
		*seglen = seg->llen;
		*segdata = ((UB*)seg) + 8;
	} else {
		*seglen = seg->len;
		*segdata = ((UB*)seg) + 4;
	}

	return 0;
}

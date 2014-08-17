/*
 * tadsegment.h
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

#include    <basic.h>

#include	"tadlangcode.h"

#ifndef __TADSEGMENT_H__
#define __TADSEGMENT_H__

struct tadsegment_ {
	enum TADSEGMENT_TYPE {
		TADSEGMENT_TYPE_VARIABLE,
		TADSEGMENT_TYPE_CHARACTOR,
		TADSEGMENT_TYPE_LANGCODE,
	} type;
	union {
		struct {
			UB *raw;
			W rawlen;
			UB segid;
			UW seglen;
			UB *segdata;
		} variable;
		TC ch;
		tadlangcode lang;
	} value;
};
typedef struct tadsegment_ tadsegment;

IMPORT W tadsegment_getvariable(tadsegment *segment, UB *segid, UW *seglen, UB **segdata);

#endif

/*
 * arraybase.h
 *
 * Copyright (c) 2010-2013 project bchan
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
/* Functionality name: array */
/* Detail name: */

#include	<basic.h>
#include	<bsys/queue.h>

#ifndef __ARRAYBASE_H__
#define __ARRAYBASE_H__

/* Functionality name: arraybase */
/* Detail name: datanode */
struct arraybase_datanode_t_ {
	QUEUE queue;
	UB *data;
};
typedef struct arraybase_datanode_t_ arraybase_datanode_t;

/* Functionality name: arraybase */
/* Detail name: */
struct arraybase_t_ {
	W unitsize;
	W denom;
	W datanum;
	arraybase_datanode_t datalist;
};
typedef struct arraybase_t_ arraybase_t;

IMPORT W arraybase_initialize(arraybase_t *arraybase, W unitsize, W denom);
IMPORT VOID arraybase_finalize(arraybase_t *arraybase);
IMPORT Bool arraybase_getunitbyindex(arraybase_t *arraybase, W index, VP *p);
IMPORT Bool arraybase_getunitfirst(arraybase_t *arraybase, VP *p);
IMPORT Bool arraybase_getunitlast(arraybase_t *arraybase, VP *p);
IMPORT W arraybase_appendunit(arraybase_t *arraybase, VP p);
IMPORT VOID arraybase_truncate(arraybase_t *arraybase, W newlength);
IMPORT W arraybase_length(arraybase_t *arraybase);

#endif

/*
 * idtocb.h
 *
 * Copyright (c) 2012-2013 project bchan
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
/* Functionality name: idtocb */
/* Detail name: */

#include    <basic.h>

/* controll block manager */
/*  relation id and block */

#ifndef __IDTOCB_H__
#define __IDTOCB_H__

/* Functionality name: idtocb */
/* Detail name: entry */
struct idtocb_entry_t_ {
	ID id;
};
typedef struct idtocb_entry_t_ idtocb_entry_t;

/* Functionality name: idtocb */
/* Detail name: */
typedef struct idtocb_t_ idtocb_t;

IMPORT idtocb_t* idtocb_new(W entry_size, W max_entries);
IMPORT VOID idtocb_delete(idtocb_t *manager);
IMPORT ID idtocb_allocate(idtocb_t *manager);
IMPORT VOID idtocb_free(idtocb_t *manager, ID id);
IMPORT W idtocb_getcontrolblock(idtocb_t *manager, ID id, idtocb_entry_t **ptr);

/* Functionality name: idtocb */
/* Detail name: iterator */
struct idtocb_iterator_t_ {
	idtocb_t *manager;
	W i;
};
typedef struct idtocb_iterator_t_ idtocb_iterator_t;

IMPORT VOID idtocb_iterator_initialize(idtocb_iterator_t *iter, idtocb_t *manager);
IMPORT VOID idtocb_iterator_finalize(idtocb_iterator_t *iter);
IMPORT Bool idtocb_iterator_next(idtocb_iterator_t *iter, idtocb_entry_t **entry);

#endif

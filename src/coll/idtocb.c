/*
 * idtocb.c
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

#include "idtocb.h"

#include	<basic.h>
#include	<bstdio.h>
#include	<bstdlib.h>
#include	<errcode.h>

#ifdef BCHAN_CONFIG_DEBUG
# define DP(arg) printf arg
# define DP_ER(msg, err) printf("%s (%d/%x)\n", msg, err>>16, err)
#else
# define DP(arg) /**/
# define DP_ER(msg, err) /**/
#endif

struct idtocb_t_ {
	W entry_size;
	W max_entries;
	W max_allocated_id;
	UB p[1];
};

EXPORT ID idtocb_allocate(idtocb_t *manager)
{
	W i, n;
	idtocb_entry_t *entry;

	for (i = 0; i < manager->max_entries; i++) {
		n = (manager->max_allocated_id + 1 + i) % manager->max_entries;
		entry = (idtocb_entry_t*)(manager->p + n * manager->entry_size);
		if (entry->id < 0) {
			entry->id = manager->max_allocated_id + 1 + i;
			if (entry->id < 0) { /* for overflow */
				entry->id = n;
			}
			manager->max_allocated_id = entry->id;
			return entry->id;
		}
	}

	return ER_NOMEM; /* TODO: sub error code */
}

EXPORT VOID idtocb_free(idtocb_t *manager, ID id)
{
	W err;
	idtocb_entry_t *entry;

	err = idtocb_getcontrolblock(manager, id, &entry);
	if (err < 0) {
		return;
	}
	entry->id = -1;
}

EXPORT W idtocb_getcontrolblock(idtocb_t *manager, ID id, idtocb_entry_t **ptr)
{
	W n;
	idtocb_entry_t *entry;

	n = id % manager->max_entries;
	entry = (idtocb_entry_t*)(manager->p + n * manager->entry_size);
	if (entry->id != id) {
		return ER_NOEXS; /* TODO: sub error code */
	}
	*ptr = entry;

	return ER_OK;
}

EXPORT idtocb_t* idtocb_new(W entry_size, W max_entries)
{
	idtocb_t *ret;
	W i;
	idtocb_entry_t *entry;

	ret = malloc(sizeof(idtocb_t) - sizeof(UB) + sizeof(UB)*entry_size*max_entries);
	if (ret == NULL) {
		return NULL;
	}
	ret->entry_size = entry_size;
	ret->max_entries = max_entries;
	ret->max_allocated_id = 0;
	for (i = 0; i < max_entries; i++) {
		entry = (idtocb_entry_t*)(ret->p + i * entry_size);
		entry->id = -1;
	}

	return ret;
}

EXPORT VOID idtocb_delete(idtocb_t *manager)
{
	free(manager);
}

/* iterator */

EXPORT Bool idtocb_iterator_next(idtocb_iterator_t *iter, idtocb_entry_t **entry)
{
	idtocb_entry_t *e0;

	for (; iter->i < iter->manager->max_entries;) {
		e0 = (idtocb_entry_t*)(iter->manager->p + iter->i * iter->manager->entry_size);
		iter->i++;
		if (e0->id > 0) {
			*entry = e0;
			return True;
		}
	}

	return False;
}

EXPORT VOID idtocb_iterator_initialize(idtocb_iterator_t *iter, idtocb_t *manager)
{
	iter->manager = manager;
	iter->i = 0;
}

EXPORT VOID idtocb_iterator_finalize(idtocb_iterator_t *iter)
{
}

/*
 * treebase.h
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

#include <basic.h>
#include <bsys/queue.h>

/* Vendor name: */
/* Functionality name: treebase */
/* Detail name: */

#ifndef __TREEBASE_H__
#define __TREEBASE_H__

/* Functionality name: treebase */
/* Detail name: node */
struct treebase_node_t_ {
	QUEUE sibling;
	struct treebase_node_t_ *firstchild;
	struct treebase_node_t_ *parent;
};
typedef struct treebase_node_t_ treebase_node_t;

IMPORT VOID treebase_node_initialize(treebase_node_t *node);
IMPORT VOID treebase_node_finalize(treebase_node_t *node);
IMPORT VOID treebase_node_appendchild(treebase_node_t *node, treebase_node_t *child);
IMPORT VOID treebase_node_remove(treebase_node_t *node);
IMPORT treebase_node_t* treebase_node_getparent(treebase_node_t *node);
IMPORT treebase_node_t* treebase_node_getnextsibling(treebase_node_t *node);
IMPORT treebase_node_t* treebase_node_getprevsibling(treebase_node_t *node);
IMPORT treebase_node_t* treebase_node_getfirstchild(treebase_node_t *node);
IMPORT treebase_node_t* treebase_node_getlastchild(treebase_node_t *node);
IMPORT Bool treebase_node_haschild(treebase_node_t *node);

/* Functionality name: treebase */
/* Detail name: traversal */
/* Data structure identifier: direction */
enum TREEBASE_TRAVERSAL_DIRECTION_ {
	TREEBASE_TRAVERSAL_DIRECTION_UP,
	TREEBASE_TRAVERSAL_DIRECTION_DOWN
};
typedef enum TREEBASE_TRAVERSAL_DIRECTION_ TREEBASE_TRAVERSAL_DIRECTION;

/* Functionality name: treebase */
/* Detail name: preordertraversal */
struct treebase_preordertraversal_t_ {
	treebase_node_t *root;
	treebase_node_t *current;
	TREEBASE_TRAVERSAL_DIRECTION dir;
};
typedef struct treebase_preordertraversal_t_ treebase_preordertraversal_t;

IMPORT VOID treebase_preordertraversal_initialize(treebase_preordertraversal_t *traversal, treebase_node_t *root);
IMPORT VOID treebase_preordertraversal_finalize(treebase_preordertraversal_t *traversal);
IMPORT Bool treebase_preordertraversal_next(treebase_preordertraversal_t *traversal, treebase_node_t **node, TREEBASE_TRAVERSAL_DIRECTION *dir);

/* Functionality name: treebase */
/* Detail name: postordertraversal */
struct treebase_postordertraversal_t_ {
	treebase_node_t *root;
	treebase_node_t *current;
	TREEBASE_TRAVERSAL_DIRECTION dir;
};
typedef struct treebase_postordertraversal_t_ treebase_postordertraversal_t;

IMPORT VOID treebase_postordertraversal_initialize(treebase_postordertraversal_t *traversal, treebase_node_t *root);
IMPORT VOID treebase_postordertraversal_finalize(treebase_postordertraversal_t *traversal);
IMPORT Bool treebase_postordertraversal_next(treebase_postordertraversal_t *traversal, treebase_node_t **node, TREEBASE_TRAVERSAL_DIRECTION *dir);

#endif

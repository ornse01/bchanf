/*
 * treebase.c
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

#include "treebase.h"

#include <basic.h>
#include <bstdio.h>
#include <bsys/queue.h>

EXPORT VOID treebase_node_appendchild(treebase_node_t *node, treebase_node_t *child)
{
	Bool ok;

	ok = treebase_node_haschild(node);
	if (ok == False) {
		node->firstchild = child;
	} else {
		QueInsert(&child->sibling, &node->firstchild->sibling);
	}
	child->parent = node;
}

EXPORT W treebase_node_insertbefore(treebase_node_t *node, treebase_node_t *child, treebase_node_t *ref)
{
	if (ref == NULL) {
		treebase_node_appendchild(node, child);
		return 0;
	}
	if (ref->parent != node) {
		return -1; /* TODO */
	}

	QueInsert(&child->sibling, &ref->sibling);
	child->parent = node;

	return 0;
}

EXPORT VOID treebase_node_remove(treebase_node_t *node)
{
	if (node->parent == NULL) { /* root node */
		return;
	}
	if (node->parent->firstchild == node) {
		node->parent->firstchild = treebase_node_getnextsibling(node);
	}
	QueRemove(&node->sibling);
	node->parent = NULL;
}

EXPORT treebase_node_t* treebase_node_getparent(treebase_node_t *node)
{
	return node->parent;
}

EXPORT treebase_node_t* treebase_node_getnextsibling(treebase_node_t *node)
{
	Bool empty;
	treebase_node_t *parent, *last;
	empty = isQueEmpty(&node->sibling);
	if (empty != False) {
		return NULL;
	}
	parent = treebase_node_getparent(node);
	if (parent == NULL) {
		return NULL;
	}
	last = treebase_node_getlastchild(parent);
	if (node == last) {
		return NULL;
	}
	return (treebase_node_t*)node->sibling.next;
}

EXPORT treebase_node_t* treebase_node_getprevsibling(treebase_node_t *node)
{
	Bool empty;
	treebase_node_t *parent, *first;
	empty = isQueEmpty(&node->sibling);
	if (empty != False) {
		return NULL;
	}
	parent = treebase_node_getparent(node);
	if (parent == NULL) {
		return NULL;
	}
	first = treebase_node_getfirstchild(parent);
	if (node == first) {
		return NULL;
	}
	return (treebase_node_t*)node->sibling.prev;
}

EXPORT treebase_node_t* treebase_node_getfirstchild(treebase_node_t *node)
{
	return node->firstchild;
}

EXPORT treebase_node_t* treebase_node_getlastchild(treebase_node_t *node)
{
	if (node->firstchild == NULL) {
		return NULL;
	}
	return (treebase_node_t*)node->firstchild->sibling.prev;
}

EXPORT Bool treebase_node_haschild(treebase_node_t *node)
{
	if (node->firstchild == NULL) {
		return False;
	}
	return True;
}

EXPORT VOID treebase_node_initialize(treebase_node_t *node)
{
	QueInit(&node->sibling);
	node->firstchild = NULL;
	node->parent = NULL;
}

EXPORT VOID treebase_node_finalize(treebase_node_t *node)
{
	treebase_node_remove(node);
}


EXPORT Bool treebase_preordertraversal_next(treebase_preordertraversal_t *traversal, treebase_node_t **node, TREEBASE_TRAVERSAL_DIRECTION *dir)
{
	treebase_node_t *child, *sibling;

	if (traversal->current == NULL) {
		return False;
	}

	*node = traversal->current;
	*dir = traversal->dir;

	if (traversal->dir == TREEBASE_TRAVERSAL_DIRECTION_DOWN) {
		child = treebase_node_getfirstchild(traversal->current);
		if (child == NULL) {
			traversal->dir = TREEBASE_TRAVERSAL_DIRECTION_UP;
		} else {
			traversal->current = child;
		}
	} else {
		sibling = treebase_node_getnextsibling(traversal->current);
		if (sibling == NULL) {
			if (traversal->current == traversal->root) {
				traversal->current = NULL;
			} else {
				traversal->current = treebase_node_getparent(traversal->current);
			}
		} else {
			traversal->current = sibling;
			traversal->dir = TREEBASE_TRAVERSAL_DIRECTION_DOWN;
		}
	}

	return True;
}

EXPORT VOID treebase_preordertraversal_initialize(treebase_preordertraversal_t *traversal, treebase_node_t *root)
{
	traversal->root = root;
	traversal->current = root;
	traversal->dir = TREEBASE_TRAVERSAL_DIRECTION_DOWN;
}

EXPORT VOID treebase_preordertraversal_finalize(treebase_preordertraversal_t *traversal)
{
}


EXPORT Bool treebase_postordertraversal_next(treebase_postordertraversal_t *traversal, treebase_node_t **node, TREEBASE_TRAVERSAL_DIRECTION *dir)
{
	treebase_node_t *child, *sibling;

	if (traversal->current == NULL) {
		return False;
	}

	*node = traversal->current;
	*dir = traversal->dir;

	if (traversal->dir == TREEBASE_TRAVERSAL_DIRECTION_DOWN) {
		child = treebase_node_getlastchild(traversal->current);
		if (child == NULL) {
			traversal->dir = TREEBASE_TRAVERSAL_DIRECTION_UP;
		} else {
			traversal->current = child;
		}
	} else {
		sibling = treebase_node_getprevsibling(traversal->current);
		if (sibling == NULL) {
			if (traversal->current == traversal->root) {
				traversal->current = NULL;
			} else {
				traversal->current = treebase_node_getparent(traversal->current);
			}
		} else {
			traversal->current = sibling;
			traversal->dir = TREEBASE_TRAVERSAL_DIRECTION_DOWN;
		}
	}

	return True;
}

EXPORT VOID treebase_postordertraversal_initialize(treebase_postordertraversal_t *traversal, treebase_node_t *root)
{
	traversal->root = root;
	traversal->current = root;
	traversal->dir = TREEBASE_TRAVERSAL_DIRECTION_DOWN;
}

EXPORT VOID treebase_postordertraversal_finalize(treebase_postordertraversal_t *traversal)
{
}

/*
 * cssrendering_box.c
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

#include	"cssrendering_box.h"

#include	<bstdio.h>
#include	<bsys/queue.h>

#include	"cssmetric.h"
#include	"treebase.h"

#ifdef BCHAN_CONFIG_DEBUG
# define DP(arg) printf arg
# define DP_ER(msg, err) printf("%s (%d/%x)\n", msg, err>>16, err)
#else
# define DP(arg) /**/
# define DP_ER(msg, err) /**/
#endif

LOCAL Bool cssmetric_rectangle_andrect(cssmetric_rectangle_t a, cssmetric_rectangle_t b)
{
	if (a.c.left < b.c.right && b.c.left < a.c.right && a.c.top < b.c.bottom && b.c.top < a.c.bottom) {
		return True;
	}
	return False;
}

LOCAL VOID cssrendering_basebox_appendchild(cssrendering_basebox_t *box, cssrendering_basebox_t *child)
{
	treebase_node_appendchild(&box->base, &child->base);
}

LOCAL W cssrendering_basebox_insertbefore(cssrendering_basebox_t *box, cssrendering_basebox_t *child, cssrendering_basebox_t *ref)
{
	return treebase_node_insertbefore(&box->base, &child->base, (ref != NULL) ? &ref->base : NULL);
}

LOCAL cssrendering_basebox_t* cssrendering_basebox_getparent(cssrendering_basebox_t *box)
{
	return (cssrendering_basebox_t*)treebase_node_getparent(&box->base);
}

LOCAL W cssrendering_basebox_removechild(cssrendering_basebox_t *box, cssrendering_basebox_t *child)
{
	cssrendering_basebox_t *parent;

	parent = cssrendering_basebox_getparent(child);
	if (parent != box) {
		return -1; /* TODO */
	}
	treebase_node_remove(&child->base);

	return 0;
}

LOCAL cssrendering_basebox_t* cssrendering_basebox_getfirstchild(cssrendering_basebox_t *box)
{
	return (cssrendering_basebox_t*)treebase_node_getfirstchild(&box->base);
}

LOCAL cssrendering_basebox_t* cssrendering_basebox_getnextsibling(cssrendering_basebox_t *box)
{
	return (cssrendering_basebox_t*)treebase_node_getnextsibling(&box->base);
}

LOCAL VOID cssrendering_basebox_setuserdata(cssrendering_basebox_t *box, VP data)
{
	box->userdata = data;
}

LOCAL VOID cssrendering_basebox_initialize(cssrendering_basebox_t *box, CSSRENDERING_BOX_TYPE type)
{
	treebase_node_initialize(&box->base);
	box->type = type;
	box->content_edge.c.left = 0;
	box->content_edge.c.top = 0;
	box->content_edge.c.right = 0;
	box->content_edge.c.bottom = 0;
	box->userdata = NULL;
}

LOCAL VOID cssrendering_basebox_finalize(cssrendering_basebox_t *box)
{
	treebase_node_finalize(&box->base);
}

/* cssrendering_linebox */

EXPORT cssrendering_linebox_t* cssrendering_linebox_getnextsibling(cssrendering_linebox_t *box)
{
	return (cssrendering_linebox_t*)cssrendering_basebox_getnextsibling(&box->base);
}

EXPORT VOID cssrendering_linebox_setuserdata(cssrendering_linebox_t *box, VP data)
{
	cssrendering_basebox_setuserdata(&box->base, data);
}

EXPORT VOID cssrendering_linebox_initialize(cssrendering_linebox_t *box)
{
	cssrendering_basebox_initialize(&box->base, CSSRENDEREING_BOX_TYPE_LINE);
}

EXPORT VOID cssrendering_linebox_finalize(cssrendering_linebox_t *box)
{
	cssrendering_basebox_finalize(&box->base);
}

/* cssrendering_anonymousbox */

EXPORT VOID cssrendering_anonymousbox_appendchild(cssrendering_anonymousbox_t *box, cssrendering_linebox_t *child)
{
	cssrendering_basebox_appendchild(&box->base, &child->base);
}

EXPORT W cssrendering_anonymousbox_insertbefore(cssrendering_anonymousbox_t *box, cssrendering_linebox_t *child, cssrendering_linebox_t *ref)
{
	return cssrendering_basebox_insertbefore(&box->base, &child->base, (ref != NULL) ? &ref->base : NULL);
}

EXPORT W cssrendering_anonymousbox_removechild(cssrendering_anonymousbox_t *box, cssrendering_linebox_t *child)
{
	return cssrendering_basebox_removechild(&box->base, &child->base);
}

EXPORT cssrendering_linebox_t* cssrendering_anonymousbox_getfirstchild(cssrendering_anonymousbox_t *box)
{
	return (cssrendering_linebox_t*)cssrendering_basebox_getfirstchild(&box->base);
}

EXPORT VOID cssrendering_anonymousbox_setuserdata(cssrendering_anonymousbox_t *box, VP data)
{
	cssrendering_basebox_setuserdata(&box->base, data);
}

EXPORT VOID cssrendering_anonymousbox_initialize(cssrendering_anonymousbox_t *box)
{
	cssrendering_basebox_initialize(&box->base, CSSRENDEREING_BOX_TYPE_ANONYMOUS);
}

EXPORT VOID cssrendering_anonymousbox_finalize(cssrendering_anonymousbox_t *box)
{
	cssrendering_basebox_finalize(&box->base);
}

/* cssrendering_blockbox */

EXPORT VOID cssrendering_blockbox_appendanonymouschild(cssrendering_blockbox_t *box, cssrendering_anonymousbox_t *child)
{
	cssrendering_basebox_appendchild(&box->base, &child->base);
}

EXPORT VOID cssrendering_blockbox_appendblockchild(cssrendering_blockbox_t *box, cssrendering_blockbox_t *child)
{
	cssrendering_basebox_appendchild(&box->base, &child->base);
}

EXPORT VOID cssrendering_blockbox_setuserdata(cssrendering_blockbox_t *box, VP data)
{
	cssrendering_basebox_setuserdata(&box->base, data);
}

EXPORT VOID cssrendering_blockbox_initialize(cssrendering_blockbox_t *box)
{
	cssrendering_basebox_initialize(&box->base, CSSRENDEREING_BOX_TYPE_BLOCK);
}

EXPORT VOID cssrendering_blockbox_finalize(cssrendering_blockbox_t *box)
{
	cssrendering_basebox_finalize(&box->base);
}

/* cssrendering_drawtraversal */

EXPORT Bool cssrendering_drawtraversal_next(cssrendering_drawtraversal_t *traversal, cssrendering_drawtraversal_result *result)
{
	Bool cont, ok;
	TREEBASE_TRAVERSAL_DIRECTION dir;
	union {
		cssrendering_basebox_t base;
		cssrendering_linebox_t l;
		cssrendering_anonymousbox_t a;
		cssrendering_blockbox_t b;
	} *box;
	cssmetric_rectangle_t r;

	for (;;) {
		cont = treebase_preordertraversal_next(&traversal->base, (treebase_node_t**)&box, &dir);
		if (cont == False) {
			break;
		}
		if (dir == TREEBASE_TRAVERSAL_DIRECTION_DOWN) {
			if (box->base.type == CSSRENDEREING_BOX_TYPE_ANONYMOUS) {
				traversal->origin.x += box->base.content_edge.c.left;
				traversal->origin.y += box->base.content_edge.c.top;
			} else if (box->base.type == CSSRENDEREING_BOX_TYPE_BLOCK) {
				r = box->base.content_edge;
				r.c.left += traversal->origin.x;
				r.c.top += traversal->origin.y;
				r.c.right += traversal->origin.x;
				r.c.bottom += traversal->origin.y;

				traversal->origin.x += box->base.content_edge.c.left;
				traversal->origin.y += box->base.content_edge.c.top;

				ok = cssmetric_rectangle_andrect(r, traversal->draw);
				if (ok == False) {
					continue;
				}

				result->type = CSSRENDERING_DRAWTRAVERSAL_RESULTTYPE_BLOCK;
				result->data.block.content_edge = r;
				result->data.block.nodedata = box->base.userdata;
				break;
			} else if (box->base.type == CSSRENDEREING_BOX_TYPE_LINE) {
				r = box->base.content_edge;
				r.c.left += traversal->origin.x;
				r.c.top += traversal->origin.y;
				r.c.right += traversal->origin.x;
				r.c.bottom += traversal->origin.y;
				ok = cssmetric_rectangle_andrect(r, traversal->draw);
				if (ok == False) {
					continue;
				}

				result->type = CSSRENDERING_DRAWTRAVERSAL_RESULTTYPE_TEXT;
				result->data.text.content_edge = r;
				result->data.text.fragment = &(box->l.text);
				result->data.text.nodedata = box->base.userdata;
				break;
			}
		} else {
			if ((box->base.type == CSSRENDEREING_BOX_TYPE_BLOCK)||(box->base.type == CSSRENDEREING_BOX_TYPE_ANONYMOUS)) {
				traversal->origin.x -= box->base.content_edge.c.left;
				traversal->origin.y -= box->base.content_edge.c.top;
			} else {
			}
		}
	}

	return cont;
}

EXPORT VOID cssrendering_drawtraversal_initialize(cssrendering_drawtraversal_t *traversal, cssrendering_blockbox_t *root, cssmetric_rectangle_t draw)
{
	treebase_preordertraversal_initialize(&traversal->base, &root->base.base);
	traversal->origin.x = 0;
	traversal->origin.y = 0;
	traversal->draw = draw;
}

EXPORT VOID cssrendering_drawtraversal_finalize(cssrendering_drawtraversal_t *traversal)
{
	treebase_preordertraversal_finalize(&traversal->base);
}

/* cssrendering_hittraversal */

EXPORT Bool cssrendering_hittraversal_next(cssrendering_hittraversal_t *traversal, cssrendering_hittraversal_result *result)
{
	Bool cont, ok;
	TREEBASE_TRAVERSAL_DIRECTION dir;
	union {
		cssrendering_basebox_t base;
		cssrendering_linebox_t l;
		cssrendering_anonymousbox_t a;
		cssrendering_blockbox_t b;
	} *box;
	cssmetric_rectangle_t r;

	for (;;) {
		cont = treebase_postordertraversal_next(&traversal->base, (treebase_node_t**)&box, &dir);
		if (cont == False) {
			break;
		}
		if (dir == TREEBASE_TRAVERSAL_DIRECTION_DOWN) {
			if ((box->base.type == CSSRENDEREING_BOX_TYPE_BLOCK)||(box->base.type == CSSRENDEREING_BOX_TYPE_ANONYMOUS)) {
				traversal->origin.x += box->base.content_edge.c.left;
				traversal->origin.y += box->base.content_edge.c.top;
			} else if (box->base.type == CSSRENDEREING_BOX_TYPE_LINE) {
				r = box->base.content_edge;
				r.c.left += traversal->origin.x;
				r.c.top += traversal->origin.y;
				r.c.right += traversal->origin.x;
				r.c.bottom += traversal->origin.y;
				ok = cssmetric_rectangle_andrect(r, traversal->draw);
				if (ok == False) {
					continue;
				}

				result->type = CSSRENDERING_HITTRAVERSAL_RESULTTYPE_LINE;
				result->data.line.content_edge = r;
				result->data.line.nodedata = box->base.userdata;
				break;
			}
		} else {
			if ((box->base.type == CSSRENDEREING_BOX_TYPE_BLOCK)||(box->base.type == CSSRENDEREING_BOX_TYPE_ANONYMOUS)) {
				traversal->origin.x -= box->base.content_edge.c.left;
				traversal->origin.y -= box->base.content_edge.c.top;
			} else {
			}
		}
	}

	return cont;
}

EXPORT VOID cssrendering_hittraversal_initialize(cssrendering_hittraversal_t *traversal, cssrendering_blockbox_t *root, cssmetric_rectangle_t draw)
{
	treebase_postordertraversal_initialize(&traversal->base, &root->base.base);
	traversal->origin.x = 0;
	traversal->origin.y = 0;
	traversal->draw = draw;
}

EXPORT VOID cssrendering_hittraversal_finalize(cssrendering_hittraversal_t *traversal)
{
	treebase_postordertraversal_finalize(&traversal->base);
}

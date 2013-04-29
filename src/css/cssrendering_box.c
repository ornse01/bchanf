/*
 * cssrendering_box.c
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

LOCAL VOID cssrendering_basebox_appendchild(cssrendering_basebox_t *box, cssrendering_basebox_t *child)
{
	treebase_node_appendchild(&box->base, &child->base);
	box->content_edge.c.left = 0;
	box->content_edge.c.top = 0;
	box->content_edge.c.right = 0;
	box->content_edge.c.bottom = 0;
}

LOCAL VOID cssrendering_basebox_initialize(cssrendering_basebox_t *box, CSSRENDERING_BOX_TYPE type)
{
	treebase_node_initialize(&box->base);
	box->type = type;
}

LOCAL VOID cssrendering_basebox_finalize(cssrendering_basebox_t *box)
{
	treebase_node_finalize(&box->base);
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
	Bool cont;
	TREEBASE_TRAVERSAL_DIRECTION dir;
	union {
		cssrendering_basebox_t base;
		cssrendering_linebox_t l;
		cssrendering_anonymousbox_t a;
		cssrendering_blockbox_t b;
	} *box;

	for (;;) {
		cont = treebase_preordertraversal_next(&traversal->base, (treebase_node_t**)&box, &dir);
		if (cont != False) {
			if (dir == TREEBASE_TRAVERSAL_DIRECTION_DOWN) {
				if ((box->base.type == CSSRENDEREING_BOX_TYPE_BLOCK)||(box->base.type == CSSRENDEREING_BOX_TYPE_ANONYMOUS)) {
					traversal->origin.x += box->base.content_edge.c.left;
					traversal->origin.y += box->base.content_edge.c.top;
				} else if (box->base.type == CSSRENDEREING_BOX_TYPE_LINE) {
					result->type = CSSRENDERING_DRAWTRAVERSAL_RESULTTYPE_TEXT;
					result->data.text.fragment = &(box->l.text);
					result->data.text.blstart.x = traversal->origin.x + box->base.content_edge.c.left;
					result->data.text.blstart.y = traversal->origin.y + box->base.content_edge.c.top + box->l.baseline;
					break;
				}
			} else {
				if ((box->base.type == CSSRENDEREING_BOX_TYPE_BLOCK)||(box->base.type == CSSRENDEREING_BOX_TYPE_ANONYMOUS)) {
					traversal->origin.x -= box->base.content_edge.c.left;
					traversal->origin.y -= box->base.content_edge.c.top;
				} else {
				}
			}
		} else {
			break;
		}
	}

	return cont;
}

EXPORT VOID cssrendering_drawtraversal_initialize(cssrendering_drawtraversal_t *traversal, cssrendering_blockbox_t *root)
{
	treebase_preordertraversal_initialize(&traversal->base, &root->base.base);
	traversal->origin.x = 0;
	traversal->origin.y = 0;
}

EXPORT VOID cssrendering_drawtraversal_finalize(cssrendering_drawtraversal_t *traversal)
{
	treebase_preordertraversal_finalize(&traversal->base);
}

/*
 * cssrendering_box.h
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

#include    <basic.h>
#include    <btron/dp.h>

#include	"cssmetric.h"
#include	<coll/treebase.h>

#ifndef __CSSRENDERING_BOX_H__
#define __CSSRENDERING_BOX_H__

/* Functionality name: cssrendering */
/* Detail name: textfragment */
struct cssrendering_textfragment_t_ {
	TC *str;
	W len;
};
typedef struct cssrendering_textfragment_t_ cssrendering_textfragment_t;

/* Functionality name: cssrendering */
/* Detail name: box */
/* Data structure identifier: type */
enum CSSRENDERING_BOX_TYPE_ {
	CSSRENDEREING_BOX_TYPE_BLOCK,
	CSSRENDEREING_BOX_TYPE_LINE,
	CSSRENDEREING_BOX_TYPE_ANONYMOUS,
}; 
typedef enum CSSRENDERING_BOX_TYPE_ CSSRENDERING_BOX_TYPE;

/* Functionality name: cssrendering */
/* Detail name: basebox */
struct cssrendering_basebox_t_ {
	treebase_node_t base;
	CSSRENDERING_BOX_TYPE type;
	cssmetric_rectangle_t content_edge;
	VP userdata;
};
typedef struct cssrendering_basebox_t_ cssrendering_basebox_t;

/* Functionality name: cssrendering */
/* Detail name: linebox */
struct cssrendering_linebox_t_ {
	cssrendering_basebox_t base;
	cssrendering_textfragment_t text;
};
typedef struct cssrendering_linebox_t_ cssrendering_linebox_t;

/* Functionality name: cssrendering */
/* Detail name: anonymousbox */
struct cssrendering_anonymousbox_t_ {
	cssrendering_basebox_t base;
};
typedef struct cssrendering_anonymousbox_t_ cssrendering_anonymousbox_t;

/* Functionality name: cssrendering */
/* Detail name: blockbox */
struct cssrendering_blockbox_t_ {
	cssrendering_basebox_t base;
	cssmetric_rectangle_t view;
};
typedef struct cssrendering_blockbox_t_ cssrendering_blockbox_t;

/* Functionality name: cssrendering */
/* Detail name: drawtraversal */
struct cssrendering_drawtraversal_t_ {
	treebase_preordertraversal_t base;
	cssmetric_point_t origin;
	cssmetric_rectangle_t draw;
};
typedef struct cssrendering_drawtraversal_t_ cssrendering_drawtraversal_t;

/* Functionality name: cssrendering */
/* Detail name: drawtraversal */
/* Data structure identifier: result */
struct cssrendering_drawtraversal_result_ {
	enum {
		CSSRENDERING_DRAWTRAVERSAL_RESULTTYPE_TEXT,
	} type;
	union {
		struct {
			cssmetric_rectangle_t content_edge;
			cssrendering_textfragment_t *fragment;
			VP nodedata;
		} text;
	} data;
};
typedef struct cssrendering_drawtraversal_result_ cssrendering_drawtraversal_result;

/* Functionality name: cssrendering */
/* Detail name: hittraversal */
struct cssrendering_hittraversal_t_ {
	treebase_postordertraversal_t base;
	cssmetric_point_t origin;
	cssmetric_rectangle_t draw;
};
typedef struct cssrendering_hittraversal_t_ cssrendering_hittraversal_t;

/* Functionality name: cssrendering */
/* Detail name: hittraversal */
/* Data structure identifier: result */
struct cssrendering_hittraversal_result_ {
	enum {
		CSSRENDERING_HITTRAVERSAL_RESULTTYPE_BLOCK,
		CSSRENDERING_HITTRAVERSAL_RESULTTYPE_LINE,
	} type;
	union {
		struct {
			cssmetric_rectangle_t content_edge;
			VP nodedata;
		} line;
	} data;
};
typedef struct cssrendering_hittraversal_result_ cssrendering_hittraversal_result;

IMPORT VOID cssrendering_linebox_initialize(cssrendering_linebox_t *box);
IMPORT VOID cssrendering_linebox_finalize(cssrendering_linebox_t *box);
IMPORT VOID cssrendering_linebox_setuserdata(cssrendering_linebox_t *box, VP data);

IMPORT VOID cssrendering_anonymousbox_initialize(cssrendering_anonymousbox_t *box);
IMPORT VOID cssrendering_anonymousbox_finalize(cssrendering_anonymousbox_t *box);
IMPORT VOID cssrendering_anonymousbox_appendchild(cssrendering_anonymousbox_t *box, cssrendering_linebox_t *child);
IMPORT VOID cssrendering_anonymousbox_setuserdata(cssrendering_anonymousbox_t *box, VP data);

IMPORT VOID cssrendering_blockbox_initialize(cssrendering_blockbox_t *box);
IMPORT VOID cssrendering_blockbox_finalize(cssrendering_blockbox_t *box);
IMPORT VOID cssrendering_blockbox_appendanonymouschild(cssrendering_blockbox_t *box, cssrendering_anonymousbox_t *child);
IMPORT VOID cssrendering_blockbox_appendblockchild(cssrendering_blockbox_t *box, cssrendering_blockbox_t *child);
IMPORT VOID cssrendering_blockbox_setuserdata(cssrendering_blockbox_t *box, VP data);

IMPORT VOID cssrendering_drawtraversal_initialize(cssrendering_drawtraversal_t *traversal, cssrendering_blockbox_t *root, cssmetric_rectangle_t draw);
IMPORT VOID cssrendering_drawtraversal_finalize(cssrendering_drawtraversal_t *traversal);
IMPORT Bool cssrendering_drawtraversal_next(cssrendering_drawtraversal_t *traversal, cssrendering_drawtraversal_result *result);

IMPORT VOID cssrendering_hittraversal_initialize(cssrendering_hittraversal_t *traversal, cssrendering_blockbox_t *root, cssmetric_rectangle_t draw);
IMPORT VOID cssrendering_hittraversal_finalize(cssrendering_hittraversal_t *traversal);
IMPORT Bool cssrendering_hittraversal_next(cssrendering_hittraversal_t *traversal, cssrendering_hittraversal_result *result);

#endif

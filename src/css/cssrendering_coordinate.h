/*
 * cssrendering_coodinate.h
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

#include    <basic.h>

#include	"cssmetric.h"

#ifndef __CSSRENDERING_COORDINATE_H__
#define __CSSRENDERING_COORDINATE_H__

/* Functionality name: cssrendering */
/* Detail name: coordinate */
struct cssrendering_coordinate_t_ {
	cssmetric_rectangle_t draw;
	cssmetric_rectangle_t view;
};
typedef struct cssrendering_coordinate_t_ cssrendering_coordinate_t;

IMPORT VOID cssrendering_coordinate_initialize(cssrendering_coordinate_t *scroll);
IMPORT VOID cssrendering_coordinate_finalize(cssrendering_coordinate_t *scroll);
IMPORT VOID cssrendering_coordinate_getabsoluterect(cssrendering_coordinate_t *scroll, RECT src, cssmetric_rectangle_t *dest);
IMPORT VOID cssrendering_coordinate_getrelativepoint(cssrendering_coordinate_t *scroll, W src_x, W src_y, W *dest_x, W *dest_y);
IMPORT VOID cssrendering_coordinate_setdrawrect(cssrendering_coordinate_t *scroll, cssmetric_rectangle_t draw);
IMPORT VOID cssrendering_coordinate_getdrawrect(cssrendering_coordinate_t *scroll, W *l, W *t, W *r, W *b);
IMPORT VOID cssrendering_coordinate_setviewrect(cssrendering_coordinate_t *scroll, W l, W t, W r, W b);
IMPORT VOID cssrendering_coordinate_getviewrect(cssrendering_coordinate_t *scroll, W *l, W *t, W *r, W *b);
IMPORT VOID cssrendering_coordinate_scrollviewrect(cssrendering_coordinate_t *scroll, W dh, W dv);

#endif

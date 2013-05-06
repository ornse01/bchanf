/*
 * cssrendering_coordinate.c
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

#include	"cssrendering_coordinate.h"

#include	<bstdio.h>

#include	"cssmetric.h"

#ifdef BCHAN_CONFIG_DEBUG
# define DP(arg) printf arg
# define DP_ER(msg, err) printf("%s (%d/%x)\n", msg, err>>16, err)
#else
# define DP(arg) /**/
# define DP_ER(msg, err) /**/
#endif

EXPORT VOID cssrendering_coordinate_getabsoluterect(cssrendering_coordinate_t *scroll, RECT src, cssmetric_rectangle_t *dest)
{
	dest->c.left = src.c.left + scroll->view.c.left;
	dest->c.top = src.c.top + scroll->view.c.top;
	dest->c.right = src.c.right + scroll->view.c.left;
	dest->c.bottom = src.c.bottom + scroll->view.c.top;
}

EXPORT VOID cssrendering_coordinate_getrelativepoint(cssrendering_coordinate_t *scroll, W src_x, W src_y, W *dest_x, W *dest_y)
{
	*dest_x = src_x - scroll->view.c.left;
	*dest_y = src_y - scroll->view.c.top;
}

EXPORT VOID cssrendering_coordinate_setdrawrect(cssrendering_coordinate_t *scroll, cssmetric_rectangle_t draw)
{
	scroll->draw = draw;
}

EXPORT VOID cssrendering_coordinate_getdrawrect(cssrendering_coordinate_t *scroll, W *l, W *t, W *r, W *b)
{
	*l = scroll->draw.c.left;
	*t = scroll->draw.c.top;
	*r = scroll->draw.c.right;
	*b = scroll->draw.c.bottom;
}

EXPORT VOID cssrendering_coordinate_setviewrect(cssrendering_coordinate_t *scroll, W l, W t, W r, W b)
{
	scroll->view.c.left = l;
	scroll->view.c.top = t;
	scroll->view.c.right = r;
	scroll->view.c.bottom = b;
}

EXPORT VOID cssrendering_coordinate_getviewrect(cssrendering_coordinate_t *scroll, W *l, W *t, W *r, W *b)
{
	*l = scroll->view.c.left;
	*t = scroll->view.c.top;
	*r = scroll->view.c.right;
	*b = scroll->view.c.bottom;
}

EXPORT VOID cssrendering_coordinate_scrollviewrect(cssrendering_coordinate_t *scroll, W dh, W dv)
{
	scroll->view.c.left += dh;
	scroll->view.c.top += dv;
	scroll->view.c.right += dh;
	scroll->view.c.bottom += dv;
}

EXPORT VOID cssrendering_coordinate_initialize(cssrendering_coordinate_t *scroll)
{
}

EXPORT VOID cssrendering_coordinate_finalize(cssrendering_coordinate_t *scroll)
{
}

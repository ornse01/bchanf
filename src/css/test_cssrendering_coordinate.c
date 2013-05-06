/*
 * test_cssrendering_coordinate.c
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

#include "test_css.h"

#include "cssrendering_coordinate.h"

#include	<basic.h>
#include	<bstdio.h>
#include	<bstdlib.h>
#include	<bstring.h>

#include    <unittest_driver.h>

#include    "cssmetric.h"

LOCAL UNITTEST_RESULT test_cssrendering_coordinate_1()
{
	cssrendering_coordinate_t coordinate;
	cssmetric_rectangle_t draw = (cssmetric_rectangle_t){{0, 0, 500, 500}};
	RECT src;
	cssmetric_rectangle_t dest;
	UNITTEST_RESULT ret = UNITTEST_RESULT_PASS;

	cssrendering_coordinate_initialize(&coordinate);
	cssrendering_coordinate_setdrawrect(&coordinate, draw);
	cssrendering_coordinate_setviewrect(&coordinate, 100, 100, 400, 400);

	src = (RECT){{10, 10, 300, 300}};
	cssrendering_coordinate_getabsoluterect(&coordinate, src, &dest);
	if ((dest.c.left != 110)||(dest.c.top != 110)||(dest.c.right != 400)||(dest.c.bottom != 400)) {
		ret = UNITTEST_RESULT_FAIL;
	}

	cssrendering_coordinate_finalize(&coordinate);

	return ret;
}

LOCAL UNITTEST_RESULT test_cssrendering_coordinate_2()
{
	cssrendering_coordinate_t coordinate;
	cssmetric_rectangle_t draw = (cssmetric_rectangle_t){{0, 0, 500, 500}};
	W x, y;
	UNITTEST_RESULT ret = UNITTEST_RESULT_PASS;

	cssrendering_coordinate_initialize(&coordinate);
	cssrendering_coordinate_setdrawrect(&coordinate, draw);
	cssrendering_coordinate_setviewrect(&coordinate, 100, 100, 400, 400);

	cssrendering_coordinate_getrelativepoint(&coordinate, 110, 120, &x, &y);
	if ((x != 10)||(y != 20)) {
		ret = UNITTEST_RESULT_FAIL;
	}

	cssrendering_coordinate_finalize(&coordinate);

	return ret;
}

LOCAL UNITTEST_RESULT test_cssrendering_coordinate_3()
{
	cssrendering_coordinate_t coordinate;
	cssmetric_rectangle_t draw = (cssmetric_rectangle_t){{0, 0, 500, 500}};
	W l, t, r, b;
	UNITTEST_RESULT ret = UNITTEST_RESULT_PASS;

	cssrendering_coordinate_initialize(&coordinate);
	cssrendering_coordinate_setdrawrect(&coordinate, draw);
	cssrendering_coordinate_setviewrect(&coordinate, 100, 100, 400, 400);

	cssrendering_coordinate_scrollviewrect(&coordinate, 30, 40);
	cssrendering_coordinate_getviewrect(&coordinate, &l, &t, &r, &b);
	if ((l != 130)||(t != 140)||(r != 430)||(b != 440)) {
		ret = UNITTEST_RESULT_FAIL;
	}

	cssrendering_coordinate_finalize(&coordinate);

	return ret;
}

EXPORT VOID test_cssrendering_coordinate_main(unittest_driver_t *driver)
{
	UNITTEST_DRIVER_REGIST(driver, test_cssrendering_coordinate_1);
	UNITTEST_DRIVER_REGIST(driver, test_cssrendering_coordinate_2);
	UNITTEST_DRIVER_REGIST(driver, test_cssrendering_coordinate_3);
}

/*
 * test_cssrendering_box.c
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

#include "cssrendering_box.h"

#include	<basic.h>
#include	<bstdio.h>
#include	<bstdlib.h>
#include	<bstring.h>

#include    <unittest_driver.h>

#include    "cssmetric.h"

LOCAL UNITTEST_RESULT test_cssrendering_drawtraversal_1()
{
	cssrendering_blockbox_t root;
	cssrendering_anonymousbox_t anon;
	cssrendering_linebox_t line[5];
	cssrendering_drawtraversal_t traversal;
	cssrendering_drawtraversal_result result;
	cssmetric_rectangle_t draw;
	Bool cont, line_called[5] = {False, False, False, False, False};
	UNITTEST_RESULT ret = UNITTEST_RESULT_PASS;

	cssrendering_blockbox_initialize(&root);
	cssrendering_anonymousbox_initialize(&anon);
	cssrendering_linebox_initialize(line+0);
	cssrendering_linebox_initialize(line+1);
	cssrendering_linebox_initialize(line+2);
	cssrendering_linebox_initialize(line+3);
	cssrendering_linebox_initialize(line+4);

	cssrendering_blockbox_appendanonymouschild(&root, &anon);
	cssrendering_anonymousbox_appendchild(&anon, line+0);
	cssrendering_anonymousbox_appendchild(&anon, line+1);
	cssrendering_anonymousbox_appendchild(&anon, line+2);
	cssrendering_anonymousbox_appendchild(&anon, line+3);
	cssrendering_anonymousbox_appendchild(&anon, line+4);

	line[0].base.content_edge = (cssmetric_rectangle_t){{0, 0, 100, 100}};
	cssrendering_linebox_setuserdata(line+0, line+0);
	line[1].base.content_edge = (cssmetric_rectangle_t){{0, 100, 100, 200}};
	cssrendering_linebox_setuserdata(line+1, line+1);
	line[2].base.content_edge = (cssmetric_rectangle_t){{0, 200, 100, 300}};
	cssrendering_linebox_setuserdata(line+2, line+2);
	line[3].base.content_edge = (cssmetric_rectangle_t){{0, 300, 100, 400}};
	cssrendering_linebox_setuserdata(line+3, line+3);
	line[4].base.content_edge = (cssmetric_rectangle_t){{0, 400, 100, 500}};
	cssrendering_linebox_setuserdata(line+4, line+4);

	draw = (cssmetric_rectangle_t){{25, 150, 75, 350}};
	cssrendering_drawtraversal_initialize(&traversal, &root, draw);
	for (;;) {
		cont = cssrendering_drawtraversal_next(&traversal, &result);
		if (cont == False) {
			break;
		}
		if (result.type != CSSRENDERING_DRAWTRAVERSAL_RESULTTYPE_TEXT) {
			continue;
		}
		if (result.data.text.nodedata == (line+0)) {
			line_called[0] = True;
		} else if (result.data.text.nodedata == (line+1)) {
			line_called[1] = True;
		} else if (result.data.text.nodedata == (line+2)) {
			line_called[2] = True;
		} else if (result.data.text.nodedata == (line+3)) {
			line_called[3] = True;
		} else if (result.data.text.nodedata == (line+4)) {
			line_called[4] = True;
		}
	}
	cssrendering_drawtraversal_finalize(&traversal);

	if ((line_called[0] == False)&&(line_called[1] != False)&&(line_called[2] != False)&&(line_called[3] != False)&&(line_called[4] == False)) {
		ret = UNITTEST_RESULT_PASS;
	} else {
		ret = UNITTEST_RESULT_FAIL;
	}

	cssrendering_linebox_finalize(line+4);
	cssrendering_linebox_finalize(line+3);
	cssrendering_linebox_finalize(line+2);
	cssrendering_linebox_finalize(line+1);
	cssrendering_linebox_finalize(line+0);
	cssrendering_anonymousbox_finalize(&anon);
	cssrendering_blockbox_finalize(&root);

	return ret;
}

EXPORT VOID test_cssrendering_box_main(unittest_driver_t *driver)
{
	UNITTEST_DRIVER_REGIST(driver, test_cssrendering_drawtraversal_1);
}

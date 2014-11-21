/*
 * test_cssrendering_box.c
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

LOCAL UNITTEST_RESULT test_cssrendering_drawtraversal_2()
{
	cssrendering_blockbox_t root, block[5];
	cssrendering_anonymousbox_t anon[5];
	cssrendering_linebox_t line[5];
	cssrendering_drawtraversal_t traversal;
	cssrendering_drawtraversal_result result;
	cssmetric_rectangle_t draw;
	Bool cont, line_called[5] = {False, False, False, False, False};
	UNITTEST_RESULT ret = UNITTEST_RESULT_PASS;

	cssrendering_blockbox_initialize(&root);
	cssrendering_blockbox_initialize(block+0);
	cssrendering_blockbox_initialize(block+1);
	cssrendering_blockbox_initialize(block+2);
	cssrendering_blockbox_initialize(block+3);
	cssrendering_blockbox_initialize(block+4);
	cssrendering_anonymousbox_initialize(anon+0);
	cssrendering_anonymousbox_initialize(anon+1);
	cssrendering_anonymousbox_initialize(anon+2);
	cssrendering_anonymousbox_initialize(anon+3);
	cssrendering_anonymousbox_initialize(anon+4);
	cssrendering_linebox_initialize(line+0);
	cssrendering_linebox_initialize(line+1);
	cssrendering_linebox_initialize(line+2);
	cssrendering_linebox_initialize(line+3);
	cssrendering_linebox_initialize(line+4);

	cssrendering_blockbox_appendblockchild(&root, block+0);
	cssrendering_blockbox_appendblockchild(&root, block+1);
	cssrendering_blockbox_appendblockchild(&root, block+2);
	cssrendering_blockbox_appendblockchild(&root, block+3);
	cssrendering_blockbox_appendblockchild(&root, block+4);
	cssrendering_blockbox_appendanonymouschild(block+0, anon+0);
	cssrendering_blockbox_appendanonymouschild(block+1, anon+1);
	cssrendering_blockbox_appendanonymouschild(block+2, anon+2);
	cssrendering_blockbox_appendanonymouschild(block+3, anon+3);
	cssrendering_blockbox_appendanonymouschild(block+4, anon+4);
	cssrendering_anonymousbox_appendchild(anon+0, line+0);
	cssrendering_anonymousbox_appendchild(anon+1, line+1);
	cssrendering_anonymousbox_appendchild(anon+2, line+2);
	cssrendering_anonymousbox_appendchild(anon+3, line+3);
	cssrendering_anonymousbox_appendchild(anon+4, line+4);

	block[0].base.content_edge = (cssmetric_rectangle_t){{0, 0, 100, 100}};
	block[1].base.content_edge = (cssmetric_rectangle_t){{0, 100, 100, 200}};
	block[2].base.content_edge = (cssmetric_rectangle_t){{0, 200, 100, 300}};
	block[3].base.content_edge = (cssmetric_rectangle_t){{0, 300, 100, 400}};
	block[4].base.content_edge = (cssmetric_rectangle_t){{0, 400, 100, 500}};
	line[0].base.content_edge = (cssmetric_rectangle_t){{0, 0, 100, 100}};
	cssrendering_linebox_setuserdata(line+0, line+0);
	line[1].base.content_edge = (cssmetric_rectangle_t){{0, 0, 100, 100}};
	cssrendering_linebox_setuserdata(line+1, line+1);
	line[2].base.content_edge = (cssmetric_rectangle_t){{0, 0, 100, 100}};
	cssrendering_linebox_setuserdata(line+2, line+2);
	line[3].base.content_edge = (cssmetric_rectangle_t){{0, 0, 100, 100}};
	cssrendering_linebox_setuserdata(line+3, line+3);
	line[4].base.content_edge = (cssmetric_rectangle_t){{0, 0, 100, 100}};
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
	cssrendering_anonymousbox_finalize(anon+4);
	cssrendering_anonymousbox_finalize(anon+3);
	cssrendering_anonymousbox_finalize(anon+2);
	cssrendering_anonymousbox_finalize(anon+1);
	cssrendering_anonymousbox_finalize(anon+0);
	cssrendering_blockbox_finalize(block+4);
	cssrendering_blockbox_finalize(block+3);
	cssrendering_blockbox_finalize(block+2);
	cssrendering_blockbox_finalize(block+1);
	cssrendering_blockbox_finalize(block+0);
	cssrendering_blockbox_finalize(&root);

	return ret;
}

LOCAL UNITTEST_RESULT test_cssrendering_drawtraversal_3()
{
	cssrendering_blockbox_t root, block[5];
	cssrendering_drawtraversal_t traversal;
	cssrendering_drawtraversal_result result;
	cssmetric_rectangle_t draw;
	Bool cont, block_called[5] = {False, False, False, False, False};
	UNITTEST_RESULT ret = UNITTEST_RESULT_PASS;

	cssrendering_blockbox_initialize(&root);
	cssrendering_blockbox_initialize(block+0);
	cssrendering_blockbox_initialize(block+1);
	cssrendering_blockbox_initialize(block+2);
	cssrendering_blockbox_initialize(block+3);
	cssrendering_blockbox_initialize(block+4);

	cssrendering_blockbox_appendblockchild(&root, block+0);
	cssrendering_blockbox_appendblockchild(&root, block+1);
	cssrendering_blockbox_appendblockchild(&root, block+2);
	cssrendering_blockbox_appendblockchild(&root, block+3);
	cssrendering_blockbox_appendblockchild(&root, block+4);

	block[0].base.content_edge = (cssmetric_rectangle_t){{0, 0, 100, 100}};
	cssrendering_blockbox_setuserdata(block+0, block+0);
	block[1].base.content_edge = (cssmetric_rectangle_t){{0, 100, 100, 200}};
	cssrendering_blockbox_setuserdata(block+1, block+1);
	block[2].base.content_edge = (cssmetric_rectangle_t){{0, 200, 100, 300}};
	cssrendering_blockbox_setuserdata(block+2, block+2);
	block[3].base.content_edge = (cssmetric_rectangle_t){{0, 300, 100, 400}};
	cssrendering_blockbox_setuserdata(block+3, block+3);
	block[4].base.content_edge = (cssmetric_rectangle_t){{0, 400, 100, 500}};
	cssrendering_blockbox_setuserdata(block+4, block+4);

	draw = (cssmetric_rectangle_t){{25, 150, 75, 350}};
	cssrendering_drawtraversal_initialize(&traversal, &root, draw);
	for (;;) {
		cont = cssrendering_drawtraversal_next(&traversal, &result);
		if (cont == False) {
			break;
		}
		if (result.type != CSSRENDERING_DRAWTRAVERSAL_RESULTTYPE_BLOCK) {
			continue;
		}
		if (result.data.block.nodedata == (block+0)) {
			block_called[0] = True;
		} else if (result.data.block.nodedata == (block+1)) {
			block_called[1] = True;
		} else if (result.data.block.nodedata == (block+2)) {
			block_called[2] = True;
		} else if (result.data.block.nodedata == (block+3)) {
			block_called[3] = True;
		} else if (result.data.block.nodedata == (block+4)) {
			block_called[4] = True;
		}
	}
	cssrendering_drawtraversal_finalize(&traversal);

	if ((block_called[0] == False)&&(block_called[1] != False)&&(block_called[2] != False)&&(block_called[3] != False)&&(block_called[4] == False)) {
		ret = UNITTEST_RESULT_PASS;
	} else {
		ret = UNITTEST_RESULT_FAIL;
	}

	cssrendering_blockbox_finalize(block+4);
	cssrendering_blockbox_finalize(block+3);
	cssrendering_blockbox_finalize(block+2);
	cssrendering_blockbox_finalize(block+1);
	cssrendering_blockbox_finalize(block+0);
	cssrendering_blockbox_finalize(&root);

	return ret;
}

LOCAL UNITTEST_RESULT test_cssrendering_hittraversal_1()
{
	cssrendering_blockbox_t root;
	cssrendering_anonymousbox_t anon;
	cssrendering_linebox_t line[5];
	cssrendering_hittraversal_t traversal;
	cssrendering_hittraversal_result result;
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
	cssrendering_hittraversal_initialize(&traversal, &root, draw);
	for (;;) {
		cont = cssrendering_hittraversal_next(&traversal, &result);
		if (cont == False) {
			break;
		}
		if (result.type != CSSRENDERING_HITTRAVERSAL_RESULTTYPE_LINE) {
			continue;
		}
		if (result.data.line.nodedata == (line+0)) {
			line_called[0] = True;
		} else if (result.data.line.nodedata == (line+1)) {
			line_called[1] = True;
		} else if (result.data.line.nodedata == (line+2)) {
			line_called[2] = True;
		} else if (result.data.line.nodedata == (line+3)) {
			line_called[3] = True;
		} else if (result.data.line.nodedata == (line+4)) {
			line_called[4] = True;
		}
	}
	cssrendering_hittraversal_finalize(&traversal);

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

LOCAL UNITTEST_RESULT test_cssrendering_blockbox_appendchild1()
{
	cssrendering_blockbox_t root;
	cssrendering_anonymousbox_t anon;
	UNITTEST_RESULT result = UNITTEST_RESULT_PASS;

	cssrendering_blockbox_initialize(&root);
	cssrendering_anonymousbox_initialize(&anon);

	root.base.content_edge = (cssmetric_rectangle_t){{0, 0, 100, 100}};
	cssrendering_blockbox_appendanonymouschild(&root, &anon);

	if (root.base.content_edge.c.left != 0) {
		result = UNITTEST_RESULT_FAIL;
	}
	if (root.base.content_edge.c.top != 0) {
		result = UNITTEST_RESULT_FAIL;
	}
	if (root.base.content_edge.c.right != 100) {
		result = UNITTEST_RESULT_FAIL;
	}
	if (root.base.content_edge.c.bottom != 100) {
		result = UNITTEST_RESULT_FAIL;
	}

	cssrendering_anonymousbox_finalize(&anon);
	cssrendering_blockbox_finalize(&root);

	return result;
}

LOCAL UNITTEST_RESULT test_cssrendering_blockbox_appendchild2()
{
	cssrendering_blockbox_t root;
	cssrendering_blockbox_t child;
	UNITTEST_RESULT result = UNITTEST_RESULT_PASS;

	cssrendering_blockbox_initialize(&root);
	cssrendering_blockbox_initialize(&child);

	root.base.content_edge = (cssmetric_rectangle_t){{0, 0, 100, 100}};
	cssrendering_blockbox_appendblockchild(&root, &child);

	if (root.base.content_edge.c.left != 0) {
		result = UNITTEST_RESULT_FAIL;
	}
	if (root.base.content_edge.c.top != 0) {
		result = UNITTEST_RESULT_FAIL;
	}
	if (root.base.content_edge.c.right != 100) {
		result = UNITTEST_RESULT_FAIL;
	}
	if (root.base.content_edge.c.bottom != 100) {
		result = UNITTEST_RESULT_FAIL;
	}

	cssrendering_blockbox_finalize(&child);
	cssrendering_blockbox_finalize(&root);

	return result;
}

LOCAL UNITTEST_RESULT test_cssrendering_anonymousbox_appendchild1()
{
	cssrendering_anonymousbox_t anon;
	cssrendering_linebox_t line;
	UNITTEST_RESULT result = UNITTEST_RESULT_PASS;

	cssrendering_anonymousbox_initialize(&anon);
	cssrendering_linebox_initialize(&line);

	anon.base.content_edge = (cssmetric_rectangle_t){{0, 0, 100, 100}};
	cssrendering_anonymousbox_appendchild(&anon, &line);

	if (anon.base.content_edge.c.left != 0) {
		result = UNITTEST_RESULT_FAIL;
	}
	if (anon.base.content_edge.c.top != 0) {
		result = UNITTEST_RESULT_FAIL;
	}
	if (anon.base.content_edge.c.right != 100) {
		result = UNITTEST_RESULT_FAIL;
	}
	if (anon.base.content_edge.c.bottom != 100) {
		result = UNITTEST_RESULT_FAIL;
	}

	cssrendering_linebox_finalize(&line);
	cssrendering_anonymousbox_finalize(&anon);

	return result;
}

LOCAL UNITTEST_RESULT test_cssrendering_anonymousbox_appendchild2()
{
	cssrendering_anonymousbox_t anon;
	cssrendering_linebox_t *line, line0, line1, line2;
	UNITTEST_RESULT result = UNITTEST_RESULT_PASS;

	cssrendering_anonymousbox_initialize(&anon);
	cssrendering_linebox_initialize(&line0);
	cssrendering_linebox_initialize(&line1);
	cssrendering_linebox_initialize(&line2);

	anon.base.content_edge = (cssmetric_rectangle_t){{0, 0, 100, 100}};
	cssrendering_anonymousbox_appendchild(&anon, &line0);
	cssrendering_anonymousbox_appendchild(&anon, &line1);
	cssrendering_anonymousbox_appendchild(&anon, &line2);

	line = cssrendering_anonymousbox_getfirstchild(&anon);
	if (line != &line0) {
		result = UNITTEST_RESULT_FAIL;
	}
	line = cssrendering_linebox_getnextsibling(&line0);
	if (line != &line1) {
		result = UNITTEST_RESULT_FAIL;
	}
	line = cssrendering_linebox_getnextsibling(&line1);
	if (line != &line2) {
		result = UNITTEST_RESULT_FAIL;
	}
	line = cssrendering_linebox_getnextsibling(&line2);
	if (line != NULL) {
		result = UNITTEST_RESULT_FAIL;
	}

	cssrendering_linebox_finalize(&line2);
	cssrendering_linebox_finalize(&line1);
	cssrendering_linebox_finalize(&line0);
	cssrendering_anonymousbox_finalize(&anon);

	return result;
}

LOCAL UNITTEST_RESULT test_cssrendering_anonymousbox_insertbefore1()
{
	cssrendering_anonymousbox_t anon;
	cssrendering_linebox_t *line, line0, line1, line2;
	UNITTEST_RESULT result = UNITTEST_RESULT_PASS;

	cssrendering_anonymousbox_initialize(&anon);
	cssrendering_linebox_initialize(&line0);
	cssrendering_linebox_initialize(&line1);
	cssrendering_linebox_initialize(&line2);

	anon.base.content_edge = (cssmetric_rectangle_t){{0, 0, 100, 100}};
	cssrendering_anonymousbox_insertbefore(&anon, &line0, NULL);
	cssrendering_anonymousbox_insertbefore(&anon, &line1, &line0);
	cssrendering_anonymousbox_insertbefore(&anon, &line2, NULL);

	line = cssrendering_anonymousbox_getfirstchild(&anon);
	if (line != &line1) {
		result = UNITTEST_RESULT_FAIL;
	}
	line = cssrendering_linebox_getnextsibling(&line1);
	if (line != &line0) {
		result = UNITTEST_RESULT_FAIL;
	}
	line = cssrendering_linebox_getnextsibling(&line0);
	if (line != &line2) {
		result = UNITTEST_RESULT_FAIL;
	}
	line = cssrendering_linebox_getnextsibling(&line2);
	if (line != NULL) {
		result = UNITTEST_RESULT_FAIL;
	}

	cssrendering_linebox_finalize(&line2);
	cssrendering_linebox_finalize(&line1);
	cssrendering_linebox_finalize(&line0);
	cssrendering_anonymousbox_finalize(&anon);

	return result;
}

LOCAL UNITTEST_RESULT test_cssrendering_anonymousbox_removechild1()
{
	cssrendering_anonymousbox_t anon;
	cssrendering_linebox_t *line, line0, line1, line2;
	UNITTEST_RESULT result = UNITTEST_RESULT_PASS;

	cssrendering_anonymousbox_initialize(&anon);
	cssrendering_linebox_initialize(&line0);
	cssrendering_linebox_initialize(&line1);
	cssrendering_linebox_initialize(&line2);

	anon.base.content_edge = (cssmetric_rectangle_t){{0, 0, 100, 100}};
	cssrendering_anonymousbox_appendchild(&anon, &line0);
	cssrendering_anonymousbox_appendchild(&anon, &line1);
	cssrendering_anonymousbox_appendchild(&anon, &line2);
	cssrendering_anonymousbox_removechild(&anon, &line1);

	line = cssrendering_anonymousbox_getfirstchild(&anon);
	if (line != &line0) {
		result = UNITTEST_RESULT_FAIL;
	}
	line = cssrendering_linebox_getnextsibling(&line0);
	if (line != &line2) {
		result = UNITTEST_RESULT_FAIL;
	}
	line = cssrendering_linebox_getnextsibling(&line2);
	if (line != NULL) {
		result = UNITTEST_RESULT_FAIL;
	}

	line = cssrendering_linebox_getnextsibling(&line1);
	if (line != NULL) {
		result = UNITTEST_RESULT_FAIL;
	}

	cssrendering_linebox_finalize(&line2);
	cssrendering_linebox_finalize(&line1);
	cssrendering_linebox_finalize(&line0);
	cssrendering_anonymousbox_finalize(&anon);

	return result;
}

EXPORT VOID test_cssrendering_box_main(unittest_driver_t *driver)
{
	UNITTEST_DRIVER_REGIST(driver, test_cssrendering_drawtraversal_1);
	UNITTEST_DRIVER_REGIST(driver, test_cssrendering_drawtraversal_2);
	UNITTEST_DRIVER_REGIST(driver, test_cssrendering_drawtraversal_3);
	UNITTEST_DRIVER_REGIST(driver, test_cssrendering_hittraversal_1);
	UNITTEST_DRIVER_REGIST(driver, test_cssrendering_blockbox_appendchild1);
	UNITTEST_DRIVER_REGIST(driver, test_cssrendering_blockbox_appendchild2);
	UNITTEST_DRIVER_REGIST(driver, test_cssrendering_anonymousbox_appendchild1);
	UNITTEST_DRIVER_REGIST(driver, test_cssrendering_anonymousbox_appendchild2);
	UNITTEST_DRIVER_REGIST(driver, test_cssrendering_anonymousbox_insertbefore1);
	UNITTEST_DRIVER_REGIST(driver, test_cssrendering_anonymousbox_removechild1);
}

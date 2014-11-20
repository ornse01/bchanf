/*
 * test_treebase.c
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

#include "test_coll.h"

#include "treebase.h"
 
#include	<basic.h>
#include	<bstdio.h>

#include    <unittest_driver.h>

typedef struct {
	treebase_node_t *parent;
	treebase_node_t *next;
	treebase_node_t *prev;
	treebase_node_t *first;
	treebase_node_t *last;
	Bool has_child;
} test_treebase_node_expected_t;

LOCAL Bool test_treebase_node_commoncheck(treebase_node_t *node, test_treebase_node_expected_t *expected, UB *msg)
{
	treebase_node_t *node1;
	Bool ret = True, ok;

	node1 = treebase_node_getparent(node);
	if (node1 != expected->parent) {
		printf("%s: parent is not expected\n", msg);
		ret = False;
	}
	node1 = treebase_node_getnextsibling(node);
	if (node1 != expected->next) {
		printf("%s: next sibling is not expected\n", msg);
		ret = False;
	}
	node1 = treebase_node_getprevsibling(node);
	if (node1 != expected->prev) {
		printf("%s: prev sibling is not expected\n", msg);
		ret = False;
	}
	node1 = treebase_node_getfirstchild(node);
	if (node1 != expected->first) {
		printf("%s: first child is not expected\n", msg);
		ret = False;
	}
	node1 = treebase_node_getlastchild(node);
	if (node1 != expected->last) {
		printf("%s: last child is not expected\n", msg);
		ret = False;
	}
	ok = treebase_node_haschild(node);
	if (ok != expected->has_child) {
		printf("%s: has child is not expected\n", msg);
		ret = False;
	}

	return ret;
}

LOCAL UNITTEST_RESULT test_treebase_node_1()
{
	treebase_node_t node;

	treebase_node_initialize(&node);
	treebase_node_finalize(&node);

	return UNITTEST_RESULT_PASS;
}

LOCAL UNITTEST_RESULT test_treebase_node_2()
{
	treebase_node_t node;
	test_treebase_node_expected_t expected;
	Bool ok;
	UNITTEST_RESULT ret = UNITTEST_RESULT_PASS;

	treebase_node_initialize(&node);

	expected.parent = NULL;
	expected.next = NULL;
	expected.prev = NULL;
	expected.first = NULL;
	expected.last = NULL;
	expected.has_child = False;
	ok = test_treebase_node_commoncheck(&node, &expected, "node");
	if (ok == False) {
		ret = UNITTEST_RESULT_FAIL;
	}

	treebase_node_finalize(&node);

	return ret;
}

LOCAL UNITTEST_RESULT test_treebase_node_3()
{
	treebase_node_t node0, node1;
	test_treebase_node_expected_t expected;
	Bool ok;
	UNITTEST_RESULT ret = UNITTEST_RESULT_PASS;

	treebase_node_initialize(&node0);
	treebase_node_initialize(&node1);

	treebase_node_appendchild(&node0, &node1);

	/**/
	expected.parent = NULL;
	expected.next = NULL;
	expected.prev = NULL;
	expected.first = &node1;
	expected.last = &node1;
	expected.has_child = True;
	ok = test_treebase_node_commoncheck(&node0, &expected, "node0");
	if (ok == False) {
		ret = UNITTEST_RESULT_FAIL;
	}
	/**/
	expected.parent = &node0;
	expected.next = NULL;
	expected.prev = NULL;
	expected.first = NULL;
	expected.last = NULL;
	expected.has_child = False;
	ok = test_treebase_node_commoncheck(&node1, &expected, "node1");
	if (ok == False) {
		ret = UNITTEST_RESULT_FAIL;
	}
	
	treebase_node_finalize(&node1);
	treebase_node_finalize(&node0);

	return ret;
}

LOCAL UNITTEST_RESULT test_treebase_node_4()
{
	treebase_node_t node0, node1;
	test_treebase_node_expected_t expected;
	Bool ok;
	UNITTEST_RESULT ret = UNITTEST_RESULT_PASS;

	treebase_node_initialize(&node0);
	treebase_node_initialize(&node1);

	treebase_node_appendchild(&node0, &node1);
	treebase_node_remove(&node1);

	/**/
	expected.parent = NULL;
	expected.next = NULL;
	expected.prev = NULL;
	expected.first = NULL;
	expected.last = NULL;
	expected.has_child = False;
	ok = test_treebase_node_commoncheck(&node0, &expected, "node0");
	if (ok == False) {
		ret = UNITTEST_RESULT_FAIL;
	}
	/**/
	expected.parent = NULL;
	expected.next = NULL;
	expected.prev = NULL;
	expected.first = NULL;
	expected.last = NULL;
	expected.has_child = False;
	ok = test_treebase_node_commoncheck(&node1, &expected, "node1");
	if (ok == False) {
		ret = UNITTEST_RESULT_FAIL;
	}
	
	treebase_node_finalize(&node1);
	treebase_node_finalize(&node0);

	return ret;
}

LOCAL UNITTEST_RESULT test_treebase_node_5()
{
	treebase_node_t node0, node1, node2;
	test_treebase_node_expected_t expected;
	Bool ok;
	UNITTEST_RESULT ret = UNITTEST_RESULT_PASS;

	treebase_node_initialize(&node0);
	treebase_node_initialize(&node1);
	treebase_node_initialize(&node2);

	treebase_node_appendchild(&node0, &node1);
	treebase_node_appendchild(&node0, &node2);

	/**/
	expected.parent = NULL;
	expected.next = NULL;
	expected.prev = NULL;
	expected.first = &node1;
	expected.last = &node2;
	expected.has_child = True;
	ok = test_treebase_node_commoncheck(&node0, &expected, "node0");
	if (ok == False) {
		ret = UNITTEST_RESULT_FAIL;
	}
	/**/
	expected.parent = &node0;
	expected.next = &node2;
	expected.prev = NULL;
	expected.first = NULL;
	expected.last = NULL;
	expected.has_child = False;
	ok = test_treebase_node_commoncheck(&node1, &expected, "node1");
	if (ok == False) {
		ret = UNITTEST_RESULT_FAIL;
	}
	/**/
	expected.parent = &node0;
	expected.next = NULL;
	expected.prev = &node1;
	expected.first = NULL;
	expected.last = NULL;
	expected.has_child = False;
	ok = test_treebase_node_commoncheck(&node2, &expected, "node2");
	if (ok == False) {
		ret = UNITTEST_RESULT_FAIL;
	}
	
	treebase_node_finalize(&node2);
	treebase_node_finalize(&node1);
	treebase_node_finalize(&node0);

	return ret;
}

LOCAL UNITTEST_RESULT test_treebase_node_6()
{
	treebase_node_t node0, node1, node2;
	test_treebase_node_expected_t expected;
	Bool ok;
	UNITTEST_RESULT ret = UNITTEST_RESULT_PASS;

	treebase_node_initialize(&node0);
	treebase_node_initialize(&node1);
	treebase_node_initialize(&node2);

	treebase_node_appendchild(&node0, &node1);
	treebase_node_appendchild(&node1, &node2);

	/**/
	expected.parent = NULL;
	expected.next = NULL;
	expected.prev = NULL;
	expected.first = &node1;
	expected.last = &node1;
	expected.has_child = True;
	ok = test_treebase_node_commoncheck(&node0, &expected, "node0");
	if (ok == False) {
		ret = UNITTEST_RESULT_FAIL;
	}
	/**/
	expected.parent = &node0;
	expected.next = NULL;
	expected.prev = NULL;
	expected.first = &node2;
	expected.last = &node2;
	expected.has_child = True;
	ok = test_treebase_node_commoncheck(&node1, &expected, "node1");
	if (ok == False) {
		ret = UNITTEST_RESULT_FAIL;
	}
	/**/
	expected.parent = &node1;
	expected.next = NULL;
	expected.prev = NULL;
	expected.first = NULL;
	expected.last = NULL;
	expected.has_child = False;
	ok = test_treebase_node_commoncheck(&node2, &expected, "node2");
	if (ok == False) {
		ret = UNITTEST_RESULT_FAIL;
	}
	
	treebase_node_finalize(&node2);
	treebase_node_finalize(&node1);
	treebase_node_finalize(&node0);

	return ret;
}

LOCAL UNITTEST_RESULT test_treebase_node_7()
{
	treebase_node_t node0, node1, node2, node3;
	test_treebase_node_expected_t expected;
	Bool ok;
	UNITTEST_RESULT ret = UNITTEST_RESULT_PASS;

	treebase_node_initialize(&node0);
	treebase_node_initialize(&node1);
	treebase_node_initialize(&node2);
	treebase_node_initialize(&node3);

	treebase_node_insertbefore(&node0, &node1, NULL);
	treebase_node_insertbefore(&node0, &node2, NULL);
	treebase_node_insertbefore(&node0, &node3, &node2);

	/**/
	expected.parent = NULL;
	expected.next = NULL;
	expected.prev = NULL;
	expected.first = &node1;
	expected.last = &node2;
	expected.has_child = True;
	ok = test_treebase_node_commoncheck(&node0, &expected, "node0");
	if (ok == False) {
		ret = UNITTEST_RESULT_FAIL;
	}
	/**/
	expected.parent = &node0;
	expected.next = &node3;
	expected.prev = NULL;
	expected.first = NULL;
	expected.last = NULL;
	expected.has_child = False;
	ok = test_treebase_node_commoncheck(&node1, &expected, "node1");
	if (ok == False) {
		ret = UNITTEST_RESULT_FAIL;
	}
	/**/
	expected.parent = &node0;
	expected.next = NULL;
	expected.prev = &node3;
	expected.first = NULL;
	expected.last = NULL;
	expected.has_child = False;
	ok = test_treebase_node_commoncheck(&node2, &expected, "node2");
	if (ok == False) {
		ret = UNITTEST_RESULT_FAIL;
	}
	/**/
	expected.parent = &node0;
	expected.next = &node2;
	expected.prev = &node1;
	expected.first = NULL;
	expected.last = NULL;
	expected.has_child = False;
	ok = test_treebase_node_commoncheck(&node3, &expected, "node3");
	if (ok == False) {
		ret = UNITTEST_RESULT_FAIL;
	}

	treebase_node_finalize(&node3);
	treebase_node_finalize(&node2);
	treebase_node_finalize(&node1);
	treebase_node_finalize(&node0);

	return ret;
}

/* treebase_preordertraversal_t */

typedef struct {
	Bool cont;
	treebase_node_t *node;
	TREEBASE_TRAVERSAL_DIRECTION dir;
} test_treebase_preordertraversal_expected_t;

LOCAL UNITTEST_RESULT test_treebase_preordertraversal_common(treebase_node_t *root, test_treebase_preordertraversal_expected_t *expected, W expected_len)
{
	treebase_node_t *node_result;
	treebase_preordertraversal_t traversal;
	TREEBASE_TRAVERSAL_DIRECTION dir;
	Bool cont;
	UNITTEST_RESULT ret = UNITTEST_RESULT_PASS;
	W i;

	treebase_preordertraversal_initialize(&traversal, root);

	for (i = 0;; i++) {
		cont = treebase_preordertraversal_next(&traversal, &node_result, &dir);
		if (i < expected_len) {
			if (expected[i].cont != cont) {
				printf("%d: cont is not expected\n", i);
				ret = UNITTEST_RESULT_FAIL;
			}
			if (expected[i].node != node_result) {
				printf("%d: node is not expected\n", i);
				ret = UNITTEST_RESULT_FAIL;
			}
			if (expected[i].dir != dir) {
				printf("%d: direction is not expected\n", i);
				ret = UNITTEST_RESULT_FAIL;
			}
		}

		if (cont == False) {
			break;
		}
	}
	if (i != expected_len) {
		printf("node number check fail: expected = %d, result = %d\n", expected_len, i);
		ret = UNITTEST_RESULT_FAIL;
	}

	treebase_preordertraversal_finalize(&traversal);

	return ret;
}

LOCAL UNITTEST_RESULT test_treebase_preordertraversal_1()
{
	treebase_node_t node0, node1, node2, node3, node4;
	test_treebase_preordertraversal_expected_t expected[] = {
		{ True, &node0, TREEBASE_TRAVERSAL_DIRECTION_DOWN },
		{ True, &node1, TREEBASE_TRAVERSAL_DIRECTION_DOWN },
		{ True, &node2, TREEBASE_TRAVERSAL_DIRECTION_DOWN },
		{ True, &node2, TREEBASE_TRAVERSAL_DIRECTION_UP },
		{ True, &node3, TREEBASE_TRAVERSAL_DIRECTION_DOWN },
		{ True, &node3, TREEBASE_TRAVERSAL_DIRECTION_UP },
		{ True, &node4, TREEBASE_TRAVERSAL_DIRECTION_DOWN },
		{ True, &node4, TREEBASE_TRAVERSAL_DIRECTION_UP },
		{ True, &node1, TREEBASE_TRAVERSAL_DIRECTION_UP },
		{ True, &node0, TREEBASE_TRAVERSAL_DIRECTION_UP },
	};
	W expected_len = sizeof(expected) / sizeof(test_treebase_preordertraversal_expected_t);
	UNITTEST_RESULT ret;

	treebase_node_initialize(&node0);
	treebase_node_initialize(&node1);
	treebase_node_initialize(&node2);
	treebase_node_initialize(&node3);
	treebase_node_initialize(&node4);

	treebase_node_appendchild(&node0, &node1);
	treebase_node_appendchild(&node1, &node2);
	treebase_node_appendchild(&node1, &node3);
	treebase_node_appendchild(&node1, &node4);

	ret = test_treebase_preordertraversal_common(&node0, expected, expected_len);

	treebase_node_finalize(&node4);
	treebase_node_finalize(&node3);
	treebase_node_finalize(&node2);
	treebase_node_finalize(&node1);
	treebase_node_finalize(&node0);

	return ret;
}


EXPORT VOID test_treebase_main(unittest_driver_t *driver)
{
	UNITTEST_DRIVER_REGIST(driver, test_treebase_node_1);
	UNITTEST_DRIVER_REGIST(driver, test_treebase_node_2);
	UNITTEST_DRIVER_REGIST(driver, test_treebase_node_3);
	UNITTEST_DRIVER_REGIST(driver, test_treebase_node_4);
	UNITTEST_DRIVER_REGIST(driver, test_treebase_node_5);
	UNITTEST_DRIVER_REGIST(driver, test_treebase_node_6);
	UNITTEST_DRIVER_REGIST(driver, test_treebase_node_7);
	UNITTEST_DRIVER_REGIST(driver, test_treebase_preordertraversal_1);
}

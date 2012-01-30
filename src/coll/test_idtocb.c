/*
 * test_idtocb.c
 *
 * Copyright (c) 2012 project bchan
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

#include "idtocb.h"

#include	<basic.h>
#include	<bstdio.h>
#include	<bstdlib.h>
#include	<bstring.h>
#include	<errcode.h>

#include    <unittest_driver.h>

typedef struct {
	idtocb_entry_t base;
	W i;
} test_idtocb_1_t;

LOCAL UNITTEST_RESULT test_idtocb_1()
{
#define TEST_IDTOCB_1_TESTSIZE 10
	idtocb_t *cb;
	ID ids[TEST_IDTOCB_1_TESTSIZE];
	test_idtocb_1_t *p;
	idtocb_iterator_t iter;
	W i, j, err;
	Bool cont;
	UNITTEST_RESULT ret = UNITTEST_RESULT_PASS;

	cb = idtocb_new(sizeof(test_idtocb_1_t), TEST_IDTOCB_1_TESTSIZE);
	if (cb == NULL) {
		return UNITTEST_RESULT_FAIL;
	}

	for (i = 0; i < TEST_IDTOCB_1_TESTSIZE; i++) {
		ids[i] = idtocb_allocate(cb);
		if (ids[i] < 0) {
			printf("idtocb_allocate %d error\n", i);
			ret = UNITTEST_RESULT_FAIL;
			break;
		}
	}

	for (i = 0; i < TEST_IDTOCB_1_TESTSIZE; i++) {
		err = idtocb_getcontrolblock(cb, ids[i], (idtocb_entry_t**)&p);
		if (err < 0) {
			printf("idtocb_getcontrolblock error: %08x\n", err);
			ret = UNITTEST_RESULT_FAIL;
			break;
		}
		p->i = i;
	}

	for (i = 0; i < TEST_IDTOCB_1_TESTSIZE; i++) {
		err = idtocb_getcontrolblock(cb, ids[i], (idtocb_entry_t**)&p);
		if (err < 0) {
			printf("idtocb_getcontrolblock error: %08x\n", err);
			ret = UNITTEST_RESULT_FAIL;
			break;
		}
		if (p->i != i) {
			printf("idtocb_getcontrolblock result is not expected\n");
			ret = UNITTEST_RESULT_FAIL;
			break;
		}
	}

	i = 0;
	idtocb_iterator_initialize(&iter, cb);
	for (;;) {
		cont = idtocb_iterator_next(&iter, (idtocb_entry_t**)&p);
		if (cont == False) {
			break;
		}
		i++;
		for (j = 0; j < TEST_IDTOCB_1_TESTSIZE; j++) {
			if (p->base.id == ids[j]) {
				if (p->i != j) {
					printf("idtocb_iterator return value irregal\n");
					ret = UNITTEST_RESULT_FAIL;
					break;
				}
			}
		}
	}
	idtocb_iterator_finalize(&iter);
	if (i != TEST_IDTOCB_1_TESTSIZE) {
		printf("idtocb_iterator iteration number is not expected:1\n");
		ret = UNITTEST_RESULT_FAIL;
	}

	for (i = 0; i < TEST_IDTOCB_1_TESTSIZE; i++) {
		idtocb_free(cb, ids[i]);
	}

	i = 0;
	idtocb_iterator_initialize(&iter, cb);
	for (;;) {
		cont = idtocb_iterator_next(&iter, (idtocb_entry_t**)&p);
		if (cont == False) {
			break;
		}
		i++;
	}
	idtocb_iterator_finalize(&iter);
	if (i != 0) {
		printf("idtocb_iterator iteration number is not expected:2\n");
		ret = UNITTEST_RESULT_FAIL;
	}

	idtocb_delete(cb);

	return ret;
}

typedef struct {
	idtocb_entry_t base;
	W i;
	ID id;
} test_idtocb_2_t;

LOCAL Bool test_idtocb_2_setvalue(idtocb_t *cb, ID *ids, W len)
{
	test_idtocb_2_t *p;
	W i, err;

	for (i = 0; i < len; i++) {
		if (ids[i] < 0) {
			continue;
		}

		err = idtocb_getcontrolblock(cb, ids[i], (idtocb_entry_t**)&p);
		if (err < 0) {
			printf("idtocb_getcontrolblock error: %08x\n", err);
			return False;
		}
		p->i = i;
		p->id = p->base.id;
	}

	return True;
}

LOCAL Bool test_idtocb_2_checkvalue(idtocb_t *cb, ID *ids, W len)
{
	test_idtocb_2_t *p;
	W i, j, err;
	idtocb_iterator_t iter;
	Bool cont;

	for (i = 0; i < len; i++) {
		if (ids[i] < 0) {
			continue;
		}

		err = idtocb_getcontrolblock(cb, ids[i], (idtocb_entry_t**)&p);
		if (err < 0) {
			printf("idtocb_getcontrolblock error: %08x\n", err);
			return False;
		}
		if (p->i != i) {
			return False;
		}
		if (p->id != p->base.id) {
			return False;
		}
	}

	idtocb_iterator_initialize(&iter, cb);
	for (;;) {
		cont = idtocb_iterator_next(&iter, (idtocb_entry_t**)&p);
		if (cont == False) {
			break;
		}
		for (j = 0; j < len; j++) {
			if (p->base.id == ids[j]) {
				if (p->i != j) {
					printf("idtocb_iterator return value irregal: i\n");
					idtocb_iterator_finalize(&iter);
					return False;
				}
				if (p->id != ids[j]) {
					printf("idtocb_iterator return value irregal: id\n");
					idtocb_iterator_finalize(&iter);
					return False;
				}
			}
		}
	}
	idtocb_iterator_finalize(&iter);

	return True;
}

LOCAL Bool test_idtocb_2_checklength(idtocb_t *cb, W expected_len)
{
	test_idtocb_2_t *p;
	W i;
	idtocb_iterator_t iter;
	Bool cont;

	i = 0;
	idtocb_iterator_initialize(&iter, cb);
	for (;;) {
		cont = idtocb_iterator_next(&iter, (idtocb_entry_t**)&p);
		if (cont == False) {
			break;
		}
		i++;
	}
	idtocb_iterator_finalize(&iter);

	if (i != expected_len) {
		printf("iteration number is not expected\n");
		return False;
	}
	return True;
}

LOCAL UNITTEST_RESULT test_idtocb_2()
{
#define TEST_IDTOCB_2_TESTSIZE 15
	idtocb_t *cb;
	ID ids[TEST_IDTOCB_2_TESTSIZE];
	W i;
	Bool is_pass;
	UNITTEST_RESULT ret = UNITTEST_RESULT_PASS;

	cb = idtocb_new(sizeof(test_idtocb_2_t), TEST_IDTOCB_2_TESTSIZE);
	if (cb == NULL) {
		return UNITTEST_RESULT_FAIL;
	}

	is_pass = test_idtocb_2_checklength(cb, 0);
	if (is_pass == False) {
		ret = UNITTEST_RESULT_FAIL;
	}

	for (i = 0; i < TEST_IDTOCB_2_TESTSIZE; i++) {
		ids[i] = idtocb_allocate(cb);
		if (ids[i] < 0) {
			printf("idtocb_allocate %d error\n", i);
			ret = UNITTEST_RESULT_FAIL;
			break;
		}
	}

	is_pass = test_idtocb_2_checklength(cb, TEST_IDTOCB_2_TESTSIZE);
	if (is_pass == False) {
		ret = UNITTEST_RESULT_FAIL;
	}
	is_pass = test_idtocb_2_setvalue(cb, ids, TEST_IDTOCB_2_TESTSIZE);
	if (is_pass == False) {
		ret = UNITTEST_RESULT_FAIL;
	}
	is_pass = test_idtocb_2_checkvalue(cb, ids, TEST_IDTOCB_2_TESTSIZE);
	if (is_pass == False) {
		ret = UNITTEST_RESULT_FAIL;
	}

	idtocb_free(cb, ids[3]);
	ids[3] = -1;
	idtocb_free(cb, ids[5]);
	ids[5] = -1;
	idtocb_free(cb, ids[7]);
	ids[7] = -1;
	idtocb_free(cb, ids[8]);
	ids[8] = -1;

	is_pass = test_idtocb_2_checklength(cb, TEST_IDTOCB_2_TESTSIZE - 4);
	if (is_pass == False) {
		ret = UNITTEST_RESULT_FAIL;
	}
	is_pass = test_idtocb_2_checkvalue(cb, ids, TEST_IDTOCB_2_TESTSIZE);
	if (is_pass == False) {
		ret = UNITTEST_RESULT_FAIL;
	}

	ids[3] = idtocb_allocate(cb);
	if (ids[3] < 0) {
		printf("idtocb_allocate %d error\n", 3);
		ret = UNITTEST_RESULT_FAIL;
	}
	ids[7] = idtocb_allocate(cb);
	if (ids[7] < 0) {
		printf("idtocb_allocate %d error\n", 7);
		ret = UNITTEST_RESULT_FAIL;
	}
	ids[8] = idtocb_allocate(cb);
	if (ids[8] < 0) {
		printf("idtocb_allocate %d error\n", 8);
		ret = UNITTEST_RESULT_FAIL;
	}
	ids[5] = idtocb_allocate(cb);
	if (ids[5] < 0) {
		printf("idtocb_allocate %d error\n", 5);
		ret = UNITTEST_RESULT_FAIL;
	}
	is_pass = test_idtocb_2_setvalue(cb, ids, TEST_IDTOCB_2_TESTSIZE);
	if (is_pass == False) {
		ret = UNITTEST_RESULT_FAIL;
	}
	is_pass = test_idtocb_2_checkvalue(cb, ids, TEST_IDTOCB_2_TESTSIZE);
	if (is_pass == False) {
		ret = UNITTEST_RESULT_FAIL;
	}
	is_pass = test_idtocb_2_checklength(cb, TEST_IDTOCB_2_TESTSIZE);
	if (is_pass == False) {
		ret = UNITTEST_RESULT_FAIL;
	}

	idtocb_free(cb, ids[1]);
	ids[1] = -1;
	idtocb_free(cb, ids[2]);
	ids[2] = -1;
	idtocb_free(cb, ids[9]);
	ids[9] = -1;
	idtocb_free(cb, ids[6]);
	ids[6] = -1;
	is_pass = test_idtocb_2_checkvalue(cb, ids, TEST_IDTOCB_2_TESTSIZE);
	if (is_pass == False) {
		ret = UNITTEST_RESULT_FAIL;
	}
	is_pass = test_idtocb_2_checklength(cb, TEST_IDTOCB_2_TESTSIZE - 4);
	if (is_pass == False) {
		ret = UNITTEST_RESULT_FAIL;
	}

	ids[2] = idtocb_allocate(cb);
	if (ids[2] < 0) {
		printf("idtocb_allocate %d error\n", 2);
		ret = UNITTEST_RESULT_FAIL;
	}
	is_pass = test_idtocb_2_setvalue(cb, ids, TEST_IDTOCB_2_TESTSIZE);
	if (is_pass == False) {
		ret = UNITTEST_RESULT_FAIL;
	}
	is_pass = test_idtocb_2_checkvalue(cb, ids, TEST_IDTOCB_2_TESTSIZE);
	if (is_pass == False) {
		ret = UNITTEST_RESULT_FAIL;
	}
	is_pass = test_idtocb_2_checklength(cb, TEST_IDTOCB_2_TESTSIZE - 3);
	if (is_pass == False) {
		ret = UNITTEST_RESULT_FAIL;
	}

	for (i = 0; i < TEST_IDTOCB_2_TESTSIZE; i++) {
		idtocb_free(cb, ids[i]);
	}

	idtocb_delete(cb);

	return ret;
}

LOCAL UNITTEST_RESULT test_idtocb_3()
{
#define TEST_IDTOCB_3_TESTSIZE 15
	idtocb_t *cb;
	ID id0, ids[TEST_IDTOCB_3_TESTSIZE];
	W i;
	UNITTEST_RESULT ret = UNITTEST_RESULT_PASS;
	Bool is_pass;

	cb = idtocb_new(sizeof(test_idtocb_2_t), TEST_IDTOCB_3_TESTSIZE);
	if (cb == NULL) {
		return UNITTEST_RESULT_FAIL;
	}

	for (i = 0; i < TEST_IDTOCB_3_TESTSIZE * 2; i++) {
		id0 = idtocb_allocate(cb);
		if (i < TEST_IDTOCB_3_TESTSIZE) {
			if (id0 < 0) {
				printf("idtocb_allocate %d error\n", i);
				ret = UNITTEST_RESULT_FAIL;
				break;
			}
			is_pass = test_idtocb_2_checklength(cb, i+1);
			if (is_pass == False) {
				ret = UNITTEST_RESULT_FAIL;
				break;
			}
		} else {
			if ((id0 & 0xFFFF0000) != ER_NOMEM) {
				printf("idtocb_allocate %d is not error\n", i);
				ret = UNITTEST_RESULT_FAIL;
				break;
			}
			is_pass = test_idtocb_2_checklength(cb, TEST_IDTOCB_3_TESTSIZE);
			if (is_pass == False) {
				ret = UNITTEST_RESULT_FAIL;
				break;
			}
		}
	}

	for (i = 0; i < TEST_IDTOCB_3_TESTSIZE; i++) {
		idtocb_free(cb, ids[i]);
	}

	idtocb_delete(cb);

	return ret;
}

EXPORT VOID test_idtocb_main(unittest_driver_t *driver)
{
	UNITTEST_DRIVER_REGIST(driver, test_idtocb_1);
	UNITTEST_DRIVER_REGIST(driver, test_idtocb_2);
	UNITTEST_DRIVER_REGIST(driver, test_idtocb_3);
}

/*
 * test_array.c
 *
 * Copyright (c) 2010-2012 project bchan
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

#include "array.h"

#include    <btron/btron.h>
#include    <bstdio.h>
#include    <tcode.h>

LOCAL UNITTEST_RESULT test_array_1()
{
	arraybase_t array;
	W i, err, *val;
	UNITTEST_RESULT result = UNITTEST_RESULT_PASS;
	Bool found;

	err = arraybase_initialize(&array, sizeof(W), 100);
	if (err < 0) {
		return UNITTEST_RESULT_FAIL;
	}

	for (i = 0; i < 100; i++) {
		err = arraybase_appendunit(&array, (VP)&i);
		if (err < 0) {
			printf("arraybase_appendunit failure\n");
			result = UNITTEST_RESULT_FAIL;
		}
	}
	for (i = 0; i < 100; i++) {
		found = arraybase_getunitbyindex(&array, i, (VP)&val);
		if (found != True) {
			printf("arraybase_getunitbyindex not found failure\n");
			result = UNITTEST_RESULT_FAIL;
		}
		if (*val != i) {
			printf("arraybase_getunitbyindex unexpected value failure: %d:%d\n", i, *val);
			result = UNITTEST_RESULT_FAIL;
		}
	}

	arraybase_finalize(&array);

	return result;
}

LOCAL UNITTEST_RESULT test_array_2()
{
	arraybase_t array;
	W i, err, *val;
	UNITTEST_RESULT result = UNITTEST_RESULT_PASS;
	Bool found;

	err = arraybase_initialize(&array, sizeof(W), 100);
	if (err < 0) {
		return UNITTEST_RESULT_FAIL;
	}

	for (i = 0; i < 200; i++) {
		err = arraybase_appendunit(&array, (VP)&i);
		if (err < 0) {
			printf("arraybase_appendunit failure\n");
			result = UNITTEST_RESULT_FAIL;
		}
	}
	for (i = 0; i < 200; i++) {
		found = arraybase_getunitbyindex(&array, i, (VP)&val);
		if (found != True) {
			printf("arraybase_getunitbyindex not found failure\n");
			result = UNITTEST_RESULT_FAIL;
		}
		if (*val != i) {
			printf("arraybase_getunitbyindex unexpected value failure: %d:%d\n", i, *val);
			result = UNITTEST_RESULT_FAIL;
		}
	}

	arraybase_finalize(&array);

	return result;
}

LOCAL UNITTEST_RESULT test_array_3()
{
	arraybase_t array;
	W i, err, *val;
	UNITTEST_RESULT result = UNITTEST_RESULT_PASS;
	Bool found;

	err = arraybase_initialize(&array, sizeof(W), 100);
	if (err < 0) {
		return UNITTEST_RESULT_FAIL;
	}

	for (i = 0; i < 2000; i++) {
		err = arraybase_appendunit(&array, (VP)&i);
		if (err < 0) {
			printf("arraybase_appendunit failure\n");
			result = UNITTEST_RESULT_FAIL;
		}
	}
	for (i = 0; i < 2000; i++) {
		found = arraybase_getunitbyindex(&array, i, (VP)&val);
		if (found != True) {
			printf("arraybase_getunitbyindex not found failure\n");
			result = UNITTEST_RESULT_FAIL;
		}
		if (*val != i) {
			printf("arraybase_getunitbyindex unexpected value failure: %d:%d\n", i, *val);
			result = UNITTEST_RESULT_FAIL;
		}
	}

	arraybase_finalize(&array);

	return result;
}

LOCAL UNITTEST_RESULT test_array_4()
{
	arraybase_t array;
	W i, err, *val;
	UNITTEST_RESULT result = UNITTEST_RESULT_PASS;
	Bool found;

	err = arraybase_initialize(&array, sizeof(W), 100);
	if (err < 0) {
		return UNITTEST_RESULT_FAIL;
	}

	for (i = 0; i < 100; i++) {
		err = arraybase_appendunit(&array, (VP)&i);
		if (err < 0) {
			printf("arraybase_appendunit failure\n");
			result = UNITTEST_RESULT_FAIL;
		}
	}
	for (i = 0; i < 200; i++) {
		found = arraybase_getunitbyindex(&array, i, (VP)&val);
		if (i < 100) {
			if (found != True) {
				printf("arraybase_getunitbyindex not found failure\n");
				result = UNITTEST_RESULT_FAIL;
			}
			if (*val != i) {
				printf("arraybase_getunitbyindex unexpected value failure: %d:%d\n", i, *val);
				result = UNITTEST_RESULT_FAIL;
			}
		} else {
			if (found != False) {
				printf("arraybase_getunitbyindex found failure\n");
				result = UNITTEST_RESULT_FAIL;
			}
		}
	}

	arraybase_finalize(&array);

	return result;
}

LOCAL UNITTEST_RESULT test_array_5()
{
	arraybase_t array;
	W i, err, *val;
	UNITTEST_RESULT result = UNITTEST_RESULT_PASS;
	Bool found;

	err = arraybase_initialize(&array, sizeof(W), 100);
	if (err < 0) {
		return UNITTEST_RESULT_FAIL;
	}

	for (i = 0; i < 50; i++) {
		err = arraybase_appendunit(&array, (VP)&i);
		if (err < 0) {
			printf("arraybase_appendunit failure\n");
			result = UNITTEST_RESULT_FAIL;
		}
	}
	for (i = 0; i < 200; i++) {
		found = arraybase_getunitbyindex(&array, i, (VP)&val);
		if (i < 50) {
			if (found != True) {
				printf("arraybase_getunitbyindex not found failure\n");
				result = UNITTEST_RESULT_FAIL;
			}
			if (*val != i) {
				printf("arraybase_getunitbyindex unexpected value failure: %d:%d\n", i, *val);
				result = UNITTEST_RESULT_FAIL;
			}
		} else {
			if (found != False) {
				printf("arraybase_getunitbyindex found failure\n");
				result = UNITTEST_RESULT_FAIL;
			}
		}
	}

	arraybase_finalize(&array);

	return result;
}

LOCAL UNITTEST_RESULT test_array_6()
{
	arraybase_t array;
	W i, err, *val;
	UNITTEST_RESULT result = UNITTEST_RESULT_PASS;
	Bool found;

	err = arraybase_initialize(&array, sizeof(W), 100);
	if (err < 0) {
		return UNITTEST_RESULT_FAIL;
	}

	for (i = 0; i < 150; i++) {
		err = arraybase_appendunit(&array, (VP)&i);
		if (err < 0) {
			printf("arraybase_appendunit failure\n");
			result = UNITTEST_RESULT_FAIL;
		}
	}
	for (i = 0; i < 200; i++) {
		found = arraybase_getunitbyindex(&array, i, (VP)&val);
		if (i < 150) {
			if (found != True) {
				printf("arraybase_getunitbyindex not found failure\n");
				result = UNITTEST_RESULT_FAIL;
			}
			if (*val != i) {
				printf("arraybase_getunitbyindex unexpected value failure: %d:%d\n", i, *val);
				result = UNITTEST_RESULT_FAIL;
			}
		} else {
			if (found != False) {
				printf("arraybase_getunitbyindex found failure\n");
				result = UNITTEST_RESULT_FAIL;
			}
		}
	}

	arraybase_finalize(&array);

	return result;
}

typedef struct {
	W a;
	W b;
	W c;
} test_array_dummydata;

LOCAL UNITTEST_RESULT test_array_7()
{
	arraybase_t array;
	W i, err;
	test_array_dummydata val, *val2;
	UNITTEST_RESULT result = UNITTEST_RESULT_PASS;
	Bool found;

	err = arraybase_initialize(&array, sizeof(test_array_dummydata), 100);
	if (err < 0) {
		return UNITTEST_RESULT_FAIL;
	}

	for (i = 0; i < 100; i++) {
		val.a = 100;
		val.b = i + 1234;
		val.c = i % 23 ;
		err = arraybase_appendunit(&array, (VP)&val);
		if (err < 0) {
			printf("arraybase_appendunit failure\n");
			result = UNITTEST_RESULT_FAIL;
		}
	}
	for (i = 0; i < 100; i++) {
		found = arraybase_getunitbyindex(&array, i, (VP)&val2);
		if (found != True) {
			printf("arraybase_getunitbyindex not found failure\n");
			result = UNITTEST_RESULT_FAIL;
		}
		if ((val2->a != 100)||(val2->b != i + 1234)||(val2->c != i % 23)) {
			printf("arraybase_getunitbyindex unexpected value failure: %d\n", i);
			result = UNITTEST_RESULT_FAIL;
		}
	}

	arraybase_finalize(&array);

	return result;
}

LOCAL UNITTEST_RESULT test_array_8()
{
	arraybase_t array;
	W i, err;
	test_array_dummydata val, *val2;
	UNITTEST_RESULT result = UNITTEST_RESULT_PASS;
	Bool found;

	err = arraybase_initialize(&array, sizeof(test_array_dummydata), 100);
	if (err < 0) {
		return UNITTEST_RESULT_FAIL;
	}

	for (i = 0; i < 200; i++) {
		val.a = 100;
		val.b = i + 1234;
		val.c = i % 23 ;
		err = arraybase_appendunit(&array, (VP)&val);
		if (err < 0) {
			printf("arraybase_appendunit failure\n");
			result = UNITTEST_RESULT_FAIL;
		}
	}
	for (i = 0; i < 200; i++) {
		found = arraybase_getunitbyindex(&array, i, (VP)&val2);
		if (found != True) {
			printf("arraybase_getunitbyindex not found failure\n");
			result = UNITTEST_RESULT_FAIL;
		}
		if ((val2->a != 100)||(val2->b != i + 1234)||(val2->c != i % 23)) {
			printf("arraybase_getunitbyindex unexpected value failure: %d\n", i);
			result = UNITTEST_RESULT_FAIL;
		}
	}

	arraybase_finalize(&array);

	return result;
}

LOCAL UNITTEST_RESULT test_array_9()
{
	arraybase_t array;
	W i, err;
	test_array_dummydata val, *val2;
	UNITTEST_RESULT result = UNITTEST_RESULT_PASS;
	Bool found;

	err = arraybase_initialize(&array, sizeof(test_array_dummydata), 100);
	if (err < 0) {
		return UNITTEST_RESULT_FAIL;
	}

	for (i = 0; i < 2000; i++) {
		val.a = 100;
		val.b = i + 1234;
		val.c = i % 23 ;
		err = arraybase_appendunit(&array, (VP)&val);
		if (err < 0) {
			printf("arraybase_appendunit failure\n");
			result = UNITTEST_RESULT_FAIL;
		}
	}
	for (i = 0; i < 2000; i++) {
		found = arraybase_getunitbyindex(&array, i, (VP)&val2);
		if (found != True) {
			printf("arraybase_getunitbyindex not found failure\n");
			result = UNITTEST_RESULT_FAIL;
		}
		if ((val2->a != 100)||(val2->b != i + 1234)||(val2->c != i % 23)) {
			printf("arraybase_getunitbyindex unexpected value failure: %d\n", i);
			result = UNITTEST_RESULT_FAIL;
		}
	}

	arraybase_finalize(&array);

	return result;
}

LOCAL UNITTEST_RESULT test_array_10()
{
	arraybase_t array;
	W i, err;
	test_array_dummydata val, *val2;
	UNITTEST_RESULT result = UNITTEST_RESULT_PASS;
	Bool found;

	err = arraybase_initialize(&array, sizeof(test_array_dummydata), 100);
	if (err < 0) {
		return UNITTEST_RESULT_FAIL;
	}

	for (i = 0; i < 100; i++) {
		val.a = 100;
		val.b = i + 1234;
		val.c = i % 23 ;
		err = arraybase_appendunit(&array, (VP)&val);
		if (err < 0) {
			printf("arraybase_appendunit failure\n");
			result = UNITTEST_RESULT_FAIL;
		}
	}
	for (i = 0; i < 200; i++) {
		found = arraybase_getunitbyindex(&array, i, (VP)&val2);
		if (i < 100) {
			if (found != True) {
				printf("arraybase_getunitbyindex not found failure\n");
				result = UNITTEST_RESULT_FAIL;
			}
			if ((val2->a != 100)||(val2->b != i + 1234)||(val2->c != i % 23)) {
				printf("arraybase_getunitbyindex unexpected value failure: %d\n", i);
				result = UNITTEST_RESULT_FAIL;
			}
		} else {
			if (found != False) {
				printf("arraybase_getunitbyindex found failure\n");
				result = UNITTEST_RESULT_FAIL;
			}
		}
	}

	arraybase_finalize(&array);

	return result;
}

LOCAL UNITTEST_RESULT test_array_11()
{
	arraybase_t array;
	W i, err;
	test_array_dummydata val, *val2;
	UNITTEST_RESULT result = UNITTEST_RESULT_PASS;
	Bool found;

	err = arraybase_initialize(&array, sizeof(test_array_dummydata), 100);
	if (err < 0) {
		return UNITTEST_RESULT_FAIL;
	}

	for (i = 0; i < 50; i++) {
		val.a = 100;
		val.b = i + 1234;
		val.c = i % 23 ;
		err = arraybase_appendunit(&array, (VP)&val);
		if (err < 0) {
			printf("arraybase_appendunit failure\n");
			result = UNITTEST_RESULT_FAIL;
		}
	}
	for (i = 0; i < 200; i++) {
		found = arraybase_getunitbyindex(&array, i, (VP)&val2);
		if (i < 50) {
			if (found != True) {
				printf("arraybase_getunitbyindex not found failure\n");
				result = UNITTEST_RESULT_FAIL;
			}
			if ((val2->a != 100)||(val2->b != i + 1234)||(val2->c != i % 23)) {
				printf("arraybase_getunitbyindex unexpected value failure: %d\n", i);
				result = UNITTEST_RESULT_FAIL;
			}
		} else {
			if (found != False) {
				printf("arraybase_getunitbyindex found failure\n");
				result = UNITTEST_RESULT_FAIL;
			}
		}
	}

	arraybase_finalize(&array);

	return result;
}

LOCAL UNITTEST_RESULT test_array_12()
{
	arraybase_t array;
	W i, err;
	test_array_dummydata val, *val2;
	UNITTEST_RESULT result = UNITTEST_RESULT_PASS;
	Bool found;

	err = arraybase_initialize(&array, sizeof(test_array_dummydata), 100);
	if (err < 0) {
		return UNITTEST_RESULT_FAIL;
	}

	for (i = 0; i < 150; i++) {
		val.a = 100;
		val.b = i + 1234;
		val.c = i % 23 ;
		err = arraybase_appendunit(&array, (VP)&val);
		if (err < 0) {
			printf("arraybase_appendunit failure\n");
			result = UNITTEST_RESULT_FAIL;
		}
	}
	for (i = 0; i < 200; i++) {
		found = arraybase_getunitbyindex(&array, i, (VP)&val2);
		if (i < 150) {
			if (found != True) {
				printf("arraybase_getunitbyindex not found failure\n");
				result = UNITTEST_RESULT_FAIL;
			}
			if ((val2->a != 100)||(val2->b != i + 1234)||(val2->c != i % 23)) {
				printf("arraybase_getunitbyindex unexpected value failure: %d\n", i);
				result = UNITTEST_RESULT_FAIL;
			}
		} else {
			if (found != False) {
				printf("arraybase_getunitbyindex found failure\n");
				result = UNITTEST_RESULT_FAIL;
			}
		}
	}

	arraybase_finalize(&array);

	return result;
}

LOCAL UNITTEST_RESULT test_array_truncate_base(W denom, W testnum, W truncate, W checklen)
{
	arraybase_t array;
	W i, err, arraylen;
	test_array_dummydata val, *val2;
	UNITTEST_RESULT result = UNITTEST_RESULT_PASS;
	Bool found;

	err = arraybase_initialize(&array, sizeof(test_array_dummydata), denom);
	if (err < 0) {
		return UNITTEST_RESULT_FAIL;
	}

	for (i = 0; i < testnum; i++) {
		val.a = 100;
		val.b = i + 1234;
		val.c = i % 23 ;
		err = arraybase_appendunit(&array, (VP)&val);
		if (err < 0) {
			printf("arraybase_appendunit failure\n");
			result = UNITTEST_RESULT_FAIL;
		}
	}

	arraybase_truncate(&array, truncate);

	if (truncate > testnum) {
		arraylen = testnum;
	} else {
		arraylen = truncate;
	}

	for (i = 0; i < checklen; i++) {
		found = arraybase_getunitbyindex(&array, i, (VP)&val2);
		if (i < arraylen) {
			if (found != True) {
				printf("arraybase_getunitbyindex not found failure\n");
				result = UNITTEST_RESULT_FAIL;
			}
			if ((val2->a != 100)||(val2->b != i + 1234)||(val2->c != i % 23)) {
				printf("arraybase_getunitbyindex unexpected value failure: %d\n", i);
				result = UNITTEST_RESULT_FAIL;
			}
		} else {
			if (found != False) {
				printf("arraybase_getunitbyindex found failure\n");
				result = UNITTEST_RESULT_FAIL;
			}
		}
	}

	arraybase_finalize(&array);

	return result;
}

LOCAL UNITTEST_RESULT test_array_13()
{
	return test_array_truncate_base(100, 100, 0, 200);
}

LOCAL UNITTEST_RESULT test_array_14()
{
	return test_array_truncate_base(100, 100, 50, 200);
}

LOCAL UNITTEST_RESULT test_array_15()
{
	return test_array_truncate_base(100, 100, 100, 200);
}

LOCAL UNITTEST_RESULT test_array_16()
{
	return test_array_truncate_base(100, 100, 200, 200);
}

LOCAL UNITTEST_RESULT test_array_17()
{
	return test_array_truncate_base(50, 100, 00, 200);
}

LOCAL UNITTEST_RESULT test_array_18()
{
	return test_array_truncate_base(50, 100, 50, 200);
}

LOCAL UNITTEST_RESULT test_array_19()
{
	return test_array_truncate_base(50, 100, 100, 200);
}

LOCAL UNITTEST_RESULT test_array_20()
{
	return test_array_truncate_base(50, 100, 200, 200);
}

LOCAL UNITTEST_RESULT test_array_21()
{
	return test_array_truncate_base(100, 500, 0, 1000);
}

LOCAL UNITTEST_RESULT test_array_22()
{
	return test_array_truncate_base(100, 500, 50, 1000);
}

LOCAL UNITTEST_RESULT test_array_23()
{
	return test_array_truncate_base(100, 500, 250, 1000);
}

LOCAL UNITTEST_RESULT test_array_24()
{
	return test_array_truncate_base(100, 500, 500, 1000);
}

LOCAL UNITTEST_RESULT test_array_25()
{
	return test_array_truncate_base(100, 500, 1000, 1000);
}

LOCAL UNITTEST_RESULT test_array_truncate_2_base(W denom, W testnum1, W truncate, W testnum2, W checklen)
{
	arraybase_t array;
	W i, err, arraylen;
	test_array_dummydata val, *val2;
	UNITTEST_RESULT result = UNITTEST_RESULT_PASS;
	Bool found;

	err = arraybase_initialize(&array, sizeof(test_array_dummydata), denom);
	if (err < 0) {
		return UNITTEST_RESULT_FAIL;
	}

	for (i = 0; i < testnum1; i++) {
		val.a = 100;
		val.b = i + 1234;
		val.c = i % 23 ;
		err = arraybase_appendunit(&array, (VP)&val);
		if (err < 0) {
			printf("arraybase_appendunit failure\n");
			result = UNITTEST_RESULT_FAIL;
		}
	}

	arraybase_truncate(&array, truncate);

	if (truncate > testnum1) {
		arraylen = testnum1;
	} else {
		arraylen = truncate;
	}

	for (i = arraylen; i < arraylen + testnum2; i++) {
		val.a = 100;
		val.b = i + 1234;
		val.c = i % 23 ;
		err = arraybase_appendunit(&array, (VP)&val);
		if (err < 0) {
			printf("arraybase_appendunit failure\n");
			result = UNITTEST_RESULT_FAIL;
		}
	}

	arraylen += testnum2;

	for (i = 0; i < checklen; i++) {
		found = arraybase_getunitbyindex(&array, i, (VP)&val2);
		if (i < arraylen) {
			if (found != True) {
				printf("arraybase_getunitbyindex not found failure\n");
				result = UNITTEST_RESULT_FAIL;
			}
			if ((val2->a != 100)||(val2->b != i + 1234)||(val2->c != i % 23)) {
				printf("arraybase_getunitbyindex unexpected value failure: %d\n", i);
				result = UNITTEST_RESULT_FAIL;
			}
		} else {
			if (found != False) {
				printf("arraybase_getunitbyindex found failure\n");
				result = UNITTEST_RESULT_FAIL;
			}
		}
	}

	arraybase_finalize(&array);

	return result;
}

LOCAL UNITTEST_RESULT test_array_26()
{
	return test_array_truncate_2_base(100, 100, 0, 50, 200);
}

LOCAL UNITTEST_RESULT test_array_27()
{
	return test_array_truncate_2_base(100, 100, 50, 50, 200);
}

LOCAL UNITTEST_RESULT test_array_28()
{
	return test_array_truncate_2_base(100, 100, 100, 50, 200);
}

LOCAL UNITTEST_RESULT test_array_29()
{
	return test_array_truncate_2_base(100, 100, 200, 50, 200);
}

LOCAL UNITTEST_RESULT test_array_30()
{
	return test_array_truncate_2_base(50, 100, 00, 100, 200);
}

LOCAL UNITTEST_RESULT test_array_31()
{
	return test_array_truncate_2_base(50, 100, 50, 100, 200);
}

LOCAL UNITTEST_RESULT test_array_32()
{
	return test_array_truncate_2_base(50, 100, 100, 100, 200);
}

LOCAL UNITTEST_RESULT test_array_33()
{
	return test_array_truncate_2_base(50, 100, 200, 100, 200);
}

LOCAL UNITTEST_RESULT test_array_34()
{
	return test_array_truncate_2_base(100, 500, 0, 100, 1000);
}

LOCAL UNITTEST_RESULT test_array_35()
{
	return test_array_truncate_2_base(100, 500, 50, 100, 1000);
}

LOCAL UNITTEST_RESULT test_array_36()
{
	return test_array_truncate_2_base(100, 500, 250, 100, 1000);
}

LOCAL UNITTEST_RESULT test_array_37()
{
	return test_array_truncate_2_base(100, 500, 500, 100, 1000);
}

LOCAL UNITTEST_RESULT test_array_38()
{
	return test_array_truncate_2_base(100, 500, 1000, 100, 1000);
}

EXPORT VOID test_arraybase_main(unittest_driver_t *driver)
{
	UNITTEST_DRIVER_REGIST(driver, test_array_1);
	UNITTEST_DRIVER_REGIST(driver, test_array_2);
	UNITTEST_DRIVER_REGIST(driver, test_array_3);
	UNITTEST_DRIVER_REGIST(driver, test_array_4);
	UNITTEST_DRIVER_REGIST(driver, test_array_5);
	UNITTEST_DRIVER_REGIST(driver, test_array_6);
	UNITTEST_DRIVER_REGIST(driver, test_array_7);
	UNITTEST_DRIVER_REGIST(driver, test_array_8);
	UNITTEST_DRIVER_REGIST(driver, test_array_9);
	UNITTEST_DRIVER_REGIST(driver, test_array_10);
	UNITTEST_DRIVER_REGIST(driver, test_array_11);
	UNITTEST_DRIVER_REGIST(driver, test_array_12);
	UNITTEST_DRIVER_REGIST(driver, test_array_13);
	UNITTEST_DRIVER_REGIST(driver, test_array_14);
	UNITTEST_DRIVER_REGIST(driver, test_array_15);
	UNITTEST_DRIVER_REGIST(driver, test_array_16);
	UNITTEST_DRIVER_REGIST(driver, test_array_17);
	UNITTEST_DRIVER_REGIST(driver, test_array_18);
	UNITTEST_DRIVER_REGIST(driver, test_array_19);
	UNITTEST_DRIVER_REGIST(driver, test_array_20);
	UNITTEST_DRIVER_REGIST(driver, test_array_21);
	UNITTEST_DRIVER_REGIST(driver, test_array_22);
	UNITTEST_DRIVER_REGIST(driver, test_array_23);
	UNITTEST_DRIVER_REGIST(driver, test_array_24);
	UNITTEST_DRIVER_REGIST(driver, test_array_25);
	UNITTEST_DRIVER_REGIST(driver, test_array_26);
	UNITTEST_DRIVER_REGIST(driver, test_array_27);
	UNITTEST_DRIVER_REGIST(driver, test_array_28);
	UNITTEST_DRIVER_REGIST(driver, test_array_29);
	UNITTEST_DRIVER_REGIST(driver, test_array_30);
	UNITTEST_DRIVER_REGIST(driver, test_array_31);
	UNITTEST_DRIVER_REGIST(driver, test_array_32);
	UNITTEST_DRIVER_REGIST(driver, test_array_33);
	UNITTEST_DRIVER_REGIST(driver, test_array_34);
	UNITTEST_DRIVER_REGIST(driver, test_array_35);
	UNITTEST_DRIVER_REGIST(driver, test_array_36);
	UNITTEST_DRIVER_REGIST(driver, test_array_37);
	UNITTEST_DRIVER_REGIST(driver, test_array_38);
}

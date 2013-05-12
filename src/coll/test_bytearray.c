/*
 * test_bytearray.c
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

#include "test_coll.h"

#include "bytearray.h"

#include    <btron/btron.h>
#include    <bstdio.h>

LOCAL UNITTEST_RESULT test_bytearray_1()
{
	bytearray_t array;
	W i, len, err;
	UNITTEST_RESULT result = UNITTEST_RESULT_PASS;
	UB data[] = {0x1, 0x2, 0x3}, *p;
	W data_len = sizeof(data);

	err = bytearray_initialize(&array);
	if (err < 0) {
		printf("bytearray_initialize error: %08x\n", err);
		return UNITTEST_RESULT_FAIL;
	}

	for (i = 0; i < data_len; i++) {
		err = bytearray_pushback(&array, data[i]);
		if (err < 0) {
			printf("bytearray_pushback error: %08x", err);
			result = UNITTEST_RESULT_FAIL;
			break;
		}
	}

	p = bytearray_getbuffer(&array);
	len = bytearray_getlength(&array);

	if (len != data_len) {
		printf("buffer length fail: expected = %d, result = %d\n", data_len, len);
		result = UNITTEST_RESULT_FAIL;
	} else {
		if (memcmp(data, p, data_len) != 0) {
			printf("buffer result fail\n");
			result = UNITTEST_RESULT_FAIL;
		}
	}

	bytearray_finalize(&array);

	return result;
}

LOCAL UNITTEST_RESULT test_bytearray_2()
{
	bytearray_t array;
	W i, len, err;
	UNITTEST_RESULT result = UNITTEST_RESULT_PASS;
	UB data[] = {0x1, 0x2, 0x3}, *p;
	W data_len = sizeof(data);

	err = bytearray_initialize(&array);
	if (err < 0) {
		printf("bytearray_initialize error: %08x\n", err);
		return UNITTEST_RESULT_FAIL;
	}

	for (i = 0; i < data_len; i++) {
		err = bytearray_pushback(&array, data[i]);
		if (err < 0) {
			printf("bytearray_pushback error: %08x", err);
			result = UNITTEST_RESULT_FAIL;
			break;
		}
	}
	err = bytearray_popback(&array);
	if (err < 0) {
		printf("bytearray_pushback error: %08x", err);
		result = UNITTEST_RESULT_FAIL;
	}

	p = bytearray_getbuffer(&array);
	len = bytearray_getlength(&array);

	if (len != (data_len-1)) {
		printf("buffer length fail: expected = %d, result = %d\n", data_len - 1, len);
		result = UNITTEST_RESULT_FAIL;
	} else {
		if (memcmp(data, p, data_len - 1) != 0) {
			printf("buffer result fail\n");
			result = UNITTEST_RESULT_FAIL;
		}
	}

	bytearray_finalize(&array);

	return result;
}

typedef struct {
	UB *init;
	W init_len;
	UB *insert;
	W insert_len;
	W insert_at;
	UB *expected;
	W expected_len;
} test_bytearray_cursor_insert;

LOCAL UNITTEST_RESULT test_bytearray_cursor_insert_common(test_bytearray_cursor_insert *testdata)
{
	bytearray_t array;
	W i, len, err;
	bytearray_cursor_t cursor;
	UB *p;
	UNITTEST_RESULT result = UNITTEST_RESULT_PASS;

	err = bytearray_initialize(&array);
	if (err < 0) {
		printf("bytearray_initialize error: %08x\n", err);
		return UNITTEST_RESULT_FAIL;
	}

	for (i = 0; i < testdata->init_len; i++) {
		err = bytearray_pushback(&array, testdata->init[i]);
		if (err < 0) {
			printf("bytearray_pushback error: %08x", err);
			result = UNITTEST_RESULT_FAIL;
			break;
		}
	}

	bytearray_cursor_initialize(&cursor, &array);

	err = bytearray_cursor_move(&cursor, testdata->insert_at);
	if (err < 0) {
		printf("bytearray_cursor_move error: %08x", err);
		result = UNITTEST_RESULT_FAIL;
	}
	err = bytearray_cursor_insert(&cursor, testdata->insert, testdata->insert_len);
	if (err < 0) {
		printf("bytearray_cursor_insert error: %08x", err);
		result = UNITTEST_RESULT_FAIL;
	}

	bytearray_cursor_finalize(&cursor);

	p = bytearray_getbuffer(&array);
	len = bytearray_getlength(&array);
	if (len != testdata->expected_len) {
		printf("buffer length fail: expected = %d, result = %d\n", testdata->expected_len, len);
		result = UNITTEST_RESULT_FAIL;
	} else {
		if (memcmp(testdata->expected, p, testdata->expected_len) != 0) {
			printf("buffer result fail\n");
			result = UNITTEST_RESULT_FAIL;
			for (i = 0; i < testdata->expected_len; i++) {
				printf("%d: %x, %x\n", i, testdata->expected[i], p[i]);
			}
		}
	}

	bytearray_finalize(&array);

	return result;
}

LOCAL UNITTEST_RESULT test_bytearray_cursor_insert_1()
{
	UB init[] = {0x1, 0x2, 0x3};
	W init_len = sizeof(init);
	UB insert[] = {0xA, 0xB};
	W insert_len = sizeof(insert);
	W insert_at = 0;
	UB expected[] = {0xA, 0xB, 0x1, 0x2, 0x3};
	W expected_len = sizeof(expected);
	test_bytearray_cursor_insert testdata = {
		init, init_len,
		insert, insert_len, insert_at,
		expected, expected_len
	};
	return test_bytearray_cursor_insert_common(&testdata);
}

LOCAL UNITTEST_RESULT test_bytearray_cursor_insert_2()
{
	UB init[] = {0x1, 0x2, 0x3};
	W init_len = sizeof(init);
	UB insert[] = {0xA, 0xB};
	W insert_len = sizeof(insert);
	W insert_at = 1;
	UB expected[] = {0x1, 0xA, 0xB, 0x2, 0x3};
	W expected_len = sizeof(expected);
	test_bytearray_cursor_insert testdata = {
		init, init_len,
		insert, insert_len, insert_at,
		expected, expected_len
	};
	return test_bytearray_cursor_insert_common(&testdata);
}

LOCAL UNITTEST_RESULT test_bytearray_cursor_insert_3()
{
	UB init[] = {0x1, 0x2, 0x3};
	W init_len = sizeof(init);
	UB insert[] = {0xA, 0xB};
	W insert_len = sizeof(insert);
	W insert_at = 2;
	UB expected[] = {0x1, 0x2, 0xA, 0xB, 0x3};
	W expected_len = sizeof(expected);
	test_bytearray_cursor_insert testdata = {
		init, init_len,
		insert, insert_len, insert_at,
		expected, expected_len
	};
	return test_bytearray_cursor_insert_common(&testdata);
}

typedef struct {
	UB *init;
	W init_len;
	W erase_at;
	W erase_len;
	UB *expected;
	W expected_len;
} test_bytearray_cursor_erase;

LOCAL UNITTEST_RESULT test_bytearray_cursor_erase_common(test_bytearray_cursor_erase *testdata)
{
	bytearray_t array;
	W i, len, err;
	bytearray_cursor_t cursor;
	UB *p;
	UNITTEST_RESULT result = UNITTEST_RESULT_PASS;

	err = bytearray_initialize(&array);
	if (err < 0) {
		printf("bytearray_initialize error: %08x\n", err);
		return UNITTEST_RESULT_FAIL;
	}

	for (i = 0; i < testdata->init_len; i++) {
		err = bytearray_pushback(&array, testdata->init[i]);
		if (err < 0) {
			printf("bytearray_pushback error: %08x", err);
			result = UNITTEST_RESULT_FAIL;
			break;
		}
	}

	bytearray_cursor_initialize(&cursor, &array);

	err = bytearray_cursor_move(&cursor, testdata->erase_at);
	if (err < 0) {
		printf("bytearray_cursor_move error: %08x", err);
		result = UNITTEST_RESULT_FAIL;
	}
	err = bytearray_cursor_erase(&cursor, testdata->erase_len);
	if (err < 0) {
		printf("bytearray_cursor_erase error: %08x", err);
		result = UNITTEST_RESULT_FAIL;
	}

	bytearray_cursor_finalize(&cursor);

	p = bytearray_getbuffer(&array);
	len = bytearray_getlength(&array);
	if (len != testdata->expected_len) {
		printf("buffer length fail: expected = %d, result = %d\n", testdata->expected_len, len);
		result = UNITTEST_RESULT_FAIL;
	} else {
		if (memcmp(testdata->expected, p, testdata->expected_len) != 0) {
			printf("buffer result fail\n");
			result = UNITTEST_RESULT_FAIL;
			for (i = 0; i < testdata->expected_len; i++) {
				printf("%d: %x, %x\n", i, testdata->expected[i], p[i]);
			}
		}
	}

	bytearray_finalize(&array);

	return result;
}

LOCAL UNITTEST_RESULT test_bytearray_cursor_erase_1()
{
	UB init[] = {0x1, 0x2, 0x3};
	W init_len = sizeof(init);
	W erase_at = 0;
	W erase_len = 0;
	UB expected[] = {0x1, 0x2, 0x3};
	W expected_len = sizeof(expected);
	test_bytearray_cursor_erase testdata = {
		init, init_len,
		erase_at, erase_len,
		expected, expected_len
	};
	return test_bytearray_cursor_erase_common(&testdata);
}

LOCAL UNITTEST_RESULT test_bytearray_cursor_erase_2()
{
	UB init[] = {0x1, 0x2, 0x3};
	W init_len = sizeof(init);
	W erase_at = 0;
	W erase_len = 1;
	UB expected[] = {0x2, 0x3};
	W expected_len = sizeof(expected);
	test_bytearray_cursor_erase testdata = {
		init, init_len,
		erase_at, erase_len,
		expected, expected_len
	};
	return test_bytearray_cursor_erase_common(&testdata);
}

LOCAL UNITTEST_RESULT test_bytearray_cursor_erase_3()
{
	UB init[] = {0x1, 0x2, 0x3};
	W init_len = sizeof(init);
	W erase_at = 0;
	W erase_len = 2;
	UB expected[] = {0x3};
	W expected_len = sizeof(expected);
	test_bytearray_cursor_erase testdata = {
		init, init_len,
		erase_at, erase_len,
		expected, expected_len
	};
	return test_bytearray_cursor_erase_common(&testdata);
}

LOCAL UNITTEST_RESULT test_bytearray_cursor_erase_4()
{
	UB init[] = {0x1, 0x2, 0x3};
	W init_len = sizeof(init);
	W erase_at = 0;
	W erase_len = 3;
	UB expected[] = {};
	W expected_len = sizeof(expected);
	test_bytearray_cursor_erase testdata = {
		init, init_len,
		erase_at, erase_len,
		expected, expected_len
	};
	return test_bytearray_cursor_erase_common(&testdata);
}

LOCAL UNITTEST_RESULT test_bytearray_cursor_erase_5()
{
	UB init[] = {0x1, 0x2, 0x3};
	W init_len = sizeof(init);
	W erase_at = 1;
	W erase_len = 0;
	UB expected[] = {0x1, 0x2, 0x3};
	W expected_len = sizeof(expected);
	test_bytearray_cursor_erase testdata = {
		init, init_len,
		erase_at, erase_len,
		expected, expected_len
	};
	return test_bytearray_cursor_erase_common(&testdata);
}

LOCAL UNITTEST_RESULT test_bytearray_cursor_erase_6()
{
	UB init[] = {0x1, 0x2, 0x3};
	W init_len = sizeof(init);
	W erase_at = 1;
	W erase_len = 1;
	UB expected[] = {0x1, 0x3};
	W expected_len = sizeof(expected);
	test_bytearray_cursor_erase testdata = {
		init, init_len,
		erase_at, erase_len,
		expected, expected_len
	};
	return test_bytearray_cursor_erase_common(&testdata);
}

LOCAL UNITTEST_RESULT test_bytearray_cursor_erase_7()
{
	UB init[] = {0x1, 0x2, 0x3};
	W init_len = sizeof(init);
	W erase_at = 1;
	W erase_len = 2;
	UB expected[] = {0x1};
	W expected_len = sizeof(expected);
	test_bytearray_cursor_erase testdata = {
		init, init_len,
		erase_at, erase_len,
		expected, expected_len
	};
	return test_bytearray_cursor_erase_common(&testdata);
}

LOCAL UNITTEST_RESULT test_bytearray_cursor_erase_8()
{
	UB init[] = {0x1, 0x2, 0x3};
	W init_len = sizeof(init);
	W erase_at = 2;
	W erase_len = 0;
	UB expected[] = {0x1, 0x2, 0x3};
	W expected_len = sizeof(expected);
	test_bytearray_cursor_erase testdata = {
		init, init_len,
		erase_at, erase_len,
		expected, expected_len
	};
	return test_bytearray_cursor_erase_common(&testdata);
}

LOCAL UNITTEST_RESULT test_bytearray_cursor_erase_9()
{
	UB init[] = {0x1, 0x2, 0x3};
	W init_len = sizeof(init);
	W erase_at = 2;
	W erase_len = 1;
	UB expected[] = {0x1, 0x2};
	W expected_len = sizeof(expected);
	test_bytearray_cursor_erase testdata = {
		init, init_len,
		erase_at, erase_len,
		expected, expected_len
	};
	return test_bytearray_cursor_erase_common(&testdata);
}

typedef struct {
	UB *init;
	W init_len;
	UB *expected;
	W expected_len;
} test_bytearray_cursor_getUB;

LOCAL UNITTEST_RESULT test_bytearray_cursor_getUB_common(test_bytearray_cursor_getUB *testdata)
{
	bytearray_t array;
	W i, err;
	bytearray_cursor_t cursor;
	UB val;
	UNITTEST_RESULT result = UNITTEST_RESULT_PASS;

	err = bytearray_initialize(&array);
	if (err < 0) {
		printf("bytearray_initialize error: %08x\n", err);
		return UNITTEST_RESULT_FAIL;
	}

	for (i = 0; i < testdata->init_len; i++) {
		err = bytearray_pushback(&array, testdata->init[i]);
		if (err < 0) {
			printf("bytearray_pushback error: %08x", err);
			result = UNITTEST_RESULT_FAIL;
			break;
		}
	}

	bytearray_cursor_initialize(&cursor, &array);

	for (i = 0; i < testdata->expected_len; i++) {
		err = bytearray_cursor_getUB(&cursor, &val);
		if (err < 0) {
			printf("bytearray_cursor_getUB error: %08x", err);
			result = UNITTEST_RESULT_FAIL;
		}
		if (val != testdata->expected[i]) {
			printf("value failed: expected = %08x, result = %02x", val, testdata->expected[i]);
			result = UNITTEST_RESULT_FAIL;
		}

		err = bytearray_cursor_move(&cursor, 1);
		if (err < 0) {
			printf("bytearray_cursor_move error: %08x", err);
			result = UNITTEST_RESULT_FAIL;
		}
	}

	bytearray_cursor_finalize(&cursor);

	bytearray_finalize(&array);

	return result;
}

LOCAL UNITTEST_RESULT test_bytearray_cursor_getUB_1()
{
	UB init[] = {0x1, 0x2, 0x3};
	W init_len = sizeof(init);
	UB expected[] = {0x1, 0x2, 0x3};
	W expected_len = sizeof(expected);
	test_bytearray_cursor_getUB testdata = {
		init, init_len,
		expected, expected_len
	};
	return test_bytearray_cursor_getUB_common(&testdata);
}

typedef struct {
	UB *init;
	W init_len;
	UH *expected;
	W expected_len;
} test_bytearray_cursor_getUH;

LOCAL UNITTEST_RESULT test_bytearray_cursor_getUH_common(test_bytearray_cursor_getUH *testdata)
{
	bytearray_t array;
	W i, err;
	bytearray_cursor_t cursor;
	UH val;
	UNITTEST_RESULT result = UNITTEST_RESULT_PASS;

	err = bytearray_initialize(&array);
	if (err < 0) {
		printf("bytearray_initialize error: %08x\n", err);
		return UNITTEST_RESULT_FAIL;
	}

	for (i = 0; i < testdata->init_len; i++) {
		err = bytearray_pushback(&array, testdata->init[i]);
		if (err < 0) {
			printf("bytearray_pushback error: %08x", err);
			result = UNITTEST_RESULT_FAIL;
			break;
		}
	}

	bytearray_cursor_initialize(&cursor, &array);

	for (i = 0; i < testdata->expected_len; i++) {
		err = bytearray_cursor_getUH(&cursor, &val);
		if (err < 0) {
			printf("bytearray_cursor_getUH error: %08x", err);
			result = UNITTEST_RESULT_FAIL;
		}
		if (val != testdata->expected[i]) {
			printf("value failed: expected = %08x, result = %02x", val, testdata->expected[i]);
			result = UNITTEST_RESULT_FAIL;
		}

		err = bytearray_cursor_move(&cursor, 1);
		if (err < 0) {
			printf("bytearray_cursor_move error: %08x", err);
			result = UNITTEST_RESULT_FAIL;
		}
	}

	bytearray_cursor_finalize(&cursor);

	bytearray_finalize(&array);

	return result;
}

LOCAL UNITTEST_RESULT test_bytearray_cursor_getUH_1()
{
	UB init[] = {0x1, 0x2, 0x3, 0x4};
	W init_len = sizeof(init);
	UH expected[] = {0x0201, 0x0302, 0x0403};
	W expected_len = 3;
	test_bytearray_cursor_getUH testdata = {
		init, init_len,
		expected, expected_len
	};
	return test_bytearray_cursor_getUH_common(&testdata);
}

typedef struct {
	UB *init;
	W init_len;
	UW *expected;
	W expected_len;
} test_bytearray_cursor_getUW;

LOCAL UNITTEST_RESULT test_bytearray_cursor_getUW_common(test_bytearray_cursor_getUW *testdata)
{
	bytearray_t array;
	W i, err;
	bytearray_cursor_t cursor;
	UW val;
	UNITTEST_RESULT result = UNITTEST_RESULT_PASS;

	err = bytearray_initialize(&array);
	if (err < 0) {
		printf("bytearray_initialize error: %08x\n", err);
		return UNITTEST_RESULT_FAIL;
	}

	for (i = 0; i < testdata->init_len; i++) {
		err = bytearray_pushback(&array, testdata->init[i]);
		if (err < 0) {
			printf("bytearray_pushback error: %08x", err);
			result = UNITTEST_RESULT_FAIL;
			break;
		}
	}

	bytearray_cursor_initialize(&cursor, &array);

	for (i = 0; i < testdata->expected_len; i++) {
		err = bytearray_cursor_getUW(&cursor, &val);
		if (err < 0) {
			printf("bytearray_cursor_getUW error: %08x", err);
			result = UNITTEST_RESULT_FAIL;
		}
		if (val != testdata->expected[i]) {
			printf("value failed: expected = %08x, result = %02x", val, testdata->expected[i]);
			result = UNITTEST_RESULT_FAIL;
		}

		err = bytearray_cursor_move(&cursor, 1);
		if (err < 0) {
			printf("bytearray_cursor_move error: %08x", err);
			result = UNITTEST_RESULT_FAIL;
		}
	}

	bytearray_cursor_finalize(&cursor);

	bytearray_finalize(&array);

	return result;
}

LOCAL UNITTEST_RESULT test_bytearray_cursor_getUW_1()
{
	UB init[] = {0x1, 0x2, 0x3, 0x4, 0x5};
	W init_len = sizeof(init);
	UW expected[] = {0x04030201, 0x05040302};
	W expected_len = 2;
	test_bytearray_cursor_getUW testdata = {
		init, init_len,
		expected, expected_len
	};
	return test_bytearray_cursor_getUW_common(&testdata);
}

EXPORT VOID test_bytearray_main(unittest_driver_t *driver)
{
	UNITTEST_DRIVER_REGIST(driver, test_bytearray_1);
	UNITTEST_DRIVER_REGIST(driver, test_bytearray_2);
	UNITTEST_DRIVER_REGIST(driver, test_bytearray_cursor_insert_1);
	UNITTEST_DRIVER_REGIST(driver, test_bytearray_cursor_insert_2);
	UNITTEST_DRIVER_REGIST(driver, test_bytearray_cursor_insert_3);
	UNITTEST_DRIVER_REGIST(driver, test_bytearray_cursor_erase_1);
	UNITTEST_DRIVER_REGIST(driver, test_bytearray_cursor_erase_2);
	UNITTEST_DRIVER_REGIST(driver, test_bytearray_cursor_erase_3);
	UNITTEST_DRIVER_REGIST(driver, test_bytearray_cursor_erase_4);
	UNITTEST_DRIVER_REGIST(driver, test_bytearray_cursor_erase_5);
	UNITTEST_DRIVER_REGIST(driver, test_bytearray_cursor_erase_6);
	UNITTEST_DRIVER_REGIST(driver, test_bytearray_cursor_erase_7);
	UNITTEST_DRIVER_REGIST(driver, test_bytearray_cursor_erase_8);
	UNITTEST_DRIVER_REGIST(driver, test_bytearray_cursor_erase_9);
	UNITTEST_DRIVER_REGIST(driver, test_bytearray_cursor_getUB_1);
	UNITTEST_DRIVER_REGIST(driver, test_bytearray_cursor_getUH_1);
	UNITTEST_DRIVER_REGIST(driver, test_bytearray_cursor_getUW_1);
}

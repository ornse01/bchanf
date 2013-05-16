/*
 * test_wordarray.c
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

#include "wordarray.h"

#include    <btron/btron.h>
#include    <bstdio.h>

LOCAL UNITTEST_RESULT test_wordarray_1()
{
	wordarray_t array;
	W i, len, err;
	UNITTEST_RESULT result = UNITTEST_RESULT_PASS;
	UW data[] = {0x1, 0x2, 0x3}, *p;
	W data_len = sizeof(data);

	err = wordarray_initialize(&array);
	if (err < 0) {
		printf("wordarray_initialize error: %08x\n", err);
		return UNITTEST_RESULT_FAIL;
	}

	for (i = 0; i < data_len; i++) {
		err = wordarray_pushback(&array, data[i]);
		if (err < 0) {
			printf("wordarray_pushback error: %08x", err);
			result = UNITTEST_RESULT_FAIL;
			break;
		}
	}

	p = wordarray_getbuffer(&array);
	len = wordarray_getlength(&array);

	if (len != data_len) {
		printf("buffer length fail: expected = %d, result = %d\n", data_len, len);
		result = UNITTEST_RESULT_FAIL;
	} else {
		if (memcmp(data, p, data_len) != 0) {
			printf("buffer result fail\n");
			result = UNITTEST_RESULT_FAIL;
		}
	}

	wordarray_finalize(&array);

	return result;
}

LOCAL UNITTEST_RESULT test_wordarray_2()
{
	wordarray_t array;
	W i, len, err;
	UNITTEST_RESULT result = UNITTEST_RESULT_PASS;
	UW data[] = {0x1, 0x2, 0x3}, *p;
	W data_len = sizeof(data);

	err = wordarray_initialize(&array);
	if (err < 0) {
		printf("wordarray_initialize error: %08x\n", err);
		return UNITTEST_RESULT_FAIL;
	}

	for (i = 0; i < data_len; i++) {
		err = wordarray_pushback(&array, data[i]);
		if (err < 0) {
			printf("wordarray_pushback error: %08x", err);
			result = UNITTEST_RESULT_FAIL;
			break;
		}
	}
	err = wordarray_popback(&array);
	if (err < 0) {
		printf("wordarray_pushback error: %08x", err);
		result = UNITTEST_RESULT_FAIL;
	}

	p = wordarray_getbuffer(&array);
	len = wordarray_getlength(&array);

	if (len != (data_len-1)) {
		printf("buffer length fail: expected = %d, result = %d\n", data_len - 1, len);
		result = UNITTEST_RESULT_FAIL;
	} else {
		if (memcmp(data, p, data_len - 1) != 0) {
			printf("buffer result fail\n");
			result = UNITTEST_RESULT_FAIL;
		}
	}

	wordarray_finalize(&array);

	return result;
}

LOCAL UNITTEST_RESULT test_wordarray_3()
{
	wordarray_t array;
	W i, err;
	UNITTEST_RESULT result = UNITTEST_RESULT_PASS;
	UW data[] = {0x1, 0x2, 0x3}, data2[] = {0x5, 0x6, 0x7}, val;
	W data_len = sizeof(data);

	err = wordarray_initialize(&array);
	if (err < 0) {
		printf("wordarray_initialize error: %08x\n", err);
		return UNITTEST_RESULT_FAIL;
	}

	for (i = 0; i < data_len; i++) {
		err = wordarray_pushback(&array, data[i]);
		if (err < 0) {
			printf("wordarray_pushback error: %08x", err);
			result = UNITTEST_RESULT_FAIL;
			break;
		}
	}

	for (i = 0; i < data_len; i++) {
		err = wordarray_getat(&array, i, &val);
		if (err < 0) {
			printf("wordarray_getat return error: %08x", err);
			result = UNITTEST_RESULT_FAIL;
		}
		if (val != data[i]) {
			printf("wordarray_getat value error: result = %d, expected = %d\n", val, data[i]);
			result = UNITTEST_RESULT_FAIL;
		}
	}

	for (i = 0; i < data_len; i++) {
		err = wordarray_setat(&array, i, data2[i]);
		if (err < 0) {
			printf("wordarray_getat return error: %08x", err);
			result = UNITTEST_RESULT_FAIL;
		}
	}

	for (i = 0; i < data_len; i++) {
		err = wordarray_getat(&array, i, &val);
		if (err < 0) {
			printf("wordarray_getat return error: %08x", err);
			result = UNITTEST_RESULT_FAIL;
		}
		if (val != data2[i]) {
			printf("wordarray_getat value error: result = %d, expected = %d\n", val, data2[i]);
			result = UNITTEST_RESULT_FAIL;
		}
	}

	wordarray_finalize(&array);

	return result;
}

typedef struct {
	UW *init;
	W init_len;
	UW *insert;
	W insert_len;
	W insert_at;
	UW *expected;
	W expected_len;
} test_wordarray_cursor_insert;

LOCAL UNITTEST_RESULT test_wordarray_cursor_insert_common(test_wordarray_cursor_insert *testdata)
{
	wordarray_t array;
	W i, len, err;
	wordarray_cursor_t cursor;
	UW *p;
	UNITTEST_RESULT result = UNITTEST_RESULT_PASS;

	err = wordarray_initialize(&array);
	if (err < 0) {
		printf("wordarray_initialize error: %08x\n", err);
		return UNITTEST_RESULT_FAIL;
	}

	for (i = 0; i < testdata->init_len; i++) {
		err = wordarray_pushback(&array, testdata->init[i]);
		if (err < 0) {
			printf("wordarray_pushback error: %08x", err);
			result = UNITTEST_RESULT_FAIL;
			break;
		}
	}

	wordarray_cursor_initialize(&cursor, &array);

	err = wordarray_cursor_move(&cursor, testdata->insert_at);
	if (err < 0) {
		printf("wordarray_cursor_move error: %08x", err);
		result = UNITTEST_RESULT_FAIL;
	}
	err = wordarray_cursor_insert(&cursor, testdata->insert, testdata->insert_len);
	if (err < 0) {
		printf("wordarray_cursor_insert error: %08x", err);
		result = UNITTEST_RESULT_FAIL;
	}

	wordarray_cursor_finalize(&cursor);

	p = wordarray_getbuffer(&array);
	len = wordarray_getlength(&array);
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

	wordarray_finalize(&array);

	return result;
}

LOCAL UNITTEST_RESULT test_wordarray_cursor_insert_1()
{
	UW init[] = {0x1, 0x2, 0x3};
	W init_len = sizeof(init)/sizeof(UW);
	UW insert[] = {0xA, 0xB};
	W insert_len = sizeof(insert)/sizeof(UW);
	W insert_at = 0;
	UW expected[] = {0xA, 0xB, 0x1, 0x2, 0x3};
	W expected_len = sizeof(expected)/sizeof(UW);
	test_wordarray_cursor_insert testdata = {
		init, init_len,
		insert, insert_len, insert_at,
		expected, expected_len
	};
	return test_wordarray_cursor_insert_common(&testdata);
}

LOCAL UNITTEST_RESULT test_wordarray_cursor_insert_2()
{
	UW init[] = {0x1, 0x2, 0x3};
	W init_len = sizeof(init)/sizeof(UW);
	UW insert[] = {0xA, 0xB};
	W insert_len = sizeof(insert)/sizeof(UW);
	W insert_at = 1;
	UW expected[] = {0x1, 0xA, 0xB, 0x2, 0x3};
	W expected_len = sizeof(expected)/sizeof(UW);
	test_wordarray_cursor_insert testdata = {
		init, init_len,
		insert, insert_len, insert_at,
		expected, expected_len
	};
	return test_wordarray_cursor_insert_common(&testdata);
}

LOCAL UNITTEST_RESULT test_wordarray_cursor_insert_3()
{
	UW init[] = {0x1, 0x2, 0x3};
	W init_len = sizeof(init)/sizeof(UW);
	UW insert[] = {0xA, 0xB};
	W insert_len = sizeof(insert)/sizeof(UW);
	W insert_at = 2;
	UW expected[] = {0x1, 0x2, 0xA, 0xB, 0x3};
	W expected_len = sizeof(expected)/sizeof(UW);
	test_wordarray_cursor_insert testdata = {
		init, init_len,
		insert, insert_len, insert_at,
		expected, expected_len
	};
	return test_wordarray_cursor_insert_common(&testdata);
}

typedef struct {
	UW *init;
	W init_len;
	W erase_at;
	W erase_len;
	UW *expected;
	W expected_len;
} test_wordarray_cursor_erase;

LOCAL UNITTEST_RESULT test_wordarray_cursor_erase_common(test_wordarray_cursor_erase *testdata)
{
	wordarray_t array;
	W i, len, err;
	wordarray_cursor_t cursor;
	UW *p;
	UNITTEST_RESULT result = UNITTEST_RESULT_PASS;

	err = wordarray_initialize(&array);
	if (err < 0) {
		printf("wordarray_initialize error: %08x\n", err);
		return UNITTEST_RESULT_FAIL;
	}

	for (i = 0; i < testdata->init_len; i++) {
		err = wordarray_pushback(&array, testdata->init[i]);
		if (err < 0) {
			printf("wordarray_pushback error: %08x", err);
			result = UNITTEST_RESULT_FAIL;
			break;
		}
	}

	wordarray_cursor_initialize(&cursor, &array);

	err = wordarray_cursor_move(&cursor, testdata->erase_at);
	if (err < 0) {
		printf("wordarray_cursor_move error: %08x", err);
		result = UNITTEST_RESULT_FAIL;
	}
	err = wordarray_cursor_erase(&cursor, testdata->erase_len);
	if (err < 0) {
		printf("wordarray_cursor_erase error: %08x", err);
		result = UNITTEST_RESULT_FAIL;
	}

	wordarray_cursor_finalize(&cursor);

	p = wordarray_getbuffer(&array);
	len = wordarray_getlength(&array);
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

	wordarray_finalize(&array);

	return result;
}

LOCAL UNITTEST_RESULT test_wordarray_cursor_erase_1()
{
	UW init[] = {0x1, 0x2, 0x3};
	W init_len = sizeof(init)/sizeof(UW);
	W erase_at = 0;
	W erase_len = 0;
	UW expected[] = {0x1, 0x2, 0x3};
	W expected_len = sizeof(expected)/sizeof(UW);
	test_wordarray_cursor_erase testdata = {
		init, init_len,
		erase_at, erase_len,
		expected, expected_len
	};
	return test_wordarray_cursor_erase_common(&testdata);
}

LOCAL UNITTEST_RESULT test_wordarray_cursor_erase_2()
{
	UW init[] = {0x1, 0x2, 0x3};
	W init_len = sizeof(init)/sizeof(UW);
	W erase_at = 0;
	W erase_len = 1;
	UW expected[] = {0x2, 0x3};
	W expected_len = sizeof(expected)/sizeof(UW);
	test_wordarray_cursor_erase testdata = {
		init, init_len,
		erase_at, erase_len,
		expected, expected_len
	};
	return test_wordarray_cursor_erase_common(&testdata);
}

LOCAL UNITTEST_RESULT test_wordarray_cursor_erase_3()
{
	UW init[] = {0x1, 0x2, 0x3};
	W init_len = sizeof(init)/sizeof(UW);
	W erase_at = 0;
	W erase_len = 2;
	UW expected[] = {0x3};
	W expected_len = sizeof(expected)/sizeof(UW);
	test_wordarray_cursor_erase testdata = {
		init, init_len,
		erase_at, erase_len,
		expected, expected_len
	};
	return test_wordarray_cursor_erase_common(&testdata);
}

LOCAL UNITTEST_RESULT test_wordarray_cursor_erase_4()
{
	UW init[] = {0x1, 0x2, 0x3};
	W init_len = sizeof(init)/sizeof(UW);
	W erase_at = 0;
	W erase_len = 3;
	UW expected[] = {};
	W expected_len = sizeof(expected)/sizeof(UW);
	test_wordarray_cursor_erase testdata = {
		init, init_len,
		erase_at, erase_len,
		expected, expected_len
	};
	return test_wordarray_cursor_erase_common(&testdata);
}

LOCAL UNITTEST_RESULT test_wordarray_cursor_erase_5()
{
	UW init[] = {0x1, 0x2, 0x3};
	W init_len = sizeof(init)/sizeof(UW);
	W erase_at = 1;
	W erase_len = 0;
	UW expected[] = {0x1, 0x2, 0x3};
	W expected_len = sizeof(expected)/sizeof(UW);
	test_wordarray_cursor_erase testdata = {
		init, init_len,
		erase_at, erase_len,
		expected, expected_len
	};
	return test_wordarray_cursor_erase_common(&testdata);
}

LOCAL UNITTEST_RESULT test_wordarray_cursor_erase_6()
{
	UW init[] = {0x1, 0x2, 0x3};
	W init_len = sizeof(init)/sizeof(UW);
	W erase_at = 1;
	W erase_len = 1;
	UW expected[] = {0x1, 0x3};
	W expected_len = sizeof(expected)/sizeof(UW);
	test_wordarray_cursor_erase testdata = {
		init, init_len,
		erase_at, erase_len,
		expected, expected_len
	};
	return test_wordarray_cursor_erase_common(&testdata);
}

LOCAL UNITTEST_RESULT test_wordarray_cursor_erase_7()
{
	UW init[] = {0x1, 0x2, 0x3};
	W init_len = sizeof(init)/sizeof(UW);
	W erase_at = 1;
	W erase_len = 2;
	UW expected[] = {0x1};
	W expected_len = sizeof(expected)/sizeof(UW);
	test_wordarray_cursor_erase testdata = {
		init, init_len,
		erase_at, erase_len,
		expected, expected_len
	};
	return test_wordarray_cursor_erase_common(&testdata);
}

LOCAL UNITTEST_RESULT test_wordarray_cursor_erase_8()
{
	UW init[] = {0x1, 0x2, 0x3};
	W init_len = sizeof(init)/sizeof(UW);
	W erase_at = 2;
	W erase_len = 0;
	UW expected[] = {0x1, 0x2, 0x3};
	W expected_len = sizeof(expected)/sizeof(UW);
	test_wordarray_cursor_erase testdata = {
		init, init_len,
		erase_at, erase_len,
		expected, expected_len
	};
	return test_wordarray_cursor_erase_common(&testdata);
}

LOCAL UNITTEST_RESULT test_wordarray_cursor_erase_9()
{
	UW init[] = {0x1, 0x2, 0x3};
	W init_len = sizeof(init)/sizeof(UW);
	W erase_at = 2;
	W erase_len = 1;
	UW expected[] = {0x1, 0x2};
	W expected_len = sizeof(expected)/sizeof(UW);
	test_wordarray_cursor_erase testdata = {
		init, init_len,
		erase_at, erase_len,
		expected, expected_len
	};
	return test_wordarray_cursor_erase_common(&testdata);
}

typedef struct {
	UW *init;
	W init_len;
	UW *expected;
	W expected_len;
} test_wordarray_cursor_getUW;

LOCAL UNITTEST_RESULT test_wordarray_cursor_getUW_common(test_wordarray_cursor_getUW *testdata)
{
	wordarray_t array;
	W i, err;
	wordarray_cursor_t cursor;
	UW val;
	UNITTEST_RESULT result = UNITTEST_RESULT_PASS;

	err = wordarray_initialize(&array);
	if (err < 0) {
		printf("wordarray_initialize error: %08x\n", err);
		return UNITTEST_RESULT_FAIL;
	}

	for (i = 0; i < testdata->init_len; i++) {
		err = wordarray_pushback(&array, testdata->init[i]);
		if (err < 0) {
			printf("wordarray_pushback error: %08x", err);
			result = UNITTEST_RESULT_FAIL;
			break;
		}
	}

	wordarray_cursor_initialize(&cursor, &array);

	for (i = 0; i < testdata->expected_len; i++) {
		err = wordarray_cursor_getUW(&cursor, &val);
		if (err < 0) {
			printf("wordarray_cursor_getUW error: %08x", err);
			result = UNITTEST_RESULT_FAIL;
		}
		if (val != testdata->expected[i]) {
			printf("value failed: expected = %08x, result = %02x", val, testdata->expected[i]);
			result = UNITTEST_RESULT_FAIL;
		}

		err = wordarray_cursor_move(&cursor, 1);
		if (err < 0) {
			printf("wordarray_cursor_move error: %08x", err);
			result = UNITTEST_RESULT_FAIL;
		}
	}

	wordarray_cursor_finalize(&cursor);

	wordarray_finalize(&array);

	return result;
}

LOCAL UNITTEST_RESULT test_wordarray_cursor_getUW_1()
{
	UW init[] = {0x1, 0x2, 0x3, 0x4, 0x5};
	W init_len = sizeof(init)/sizeof(UW);
	UW expected[] = {0x1, 0x2, 0x3, 0x4, 0x5};
	W expected_len = sizeof(expected)/sizeof(UW);
	test_wordarray_cursor_getUW testdata = {
		init, init_len,
		expected, expected_len
	};
	return test_wordarray_cursor_getUW_common(&testdata);
}

typedef struct {
	UW *init;
	W init_len;
	UW val;
	W val_at;
	UW *expected;
	W expected_len;
} test_wordarray_cursor_setUW;

LOCAL UNITTEST_RESULT test_wordarray_cursor_setUW_common(test_wordarray_cursor_setUW *testdata)
{
	wordarray_t array;
	W i, len, err;
	wordarray_cursor_t cursor;
	UW *p;
	UNITTEST_RESULT result = UNITTEST_RESULT_PASS;

	err = wordarray_initialize(&array);
	if (err < 0) {
		printf("wordarray_initialize error: %08x\n", err);
		return UNITTEST_RESULT_FAIL;
	}

	for (i = 0; i < testdata->init_len; i++) {
		err = wordarray_pushback(&array, testdata->init[i]);
		if (err < 0) {
			printf("wordarray_pushback error: %08x", err);
			result = UNITTEST_RESULT_FAIL;
			break;
		}
	}

	wordarray_cursor_initialize(&cursor, &array);

	err = wordarray_cursor_move(&cursor, testdata->val_at);
	if (err < 0) {
		printf("wordarray_cursor_move error: %08x", err);
		result = UNITTEST_RESULT_FAIL;
	}
	err = wordarray_cursor_setUW(&cursor, testdata->val);
	if (err < 0) {
		printf("wordarray_cursor_setUW error: %08x", err);
		result = UNITTEST_RESULT_FAIL;
	}

	wordarray_cursor_finalize(&cursor);

	p = wordarray_getbuffer(&array);
	len = wordarray_getlength(&array);
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

	wordarray_finalize(&array);

	return result;
}

LOCAL UNITTEST_RESULT test_wordarray_cursor_setUW_1()
{
	UW init[] = {0x1, 0x2, 0x3};
	W init_len = sizeof(init);
	UW val = 0x4;
	W val_at = 0;
	UW expected[] = {0x4, 0x2, 0x3};
	W expected_len = sizeof(expected);
	test_wordarray_cursor_setUW testdata = {
		init, init_len,
		val, val_at,
		expected, expected_len
	};
	return test_wordarray_cursor_setUW_common(&testdata);
}

LOCAL UNITTEST_RESULT test_wordarray_cursor_setUW_2()
{
	UW init[] = {0x1, 0x2, 0x3};
	W init_len = sizeof(init);
	UW val = 0x4;
	W val_at = 1;
	UW expected[] = {0x1, 0x4, 0x3};
	W expected_len = sizeof(expected);
	test_wordarray_cursor_setUW testdata = {
		init, init_len,
		val, val_at,
		expected, expected_len
	};
	return test_wordarray_cursor_setUW_common(&testdata);
}

LOCAL UNITTEST_RESULT test_wordarray_cursor_setUW_3()
{
	UW init[] = {0x1, 0x2, 0x3};
	W init_len = sizeof(init);
	UW val = 0x4;
	W val_at = 2;
	UW expected[] = {0x1, 0x2, 0x4};
	W expected_len = sizeof(expected);
	test_wordarray_cursor_setUW testdata = {
		init, init_len,
		val, val_at,
		expected, expected_len
	};
	return test_wordarray_cursor_setUW_common(&testdata);
}

EXPORT VOID test_wordarray_main(unittest_driver_t *driver)
{
	UNITTEST_DRIVER_REGIST(driver, test_wordarray_1);
	UNITTEST_DRIVER_REGIST(driver, test_wordarray_2);
	UNITTEST_DRIVER_REGIST(driver, test_wordarray_3);
	UNITTEST_DRIVER_REGIST(driver, test_wordarray_cursor_insert_1);
	UNITTEST_DRIVER_REGIST(driver, test_wordarray_cursor_insert_2);
	UNITTEST_DRIVER_REGIST(driver, test_wordarray_cursor_insert_3);
	UNITTEST_DRIVER_REGIST(driver, test_wordarray_cursor_erase_1);
	UNITTEST_DRIVER_REGIST(driver, test_wordarray_cursor_erase_2);
	UNITTEST_DRIVER_REGIST(driver, test_wordarray_cursor_erase_3);
	UNITTEST_DRIVER_REGIST(driver, test_wordarray_cursor_erase_4);
	UNITTEST_DRIVER_REGIST(driver, test_wordarray_cursor_erase_5);
	UNITTEST_DRIVER_REGIST(driver, test_wordarray_cursor_erase_6);
	UNITTEST_DRIVER_REGIST(driver, test_wordarray_cursor_erase_7);
	UNITTEST_DRIVER_REGIST(driver, test_wordarray_cursor_erase_8);
	UNITTEST_DRIVER_REGIST(driver, test_wordarray_cursor_erase_9);
	UNITTEST_DRIVER_REGIST(driver, test_wordarray_cursor_getUW_1);
	UNITTEST_DRIVER_REGIST(driver, test_wordarray_cursor_setUW_1);
	UNITTEST_DRIVER_REGIST(driver, test_wordarray_cursor_setUW_2);
	UNITTEST_DRIVER_REGIST(driver, test_wordarray_cursor_setUW_3);
}

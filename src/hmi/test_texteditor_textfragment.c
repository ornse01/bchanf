/*
 * test_texteditor_textfragment.c
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

#include "test_hmi.h"

#include "texteditor_textfragment.h"

#include	<basic.h>
#include	<bstdio.h>
#include	<bstdlib.h>
#include	<bstring.h>
#include	<tcode.h>

#include    <tad/taddecoder.h>

#include    <unittest_driver.h>

LOCAL UNITTEST_RESULT test_texteditor_textfragment_1()
{
	texteditor_textfragment_t fragment;
	W err;

	err = texteditor_textfragment_initialize(&fragment);
	if (err < 0) {
		return UNITTEST_RESULT_FAIL;
	}
	texteditor_textfragment_finalize(&fragment);

	return UNITTEST_RESULT_PASS;
}

typedef struct {
	TC *data;
	W len;
} test_data_t;

typedef struct {
	test_data_t original;
	test_data_t *insert;
	W insert_len;
	W insert_pos;
	test_data_t expected;
	W expected_segment_num;
} test_texteditor_insertcontext_t;

LOCAL W test_texteditor_textfragment_inserttestdata(texteditor_insertcontext_t *context, test_data_t *testdata)
{
	taddecoder_t decoder;
	tadsegment segment;
	Bool cont;
	W err = 0;

	taddecoder_initialize(&decoder, testdata->data, testdata->len / sizeof(TC));

	for (;;) {
		cont = taddecoder_next(&decoder, &segment);
		if (cont == False) {
			break;
		}

		err = texteditor_insertcontext_insert(context, &segment);
		if (err < 0) {
			printf("texteditor_insertcontext_insert error\n");
			break;
		}
	}

	taddecoder_finalize(&decoder);

	return err;
}

LOCAL W test_texteditor_textfragment_initialvalue(texteditor_textfragment_t *fragment, test_data_t *original)
{
	texteditor_insertcontext_t context;
	W err;

	err = texteditor_insertcontext_initialize(&context, fragment, -1, 0);
	if (err < 0) {
		printf("texteditor_insertcontext_initialize error\n");
		return err;
	}

	err = test_texteditor_textfragment_inserttestdata(&context, original);
	if (err < 0) {
		texteditor_insertcontext_finalize(&context);
		return err;
	}

	return texteditor_insertcontext_finalize(&context);
}

LOCAL UNITTEST_RESULT test_texteditor_insertcontext_common(test_texteditor_insertcontext_t *testdata)
{
	texteditor_textfragment_t fragment;
	texteditor_insertcontext_t context;
	W i, len, err;
	UB *buf;
	UNITTEST_RESULT result = UNITTEST_RESULT_PASS;

	err = texteditor_textfragment_initialize(&fragment);
	if (err < 0) {
		return UNITTEST_RESULT_FAIL;
	}

	err = test_texteditor_textfragment_initialvalue(&fragment, &testdata->original);
	if (err < 0) {
		printf("texteditor value initialize error\n");
		result = UNITTEST_RESULT_FAIL;
	}

	err = texteditor_insertcontext_initialize(&context, &fragment, -1, testdata->insert_pos);
	if (err < 0) {
		printf("texteditor_insertcontext_initialize error\n");
		result = UNITTEST_RESULT_FAIL;
	} else {
		for (i = 0; i < testdata->insert_len; i++) {
			err = test_texteditor_textfragment_inserttestdata(&context, testdata->insert + i);
			if (err < 0) {
				printf("tadfragment_cursor_insert error\n");
				result = UNITTEST_RESULT_FAIL;
			}
		}

		err = texteditor_insertcontext_finalize(&context);

		len = texteditor_textfragment_getsegmentlength(&fragment);
		if (len != testdata->expected_segment_num) {
			printf("texteditor_textfragment_getsegmentlength fail: expected = %d, result = %d\n", testdata->expected_segment_num, len);
			result = UNITTEST_RESULT_FAIL;
		}

		len = texteditor_textfragment_getbufferlength(&fragment);
		if (len != testdata->expected.len) {
			printf("texteditor_textfragment_getbufferlength fail: expected = %d, result = %d\n", testdata->expected.len, len);
			result = UNITTEST_RESULT_FAIL;
		}

		buf = texteditor_textfragment_getbuffer(&fragment);
		if (memcmp(buf, testdata->expected.data, testdata->expected.len) != 0) {
			printf("texteditor_textfragment_getbuffer fail\n");
			{
				W i;
				for (i = 0; i < testdata->expected.len; i++) {
					printf("%02x, %02x\n", buf[i], ((UB*)testdata->expected.data)[i]);
				}
			}
			result = UNITTEST_RESULT_FAIL;
		}
	}

	texteditor_textfragment_finalize(&fragment);

	return result;
}

LOCAL UNITTEST_RESULT test_texteditor_insertcontext_1()
{
	test_data_t original = {
		(TC[]){TK_A, TK_B, TK_C},
		6
	};
	test_data_t *insert = NULL;
	W insert_len = 0;
	W insert_pos = 0;
	test_data_t expected = {
		(TC[]){TK_A, TK_B, TK_C},
		6
	};
	W expected_segment_num = 3;
	test_texteditor_insertcontext_t testdata = {
		original,
		insert, insert_len, insert_pos,
		expected, expected_segment_num
	};
	return test_texteditor_insertcontext_common(&testdata);
}

LOCAL UNITTEST_RESULT test_texteditor_insertcontext_2()
{
	test_data_t original = {
		(TC[]){TK_A, TK_B, TK_C},
		6
	};
	test_data_t insert[] = {
		{ (TC[]){TK_D, TK_E}, 4 }
	};
	W insert_len = 1;
	W insert_pos = 0;
	test_data_t expected = {
		(TC[]){TK_D, TK_E, TK_A, TK_B, TK_C},
		10
	};
	W expected_segment_num = 5;
	test_texteditor_insertcontext_t testdata = {
		original,
		insert, insert_len, insert_pos,
		expected, expected_segment_num
	};
	return test_texteditor_insertcontext_common(&testdata);
}

LOCAL UNITTEST_RESULT test_texteditor_insertcontext_3()
{
	test_data_t original = {
		(TC[]){TK_A, TK_B, TK_C},
		6
	};
	test_data_t insert[] = {
		{ (TC[]){TK_D, TK_E}, 4 }
	};
	W insert_len = 1;
	W insert_pos = 1;
	test_data_t expected = {
		(TC[]){TK_A, TK_D, TK_E, TK_B, TK_C},
		10
	};
	W expected_segment_num = 5;
	test_texteditor_insertcontext_t testdata = {
		original,
		insert, insert_len, insert_pos,
		expected, expected_segment_num
	};
	return test_texteditor_insertcontext_common(&testdata);
}

LOCAL UNITTEST_RESULT test_texteditor_insertcontext_4()
{
	test_data_t original = {
		(TC[]){TK_A, TK_B, TK_C},
		6
	};
	test_data_t insert[] = {
		{ (TC[]){TK_D, TK_E}, 4 }
	};
	W insert_len = 1;
	W insert_pos = 2;
	test_data_t expected = {
		(TC[]){TK_A, TK_B, TK_D, TK_E, TK_C},
		10
	};
	W expected_segment_num = 5;
	test_texteditor_insertcontext_t testdata = {
		original,
		insert, insert_len, insert_pos,
		expected, expected_segment_num
	};
	return test_texteditor_insertcontext_common(&testdata);
}

LOCAL UNITTEST_RESULT test_texteditor_insertcontext_5()
{
	test_data_t original = {
		(TC[]){TK_A, TK_B, TK_C},
		6
	};
	test_data_t insert[] = {
		{ (TC[]){TK_D, TK_E}, 4 }
	};
	W insert_len = 1;
	W insert_pos = 3;
	test_data_t expected = {
		(TC[]){TK_A, TK_B, TK_C, TK_D, TK_E},
		10
	};
	W expected_segment_num = 5;
	test_texteditor_insertcontext_t testdata = {
		original,
		insert, insert_len, insert_pos,
		expected, expected_segment_num
	};
	return test_texteditor_insertcontext_common(&testdata);
}

LOCAL UNITTEST_RESULT test_texteditor_insertcontext_6()
{
	test_data_t original = {
		(TC[]){TK_A, TK_B, TK_C},
		6
	};
	test_data_t insert[] = {
		{ (TC[]){0xFE22, TK_D, TK_E}, 6 }
	};
	W insert_len = 1;
	W insert_pos = 0;
	test_data_t expected = {
		(TC[]){0xFE22, TK_D, TK_E, 0xFE21, TK_A, TK_B, TK_C},
		14
	};
	W expected_segment_num = 7;
	test_texteditor_insertcontext_t testdata = {
		original,
		insert, insert_len, insert_pos,
		expected, expected_segment_num
	};
	return test_texteditor_insertcontext_common(&testdata);
}

LOCAL UNITTEST_RESULT test_texteditor_insertcontext_7()
{
	test_data_t original = {
		(TC[]){TK_A, TK_B, TK_C},
		6
	};
	test_data_t insert[] = {
		{ (TC[]){0xFE22, TK_D, TK_E}, 6 }
	};
	W insert_len = 1;
	W insert_pos = 1;
	test_data_t expected = {
		(TC[]){TK_A, 0xFE22, TK_D, TK_E, 0xFE21, TK_B, TK_C},
		14
	};
	W expected_segment_num = 7;
	test_texteditor_insertcontext_t testdata = {
		original,
		insert, insert_len, insert_pos,
		expected, expected_segment_num
	};
	return test_texteditor_insertcontext_common(&testdata);
}

LOCAL UNITTEST_RESULT test_texteditor_insertcontext_8()
{
	test_data_t original = {
		(TC[]){TK_A, TK_B, TK_C},
		6
	};
	test_data_t insert[] = {
		{ (TC[]){0xFE22, TK_D, TK_E}, 6 }
	};
	W insert_len = 1;
	W insert_pos = 2;
	test_data_t expected = {
		(TC[]){TK_A, TK_B, 0xFE22, TK_D, TK_E, 0xFE21, TK_C},
		14
	};
	W expected_segment_num = 7;
	test_texteditor_insertcontext_t testdata = {
		original,
		insert, insert_len, insert_pos,
		expected, expected_segment_num
	};
	return test_texteditor_insertcontext_common(&testdata);
}

LOCAL UNITTEST_RESULT test_texteditor_insertcontext_9()
{
	test_data_t original = {
		(TC[]){TK_A, TK_B, TK_C},
		6
	};
	test_data_t insert[] = {
		{ (TC[]){0xFE22, TK_D, TK_E}, 6 }
	};
	W insert_len = 1;
	W insert_pos = 3;
	test_data_t expected = {
		(TC[]){TK_A, TK_B, TK_C, 0xFE22, TK_D, TK_E, 0xFE21},
		14
	};
	W expected_segment_num = 7;
	test_texteditor_insertcontext_t testdata = {
		original,
		insert, insert_len, insert_pos,
		expected, expected_segment_num
	};
	return test_texteditor_insertcontext_common(&testdata);
}

LOCAL UNITTEST_RESULT test_texteditor_insertcontext_10()
{
	test_data_t original = {
		(TC[]){TK_A, TK_B, TK_C},
		6
	};
	test_data_t insert[] = {
		{ (TC[]){0xFFA2, 0x0006, 0x0300, 0x0101, 0x0102, TK_D, TK_E}, 14 }
	};
	W insert_len = 1;
	W insert_pos = 0;
	test_data_t expected = {
		(TC[]){0xFFA2, 0x0006, 0x0300, 0x0101, 0x0102,
			   TK_D, TK_E,
			   0xFFA2, 0x0006, 0x0300, 0x0101, 0x0101,
			   TK_A, TK_B, TK_C },
		30
	};
	W expected_segment_num = 7;
	test_texteditor_insertcontext_t testdata = {
		original,
		insert, insert_len, insert_pos,
		expected, expected_segment_num
	};
	return test_texteditor_insertcontext_common(&testdata);
}

LOCAL UNITTEST_RESULT test_texteditor_insertcontext_11()
{
	test_data_t original = {
		(TC[]){TK_A, TK_B, TK_C},
		6
	};
	test_data_t insert[] = {
		{ (TC[]){0xFFA2, 0x0006, 0x0300, 0x0101, 0x0102, TK_D, TK_E}, 14 }
	};
	W insert_len = 1;
	W insert_pos = 1;
	test_data_t expected = {
		(TC[]){TK_A,
			   0xFFA2, 0x0006, 0x0300, 0x0101, 0x0102,
			   TK_D, TK_E,
			   0xFFA2, 0x0006, 0x0300, 0x0101, 0x0101,
			   TK_B, TK_C },
		30
	};
	W expected_segment_num = 7;
	test_texteditor_insertcontext_t testdata = {
		original,
		insert, insert_len, insert_pos,
		expected, expected_segment_num
	};
	return test_texteditor_insertcontext_common(&testdata);
}

LOCAL UNITTEST_RESULT test_texteditor_insertcontext_12()
{
	test_data_t original = {
		(TC[]){TK_A, TK_B, TK_C},
		6
	};
	test_data_t insert[] = {
		{ (TC[]){0xFFA2, 0x0006, 0x0300, 0x0101, 0x0102, TK_D, TK_E}, 14 }
	};
	W insert_len = 1;
	W insert_pos = 2;
	test_data_t expected = {
		(TC[]){TK_A, TK_B,
			   0xFFA2, 0x0006, 0x0300, 0x0101, 0x0102,
			   TK_D, TK_E,
			   0xFFA2, 0x0006, 0x0300, 0x0101, 0x0101,
			   TK_C },
		30
	};
	W expected_segment_num = 7;
	test_texteditor_insertcontext_t testdata = {
		original,
		insert, insert_len, insert_pos,
		expected, expected_segment_num
	};
	return test_texteditor_insertcontext_common(&testdata);
}

LOCAL UNITTEST_RESULT test_texteditor_insertcontext_13()
{
	test_data_t original = {
		(TC[]){TK_A, TK_B, TK_C},
		6
	};
	test_data_t insert[] = {
		{ (TC[]){0xFFA2, 0x0006, 0x0300, 0x0101, 0x0102, TK_D, TK_E}, 14 }
	};
	W insert_len = 1;
	W insert_pos = 3;
	test_data_t expected = {
		(TC[]){TK_A, TK_B, TK_C,
			   0xFFA2, 0x0006, 0x0300, 0x0101, 0x0102,
			   TK_D, TK_E,
			   0xFFA2, 0x0006, 0x0300, 0x0101, 0x0101},
		30
	};
	W expected_segment_num = 7;
	test_texteditor_insertcontext_t testdata = {
		original,
		insert, insert_len, insert_pos,
		expected, expected_segment_num
	};
	return test_texteditor_insertcontext_common(&testdata);
}

/* */

typedef struct {
	test_data_t src;
	W expected;
} test_texteditor_charlength_t;

LOCAL UNITTEST_RESULT test_texteditor_charlength_common(test_texteditor_charlength_t *testdata)
{
	texteditor_textfragment_t fragment;
	W len, err;
	UNITTEST_RESULT result = UNITTEST_RESULT_PASS;

	err = texteditor_textfragment_initialize(&fragment);
	if (err < 0) {
		return UNITTEST_RESULT_FAIL;
	}

	err = test_texteditor_textfragment_initialvalue(&fragment, &testdata->src);
	if (err < 0) {
		printf("texteditor value initialize error\n");
		result = UNITTEST_RESULT_FAIL;
	}

	len = texteditor_textfragment_getcharlength(&fragment);
	if (len != testdata->expected) {
		printf("length error: expected = %d, result = %d\n", testdata->expected, len);
	}

	texteditor_textfragment_finalize(&fragment);

	return result;
}

LOCAL UNITTEST_RESULT test_texteditor_charlength_1()
{
	test_data_t src = {
		(TC[]){TK_A, TK_B, TK_C},
		6
	};
	W expected = 3;
	test_texteditor_charlength_t testdata = {
		src,
		expected
	};
	return test_texteditor_charlength_common(&testdata);
}

LOCAL UNITTEST_RESULT test_texteditor_charlength_2()
{
	test_data_t src = {
		(TC[]){0xFE22, TK_D, TK_E, 0xFE21, TK_A, TK_B, TK_C},
		14
	};
	W expected = 5;
	test_texteditor_charlength_t testdata = {
		src,
		expected
	};
	return test_texteditor_charlength_common(&testdata);
}

LOCAL UNITTEST_RESULT test_texteditor_charlength_3()
{
	test_data_t src = {
		(TC[]){0xFFA2, 0x0006, 0x0300, 0x0101, 0x0102, TK_D, TK_E},
		14
	};
	W expected = 2;
	test_texteditor_charlength_t testdata = {
		src,
		expected
	};
	return test_texteditor_charlength_common(&testdata);
}

LOCAL UNITTEST_RESULT test_texteditor_charlength_4()
{
	test_data_t src = {
		NULL,
		0
	};
	W expected = 0;
	test_texteditor_charlength_t testdata = {
		src,
		expected
	};
	return test_texteditor_charlength_common(&testdata);
}

EXPORT VOID test_texteditor_textfragment_main(unittest_driver_t *driver)
{
	UNITTEST_DRIVER_REGIST(driver, test_texteditor_textfragment_1);
	UNITTEST_DRIVER_REGIST(driver, test_texteditor_insertcontext_1);
	UNITTEST_DRIVER_REGIST(driver, test_texteditor_insertcontext_2);
	UNITTEST_DRIVER_REGIST(driver, test_texteditor_insertcontext_3);
	UNITTEST_DRIVER_REGIST(driver, test_texteditor_insertcontext_4);
	UNITTEST_DRIVER_REGIST(driver, test_texteditor_insertcontext_5);
	UNITTEST_DRIVER_REGIST(driver, test_texteditor_insertcontext_6);
	UNITTEST_DRIVER_REGIST(driver, test_texteditor_insertcontext_7);
	UNITTEST_DRIVER_REGIST(driver, test_texteditor_insertcontext_8);
	UNITTEST_DRIVER_REGIST(driver, test_texteditor_insertcontext_9);
	UNITTEST_DRIVER_REGIST(driver, test_texteditor_insertcontext_10);
	UNITTEST_DRIVER_REGIST(driver, test_texteditor_insertcontext_11);
	UNITTEST_DRIVER_REGIST(driver, test_texteditor_insertcontext_12);
	UNITTEST_DRIVER_REGIST(driver, test_texteditor_insertcontext_13);
	UNITTEST_DRIVER_REGIST(driver, test_texteditor_charlength_1);
	UNITTEST_DRIVER_REGIST(driver, test_texteditor_charlength_2);
	UNITTEST_DRIVER_REGIST(driver, test_texteditor_charlength_3);
	UNITTEST_DRIVER_REGIST(driver, test_texteditor_charlength_4);
}

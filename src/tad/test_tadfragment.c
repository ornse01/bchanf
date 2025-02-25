/*
 * test_tadfragment.c
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

#include "test_tad.h"

#include "tadfragment.h"

#include	<basic.h>
#include	<bstdio.h>
#include	<bstdlib.h>
#include	<tcode.h>

#include "tadlangcode.h"
#include "tadsegment.h"

#include    <unittest_driver.h>

typedef struct {
	TC *input;
	W input_len;
	W segment_num;
} test_tadfragment_pushback_t;

LOCAL UNITTEST_RESULT test_tadfragment_pushback_common(test_tadfragment_pushback_t *testdata)
{
	tadfragment_t fragment;
	W len, err;
	UNITTEST_RESULT result = UNITTEST_RESULT_PASS;

	err = tadfragment_initialize(&fragment);
	if (err < 0) {
		return UNITTEST_RESULT_FAIL;
	}

	err = tadfragment_pushback(&fragment, (UB*)testdata->input, testdata->input_len);
	if (err < 0) {
		printf("tadfragment_pushback error\n");
		result = UNITTEST_RESULT_FAIL;
	}

	len = tadfragment_getsegmentlength(&fragment);
	if (len != testdata->segment_num) {
		printf("tadfragment_getsegmentlength fail: expected = %d, result = %d\n", testdata->segment_num, len);
		result = UNITTEST_RESULT_FAIL;
	}

	len = tadfragment_getbufferlength(&fragment);
	if (len != testdata->input_len) {
		printf("tadfragment_getbufferlength fail: expected = %d, result = %d\n", testdata->input_len, len);
		result = UNITTEST_RESULT_FAIL;
	}

	tadfragment_finalize(&fragment);

	return result;
}

LOCAL UNITTEST_RESULT test_tadfragment_pushback_1()
{
	TC tad[] = {TK_A, TK_B, TK_C};
	W tad_len = sizeof(tad);
	W seg_len = 3;
	test_tadfragment_pushback_t testdata = {
		tad, tad_len, seg_len
	};
	return test_tadfragment_pushback_common(&testdata);
}

LOCAL UNITTEST_RESULT test_tadfragment_pushback_2()
{
	TC tad[] = {TK_A, TK_B, TK_C, TK_D};
	W tad_len = sizeof(tad);
	W seg_len = 4;
	test_tadfragment_pushback_t testdata = {
		tad, tad_len, seg_len
	};
	return test_tadfragment_pushback_common(&testdata);
}

LOCAL UNITTEST_RESULT test_tadfragment_pushback_3()
{
	TC tad[] = {TK_A, 0xFF80, 0x0002, 0x0000, TK_C};
	W tad_len = sizeof(tad);
	W seg_len = 3;
	test_tadfragment_pushback_t testdata = {
		tad, tad_len, seg_len
	};
	return test_tadfragment_pushback_common(&testdata);
}

LOCAL UNITTEST_RESULT test_tadfragment_pushback_4()
{
	TC tad[] = {TK_A, TK_B, 0xFE21};
	W tad_len = sizeof(tad);
	W seg_len = 3;
	test_tadfragment_pushback_t testdata = {
		tad, tad_len, seg_len
	};
	return test_tadfragment_pushback_common(&testdata);
}

LOCAL UNITTEST_RESULT test_tadfragment_pushback_5()
{
	TC tad[] = {TK_A, TK_B, 0xFE21, TK_D};
	W tad_len = sizeof(tad);
	W seg_len = 4;
	test_tadfragment_pushback_t testdata = {
		tad, tad_len, seg_len
	};
	return test_tadfragment_pushback_common(&testdata);
}

LOCAL UNITTEST_RESULT test_tadfragment_pushback_6()
{
	TC tad[] = {TK_A, TK_B, 0xFEFE, 0xFE21};
	W tad_len = sizeof(tad);
	W seg_len = 3;
	test_tadfragment_pushback_t testdata = {
		tad, tad_len, seg_len
	};
	return test_tadfragment_pushback_common(&testdata);
}

LOCAL UNITTEST_RESULT test_tadfragment_pushback_7()
{
	TC tad[] = {TK_A, TK_B, 0xFEFE, 0xFE21, TK_C};
	W tad_len = sizeof(tad);
	W seg_len = 4;
	test_tadfragment_pushback_t testdata = {
		tad, tad_len, seg_len
	};
	return test_tadfragment_pushback_common(&testdata);
}

typedef struct {
	TC *input;
	W input_len;
	W segment_num;
	W expected_len;
} test_tadfragment_popback_t;

LOCAL UNITTEST_RESULT test_tadfragment_popback_common(test_tadfragment_popback_t *testdata)
{
	tadfragment_t fragment;
	W len, err;
	UNITTEST_RESULT result = UNITTEST_RESULT_PASS;

	err = tadfragment_initialize(&fragment);
	if (err < 0) {
		return UNITTEST_RESULT_FAIL;
	}

	err = tadfragment_pushback(&fragment, (UB*)testdata->input, testdata->input_len);
	if (err < 0) {
		printf("tadfragment_pushback error\n");
		result = UNITTEST_RESULT_FAIL;
	}
	err = tadfragment_popback(&fragment);
	if (err < 0) {
		printf("tadfragment_popback error\n");
		result = UNITTEST_RESULT_FAIL;
	}

	len = tadfragment_getsegmentlength(&fragment);
	if (len != testdata->segment_num) {
		printf("tadfragment_getsegmentlength fail: expected = %d, result = %d\n", testdata->segment_num, len);
		result = UNITTEST_RESULT_FAIL;
	}

	len = tadfragment_getbufferlength(&fragment);
	if (len != testdata->expected_len) {
		printf("tadfragment_getbufferlength fail: expected = %d, result = %d\n", testdata->expected_len, len);
		result = UNITTEST_RESULT_FAIL;
	}

	tadfragment_finalize(&fragment);

	return result;
}

LOCAL UNITTEST_RESULT test_tadfragment_popback_1()
{
	TC tad[] = {TK_A, TK_B, TK_C};
	W tad_len = sizeof(tad);
	W expected_len = tad_len - sizeof(TC);
	W seg_len = 2;
	test_tadfragment_popback_t testdata = {
		tad, tad_len, seg_len, expected_len
	};
	return test_tadfragment_popback_common(&testdata);
}

LOCAL UNITTEST_RESULT test_tadfragment_popback_2()
{
	TC tad[] = {TK_A, TK_B, TK_C, TK_D};
	W tad_len = sizeof(tad);
	W expected_len = tad_len - sizeof(TC);
	W seg_len = 3;
	test_tadfragment_popback_t testdata = {
		tad, tad_len, seg_len, expected_len
	};
	return test_tadfragment_popback_common(&testdata);
}

LOCAL UNITTEST_RESULT test_tadfragment_popback_3()
{
	TC tad[] = {TK_A, 0xFF80, 0x0002, 0x0000, TK_C};
	W tad_len = sizeof(tad);
	W expected_len = tad_len - sizeof(TC);
	W seg_len = 2;
	test_tadfragment_popback_t testdata = {
		tad, tad_len, seg_len, expected_len
	};
	return test_tadfragment_popback_common(&testdata);
}

LOCAL UNITTEST_RESULT test_tadfragment_popback_4()
{
	TC tad[] = {TK_A, TK_B, 0xFF80, 0x0002, 0x0000};
	W tad_len = sizeof(tad);
	W expected_len = tad_len - sizeof(TC)*3;
	W seg_len = 2;
	test_tadfragment_popback_t testdata = {
		tad, tad_len, seg_len, expected_len
	};
	return test_tadfragment_popback_common(&testdata);
}

LOCAL UNITTEST_RESULT test_tadfragment_popback_5()
{
	TC tad[] = {TK_A, TK_B, 0xFE21};
	W tad_len = sizeof(tad);
	W expected_len = tad_len - sizeof(TC);
	W seg_len = 2;
	test_tadfragment_popback_t testdata = {
		tad, tad_len, seg_len, expected_len
	};
	return test_tadfragment_popback_common(&testdata);
}

LOCAL UNITTEST_RESULT test_tadfragment_popback_6()
{
	TC tad[] = {TK_A, TK_B, 0xFE21, TK_D};
	W tad_len = sizeof(tad);
	W expected_len = tad_len - sizeof(TC);
	W seg_len = 3;
	test_tadfragment_popback_t testdata = {
		tad, tad_len, seg_len, expected_len
	};
	return test_tadfragment_popback_common(&testdata);
}

LOCAL UNITTEST_RESULT test_tadfragment_popback_7()
{
	TC tad[] = {TK_A, TK_B, 0xFEFE, 0xFE21};
	W tad_len = sizeof(tad);
	W expected_len = tad_len - sizeof(TC)*2;
	W seg_len = 2;
	test_tadfragment_popback_t testdata = {
		tad, tad_len, seg_len, expected_len
	};
	return test_tadfragment_popback_common(&testdata);
}

LOCAL UNITTEST_RESULT test_tadfragment_popback_8()
{
	TC tad[] = {TK_A, TK_B, 0xFEFE, 0xFE21, TK_C};
	W tad_len = sizeof(tad);
	W expected_len = tad_len - sizeof(TC);
	W seg_len = 3;
	test_tadfragment_popback_t testdata = {
		tad, tad_len, seg_len, expected_len
	};
	return test_tadfragment_popback_common(&testdata);
}

typedef struct {
	TC *original;
	W original_len;
	W truncate_size;
	TC *expected;
	W expected_len;
	W expected_segment_num;
} test_tadfragment_truncate_t;

LOCAL UNITTEST_RESULT test_tadfragment_truncate_common(test_tadfragment_truncate_t *testdata)
{
	tadfragment_t fragment;
	W len, err;
	UB *buf;
	UNITTEST_RESULT result = UNITTEST_RESULT_PASS;

	err = tadfragment_initialize(&fragment);
	if (err < 0) {
		return UNITTEST_RESULT_FAIL;
	}

	err = tadfragment_pushback(&fragment, (UB*)testdata->original, testdata->original_len);
	if (err < 0) {
		printf("tadfragment_pushback error\n");
		result = UNITTEST_RESULT_FAIL;
	}

	tadfragment_truncate(&fragment, testdata->truncate_size);

	len = tadfragment_getsegmentlength(&fragment);
	if (len != testdata->expected_segment_num) {
		printf("tadfragment_getsegmentlength fail: expected = %d, result = %d\n", testdata->expected_segment_num, len);
		result = UNITTEST_RESULT_FAIL;
	}

	len = tadfragment_getbufferlength(&fragment);
	if (len != testdata->expected_len) {
		printf("tadfragment_getbufferlength fail: expected = %d, result = %d\n", testdata->expected_len, len);
		result = UNITTEST_RESULT_FAIL;
	}

	buf = tadfragment_getbuffer(&fragment);
	if (memcmp(buf, testdata->expected, testdata->expected_len) != 0) {
		printf("tadfragment_getbuffer fail\n");
		{
			W i;
			for (i = 0; i < testdata->expected_len; i++) {
				printf("%02x, %02x\n", buf[i], ((UB*)testdata->expected)[i]);
			}
		}
		result = UNITTEST_RESULT_FAIL;
	}

	tadfragment_finalize(&fragment);

	return result;
}

LOCAL UNITTEST_RESULT test_tadfragment_truncate_1()
{
	TC original[] = {TK_A, TK_B, TK_C};
	W original_len = sizeof(original);
	W truncate_size = 0;
	TC expected[] = {};
	W expected_len = sizeof(expected);
	W expected_segment_num = 0;
	test_tadfragment_truncate_t testdata = {
		original, original_len,
		truncate_size,
		expected, expected_len, expected_segment_num
	};
	return test_tadfragment_truncate_common(&testdata);
}

LOCAL UNITTEST_RESULT test_tadfragment_truncate_2()
{
	TC original[] = {TK_A, TK_B, TK_C};
	W original_len = sizeof(original);
	W truncate_size = 1;
	TC expected[] = {TK_A};
	W expected_len = sizeof(expected);
	W expected_segment_num = 1;
	test_tadfragment_truncate_t testdata = {
		original, original_len,
		truncate_size,
		expected, expected_len, expected_segment_num
	};
	return test_tadfragment_truncate_common(&testdata);
}

LOCAL UNITTEST_RESULT test_tadfragment_truncate_3()
{
	TC original[] = {TK_A, TK_B, TK_C};
	W original_len = sizeof(original);
	W truncate_size = 2;
	TC expected[] = {TK_A, TK_B};
	W expected_len = sizeof(expected);
	W expected_segment_num = 2;
	test_tadfragment_truncate_t testdata = {
		original, original_len,
		truncate_size,
		expected, expected_len, expected_segment_num
	};
	return test_tadfragment_truncate_common(&testdata);
}

LOCAL UNITTEST_RESULT test_tadfragment_truncate_4()
{
	TC original[] = {TK_A, TK_B, TK_C};
	W original_len = sizeof(original);
	W truncate_size = 3;
	TC expected[] = {TK_A, TK_B, TK_C};
	W expected_len = sizeof(expected);
	W expected_segment_num = 3;
	test_tadfragment_truncate_t testdata = {
		original, original_len,
		truncate_size,
		expected, expected_len, expected_segment_num
	};
	return test_tadfragment_truncate_common(&testdata);
}

LOCAL UNITTEST_RESULT test_tadfragment_truncate_5()
{
	TC original[] = {TK_A, TK_B, TK_C};
	W original_len = sizeof(original);
	W truncate_size = 4;
	TC expected[] = {TK_A, TK_B, TK_C};
	W expected_len = sizeof(expected);
	W expected_segment_num = 3;
	test_tadfragment_truncate_t testdata = {
		original, original_len,
		truncate_size,
		expected, expected_len, expected_segment_num
	};
	return test_tadfragment_truncate_common(&testdata);
}

typedef struct {
	TC *original;
	W original_len;
	TC *insert;
	W insert_len;
	W insert_pos;
	TC *expected;
	W expected_len;
	W expected_segment_num;
} test_tadfragment_cursor_insert_t;

LOCAL UNITTEST_RESULT test_tadfragment_cursor_insert_common(test_tadfragment_cursor_insert_t *testdata)
{
	tadfragment_t fragment;
	tadfragment_cursor_t cursor;
	W len, err;
	UB *buf;
	UNITTEST_RESULT result = UNITTEST_RESULT_PASS;

	err = tadfragment_initialize(&fragment);
	if (err < 0) {
		return UNITTEST_RESULT_FAIL;
	}

	err = tadfragment_pushback(&fragment, (UB*)testdata->original, testdata->original_len);
	if (err < 0) {
		printf("tadfragment_pushback error\n");
		result = UNITTEST_RESULT_FAIL;
	}

	tadfragment_cursor_initialize(&cursor, &fragment);

	err = tadfragment_cursor_move(&cursor, testdata->insert_pos);
	if (err < 0) {
		printf("tadfragment_cursor_move error\n");
		result = UNITTEST_RESULT_FAIL;
	}
	err = tadfragment_cursor_insert(&cursor, (UB*)testdata->insert, testdata->insert_len);
	if (err < 0) {
		printf("tadfragment_cursor_insert error\n");
		result = UNITTEST_RESULT_FAIL;
	}

	tadfragment_cursor_finalize(&cursor);

	len = tadfragment_getsegmentlength(&fragment);
	if (len != testdata->expected_segment_num) {
		printf("tadfragment_getsegmentlength fail: expected = %d, result = %d\n", testdata->expected_segment_num, len);
		result = UNITTEST_RESULT_FAIL;
	}

	len = tadfragment_getbufferlength(&fragment);
	if (len != testdata->expected_len) {
		printf("tadfragment_getbufferlength fail: expected = %d, result = %d\n", testdata->expected_len, len);
		result = UNITTEST_RESULT_FAIL;
	}

	buf = tadfragment_getbuffer(&fragment);
	if (memcmp(buf, testdata->expected, testdata->expected_len) != 0) {
		printf("tadfragment_getbuffer fail\n");
		{
			W i;
			for (i = 0; i < testdata->expected_len; i++) {
				printf("%02x, %02x\n", buf[i], ((UB*)testdata->expected)[i]);
			}
		}
		result = UNITTEST_RESULT_FAIL;
	}

	tadfragment_finalize(&fragment);

	return result;
}

LOCAL UNITTEST_RESULT test_tadfragment_cursor_insert_1()
{
	TC original[] = {TK_A, TK_B, TK_C};
	W original_len = sizeof(original);
	TC insert[] = {TK_D, TK_E};
	W insert_len = sizeof(insert);
	W insert_pos = 0;
	TC expected[] = {TK_D, TK_E, TK_A, TK_B, TK_C};
	W expected_len = sizeof(expected);
	W expected_segment_num = 5;
	test_tadfragment_cursor_insert_t testdata = {
		original, original_len,
		insert, insert_len, insert_pos,
		expected, expected_len, expected_segment_num
	};
	return test_tadfragment_cursor_insert_common(&testdata);
}

LOCAL UNITTEST_RESULT test_tadfragment_cursor_insert_2()
{
	TC original[] = {TK_A, TK_B, TK_C};
	W original_len = sizeof(original);
	TC insert[] = {TK_D, TK_E};
	W insert_len = sizeof(insert);
	W insert_pos = 1;
	TC expected[] = {TK_A, TK_D, TK_E, TK_B, TK_C};
	W expected_len = sizeof(expected);
	W expected_segment_num = 5;
	test_tadfragment_cursor_insert_t testdata = {
		original, original_len,
		insert, insert_len, insert_pos,
		expected, expected_len, expected_segment_num
	};
	return test_tadfragment_cursor_insert_common(&testdata);
}

LOCAL UNITTEST_RESULT test_tadfragment_cursor_insert_3()
{
	TC original[] = {TK_A, TK_B, TK_C};
	W original_len = sizeof(original);
	TC insert[] = {TK_D, TK_E};
	W insert_len = sizeof(insert);
	W insert_pos = 2;
	TC expected[] = {TK_A, TK_B, TK_D, TK_E, TK_C};
	W expected_len = sizeof(expected);
	W expected_segment_num = 5;
	test_tadfragment_cursor_insert_t testdata = {
		original, original_len,
		insert, insert_len, insert_pos,
		expected, expected_len, expected_segment_num
	};
	return test_tadfragment_cursor_insert_common(&testdata);
}

LOCAL UNITTEST_RESULT test_tadfragment_cursor_insert_4()
{
	TC original[] = {TK_A, TK_B, TK_C};
	W original_len = sizeof(original);
	TC insert[] = {TK_D, TK_E};
	W insert_len = sizeof(insert);
	W insert_pos = 3;
	TC expected[] = {TK_A, TK_B, TK_C, TK_D, TK_E};
	W expected_len = sizeof(expected);
	W expected_segment_num = 5;
	test_tadfragment_cursor_insert_t testdata = {
		original, original_len,
		insert, insert_len, insert_pos,
		expected, expected_len, expected_segment_num
	};
	return test_tadfragment_cursor_insert_common(&testdata);
}

typedef struct {
	TC *original;
	W original_len;
	tadlangcode *insert;
	W insert_pos;
	TC *expected;
	W expected_len;
	W expected_segment_num;
} test_tadfragment_cursor_insertlang_t;

LOCAL UNITTEST_RESULT test_tadfragment_cursor_insertlang_common(test_tadfragment_cursor_insertlang_t *testdata)
{
	tadfragment_t fragment;
	tadfragment_cursor_t cursor;
	W len, err;
	UB *buf;
	UNITTEST_RESULT result = UNITTEST_RESULT_PASS;

	err = tadfragment_initialize(&fragment);
	if (err < 0) {
		return UNITTEST_RESULT_FAIL;
	}

	err = tadfragment_pushback(&fragment, (UB*)testdata->original, testdata->original_len);
	if (err < 0) {
		printf("tadfragment_pushback error\n");
		result = UNITTEST_RESULT_FAIL;
	}

	tadfragment_cursor_initialize(&cursor, &fragment);

	err = tadfragment_cursor_move(&cursor, testdata->insert_pos);
	if (err < 0) {
		printf("tadfragment_cursor_move error\n");
		result = UNITTEST_RESULT_FAIL;
	}
	err = tadfragment_cursor_insertlang(&cursor, testdata->insert);
	if (err < 0) {
		printf("tadfragment_cursor_insertlang error\n");
		result = UNITTEST_RESULT_FAIL;
	}

	tadfragment_cursor_finalize(&cursor);

	len = tadfragment_getsegmentlength(&fragment);
	if (len != testdata->expected_segment_num) {
		printf("tadfragment_getsegmentlength fail: expected = %d, result = %d\n", testdata->expected_segment_num, len);
		result = UNITTEST_RESULT_FAIL;
	}

	len = tadfragment_getbufferlength(&fragment);
	if (len != testdata->expected_len) {
		printf("tadfragment_getbufferlength fail: expected = %d, result = %d\n", testdata->expected_len, len);
		result = UNITTEST_RESULT_FAIL;
	}

	buf = tadfragment_getbuffer(&fragment);
	if (memcmp(buf, testdata->expected, testdata->expected_len) != 0) {
		printf("tadfragment_getbuffer fail\n");
		{
			W i;
			for (i = 0; i < testdata->expected_len; i++) {
				printf("%02x, %02x\n", buf[i], ((UB*)testdata->expected)[i]);
			}
		}
		result = UNITTEST_RESULT_FAIL;
	}

	tadfragment_finalize(&fragment);

	return result;
}

LOCAL UNITTEST_RESULT test_tadfragment_cursor_insertlang_1()
{
	TC original[] = {TK_A, TK_B, TK_C};
	W original_len = sizeof(original);
	tadlangcode insert = {0, 0x21};
	W insert_pos = 0;
	TC expected[] = {0xFE21, TK_A, TK_B, TK_C};
	W expected_len = sizeof(expected);
	W expected_segment_num = 4;
	test_tadfragment_cursor_insertlang_t testdata = {
		original, original_len,
		&insert, insert_pos,
		expected, expected_len, expected_segment_num
	};
	return test_tadfragment_cursor_insertlang_common(&testdata);
}

LOCAL UNITTEST_RESULT test_tadfragment_cursor_insertlang_2()
{
	TC original[] = {TK_A, TK_B, TK_C};
	W original_len = sizeof(original);
	tadlangcode insert = {0, 0x21};
	W insert_pos = 1;
	TC expected[] = {TK_A, 0xFE21, TK_B, TK_C};
	W expected_len = sizeof(expected);
	W expected_segment_num = 4;
	test_tadfragment_cursor_insertlang_t testdata = {
		original, original_len,
		&insert, insert_pos,
		expected, expected_len, expected_segment_num
	};
	return test_tadfragment_cursor_insertlang_common(&testdata);
}

LOCAL UNITTEST_RESULT test_tadfragment_cursor_insertlang_3()
{
	TC original[] = {TK_A, TK_B, TK_C};
	W original_len = sizeof(original);
	tadlangcode insert = {0, 0x21};
	W insert_pos = 2;
	TC expected[] = {TK_A, TK_B, 0xFE21, TK_C};
	W expected_len = sizeof(expected);
	W expected_segment_num = 4;
	test_tadfragment_cursor_insertlang_t testdata = {
		original, original_len,
		&insert, insert_pos,
		expected, expected_len, expected_segment_num
	};
	return test_tadfragment_cursor_insertlang_common(&testdata);
}


LOCAL UNITTEST_RESULT test_tadfragment_cursor_insertlang_4()
{
	TC original[] = {TK_A, TK_B, TK_C};
	W original_len = sizeof(original);
	tadlangcode insert = {0, 0x21};
	W insert_pos = 3;
	TC expected[] = {TK_A, TK_B, TK_C, 0xFE21};
	W expected_len = sizeof(expected);
	W expected_segment_num = 4;
	test_tadfragment_cursor_insertlang_t testdata = {
		original, original_len,
		&insert, insert_pos,
		expected, expected_len, expected_segment_num
	};
	return test_tadfragment_cursor_insertlang_common(&testdata);
}

LOCAL UNITTEST_RESULT test_tadfragment_cursor_insertlang_5()
{
	TC original[] = {TK_A, TK_B, TK_C};
	W original_len = sizeof(original);
	tadlangcode insert = {2, 0x21};
	W insert_pos = 1;
	TC expected[] = {TK_A, 0xFEFE, 0xFE21, TK_B, TK_C};
	W expected_len = sizeof(expected);
	W expected_segment_num = 4;
	test_tadfragment_cursor_insertlang_t testdata = {
		original, original_len,
		&insert, insert_pos,
		expected, expected_len, expected_segment_num
	};
	return test_tadfragment_cursor_insertlang_common(&testdata);
}

typedef struct {
	W type;
	TC *data;
	W len;
} test_tadsegment_expected_t;

LOCAL Bool test_tadfragment_common_verify_segment(tadsegment *segment, test_tadsegment_expected_t *expected)
{
	Bool result = True;

	if (segment->type != expected->type) {
		printf("segment type fail: expected = %d, result = %d\n", expected->type, segment->type);
		result = False;
	}
	switch (segment->type) {
	case TADSEGMENT_TYPE_VARIABLE:
		if (segment->value.variable.rawlen != expected->len * sizeof(TC)) {
			printf("segment length fail: expected = %d, result = %d\n", expected->len * sizeof(TC), segment->value.variable.rawlen);
			result = False;
		}
		if (memcmp(segment->value.variable.raw, expected->data, segment->value.variable.rawlen) != 0) {
			printf("segment data fail\n");
			result = False;
		}
		break;
	case TADSEGMENT_TYPE_CHARACTOR:
		if (expected->len != 1) {
			printf("charactor length fail\n");
			result = False;
		} else if (segment->value.ch != expected->data[0]) {
			printf("charactor ch fail\n");
			result = False;
		}
		break;
	case TADSEGMENT_TYPE_LANGCODE:
		result = tadlangcodecmpTC(expected->data, expected->len, &segment->value.lang);
		break;
	default:
		result = False;
		break;
	}

	return result;
}

LOCAL Bool test_tadfragment_common_verify_segments(tadfragment_cursor_t *cursor, test_tadsegment_expected_t *expected, W expected_len)
{
	tadsegment segment;
	W i, err;
	Bool result = True, verify;

	for (i = 0;; i++) {
		err = tadfragment_cursor_getdata(cursor, &segment);
		if (i < expected_len) {
			if (err < 0) {
				printf("tadfragment_pushback error\n");
				result = False;
			}

			verify = test_tadfragment_common_verify_segment(&segment, expected + i);
			if (verify == False) {
				printf("segment %d is not expected\n", i);
				result = False;
			}
		}

		err = tadfragment_cursor_move(cursor, 1);
		if (err < 0) {
			break;
		}
	}
	if (i != expected_len) {
		printf("tadfragment_cursor_move time fail: expected = %d, result = %d\n", expected_len, i);
		result = False;
	}

	return result;
}

typedef struct {
	TC *input;
	W input_len;
	test_tadsegment_expected_t *expected;
	W expected_len;
} test_tadfragment_cursor_getdata_t;

LOCAL UNITTEST_RESULT test_tadfragment_cursor_getdata_common(test_tadfragment_cursor_getdata_t *testdata)
{
	tadfragment_t fragment;
	tadfragment_cursor_t cursor;
	W err;
	Bool verify;
	UNITTEST_RESULT result = UNITTEST_RESULT_PASS;

	err = tadfragment_initialize(&fragment);
	if (err < 0) {
		return UNITTEST_RESULT_FAIL;
	}

	err = tadfragment_pushback(&fragment, (UB*)testdata->input, testdata->input_len);
	if (err < 0) {
		printf("tadfragment_pushback error\n");
		result = UNITTEST_RESULT_FAIL;
	}

	tadfragment_cursor_initialize(&cursor, &fragment);

	verify = test_tadfragment_common_verify_segments(&cursor, testdata->expected, testdata->expected_len);
	if (verify == False) {
		result = UNITTEST_RESULT_FAIL;
	}

	tadfragment_cursor_finalize(&cursor);

	tadfragment_finalize(&fragment);

	return result;
}

LOCAL UNITTEST_RESULT test_tadfragment_cursor_getdata_1()
{
	TC input[] = {TK_A, TK_B, TK_C};
	W input_len = sizeof(input);
	test_tadsegment_expected_t expected[] = {
		{TADSEGMENT_TYPE_CHARACTOR, (TC[]){TK_A}, 1},
		{TADSEGMENT_TYPE_CHARACTOR, (TC[]){TK_B}, 1},
		{TADSEGMENT_TYPE_CHARACTOR, (TC[]){TK_C}, 1},
	};
	W expected_len = 3;
	test_tadfragment_cursor_getdata_t testdata = {
		input, input_len,
		expected, expected_len
	};
	return test_tadfragment_cursor_getdata_common(&testdata);
}

LOCAL UNITTEST_RESULT test_tadfragment_cursor_getdata_2()
{
	TC input[] = {TK_A, 0xFF80, 0x0002, 0x0000, TK_C};
	W input_len = sizeof(input);
	test_tadsegment_expected_t expected[] = {
		{TADSEGMENT_TYPE_CHARACTOR, (TC[]){TK_A}, 1},
		{TADSEGMENT_TYPE_VARIABLE, (TC[]){0xFF80, 0x0002, 0x0000}, 3},
		{TADSEGMENT_TYPE_CHARACTOR, (TC[]){TK_C}, 1},
	};
	W expected_len = 3;
	test_tadfragment_cursor_getdata_t testdata = {
		input, input_len,
		expected, expected_len
	};
	return test_tadfragment_cursor_getdata_common(&testdata);
}

LOCAL UNITTEST_RESULT test_tadfragment_cursor_getdata_3()
{
	TC input[] = {TK_A, TK_B, 0xFE21};
	W input_len = sizeof(input);
	test_tadsegment_expected_t expected[] = {
		{TADSEGMENT_TYPE_CHARACTOR, (TC[]){TK_A}, 1},
		{TADSEGMENT_TYPE_CHARACTOR, (TC[]){TK_B}, 1},
		{TADSEGMENT_TYPE_LANGCODE, (TC[]){0xFE21}, 1},
	};
	W expected_len = 3;
	test_tadfragment_cursor_getdata_t testdata = {
		input, input_len,
		expected, expected_len
	};
	return test_tadfragment_cursor_getdata_common(&testdata);
}

LOCAL UNITTEST_RESULT test_tadfragment_cursor_getdata_4()
{
	TC input[] = {TK_A, 0xFEFE, 0xFE21, TK_C};
	W input_len = sizeof(input);
	test_tadsegment_expected_t expected[] = {
		{TADSEGMENT_TYPE_CHARACTOR, (TC[]){TK_A}, 1},
		{TADSEGMENT_TYPE_LANGCODE, (TC[]){0xFEFE, 0xFE21}, 2},
		{TADSEGMENT_TYPE_CHARACTOR, (TC[]){TK_C}, 1},
	};
	W expected_len = 3;
	test_tadfragment_cursor_getdata_t testdata = {
		input, input_len,
		expected, expected_len
	};
	return test_tadfragment_cursor_getdata_common(&testdata);
}

typedef struct {
	TC *input;
	W input_len;
	W erase_pos;
	W erase_len;
	Bool expected_pos_after_erase_is_end;
	test_tadsegment_expected_t *expected_pos_after_erase;
	test_tadsegment_expected_t *expected;
	W expected_len;
} test_tadfragment_cursor_erase_t;

LOCAL UNITTEST_RESULT test_tadfragment_cursor_erase_common(test_tadfragment_cursor_erase_t *testdata)
{
	tadfragment_t fragment;
	tadfragment_cursor_t cursor;
	tadsegment segment;
	W err;
	Bool verify;
	UNITTEST_RESULT result = UNITTEST_RESULT_PASS;

	err = tadfragment_initialize(&fragment);
	if (err < 0) {
		return UNITTEST_RESULT_FAIL;
	}

	err = tadfragment_pushback(&fragment, (UB*)testdata->input, testdata->input_len);
	if (err < 0) {
		printf("tadfragment_pushback error\n");
		result = UNITTEST_RESULT_FAIL;
	}

	tadfragment_cursor_initialize(&cursor, &fragment);
	tadfragment_cursor_move(&cursor, testdata->erase_pos);
	err = tadfragment_cursor_erase(&cursor, testdata->erase_len);
	if (err < 0) {
		printf("tadfragment_erase error\n");
		result = UNITTEST_RESULT_FAIL;
	}
	if (testdata->expected_pos_after_erase_is_end != False) {
		if (tadfragment_cursor_isend(&cursor) == False) {
			printf("tadfragment_cursor is not end error\n");
			result = UNITTEST_RESULT_FAIL;
		}
	} else {
		err = tadfragment_cursor_getdata(&cursor, &segment);
		if (err < 0) {
			printf("tadfragment_cursor_getdata error\n");
			result = UNITTEST_RESULT_FAIL;
		}
		verify = test_tadfragment_common_verify_segment(&segment, testdata->expected_pos_after_erase);
		if (verify == False) {
			printf("tadfragment_cursor_erase position error\n");
			result = UNITTEST_RESULT_FAIL;
		}
	}
	tadfragment_cursor_finalize(&cursor);


	tadfragment_cursor_initialize(&cursor, &fragment);
	if (test_tadfragment_common_verify_segments(&cursor, testdata->expected, testdata->expected_len) == False) {
		result = UNITTEST_RESULT_FAIL;
	}
	tadfragment_cursor_finalize(&cursor);

	tadfragment_finalize(&fragment);

	return result;
}

LOCAL UNITTEST_RESULT test_tadfragment_cursor_erase_1()
{
	TC input[] = {TK_A, 0xFF80, 0x0002, 0x0000, TK_C};
	W input_len = sizeof(input);
	Bool expected_pos_after_erase_is_end = False;
	test_tadsegment_expected_t expected_pos_after_erase = {
		TADSEGMENT_TYPE_VARIABLE,
		(TC[]){0xFF80, 0x0002, 0x0000},
		3
	};
	W erase_pos = 0;
	W erase_len = 1;
	test_tadsegment_expected_t expected[] = {
		{TADSEGMENT_TYPE_VARIABLE, (TC[]){0xFF80, 0x0002, 0x0000}, 3},
		{TADSEGMENT_TYPE_CHARACTOR, (TC[]){TK_C}, 1},
	};
	W expected_len = 2;
	test_tadfragment_cursor_erase_t testdata = {
		input, input_len,
		erase_pos, erase_len,
		expected_pos_after_erase_is_end, &expected_pos_after_erase,
		expected, expected_len
	};
	return test_tadfragment_cursor_erase_common(&testdata);
}

LOCAL UNITTEST_RESULT test_tadfragment_cursor_erase_2()
{
	TC input[] = {TK_A, 0xFF80, 0x0002, 0x0000, TK_C};
	W input_len = sizeof(input);
	W erase_pos = 0;
	W erase_len = 2;
	Bool expected_pos_after_erase_is_end = False;
	test_tadsegment_expected_t expected_pos_after_erase = {
		TADSEGMENT_TYPE_CHARACTOR,
		(TC[]){TK_C},
		1
	};
	test_tadsegment_expected_t expected[] = {
		{TADSEGMENT_TYPE_CHARACTOR, (TC[]){TK_C}, 1},
	};
	W expected_len = 1;
	test_tadfragment_cursor_erase_t testdata = {
		input, input_len,
		erase_pos, erase_len,
		expected_pos_after_erase_is_end, &expected_pos_after_erase,
		expected, expected_len
	};
	return test_tadfragment_cursor_erase_common(&testdata);
}

LOCAL UNITTEST_RESULT test_tadfragment_cursor_erase_3()
{
	TC input[] = {TK_A, 0xFF80, 0x0002, 0x0000, TK_C};
	W input_len = sizeof(input);
	W erase_pos = 0;
	W erase_len = 3;
	Bool expected_pos_after_erase_is_end = True;
	test_tadsegment_expected_t expected_pos_after_erase = {
	};
	test_tadsegment_expected_t expected[] = {
	};
	W expected_len = 0;
	test_tadfragment_cursor_erase_t testdata = {
		input, input_len,
		erase_pos, erase_len,
		expected_pos_after_erase_is_end, &expected_pos_after_erase,
		expected, expected_len
	};
	return test_tadfragment_cursor_erase_common(&testdata);
}

LOCAL UNITTEST_RESULT test_tadfragment_cursor_erase_4()
{
	TC input[] = {TK_A, 0xFF80, 0x0002, 0x0000, TK_C};
	W input_len = sizeof(input);
	W erase_pos = 0;
	W erase_len = 4;
	Bool expected_pos_after_erase_is_end = True;
	test_tadsegment_expected_t expected_pos_after_erase = {
	};
	test_tadsegment_expected_t expected[] = {
	};
	W expected_len = 0;
	test_tadfragment_cursor_erase_t testdata = {
		input, input_len,
		erase_pos, erase_len,
		expected_pos_after_erase_is_end, &expected_pos_after_erase,
		expected, expected_len
	};
	return test_tadfragment_cursor_erase_common(&testdata);
}

LOCAL UNITTEST_RESULT test_tadfragment_cursor_erase_5()
{
	TC input[] = {TK_A, 0xFF80, 0x0002, 0x0000, TK_C};
	W input_len = sizeof(input);
	W erase_pos = 1;
	W erase_len = 1;
	Bool expected_pos_after_erase_is_end = False;
	test_tadsegment_expected_t expected_pos_after_erase = {
		TADSEGMENT_TYPE_CHARACTOR,
		(TC[]){TK_C},
		1
	};
	test_tadsegment_expected_t expected[] = {
		{TADSEGMENT_TYPE_CHARACTOR, (TC[]){TK_A}, 1},
		{TADSEGMENT_TYPE_CHARACTOR, (TC[]){TK_C}, 1},
	};
	W expected_len = 2;
	test_tadfragment_cursor_erase_t testdata = {
		input, input_len,
		erase_pos, erase_len,
		expected_pos_after_erase_is_end, &expected_pos_after_erase,
		expected, expected_len
	};
	return test_tadfragment_cursor_erase_common(&testdata);
}

LOCAL UNITTEST_RESULT test_tadfragment_cursor_erase_6()
{
	TC input[] = {TK_A, 0xFF80, 0x0002, 0x0000, TK_C};
	W input_len = sizeof(input);
	W erase_pos = 1;
	W erase_len = 2;
	Bool expected_pos_after_erase_is_end = True;
	test_tadsegment_expected_t expected_pos_after_erase = {
	};
	test_tadsegment_expected_t expected[] = {
		{TADSEGMENT_TYPE_CHARACTOR, (TC[]){TK_A}, 1},
	};
	W expected_len = 1;
	test_tadfragment_cursor_erase_t testdata = {
		input, input_len,
		erase_pos, erase_len,
		expected_pos_after_erase_is_end, &expected_pos_after_erase,
		expected, expected_len
	};
	return test_tadfragment_cursor_erase_common(&testdata);
}

LOCAL UNITTEST_RESULT test_tadfragment_cursor_erase_7()
{
	TC input[] = {TK_A, 0xFF80, 0x0002, 0x0000, TK_C};
	W input_len = sizeof(input);
	W erase_pos = 1;
	W erase_len = 3;
	Bool expected_pos_after_erase_is_end = True;
	test_tadsegment_expected_t expected_pos_after_erase = {
	};
	test_tadsegment_expected_t expected[] = {
		{TADSEGMENT_TYPE_CHARACTOR, (TC[]){TK_A}, 1},
	};
	W expected_len = 1;
	test_tadfragment_cursor_erase_t testdata = {
		input, input_len,
		erase_pos, erase_len,
		expected_pos_after_erase_is_end, &expected_pos_after_erase,
		expected, expected_len
	};
	return test_tadfragment_cursor_erase_common(&testdata);
}

LOCAL UNITTEST_RESULT test_tadfragment_cursor_erase_8()
{
	TC input[] = {TK_A, 0xFF80, 0x0002, 0x0000, TK_C};
	W input_len = sizeof(input);
	W erase_pos = 2;
	W erase_len = 1;
	Bool expected_pos_after_erase_is_end = True;
	test_tadsegment_expected_t expected_pos_after_erase = {
	};
	test_tadsegment_expected_t expected[] = {
		{TADSEGMENT_TYPE_CHARACTOR, (TC[]){TK_A}, 1},
		{TADSEGMENT_TYPE_VARIABLE, (TC[]){0xFF80, 0x0002, 0x0000}, 3},
	};
	W expected_len = 2;
	test_tadfragment_cursor_erase_t testdata = {
		input, input_len,
		erase_pos, erase_len,
		expected_pos_after_erase_is_end, &expected_pos_after_erase,
		expected, expected_len
	};
	return test_tadfragment_cursor_erase_common(&testdata);
}

LOCAL UNITTEST_RESULT test_tadfragment_cursor_erase_9()
{
	TC input[] = {TK_A, 0xFF80, 0x0002, 0x0000, TK_C};
	W input_len = sizeof(input);
	W erase_pos = 2;
	W erase_len = 2;
	Bool expected_pos_after_erase_is_end = True;
	test_tadsegment_expected_t expected_pos_after_erase = {
	};
	test_tadsegment_expected_t expected[] = {
		{TADSEGMENT_TYPE_CHARACTOR, (TC[]){TK_A}, 1},
		{TADSEGMENT_TYPE_VARIABLE, (TC[]){0xFF80, 0x0002, 0x0000}, 3},
	};
	W expected_len = 2;
	test_tadfragment_cursor_erase_t testdata = {
		input, input_len,
		erase_pos, erase_len,
		expected_pos_after_erase_is_end, &expected_pos_after_erase,
		expected, expected_len
	};
	return test_tadfragment_cursor_erase_common(&testdata);
}

EXPORT VOID test_tadfragment_main(unittest_driver_t *driver)
{
	UNITTEST_DRIVER_REGIST(driver, test_tadfragment_pushback_1);
	UNITTEST_DRIVER_REGIST(driver, test_tadfragment_pushback_2);
	UNITTEST_DRIVER_REGIST(driver, test_tadfragment_pushback_3);
	UNITTEST_DRIVER_REGIST(driver, test_tadfragment_pushback_4);
	UNITTEST_DRIVER_REGIST(driver, test_tadfragment_pushback_5);
	UNITTEST_DRIVER_REGIST(driver, test_tadfragment_pushback_6);
	UNITTEST_DRIVER_REGIST(driver, test_tadfragment_pushback_7);
	UNITTEST_DRIVER_REGIST(driver, test_tadfragment_popback_1);
	UNITTEST_DRIVER_REGIST(driver, test_tadfragment_popback_2);
	UNITTEST_DRIVER_REGIST(driver, test_tadfragment_popback_3);
	UNITTEST_DRIVER_REGIST(driver, test_tadfragment_popback_4);
	UNITTEST_DRIVER_REGIST(driver, test_tadfragment_popback_5);
	UNITTEST_DRIVER_REGIST(driver, test_tadfragment_popback_6);
	UNITTEST_DRIVER_REGIST(driver, test_tadfragment_popback_7);
	UNITTEST_DRIVER_REGIST(driver, test_tadfragment_popback_8);
	UNITTEST_DRIVER_REGIST(driver, test_tadfragment_truncate_1);
	UNITTEST_DRIVER_REGIST(driver, test_tadfragment_truncate_2);
	UNITTEST_DRIVER_REGIST(driver, test_tadfragment_truncate_3);
	UNITTEST_DRIVER_REGIST(driver, test_tadfragment_truncate_4);
	UNITTEST_DRIVER_REGIST(driver, test_tadfragment_truncate_5);
	UNITTEST_DRIVER_REGIST(driver, test_tadfragment_cursor_insert_1);
	UNITTEST_DRIVER_REGIST(driver, test_tadfragment_cursor_insert_2);
	UNITTEST_DRIVER_REGIST(driver, test_tadfragment_cursor_insert_3);
	UNITTEST_DRIVER_REGIST(driver, test_tadfragment_cursor_insert_4);
	UNITTEST_DRIVER_REGIST(driver, test_tadfragment_cursor_insertlang_1);
	UNITTEST_DRIVER_REGIST(driver, test_tadfragment_cursor_insertlang_2);
	UNITTEST_DRIVER_REGIST(driver, test_tadfragment_cursor_insertlang_3);
	UNITTEST_DRIVER_REGIST(driver, test_tadfragment_cursor_insertlang_4);
	UNITTEST_DRIVER_REGIST(driver, test_tadfragment_cursor_insertlang_5);
	UNITTEST_DRIVER_REGIST(driver, test_tadfragment_cursor_getdata_1);
	UNITTEST_DRIVER_REGIST(driver, test_tadfragment_cursor_getdata_2);
	UNITTEST_DRIVER_REGIST(driver, test_tadfragment_cursor_getdata_3);
	UNITTEST_DRIVER_REGIST(driver, test_tadfragment_cursor_getdata_4);
	UNITTEST_DRIVER_REGIST(driver, test_tadfragment_cursor_erase_1);
	UNITTEST_DRIVER_REGIST(driver, test_tadfragment_cursor_erase_2);
	UNITTEST_DRIVER_REGIST(driver, test_tadfragment_cursor_erase_3);
	UNITTEST_DRIVER_REGIST(driver, test_tadfragment_cursor_erase_4);
	UNITTEST_DRIVER_REGIST(driver, test_tadfragment_cursor_erase_5);
	UNITTEST_DRIVER_REGIST(driver, test_tadfragment_cursor_erase_6);
	UNITTEST_DRIVER_REGIST(driver, test_tadfragment_cursor_erase_7);
	UNITTEST_DRIVER_REGIST(driver, test_tadfragment_cursor_erase_8);
	UNITTEST_DRIVER_REGIST(driver, test_tadfragment_cursor_erase_9);
}

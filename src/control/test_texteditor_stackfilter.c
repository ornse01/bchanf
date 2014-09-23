/*
 * test_texteditor_stackfilter.c
 *
 * Copyright (c) 2014 project bchan
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

#include "test_control.h"

#include "texteditor_stackfilter.h"

#include	<basic.h>
#include	<bstdio.h>
#include	<bstdlib.h>
#include	<bstring.h>
#include	<tstring.h>
#include	<tcode.h>

#include    <tad/taddecoder.h>
#include    <tad/tadfragment.h>

#include    <unittest_driver.h>

typedef struct {
	TC *src;
	W src_len;
	TC *expected;
	W expected_len;
} test_texteditor_stackfilter_testdata_t;

LOCAL UNITTEST_RESULT test_texteditor_stackfilter_common(test_texteditor_stackfilter_testdata_t *testdata)
{
	texteditor_stackfilter_t filter;
	taddecoder_t decoder;
	tadsegment segment, *filterd;
	tadfragment_t fragment;
	tadfragment_cursor_t cursor;
	UB *buf;
	Bool cont;
	W input_result, err, len;
	UNITTEST_RESULT result = UNITTEST_RESULT_PASS;

	err = tadfragment_initialize(&fragment);
	if (err < 0) {
		return UNITTEST_RESULT_FAIL;
	}

	texteditor_stackfilter_initialize(&filter);

	taddecoder_initialize(&decoder, testdata->src, testdata->src_len);

	tadfragment_cursor_initialize(&cursor, &fragment);
	for (;;) {
		cont = taddecoder_next(&decoder, &segment);
		if (cont == False) {
			break;
		}
		input_result = texteditor_stackfilter_setinput(&filter, &segment);
		if (input_result == TEXTEDITOR_STACKFILTER_SETINPUT_RESULT_FULL) {
			for (;;) {
				cont = texteditor_stackfilter_getoutput(&filter, &filterd);
				if (cont == False) {
					break;
				}
				tadfragment_cursor_insertsegment(&cursor, filterd);
			}
			input_result = texteditor_stackfilter_setinput(&filter, &segment);
		}
		if (input_result != TEXTEDITOR_STACKFILTER_SETINPUT_RESULT_OK) {
			result = UNITTEST_RESULT_FAIL;
			break;
		}
	}
	for (;;) {
		cont = texteditor_stackfilter_getoutput(&filter, &filterd);
		if (cont == False) {
			break;
		}
		tadfragment_cursor_insertsegment(&cursor, filterd);
	}
	tadfragment_cursor_finalize(&cursor);

	taddecoder_finalize(&decoder);

	texteditor_stackfilter_finalize(&filter);

	buf = tadfragment_getbuffer(&fragment);
	len = tadfragment_getbufferlength(&fragment);

	if (len != testdata->expected_len * sizeof(TC)) {
		printf("length error: expected = %d, result = %d\n", testdata->expected_len * sizeof(TC), len);
		result = UNITTEST_RESULT_FAIL;
	} else if (memcmp(buf, testdata->expected, len) != 0) {
		printf("buffer error\n");
		result = UNITTEST_RESULT_FAIL;
	}

	tadfragment_finalize(&fragment);

	return result;
}

LOCAL UNITTEST_RESULT test_texteditor_stackfilter_1()
{
	TC src[] = (TC[]){
		0xffe1, 0x0018, 0x0000, 0x0000,
		0x0000, 0x0000, 0x0000, 0x0000,
		0x0000, 0x0000, 0xff88, 0xff88,
		0x0021, 0x0000, 0x2422, 0xffa2,
		0x0006, 0x0300, 0x0101, 0x0101,
		0x2422,
	};
	W src_len = 21;
	TC expected[] = (TC[]){
		0xffe1, 0x0018, 0x0000, 0x0000,
		0x0000, 0x0000, 0x0000, 0x0000,
		0x0000, 0x0000, 0xff88, 0xff88,
		0x0021, 0x0000, 0x2422, 0xffa2,
		0x0006, 0x0300, 0x0101, 0x0101,
		0x2422,
	};
	W expected_len = 21;
	test_texteditor_stackfilter_testdata_t testdata = {
		src, src_len, expected, expected_len
	};
	return test_texteditor_stackfilter_common(&testdata);
}

LOCAL UNITTEST_RESULT test_texteditor_stackfilter_2()
{
	TC src[] = (TC[]){
		0x2422, 0x2422,
	};
	W src_len = 2;
	TC expected[] = (TC[]){
		0x2422, 0x2422,
	};
	W expected_len = 2;
	test_texteditor_stackfilter_testdata_t testdata = {
		src, src_len, expected, expected_len
	};
	return test_texteditor_stackfilter_common(&testdata);
}

LOCAL UNITTEST_RESULT test_texteditor_stackfilter_3()
{
	TC src[] = (TC[]){
		0x2422,
		0xffe3, 0x0018, 0x0000, 0x0000,
		0x0000, 0x0000, 0x0000, 0x0000,
		0x0000, 0x0000, 0xff88, 0xff88,
		0x0000, 0x0000,
		0xffb0, 0x0012, 0x0000, 0x0000,
		0x0000, 0x0000, 0x0000, 0x0000,
		0x0000, 0x0000, 0x0000,
		0xffe4, 0x0000,
		0x2422,
	};
	W src_len = 29;
	TC expected[] = (TC[]){
		0x2422, 0x2422,
	};
	W expected_len = 2;
	test_texteditor_stackfilter_testdata_t testdata = {
		src, src_len, expected, expected_len
	};
	return test_texteditor_stackfilter_common(&testdata);
}

LOCAL UNITTEST_RESULT test_texteditor_stackfilter_4()
{
	TC src[] = (TC[]){
		0x2422,
		0xffe1, 0x0018, 0x0000, 0x0000,
		0x0000, 0x0000, 0x0000, 0x0000,
		0x0000, 0x0000, 0xff88, 0xff88,
		0x0021, 0x0000,
		0x2422,
		0xffe2, 0x0000,
		0x2422,
	};
	W src_len = 19;
	TC expected[] = (TC[]){
		0x2422, 0x2422,
	};
	W expected_len = 2;
	test_texteditor_stackfilter_testdata_t testdata = {
		src, src_len, expected, expected_len
	};
	return test_texteditor_stackfilter_common(&testdata);
}

EXPORT VOID test_texteditor_stackfilter_main(unittest_driver_t *driver)
{
	UNITTEST_DRIVER_REGIST(driver, test_texteditor_stackfilter_1);
	UNITTEST_DRIVER_REGIST(driver, test_texteditor_stackfilter_2);
	UNITTEST_DRIVER_REGIST(driver, test_texteditor_stackfilter_3);
	UNITTEST_DRIVER_REGIST(driver, test_texteditor_stackfilter_4);
}

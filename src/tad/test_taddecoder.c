/*
 * test_taddecoder.c
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

#include "test_tad.h"

#include "taddecoder.h"

#include	<basic.h>
#include	<bstdio.h>
#include	<bstdlib.h>
#include	<bstring.h>
#include	<btron/dp.h>
#include	<tad.h>
#include	<tcode.h>

#include	"tadsegment.h"
#include	"tadlangcode.h"

#include    <unittest_driver.h>

LOCAL UB test_taddecoder_testdata01[] = {
	0xe0, 0xff, 0x06, 0x00, 0x00, 0x00, 0x02, 0x00,
	0x22, 0x01, 0xe1, 0xff, 0x18, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x88, 0xff,
	0x88, 0xff, 0x21, 0x00, 0x00, 0x00, 0xa0, 0xff,
	0x0e, 0x00, 0x00, 0x00, 0x7b, 0x05, 0xe0, 0x03,
	0x5e, 0x00, 0x46, 0x00, 0x6c, 0x00, 0x55, 0x00,
	0xa0, 0xff, 0x0a, 0x00, 0x00, 0x01, 0x5e, 0x00,
	0x76, 0x00, 0x6c, 0x00, 0x55, 0x00, 0xa0, 0xff,
	0x22, 0x00, 0x00, 0x03, 0xa0, 0xff, 0x02, 0x00,
	0x00, 0x08, 0xa1, 0xff, 0x04, 0x00, 0x00, 0x00,
	0x00, 0x80, 0xa1, 0xff, 0x02, 0x00, 0x01, 0x01,
	0x5d, 0x21, 0xad, 0xff, 0x04, 0x00, 0x00, 0x00,
	0xc8, 0x00, 0x5d, 0x21, 0xa0, 0xff, 0x04, 0x00,
	0x00, 0x04, 0x00, 0x80, 0xa4, 0xff, 0x24, 0x00,
	0x11, 0x08, 0x22, 0x21, 0x23, 0x21, 0x24, 0x21,
	0x25, 0x21, 0x2b, 0x21, 0x2c, 0x21, 0x47, 0x21,
	0x49, 0x21, 0x4b, 0x21, 0x4d, 0x21, 0x4f, 0x21,
	0x51, 0x21, 0x53, 0x21, 0x55, 0x21, 0x57, 0x21,
	0x59, 0x21, 0x5b, 0x21, 0xa4, 0xff, 0x18, 0x00,
	0x11, 0x09, 0x46, 0x21, 0x48, 0x21, 0x4a, 0x21,
	0x4c, 0x21, 0x4e, 0x21, 0x50, 0x21, 0x52, 0x21,
	0x54, 0x21, 0x56, 0x21, 0x58, 0x21, 0x5a, 0x21,
	0xa1, 0xff, 0x24, 0x00, 0x00, 0x02, 0x04, 0x01,
	0x04, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x0b, 0x00, 0x48, 0x00, 0x90, 0x00, 0xd8, 0x00,
	0x20, 0x01, 0x68, 0x01, 0xb0, 0x01, 0xf8, 0x01,
	0x40, 0x02, 0x88, 0x02, 0xd0, 0x02, 0x18, 0x03,
	0xa1, 0xff, 0x04, 0x00, 0x01, 0x00, 0x04, 0x01,
	0x46, 0x23, 0x52, 0x23, 0x4f, 0x23, 0x4d, 0x23,
	0x27, 0x21, 0x21, 0x21, 0x6e, 0x23, 0x61, 0x23,
	0x6d, 0x23, 0x65, 0x23, 0x0a, 0x00, 0x6d, 0x23,
	0x61, 0x23, 0x69, 0x23, 0x6c, 0x23, 0x27, 0x21,
	0x21, 0x21, 0x6d, 0x23, 0x61, 0x23, 0x69, 0x23,
	0x6c, 0x23, 0x0a, 0x00, 0x0a, 0x00, 0x62, 0x23,
	0x6f, 0x23, 0x64, 0x23, 0x79, 0x23, 0xe2, 0xff,
	0x00, 0x00,
};

LOCAL UNITTEST_RESULT test_taddecoder_1()
{
	taddecoder_t iter;
	tadsegment result;
	Bool cont;

	taddecoder_initialize(&iter, (TC*)test_taddecoder_testdata01, sizeof(test_taddecoder_testdata01)/sizeof(TC));

	for (;;) {
		cont = taddecoder_next(&iter, &result);
		if (cont == False) {
			break;
		}
	}

	taddecoder_finalize(&iter);

	return UNITTEST_RESULT_PASS;
}

typedef struct {
	TC *data;
	W len;
} test_taddecoder_expected_t;

typedef struct {
	TC *src;
	W src_len;
	test_taddecoder_expected_t *expected;
	W expected_len;
} test_taddecoder_t;

LOCAL UNITTEST_RESULT test_taddecoder_common(test_taddecoder_t *testdata)
{
	taddecoder_t iter;
	tadsegment result;
	Bool cont, ok;
	test_taddecoder_expected_t *expected;
	UNITTEST_RESULT ret = UNITTEST_RESULT_PASS;
	W i = 0;

	taddecoder_initialize(&iter, testdata->src, testdata->src_len);

	for (i = 0;; i++) {
		cont = taddecoder_next(&iter, &result);
		if (cont == False) {
			break;
		}

		if (i >= testdata->expected_len) {
			continue;
		}

		expected = testdata->expected + i;

		switch (result.type) {
		case TADSEGMENT_TYPE_VARIABLE:
			if (result.value.variable.rawlen != expected->len * sizeof(TC)) {
				ret = UNITTEST_RESULT_FAIL;
				printf("VARIABLE length failure: expected = %d, result = %d\n", expected->len * sizeof(TC), result.value.variable.rawlen);
			} else if (memcmp(result.value.variable.raw, expected->data, result.value.variable.rawlen) != 0) {
				ret = UNITTEST_RESULT_FAIL;
				printf("VARIABLE data failure\n");
				{
					W j;
					for (j = 0; j < expected->len; j++) {
						printf("%02x, %02x\n", result.value.variable.raw[j], ((UB*)expected->data)[j]);
					}
				}
			}
			break;
		case TADSEGMENT_TYPE_CHARACTOR:
			if (expected->len != 1) {
				ret = UNITTEST_RESULT_FAIL;
				printf("CHARACTOR length failure: expected = %d, result = %d\n", sizeof(TC), expected->len * sizeof(TC));
			} else if (memcmp(&result.value.ch, expected->data, sizeof(TC)) != 0) {
				printf("CHARACTOR data failure: expected = %04x, result = %04x\n", result.value.ch, expected->data[0]);
			}
			break;
		case TADSEGMENT_TYPE_LANGCODE:
			ok = tadlangcodecmpTC(expected->data, expected->len, &result.value.lang);
			if (ok == False) {
				ret = UNITTEST_RESULT_FAIL;
				printf("LANGCODE failure\n");
			}
			break;
		default:
			ret = UNITTEST_RESULT_FAIL;
			printf("invalid tadsegment->type = %d\n", result.type);
			break;
		}
	}

	taddecoder_finalize(&iter);

	if (i != testdata->expected_len) {
		printf("iteration coun fail: expected = %d, result = %d\n", testdata->expected_len, i);
		ret = UNITTEST_RESULT_FAIL;
	}

	return ret;
}

LOCAL UNITTEST_RESULT test_taddecoder_2()
{
	TC src[] = (TC[]){TK_A, TK_B, TK_C};
	W src_len = sizeof(src) / sizeof(TC);
	test_taddecoder_expected_t expected[] = {
		{ (TC[]){TK_A}, 1 },
		{ (TC[]){TK_B}, 1 },
		{ (TC[]){TK_C}, 1 },
	};
	W expected_len = 3;
	test_taddecoder_t testdata = {
		src, src_len,
		expected, expected_len
	};
	return test_taddecoder_common(&testdata);
}

LOCAL UNITTEST_RESULT test_taddecoder_3()
{
	TC src[] = (TC[]){TK_A, 0xFE21, TK_C};
	W src_len = sizeof(src) / sizeof(TC);
	test_taddecoder_expected_t expected[] = {
		{ (TC[]){TK_A}, 1 },
		{ (TC[]){0xFE21}, 1 },
		{ (TC[]){TK_C}, 1 },
	};
	W expected_len = 3;
	test_taddecoder_t testdata = {
		src, src_len,
		expected, expected_len
	};
	return test_taddecoder_common(&testdata);
}

LOCAL UNITTEST_RESULT test_taddecoder_4()
{
	TC src[] = (TC[]){TK_A, 0xFEFE, 0xFE21, TK_C};
	W src_len = sizeof(src) / sizeof(TC);
	test_taddecoder_expected_t expected[] = {
		{ (TC[]){TK_A}, 1 },
		{ (TC[]){0xFEFE, 0xFE21}, 2 },
		{ (TC[]){TK_C}, 1 },
	};
	W expected_len = 3;
	test_taddecoder_t testdata = {
		src, src_len,
		expected, expected_len
	};
	return test_taddecoder_common(&testdata);
}

LOCAL UNITTEST_RESULT test_taddecoder_5()
{
	TC src[] = (TC[]){TK_A, 0xFFA2, 0x0006, 0x0300, 0x0101, 0x0102, TK_C};
	W src_len = sizeof(src) / sizeof(TC);
	test_taddecoder_expected_t expected[] = {
		{ (TC[]){TK_A}, 1 },
		{ (TC[]){0xFFA2, 0x0006, 0x0300, 0x0101, 0x0102}, 5 },
		{ (TC[]){TK_C}, 1 },
	};
	W expected_len = 3;
	test_taddecoder_t testdata = {
		src, src_len,
		expected, expected_len
	};
	return test_taddecoder_common(&testdata);
}

EXPORT VOID test_taddecoder_main(unittest_driver_t *driver)
{
	UNITTEST_DRIVER_REGIST(driver, test_taddecoder_1);
	UNITTEST_DRIVER_REGIST(driver, test_taddecoder_2);
	UNITTEST_DRIVER_REGIST(driver, test_taddecoder_3);
	UNITTEST_DRIVER_REGIST(driver, test_taddecoder_4);
	UNITTEST_DRIVER_REGIST(driver, test_taddecoder_5);
}

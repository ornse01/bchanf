/*
 * test_texteditor_insertfilter.c
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

#include "texteditor_insertfilter.h"

#include	<basic.h>
#include	<bstdio.h>
#include	<bstdlib.h>
#include	<bstring.h>
#include	<tstring.h>
#include	<tcode.h>

#include    <tad/tadlangcode.h>
#include    <tad/taddecoder.h>

#include    <unittest_driver.h>

typedef struct {
	UB *data;
	W len;
} test_texteditor_insertfilter_expected;

LOCAL Bool test_texteditor_insertfilter_cmpsegment(test_texteditor_insertfilter_expected *expected, tadsegment *result)
{
	Bool ok = True;

	switch (result->type) {
	case TADSEGMENT_TYPE_VARIABLE:
		if (result->value.variable.rawlen != expected->len) {
			printf("VAR: return length failure: expected = %d, result = %d\n", expected->len, result->value.variable.rawlen);
			ok = False;
		} else if (memcmp(result->value.variable.raw, expected->data, expected->len) != 0) {
			printf("VAR: return data failure\n");
			ok = False;
		}
		return ok;
	case TADSEGMENT_TYPE_CHARACTOR:
		if (expected->len != sizeof(TC)) {
			printf("CH: return length failure: expected = %d\n", expected->len);
			ok = False;
		} else if (memcmp((UB*)&result->value.ch, expected->data, expected->len) != 0) {
			printf("CH: return data failure: expected = %04x, result = %04x\n", *(TC*)expected->data, result->value.ch);
			ok = False;
		}
		return ok;
	case TADSEGMENT_TYPE_LANGCODE:
		ok = tadlangcodecmpTC((TC*)expected->data, expected->len/sizeof(TC), &result->value.lang);
		if (ok == False) {
			printf("LANG: langcode faiure\n");
		}
		return ok;
	}
	return False;
}

LOCAL UNITTEST_RESULT test_texteditor_insertfilter_common(tadlangcode lang, RATIO w_ratio, UB *data, W len, test_texteditor_insertfilter_expected *expected, W expected_len)
{
	texteditor_insertfilter_t filter;
	texteditor_insertfilterresult_t *result;
	taddecoder_t decoder;
	tadsegment segment, *filterd;
	Bool cont, ok;
	W i, err;
	UNITTEST_RESULT ret = UNITTEST_RESULT_PASS;

	texteditor_insertfilter_initialize(&filter, &lang, w_ratio);

	taddecoder_initialize(&decoder, (TC*)data, len/sizeof(TC));

	for (i = 0;;) {
		cont = taddecoder_next(&decoder, &segment);
		if (cont == False) {
			break;
		}
		err = texteditor_insertfilter_put(&filter, &segment, &result);
		if (err < 0) {
			ret = UNITTEST_RESULT_FAIL;
			break;
		}
		for (;; i++) {
			cont = texteditor_insertfilterresult_next(result, &filterd);
			if (cont == False) {
				break;
			}
			ok = test_texteditor_insertfilter_cmpsegment(expected + i, filterd);
			if (ok == False) {
				printf("segment[%d] fail\n", i);
				ret = UNITTEST_RESULT_FAIL;
			}
		}
	}
	texteditor_insertfilter_endinput(&filter, &result);
	for (;; i++) {
		cont = texteditor_insertfilterresult_next(result, &filterd);
		if (cont == False) {
			break;
		}
		ok = test_texteditor_insertfilter_cmpsegment(expected + i, filterd);
		if (ok == False) {
			ret = UNITTEST_RESULT_FAIL;
		}
	}

	if (i != expected_len) {
		printf("iteration number failure: expected = %d, result = %d\n", expected_len, i);
		ret = UNITTEST_RESULT_FAIL;
	}

	taddecoder_finalize(&decoder);

	texteditor_insertfilter_finalize(&filter);

	return ret;
}

LOCAL UNITTEST_RESULT test_texteditor_insertfilter_1()
{
	tadlangcode lang = (tadlangcode){0, 0x21};
	RATIO w_ratio = 0x0101;
	UB *data = (UB*)(TC[]){TK_A, TK_B, TK_C};
	W len = 6;
	test_texteditor_insertfilter_expected expected[] = {
		{ (UB*)(TC[]){TK_A}, 2 },
		{ (UB*)(TC[]){TK_B}, 2 },
		{ (UB*)(TC[]){TK_C}, 2 },
	};
	W expected_len = 3;
	return test_texteditor_insertfilter_common(lang, w_ratio, data, len, expected, expected_len);
}

LOCAL UNITTEST_RESULT test_texteditor_insertfilter_2()
{
	tadlangcode lang = (tadlangcode){0, 0x22};
	RATIO w_ratio = 0x0101;
	UB *data = (UB*)(TC[]){TK_A, TK_B, TK_C};
	W len = 6;
	test_texteditor_insertfilter_expected expected[] = {
		{ (UB*)(TC[]){0xFE21}, 2 },
		{ (UB*)(TC[]){TK_A}, 2 },
		{ (UB*)(TC[]){TK_B}, 2 },
		{ (UB*)(TC[]){TK_C}, 2 },
		{ (UB*)(TC[]){0xFE22}, 2 },
	};
	W expected_len = 5;
	return test_texteditor_insertfilter_common(lang, w_ratio, data, len, expected, expected_len);
}

LOCAL UNITTEST_RESULT test_texteditor_insertfilter_3()
{
	tadlangcode lang = (tadlangcode){0, 0x21};
	RATIO w_ratio = 0x0102;
	UB *data = (UB*)(TC[]){TK_A, TK_B, TK_C};
	W len = 6;
	test_texteditor_insertfilter_expected expected[] = {
		{ (UB*)(TC[]){ 0xFFA2, 0x0006, 0x0300, 0x0101, 0x0101 }, 10 },
		{ (UB*)(TC[]){TK_A}, 2 },
		{ (UB*)(TC[]){TK_B}, 2 },
		{ (UB*)(TC[]){TK_C}, 2 },
		{ (UB*)(TC[]){ 0xFFA2, 0x0006, 0x0300, 0x0101, 0x0102 }, 10 },
	};
	W expected_len = 5;
	return test_texteditor_insertfilter_common(lang, w_ratio, data, len, expected, expected_len);
}

LOCAL UNITTEST_RESULT test_texteditor_insertfilter_4()
{
	tadlangcode lang = (tadlangcode){0, 0x21};
	RATIO w_ratio = 0x0101;
	UB *data = (UB*)(TC[]){TK_A, TK_B, 0xFE22, TK_C};
	W len = 8;
	test_texteditor_insertfilter_expected expected[] = {
		{ (UB*)(TC[]){TK_A}, 2 },
		{ (UB*)(TC[]){TK_B}, 2 },
		{ (UB*)(TC[]){0xFE22}, 2 },
		{ (UB*)(TC[]){TK_C}, 2 },
		{ (UB*)(TC[]){0xFE21}, 2 },
	};
	W expected_len = 5;
	return test_texteditor_insertfilter_common(lang, w_ratio, data, len, expected, expected_len);
}

LOCAL UNITTEST_RESULT test_texteditor_insertfilter_5()
{
	tadlangcode lang = (tadlangcode){0, 0x21};
	RATIO w_ratio = 0x0101;
	UB *data = (UB*)(TC[]){TK_A, TK_B, 0xFFA2, 0x0006, 0x0300, 0x0101, 0x0102, TK_C};
	W len = 16;
	test_texteditor_insertfilter_expected expected[] = {
		{ (UB*)(TC[]){TK_A}, 2 },
		{ (UB*)(TC[]){TK_B}, 2 },
		{ (UB*)(TC[]){ 0xFFA2, 0x0006, 0x0300, 0x0101, 0x0102 }, 10 },
		{ (UB*)(TC[]){TK_C}, 2 },
		{ (UB*)(TC[]){ 0xFFA2, 0x0006, 0x0300, 0x0101, 0x0101 }, 10 },
	};
	W expected_len = 5;
	return test_texteditor_insertfilter_common(lang, w_ratio, data, len, expected, expected_len);
}

EXPORT VOID test_texteditor_insertfilter_main(unittest_driver_t *driver)
{
	UNITTEST_DRIVER_REGIST(driver, test_texteditor_insertfilter_1);
	UNITTEST_DRIVER_REGIST(driver, test_texteditor_insertfilter_2);
	UNITTEST_DRIVER_REGIST(driver, test_texteditor_insertfilter_3);
	UNITTEST_DRIVER_REGIST(driver, test_texteditor_insertfilter_4);
	UNITTEST_DRIVER_REGIST(driver, test_texteditor_insertfilter_5);
}

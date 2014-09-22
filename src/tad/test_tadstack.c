/*
 * test_tadstack.c
 *
 * Copyright (c) 2012-2014 project bchan
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

#include "tadstack.h"

#include	<basic.h>
#include	<bstdio.h>
#include	<bstdlib.h>
#include	<bstring.h>

#include "taditerator.h"
#include "taddecoder.h"

#include    <unittest_driver.h>

LOCAL UB test_tadstack_testdata01[] = {
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

LOCAL UNITTEST_RESULT test_tadstack_1()
{
	tadstack_t stack;
	taditerator_t iter;
	taditerator_result result;
	TADSTACK_RESULT stk_result;
	UNITTEST_RESULT ok = UNITTEST_RESULT_PASS;

	tadstack_initialize(&stack);
	taditerator_initialize(&iter, (TC*)test_tadstack_testdata01, sizeof(test_tadstack_testdata01)/sizeof(TC));

	for (;;) {
		taditerator_next2(&iter, &result);
		if (result.type == TADITERATOR_RESULTTYPE_END) {
			break;
		}
		if (result.type == TADITERATOR_RESULTTYPE_CHARCTOR) {
			
			stk_result = tadstack_inputcharactor(&stack, result.segment);
		} else if (result.type == TADITERATOR_RESULTTYPE_SEGMENT) {
			stk_result = tadstack_inputvsegment(&stack, result.segment, result.data, result.segsize);
		}
		if (stk_result == TADSTACK_RESULT_FORMAT_ERROR) {
			printf("format error\n");
			ok = UNITTEST_RESULT_FAIL;
		}
	}

	taditerator_finalize(&iter);
	tadstack_finalize(&stack);

	return ok;
}

LOCAL UNITTEST_RESULT test_tadstack_2()
{
	tadstack_t stack;
	taddecoder_t decoder;
	tadsegment segment;
	Bool cont;
	TADSTACK_RESULT stk_result;
	UNITTEST_RESULT ok = UNITTEST_RESULT_PASS;

	tadstack_initialize(&stack);
	taddecoder_initialize(&decoder, (TC*)test_tadstack_testdata01, sizeof(test_tadstack_testdata01)/sizeof(TC));

	for (;;) {
		cont = taddecoder_next(&decoder, &segment);
		if (cont == False) {
			break;
		}
			
		stk_result = tadstack_inputsegment(&stack, &segment);
		if (stk_result == TADSTACK_RESULT_FORMAT_ERROR) {
			printf("format error\n");
			ok = UNITTEST_RESULT_FAIL;
		}
	}

	taddecoder_finalize(&decoder);
	tadstack_finalize(&stack);

	return ok;
}

LOCAL UNITTEST_RESULT test_tadstack_3()
{
	tadstack_t stack;
	tadlangcode langcode;
	TADSTACK_DATATYPE type;
	RECT r;
	UNITS units;
	W err;
	UNITTEST_RESULT ok = UNITTEST_RESULT_PASS;

	tadstack_initialize(&stack);

	err = tadstack_nestlevel(&stack);
	if (err >= 0) {
		printf("tadstack_nestlevel: %d\n", err);
		ok = UNITTEST_RESULT_FAIL;
	}
	err = tadstack_currentlangcode(&stack, &langcode);
	if (err >= 0) {
		printf("tadstack_currentlangcode: %d\n", err);
		ok = UNITTEST_RESULT_FAIL;
	}
	err = tadstack_currentdata(&stack, &type);
	if (err >= 0) {
		printf("tadstack_currentdata: %d\n", err);
		ok = UNITTEST_RESULT_FAIL;
	}
	err = tadstack_currentview(&stack, &r);
	if (err >= 0) {
		printf("tadstack_currentview: %d\n", err);
		ok = UNITTEST_RESULT_FAIL;
	}
	err = tadstack_currentdraw(&stack, &r);
	if (err >= 0) {
		printf("tadstack_currentdraw: %d\n", err);
		ok = UNITTEST_RESULT_FAIL;
	}
	err = tadstack_currenthunit(&stack, &units);
	if (err >= 0) {
		printf("tadstack_currenthunit: %d\n", err);
		ok = UNITTEST_RESULT_FAIL;
	}
	err = tadstack_currentvunit(&stack, &units);
	if (err >= 0) {
		printf("tadstack_currentvunit: %d\n", err);
		ok = UNITTEST_RESULT_FAIL;
	}

	tadstack_finalize(&stack);

	return ok;
}

typedef struct {
	W nestlevel;
	TADSTACK_DATATYPE type;
	TC *langcode;
	W langlen;
	RECT view;
	RECT draw;
	UNITS hunit;
	UNITS vunit;
} test_tadstack_validate_status_t;

LOCAL Bool test_tadstack_validate_status(tadstack_t *stack, test_tadstack_validate_status_t *expected)
{
	tadlangcode langcode;
	TADSTACK_DATATYPE type;
	RECT r;
	UNITS units;
	W err;
	Bool ok = True, cmp;

	err = tadstack_nestlevel(stack);
	if (err < 0) {
		printf("tadstack_nestlevel error: %d\n", err);
		ok = False;
	}
	if (expected->nestlevel != err) {
		printf("tadstack_nestlevel: %d\n", err);
		ok = False;
	}
	err = tadstack_currentdata(stack, &type);
	if (err < 0) {
		printf("tadstack_currentdata error: %d\n", err);
		ok = False;
	}
	if (expected->type != type) {
		printf("tadstack_currentdata: expected = %d, result = %d\n", expected->type, type);
		ok = False;
	}
	if (expected->type == TADSTACK_DATATYPE_TEXT) {
		err = tadstack_currentlangcode(stack, &langcode);
		if (err < 0) {
			printf("tadstack_currentlangcode error: %d\n", err);
			ok = False;
		}
		cmp = tadlangcodecmpTC(expected->langcode, expected->langlen, &langcode);
		if (cmp == False) {
			printf("tadstack_currentlangcode: Fail\n");
			ok = False;
		}
	}
	err = tadstack_currentview(stack, &r);
	if (err < 0) {
		printf("tadstack_currentview error: %d\n", err);
		ok = False;
	}
	if (memcmp(&expected->view, &r, sizeof(RECT)) != 0) {
		printf("tadstack_currentview: Fail\n");
		ok = False;
	}
	err = tadstack_currentdraw(stack, &r);
	if (err < 0) {
		printf("tadstack_currentdraw error: %d\n", err);
		ok = False;
	}
	if (memcmp(&expected->draw, &r, sizeof(RECT)) != 0) {
		printf("tadstack_currentdraw: Fail\n");
		ok = False;
	}
	err = tadstack_currenthunit(stack, &units);
	if (err < 0) {
		printf("tadstack_currenthunit error: %d\n", err);
		ok = False;
	}
	if (expected->hunit != units) {
		printf("tadstack_currenthunit Fail: expected = %04x, result = %04x\n", expected->hunit, units);
		ok = False;
	}
	err = tadstack_currentvunit(stack, &units);
	if (err < 0) {
		printf("tadstack_currentvunit error: %d\n", err);
		ok = False;
	}
	if (expected->vunit != units) {
		printf("tadstack_currentvunit Fail: expected = %04x, result = %04x\n", expected->vunit, units);
		ok = False;
	}

	return ok;
}

LOCAL UNITTEST_RESULT test_tadstack_4()
{
	tadstack_t stack;
	tadsegment segment;
	TADSTACK_RESULT stk_result;
	test_tadstack_validate_status_t expected;
	UB textsegraw[] = {
		0xe1, 0xff, 0x18, 0x00, 0x00, 0x00, 0x00, 0x00,
		0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
		0x00, 0x00, 0x00, 0x00, 0x88, 0xff, 0x88, 0xff,
		0x21, 0x00, 0x00, 0x00
	};
	Bool cmp;
	UNITTEST_RESULT ok = UNITTEST_RESULT_PASS;

	tadstack_initialize(&stack);

	segment.type = TADSEGMENT_TYPE_VARIABLE;
	segment.value.variable.raw = textsegraw;
	segment.value.variable.rawlen = sizeof(textsegraw);

	stk_result = tadstack_inputsegment(&stack, &segment);
	if (stk_result != TADSTACK_RESULT_PUSH_STACK) {
		ok = UNITTEST_RESULT_FAIL;
	}

	expected.nestlevel = 0;
	expected.type = TADSTACK_DATATYPE_TEXT;
	expected.langcode = (TC[]){0xFE21};
	expected.langlen = 1;
	expected.view = (RECT){{0,0,0,0}};
	expected.draw = (RECT){{0,0,0,0}};
	expected.hunit = 0xff88;
	expected.vunit = 0xff88;
	cmp = test_tadstack_validate_status(&stack, &expected);
	if (cmp == False) {
		ok = UNITTEST_RESULT_FAIL;
	}

	tadstack_finalize(&stack);

	return ok;
}

LOCAL UNITTEST_RESULT test_tadstack_5()
{
	tadstack_t stack;
	tadsegment segment;
	TADSTACK_RESULT stk_result;
	test_tadstack_validate_status_t expected;
	UB figsegraw[] = {
		0xe3, 0xff, 0x18, 0x00, 0x00, 0x00, 0x00, 0x00,
		0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
		0x00, 0x00, 0x00, 0x00, 0x88, 0xff, 0x88, 0xff,
		0x00, 0x00, 0x00, 0x00
	};
	Bool cmp;
	UNITTEST_RESULT ok = UNITTEST_RESULT_PASS;

	tadstack_initialize(&stack);

	segment.type = TADSEGMENT_TYPE_VARIABLE;
	segment.value.variable.raw = figsegraw;
	segment.value.variable.rawlen = sizeof(figsegraw);

	stk_result = tadstack_inputsegment(&stack, &segment);
	if (stk_result != TADSTACK_RESULT_PUSH_STACK) {
		ok = UNITTEST_RESULT_FAIL;
	}

	expected.nestlevel = 0;
	expected.type = TADSTACK_DATATYPE_FIG;
	expected.langcode = NULL;
	expected.langlen = 0;
	expected.view = (RECT){{0,0,0,0}};
	expected.draw = (RECT){{0,0,0,0}};
	expected.hunit = 0xff88;
	expected.vunit = 0xff88;
	cmp = test_tadstack_validate_status(&stack, &expected);
	if (cmp == False) {
		ok = UNITTEST_RESULT_FAIL;
	}

	tadstack_finalize(&stack);

	return ok;
}

LOCAL UNITTEST_RESULT test_tadstack_6()
{
	tadstack_t stack;
	tadsegment segment;
	TADSTACK_RESULT stk_result;
	test_tadstack_validate_status_t expected;
	Bool cmp;
	UNITTEST_RESULT ok = UNITTEST_RESULT_PASS;

	tadstack_initialize(&stack);

	segment.type = TADSEGMENT_TYPE_CHARACTOR;
	segment.value.ch = 0x2422;

	stk_result = tadstack_inputsegment(&stack, &segment);
	if (stk_result != TADSTACK_RESULT_PUSH_STACK) {
		ok = UNITTEST_RESULT_FAIL;
	}

	expected.nestlevel = 0;
	expected.type = TADSTACK_DATATYPE_TEXT;
	expected.langcode = (TC[]){0xFE21};
	expected.langlen = 1;
	expected.view = (RECT){{0,0,0,0}};
	expected.draw = (RECT){{0,0,0,0}};
	expected.hunit = 0;
	expected.vunit = 0;
	cmp = test_tadstack_validate_status(&stack, &expected);
	if (cmp == False) {
		ok = UNITTEST_RESULT_FAIL;
	}

	tadstack_finalize(&stack);

	return ok;
}

LOCAL UNITTEST_RESULT test_tadstack_7()
{
	tadstack_t stack;
	tadsegment segment;
	TADSTACK_RESULT stk_result;
	test_tadstack_validate_status_t expected;
	UB textsegraw[] = {
		0xa2, 0xff, 0x06, 0x00, 0x00, 0x03, 0x01, 0x01, 0x01, 0x01
	};
	Bool cmp;
	UNITTEST_RESULT ok = UNITTEST_RESULT_PASS;

	tadstack_initialize(&stack);

	segment.type = TADSEGMENT_TYPE_VARIABLE;
	segment.value.variable.raw = textsegraw;
	segment.value.variable.rawlen = sizeof(textsegraw);

	stk_result = tadstack_inputsegment(&stack, &segment);
	if (stk_result != TADSTACK_RESULT_PUSH_STACK) {
		ok = UNITTEST_RESULT_FAIL;
	}

	expected.nestlevel = 0;
	expected.type = TADSTACK_DATATYPE_TEXT;
	expected.langcode = (TC[]){0xFE21};
	expected.langlen = 1;
	expected.view = (RECT){{0,0,0,0}};
	expected.draw = (RECT){{0,0,0,0}};
	expected.hunit = 0;
	expected.vunit = 0;
	cmp = test_tadstack_validate_status(&stack, &expected);
	if (cmp == False) {
		ok = UNITTEST_RESULT_FAIL;
	}

	tadstack_finalize(&stack);

	return ok;
}

EXPORT VOID test_tadstack_main(unittest_driver_t *driver)
{
	UNITTEST_DRIVER_REGIST(driver, test_tadstack_1);
	UNITTEST_DRIVER_REGIST(driver, test_tadstack_2);
	UNITTEST_DRIVER_REGIST(driver, test_tadstack_3);
	UNITTEST_DRIVER_REGIST(driver, test_tadstack_4);
	UNITTEST_DRIVER_REGIST(driver, test_tadstack_5);
	UNITTEST_DRIVER_REGIST(driver, test_tadstack_6);
	UNITTEST_DRIVER_REGIST(driver, test_tadstack_7);
}

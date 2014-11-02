/*
 * test_texteditor_characterstate.c
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

#include "texteditor_characterstate.h"

#include	<basic.h>
#include	<bstdio.h>
#include	<bstdlib.h>
#include	<bstring.h>
#include	<tstring.h>

#include    <unittest_driver.h>

typedef struct {
	tadsegment input;
	struct {
		TC *lang;
		W lang_len;
		Bool is_hankaku;
	} expected;
} test_texteditor_characterstate_t;

LOCAL UNITTEST_RESULT test_texteditor_characterstate_common(test_texteditor_characterstate_t *testdata, W len)
{
	texteditor_characterstate_t state;
	W i, err;
	Bool is_hankaku, ok;
	TC *lang, lang_len;
	UNITTEST_RESULT result = UNITTEST_RESULT_PASS;

	texteditor_characterstate_initialize(&state);

	for (i = 0; i < len; i++) {
		err = texteditor_charactorstate_input(&state, &(testdata[i].input));
		if (err < 0) {
			printf("[%d] texteditor_charactorstate_input error\n", i);
			result = UNITTEST_RESULT_FAIL;
			break;
		}
		is_hankaku = texteditor_characterstate_ishankaku(&state);
		if (is_hankaku != False) {
			if (testdata[i].expected.is_hankaku == False) {
				printf("[%d] ishankaku error\n", i);
				result = UNITTEST_RESULT_FAIL;
			}
		} else {
			if (testdata[i].expected.is_hankaku != False) {
				printf("[%d] ishankaku error\n", i);
				result = UNITTEST_RESULT_FAIL;
			}
		}

		ok = texteditor_characterstate_islang(&state, testdata[i].expected.lang, testdata[i].expected.lang_len);
		if (ok == False) {
			printf("[%d] islang error\n", i);
			result = UNITTEST_RESULT_FAIL;
		}

		lang_len = texteditor_characterstate_getlang(&state, NULL, -1);
		if (lang_len != testdata[i].expected.lang_len) {
			printf("[%d] getlang length 1 error. expected = %d, result = %d\n", i, testdata[i].expected.lang_len, lang_len);
			result = UNITTEST_RESULT_FAIL;
		}

		lang = malloc(sizeof(TC)*testdata[i].expected.lang_len);
		lang_len = texteditor_characterstate_getlang(&state, lang, testdata[i].expected.lang_len);
		if (lang_len != testdata[i].expected.lang_len) {
			printf("[%d] getlang length 2 error. expected = %d, result = %d\n", i, testdata[i].expected.lang_len, lang_len);
			result = UNITTEST_RESULT_FAIL;
		} else {
			if (tc_strncmp(lang, testdata[i].expected.lang, lang_len) != 0) {
				printf("[%d] getlang code error\n", i);
				result = UNITTEST_RESULT_FAIL;
			}
		}
		free(lang);
	}

	texteditor_characterstate_finalize(&state);

	return result;
}

LOCAL UNITTEST_RESULT test_texteditor_characterstate_1()
{
	test_texteditor_characterstate_t testdata[] = {
		{ {TADSEGMENT_TYPE_LANGCODE}, {(TC[]){0xFE21}, 1, False} },
		{ {TADSEGMENT_TYPE_LANGCODE}, {(TC[]){0xFE22}, 1, False} },
		{ {TADSEGMENT_TYPE_LANGCODE}, {(TC[]){0xFEFE, 0xFE21}, 2, False} },
		{ {TADSEGMENT_TYPE_LANGCODE}, {(TC[]){0xFEFE, 0x0021}, 2, False} },
		{ {TADSEGMENT_TYPE_LANGCODE}, {(TC[]){0xFEFE, 0xFEFE, 0xFE21}, 3, False} },
		{ {TADSEGMENT_TYPE_LANGCODE}, {(TC[]){0xFEFE, 0xFEFE, 0x0021}, 3, False} },
	};
	TCtotadlangcode((TC[]){0xFE21}, 1, &testdata[0].input.value.lang);
	TCtotadlangcode((TC[]){0xFE22}, 1, &testdata[1].input.value.lang);
	TCtotadlangcode((TC[]){0xFEFE, 0xFE21}, 2, &testdata[2].input.value.lang);
	TCtotadlangcode((TC[]){0xFEFE, 0x0021}, 2, &testdata[3].input.value.lang);
	TCtotadlangcode((TC[]){0xFEFE, 0xFEFE, 0xFE21}, 3, &testdata[4].input.value.lang);
	TCtotadlangcode((TC[]){0xFEFE, 0xFEFE, 0x0021}, 3, &testdata[5].input.value.lang);
	return test_texteditor_characterstate_common(testdata, 6);
}

LOCAL UNITTEST_RESULT test_texteditor_characterstate_2()
{
	test_texteditor_characterstate_t testdata[] = {
		{ {TADSEGMENT_TYPE_CHARACTOR}, {(TC[]){0xFE21}, 1, False} },
		{ {TADSEGMENT_TYPE_LANGCODE}, {(TC[]){0xFE22}, 1, False} },
		{ {TADSEGMENT_TYPE_CHARACTOR}, {(TC[]){0xFE22}, 1, False} },
		{ {TADSEGMENT_TYPE_LANGCODE}, {(TC[]){0xFE21}, 1, False} },
		{ {TADSEGMENT_TYPE_CHARACTOR}, {(TC[]){0xFE21}, 1, False} },
	};
	testdata[0].input.value.ch = 0x2121;
	TCtotadlangcode((TC[]){0xFE22}, 1, &testdata[1].input.value.lang);
	testdata[2].input.value.ch = 0x2121;
	TCtotadlangcode((TC[]){0xFE21}, 1, &testdata[3].input.value.lang);
	testdata[4].input.value.ch = 0x2121;
	return test_texteditor_characterstate_common(testdata, 5);
}

LOCAL UNITTEST_RESULT test_texteditor_characterstate_3()
{
	test_texteditor_characterstate_t testdata[] = {
		{ {TADSEGMENT_TYPE_VARIABLE}, {(TC[]){0xFE21}, 1, False} },
		{ {TADSEGMENT_TYPE_VARIABLE}, {(TC[]){0xFE21}, 1, True} },
		{ {TADSEGMENT_TYPE_VARIABLE}, {(TC[]){0xFE21}, 1, False} },
		{ {TADSEGMENT_TYPE_VARIABLE}, {(TC[]){0xFE21}, 1, True} },
		{ {TADSEGMENT_TYPE_VARIABLE}, {(TC[]){0xFE21}, 1, False} },
	};
	testdata[0].input.value.variable.raw = (UB[]){0xA2, 0xFF, 0x06, 0x00, 0x00, 0x03, 0x00, 0x00, 0x00, 0x00};
	testdata[0].input.value.variable.rawlen = 10;
	testdata[1].input.value.variable.raw = (UB[]){0xA2, 0xFF, 0x06, 0x00, 0x00, 0x03, 0x00, 0x00, 0x02, 0x01};
	testdata[1].input.value.variable.rawlen = 10;
	testdata[2].input.value.variable.raw = (UB[]){0xA2, 0xFF, 0x06, 0x00, 0x00, 0x03, 0x00, 0x00, 0x00, 0x00};
	testdata[2].input.value.variable.rawlen = 10;
	testdata[3].input.value.variable.raw = (UB[]){0xA2, 0xFF, 0x06, 0x00, 0x00, 0x03, 0x00, 0x00, 0x02, 0x01};
	testdata[3].input.value.variable.rawlen = 10;
	testdata[4].input.value.variable.raw = (UB[]){0xA2, 0xFF, 0x06, 0x00, 0x00, 0x03, 0x00, 0x00, 0x01, 0x01};
	testdata[4].input.value.variable.rawlen = 10;
	return test_texteditor_characterstate_common(testdata, 5);
}


LOCAL UNITTEST_RESULT test_texteditor_characterstate_4()
{
	test_texteditor_characterstate_t testdata[] = {
		{ {TADSEGMENT_TYPE_VARIABLE}, {(TC[]){0xFE21}, 1, True} },
		{ {TADSEGMENT_TYPE_CHARACTOR}, {(TC[]){0xFE21}, 1, True} },
		{ {TADSEGMENT_TYPE_LANGCODE}, {(TC[]){0xFE22}, 1, True} },
		{ {TADSEGMENT_TYPE_CHARACTOR}, {(TC[]){0xFE22}, 1, True} },
		{ {TADSEGMENT_TYPE_VARIABLE}, {(TC[]){0xFE22}, 1, False} },
		{ {TADSEGMENT_TYPE_CHARACTOR}, {(TC[]){0xFE22}, 1, False} },
		{ {TADSEGMENT_TYPE_LANGCODE}, {(TC[]){0xFE21}, 1, False} },
		{ {TADSEGMENT_TYPE_CHARACTOR}, {(TC[]){0xFE21}, 1, False} },
	};
	testdata[0].input.value.variable.raw = (UB[]){0xA2, 0xFF, 0x06, 0x00, 0x00, 0x03, 0x00, 0x00, 0x02, 0x01};
	testdata[0].input.value.variable.rawlen = 10;
	testdata[1].input.value.ch = 0x2121;
	TCtotadlangcode((TC[]){0xFE22}, 1, &testdata[2].input.value.lang);
	testdata[3].input.value.ch = 0x2121;
	testdata[4].input.value.variable.raw = (UB[]){0xA2, 0xFF, 0x06, 0x00, 0x00, 0x03, 0x00, 0x00, 0x00, 0x00};
	testdata[4].input.value.variable.rawlen = 10;
	testdata[5].input.value.ch = 0x2121;
	TCtotadlangcode((TC[]){0xFE21}, 1, &testdata[6].input.value.lang);
	testdata[7].input.value.ch = 0x2121;
	return test_texteditor_characterstate_common(testdata, 8);
}

EXPORT VOID test_texteditor_characterstate_main(unittest_driver_t *driver)
{
	UNITTEST_DRIVER_REGIST(driver, test_texteditor_characterstate_1);
	UNITTEST_DRIVER_REGIST(driver, test_texteditor_characterstate_2);
	UNITTEST_DRIVER_REGIST(driver, test_texteditor_characterstate_3);
	UNITTEST_DRIVER_REGIST(driver, test_texteditor_characterstate_4);
}

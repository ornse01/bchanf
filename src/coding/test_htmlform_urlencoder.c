/*
 * test_htmlform_urlencoder.c
 *
 * Copyright (c) 2015 project bchan
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

#include    "test_coding.h"

#include    "htmlform_urlencoder.h"

#include    <btron/btron.h>
#include	<bstdio.h>
#include	<bstring.h>
#include	<bstdlib.h>

#include    <unittest_driver.h>

typedef struct urlencoder_testdata_t_ {
	htmlform_field *src;
	W src_len;
	UB *expected;
	W expected_len;
} urlencoder_testdata_t;

LOCAL UNITTEST_RESULT test_htmlform_urlencoder_common(urlencoder_testdata_t *testdata)
{
	htmlform_urlencoder_t encoder;
	Bool cont;
	UB *str, *result;
	W len, result_len;
	UNITTEST_RESULT ret = UNITTEST_RESULT_PASS;

	result = (UB*)malloc(sizeof(UB));
	if (result == NULL) {
		return UNITTEST_RESULT_FAIL;
	}
	result[0] = '\0';
	result_len = 0;

	htmlform_urlencoder_initialize(&encoder, testdata->src, testdata->src_len);

	for (;;) {
		cont = htmlform_urlencoder_next(&encoder, &str, &len);
		if (cont == False) {
			break;
		}
		if (len == 0) {
			continue;
		}
		result = realloc(result, result_len + len + 1);
		if (result == NULL) {
			ret = UNITTEST_RESULT_FAIL;
			break;
		}
		strncat(result, str, len);
		result_len += len;
	}

	htmlform_urlencoder_finalize(&encoder);

	if (result != NULL) {
		if (testdata->expected_len != result_len) {
			printf("length failure: expected = %d, result = %d\n", testdata->expected_len, result_len);
			ret = UNITTEST_RESULT_FAIL;
		}
		if (strncmp(testdata->expected, result, result_len) != 0) {
			printf("encoded string failure: %s\n", result);
			ret = UNITTEST_RESULT_FAIL;
		}

		free(result);
	}

	return ret;
}

LOCAL UNITTEST_RESULT test_htmlform_urlencoder_1()
{
	htmlform_field src[] = {
		(htmlform_field){"aaa", 3, "bbb", 3},
		(htmlform_field){"cc", 2, "dd", 2},
	};
	W src_len = 2;
	UB expected[] = "aaa=bbb&cc=dd";
	W expected_len = strlen(expected);
	urlencoder_testdata_t testdata = {
		src, src_len, expected, expected_len
	};
	return test_htmlform_urlencoder_common(&testdata);
}

LOCAL UNITTEST_RESULT test_htmlform_urlencoder_2()
{
	htmlform_field src[] = {
		(htmlform_field){"aaa", 3, "bb", 2},
		(htmlform_field){"cc", 2, "ddd", 3},
	};
	W src_len = 2;
	UB expected[] = "aaa=bb&cc=ddd";
	W expected_len = strlen(expected);
	urlencoder_testdata_t testdata = {
		src, src_len, expected, expected_len
	};
	return test_htmlform_urlencoder_common(&testdata);
}

LOCAL UNITTEST_RESULT test_htmlform_urlencoder_3()
{
	htmlform_field src[] = {
		(htmlform_field){"a a", 3, "b b", 3},
	};
	W src_len = 1;
	UB expected[] = "a+a=b+b";
	W expected_len = strlen(expected);
	urlencoder_testdata_t testdata = {
		src, src_len, expected, expected_len
	};
	return test_htmlform_urlencoder_common(&testdata);
}

LOCAL UNITTEST_RESULT test_htmlform_urlencoder_4()
{
	htmlform_field src[] = {
		(htmlform_field){(UB[]){0xA0, 0xA1, 0xA2}, 3, (UB[]){0xB3, 0xB4, 0xB5}, 3},
	};
	W src_len = 1;
	UB expected[] = "%A0%A1%A2=%B3%B4%B5";
	W expected_len = strlen(expected);
	urlencoder_testdata_t testdata = {
		src, src_len, expected, expected_len
	};
	return test_htmlform_urlencoder_common(&testdata);
}

LOCAL UNITTEST_RESULT test_htmlform_urlencoder_5()
{
	htmlform_field src[] = {
		(htmlform_field){"aaa", 3, "", 0},
		(htmlform_field){"cc", 2, "", 0},
	};
	W src_len = 2;
	UB expected[] = "aaa=&cc=";
	W expected_len = strlen(expected);
	urlencoder_testdata_t testdata = {
		src, src_len, expected, expected_len
	};
	return test_htmlform_urlencoder_common(&testdata);
}

EXPORT VOID test_htmlform_urlencoder_main(unittest_driver_t *driver)
{
	UNITTEST_DRIVER_REGIST(driver, test_htmlform_urlencoder_1);
	UNITTEST_DRIVER_REGIST(driver, test_htmlform_urlencoder_2);
	UNITTEST_DRIVER_REGIST(driver, test_htmlform_urlencoder_3);
	UNITTEST_DRIVER_REGIST(driver, test_htmlform_urlencoder_4);
	UNITTEST_DRIVER_REGIST(driver, test_htmlform_urlencoder_5);
}

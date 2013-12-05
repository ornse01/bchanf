/*
 * test_tadlangcode.c
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

#include "test_tad.h"

#include "tadlangcode.h"

#include	<basic.h>
#include	<bstdio.h>
#include	<bstdlib.h>
#include	<bstring.h>
#include	<tstring.h>

#include    <unittest_driver.h>

LOCAL UNITTEST_RESULT test_TCtotadlangcode_common(TC *testdata, W testdata_len, tadlangcode *expected)
{
	W err;
	Bool cmp;
	tadlangcode result;

	err = TCtotadlangcode(testdata, testdata_len, &result);
	if (err < 0) {
		return UNITTEST_RESULT_FAIL;
	}
	cmp = tadlangcodecmp(&result, expected);
	if (cmp == False) {
		return UNITTEST_RESULT_FAIL;
	}
	return UNITTEST_RESULT_PASS;
}

LOCAL UNITTEST_RESULT test_TCtotadlangcode_1()
{
	TC testdata[] = {0xFE21};
	W testdata_len = 1;
	tadlangcode expected = {0, 0x21};
	return test_TCtotadlangcode_common(testdata, testdata_len, &expected);
}

LOCAL UNITTEST_RESULT test_TCtotadlangcode_2()
{
	TC testdata[] = {0xFE22};
	W testdata_len = 1;
	tadlangcode expected = {0, 0x22};
	return test_TCtotadlangcode_common(testdata, testdata_len, &expected);
}

LOCAL UNITTEST_RESULT test_TCtotadlangcode_3()
{
	TC testdata[] = {0xFEFE, 0xFE21};
	W testdata_len = 2;
	tadlangcode expected = {2, 0x21};
	return test_TCtotadlangcode_common(testdata, testdata_len, &expected);
}

LOCAL UNITTEST_RESULT test_TCtotadlangcode_4()
{
	TC testdata[] = {0xFEFE, 0x0021};
	W testdata_len = 2;
	tadlangcode expected = {1, 0x21};
	return test_TCtotadlangcode_common(testdata, testdata_len, &expected);
}

LOCAL UNITTEST_RESULT test_TCtotadlangcode_5()
{
	TC testdata[] = {0xFEFE, 0xFEFE, 0xFE21};
	W testdata_len = 3;
	tadlangcode expected = {4, 0x21};
	return test_TCtotadlangcode_common(testdata, testdata_len, &expected);
}

LOCAL UNITTEST_RESULT test_TCtotadlangcode_6()
{
	TC testdata[] = {0xFEFE, 0xFEFE, 0x0021};
	W testdata_len = 3;
	tadlangcode expected = {3, 0x21};
	return test_TCtotadlangcode_common(testdata, testdata_len, &expected);
}

LOCAL UNITTEST_RESULT test_tadlangcodetoTC_common(tadlangcode *testdata, TC *expected, W expected_len)
{
	W len;
	TC *str;

	len = tadlangcodetoTC(testdata, NULL, 1);
	if (len != expected_len) {
		printf("tadlangcodetoTC length check 1 error\n");
		return UNITTEST_RESULT_FAIL;
	}

	str = malloc(sizeof(TC)*len);
	if (str == NULL) {
		return UNITTEST_RESULT_FAIL;
	}

	len = tadlangcodetoTC(testdata, str, -1);
	if (len != expected_len) {
		free(str);
		printf("tadlangcodetoTC length check 2 error\n");
		return UNITTEST_RESULT_FAIL;
	}

	len = tadlangcodetoTC(testdata, str, len);
	if (len != expected_len) {
		free(str);
		printf("tadlangcodetoTC converted length error\n");
		return UNITTEST_RESULT_FAIL;
	}
	if (tc_strncmp(str, expected, len) != 0) {
		free(str);
		printf("tadlangcodetoTC convarted string error\n");
		return UNITTEST_RESULT_FAIL;
	}

	free(str);

	return UNITTEST_RESULT_PASS;
}

LOCAL UNITTEST_RESULT test_tadlangcodetoTC_1()
{
	tadlangcode testdata = {0, 0x21};
	TC expected[] = {0xFE21};
	W expected_len = 1;
	return test_tadlangcodetoTC_common(&testdata, expected, expected_len);
}

LOCAL UNITTEST_RESULT test_tadlangcodetoTC_2()
{
	tadlangcode testdata = {0, 0x22};
	TC expected[] = {0xFE22};
	W expected_len = 1;
	return test_tadlangcodetoTC_common(&testdata, expected, expected_len);
}

LOCAL UNITTEST_RESULT test_tadlangcodetoTC_3()
{
	tadlangcode testdata = {2, 0x21};
	TC expected[] = {0xFEFE, 0xFE21};
	W expected_len = 2;
	return test_tadlangcodetoTC_common(&testdata, expected, expected_len);
}

LOCAL UNITTEST_RESULT test_tadlangcodetoTC_4()
{
	tadlangcode testdata = {1, 0x21};
	TC expected[] = {0xFEFE, 0x0021};
	W expected_len = 2;
	return test_tadlangcodetoTC_common(&testdata, expected, expected_len);
}

LOCAL UNITTEST_RESULT test_tadlangcodetoTC_5()
{
	tadlangcode testdata = {4, 0x21};
	TC expected[] = {0xFEFE, 0xFEFE, 0xFE21};
	W expected_len = 3;
	return test_tadlangcodetoTC_common(&testdata, expected, expected_len);
}

LOCAL UNITTEST_RESULT test_tadlangcodetoTC_6()
{
	tadlangcode testdata = {3, 0x21};
	TC expected[] = {0xFEFE, 0xFEFE, 0x0021};
	W expected_len = 3;
	return test_tadlangcodetoTC_common(&testdata, expected, expected_len);
}

LOCAL UNITTEST_RESULT test_tadlangcodecmpTC_common(tadlangcode *testdata1, TC *testdata2, W testdata2_len, Bool expected)
{
	Bool ret;
	UNITTEST_RESULT result = UNITTEST_RESULT_PASS;

	ret = tadlangcodecmpTC(testdata2, testdata2_len, testdata1);
	if (ret == False) {
		if (expected != False) {
			result = UNITTEST_RESULT_FAIL;
		}
	} else {
		if (expected == False) {
			result = UNITTEST_RESULT_FAIL;
		}
	}

	return result;
}

LOCAL UNITTEST_RESULT test_tadlangcodecmpTC_1()
{
	tadlangcode testdata1 = {0, 0x21};
	TC testdata2[] = {0xFE21};
	W testdata2_len = 1;
	Bool expected = True;
	return test_tadlangcodecmpTC_common(&testdata1, testdata2, testdata2_len, expected);
}

LOCAL UNITTEST_RESULT test_tadlangcodecmpTC_2()
{
	tadlangcode testdata1 = {0, 0x22};
	TC testdata2[] = {0xFE22};
	W testdata2_len = 1;
	Bool expected = True;
	return test_tadlangcodecmpTC_common(&testdata1, testdata2, testdata2_len, expected);
}

LOCAL UNITTEST_RESULT test_tadlangcodecmpTC_3()
{
	tadlangcode testdata1 = {2, 0x21};
	TC testdata2[] = {0xFEFE, 0xFE21};
	W testdata2_len = 2;
	Bool expected = True;
	return test_tadlangcodecmpTC_common(&testdata1, testdata2, testdata2_len, expected);
}

LOCAL UNITTEST_RESULT test_tadlangcodecmpTC_4()
{
	tadlangcode testdata1 = {1, 0x21};
	TC testdata2[] = {0xFEFE, 0x0021};
	W testdata2_len = 2;
	Bool expected = True;
	return test_tadlangcodecmpTC_common(&testdata1, testdata2, testdata2_len, expected);
}

LOCAL UNITTEST_RESULT test_tadlangcodecmpTC_5()
{
	tadlangcode testdata1 = {4, 0x21};
	TC testdata2[] = {0xFEFE, 0xFEFE, 0xFE21};
	W testdata2_len = 3;
	Bool expected = True;
	return test_tadlangcodecmpTC_common(&testdata1, testdata2, testdata2_len, expected);
}

LOCAL UNITTEST_RESULT test_tadlangcodecmpTC_6()
{
	tadlangcode testdata1 = {3, 0x21};
	TC testdata2[] = {0xFEFE, 0xFEFE, 0x0021};
	W testdata2_len = 3;
	Bool expected = True;
	return test_tadlangcodecmpTC_common(&testdata1, testdata2, testdata2_len, expected);
}

LOCAL UNITTEST_RESULT test_tadlangcodecmpTC_7()
{
	tadlangcode testdata1 = {0, 0x21};
	TC testdata2[] = {0xFE22};
	W testdata2_len = 1;
	Bool expected = False;
	return test_tadlangcodecmpTC_common(&testdata1, testdata2, testdata2_len, expected);
}

LOCAL UNITTEST_RESULT test_tadlangcodecmpTC_8()
{
	tadlangcode testdata1 = {2, 0x21};
	TC testdata2[] = {0xFEFE, 0xFE22};
	W testdata2_len = 2;
	Bool expected = False;
	return test_tadlangcodecmpTC_common(&testdata1, testdata2, testdata2_len, expected);
}

LOCAL UNITTEST_RESULT test_tadlangcodecmpTC_9()
{
	tadlangcode testdata1 = {4, 0x21};
	TC testdata2[] = {0xFEFE, 0xFEFE, 0xFE22};
	W testdata2_len = 3;
	Bool expected = False;
	return test_tadlangcodecmpTC_common(&testdata1, testdata2, testdata2_len, expected);
}

LOCAL UNITTEST_RESULT test_tadlangcodecmp_common(tadlangcode *testdata1, tadlangcode *testdata2, Bool expected)
{
	Bool ret;
	UNITTEST_RESULT result = UNITTEST_RESULT_PASS;

	ret = tadlangcodecmp(testdata1, testdata2);
	if (ret == False) {
		if (expected != False) {
			result = UNITTEST_RESULT_FAIL;
		}
	} else {
		if (expected == False) {
			result = UNITTEST_RESULT_FAIL;
		}
	}

	return result;
}

LOCAL UNITTEST_RESULT test_tadlangcodecmp_1()
{
	tadlangcode testdata1 = {0, 0x21};
	tadlangcode testdata2 = {0, 0x21};
	Bool expected = True;
	return test_tadlangcodecmp_common(&testdata1, &testdata2, expected);
}


LOCAL UNITTEST_RESULT test_tadlangcodecmp_2()
{
	tadlangcode testdata1 = {0, 0x21};
	tadlangcode testdata2 = {0, 0x22};
	Bool expected = False;
	return test_tadlangcodecmp_common(&testdata1, &testdata2, expected);
}

LOCAL UNITTEST_RESULT test_tadlangcodecmp_3()
{
	tadlangcode testdata1 = {0, 0x21};
	tadlangcode testdata2 = {1, 0x21};
	Bool expected = False;
	return test_tadlangcodecmp_common(&testdata1, &testdata2, expected);
}

LOCAL UNITTEST_RESULT test_tadlangcodecmp_4()
{
	tadlangcode testdata1 = {1, 0x21};
	tadlangcode testdata2 = {1, 0x21};
	Bool expected = True;
	return test_tadlangcodecmp_common(&testdata1, &testdata2, expected);
}

LOCAL UNITTEST_RESULT test_tadlangcodecmp_5()
{
	tadlangcode testdata1 = {1, 0x21};
	tadlangcode testdata2 = {1, 0x22};
	Bool expected = False;
	return test_tadlangcodecmp_common(&testdata1, &testdata2, expected);
}

LOCAL UNITTEST_RESULT test_tadlangcodecmp_6()
{
	tadlangcode testdata1 = {1, 0x21};
	tadlangcode testdata2 = {2, 0x21};
	Bool expected = False;
	return test_tadlangcodecmp_common(&testdata1, &testdata2, expected);
}

EXPORT VOID test_tadlangcode_main(unittest_driver_t *driver)
{
	UNITTEST_DRIVER_REGIST(driver, test_TCtotadlangcode_1);
	UNITTEST_DRIVER_REGIST(driver, test_TCtotadlangcode_2);
	UNITTEST_DRIVER_REGIST(driver, test_TCtotadlangcode_3);
	UNITTEST_DRIVER_REGIST(driver, test_TCtotadlangcode_4);
	UNITTEST_DRIVER_REGIST(driver, test_TCtotadlangcode_5);
	UNITTEST_DRIVER_REGIST(driver, test_TCtotadlangcode_6);
	UNITTEST_DRIVER_REGIST(driver, test_tadlangcodetoTC_1);
	UNITTEST_DRIVER_REGIST(driver, test_tadlangcodetoTC_2);
	UNITTEST_DRIVER_REGIST(driver, test_tadlangcodetoTC_3);
	UNITTEST_DRIVER_REGIST(driver, test_tadlangcodetoTC_4);
	UNITTEST_DRIVER_REGIST(driver, test_tadlangcodetoTC_5);
	UNITTEST_DRIVER_REGIST(driver, test_tadlangcodetoTC_6);
	UNITTEST_DRIVER_REGIST(driver, test_tadlangcodecmpTC_1);
	UNITTEST_DRIVER_REGIST(driver, test_tadlangcodecmpTC_2);
	UNITTEST_DRIVER_REGIST(driver, test_tadlangcodecmpTC_3);
	UNITTEST_DRIVER_REGIST(driver, test_tadlangcodecmpTC_4);
	UNITTEST_DRIVER_REGIST(driver, test_tadlangcodecmpTC_5);
	UNITTEST_DRIVER_REGIST(driver, test_tadlangcodecmpTC_6);
	UNITTEST_DRIVER_REGIST(driver, test_tadlangcodecmpTC_7);
	UNITTEST_DRIVER_REGIST(driver, test_tadlangcodecmpTC_8);
	UNITTEST_DRIVER_REGIST(driver, test_tadlangcodecmpTC_9);
	UNITTEST_DRIVER_REGIST(driver, test_tadlangcodecmp_1);
	UNITTEST_DRIVER_REGIST(driver, test_tadlangcodecmp_2);
	UNITTEST_DRIVER_REGIST(driver, test_tadlangcodecmp_3);
	UNITTEST_DRIVER_REGIST(driver, test_tadlangcodecmp_4);
	UNITTEST_DRIVER_REGIST(driver, test_tadlangcodecmp_5);
	UNITTEST_DRIVER_REGIST(driver, test_tadlangcodecmp_6);
}

/*
 * test_http_transferdecoder.c
 *
 * Copyright (c) 2012 project bchan
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

#include "test_http.h"

#include "http_transferdecoder.h"

#include	<basic.h>
#include	<bstdio.h>
#include	<bstdlib.h>
#include	<bstring.h>

#include    <unittest_driver.h>

#include    "http_typedef.h"

LOCAL UB testdata_identity_01[] = {
	"abcfefghijklmnopqrstuvwxyz"
	"ABCDEFGHIJKLMNOPQRSTUVWXYZ"
};

LOCAL UB testdata_chunked_01[] = {
	"1A\r\n"
	"abcfefghijklmnopqrstuvwxyz\r\n"
	"1a\r\n"
	"ABCDEFGHIJKLMNOPQRSTUVWXYZ\r\n"
	"000\r\n"
	"\r\n"
};

LOCAL UB testdata_chunked_02[] = {
	"1a;name=value\r\n"
	"abcfefghijklmnopqrstuvwxyz\r\n"
	"1A;name\r\n"
	"ABCDEFGHIJKLMNOPQRSTUVWXYZ\r\n"
	"0;name=value\r\n"
	"\r\n"
};

LOCAL UB testdata_chunked_03[] = {
	"1A\r\n"
	"abcfefghijklmnopqrstuvwxyz\r\n"
	"1a\r\n"
	"ABCDEFGHIJKLMNOPQRSTUVWXYZ\r\n"
	"0\r\n"
	"Header0: value\r\n"
	"\r\n"
};

LOCAL UB testdata_chunked_04[] = {
	"1A\r\n"
	"abcfefghijklmnopqrstuvwxyz\r\n"
	"1a\r\n"
	"ABCDEFGHIJKLMNOPQRSTUVWXYZ\r\n"
	"0\r\n"
	"Header0: value\r\n"
	"Header1: value\r\n"
	"Header2: value\r\n"
	"\r\n"
};

LOCAL UB testdata_chunked_05[] = {
	"1A\r\n"
	"abcfefghijklmnopqrstuvwxyz\r\n"
	"1a\r\n"
	"ABCDEFGHIJKLMNOPQRSTUVWXYZ\r\n"
	"0;name=value\r\n"
	"Header0: value\r\n"
	"\r\n"
};

LOCAL UB testdata_chunked_06[] = {
	"1A\r\n"
	"abcfefghijklmnopqrstuvwxyz\r\n"
	"1a\r\n"
	"ABCDEFGHIJKLMNOPQRSTUVWXYZ\r\n"
	"0;name=value\r\n"
	"Header0: value\r\n"
	"Header1: value\r\n"
	"Header2: value\r\n"
	"\r\n"
};

LOCAL UB testdata_chunked_07[] = {
	"0;name=value\r\n"
	"\r\n"
};

LOCAL UB testdata_chunked_08[] = {
	"0\r\n"
	"\r\n"
};

LOCAL UNITTEST_RESULT test_http_transferdecoder_common_perone(HTTP_TRANSFERCODING_TYPE type, UB *testdata, W testdata_len, W devide_len, UB *expected, W expected_len)
{
	http_transferdecoder_t decoder;
	http_transferdecoder_result *ret;
	UNITTEST_RESULT result = UNITTEST_RESULT_PASS;
	W i, j, read, len, ret_len, str_len = 0, cmp;
	UB *str = NULL;
	Bool end = False;

	http_transferdecoder_initialize(&decoder, type, testdata_len);

	for (i = 0; i < testdata_len;) {
		if (testdata_len - i > devide_len) {
			len = devide_len;
		} else {
			len = testdata_len - i;
		}
		read = http_transferdecoder_inputentitybody(&decoder, testdata+i, len, &ret, &ret_len);
		if (read < 0) {
			printf("http_transferdecoder_inputentitybody return value\n");
			result = UNITTEST_RESULT_FAIL;
			break;
		}
		i += read;
		for (j = 0; j < ret_len; j++) {
			if (ret[j].type == HTTP_TRANSFERDECODER_RESULTTYPE_DATA) {
				str = realloc(str, str_len+ret[j].len);
				if (str == NULL) {
					printf("realloc error\n");
					end = True;
					break;
				}
				memcpy(str + str_len, ret[j].data, ret[j].len);
				str_len += ret[j].len;
			} else if (ret[j].type == HTTP_TRANSFERDECODER_RESULTTYPE_END) {
				end = True;
				break;
			} else {
				printf("http_transferdecoder_inputentitybody type error\n");
				end = True;
				result = UNITTEST_RESULT_FAIL;
				break;
			}
		}
		if (end == True) {
			break;
		}
	}

	http_transferdecoder_finalize(&decoder);

	if (end == False) {
		printf("not ended by HTTP_TRANSFERDECODER_RESULTTYPE_END\n");
		result = UNITTEST_RESULT_FAIL;
	}

	if (str == NULL) {
		if (expected_len != 0) {
			printf("expected length fail (result buffer empty): expected = %d, result = %d\n", expected_len, result);
			result = UNITTEST_RESULT_FAIL;
		}
	} else {
		if (str_len == expected_len) {
			cmp = memcmp(str, expected, expected_len);
			if (cmp != 0) {
				printf("expected data fail\n");
				result = UNITTEST_RESULT_FAIL;
			}
		} else {
			printf("expected length fail: expected = %d, result = %d\n", expected_len, result);
			result = UNITTEST_RESULT_FAIL;
		}
	}

	return result;
}

LOCAL UNITTEST_RESULT test_http_transferdecoder_common(HTTP_TRANSFERCODING_TYPE type, UB *testdata, W testdata_len, UB *expected, W expected_len)
{
	UNITTEST_RESULT result = UNITTEST_RESULT_PASS;
	W devide;

	result = test_http_transferdecoder_common_perone(type, testdata, testdata_len, 1, expected, expected_len);
	if (result != UNITTEST_RESULT_PASS) {
		printf("devided length = 1 fail\n");
		return result;
	}

	for (devide = 5; devide < testdata_len ; devide += 5) {
		result = test_http_transferdecoder_common_perone(type, testdata, testdata_len, devide, expected, expected_len);
		if (result != UNITTEST_RESULT_PASS) {
			printf("devided length = %d fail\n", devide);
			return result;
		}
	}

	result = test_http_transferdecoder_common_perone(type, testdata, testdata_len, testdata_len, expected, expected_len);
	if (result != UNITTEST_RESULT_PASS) {
		printf("devided length = testdata length fail\n");
		return result;
	}

	return UNITTEST_RESULT_PASS;
}

LOCAL UNITTEST_RESULT test_http_transferdecoder_identity_1()
{
	UB *testdata = testdata_identity_01;
	W testdata_len = strlen(testdata_identity_01);
	UB *expected = testdata_identity_01;
	W expected_len = strlen(testdata_identity_01);
	return test_http_transferdecoder_common(HTTP_TRANSFERCODING_TYPE_IDENTITY, testdata, testdata_len, expected, expected_len);
}

/* Transfer-Encoding: chunked */

LOCAL UNITTEST_RESULT test_http_transferdecoder_chunked_1()
{
	UB *testdata = testdata_chunked_01;
	W testdata_len = strlen(testdata_chunked_01);
	UB *expected = testdata_identity_01;
	W expected_len = strlen(testdata_identity_01);
	return test_http_transferdecoder_common(HTTP_TRANSFERCODING_TYPE_CHUNKED, testdata, testdata_len, expected, expected_len);
}

LOCAL UNITTEST_RESULT test_http_transferdecoder_chunked_2()
{
	UB *testdata = testdata_chunked_02;
	W testdata_len = strlen(testdata_chunked_02);
	UB *expected = testdata_identity_01;
	W expected_len = strlen(testdata_identity_01);
	return test_http_transferdecoder_common(HTTP_TRANSFERCODING_TYPE_CHUNKED, testdata, testdata_len, expected, expected_len);
}

LOCAL UNITTEST_RESULT test_http_transferdecoder_chunked_3()
{
	UB *testdata = testdata_chunked_03;
	W testdata_len = strlen(testdata_chunked_03);
	UB *expected = testdata_identity_01;
	W expected_len = strlen(testdata_identity_01);
	return test_http_transferdecoder_common(HTTP_TRANSFERCODING_TYPE_CHUNKED, testdata, testdata_len, expected, expected_len);
}

LOCAL UNITTEST_RESULT test_http_transferdecoder_chunked_4()
{
	UB *testdata = testdata_chunked_04;
	W testdata_len = strlen(testdata_chunked_04);
	UB *expected = testdata_identity_01;
	W expected_len = strlen(testdata_identity_01);
	return test_http_transferdecoder_common(HTTP_TRANSFERCODING_TYPE_CHUNKED, testdata, testdata_len, expected, expected_len);
}

LOCAL UNITTEST_RESULT test_http_transferdecoder_chunked_5()
{
	UB *testdata = testdata_chunked_05;
	W testdata_len = strlen(testdata_chunked_05);
	UB *expected = testdata_identity_01;
	W expected_len = strlen(testdata_identity_01);
	return test_http_transferdecoder_common(HTTP_TRANSFERCODING_TYPE_CHUNKED, testdata, testdata_len, expected, expected_len);
}

LOCAL UNITTEST_RESULT test_http_transferdecoder_chunked_6()
{
	UB *testdata = testdata_chunked_06;
	W testdata_len = strlen(testdata_chunked_06);
	UB *expected = testdata_identity_01;
	W expected_len = strlen(testdata_identity_01);
	return test_http_transferdecoder_common(HTTP_TRANSFERCODING_TYPE_CHUNKED, testdata, testdata_len, expected, expected_len);
}

LOCAL UNITTEST_RESULT test_http_transferdecoder_chunked_7()
{
	UB *testdata = testdata_chunked_07;
	W testdata_len = strlen(testdata_chunked_07);
	UB *expected = NULL;
	W expected_len = 0;
	return test_http_transferdecoder_common(HTTP_TRANSFERCODING_TYPE_CHUNKED, testdata, testdata_len, expected, expected_len);
}

LOCAL UNITTEST_RESULT test_http_transferdecoder_chunked_8()
{
	UB *testdata = testdata_chunked_08;
	W testdata_len = strlen(testdata_chunked_08);
	UB *expected = NULL;
	W expected_len = 0;
	return test_http_transferdecoder_common(HTTP_TRANSFERCODING_TYPE_CHUNKED, testdata, testdata_len, expected, expected_len);
}

EXPORT VOID test_http_transferdecoder_main(unittest_driver_t *driver)
{
	UNITTEST_DRIVER_REGIST(driver, test_http_transferdecoder_identity_1);
	UNITTEST_DRIVER_REGIST(driver, test_http_transferdecoder_chunked_1);
	UNITTEST_DRIVER_REGIST(driver, test_http_transferdecoder_chunked_2);
	UNITTEST_DRIVER_REGIST(driver, test_http_transferdecoder_chunked_3);
	UNITTEST_DRIVER_REGIST(driver, test_http_transferdecoder_chunked_4);
	UNITTEST_DRIVER_REGIST(driver, test_http_transferdecoder_chunked_5);
	UNITTEST_DRIVER_REGIST(driver, test_http_transferdecoder_chunked_6);
	UNITTEST_DRIVER_REGIST(driver, test_http_transferdecoder_chunked_7);
	UNITTEST_DRIVER_REGIST(driver, test_http_transferdecoder_chunked_8);
}

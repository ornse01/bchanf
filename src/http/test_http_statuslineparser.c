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

#include "http_statuslineparser.h"

#include	<basic.h>
#include	<bstdio.h>
#include	<bstdlib.h>
#include	<bstring.h>

#include    <unittest_driver.h>

#include    "http_typedef.h"

LOCAL UNITTEST_RESULT test_http_statuslineparser_common(UB *testdata, W testdata_len, http_statuslineparser_result *expected)
{
	http_statuslineparser_t parser;
	http_statuslineparser_result result;
	UNITTEST_RESULT ret = UNITTEST_RESULT_PASS;
	Bool cont;
	W i;

	http_statuslineparser_initialize(&parser);

	for (i = 0; i < testdata_len - 1; i++) {
		cont = http_statuslineparser_inputchar(&parser, testdata[i], &result);
		if (cont != False) {
			ret = UNITTEST_RESULT_FAIL;
			printf("parser ended at not status line end\n");
		}
	}
	cont = http_statuslineparser_inputchar(&parser, testdata[i], &result);
	if (cont == False) {
		ret = UNITTEST_RESULT_FAIL;
		printf("parser not ended at status line end\n");
	}
	if (result.error != expected->error) {
		ret = UNITTEST_RESULT_FAIL;
		printf("error value is not expected\n");
	}
	if (result.version != expected->version) {
		ret = UNITTEST_RESULT_FAIL;
		printf("version value is not expected\n");
	}
	if (result.statuscode != expected->statuscode) {
		ret = UNITTEST_RESULT_FAIL;
		printf("statuscode value is not expected\n");
	}

	http_statuslineparser_finalize(&parser);

	return ret;
}

LOCAL UNITTEST_RESULT test_http_statuslineparser_1()
{
	UB testdata[] = "HTTP/1.1 200 OK\r\n";
	W testdata_len = strlen(testdata);
	http_statuslineparser_result expected = {
		HTTP_STATUSLINEPARSER_ERROR_NONE,
		HTTP_HTTPVERSION_1_1,
		HTTP_STATUSCODE_200_OK,
	};
	return test_http_statuslineparser_common(testdata, testdata_len, &expected);
}

LOCAL UNITTEST_RESULT test_http_statuslineparser_2()
{
	UB testdata[] = "HTTP/0.9 200 OK\r\n";
	W testdata_len = strlen(testdata);
	http_statuslineparser_result expected = {
		HTTP_STATUSLINEPARSER_ERROR_NONE,
		HTTP_HTTPVERSION_0_9,
		HTTP_STATUSCODE_200_OK,
	};
	return test_http_statuslineparser_common(testdata, testdata_len, &expected);
}

LOCAL UNITTEST_RESULT test_http_statuslineparser_3()
{
	UB testdata[] = "HTTP/1.0 200 OK\r\n";
	W testdata_len = strlen(testdata);
	http_statuslineparser_result expected = {
		HTTP_STATUSLINEPARSER_ERROR_NONE,
		HTTP_HTTPVERSION_1_0,
		HTTP_STATUSCODE_200_OK,
	};
	return test_http_statuslineparser_common(testdata, testdata_len, &expected);
}

LOCAL UNITTEST_RESULT test_http_statuslineparser_4()
{
	UB testdata[] = "HTTP/1.1 206 Partial Content\r\n";
	W testdata_len = strlen(testdata);
	http_statuslineparser_result expected = {
		HTTP_STATUSLINEPARSER_ERROR_NONE,
		HTTP_HTTPVERSION_1_1,
		HTTP_STATUSCODE_206_Partial_Content,
	};
	return test_http_statuslineparser_common(testdata, testdata_len, &expected);
}

LOCAL UNITTEST_RESULT test_http_statuslineparser_5()
{
	UB testdata[] = "HTTP/1.1 304 Not Modified\r\n";
	W testdata_len = strlen(testdata);
	http_statuslineparser_result expected = {
		HTTP_STATUSLINEPARSER_ERROR_NONE,
		HTTP_HTTPVERSION_1_1,
		HTTP_STATUSCODE_304_Not_Modified,
	};
	return test_http_statuslineparser_common(testdata, testdata_len, &expected);
}

LOCAL UNITTEST_RESULT test_http_statuslineparser_6()
{
	UB testdata[] = "HTTP/1.1 203 Non-Authoritative Information\r\n";
	W testdata_len = strlen(testdata);
	http_statuslineparser_result expected = {
		HTTP_STATUSLINEPARSER_ERROR_NONE,
		HTTP_HTTPVERSION_1_1,
		HTTP_STATUSCODE_203_Non_Authoritative_Information,
	};
	return test_http_statuslineparser_common(testdata, testdata_len, &expected);
}

LOCAL UNITTEST_RESULT test_http_statuslineparser_7()
{
	UB testdata[] = "HTTP/1.1 404 Not Found\r\n";
	W testdata_len = strlen(testdata);
	http_statuslineparser_result expected = {
		HTTP_STATUSLINEPARSER_ERROR_NONE,
		HTTP_HTTPVERSION_1_1,
		HTTP_STATUSCODE_404_Not_Found,
	};
	return test_http_statuslineparser_common(testdata, testdata_len, &expected);
}

LOCAL UNITTEST_RESULT test_http_statuslineparser_8()
{
	UB testdata[] = "HTTP/1.1 416 Requested range not satisfiable\r\n";
	W testdata_len = strlen(testdata);
	http_statuslineparser_result expected = {
		HTTP_STATUSLINEPARSER_ERROR_NONE,
		HTTP_HTTPVERSION_1_1,
		HTTP_STATUSCODE_416_Requested_range_not_satisfiable,
	};
	return test_http_statuslineparser_common(testdata, testdata_len, &expected);
}

LOCAL UNITTEST_RESULT test_http_statuslineparser_9()
{
	UB testdata[] = "HTTP/1.1 505 HTTP Version not supported\r\n";
	W testdata_len = strlen(testdata);
	http_statuslineparser_result expected = {
		HTTP_STATUSLINEPARSER_ERROR_NONE,
		HTTP_HTTPVERSION_1_1,
		HTTP_STATUSCODE_505_HTTP_Version_not_supported,
	};
	return test_http_statuslineparser_common(testdata, testdata_len, &expected);
}

LOCAL UNITTEST_RESULT test_http_statuslineparser_10()
{
	UB testdata[] = "HTTP/1.1 510 Not Extended\r\n";
	W testdata_len = strlen(testdata);
	http_statuslineparser_result expected = {
		HTTP_STATUSLINEPARSER_ERROR_UNSUPPORTED_STATUS_CODE,
		HTTP_HTTPVERSION_1_1,
		-1,
	};
	return test_http_statuslineparser_common(testdata, testdata_len, &expected);
}

EXPORT VOID test_http_statuslineparser_main(unittest_driver_t *driver)
{
	UNITTEST_DRIVER_REGIST(driver, test_http_statuslineparser_1);
	UNITTEST_DRIVER_REGIST(driver, test_http_statuslineparser_2);
	UNITTEST_DRIVER_REGIST(driver, test_http_statuslineparser_3);
	UNITTEST_DRIVER_REGIST(driver, test_http_statuslineparser_4);
	UNITTEST_DRIVER_REGIST(driver, test_http_statuslineparser_5);
	UNITTEST_DRIVER_REGIST(driver, test_http_statuslineparser_6);
	UNITTEST_DRIVER_REGIST(driver, test_http_statuslineparser_7);
	UNITTEST_DRIVER_REGIST(driver, test_http_statuslineparser_8);
	UNITTEST_DRIVER_REGIST(driver, test_http_statuslineparser_9);
	UNITTEST_DRIVER_REGIST(driver, test_http_statuslineparser_10);
}

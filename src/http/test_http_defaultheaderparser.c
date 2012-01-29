/*
 * test_http_defaultheaderparser.c
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

#include "http_defaultheaderparser.h"

#include	<basic.h>
#include	<bstdio.h>
#include	<bstdlib.h>
#include	<bstring.h>

#include    <unittest_driver.h>

#include    "http_typedef.h"

LOCAL UNITTEST_RESULT test_http_defaultheaderparser_common(UB *testdata, W testdata_len, W expected_cl, HTTP_TRANSFERCODING_TYPE expected_tc, HTTP_CONTENTCODING_VALUE expected_cc, Bool expected_connectionclose)
{
	http_defaultheaderparser_t parser;
	HTTP_DEFAULTHEADERPARSER_RESULT result;
	W i;
	UNITTEST_RESULT ret = UNITTEST_RESULT_PASS;
	W cl;
	HTTP_TRANSFERCODING_TYPE tc;
	HTTP_CONTENTCODING_VALUE cc;
	Bool connectionclosed;

	http_defaultheaderparser_initialize(&parser);

	for (i = 0; i < testdata_len; i++) {
		http_defaultheaderparser_inputchar(&parser, testdata[i], &result);
		if (result == HTTP_DEFAULTHEADERPARSER_RESULT_MESSAGE_HEADER_END) {
			i++;
			break;
		}
	}
	if (i != testdata_len) {
		printf("end before header end\n");
		ret = UNITTEST_RESULT_FAIL;
	}
	if (result != HTTP_DEFAULTHEADERPARSER_RESULT_MESSAGE_HEADER_END) {
		printf("not return MESSAGE_HEADER_END\n");
		ret = UNITTEST_RESULT_FAIL;
	}

	cl = http_defaultheaderparsr_contentlength(&parser);
	if (expected_cl != cl) {
		printf("Content-Length is not expected. expected = %d, result = %d\n", expected_cl, cl);
		ret = UNITTEST_RESULT_FAIL;
	}
	tc = http_defaultheaderparsr_transferencoding(&parser);
	if (expected_tc != tc) {
		printf("Transfer-Encoding is not expected. expected = %d, result = %d\n", expected_tc, tc);
		ret = UNITTEST_RESULT_FAIL;
	}
	cc = http_defaultheaderparsr_contentencoding(&parser);
	if (expected_cc != cc) {
		printf("Content-Encoding is not expected. expected = %d, result = %d\n", expected_cc, cc);
		ret = UNITTEST_RESULT_FAIL;
	}
	connectionclosed = http_defaultheaderparser_connectionclosed(&parser);
	if (expected_connectionclose != connectionclosed) {
		printf("Connection is not expected. expected = %d, result = %d\n", expected_connectionclose, connectionclosed);
		ret = UNITTEST_RESULT_FAIL;
	}

	http_defaultheaderparser_finalize(&parser);

	return ret;
}

LOCAL UB testdata_header_01[] = {
	"Date: Wed, 10 Dec 2008 21:33:30 GMT\r\n"
	"Server: Apache/2.2.9\r\n"
	"Vary: User-Agent,Accept-Encoding\r\n"
	"Last-Modified: Wed, 24 Dec 2008 21:14:12 GMT\r\n"
	"ETag: \"1a2dd38-446e-45ed15f9ad100\"-gzip\r\n"
	"Accept-Ranges: bytes\r\n"
	"Content-Encoding: gzip\r\n"
	"Content-Length: 3511\r\n"
	"Connection: close\r\n"
	"Content-Type: text/plain; charset=Shift_JIS\r\n"
	"\r\n"
};

LOCAL UNITTEST_RESULT test_http_defaultheaderparser_1()
{
	UB *testdata = testdata_header_01;
	W testdata_len = strlen(testdata_header_01);
	W expected_contentlength = 3511;
	HTTP_TRANSFERCODING_TYPE expected_transfercoding = HTTP_TRANSFERCODING_TYPE_IDENTITY;
	HTTP_CONTENTCODING_VALUE expected_contentcoding = HTTP_CONTENTCODING_VALUE_GZIP;
	Bool expected_connectionclose = True;
	return test_http_defaultheaderparser_common(testdata, testdata_len, expected_contentlength, expected_transfercoding, expected_contentcoding, expected_connectionclose);
}

LOCAL UB testdata_header_02[] = {
	"Date: Wed, 10 Dec 2008 21:40:53 GMT\r\n"
	"Server: Apache/2.2.9\r\n"
	"Vary: User-Agent,Accept-Encoding\r\n"
	"Last-Modified: Wed, 24 Dec 2008 21:39:58 GMT\r\n"
	"ETag: \"1a2dd38-44a7-45ed1bbc0e780\"\r\n"
	"Accept-Ranges: bytes\r\n"
	"Content-Range: bytes 17517-17574/17575\r\n"
	"Content-Length: 58\r\n"
	"Connection: close\r\n"
	"Content-Type: text/plain; charset=Shift_JIS\r\n"
	"\r\n"
};

LOCAL UNITTEST_RESULT test_http_defaultheaderparser_2()
{
	UB *testdata = testdata_header_02;
	W testdata_len = strlen(testdata_header_02);
	W expected_contentlength = 58;
	HTTP_TRANSFERCODING_TYPE expected_transfercoding = HTTP_TRANSFERCODING_TYPE_IDENTITY;
	HTTP_CONTENTCODING_VALUE expected_contentcoding = HTTP_CONTENTCODING_VALUE_IDENTITY;
	Bool expected_connectionclose = True;
	return test_http_defaultheaderparser_common(testdata, testdata_len, expected_contentlength, expected_transfercoding, expected_contentcoding, expected_connectionclose);
}

LOCAL UB testdata_header_03[] = {
	"Date: Wed, 10 Dec 2008 21:33:30 GMT\r\n"
	"Server: Apache/2.2.9\r\n"
	"Vary: User-Agent,Accept-Encoding\r\n"
	"Last-Modified: Wed, 24 Dec 2008 21:14:12 GMT\r\n"
	"ETag: \"1a2dd38-446e-45ed15f9ad100\"-gzip\r\n"
	"Accept-Ranges: bytes\r\n"
	"Content-Encoding: compress\r\n"
	"Content-Length: 0\r\n"
	"Content-Type: text/plain; charset=Shift_JIS\r\n"
	"\r\n"
};

LOCAL UNITTEST_RESULT test_http_defaultheaderparser_3()
{
	UB *testdata = testdata_header_03;
	W testdata_len = strlen(testdata_header_03);
	W expected_contentlength = 0;
	HTTP_TRANSFERCODING_TYPE expected_transfercoding = HTTP_TRANSFERCODING_TYPE_IDENTITY;
	HTTP_CONTENTCODING_VALUE expected_contentcoding = HTTP_CONTENTCODING_VALUE_COMPRESS;
	Bool expected_connectionclose = False;
	return test_http_defaultheaderparser_common(testdata, testdata_len, expected_contentlength, expected_transfercoding, expected_contentcoding, expected_connectionclose);
}

LOCAL UB testdata_header_04[] = {
	"Date: Wed, 10 Dec 2008 21:33:30 GMT\r\n"
	"Server: Apache/2.2.9\r\n"
	"Vary: User-Agent,Accept-Encoding\r\n"
	"Last-Modified: Wed, 24 Dec 2008 21:14:12 GMT\r\n"
	"ETag: \"1a2dd38-446e-45ed15f9ad100\"-gzip\r\n"
	"Accept-Ranges: bytes\r\n"
	"Content-Encoding: deflate\r\n"
	"Content-Length: 12340\r\n"
	"Content-Type: text/plain; charset=Shift_JIS\r\n"
	"\r\n"
};

LOCAL UNITTEST_RESULT test_http_defaultheaderparser_4()
{
	UB *testdata = testdata_header_04;
	W testdata_len = strlen(testdata_header_04);
	W expected_contentlength = 12340;
	HTTP_TRANSFERCODING_TYPE expected_transfercoding = HTTP_TRANSFERCODING_TYPE_IDENTITY;
	HTTP_CONTENTCODING_VALUE expected_contentcoding = HTTP_CONTENTCODING_VALUE_DEFLATE;
	Bool expected_connectionclose = False;
	return test_http_defaultheaderparser_common(testdata, testdata_len, expected_contentlength, expected_transfercoding, expected_contentcoding, expected_connectionclose);
}

LOCAL UB testdata_header_05[] = {
	"Date: Wed, 10 Dec 2008 21:33:30 GMT\r\n"
	"Server: Apache/2.2.9\r\n"
	"Vary: User-Agent,Accept-Encoding\r\n"
	"Last-Modified: Wed, 24 Dec 2008 21:14:12 GMT\r\n"
	"ETag: \"1a2dd38-446e-45ed15f9ad100\"-gzip\r\n"
	"Accept-Ranges: bytes\r\n"
	"Transfer-Encoding: chunked\r\n"
	"Connection: close"
	"Content-Type: text/plain; charset=Shift_JIS\r\n"
	"\r\n"
};

LOCAL UNITTEST_RESULT test_http_defaultheaderparser_5()
{
	UB *testdata = testdata_header_05;
	W testdata_len = strlen(testdata_header_05);
	W expected_contentlength = 0;
	HTTP_TRANSFERCODING_TYPE expected_transfercoding = HTTP_TRANSFERCODING_TYPE_CHUNKED;
	HTTP_CONTENTCODING_VALUE expected_contentcoding = HTTP_CONTENTCODING_VALUE_IDENTITY;
	Bool expected_connectionclose = False;
	return test_http_defaultheaderparser_common(testdata, testdata_len, expected_contentlength, expected_transfercoding, expected_contentcoding, expected_connectionclose);
}

EXPORT VOID test_http_defaultheaderparser_main(unittest_driver_t *driver)
{
	UNITTEST_DRIVER_REGIST(driver, test_http_defaultheaderparser_1);
	UNITTEST_DRIVER_REGIST(driver, test_http_defaultheaderparser_2);
	UNITTEST_DRIVER_REGIST(driver, test_http_defaultheaderparser_3);
	UNITTEST_DRIVER_REGIST(driver, test_http_defaultheaderparser_4);
	UNITTEST_DRIVER_REGIST(driver, test_http_defaultheaderparser_5);
}

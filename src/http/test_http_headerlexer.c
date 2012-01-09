/*
 * test_http_headerlexer.c
 *
 * Copyright (c) 2011-2012 project bchan
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

#include "http_headerlexer.h"

#include    <btron/btron.h>
#include    <bstdio.h>
#include    <tcode.h>

#include    <unittest_driver.h>

LOCAL UB test_http_headerlexer_testdata_01[] = {
"ABCDEF: afgce\r
BCDERF:   aaaa bbb \r
XYZ:  qwer\r
   AAA\r
LMNOPQ: aaa\r
\r
AAABBBCCC
DDDEEEFFF"
};

LOCAL UNITTEST_RESULT test_http_headerlexer_1()
{
	http_headerlexer_t lexer;
	W i,err,len;
	HTTP_HEADERLEXER_RESULT hr;

	len = strlen(test_http_headerlexer_testdata_01);

	err = http_headerlexer_initialize(&lexer);
	if (err < 0) {
		return UNITTEST_RESULT_FAIL;
	}

	for (i = 0; i < len; i++) {
		http_headerlexer_inputchar(&lexer, test_http_headerlexer_testdata_01[i], &hr);
		switch (hr) {
		case HTTP_HEADERLEXER_RESULT_NONE:
			printf("NONE\n");
			break;
		case HTTP_HEADERLEXER_RESULT_FIELDNAME:
			printf("FIELDNAME\n");
			break;
		case HTTP_HEADERLEXER_RESULT_FIELDNAME_END:
			printf("FIELDNAME_END\n");
			break;
		case HTTP_HEADERLEXER_RESULT_FIELDCONTENT:
			printf("FIELDCONTENT\n");
			break;
		case HTTP_HEADERLEXER_RESULT_LWS:
			printf("LWS\n");
			break;
		case HTTP_HEADERLEXER_RESULT_MESSAGEHEADER_END:
			printf("MESSAGEHEADER_END\n");
			break;
		}
	}

	http_headerlexer_finalize(&lexer);

	return UNITTEST_RESULT_PASS;
}

LOCAL UNITTEST_RESULT test_http_headerlexer_2()
{
	http_headerlexer_t lexer;
	W err;
	HTTP_HEADERLEXER_RESULT hr;
	UNITTEST_RESULT result = UNITTEST_RESULT_PASS;

#define check_result_2(info, e, r) \
	if (e != r) { \
		result = UNITTEST_RESULT_FAIL; \
		printf("test expected = %d, result = %d, fail = %s", e, r, info); \
	}

	err = http_headerlexer_initialize(&lexer);
	if (err < 0) {
		return UNITTEST_RESULT_FAIL;
	}

	http_headerlexer_inputchar(&lexer, 'A', &hr);
	check_result_2("A", HTTP_HEADERLEXER_RESULT_FIELDNAME, hr);
	http_headerlexer_inputchar(&lexer, 'B', &hr);
	check_result_2("B", HTTP_HEADERLEXER_RESULT_FIELDNAME, hr);
	http_headerlexer_inputchar(&lexer, 'C', &hr);
	check_result_2("C", HTTP_HEADERLEXER_RESULT_FIELDNAME, hr);
	http_headerlexer_inputchar(&lexer, ':', &hr);
	check_result_2(":", HTTP_HEADERLEXER_RESULT_FIELDNAME_END, hr);
	http_headerlexer_inputchar(&lexer, ' ', &hr);
	check_result_2("[SP]", HTTP_HEADERLEXER_RESULT_LWS, hr);
	http_headerlexer_inputchar(&lexer, 'a', &hr);
	check_result_2("a", HTTP_HEADERLEXER_RESULT_FIELDCONTENT, hr);
	http_headerlexer_inputchar(&lexer, 'b', &hr);
	check_result_2("b", HTTP_HEADERLEXER_RESULT_FIELDCONTENT, hr);
	http_headerlexer_inputchar(&lexer, 'c', &hr);
	check_result_2("c", HTTP_HEADERLEXER_RESULT_FIELDCONTENT, hr);
	http_headerlexer_inputchar(&lexer, '\r', &hr);
	check_result_2("[CR]", HTTP_HEADERLEXER_RESULT_NONE, hr);
	http_headerlexer_inputchar(&lexer, '\n', &hr);
	check_result_2("[LF]", HTTP_HEADERLEXER_RESULT_NONE, hr);
	http_headerlexer_inputchar(&lexer, 'D', &hr);
	check_result_2("D", HTTP_HEADERLEXER_RESULT_FIELDNAME, hr);
	http_headerlexer_inputchar(&lexer, 'E', &hr);
	check_result_2("E", HTTP_HEADERLEXER_RESULT_FIELDNAME, hr);
	http_headerlexer_inputchar(&lexer, 'F', &hr);
	check_result_2("F", HTTP_HEADERLEXER_RESULT_FIELDNAME, hr);
	http_headerlexer_inputchar(&lexer, ':', &hr);
	check_result_2(":", HTTP_HEADERLEXER_RESULT_FIELDNAME_END, hr);
	http_headerlexer_inputchar(&lexer, ' ', &hr);
	check_result_2("[SP]", HTTP_HEADERLEXER_RESULT_LWS, hr);
	http_headerlexer_inputchar(&lexer, ' ', &hr);
	check_result_2("[SP]", HTTP_HEADERLEXER_RESULT_NONE, hr);
	http_headerlexer_inputchar(&lexer, ' ', &hr);
	check_result_2("[SP]", HTTP_HEADERLEXER_RESULT_NONE, hr);
	http_headerlexer_inputchar(&lexer, 'a', &hr);
	check_result_2("a", HTTP_HEADERLEXER_RESULT_FIELDCONTENT, hr);
	http_headerlexer_inputchar(&lexer, ' ', &hr);
	check_result_2("[SP]", HTTP_HEADERLEXER_RESULT_LWS, hr);
	http_headerlexer_inputchar(&lexer, 'b', &hr);
	check_result_2("b", HTTP_HEADERLEXER_RESULT_FIELDCONTENT, hr);
	http_headerlexer_inputchar(&lexer, 'b', &hr);
	check_result_2("b", HTTP_HEADERLEXER_RESULT_FIELDCONTENT, hr);
	http_headerlexer_inputchar(&lexer, ' ', &hr);
	check_result_2("[SP]", HTTP_HEADERLEXER_RESULT_LWS, hr);
	http_headerlexer_inputchar(&lexer, '\r', &hr);
	check_result_2("[CR]", HTTP_HEADERLEXER_RESULT_NONE, hr);
	http_headerlexer_inputchar(&lexer, '\n', &hr);
	check_result_2("[LF]", HTTP_HEADERLEXER_RESULT_NONE, hr);
	http_headerlexer_inputchar(&lexer, 'X', &hr);
	check_result_2("X", HTTP_HEADERLEXER_RESULT_FIELDNAME, hr);
	http_headerlexer_inputchar(&lexer, 'Y', &hr);
	check_result_2("Y", HTTP_HEADERLEXER_RESULT_FIELDNAME, hr);
	http_headerlexer_inputchar(&lexer, 'Z', &hr);
	check_result_2("Z", HTTP_HEADERLEXER_RESULT_FIELDNAME, hr);
	http_headerlexer_inputchar(&lexer, ':', &hr);
	check_result_2(":", HTTP_HEADERLEXER_RESULT_FIELDNAME_END, hr);
	http_headerlexer_inputchar(&lexer, 'x', &hr);
	check_result_2("x", HTTP_HEADERLEXER_RESULT_FIELDCONTENT, hr);
	http_headerlexer_inputchar(&lexer, 'y', &hr);
	check_result_2("y", HTTP_HEADERLEXER_RESULT_FIELDCONTENT, hr);
	http_headerlexer_inputchar(&lexer, 'z', &hr);
	check_result_2("z", HTTP_HEADERLEXER_RESULT_FIELDCONTENT, hr);
	http_headerlexer_inputchar(&lexer, '\r', &hr);
	check_result_2("[CR]", HTTP_HEADERLEXER_RESULT_NONE, hr);
	http_headerlexer_inputchar(&lexer, '\n', &hr);
	check_result_2("[LF]", HTTP_HEADERLEXER_RESULT_NONE, hr);
	http_headerlexer_inputchar(&lexer, ' ', &hr);
	check_result_2("[SP]", HTTP_HEADERLEXER_RESULT_LWS, hr);
	http_headerlexer_inputchar(&lexer, ' ', &hr);
	check_result_2("[SP]", HTTP_HEADERLEXER_RESULT_NONE, hr);
	http_headerlexer_inputchar(&lexer, 'A', &hr);
	check_result_2("A", HTTP_HEADERLEXER_RESULT_FIELDCONTENT, hr);
	http_headerlexer_inputchar(&lexer, 'A', &hr);
	check_result_2("A", HTTP_HEADERLEXER_RESULT_FIELDCONTENT, hr);
	http_headerlexer_inputchar(&lexer, 'A', &hr);
	check_result_2("A", HTTP_HEADERLEXER_RESULT_FIELDCONTENT, hr);
	http_headerlexer_inputchar(&lexer, '\r', &hr);
	check_result_2("[CR]", HTTP_HEADERLEXER_RESULT_NONE, hr);
	http_headerlexer_inputchar(&lexer, '\n', &hr);
	check_result_2("[LF]", HTTP_HEADERLEXER_RESULT_NONE, hr);
	http_headerlexer_inputchar(&lexer, 'L', &hr);
	check_result_2("L", HTTP_HEADERLEXER_RESULT_FIELDNAME, hr);
	http_headerlexer_inputchar(&lexer, 'M', &hr);
	check_result_2("M", HTTP_HEADERLEXER_RESULT_FIELDNAME, hr);
	http_headerlexer_inputchar(&lexer, 'N', &hr);
	check_result_2("N", HTTP_HEADERLEXER_RESULT_FIELDNAME, hr);
	http_headerlexer_inputchar(&lexer, ':', &hr);
	check_result_2(":", HTTP_HEADERLEXER_RESULT_FIELDNAME_END, hr);
	http_headerlexer_inputchar(&lexer, ' ', &hr);
	check_result_2("[SP]", HTTP_HEADERLEXER_RESULT_LWS, hr);
	http_headerlexer_inputchar(&lexer, 'l', &hr);
	check_result_2("l", HTTP_HEADERLEXER_RESULT_FIELDCONTENT, hr);
	http_headerlexer_inputchar(&lexer, 'm', &hr);
	check_result_2("m", HTTP_HEADERLEXER_RESULT_FIELDCONTENT, hr);
	http_headerlexer_inputchar(&lexer, 'n', &hr);
	check_result_2("n", HTTP_HEADERLEXER_RESULT_FIELDCONTENT, hr);
	http_headerlexer_inputchar(&lexer, '\r', &hr);
	check_result_2("[CR]", HTTP_HEADERLEXER_RESULT_NONE, hr);
	http_headerlexer_inputchar(&lexer, '\n', &hr);
	check_result_2("[LF]", HTTP_HEADERLEXER_RESULT_NONE, hr);
	http_headerlexer_inputchar(&lexer, '\r', &hr);
	check_result_2("[CR]", HTTP_HEADERLEXER_RESULT_NONE, hr);
	http_headerlexer_inputchar(&lexer, '\n', &hr);
	check_result_2("[LF]", HTTP_HEADERLEXER_RESULT_MESSAGEHEADER_END, hr);

	http_headerlexer_finalize(&lexer);

	return result;
}

EXPORT VOID test_httpheaderlexer_main(unittest_driver_t *driver)
{
	UNITTEST_DRIVER_REGIST(driver, test_http_headerlexer_1);
	UNITTEST_DRIVER_REGIST(driver, test_http_headerlexer_2);
}

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
	W i,j,err,len,hr_len;
	http_headerlexer_result *hr;

	len = strlen(test_http_headerlexer_testdata_01);

	err = http_headerlexer_initialize(&lexer);
	if (err < 0) {
		return UNITTEST_RESULT_FAIL;
	}

	for (i = 0; i < len; i++) {
		http_headerlexer_inputchar(&lexer, test_http_headerlexer_testdata_01[i], &hr, &hr_len);
		for (j = 0; j < hr_len; j++) {
			switch (hr[j].type) {
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
			case HTTP_HEADERLEXER_RESULT_FIELDVALUE_END:
				printf("FIELDVALUE_END\n");
				break;
			case HTTP_HEADERLEXER_RESULT_MESSAGEHEADER_END:
				printf("MESSAGEHEADER_END\n");
				break;
			}
		}
	}

	http_headerlexer_finalize(&lexer);

	return UNITTEST_RESULT_PASS;
}

typedef struct {
	W type;
	UB ch;
} test_http_headerlexer_expected_t;

LOCAL UNITTEST_RESULT test_http_headerlexer_common(UB *testdata, W testdata_len, test_http_headerlexer_expected_t *expected, W expected_len)
{
	http_headerlexer_t lexer;
	W err, hr_len, i, j, k = 0;
	UB ch;
	http_headerlexer_result *hr;
	UNITTEST_RESULT result = UNITTEST_RESULT_PASS;

	err = http_headerlexer_initialize(&lexer);
	if (err < 0) {
		return UNITTEST_RESULT_FAIL;
	}

	for (i = 0; i < testdata_len ; i++) {
		ch = testdata[i];
		http_headerlexer_inputchar(&lexer, ch, &hr, &hr_len);
		for (j = 0; j < hr_len; j++) {
			if (k >= expected_len) {
				result = UNITTEST_RESULT_FAIL;
				continue;
			}
			if (hr[j].type != expected[k].type) {
				result = UNITTEST_RESULT_FAIL;
				printf("test %d: expected = %d, result = %d, fail = ", k, expected[k].type, hr[j].type);
				if (ch == ' ') {
					printf("[SP]\n");
				} else if (ch == '\t') {
					printf("[HT]\n");
				} else if (ch == '\r') {
					printf("[CR]\n");
				} else if (ch == '\n') {
					printf("[LF]\n");
				} else {
					printf("%c\n", ch);
				}
			}
			if ((hr[j].type == HTTP_HEADERLEXER_RESULT_FIELDNAME)&&(hr[j].type == HTTP_HEADERLEXER_RESULT_FIELDCONTENT)) {
				if (hr[j].ch != expected[k].ch) {
					printf("charactor is not expected: %c, %c\n", expected[k].ch, hr[j].ch);
				}
			}
			k++;
		}
	}
	if (k != expected_len) {
		result = UNITTEST_RESULT_FAIL;
		printf("result number is not expected: %d, %d\n", expected_len, k);
	}

	http_headerlexer_finalize(&lexer);

	return result;
}

LOCAL W test_http_headerlexer_expected__count(test_http_headerlexer_expected_t *expected)
{
	W i;

	for (i = 0; ;i++) {
		if (expected[i].type < 0) {
			break;
		}
	}

	return i;
}

LOCAL UNITTEST_RESULT test_http_headerlexer_2()
{
	UB testdata[] = {
		"ABC: abc\r\n"
		"DEF:   a bb \r\n"
		"XYZ:xyz\r\n"
		"  AAA\r\n"
		"LMN: lmn\r\n"
		"\r\n"
	};
	W testdata_len = strlen(testdata);
	test_http_headerlexer_expected_t expected[] = {
		{HTTP_HEADERLEXER_RESULT_FIELDNAME, 'A'},
		{HTTP_HEADERLEXER_RESULT_FIELDNAME, 'B'},
		{HTTP_HEADERLEXER_RESULT_FIELDNAME, 'C'},
		{HTTP_HEADERLEXER_RESULT_FIELDNAME_END, '\0'},
		{HTTP_HEADERLEXER_RESULT_LWS, ' '},
		{HTTP_HEADERLEXER_RESULT_FIELDCONTENT, 'a'},
		{HTTP_HEADERLEXER_RESULT_FIELDCONTENT, 'b'},
		{HTTP_HEADERLEXER_RESULT_FIELDCONTENT, 'c'},
		{HTTP_HEADERLEXER_RESULT_FIELDVALUE_END, '\0'},
		{HTTP_HEADERLEXER_RESULT_FIELDNAME, 'D'},
		{HTTP_HEADERLEXER_RESULT_FIELDNAME, 'E'},
		{HTTP_HEADERLEXER_RESULT_FIELDNAME, 'F'},
		{HTTP_HEADERLEXER_RESULT_FIELDNAME_END, '\0'},
		{HTTP_HEADERLEXER_RESULT_LWS, ' '},
		{HTTP_HEADERLEXER_RESULT_FIELDCONTENT, 'a'},
		{HTTP_HEADERLEXER_RESULT_LWS, ' '},
		{HTTP_HEADERLEXER_RESULT_FIELDCONTENT, 'b'},
		{HTTP_HEADERLEXER_RESULT_FIELDCONTENT, 'b'},
		{HTTP_HEADERLEXER_RESULT_LWS, ' '},
		{HTTP_HEADERLEXER_RESULT_FIELDVALUE_END, '\0'},
		{HTTP_HEADERLEXER_RESULT_FIELDNAME, 'X'},
		{HTTP_HEADERLEXER_RESULT_FIELDNAME, 'Y'},
		{HTTP_HEADERLEXER_RESULT_FIELDNAME, 'Z'},
		{HTTP_HEADERLEXER_RESULT_FIELDNAME_END, '\0'},
		{HTTP_HEADERLEXER_RESULT_FIELDCONTENT, 'x'},
		{HTTP_HEADERLEXER_RESULT_FIELDCONTENT, 'y'},
		{HTTP_HEADERLEXER_RESULT_FIELDCONTENT, 'z'},
		{HTTP_HEADERLEXER_RESULT_LWS, ' '},
		{HTTP_HEADERLEXER_RESULT_FIELDCONTENT, 'A'},
		{HTTP_HEADERLEXER_RESULT_FIELDCONTENT, 'A'},
		{HTTP_HEADERLEXER_RESULT_FIELDCONTENT, 'A'},
		{HTTP_HEADERLEXER_RESULT_FIELDVALUE_END, '\0'},
		{HTTP_HEADERLEXER_RESULT_FIELDNAME, 'L'},
		{HTTP_HEADERLEXER_RESULT_FIELDNAME, 'M'},
		{HTTP_HEADERLEXER_RESULT_FIELDNAME, 'N'},
		{HTTP_HEADERLEXER_RESULT_FIELDNAME_END, '\0'},
		{HTTP_HEADERLEXER_RESULT_LWS, ' '},
		{HTTP_HEADERLEXER_RESULT_FIELDCONTENT, 'l'},
		{HTTP_HEADERLEXER_RESULT_FIELDCONTENT, 'm'},
		{HTTP_HEADERLEXER_RESULT_FIELDCONTENT, 'n'},
		{HTTP_HEADERLEXER_RESULT_FIELDVALUE_END, '\0'},
		{HTTP_HEADERLEXER_RESULT_MESSAGEHEADER_END, '\0'},
		{-1, '\0'}
	};
	W expected_len = test_http_headerlexer_expected__count(expected);

	return test_http_headerlexer_common(testdata, testdata_len, expected, expected_len);
}

EXPORT VOID test_http_headerlexer_main(unittest_driver_t *driver)
{
	UNITTEST_DRIVER_REGIST(driver, test_http_headerlexer_1);
	UNITTEST_DRIVER_REGIST(driver, test_http_headerlexer_2);
}

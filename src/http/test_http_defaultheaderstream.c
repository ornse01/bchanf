/*
 * test_http_defaultheaderstream.c
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

#include "http_defaultheaderstream.h"

#include	<basic.h>
#include	<bstdio.h>
#include	<bstdlib.h>
#include	<bstring.h>

#include    <unittest_driver.h>

#include    "http_typedef.h"

LOCAL UNITTEST_RESULT test_http_defaultheaderstream_common(CONST UB *host, W host_len, UB *expected, W expected_len)
{
	http_defaultheaderstream_t stream;
	UB *str = NULL;
	CONST UB *s0;
	W str_len = 0, s0_len, cmp;
	Bool cont;
	UNITTEST_RESULT result = UNITTEST_RESULT_PASS;

	http_defaultheaderstream_initialize(&stream, host, host_len);
	for (;;) {
		cont = http_defaultheaderstream_make(&stream, &s0, &s0_len);
		if (cont == False) {
			break;
		}
		str = realloc(str, str_len+s0_len+1);
		if (str == NULL) {
			printf("realloc error\n");
			result = UNITTEST_RESULT_FAIL;
			break;
		}
		memcpy(str+str_len, s0, s0_len);
		str_len += s0_len;
		str[str_len] = '\0';
	}
	http_defaultheaderstream_finalize(&stream);

	if (expected_len != str_len) {
		printf("length is not expected. expected = %d, result = %d\n", expected_len, str_len);
		printf("  result = %s", str);
		printf("  expected = %s", expected);
		result = UNITTEST_RESULT_FAIL;
	} else {
		cmp = memcmp(str, expected, expected_len);
		if (cmp != 0) {
			printf("string is not expected\n");
			result = UNITTEST_RESULT_FAIL;
		}
	}

	if (str != NULL) {
		free(str);
	}

	return result;
}

LOCAL UNITTEST_RESULT test_http_defaultheaderstream_1()
{
	UB host[] = "aaa.bbb.ccc.jp";
	W host_len = strlen(host);
	UB expected[] = "Accept-Encoding: gzip\r\nHost: aaa.bbb.ccc.jp\r\n";
	W expected_len = strlen(expected);
	return test_http_defaultheaderstream_common(host, host_len, expected, expected_len);
}

LOCAL UNITTEST_RESULT test_http_defaultheaderstream_2()
{
	UB host[] = "xxx.yyy.zzz.com";
	W host_len = strlen(host);
	UB expected[] = "Accept-Encoding: gzip\r\nHost: xxx.yyy.zzz.com\r\n";
	W expected_len = strlen(expected);
	return test_http_defaultheaderstream_common(host, host_len, expected, expected_len);
}

EXPORT VOID test_http_defaultheaderstream_main(unittest_driver_t *driver)
{
	UNITTEST_DRIVER_REGIST(driver, test_http_defaultheaderstream_1);
	UNITTEST_DRIVER_REGIST(driver, test_http_defaultheaderstream_2);
}

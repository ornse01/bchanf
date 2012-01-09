/*
 * test_parselib.c
 *
 * Copyright (c) 2009-2012 project bchan
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

#include "test_parse.h"

#include "parselib.h"

#include    <btron/btron.h>
#include    <bstdio.h>
#include    <bstring.h>
#include    <bstdlib.h>

#include    <unittest_driver.h>

LOCAL tokenchecker_valuetuple_t test_tokenchecker_testdata_tuple_0001[] = {
	{"aaa", 1},
	{"ab", 2},
	{"abcde", 3},
	{"bbb", 4},
	{"bcdef", 5},
	{"cdefghi", 6},
	{"d", 7}
};
LOCAL UB test_tokenchecker_testdata_etoken_0001[] = ":>";

LOCAL UNITTEST_RESULT test_tokenchecker_1()
{
	tokenchecker_t checker;
	W i, j, len, len2, val, ret;
	UB *str, *str2;

	str = strdup(test_tokenchecker_testdata_tuple_0001[0].name);
	len = strlen(str);

	tokenchecker_initialize(&checker,
							 test_tokenchecker_testdata_tuple_0001,
							 7,
							 test_tokenchecker_testdata_etoken_0001);

	for (i = 0; i < len; i++) {
		ret = tokenchecker_inputchar(&checker, str[i], &val);
		if (ret != TOKENCHECKER_CONTINUE) {
			printf("tokenchecker_inputchar fail: right string\n");
			free(str);
			return UNITTEST_RESULT_FAIL;
		}
		tokenchecker_getlastmatchedstring(&checker, &str2, &len2);
		if (strncmp(str, str2, len2) != 0) {
			printf("tokenchecker_getlastmatchedstring fail: right string\n");
			printf(" ");
			for (j = 0; j < len; j++) {
				printf("%c", str[j]);
			}
			printf(" : ");
			for (j = 0; j < len; j++) {
				printf("%c", str2[j]);
			}
			printf("\n");
			free(str);
			return UNITTEST_RESULT_FAIL;
		}
	}
	ret = tokenchecker_inputchar(&checker, ':', &val);
	if (ret != TOKENCHECKER_DETERMINE) {
		printf("tokenchecker_inputchar fail: end character\n");
		free(str);
		return UNITTEST_RESULT_FAIL;
	}
	if (val != 1) {
		printf("tokenchecker_inputchar fail: select value\n");
		free(str);
		return UNITTEST_RESULT_FAIL;
	}
	tokenchecker_getlastmatchedstring(&checker, &str2, &len2);
	if (strncmp(str, str2, len2) != 0) {
		printf("tokenchecker_getlastmatchedstring fail: end token\n");
		free(str);
		return UNITTEST_RESULT_FAIL;
	}
	ret = tokenchecker_inputchar(&checker, ':', &val);
	if (ret != TOKENCHECKER_AFTER_END) {
		printf("tokenchecker_inputchar fail: after end\n");
		free(str);
		return UNITTEST_RESULT_FAIL;
	}

	free(str);
	return UNITTEST_RESULT_PASS;
}

LOCAL UNITTEST_RESULT test_tokenchecker_2()
{
	tokenchecker_t checker;
	W i, j, len, len2, val, ret;
	UB *str, *str2;

	str = strdup(test_tokenchecker_testdata_tuple_0001[1].name);
	len = strlen(str);

	tokenchecker_initialize(&checker,
							 test_tokenchecker_testdata_tuple_0001,
							 7,
							 test_tokenchecker_testdata_etoken_0001);

	for (i = 0; i < len; i++) {
		ret = tokenchecker_inputchar(&checker, str[i], &val);
		if (ret != TOKENCHECKER_CONTINUE) {
			printf("tokenchecker_inputchar fail: right string\n");
			free(str);
			return UNITTEST_RESULT_FAIL;
		}
		tokenchecker_getlastmatchedstring(&checker, &str2, &len2);
		if (strncmp(str, str2, len2) != 0) {
			printf("tokenchecker_getlastmatchedstring fail: right string\n");
			printf(" ");
			for (j = 0; j < len; j++) {
				printf("%c", str[j]);
			}
			printf(" : ");
			for (j = 0; j < len; j++) {
				printf("%c", str2[j]);
			}
			printf("\n");
			free(str);
			return UNITTEST_RESULT_FAIL;
		}
	}
	ret = tokenchecker_inputchar(&checker, ':', &val);
	if (ret != TOKENCHECKER_DETERMINE) {
		printf("tokenchecker_inputchar fail: end character\n");
		free(str);
		return UNITTEST_RESULT_FAIL;
	}
	if (val != 2) {
		printf("tokenchecker_inputchar fail: select value\n");
		free(str);
		return UNITTEST_RESULT_FAIL;
	}
	tokenchecker_getlastmatchedstring(&checker, &str2, &len2);
	if (strncmp(str, str2, len2) != 0) {
		printf("tokenchecker_getlastmatchedstring fail: end token\n");
		free(str);
		return UNITTEST_RESULT_FAIL;
	}
	ret = tokenchecker_inputchar(&checker, ':', &val);
	if (ret != TOKENCHECKER_AFTER_END) {
		printf("tokenchecker_inputchar fail: after end\n");
		free(str);
		return UNITTEST_RESULT_FAIL;
	}

	free(str);
	return UNITTEST_RESULT_PASS;
}

LOCAL UNITTEST_RESULT test_tokenchecker_3()
{
	tokenchecker_t checker;
	W i, j, len, len2, val, ret;
	UB *str, *str2;

	str = strdup(test_tokenchecker_testdata_tuple_0001[2].name);
	len = strlen(str);

	tokenchecker_initialize(&checker,
							 test_tokenchecker_testdata_tuple_0001,
							 7,
							 test_tokenchecker_testdata_etoken_0001);

	for (i = 0; i < len; i++) {
		ret = tokenchecker_inputchar(&checker, str[i], &val);
		if (ret != TOKENCHECKER_CONTINUE) {
			printf("tokenchecker_inputchar fail: right string\n");
			free(str);
			return UNITTEST_RESULT_FAIL;
		}
		tokenchecker_getlastmatchedstring(&checker, &str2, &len2);
		if (strncmp(str, str2, len2) != 0) {
			printf("tokenchecker_getlastmatchedstring fail: right string\n");
			printf(" ");
			for (j = 0; j < len; j++) {
				printf("%c", str[j]);
			}
			printf(" : ");
			for (j = 0; j < len; j++) {
				printf("%c", str2[j]);
			}
			printf("\n");
			free(str);
			return UNITTEST_RESULT_FAIL;
		}
	}
	ret = tokenchecker_inputchar(&checker, ':', &val);
	if (ret != TOKENCHECKER_DETERMINE) {
		printf("tokenchecker_inputchar fail: end character\n");
		free(str);
		return UNITTEST_RESULT_FAIL;
	}
	if (val != 3) {
		printf("tokenchecker_inputchar fail: select value\n");
		free(str);
		return UNITTEST_RESULT_FAIL;
	}
	tokenchecker_getlastmatchedstring(&checker, &str2, &len2);
	if (strncmp(str, str2, len2) != 0) {
		printf("tokenchecker_getlastmatchedstring fail: end token\n");
		free(str);
		return UNITTEST_RESULT_FAIL;
	}
	ret = tokenchecker_inputchar(&checker, ':', &val);
	if (ret != TOKENCHECKER_AFTER_END) {
		printf("tokenchecker_inputchar fail: after end\n");
		free(str);
		return UNITTEST_RESULT_FAIL;
	}

	free(str);
	return UNITTEST_RESULT_PASS;
}

LOCAL UNITTEST_RESULT test_tokenchecker_4()
{
	tokenchecker_t checker;
	W i, j, len, len2, val, ret;
	UB *str, *str2;

	str = strdup(test_tokenchecker_testdata_tuple_0001[3].name);
	len = strlen(str);

	tokenchecker_initialize(&checker,
							 test_tokenchecker_testdata_tuple_0001,
							 7,
							 test_tokenchecker_testdata_etoken_0001);

	for (i = 0; i < len; i++) {
		ret = tokenchecker_inputchar(&checker, str[i], &val);
		if (ret != TOKENCHECKER_CONTINUE) {
			printf("tokenchecker_inputchar fail: right string\n");
			free(str);
			return UNITTEST_RESULT_FAIL;
		}
		tokenchecker_getlastmatchedstring(&checker, &str2, &len2);
		if (strncmp(str, str2, len2) != 0) {
			printf("tokenchecker_getlastmatchedstring fail: right string\n");
			printf(" ");
			for (j = 0; j < len; j++) {
				printf("%c", str[j]);
			}
			printf(" : ");
			for (j = 0; j < len; j++) {
				printf("%c", str2[j]);
			}
			printf("\n");
			free(str);
			return UNITTEST_RESULT_FAIL;
		}
	}
	ret = tokenchecker_inputchar(&checker, ':', &val);
	if (ret != TOKENCHECKER_DETERMINE) {
		printf("tokenchecker_inputchar fail: end character\n");
		free(str);
		return UNITTEST_RESULT_FAIL;
	}
	if (val != 4) {
		printf("tokenchecker_inputchar fail: select value\n");
		free(str);
		return UNITTEST_RESULT_FAIL;
	}
	tokenchecker_getlastmatchedstring(&checker, &str2, &len2);
	if (strncmp(str, str2, len2) != 0) {
		printf("tokenchecker_getlastmatchedstring fail: end token\n");
		free(str);
		return UNITTEST_RESULT_FAIL;
	}
	ret = tokenchecker_inputchar(&checker, ':', &val);
	if (ret != TOKENCHECKER_AFTER_END) {
		printf("tokenchecker_inputchar fail: after end\n");
		free(str);
		return UNITTEST_RESULT_FAIL;
	}

	free(str);
	return UNITTEST_RESULT_PASS;
}

LOCAL UNITTEST_RESULT test_tokenchecker_5()
{
	tokenchecker_t checker;
	W i, j, len, len2, val, ret;
	UB *str, *str2;

	str = strdup(test_tokenchecker_testdata_tuple_0001[4].name);
	len = strlen(str);

	tokenchecker_initialize(&checker,
							 test_tokenchecker_testdata_tuple_0001,
							 7,
							 test_tokenchecker_testdata_etoken_0001);

	for (i = 0; i < len; i++) {
		ret = tokenchecker_inputchar(&checker, str[i], &val);
		if (ret != TOKENCHECKER_CONTINUE) {
			printf("tokenchecker_inputchar fail: right string\n");
			free(str);
			return UNITTEST_RESULT_FAIL;
		}
		tokenchecker_getlastmatchedstring(&checker, &str2, &len2);
		if (strncmp(str, str2, len2) != 0) {
			printf("tokenchecker_getlastmatchedstring fail: right string\n");
			printf(" ");
			for (j = 0; j < len; j++) {
				printf("%c", str[j]);
			}
			printf(" : ");
			for (j = 0; j < len; j++) {
				printf("%c", str2[j]);
			}
			printf("\n");
			free(str);
			return UNITTEST_RESULT_FAIL;
		}
	}
	ret = tokenchecker_inputchar(&checker, ':', &val);
	if (ret != TOKENCHECKER_DETERMINE) {
		printf("tokenchecker_inputchar fail: end character\n");
		free(str);
		return UNITTEST_RESULT_FAIL;
	}
	if (val != 5) {
		printf("tokenchecker_inputchar fail: select value\n");
		free(str);
		return UNITTEST_RESULT_FAIL;
	}
	tokenchecker_getlastmatchedstring(&checker, &str2, &len2);
	if (strncmp(str, str2, len2) != 0) {
		printf("tokenchecker_getlastmatchedstring fail: end token\n");
		free(str);
		return UNITTEST_RESULT_FAIL;
	}
	ret = tokenchecker_inputchar(&checker, ':', &val);
	if (ret != TOKENCHECKER_AFTER_END) {
		printf("tokenchecker_inputchar fail: after end\n");
		free(str);
		return UNITTEST_RESULT_FAIL;
	}

	free(str);
	return UNITTEST_RESULT_PASS;
}

LOCAL UNITTEST_RESULT test_tokenchecker_6()
{
	tokenchecker_t checker;
	W i, j, len, len2, val, ret;
	UB *str, *str2;

	str = strdup(test_tokenchecker_testdata_tuple_0001[5].name);
	len = strlen(str);

	tokenchecker_initialize(&checker,
							 test_tokenchecker_testdata_tuple_0001,
							 7,
							 test_tokenchecker_testdata_etoken_0001);

	for (i = 0; i < len; i++) {
		ret = tokenchecker_inputchar(&checker, str[i], &val);
		if (ret != TOKENCHECKER_CONTINUE) {
			printf("tokenchecker_inputchar fail: right string\n");
			free(str);
			return UNITTEST_RESULT_FAIL;
		}
		tokenchecker_getlastmatchedstring(&checker, &str2, &len2);
		if (strncmp(str, str2, len2) != 0) {
			printf("tokenchecker_getlastmatchedstring fail: right string\n");
			printf(" ");
			for (j = 0; j < len; j++) {
				printf("%c", str[j]);
			}
			printf(" : ");
			for (j = 0; j < len; j++) {
				printf("%c", str2[j]);
			}
			printf("\n");
			free(str);
			return UNITTEST_RESULT_FAIL;
		}
	}
	ret = tokenchecker_inputchar(&checker, ':', &val);
	if (ret != TOKENCHECKER_DETERMINE) {
		printf("tokenchecker_inputchar fail: end character\n");
		free(str);
		return UNITTEST_RESULT_FAIL;
	}
	if (val != 6) {
		printf("tokenchecker_inputchar fail: select value\n");
		free(str);
		return UNITTEST_RESULT_FAIL;
	}
	tokenchecker_getlastmatchedstring(&checker, &str2, &len2);
	if (strncmp(str, str2, len2) != 0) {
		printf("tokenchecker_getlastmatchedstring fail: end token\n");
		free(str);
		return UNITTEST_RESULT_FAIL;
	}
	ret = tokenchecker_inputchar(&checker, ':', &val);
	if (ret != TOKENCHECKER_AFTER_END) {
		printf("tokenchecker_inputchar fail: after end\n");
		free(str);
		return UNITTEST_RESULT_FAIL;
	}

	free(str);
	return UNITTEST_RESULT_PASS;
}

LOCAL UNITTEST_RESULT test_tokenchecker_7()
{
	tokenchecker_t checker;
	W i, j, len, len2, val, ret;
	UB *str, *str2;

	str = strdup(test_tokenchecker_testdata_tuple_0001[6].name);
	len = strlen(str);

	tokenchecker_initialize(&checker,
							 test_tokenchecker_testdata_tuple_0001,
							 7,
							 test_tokenchecker_testdata_etoken_0001);

	for (i = 0; i < len; i++) {
		ret = tokenchecker_inputchar(&checker, str[i], &val);
		if (ret != TOKENCHECKER_CONTINUE) {
			printf("tokenchecker_inputchar fail: right string\n");
			free(str);
			return UNITTEST_RESULT_FAIL;
		}
		tokenchecker_getlastmatchedstring(&checker, &str2, &len2);
		if (strncmp(str, str2, len2) != 0) {
			printf("tokenchecker_getlastmatchedstring fail: right string\n");
			printf(" ");
			for (j = 0; j < len; j++) {
				printf("%c", str[j]);
			}
			printf(" : ");
			for (j = 0; j < len; j++) {
				printf("%c", str2[j]);
			}
			printf("\n");
			free(str);
			return UNITTEST_RESULT_FAIL;
		}
	}
	ret = tokenchecker_inputchar(&checker, ':', &val);
	if (ret != TOKENCHECKER_DETERMINE) {
		printf("tokenchecker_inputchar fail: end character\n");
		free(str);
		return UNITTEST_RESULT_FAIL;
	}
	if (val != 7) {
		printf("tokenchecker_inputchar fail: select value\n");
		free(str);
		return UNITTEST_RESULT_FAIL;
	}
	tokenchecker_getlastmatchedstring(&checker, &str2, &len2);
	if (strncmp(str, str2, len2) != 0) {
		printf("tokenchecker_getlastmatchedstring fail: end token\n");
		free(str);
		return UNITTEST_RESULT_FAIL;
	}
	ret = tokenchecker_inputchar(&checker, ':', &val);
	if (ret != TOKENCHECKER_AFTER_END) {
		printf("tokenchecker_inputchar fail: after end\n");
		free(str);
		return UNITTEST_RESULT_FAIL;
	}

	free(str);
	return UNITTEST_RESULT_PASS;
}

LOCAL UNITTEST_RESULT test_tokenchecker_nomatch_getlastmatchedstring_sequence(tokenchecker_t *checker, W num, UB *teststr, W teststr_len)
{
	W i, j, len2, ret, val;
	UB *str2;

	for (i = 0; i < num; i++) {
		ret = tokenchecker_inputchar(checker, teststr[i], &val);
		if (ret != TOKENCHECKER_CONTINUE) {
			printf("tokenchecker_inputchar fail: right string\n");
			return UNITTEST_RESULT_FAIL;
		}
	}
	ret = tokenchecker_inputchar(checker, 'z', &val);
	if (ret != TOKENCHECKER_CONTINUE_NOMATCH) {
		printf("tokenchecker_inputchar fail: nomatch char\n");
		return UNITTEST_RESULT_FAIL;
	}
	tokenchecker_getlastmatchedstring(checker, &str2, &len2);
	if (len2 != num) {
		printf("tokenchecker_getlastmatchedstring fail: length\n");
		return UNITTEST_RESULT_FAIL;
	}
	if (strncmp(teststr, str2, len2) != 0) {
		printf("tokenchecker_getlastmatchedstring fail: nomatch string\n");
		printf(" ");
		for (j = 0; j < len2; j++) {
			printf("%c", teststr[j]);
		}
		printf(" : ");
		for (j = 0; j < len2; j++) {
			printf("%c", str2[j]);
		}
		printf("\n");
		return UNITTEST_RESULT_FAIL;
	}

	return UNITTEST_RESULT_PASS;
}

LOCAL UNITTEST_RESULT test_tokenchecker_8()
{
	tokenchecker_t checker;
	W i,len;
	UB *str;
	UNITTEST_RESULT result;

	str = strdup(test_tokenchecker_testdata_tuple_0001[0].name);
	len = strlen(str);

	tokenchecker_initialize(&checker,
							 test_tokenchecker_testdata_tuple_0001,
							 7,
							 test_tokenchecker_testdata_etoken_0001);

	for (i = 1; i < len; i++) {
		result = test_tokenchecker_nomatch_getlastmatchedstring_sequence(&checker, i, str, len);
		if (result != UNITTEST_RESULT_PASS) {
			return result;
		}
		tokenchecker_clear(&checker);
	}

	free(str);

	return UNITTEST_RESULT_PASS;
}

LOCAL UNITTEST_RESULT test_tokenchecker_9()
{
	tokenchecker_t checker;
	W i,len;
	UB *str;
	UNITTEST_RESULT result;

	str = strdup(test_tokenchecker_testdata_tuple_0001[1].name);
	len = strlen(str);

	tokenchecker_initialize(&checker,
							 test_tokenchecker_testdata_tuple_0001,
							 7,
							 test_tokenchecker_testdata_etoken_0001);

	for (i = 1; i < len; i++) {
		result = test_tokenchecker_nomatch_getlastmatchedstring_sequence(&checker, i, str, len);
		if (result != UNITTEST_RESULT_PASS) {
			return result;
		}
		tokenchecker_clear(&checker);
	}

	free(str);

	return UNITTEST_RESULT_PASS;
}

LOCAL UNITTEST_RESULT test_tokenchecker_10()
{
	tokenchecker_t checker;
	W i,len;
	UB *str;
	UNITTEST_RESULT result;

	str = strdup(test_tokenchecker_testdata_tuple_0001[2].name);
	len = strlen(str);

	tokenchecker_initialize(&checker,
							 test_tokenchecker_testdata_tuple_0001,
							 7,
							 test_tokenchecker_testdata_etoken_0001);

	for (i = 1; i < len; i++) {
		result = test_tokenchecker_nomatch_getlastmatchedstring_sequence(&checker, i, str, len);
		if (result != UNITTEST_RESULT_PASS) {
			return result;
		}
		tokenchecker_clear(&checker);
	}

	free(str);

	return UNITTEST_RESULT_PASS;
}

LOCAL UNITTEST_RESULT test_tokenchecker_11()
{
	tokenchecker_t checker;
	W i,len;
	UB *str;
	UNITTEST_RESULT result;

	str = strdup(test_tokenchecker_testdata_tuple_0001[3].name);
	len = strlen(str);

	tokenchecker_initialize(&checker,
							 test_tokenchecker_testdata_tuple_0001,
							 7,
							 test_tokenchecker_testdata_etoken_0001);

	for (i = 1; i < len; i++) {
		result = test_tokenchecker_nomatch_getlastmatchedstring_sequence(&checker, i, str, len);
		if (result != UNITTEST_RESULT_PASS) {
			return result;
		}
		tokenchecker_clear(&checker);
	}

	free(str);

	return UNITTEST_RESULT_PASS;
}

LOCAL UNITTEST_RESULT test_tokenchecker_12()
{
	tokenchecker_t checker;
	W i,len;
	UB *str;
	UNITTEST_RESULT result;

	str = strdup(test_tokenchecker_testdata_tuple_0001[4].name);
	len = strlen(str);

	tokenchecker_initialize(&checker,
							 test_tokenchecker_testdata_tuple_0001,
							 7,
							 test_tokenchecker_testdata_etoken_0001);

	for (i = 1; i < len; i++) {
		result = test_tokenchecker_nomatch_getlastmatchedstring_sequence(&checker, i, str, len);
		if (result != UNITTEST_RESULT_PASS) {
			return result;
		}
		tokenchecker_clear(&checker);
	}

	free(str);

	return UNITTEST_RESULT_PASS;
}

LOCAL UNITTEST_RESULT test_tokenchecker_13()
{
	tokenchecker_t checker;
	W i,len;
	UB *str;
	UNITTEST_RESULT result;

	str = strdup(test_tokenchecker_testdata_tuple_0001[5].name);
	len = strlen(str);

	tokenchecker_initialize(&checker,
							 test_tokenchecker_testdata_tuple_0001,
							 7,
							 test_tokenchecker_testdata_etoken_0001);

	for (i = 1; i < len; i++) {
		result = test_tokenchecker_nomatch_getlastmatchedstring_sequence(&checker, i, str, len);
		if (result != UNITTEST_RESULT_PASS) {
			return result;
		}
		tokenchecker_clear(&checker);
	}

	free(str);

	return UNITTEST_RESULT_PASS;
}

LOCAL UNITTEST_RESULT test_tokenchecker_14()
{
	tokenchecker_t checker;
	W i,len;
	UB *str;
	UNITTEST_RESULT result;

	str = strdup(test_tokenchecker_testdata_tuple_0001[6].name);
	len = strlen(str);

	tokenchecker_initialize(&checker,
							 test_tokenchecker_testdata_tuple_0001,
							 7,
							 test_tokenchecker_testdata_etoken_0001);

	for (i = 1; i < len; i++) {
		result = test_tokenchecker_nomatch_getlastmatchedstring_sequence(&checker, i, str, len);
		if (result != UNITTEST_RESULT_PASS) {
			return result;
		}
		tokenchecker_clear(&checker);
	}

	free(str);

	return UNITTEST_RESULT_PASS;
}

/* testing endinput */

LOCAL UNITTEST_RESULT test_tokenchecker_15()
{
	tokenchecker_t checker;
	W i, j, len, len2, val, ret;
	UB *str, *str2;

	str = strdup(test_tokenchecker_testdata_tuple_0001[0].name);
	len = strlen(str);

	tokenchecker_initialize(&checker,
							 test_tokenchecker_testdata_tuple_0001,
							 7,
							 test_tokenchecker_testdata_etoken_0001);

	for (i = 0; i < len; i++) {
		ret = tokenchecker_inputchar(&checker, str[i], &val);
		if (ret != TOKENCHECKER_CONTINUE) {
			printf("tokenchecker_inputchar fail: right string\n");
			free(str);
			return UNITTEST_RESULT_FAIL;
		}
		tokenchecker_getlastmatchedstring(&checker, &str2, &len2);
		if (strncmp(str, str2, len2) != 0) {
			printf("tokenchecker_getlastmatchedstring fail: right string\n");
			printf(" ");
			for (j = 0; j < len; j++) {
				printf("%c", str[j]);
			}
			printf(" : ");
			for (j = 0; j < len; j++) {
				printf("%c", str2[j]);
			}
			printf("\n");
			free(str);
			return UNITTEST_RESULT_FAIL;
		}
	}
	ret = tokenchecker_endinput(&checker, &val);
	if (ret != TOKENCHECKER_DETERMINE) {
		printf("tokenchecker_endinput fail\n");
		free(str);
		return UNITTEST_RESULT_FAIL;
	}
	if (val != 1) {
		printf("tokenchecker_endinput fail: select value\n");
		free(str);
		return UNITTEST_RESULT_FAIL;
	}
	tokenchecker_getlastmatchedstring(&checker, &str2, &len2);
	if (strncmp(str, str2, len2) != 0) {
		printf("tokenchecker_getlastmatchedstring fail: end token\n");
		free(str);
		return UNITTEST_RESULT_FAIL;
	}
	ret = tokenchecker_inputchar(&checker, ':', &val);
	if (ret != TOKENCHECKER_AFTER_END) {
		printf("tokenchecker_inputchar fail: after end\n");
		free(str);
		return UNITTEST_RESULT_FAIL;
	}

	free(str);
	return UNITTEST_RESULT_PASS;
}

LOCAL UNITTEST_RESULT test_tokenchecker_16()
{
	tokenchecker_t checker;
	W i, j, len, len2, val, ret;
	UB *str, *str2;

	str = strdup(test_tokenchecker_testdata_tuple_0001[1].name);
	len = strlen(str);

	tokenchecker_initialize(&checker,
							 test_tokenchecker_testdata_tuple_0001,
							 7,
							 test_tokenchecker_testdata_etoken_0001);

	for (i = 0; i < len; i++) {
		ret = tokenchecker_inputchar(&checker, str[i], &val);
		if (ret != TOKENCHECKER_CONTINUE) {
			printf("tokenchecker_inputchar fail: right string\n");
			free(str);
			return UNITTEST_RESULT_FAIL;
		}
		tokenchecker_getlastmatchedstring(&checker, &str2, &len2);
		if (strncmp(str, str2, len2) != 0) {
			printf("tokenchecker_getlastmatchedstring fail: right string\n");
			printf(" ");
			for (j = 0; j < len; j++) {
				printf("%c", str[j]);
			}
			printf(" : ");
			for (j = 0; j < len; j++) {
				printf("%c", str2[j]);
			}
			printf("\n");
			free(str);
			return UNITTEST_RESULT_FAIL;
		}
	}
	ret = tokenchecker_endinput(&checker, &val);
	if (ret != TOKENCHECKER_DETERMINE) {
		printf("tokenchecker_endinput fail\n");
		free(str);
		return UNITTEST_RESULT_FAIL;
	}
	if (val != 2) {
		printf("tokenchecker_endinput fail: select value\n");
		free(str);
		return UNITTEST_RESULT_FAIL;
	}
	tokenchecker_getlastmatchedstring(&checker, &str2, &len2);
	if (strncmp(str, str2, len2) != 0) {
		printf("tokenchecker_getlastmatchedstring fail: end token\n");
		free(str);
		return UNITTEST_RESULT_FAIL;
	}
	ret = tokenchecker_inputchar(&checker, ':', &val);
	if (ret != TOKENCHECKER_AFTER_END) {
		printf("tokenchecker_inputchar fail: after end\n");
		free(str);
		return UNITTEST_RESULT_FAIL;
	}

	free(str);
	return UNITTEST_RESULT_PASS;
}

LOCAL UNITTEST_RESULT test_tokenchecker_17()
{
	tokenchecker_t checker;
	W i, j, len, len2, val, ret;
	UB *str, *str2;

	str = strdup(test_tokenchecker_testdata_tuple_0001[2].name);
	len = strlen(str);

	tokenchecker_initialize(&checker,
							 test_tokenchecker_testdata_tuple_0001,
							 7,
							 test_tokenchecker_testdata_etoken_0001);

	for (i = 0; i < len; i++) {
		ret = tokenchecker_inputchar(&checker, str[i], &val);
		if (ret != TOKENCHECKER_CONTINUE) {
			printf("tokenchecker_inputchar fail: right string\n");
			free(str);
			return UNITTEST_RESULT_FAIL;
		}
		tokenchecker_getlastmatchedstring(&checker, &str2, &len2);
		if (strncmp(str, str2, len2) != 0) {
			printf("tokenchecker_getlastmatchedstring fail: right string\n");
			printf(" ");
			for (j = 0; j < len; j++) {
				printf("%c", str[j]);
			}
			printf(" : ");
			for (j = 0; j < len; j++) {
				printf("%c", str2[j]);
			}
			printf("\n");
			free(str);
			return UNITTEST_RESULT_FAIL;
		}
	}
	ret = tokenchecker_endinput(&checker, &val);
	if (ret != TOKENCHECKER_DETERMINE) {
		printf("tokenchecker_endinput fail\n");
		free(str);
		return UNITTEST_RESULT_FAIL;
	}
	if (val != 3) {
		printf("tokenchecker_endinput fail: select value\n");
		free(str);
		return UNITTEST_RESULT_FAIL;
	}
	tokenchecker_getlastmatchedstring(&checker, &str2, &len2);
	if (strncmp(str, str2, len2) != 0) {
		printf("tokenchecker_getlastmatchedstring fail: end token\n");
		free(str);
		return UNITTEST_RESULT_FAIL;
	}
	ret = tokenchecker_inputchar(&checker, ':', &val);
	if (ret != TOKENCHECKER_AFTER_END) {
		printf("tokenchecker_inputchar fail: after end\n");
		free(str);
		return UNITTEST_RESULT_FAIL;
	}

	free(str);
	return UNITTEST_RESULT_PASS;
}

LOCAL UNITTEST_RESULT test_tokenchecker_18()
{
	tokenchecker_t checker;
	W i, j, len, len2, val, ret;
	UB *str, *str2;

	str = strdup(test_tokenchecker_testdata_tuple_0001[3].name);
	len = strlen(str);

	tokenchecker_initialize(&checker,
							 test_tokenchecker_testdata_tuple_0001,
							 7,
							 test_tokenchecker_testdata_etoken_0001);

	for (i = 0; i < len; i++) {
		ret = tokenchecker_inputchar(&checker, str[i], &val);
		if (ret != TOKENCHECKER_CONTINUE) {
			printf("tokenchecker_inputchar fail: right string\n");
			free(str);
			return UNITTEST_RESULT_FAIL;
		}
		tokenchecker_getlastmatchedstring(&checker, &str2, &len2);
		if (strncmp(str, str2, len2) != 0) {
			printf("tokenchecker_getlastmatchedstring fail: right string\n");
			printf(" ");
			for (j = 0; j < len; j++) {
				printf("%c", str[j]);
			}
			printf(" : ");
			for (j = 0; j < len; j++) {
				printf("%c", str2[j]);
			}
			printf("\n");
			free(str);
			return UNITTEST_RESULT_FAIL;
		}
	}
	ret = tokenchecker_endinput(&checker, &val);
	if (ret != TOKENCHECKER_DETERMINE) {
		printf("tokenchecker_endinput fail\n");
		free(str);
		return UNITTEST_RESULT_FAIL;
	}
	if (val != 4) {
		printf("tokenchecker_endinput fail: select value\n");
		free(str);
		return UNITTEST_RESULT_FAIL;
	}
	tokenchecker_getlastmatchedstring(&checker, &str2, &len2);
	if (strncmp(str, str2, len2) != 0) {
		printf("tokenchecker_getlastmatchedstring fail: end token\n");
		free(str);
		return UNITTEST_RESULT_FAIL;
	}
	ret = tokenchecker_inputchar(&checker, ':', &val);
	if (ret != TOKENCHECKER_AFTER_END) {
		printf("tokenchecker_inputchar fail: after end\n");
		free(str);
		return UNITTEST_RESULT_FAIL;
	}

	free(str);
	return UNITTEST_RESULT_PASS;
}

LOCAL UNITTEST_RESULT test_tokenchecker_19()
{
	tokenchecker_t checker;
	W i, j, len, len2, val, ret;
	UB *str, *str2;

	str = strdup(test_tokenchecker_testdata_tuple_0001[4].name);
	len = strlen(str);

	tokenchecker_initialize(&checker,
							 test_tokenchecker_testdata_tuple_0001,
							 7,
							 test_tokenchecker_testdata_etoken_0001);

	for (i = 0; i < len; i++) {
		ret = tokenchecker_inputchar(&checker, str[i], &val);
		if (ret != TOKENCHECKER_CONTINUE) {
			printf("tokenchecker_inputchar fail: right string\n");
			free(str);
			return UNITTEST_RESULT_FAIL;
		}
		tokenchecker_getlastmatchedstring(&checker, &str2, &len2);
		if (strncmp(str, str2, len2) != 0) {
			printf("tokenchecker_getlastmatchedstring fail: right string\n");
			printf(" ");
			for (j = 0; j < len; j++) {
				printf("%c", str[j]);
			}
			printf(" : ");
			for (j = 0; j < len; j++) {
				printf("%c", str2[j]);
			}
			printf("\n");
			free(str);
			return UNITTEST_RESULT_FAIL;
		}
	}
	ret = tokenchecker_endinput(&checker, &val);
	if (ret != TOKENCHECKER_DETERMINE) {
		printf("tokenchecker_endinput fail\n");
		free(str);
		return UNITTEST_RESULT_FAIL;
	}
	if (val != 5) {
		printf("tokenchecker_endinput fail: select value\n");
		free(str);
		return UNITTEST_RESULT_FAIL;
	}
	tokenchecker_getlastmatchedstring(&checker, &str2, &len2);
	if (strncmp(str, str2, len2) != 0) {
		printf("tokenchecker_getlastmatchedstring fail: end token\n");
		free(str);
		return UNITTEST_RESULT_FAIL;
	}
	ret = tokenchecker_inputchar(&checker, ':', &val);
	if (ret != TOKENCHECKER_AFTER_END) {
		printf("tokenchecker_inputchar fail: after end\n");
		free(str);
		return UNITTEST_RESULT_FAIL;
	}

	free(str);
	return UNITTEST_RESULT_PASS;
}

LOCAL UNITTEST_RESULT test_tokenchecker_20()
{
	tokenchecker_t checker;
	W i, j, len, len2, val, ret;
	UB *str, *str2;

	str = strdup(test_tokenchecker_testdata_tuple_0001[5].name);
	len = strlen(str);

	tokenchecker_initialize(&checker,
							 test_tokenchecker_testdata_tuple_0001,
							 7,
							 test_tokenchecker_testdata_etoken_0001);

	for (i = 0; i < len; i++) {
		ret = tokenchecker_inputchar(&checker, str[i], &val);
		if (ret != TOKENCHECKER_CONTINUE) {
			printf("tokenchecker_inputchar fail: right string\n");
			free(str);
			return UNITTEST_RESULT_FAIL;
		}
		tokenchecker_getlastmatchedstring(&checker, &str2, &len2);
		if (strncmp(str, str2, len2) != 0) {
			printf("tokenchecker_getlastmatchedstring fail: right string\n");
			printf(" ");
			for (j = 0; j < len; j++) {
				printf("%c", str[j]);
			}
			printf(" : ");
			for (j = 0; j < len; j++) {
				printf("%c", str2[j]);
			}
			printf("\n");
			free(str);
			return UNITTEST_RESULT_FAIL;
		}
	}
	ret = tokenchecker_endinput(&checker, &val);
	if (ret != TOKENCHECKER_DETERMINE) {
		printf("tokenchecker_endinput fail\n");
		free(str);
		return UNITTEST_RESULT_FAIL;
	}
	if (val != 6) {
		printf("tokenchecker_endinput fail: select value\n");
		free(str);
		return UNITTEST_RESULT_FAIL;
	}
	tokenchecker_getlastmatchedstring(&checker, &str2, &len2);
	if (strncmp(str, str2, len2) != 0) {
		printf("tokenchecker_getlastmatchedstring fail: end token\n");
		free(str);
		return UNITTEST_RESULT_FAIL;
	}
	ret = tokenchecker_inputchar(&checker, ':', &val);
	if (ret != TOKENCHECKER_AFTER_END) {
		printf("tokenchecker_inputchar fail: after end\n");
		free(str);
		return UNITTEST_RESULT_FAIL;
	}

	free(str);
	return UNITTEST_RESULT_PASS;
}

LOCAL UNITTEST_RESULT test_tokenchecker_21()
{
	tokenchecker_t checker;
	W i, j, len, len2, val, ret;
	UB *str, *str2;

	str = strdup(test_tokenchecker_testdata_tuple_0001[6].name);
	len = strlen(str);

	tokenchecker_initialize(&checker,
							 test_tokenchecker_testdata_tuple_0001,
							 7,
							 test_tokenchecker_testdata_etoken_0001);

	for (i = 0; i < len; i++) {
		ret = tokenchecker_inputchar(&checker, str[i], &val);
		if (ret != TOKENCHECKER_CONTINUE) {
			printf("tokenchecker_inputchar fail: right string\n");
			free(str);
			return UNITTEST_RESULT_FAIL;
		}
		tokenchecker_getlastmatchedstring(&checker, &str2, &len2);
		if (strncmp(str, str2, len2) != 0) {
			printf("tokenchecker_getlastmatchedstring fail: right string\n");
			printf(" ");
			for (j = 0; j < len; j++) {
				printf("%c", str[j]);
			}
			printf(" : ");
			for (j = 0; j < len; j++) {
				printf("%c", str2[j]);
			}
			printf("\n");
			free(str);
			return UNITTEST_RESULT_FAIL;
		}
	}
	ret = tokenchecker_endinput(&checker, &val);
	if (ret != TOKENCHECKER_DETERMINE) {
		printf("tokenchecker_endinput fail\n");
		free(str);
		return UNITTEST_RESULT_FAIL;
	}
	if (val != 7) {
		printf("tokenchecker_endinput fail: select value\n");
		free(str);
		return UNITTEST_RESULT_FAIL;
	}
	tokenchecker_getlastmatchedstring(&checker, &str2, &len2);
	if (strncmp(str, str2, len2) != 0) {
		printf("tokenchecker_getlastmatchedstring fail: end token\n");
		free(str);
		return UNITTEST_RESULT_FAIL;
	}
	ret = tokenchecker_inputchar(&checker, ':', &val);
	if (ret != TOKENCHECKER_AFTER_END) {
		printf("tokenchecker_inputchar fail: after end\n");
		free(str);
		return UNITTEST_RESULT_FAIL;
	}

	free(str);
	return UNITTEST_RESULT_PASS;
}

EXPORT VOID test_tokenchecker_main(unittest_driver_t *driver)
{
	UNITTEST_DRIVER_REGIST(driver, test_tokenchecker_1);
	UNITTEST_DRIVER_REGIST(driver, test_tokenchecker_2);
	UNITTEST_DRIVER_REGIST(driver, test_tokenchecker_3);
	UNITTEST_DRIVER_REGIST(driver, test_tokenchecker_4);
	UNITTEST_DRIVER_REGIST(driver, test_tokenchecker_5);
	UNITTEST_DRIVER_REGIST(driver, test_tokenchecker_6);
	UNITTEST_DRIVER_REGIST(driver, test_tokenchecker_7);
	UNITTEST_DRIVER_REGIST(driver, test_tokenchecker_8);
	UNITTEST_DRIVER_REGIST(driver, test_tokenchecker_9);
	UNITTEST_DRIVER_REGIST(driver, test_tokenchecker_10);
	UNITTEST_DRIVER_REGIST(driver, test_tokenchecker_11);
	UNITTEST_DRIVER_REGIST(driver, test_tokenchecker_12);
	UNITTEST_DRIVER_REGIST(driver, test_tokenchecker_13);
	UNITTEST_DRIVER_REGIST(driver, test_tokenchecker_14);
	UNITTEST_DRIVER_REGIST(driver, test_tokenchecker_15);
	UNITTEST_DRIVER_REGIST(driver, test_tokenchecker_16);
	UNITTEST_DRIVER_REGIST(driver, test_tokenchecker_17);
	UNITTEST_DRIVER_REGIST(driver, test_tokenchecker_18);
	UNITTEST_DRIVER_REGIST(driver, test_tokenchecker_19);
	UNITTEST_DRIVER_REGIST(driver, test_tokenchecker_20);
	UNITTEST_DRIVER_REGIST(driver, test_tokenchecker_21);
}

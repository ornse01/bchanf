/*
 * test_charreferparser.c
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

#include "test_parse.h"

#include "charreferparser.h"

#include	<basic.h>
#include	<bstdio.h>
#include	<bstdlib.h>
#include	<bstring.h>

#include    <unittest_driver.h>

LOCAL UNITTEST_RESULT test_character_reference_1()
{
	charreferparser_t parser;
	CHARREFERPARSER_RESULT parseresult;
	W err;

	err = charreferparser_initialize(&parser);
	if (err < 0) {
		return UNITTEST_RESULT_FAIL;
	}
	parseresult = charreferparser_parsechar(&parser, '&');
	if (parseresult != CHARREFERPARSER_RESULT_CONTINUE) {
		charreferparser_finalize(&parser);
		return UNITTEST_RESULT_FAIL;
	}
	parseresult = charreferparser_parsechar(&parser, '#');
	if (parseresult != CHARREFERPARSER_RESULT_CONTINUE) {
		charreferparser_finalize(&parser);
		return UNITTEST_RESULT_FAIL;
	}
	parseresult = charreferparser_parsechar(&parser, '1');
	if (parseresult != CHARREFERPARSER_RESULT_CONTINUE) {
		charreferparser_finalize(&parser);
		return UNITTEST_RESULT_FAIL;
	}
	parseresult = charreferparser_parsechar(&parser, '0');
	if (parseresult != CHARREFERPARSER_RESULT_CONTINUE) {
		charreferparser_finalize(&parser);
		return UNITTEST_RESULT_FAIL;
	}
	parseresult = charreferparser_parsechar(&parser, ';');
		charreferparser_finalize(&parser);
	if (parseresult != CHARREFERPARSER_RESULT_DETERMINE) {
		return UNITTEST_RESULT_FAIL;
	}
	err = charreferparser_getcharnumber(&parser);
	if (err < 0) {
		charreferparser_finalize(&parser);
		return UNITTEST_RESULT_FAIL;
	}
	if (err != 10) {
		charreferparser_finalize(&parser);
		return UNITTEST_RESULT_FAIL;
	}
	charreferparser_finalize(&parser);

	return UNITTEST_RESULT_PASS;
}

LOCAL W test_charrefer_parsestr(charreferparser_t *parser, UB *str)
{
	W i;
	CHARREFERPARSER_RESULT result;

	for (i = 0; str[i] != '\0'; i++) {
		result = charreferparser_parsechar(parser, str[i]);
		if (result == CHARREFERPARSER_RESULT_INVALID) {
			return -1;
		}
		if (result == CHARREFERPARSER_RESULT_DETERMINE) {
			return 0;
		}
	}
	return -1;
}

LOCAL UNITTEST_RESULT test_character_reference_2()
{
	W i;
	struct {
		UB *str;
		W val;
	} tuple[] = {
		{"&#0;", 0},
		{"&#1;", 1},
		{"&#2;", 2},
		{"&#3;", 3},
		{"&#4;", 4},
		{"&#5;", 5},
		{"&#6;", 6},
		{"&#7;", 7},
		{"&#8;", 8},
		{"&#9;", 9},
		{"&#10;", 10},
		{"&#11;", 11},
		{"&#12;", 12},
		{"&#13;", 13},
		{"&#14;", 14},
		{"&#15;", 15},
		{"&#16;", 16},
		{"&#17;", 17},
		{"&#18;", 18},
		{"&#19;", 19},
		{"&#20;", 20},
		{"&#x0;", 0x0},
		{"&#x1;", 0x1},
		{"&#x2;", 0x2},
		{"&#x3;", 0x3},
		{"&#x4;", 0x4},
		{"&#x5;", 0x5},
		{"&#x6;", 0x6},
		{"&#x7;", 0x7},
		{"&#x8;", 0x8},
		{"&#x9;", 0x9},
		{"&#xa;", 0xa},
		{"&#xb;", 0xb},
		{"&#xc;", 0xc},
		{"&#xd;", 0xd},
		{"&#xe;", 0xe},
		{"&#xf;", 0xf},
		{"&#xA;", 0xa},
		{"&#xB;", 0xb},
		{"&#xC;", 0xc},
		{"&#xD;", 0xd},
		{"&#xE;", 0xe},
		{"&#xF;", 0xf},
		{"&#x10;", 0x10},
		{"&#x11;", 0x11},
		{"&#x12;", 0x12},
		{"&#x13;", 0x13},
		{"&#x14;", 0x14},
		{"&#x15;", 0x15},
		{"&#xA0;", 0xA0},
		{"&#xB1;", 0xB1},
		{"&#xC2;", 0xC2},
		{"&#xD3;", 0xD3},
		{"&#xE4;", 0xE4},
		{"&#xF5;", 0xF5},
		{"&#xa0;", 0xA0},
		{"&#xb1;", 0xB1},
		{"&#xc2;", 0xC2},
		{"&#xd3;", 0xD3},
		{"&#xe4;", 0xE4},
		{"&#xf5;", 0xF5},
		{NULL, 0}
	};
	charreferparser_t parser;
	W err;

	for (i = 0; tuple[i].str != NULL; i++) {
		err = charreferparser_initialize(&parser);
		if (err < 0) {
			return UNITTEST_RESULT_FAIL;
		}
		err = test_charrefer_parsestr(&parser, tuple[i].str);
		if (err < 0) {
			printf("failed to parsing\n");
			return UNITTEST_RESULT_FAIL;
		}
		err = charreferparser_getcharnumber(&parser);
		if (err < 0) {
			printf("failed to get character number\n");
			charreferparser_finalize(&parser);
			return UNITTEST_RESULT_FAIL;
		}
		if (err != tuple[i].val) {
			printf("invalid parse result: %d(0x%x)\n", err, err);
			printf("                         <- %s\n", tuple[i].str);
			printf("                         <- %d\n", tuple[i].val);
			printf("                         <- 0x%x\n", tuple[i].val);
			charreferparser_finalize(&parser);
			return UNITTEST_RESULT_FAIL;
		}
		charreferparser_finalize(&parser);
	}

	return UNITTEST_RESULT_PASS;
}

LOCAL UNITTEST_RESULT test_character_reference_3()
{
	W i;
	struct {
		UB *str;
		W val;
	} tuple[] = {
		{"&amp;", '&'},
		{"&gt;", '>'},
		{"&lt;", '<'},
		{"&quot;", '"'},
		{NULL, 0}
	};
	charreferparser_t parser;
	W err;

	for (i = 0; tuple[i].str != NULL; i++) {
		err = charreferparser_initialize(&parser);
		if (err < 0) {
			return UNITTEST_RESULT_FAIL;
		}
		err = test_charrefer_parsestr(&parser, tuple[i].str);
		if (err < 0) {
			printf("failed to parsing\n");
			return UNITTEST_RESULT_FAIL;
		}
		err = charreferparser_getcharnumber(&parser);
		if (err < 0) {
			printf("failed to get character number\n");
			charreferparser_finalize(&parser);
			return UNITTEST_RESULT_FAIL;
		}
		if (err != tuple[i].val) {
			printf("invalid parse result: %d(0x%x)\n", err, err);
			printf("                         <- %s\n", tuple[i].str);
			printf("                         <- %d\n", tuple[i].val);
			printf("                         <- 0x%x\n", tuple[i].val);
			charreferparser_finalize(&parser);
			return UNITTEST_RESULT_FAIL;
		}
		charreferparser_finalize(&parser);
	}

	return UNITTEST_RESULT_PASS;
}

LOCAL UNITTEST_RESULT test_character_reference_4()
{
	W i, len, err;
	UB *str;
	UB data[] = "&aaa;";
	charreferparser_t parser;

	err = charreferparser_initialize(&parser);
	if (err < 0) {
		return UNITTEST_RESULT_FAIL;
	}
	for (i = 0; data[i] != '\0'; i++) {
		err = charreferparser_parsechar(&parser, data[i]);
		if (err == CHARREFERPARSER_RESULT_INVALID) {
			break;
		}
	}
	if (i != 2) {
		printf("failed to invalid position\n");
		charreferparser_finalize(&parser);
		return UNITTEST_RESULT_FAIL;
	}
	charreferparser_getlastmatchedstring(&parser, &str, &len);
	if (len != 1) {
		printf("failed to get last matched string length\n");
		charreferparser_finalize(&parser);
		return UNITTEST_RESULT_FAIL;
	}
	err = strncmp(data + 1, str, len);
	if (err != 0) {
		printf("failed to get last matched string\n");
		charreferparser_finalize(&parser);
		return UNITTEST_RESULT_FAIL;
	}
	charreferparser_finalize(&parser);

	return UNITTEST_RESULT_PASS;
}

LOCAL UNITTEST_RESULT test_character_reference_5()
{
	W i, len, err;
	UB *str;
	UB data[] = "&;";
	charreferparser_t parser;

	err = charreferparser_initialize(&parser);
	if (err < 0) {
		return UNITTEST_RESULT_FAIL;
	}
	for (i = 0; data[i] != '\0'; i++) {
		err = charreferparser_parsechar(&parser, data[i]);
		if (err == CHARREFERPARSER_RESULT_INVALID) {
			break;
		}
	}
	if (i != 1) {
		printf("failed to invalid position\n");
		charreferparser_finalize(&parser);
		return UNITTEST_RESULT_FAIL;
	}
	charreferparser_getlastmatchedstring(&parser, &str, &len);
	if (len != 0) {
		printf("failed to get last matched string length\n");
		charreferparser_finalize(&parser);
		return UNITTEST_RESULT_FAIL;
	}
	err = strncmp(data + 1, str, len);
	if (err != 0) {
		printf("failed to get last matched string\n");
		charreferparser_finalize(&parser);
		return UNITTEST_RESULT_FAIL;
	}
	charreferparser_finalize(&parser);

	return UNITTEST_RESULT_PASS;
}

LOCAL UNITTEST_RESULT test_character_reference_6()
{
	W i, len, err;
	UB *str;
	UB data[] = "&amm;";
	charreferparser_t parser;

	err = charreferparser_initialize(&parser);
	if (err < 0) {
		return UNITTEST_RESULT_FAIL;
	}
	for (i = 0; data[i] != '\0'; i++) {
		err = charreferparser_parsechar(&parser, data[i]);
		if (err == CHARREFERPARSER_RESULT_INVALID) {
			break;
		}
	}
	if (i != 3) {
		printf("failed to invalid position\n");
		charreferparser_finalize(&parser);
		return UNITTEST_RESULT_FAIL;
	}
	charreferparser_getlastmatchedstring(&parser, &str, &len);
	if (len != 2) {
		printf("failed to get last matched string length\n");
		charreferparser_finalize(&parser);
		return UNITTEST_RESULT_FAIL;
	}
	err = strncmp(data + 1, str, len);
	if (err != 0) {
		printf("failed to get last matched string\n");
		charreferparser_finalize(&parser);
		return UNITTEST_RESULT_FAIL;
	}
	charreferparser_finalize(&parser);

	return UNITTEST_RESULT_PASS;
}

LOCAL UNITTEST_RESULT test_character_reference_7()
{
	W i, len, err;
	UB *str;
	UB data[] = "&amps;";
	charreferparser_t parser;

	err = charreferparser_initialize(&parser);
	if (err < 0) {
		return UNITTEST_RESULT_FAIL;
	}
	for (i = 0; data[i] != '\0'; i++) {
		err = charreferparser_parsechar(&parser, data[i]);
		if (err == CHARREFERPARSER_RESULT_INVALID) {
			break;
		}
	}
	if (i != 4) {
		printf("failed to invalid position\n");
		charreferparser_finalize(&parser);
		return UNITTEST_RESULT_FAIL;
	}
	charreferparser_getlastmatchedstring(&parser, &str, &len);
	if (len != 3) {
		printf("failed to get last matched string length\n");
		charreferparser_finalize(&parser);
		return UNITTEST_RESULT_FAIL;
	}
	err = strncmp(data + 1, str, len);
	if (err != 0) {
		printf("failed to get last matched string\n");
		charreferparser_finalize(&parser);
		return UNITTEST_RESULT_FAIL;
	}
	charreferparser_finalize(&parser);

	return UNITTEST_RESULT_PASS;
}

LOCAL UNITTEST_RESULT test_character_reference_8()
{
	W i, len, err;
	UB *str;
	UB data[] = "&qqqq;";
	charreferparser_t parser;

	err = charreferparser_initialize(&parser);
	if (err < 0) {
		return UNITTEST_RESULT_FAIL;
	}
	for (i = 0; data[i] != '\0'; i++) {
		err = charreferparser_parsechar(&parser, data[i]);
		if (err == CHARREFERPARSER_RESULT_INVALID) {
			break;
		}
	}
	if (i != 2) {
		printf("failed to invalid position\n");
		charreferparser_finalize(&parser);
		return UNITTEST_RESULT_FAIL;
	}
	charreferparser_getlastmatchedstring(&parser, &str, &len);
	if (len != 1) {
		printf("failed to get last matched string length\n");
		charreferparser_finalize(&parser);
		return UNITTEST_RESULT_FAIL;
	}
	err = strncmp(data + 1, str, len);
	if (err != 0) {
		printf("failed to get last matched string\n");
		charreferparser_finalize(&parser);
		return UNITTEST_RESULT_FAIL;
	}
	charreferparser_finalize(&parser);

	return UNITTEST_RESULT_PASS;
}

LOCAL UNITTEST_RESULT test_character_reference_9()
{
	W i, len, err;
	UB *str;
	UB data[] = "&quuu;";
	charreferparser_t parser;

	err = charreferparser_initialize(&parser);
	if (err < 0) {
		return UNITTEST_RESULT_FAIL;
	}
	for (i = 0; data[i] != '\0'; i++) {
		err = charreferparser_parsechar(&parser, data[i]);
		if (err == CHARREFERPARSER_RESULT_INVALID) {
			break;
		}
	}
	if (i != 3) {
		printf("failed to invalid position\n");
		charreferparser_finalize(&parser);
		return UNITTEST_RESULT_FAIL;
	}
	charreferparser_getlastmatchedstring(&parser, &str, &len);
	if (len != 2) {
		printf("failed to get last matched string length\n");
		charreferparser_finalize(&parser);
		return UNITTEST_RESULT_FAIL;
	}
	err = strncmp(data + 1, str, len);
	if (err != 0) {
		printf("failed to get last matched string\n");
		charreferparser_finalize(&parser);
		return UNITTEST_RESULT_FAIL;
	}
	charreferparser_finalize(&parser);

	return UNITTEST_RESULT_PASS;
}

LOCAL UNITTEST_RESULT test_character_reference_10()
{
	W i, len, err;
	UB *str;
	UB data[] = "&quoo;";
	charreferparser_t parser;

	err = charreferparser_initialize(&parser);
	if (err < 0) {
		return UNITTEST_RESULT_FAIL;
	}
	for (i = 0; data[i] != '\0'; i++) {
		err = charreferparser_parsechar(&parser, data[i]);
		if (err == CHARREFERPARSER_RESULT_INVALID) {
			break;
		}
	}
	if (i != 4) {
		printf("failed to invalid position\n");
		charreferparser_finalize(&parser);
		return UNITTEST_RESULT_FAIL;
	}
	charreferparser_getlastmatchedstring(&parser, &str, &len);
	if (len != 3) {
		printf("failed to get last matched string length\n");
		charreferparser_finalize(&parser);
		return UNITTEST_RESULT_FAIL;
	}
	err = strncmp(data + 1, str, len);
	if (err != 0) {
		printf("failed to get last matched string\n");
		charreferparser_finalize(&parser);
		return UNITTEST_RESULT_FAIL;
	}
	charreferparser_finalize(&parser);

	return UNITTEST_RESULT_PASS;
}

LOCAL UNITTEST_RESULT test_character_reference_11()
{
	W i, len, err;
	UB *str;
	UB data[] = "&quott;";
	charreferparser_t parser;

	err = charreferparser_initialize(&parser);
	if (err < 0) {
		return UNITTEST_RESULT_FAIL;
	}
	for (i = 0; data[i] != '\0'; i++) {
		err = charreferparser_parsechar(&parser, data[i]);
		if (err == CHARREFERPARSER_RESULT_INVALID) {
			break;
		}
	}
	if (i != 5) {
		printf("failed to invalid position\n");
		charreferparser_finalize(&parser);
		return UNITTEST_RESULT_FAIL;
	}
	charreferparser_getlastmatchedstring(&parser, &str, &len);
	if (len != 4) {
		printf("failed to get last matched string length\n");
		charreferparser_finalize(&parser);
		return UNITTEST_RESULT_FAIL;
	}
	err = strncmp(data + 1, str, len);
	if (err != 0) {
		printf("failed to get last matched string\n");
		charreferparser_finalize(&parser);
		return UNITTEST_RESULT_FAIL;
	}
	charreferparser_finalize(&parser);

	return UNITTEST_RESULT_PASS;
}

EXPORT VOID test_charreferparser_main(unittest_driver_t *driver)
{
	UNITTEST_DRIVER_REGIST(driver, test_character_reference_1);
	UNITTEST_DRIVER_REGIST(driver, test_character_reference_2);
	UNITTEST_DRIVER_REGIST(driver, test_character_reference_3);
	UNITTEST_DRIVER_REGIST(driver, test_character_reference_4);
	UNITTEST_DRIVER_REGIST(driver, test_character_reference_5);
	UNITTEST_DRIVER_REGIST(driver, test_character_reference_6);
	UNITTEST_DRIVER_REGIST(driver, test_character_reference_7);
	UNITTEST_DRIVER_REGIST(driver, test_character_reference_8);
	UNITTEST_DRIVER_REGIST(driver, test_character_reference_9);
	UNITTEST_DRIVER_REGIST(driver, test_character_reference_10);
	UNITTEST_DRIVER_REGIST(driver, test_character_reference_11);
}

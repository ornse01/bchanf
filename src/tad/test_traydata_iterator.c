/*
 * test_tadlexer_le.c
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

#include "test_tad.h"

#include "traydata_iterator.h"

#include	<basic.h>
#include	<bstdio.h>
#include	<bstdlib.h>
#include	<bstring.h>

#include    <unittest_driver.h>

LOCAL UB test_traydata_testdata01[] = {
	0xe0, 0xff, 0x06, 0x00, 0x00, 0x00, 0x02, 0x00,
	0x22, 0x01, 0xe1, 0xff, 0x18, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x88, 0xff,
	0x88, 0xff, 0x21, 0x00, 0x00, 0x00, 0xa0, 0xff,
	0x0e, 0x00, 0x00, 0x00, 0x7b, 0x05, 0xe0, 0x03,
	0x5e, 0x00, 0x46, 0x00, 0x6c, 0x00, 0x55, 0x00,
	0xa0, 0xff, 0x0a, 0x00, 0x00, 0x01, 0x5e, 0x00,
	0x76, 0x00, 0x6c, 0x00, 0x55, 0x00, 0xa0, 0xff,
	0x22, 0x00, 0x00, 0x03, 0xa0, 0xff, 0x02, 0x00,
	0x00, 0x08, 0xa1, 0xff, 0x04, 0x00, 0x00, 0x00,
	0x00, 0x80, 0xa1, 0xff, 0x02, 0x00, 0x01, 0x01,
	0x5d, 0x21, 0xad, 0xff, 0x04, 0x00, 0x00, 0x00,
	0xc8, 0x00, 0x5d, 0x21, 0xa0, 0xff, 0x04, 0x00,
	0x00, 0x04, 0x00, 0x80, 0xa4, 0xff, 0x24, 0x00,
	0x11, 0x08, 0x22, 0x21, 0x23, 0x21, 0x24, 0x21,
	0x25, 0x21, 0x2b, 0x21, 0x2c, 0x21, 0x47, 0x21,
	0x49, 0x21, 0x4b, 0x21, 0x4d, 0x21, 0x4f, 0x21,
	0x51, 0x21, 0x53, 0x21, 0x55, 0x21, 0x57, 0x21,
	0x59, 0x21, 0x5b, 0x21, 0xa4, 0xff, 0x18, 0x00,
	0x11, 0x09, 0x46, 0x21, 0x48, 0x21, 0x4a, 0x21,
	0x4c, 0x21, 0x4e, 0x21, 0x50, 0x21, 0x52, 0x21,
	0x54, 0x21, 0x56, 0x21, 0x58, 0x21, 0x5a, 0x21,
	0xa1, 0xff, 0x24, 0x00, 0x00, 0x02, 0x04, 0x01,
	0x04, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x0b, 0x00, 0x48, 0x00, 0x90, 0x00, 0xd8, 0x00,
	0x20, 0x01, 0x68, 0x01, 0xb0, 0x01, 0xf8, 0x01,
	0x40, 0x02, 0x88, 0x02, 0xd0, 0x02, 0x18, 0x03,
	0xa1, 0xff, 0x04, 0x00, 0x01, 0x00, 0x04, 0x01,
	0x46, 0x23, 0x52, 0x23, 0x4f, 0x23, 0x4d, 0x23,
	0x27, 0x21, 0x21, 0x21, 0x6e, 0x23, 0x61, 0x23,
	0x6d, 0x23, 0x65, 0x23, 0x0a, 0x00, 0x6d, 0x23,
	0x61, 0x23, 0x69, 0x23, 0x6c, 0x23, 0x27, 0x21,
	0x21, 0x21, 0x6d, 0x23, 0x61, 0x23, 0x69, 0x23,
	0x6c, 0x23, 0x0a, 0x00, 0x0a, 0x00, 0x62, 0x23,
	0x6f, 0x23, 0x64, 0x23, 0x79, 0x23, 0xe2, 0xff,
	0x00, 0x00,
};

LOCAL UB testdata_infoseg[] = {0x00, 0x00, 0x02, 0x00, 0x22, 0x01};
LOCAL UB testdata_textseg[] = {
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x88, 0xff, 0x88, 0xff, 0x21, 0x00, 0x00, 0x00
};
LOCAL struct {
	VLINK vlnk;
	VOBJSEG vseg;
	UB data[4];
} testdata_vobjrec = {
	{
		{0x2341, 0x2342, TNULL},
		0, 1, 2,
		{1, 2, 3}
	},
	{
		{{0, 1, 100, 101}},
		100,
		-1,
		0x10ffffff,
		0x10ffffff,
		0x10ffffff,
		0x10ffffff,
		4
	},
	{1, 2, 3, 4}
};
LOCAL TC testdata_text[] = {0x2346, 0x2352, 0x234f, 0x234d};

typedef struct {
	enum {
		TEST_TRAYDATA_ITERATOR_CH,
		TEST_TRAYDATA_ITERATOR_SEGMENT,
		TEST_TRAYDATA_ITERATOR_VOBJ
	} type;
	TC ch;
	UB segid;
	W len;
} test_traydata_iterator_expected_t;

LOCAL test_traydata_iterator_expected_t test_traydata_expected01[] = {
	{TEST_TRAYDATA_ITERATOR_SEGMENT, 0, 0xe0, 6},
	{TEST_TRAYDATA_ITERATOR_SEGMENT, 0, 0xe1, 24},
	{TEST_TRAYDATA_ITERATOR_SEGMENT, 0, 0xa0, 14},
	{TEST_TRAYDATA_ITERATOR_SEGMENT, 0, 0xa0, 10},
	{TEST_TRAYDATA_ITERATOR_SEGMENT, 0, 0xa0, 34},
	{TEST_TRAYDATA_ITERATOR_SEGMENT, 0, 0xa0, 4},
	{TEST_TRAYDATA_ITERATOR_SEGMENT, 0, 0xa4, 36},
	{TEST_TRAYDATA_ITERATOR_SEGMENT, 0, 0xa4, 24},
	{TEST_TRAYDATA_ITERATOR_SEGMENT, 0, 0xa1, 36},
	{TEST_TRAYDATA_ITERATOR_SEGMENT, 0, 0xa1, 4},
	{TEST_TRAYDATA_ITERATOR_CH, 0x2346, 0, 0},
	{TEST_TRAYDATA_ITERATOR_CH, 0x2352, 0, 0},
	{TEST_TRAYDATA_ITERATOR_CH, 0x234f, 0, 0},
	{TEST_TRAYDATA_ITERATOR_CH, 0x234d, 0, 0},
	{TEST_TRAYDATA_ITERATOR_CH, 0x2127, 0, 0},
	{TEST_TRAYDATA_ITERATOR_CH, 0x2121, 0, 0},
	{TEST_TRAYDATA_ITERATOR_CH, 0x236e, 0, 0},
	{TEST_TRAYDATA_ITERATOR_CH, 0x2361, 0, 0},
	{TEST_TRAYDATA_ITERATOR_CH, 0x236d, 0, 0},
	{TEST_TRAYDATA_ITERATOR_CH, 0x2365, 0, 0},
	{TEST_TRAYDATA_ITERATOR_CH, 0x000a, 0, 0},
	{TEST_TRAYDATA_ITERATOR_CH, 0x236d, 0, 0},
	{TEST_TRAYDATA_ITERATOR_CH, 0x2361, 0, 0},
	{TEST_TRAYDATA_ITERATOR_CH, 0x2369, 0, 0},
	{TEST_TRAYDATA_ITERATOR_CH, 0x236c, 0, 0},
	{TEST_TRAYDATA_ITERATOR_CH, 0x2127, 0, 0},
	{TEST_TRAYDATA_ITERATOR_CH, 0x2121, 0, 0},
	{TEST_TRAYDATA_ITERATOR_CH, 0x236d, 0, 0},
	{TEST_TRAYDATA_ITERATOR_CH, 0x2361, 0, 0},
	{TEST_TRAYDATA_ITERATOR_CH, 0x2369, 0, 0},
	{TEST_TRAYDATA_ITERATOR_CH, 0x236c, 0, 0},
	{TEST_TRAYDATA_ITERATOR_CH, 0x000a, 0, 0},
	{TEST_TRAYDATA_ITERATOR_CH, 0x000a, 0, 0},
	{TEST_TRAYDATA_ITERATOR_CH, 0x2362, 0, 0},
	{TEST_TRAYDATA_ITERATOR_CH, 0x236f, 0, 0},
	{TEST_TRAYDATA_ITERATOR_CH, 0x2364, 0, 0},
	{TEST_TRAYDATA_ITERATOR_CH, 0x2379, 0, 0},
	{TEST_TRAYDATA_ITERATOR_SEGMENT, 0, 0xe2, 0},
};

LOCAL test_traydata_iterator_expected_t test_traydata_expected02[] = {
	{TEST_TRAYDATA_ITERATOR_SEGMENT, 0, 0xe0, 6},
	{TEST_TRAYDATA_ITERATOR_SEGMENT, 0, 0xe1, 24},
	{TEST_TRAYDATA_ITERATOR_VOBJ, 0, 0, sizeof(testdata_vobjrec)},
	{TEST_TRAYDATA_ITERATOR_CH, 0x2346, 0, 0},
	{TEST_TRAYDATA_ITERATOR_CH, 0x2352, 0, 0},
	{TEST_TRAYDATA_ITERATOR_CH, 0x234f, 0, 0},
	{TEST_TRAYDATA_ITERATOR_CH, 0x234d, 0, 0},
};

LOCAL UNITTEST_RESULT test_traydata_iterator_common(TRAYREC *rec, W len, test_traydata_iterator_expected_t *expected, W expected_len)
{
	traydata_iterator_t iter;
	traydata_iterator_result result;
	Bool cont;
	UNITTEST_RESULT ret = UNITTEST_RESULT_PASS;
	W j, sz = 0;

	traydata_iterator_initialize(&iter, rec, len);

	j = 0;
	for (;;) {
		cont = traydata_iterator_next(&iter, &result);
		if (cont == False) {
			break;
		}
		if (result.type == TRAYDATA_ITERATOR_RESULTTYPE_FIXED_SEGMENT) {
			if (j < expected_len) {
				if (expected[j].type == TEST_TRAYDATA_ITERATOR_CH) {
					if (expected[j].ch != result.val.ch) {
						printf("character is not expected: expected %04x, result %04x\n", expected[j].ch, result.val.ch);
						ret = UNITTEST_RESULT_FAIL;
					}
				} else {
					printf("segment type is not expected: expected ch\n");
					ret = UNITTEST_RESULT_FAIL;
				}
			} else {
				ret = UNITTEST_RESULT_FAIL;
			}
			j++;
		} else if (result.type == TRAYDATA_ITERATOR_RESULTTYPE_VARIABLE_SEGMENT_CONT) {
			if (j < expected_len) {
				if (expected[j].type == TEST_TRAYDATA_ITERATOR_SEGMENT) {
					if (expected[j].segid != result.val.seg.id) {
						printf("devided segment id is not expected: expected = %02x, result = %02x\n", expected[j].segid, result.val.seg.id);
						ret = UNITTEST_RESULT_FAIL;
					}
				} else {
					printf("segment type is not expected: expected segment\n");
					ret = UNITTEST_RESULT_FAIL;
				}
			} else {
				ret = UNITTEST_RESULT_FAIL;
			}
			sz += result.val.seg.chunk_data_len;
		} else if (result.type == TRAYDATA_ITERATOR_RESULTTYPE_VARIABLE_SEGMENT) {
			sz += result.val.seg.chunk_data_len;
			if (j < expected_len) {
				if (expected[j].type == TEST_TRAYDATA_ITERATOR_SEGMENT) {
					if (expected[j].segid != result.val.seg.id) {
						printf("segment id is not expected: expected = %02x, result = %02x\n", expected[j].segid, result.val.seg.id);
						ret = UNITTEST_RESULT_FAIL;
					}
					if (expected[j].len != sz) {
						printf("segment size is not expected: expected = %d, result = %d(%d)\n", expected[j].len, result.val.seg.seglen, sz);
						ret = UNITTEST_RESULT_FAIL;
					}
				} else {
					printf("segment type is not expected: expected segment\n");
					ret = UNITTEST_RESULT_FAIL;
				}
			} else {
				ret = UNITTEST_RESULT_FAIL;
			}
			j++;
			sz = 0;
		} else if (result.type == TRAYDATA_ITERATOR_RESULTTYPE_VOBJREC) {
			if (j < expected_len) {
				if (expected[j].type == TEST_TRAYDATA_ITERATOR_VOBJ) {
				} else {
					printf("segment type is not expected: expected vobj\n");
					ret = UNITTEST_RESULT_FAIL;
				}
			} else {
				ret = UNITTEST_RESULT_FAIL;
			}
			j++;
		} else if (result.type == TRAYDATA_ITERATOR_RESULTTYPE_END) {
			break;
		}
	}

	traydata_iterator_finalize(&iter);

	if (j != expected_len) {
		printf("segument number is not expected: len = %d, expected = %d\n", j, expected_len);
		ret = UNITTEST_RESULT_FAIL;
	}

	return ret;
}

LOCAL UNITTEST_RESULT test_traydata_1()
{
	TRAYREC rec[] = {
		{TS_INFO, 6, test_traydata_testdata01+4},
		{TR_TEXT, sizeof(test_traydata_testdata01) - 10, test_traydata_testdata01+10},
	};
	W len = sizeof(rec)/sizeof(TRAYREC);
	test_traydata_iterator_expected_t *expected = test_traydata_expected01;
	W expected_len = sizeof(test_traydata_expected01)/sizeof(test_traydata_iterator_expected_t);
	return test_traydata_iterator_common(rec, len, expected, expected_len);
}

LOCAL UNITTEST_RESULT test_traydata_2()
{
	TRAYREC rec[] = {
		{TS_INFO, 6, test_traydata_testdata01+4},
		{TR_TEXT|TR_CONT, 100, test_traydata_testdata01+10},
		{TR_TEXT, sizeof(test_traydata_testdata01) - 10 - 100, test_traydata_testdata01+10 + 100},
	};
	W len = sizeof(rec)/sizeof(TRAYREC);
	test_traydata_iterator_expected_t *expected = test_traydata_expected01;
	W expected_len = sizeof(test_traydata_expected01)/sizeof(test_traydata_iterator_expected_t);
	return test_traydata_iterator_common(rec, len, expected, expected_len);
}

LOCAL UNITTEST_RESULT test_traydata_3()
{
	TRAYREC rec[] = {
		{TS_INFO, 6, test_traydata_testdata01+4},
		{TR_TEXT|TR_CONT, 100, test_traydata_testdata01+10},
		{TR_TEXT|TR_CONT, 1, test_traydata_testdata01+10+100},
		{TR_TEXT|TR_CONT, 1, test_traydata_testdata01+10+100+1},
		{TR_TEXT|TR_CONT, 1, test_traydata_testdata01+10+100+2},
		{TR_TEXT|TR_CONT, 1, test_traydata_testdata01+10+100+3},
		{TR_TEXT|TR_CONT, 1, test_traydata_testdata01+10+100+4},
		{TR_TEXT|TR_CONT, 1, test_traydata_testdata01+10+100+5},
		{TR_TEXT|TR_CONT, 1, test_traydata_testdata01+10+100+6},
		{TR_TEXT|TR_CONT, 1, test_traydata_testdata01+10+100+7},
		{TR_TEXT|TR_CONT, 1, test_traydata_testdata01+10+100+8},
		{TR_TEXT|TR_CONT, 1, test_traydata_testdata01+10+100+9},
		{TR_TEXT, sizeof(test_traydata_testdata01) - 10 - 100 - 10, test_traydata_testdata01+10 + 100 + 10},
	};
	W len = sizeof(rec)/sizeof(TRAYREC);
	test_traydata_iterator_expected_t *expected = test_traydata_expected01;
	W expected_len = sizeof(test_traydata_expected01)/sizeof(test_traydata_iterator_expected_t);
	return test_traydata_iterator_common(rec, len, expected, expected_len);
}

LOCAL UNITTEST_RESULT test_traydata_4()
{
	TRAYREC rec[] = {
		{TS_INFO, 6, testdata_infoseg},
		{TS_TEXT, 24, testdata_textseg},
		{TR_VOBJ, sizeof(testdata_vobjrec), (UB*)&testdata_vobjrec},
		{TR_TEXT, sizeof(testdata_text), (UB*)testdata_text},
	};
	W len = sizeof(rec)/sizeof(TRAYREC);
	test_traydata_iterator_expected_t *expected = test_traydata_expected02;
	W expected_len = sizeof(test_traydata_expected02)/sizeof(test_traydata_iterator_expected_t);
	return test_traydata_iterator_common(rec, len, expected, expected_len);
}

LOCAL UNITTEST_RESULT test_traydata_5()
{
	TRAYREC rec[] = {
		{TS_INFO, 6, testdata_infoseg},
		{TS_TEXT, 24, testdata_textseg},
		{TR_VOBJ|TR_CONT, 10, (UB*)&testdata_vobjrec},
		{TR_VOBJ, sizeof(testdata_vobjrec) - 10, ((UB*)&testdata_vobjrec)+10},
		{TR_TEXT, sizeof(testdata_text), (UB*)testdata_text},
	};
	W len = sizeof(rec)/sizeof(TRAYREC);
	test_traydata_iterator_expected_t *expected = test_traydata_expected02;
	W expected_len = sizeof(test_traydata_expected02)/sizeof(test_traydata_iterator_expected_t);
	return test_traydata_iterator_common(rec, len, expected, expected_len);
}

LOCAL UNITTEST_RESULT test_traydata_6()
{
	TRAYREC rec[] = {
		{TS_INFO, 6, testdata_infoseg},
		{TS_TEXT, 24, testdata_textseg},
		{TR_VOBJ|TR_CONT, 10, (UB*)&testdata_vobjrec},
		{TR_VOBJ|TR_CONT, 15, ((UB*)&testdata_vobjrec)+10},
		{TR_VOBJ, sizeof(testdata_vobjrec) - 15, ((UB*)&testdata_vobjrec)+15},
		{TR_TEXT, sizeof(testdata_text), (UB*)testdata_text},
	};
	W len = sizeof(rec)/sizeof(TRAYREC);
	test_traydata_iterator_expected_t *expected = test_traydata_expected02;
	W expected_len = sizeof(test_traydata_expected02)/sizeof(test_traydata_iterator_expected_t);
	return test_traydata_iterator_common(rec, len, expected, expected_len);
}

LOCAL UNITTEST_RESULT test_traydata_7()
{
	TRAYREC rec[] = {
		{TS_INFO, 6, testdata_infoseg},
		{TS_TEXT|TR_CONT, 10, testdata_textseg},
		{TS_TEXT, 24-10, testdata_textseg+10},
		{TR_VOBJ, sizeof(testdata_vobjrec), (UB*)&testdata_vobjrec},
		{TR_TEXT, sizeof(testdata_text), (UB*)testdata_text},
	};
	W len = sizeof(rec)/sizeof(TRAYREC);
	test_traydata_iterator_expected_t *expected = test_traydata_expected02;
	W expected_len = sizeof(test_traydata_expected02)/sizeof(test_traydata_iterator_expected_t);
	return test_traydata_iterator_common(rec, len, expected, expected_len);
}

LOCAL UNITTEST_RESULT test_traydata_8()
{
	TRAYREC rec[] = {
		{TS_INFO, 6, testdata_infoseg},
		{TS_TEXT|TR_CONT, 10, testdata_textseg},
		{TS_TEXT|TR_CONT, 10, testdata_textseg+10},
		{TS_TEXT, 24-20, testdata_textseg+20},
		{TR_VOBJ, sizeof(testdata_vobjrec), (UB*)&testdata_vobjrec},
		{TR_TEXT, sizeof(testdata_text), (UB*)testdata_text},
	};
	W len = sizeof(rec)/sizeof(TRAYREC);
	test_traydata_iterator_expected_t *expected = test_traydata_expected02;
	W expected_len = sizeof(test_traydata_expected02)/sizeof(test_traydata_iterator_expected_t);
	return test_traydata_iterator_common(rec, len, expected, expected_len);
}

EXPORT VOID test_traydata_iterator_main(unittest_driver_t *driver)
{
	UNITTEST_DRIVER_REGIST(driver, test_traydata_1);
	UNITTEST_DRIVER_REGIST(driver, test_traydata_2);
	UNITTEST_DRIVER_REGIST(driver, test_traydata_3);
	UNITTEST_DRIVER_REGIST(driver, test_traydata_4);
	UNITTEST_DRIVER_REGIST(driver, test_traydata_5);
	UNITTEST_DRIVER_REGIST(driver, test_traydata_6);
	UNITTEST_DRIVER_REGIST(driver, test_traydata_7);
	UNITTEST_DRIVER_REGIST(driver, test_traydata_8);
}

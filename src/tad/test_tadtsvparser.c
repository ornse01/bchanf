/*
 * test_tadtsvparser.c
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

#include "tadtsvparser.h"

#include	<basic.h>
#include	<bstdio.h>
#include	<bstdlib.h>
#include	<bstring.h>

#include "taditerator.h"

#include    <unittest_driver.h>

LOCAL UB test_tadtsvparser_testdata01[] = {
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
	0xa2, 0xff, 0x04, 0x00, 0x00, 0x01, 0x00, 0x80,
	0xa1, 0xff, 0x04, 0x00, 0x01, 0x00, 0x04, 0x01,
	0x22, 0x24, 0x24, 0x24, 0x26, 0x24, 0x28, 0x24,
	0x2a, 0x24, 0x09, 0x00, 0x2b, 0x24, 0x2d, 0x24,
	0x2f, 0x24, 0x31, 0x24, 0x33, 0x24, 0x0a, 0x00,
	0x35, 0x24, 0x37, 0x24, 0x39, 0x24, 0x3b, 0x24,
	0x3d, 0x24, 0x0a, 0x00, 0x0a, 0x00, 0x3f, 0x24,
	0x41, 0x24, 0x44, 0x24, 0x46, 0x24, 0x48, 0x24,
	0x09, 0x00, 0x4a, 0x24, 0x4b, 0x24, 0x4c, 0x24,
	0x4d, 0x24, 0x4e, 0x24, 0x09, 0x00, 0x0a, 0x00,
	0x4f, 0x24, 0x52, 0x24, 0x55, 0x24, 0x58, 0x24,
	0x5b, 0x24, 0x09, 0x00, 0x5e, 0x24, 0x5f, 0x24,
	0x60, 0x24, 0x61, 0x24, 0x62, 0x24, 0x09, 0x00,
	0x64, 0x24, 0x66, 0x24, 0x68, 0x24, 0x09, 0x00,
	0xe2, 0xff, 0x00, 0x00,
};
LOCAL UB test_tadtsvparser_testdata02[] = {
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
	0xa2, 0xff, 0x04, 0x00, 0x00, 0x01, 0x00, 0x80,
	0xa1, 0xff, 0x04, 0x00, 0x01, 0x00, 0x04, 0x01,
	0x22, 0x24, 0x24, 0x24, 0x26, 0x24, 0x28, 0x24,
	0x2a, 0x24, 0x09, 0x00, 0x2b, 0x24, 0x2d, 0x24,
	0x2f, 0x24, 0x31, 0x24, 0x33, 0x24, 0x0a, 0x00,
	0x35, 0x24, 0x37, 0x24, 0x39, 0x24, 0x3b, 0x24,
	0x3d, 0x24, 0x0a, 0x00, 0x0a, 0x00, 0x3f, 0x24,
	0x41, 0x24, 0x44, 0x24, 0x46, 0x24, 0x48, 0x24,
	0x09, 0x00, 0x4a, 0x24, 0x4b, 0x24, 0x4c, 0x24,
	0x4d, 0x24, 0x4e, 0x24, 0x09, 0x00, 0x0a, 0x00,
	0x4f, 0x24, 0x52, 0x24, 0x55, 0x24, 0x58, 0x24,
	0x5b, 0x24, 0x09, 0x00, 0x5e, 0x24, 0x5f, 0x24,
	0x60, 0x24, 0x61, 0x24, 0x62, 0x24, 0x09, 0x00,
	0x64, 0x24, 0x66, 0x24, 0x68, 0x24, 0x09, 0x00,
	0x0a, 0x00, 0xe2, 0xff, 0x00, 0x00,
};
LOCAL UB test_tadtsvparser_testdata03[] = {
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
	0xa2, 0xff, 0x04, 0x00, 0x00, 0x01, 0x00, 0x80,
	0xa1, 0xff, 0x04, 0x00, 0x01, 0x00, 0x04, 0x01,
	0x22, 0x24, 0x24, 0x24, 0x26, 0x24, 0x28, 0x24,
	0x29, 0xfe, 0x21, 0x91, 0x21, 0xfe, 0x09, 0x00,
	0x2b, 0x24, 0x2d, 0x24, 0x2f, 0x24, 0x31, 0x24,
	0x33, 0x24, 0x0a, 0x00, 0x35, 0x24, 0x37, 0x24,
	0x39, 0x24, 0x3b, 0x24, 0x3d, 0x24, 0x0a, 0x00,
	0x0a, 0x00, 0x3f, 0x24, 0x41, 0x24, 0x44, 0x24,
	0x46, 0x24, 0x48, 0x24, 0x09, 0x00, 0x4a, 0x24,
	0x4b, 0x24, 0x4c, 0x24, 0x29, 0xfe, 0x21, 0x91,
	0x21, 0xfe, 0x4e, 0x24, 0x09, 0x00, 0x0a, 0x00,
	0x4f, 0x24, 0x52, 0x24, 0x55, 0x24, 0x58, 0x24,
	0x5b, 0x24, 0x09, 0x00, 0x5e, 0x24, 0x5f, 0x24,
	0x60, 0x24, 0x61, 0x24, 0x62, 0x24, 0x09, 0x00,
	0x64, 0x24, 0x66, 0x24, 0x68, 0x24, 0x09, 0x00,
	0x0a, 0x00, 0xe2, 0xff, 0x00, 0x00,
};
LOCAL UB test_tadtsvparser_testdata04[] = {
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
	0xa2, 0xff, 0x04, 0x00, 0x00, 0x01, 0x00, 0x80,
	0xa1, 0xff, 0x04, 0x00, 0x01, 0x00, 0x04, 0x01,
	0x22, 0x24, 0x24, 0x24, 0x26, 0x24, 0x28, 0x24,
	0x29, 0xfe, 0x21, 0x91, 0x21, 0xfe, 0x09, 0x00,
	0x2b, 0x24, 0x2d, 0x24, 0x2f, 0x24, 0x31, 0x24,
	0x33, 0x24, 0x0a, 0x00, 0x35, 0x24, 0x37, 0x24,
	0x39, 0x24, 0x3b, 0x24, 0x3d, 0x24, 0x0a, 0x00,
	0x0a, 0x00, 0x3f, 0x24, 0x41, 0x24, 0x44, 0x24,
	0x46, 0x24, 0x48, 0x24, 0x09, 0x00, 0x4a, 0x24,
	0x4b, 0x24, 0x4c, 0x24, 0x29, 0xfe, 0x21, 0x91,
	0x21, 0xfe, 0x4e, 0x24, 0x09, 0x00, 0x0a, 0x00,
	0x4f, 0x24, 0x52, 0x24, 0x55, 0x24, 0x58, 0x24,
	0x5b, 0x24, 0x09, 0x00, 0x5e, 0x24, 0x5f, 0x24,
	0x60, 0x24, 0x61, 0x24, 0x62, 0x24, 0x09, 0x00,
	0x64, 0x24, 0x66, 0x24, 0x68, 0x24, 0x09, 0x00,
	0xe2, 0xff, 0x00, 0x00,
};

LOCAL UNITTEST_RESULT test_tadstack_counttest_common(UB *testdata, W testdata_len, W *expected, W expected_len)
{
	tadtsvparser_t parser;
	taditerator_t iter;
	taditerator_result result;
	TADSTACK_RESULT psr_result;
	UNITTEST_RESULT ok = UNITTEST_RESULT_PASS;
	W n_record = 0, n_field = 1;

	tadtsvparser_initialize(&parser);
	taditerator_initialize(&iter, (TC*)testdata, testdata_len/sizeof(TC));

	for (;;) {
		taditerator_next2(&iter, &result);
		if (result.type == TADITERATOR_RESULTTYPE_END) {
			break;
		}
		if (result.type == TADITERATOR_RESULTTYPE_CHARCTOR) {
			
			psr_result = tadtsvparser_inputcharactor(&parser, result.segment);
		} else if (result.type == TADITERATOR_RESULTTYPE_SEGMENT) {
			psr_result = tadtsvparser_inputvsegment(&parser, result.segment, result.data, result.segsize);
		}
		if (psr_result == TADTSVPARSER_RESULT_FORMAT_ERROR) {
			ok = UNITTEST_RESULT_FAIL;
			break;
		}
		if (psr_result == TADTSVPARSER_RESULT_FIELD_END) {
			n_field++;
		}
		if (psr_result == TADTSVPARSER_RESULT_RECORD_END) {
			if (n_record >= expected_len) {
				ok = UNITTEST_RESULT_FAIL;
				printf("records number exceed expected number\n");
			} else if (expected[n_record] != n_field) {
				ok = UNITTEST_RESULT_FAIL;
				printf("record %d's fields is not expected: result = %d, expected = %d\n", n_record, n_field, expected[n_record]);
			}
			n_record++;
			n_field = 1;
		}
	}
	psr_result = tadtsvparser_inputendofdata(&parser);
	if (psr_result == TADTSVPARSER_RESULT_FORMAT_ERROR) {
		ok = UNITTEST_RESULT_FAIL;
	}
	if (psr_result == TADTSVPARSER_RESULT_FIELD_END) {
		n_field++;
	}
	if (psr_result == TADTSVPARSER_RESULT_RECORD_END) {
		if (n_record >= expected_len) {
			ok = UNITTEST_RESULT_FAIL;
			printf("records number exceed expected number\n");
		} else if (expected[n_record] != n_field) {
			ok = UNITTEST_RESULT_FAIL;
			printf("record %d's fields is not expected: result = %d, expected = %d\n", n_record, n_field, expected[n_record]);
		}
		n_record++;
		n_field = 1;
	}
	if (n_record != expected_len) {
		ok = UNITTEST_RESULT_FAIL;
		printf("records number is not expected: result = %d, expected = %d\n", n_record, expected_len);
	}

	taditerator_finalize(&iter);
	tadtsvparser_finalize(&parser);

	return ok;
}

LOCAL UNITTEST_RESULT test_tadtsvparser_1()
{
	UB *testdata = test_tadtsvparser_testdata01;
	W testdata_len = sizeof(test_tadtsvparser_testdata01);
	W expected[] = {2, 1, 1, 3, 4};
	W expected_len = 5;
	return test_tadstack_counttest_common(testdata, testdata_len, expected, expected_len);
}

LOCAL UNITTEST_RESULT test_tadtsvparser_2()
{
	UB *testdata = test_tadtsvparser_testdata02;
	W testdata_len = sizeof(test_tadtsvparser_testdata02);
	W expected[] = {2, 1, 1, 3, 4};
	W expected_len = 5;
	return test_tadstack_counttest_common(testdata, testdata_len, expected, expected_len);
}

LOCAL UNITTEST_RESULT test_tadtsvparser_3()
{
	UB *testdata = test_tadtsvparser_testdata03;
	W testdata_len = sizeof(test_tadtsvparser_testdata03);
	W expected[] = {2, 1, 1, 3, 4};
	W expected_len = 5;
	return test_tadstack_counttest_common(testdata, testdata_len, expected, expected_len);
}

LOCAL UNITTEST_RESULT test_tadtsvparser_4()
{
	UB *testdata = test_tadtsvparser_testdata04;
	W testdata_len = sizeof(test_tadtsvparser_testdata04);
	W expected[] = {2, 1, 1, 3, 4};
	W expected_len = 5;
	return test_tadstack_counttest_common(testdata, testdata_len, expected, expected_len);
}

EXPORT VOID test_tadtsvparser_main(unittest_driver_t *driver)
{
	UNITTEST_DRIVER_REGIST(driver, test_tadtsvparser_1);
	UNITTEST_DRIVER_REGIST(driver, test_tadtsvparser_2);
	UNITTEST_DRIVER_REGIST(driver, test_tadtsvparser_3);
	UNITTEST_DRIVER_REGIST(driver, test_tadtsvparser_4);
}

/*
 * test_tadstack.c
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

#include "tadstack.h"

#include	<basic.h>
#include	<bstdio.h>
#include	<bstdlib.h>
#include	<bstring.h>

#include "taditerator.h"

#include    <unittest_driver.h>

LOCAL UB test_tadstack_testdata01[] = {
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

LOCAL UNITTEST_RESULT test_tadstack_1()
{
	tadstack_t stack;
	taditerator_t iter;
	taditerator_result result;
	TADSTACK_RESULT stk_result;
	UNITTEST_RESULT ok = UNITTEST_RESULT_PASS;

	tadstack_initialize(&stack);
	taditerator_initialize(&iter, (TC*)test_tadstack_testdata01, sizeof(test_tadstack_testdata01)/sizeof(TC));

	for (;;) {
		taditerator_next2(&iter, &result);
		if (result.type == TADITERATOR_RESULTTYPE_END) {
			break;
		}
		if (result.type == TADITERATOR_RESULTTYPE_CHARCTOR) {
			
			stk_result = tadstack_inputcharactor(&stack, result.segment);
		} else if (result.type == TADITERATOR_RESULTTYPE_CHARCTOR) {
			stk_result = tadstack_inputvsegment(&stack, result.segment, result.data, result.segsize);
		}
		if (stk_result == TADSTACK_RESULT_FORMAT_ERROR) {
			ok = UNITTEST_RESULT_FAIL;
		}
	}

	taditerator_finalize(&iter);
	tadstack_finalize(&stack);

	return ok;
}

EXPORT VOID test_tadstack_main(unittest_driver_t *driver)
{
	UNITTEST_DRIVER_REGIST(driver, test_tadstack_1);
}

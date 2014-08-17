/*
 * test_texteditor_characterstate.c
 *
 * Copyright (c) 2014 project bchan
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

#include "tadsegment.h"

#include	<basic.h>
#include	<bstdio.h>

#include    <unittest_driver.h>

LOCAL UNITTEST_RESULT test_tadsegment_1()
{
	tadsegment segment;
	W err;
	UB segid;
	UW seglen;
	UB *segdata;
	UNITTEST_RESULT result = UNITTEST_RESULT_PASS;

	segment.type = TADSEGMENT_TYPE_VARIABLE;
	segment.value.variable.raw = (UB*)(TC[]){ 0xFFA2, 0x0006, 0x0300, 0x0101, 0x0101 };
	segment.value.variable.rawlen = 10;

	err = tadsegment_getvariable(&segment, &segid, &seglen, &segdata);
	if (err < 0) {
		return UNITTEST_RESULT_FAIL;
	}

	if (segid != 0xA2) {
		result = UNITTEST_RESULT_FAIL;
	}
	if (seglen != 6) {
		result = UNITTEST_RESULT_FAIL;
	}
	if (memcmp(segdata, segment.value.variable.raw + 6, seglen) != 0) {
		result = UNITTEST_RESULT_FAIL;
	}

	return UNITTEST_RESULT_PASS;
}

LOCAL UNITTEST_RESULT test_tadsegment_2()
{
	tadsegment segment;
	W err;
	UB segid;
	UW seglen;
	UB *segdata;
	UNITTEST_RESULT result = UNITTEST_RESULT_PASS;

	segment.type = TADSEGMENT_TYPE_VARIABLE;
	segment.value.variable.raw = (UB*)(TC[]){ 0xFFE5, 0xFFFF, 0xFF00, 0x0001 };
	segment.value.variable.rawlen = 0x1FF00 + 8;

	err = tadsegment_getvariable(&segment, &segid, &seglen, &segdata);
	if (err < 0) {
		return UNITTEST_RESULT_FAIL;
	}

	if (segid != 0xE5) {
		result = UNITTEST_RESULT_FAIL;
	}
	if (seglen != 0x1FF00) {
		result = UNITTEST_RESULT_FAIL;
	}

	return UNITTEST_RESULT_PASS;
}

LOCAL UNITTEST_RESULT test_tadsegment_3()
{
	tadsegment segment;
	W err;
	UB segid;
	UW seglen;
	UB *segdata;

	segment.type = TADSEGMENT_TYPE_CHARACTOR;

	err = tadsegment_getvariable(&segment, &segid, &seglen, &segdata);
	if (err < 0) {
		return UNITTEST_RESULT_PASS;
	}

	return UNITTEST_RESULT_FAIL;
}

LOCAL UNITTEST_RESULT test_tadsegment_4()
{
	tadsegment segment;
	W err;
	UB segid;
	UW seglen;
	UB *segdata;

	segment.type = TADSEGMENT_TYPE_LANGCODE;

	err = tadsegment_getvariable(&segment, &segid, &seglen, &segdata);
	if (err < 0) {
		return UNITTEST_RESULT_PASS;
	}

	return UNITTEST_RESULT_FAIL;
}

EXPORT VOID test_tadsegment_main(unittest_driver_t *driver)
{
	UNITTEST_DRIVER_REGIST(driver, test_tadsegment_1);
	UNITTEST_DRIVER_REGIST(driver, test_tadsegment_2);
	UNITTEST_DRIVER_REGIST(driver, test_tadsegment_3);
	UNITTEST_DRIVER_REGIST(driver, test_tadsegment_4);
}

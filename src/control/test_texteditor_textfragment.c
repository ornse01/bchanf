/*
 * test_texteditor_textfragment.c
 *
 * Copyright (c) 2013 project bchan
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

#include "test_control.h"

#include "texteditor_textfragment.h"

#include	<basic.h>
#include	<bstdio.h>
#include	<bstdlib.h>
#include	<bstring.h>

#include    <unittest_driver.h>

LOCAL UNITTEST_RESULT test_texteditor_textfragment_1()
{
	texteditor_textfragment_t fragment;
	W err;

	err = texteditor_textfragment_initialize(&fragment);
	if (err < 0) {
		return UNITTEST_RESULT_FAIL;
	}
	texteditor_textfragment_finalize(&fragment);

	return UNITTEST_RESULT_PASS;
}

EXPORT VOID test_texteditor_textfragment_main(unittest_driver_t *driver)
{
	UNITTEST_DRIVER_REGIST(driver, test_texteditor_textfragment_1);
}

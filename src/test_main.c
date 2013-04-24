/*
 * test_main.c
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

#include	<basic.h>
#include	<bstdlib.h>
#include	<bstdio.h>
#include	<errcode.h>
#include	<btron/btron.h>

#include    <unittest_driver.h>

#include    <test_http.h>
#include    <test_parse.h>
#include    <test_coll.h>
#include    <test_coding.h>
#include    <test_tad.h>


EXPORT	W	MAIN(MESSAGE *msg)
{
	unittest_driver_t *driver;

	driver = unittest_driver_new();
	if (driver == NULL) {
		return 0;
	}

	test_http_transferdecoder_main(driver);
	test_http_contentdecoder_main(driver);
	test_http_headerlexer_main(driver);
	test_http_statuslineparser_main(driver);
	test_http_requestlinestream_main(driver);
	test_http_defaultheaderstream_main(driver);
	test_http_defaultheaderparser_main(driver);

	test_tokenchecker_main(driver);
	test_charreferparser_main(driver);

	test_idtocb_main(driver);
	test_arraybase_main(driver);
	test_treebase_main(driver);

	test_base64encoder_main(driver);
	test_base64decoder_main(driver);

	test_taditerator_main(driver);
	test_tadstack_main(driver);
	test_tadtsvparser_main(driver);
	test_tadlexer_le_main(driver);
	test_traydata_iterator_main(driver);

	unittest_driver_runnning(driver);
	unittest_driver_delete(driver);

	return 0;
}

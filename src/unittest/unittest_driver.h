/*
 * unittest_driver.h
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

#include    <basic.h>

/* Vendor name: */
/* Functionality name: unittest */
/* Detail name: driver */

#ifndef __UNITTEST_DRIVER_H__
#define __UNITTEST_DRIVER_H__

/* Functionality name: unittest */
/* Data structure identifier: result */
typedef enum {
	UNITTEST_RESULT_PASS,
	UNITTEST_RESULT_FAIL
} UNITTEST_RESULT;

/* Functionality name: unittest */
/* Data structure identifier: driver */
typedef struct unittest_driver_t_ unittest_driver_t;

typedef UNITTEST_RESULT (*unittest_driver_testfunc)();

IMPORT unittest_driver_t* unittest_driver_new();
IMPORT VOID unittest_driver_delete(unittest_driver_t *driver);
IMPORT W unittest_driver_registfunc(unittest_driver_t *driver, UB *name, unittest_driver_testfunc testfunc);
#define UNITTEST_DRIVER_REGIST(driver, testfunc) unittest_driver_registfunc((driver), #testfunc, testfunc)
IMPORT W unittest_driver_runnning(unittest_driver_t *driver);

#endif

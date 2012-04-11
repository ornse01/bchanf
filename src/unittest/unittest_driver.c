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

#include "unittest_driver.h"

#include	<basic.h>
#include	<bstdio.h>
#include	<bstdlib.h>
#include	<bstring.h>
#include	<bctype.h>
#include	<errcode.h>
#include	<btron/btron.h>
#include	<bsys/queue.h>

#ifdef BCHAN_CONFIG_DEBUG
# define DP(arg) printf arg
# define DP_ER(msg, err) printf("%s (%d/%x)\n", msg, err>>16, err)
#else
# define DP(arg) /**/
# define DP_ER(msg, err) /**/
#endif

struct unittest_driverentry_t_ {
	QUEUE que;
	UB *name;
	W name_len;
	unittest_driver_testfunc testfunc;
	UNITTEST_RESULT result;
};
typedef struct unittest_driverentry_t_ unittest_driverentry_t;

LOCAL VOID unittest_driverentry_QueInsert(unittest_driverentry_t *entry, unittest_driverentry_t *que)
{
	QueInsert(&entry->que, &que->que);
}

LOCAL unittest_driverentry_t* unittest_driverentry_nextnode(unittest_driverentry_t *entry)
{
	return (unittest_driverentry_t*)entry->que.next;
}

LOCAL VOID unittest_driverentry_runtest(unittest_driverentry_t *entry)
{
	printf("%s\n", entry->name);
	printf("---------------------------------------------\n");
	entry->result = (*entry->testfunc)();
	if (entry->result == UNITTEST_RESULT_PASS) {
		printf("--pass---------------------------------------\n");
	} else {
		printf("--fail---------------------------------------\n");
	}
	printf("---------------------------------------------\n");
}

LOCAL unittest_driverentry_t* unittest_driverentry_new(UB *name, W name_len, unittest_driver_testfunc testfunc)
{
	unittest_driverentry_t *entry;

	entry = (unittest_driverentry_t *)malloc(sizeof(unittest_driverentry_t));
	if (entry == NULL) {
		return NULL;
	}
	entry->name = malloc(sizeof(UB)*(name_len+1));
	if (entry->name == NULL) {
		free(entry);
		return NULL;
	}
	memcpy(entry->name, name, sizeof(UB)*name_len);
	entry->name[name_len] = '\0';
	entry->name_len = name_len;

	entry->testfunc = testfunc;
	entry->result = UNITTEST_RESULT_FAIL;

	return entry;
}

LOCAL VOID unittest_driverentry_delete(unittest_driverentry_t *entry)
{
	free(entry->name);
	QueRemove(&entry->que);
	free(entry);
}

struct unittest_driver_t_ {
	QUEUE sentinel;
};

LOCAL unittest_driverentry_t* unittest_driver_sentinelnode(unittest_driver_t *driver)
{
	return (unittest_driverentry_t*)&driver->sentinel;
}

EXPORT W unittest_driver_registfunc(unittest_driver_t *driver, UB *name, unittest_driver_testfunc testfunc)
{
	unittest_driverentry_t *entry, *senti;

	entry = unittest_driverentry_new(name, strlen(name), testfunc);
	if (entry == NULL) {
		return -1; /* TODO */
	}
	senti = unittest_driver_sentinelnode(driver);
	unittest_driverentry_QueInsert(entry, senti);

	return 0;
}

EXPORT W unittest_driver_runnning(unittest_driver_t *driver)
{
	unittest_driverentry_t *senti, *next;
	Bool all = True;

	senti = unittest_driver_sentinelnode(driver);
	next = unittest_driverentry_nextnode(senti);
	for (;;) {
		if (senti == next) {
			break;
		}
		unittest_driverentry_runtest(next);
		next = unittest_driverentry_nextnode(next);
	}

	senti = unittest_driver_sentinelnode(driver);
	next = unittest_driverentry_nextnode(senti);
	for (;;) {
		if (senti == next) {
			break;
		}
		if (next->result == UNITTEST_RESULT_FAIL) {
			printf("failed: %s\n", next->name);
			all = False;
		}
		next = unittest_driverentry_nextnode(next);
	}

	if (all == True) {
		printf("all test is succeed\n");
	}

	return 0;
}

EXPORT unittest_driver_t* unittest_driver_new()
{
	unittest_driver_t *driver;

	driver = (unittest_driver_t*)malloc(sizeof(unittest_driver_t));
	if (driver == NULL) {
		return NULL;
	}

	QueInit(&driver->sentinel);

	return driver;
}

EXPORT VOID unittest_driver_delete(unittest_driver_t *driver)
{
	unittest_driverentry_t *senti, *entry;
	Bool empty;

	senti = unittest_driver_sentinelnode(driver);
	for (;;) {
		empty = isQueEmpty(&driver->sentinel);
		if (empty != False) {
			break;
		}
		entry = unittest_driverentry_nextnode(senti);
		unittest_driverentry_delete(entry);
	}

	free(driver);
}

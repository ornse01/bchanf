/*
 * hmi_windowscroll.h
 *
 * Copyright (c) 2011-2012 project bchan
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

/* Vendor name: */
/* Functionality name: hmi */
/* Detail name: windowscroll */

#include    <basic.h>
#include	<btron/dp.h>
#include	<btron/hmi.h>

#ifndef __HMI_WINDOWSCROLL_H__
#define __HMI_WINDOWSCROLL_H__

/* Functionality name: hmi */
/* Detail name: windowscroll */
/* Data structure identifier: */
struct hmi_windowscroll_t_ {
	PAID rbar;
	PAID bbar;
	W draw_l,draw_t,draw_r,draw_b;
	W work_l,work_t,work_r,work_b;
};
typedef struct hmi_windowscroll_t_ hmi_windowscroll_t;

IMPORT W hmi_windowscroll_initialize(hmi_windowscroll_t *wscr, WID target);
IMPORT VOID hmi_windowscroll_finalize(hmi_windowscroll_t *wscr);
IMPORT W hmi_windowscroll_updatebar(hmi_windowscroll_t *wscr);
IMPORT W hmi_windowscroll_weventrbar(hmi_windowscroll_t *wscr, WEVENT *wev, W *dh, W *dv);
IMPORT W hmi_windowscroll_weventbbar(hmi_windowscroll_t *wscr, WEVENT *wev, W *dh, W *dv);
IMPORT W hmi_windowscroll_scrollworkrect(hmi_windowscroll_t *wscr, W dh, W dv);
IMPORT W hmi_windowscroll_setworkrect(hmi_windowscroll_t *wscr, W l, W t, W r, W b);
IMPORT W hmi_windowscroll_setdrawrect(hmi_windowscroll_t *wscr, W l, W t, W r, W b);
IMPORT VOID hmi_windowscroll_settarget(hmi_windowscroll_t *wscr, WID target);

#endif

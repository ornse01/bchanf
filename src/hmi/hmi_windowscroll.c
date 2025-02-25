/*
 * hmi_windowscroll.c
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

#include    "hmi_windowscroll.h"

#include	<bstdio.h>
#include	<bstdlib.h>
#include	<btron/hmi.h>

#ifdef BCHAN_CONFIG_DEBUG
# define DP(arg) printf arg
# define DP_ER(msg, err) printf("%s (%d/%x)\n", msg, err>>16, err)
#else
# define DP(arg) /**/
# define DP_ER(msg, err) /**/
#endif

EXPORT W hmi_windowscroll_updaterbar(hmi_windowscroll_t *wscr)
{
	W sbarval[4],err;

	sbarval[0] = wscr->work_t;
	sbarval[1] = wscr->work_b;
	sbarval[2] = 0;
	sbarval[3] = wscr->draw_b - wscr->draw_t;
	if (sbarval[1] > sbarval[3]) {
		sbarval[1] = sbarval[3];
	}
	if((err = cset_val(wscr->rbar, 4, sbarval)) < 0){
		DP(("hmi_windowscroll_updaterbar:cset_val rbar\n"));
		DP(("                       :%d %d %d %d\n", sbarval[0], sbarval[1], sbarval[2], sbarval[3]));
		return err;
	}

	return 0;
}

LOCAL W hmi_windowscroll_updatebbar(hmi_windowscroll_t *wscr)
{
	W sbarval[4],err;

	sbarval[0] = wscr->work_r;
	sbarval[1] = wscr->work_l;
	sbarval[2] = wscr->draw_r - wscr->draw_l;
	sbarval[3] = 0;
	if (sbarval[0] > sbarval[2]) {
		sbarval[0] = sbarval[2];
	}
	if((err = cset_val(wscr->bbar, 4, sbarval)) < 0){
		DP(("hmi_windowscroll_updatebbar:cset_val bbar\n"));
		DP(("                       :%d %d %d %d\n", sbarval[0], sbarval[1], sbarval[2], sbarval[3]));
		return err;
    }

	return 0;
}

EXPORT W hmi_windowscroll_updatebar(hmi_windowscroll_t *wscr)
{
	W err;

	err = hmi_windowscroll_updaterbar(wscr);
	if (err < 0) {
		return err;
	}

	err = hmi_windowscroll_updatebbar(wscr);
	if (err < 0) {
		return err;
	}

	return 0;
}

/*
  return value
    <0 : error
    =0 : finish
    =1 : continue
 */
LOCAL W hmi_windowscroll_scrollbar(hmi_windowscroll_t *wscr, WEVENT *wev, PAID scrbarPAID, W *dh_ret, W *dv_ret)
{
	W i,err,barval[4];
	W *clo = barval,*chi = barval+1,*lo = barval+2,*hi = barval+3;
	RECT sbarRECT;
	W pressval,smoothscrolldiff;
	W dh,dv;
	WID wid;

	wid = wev->g.wid;

	*dh_ret = 0;
	*dv_ret = 0;

	for(;;) {
		if((i = cact_par(scrbarPAID, wev)) < 0) {
			DP_ER("cact_par error", i);
			return i;
		}
		cget_val(scrbarPAID, 4, barval);
		if((i & 0xc) == 0x8) { /*ジャンプ*/
			if (scrbarPAID == wscr->rbar) { /*右バー*/
				dh = 0;
				dv = -(*clo-wscr->work_t);
				wscr->work_t -= dv;
				wscr->work_b -= dv;
				*dh_ret = -dh;
				*dv_ret = -dv;
			} else if (scrbarPAID == wscr->bbar) { /*下バー*/
				dh = -(*chi-wscr->work_l);
				dv = 0;
				wscr->work_l -= dh;
				wscr->work_r -= dh;
				*dh_ret = -dh;
				*dv_ret = -dv;
			}
			if ((i & 0x6000) == 0x6000) {
				/*ジャンプ移動中の値の変更*/
				return 1;
			}
			return 0;
		}
		switch(i) {
			/*スムーススクロール*/
			/*プレス位置とノブの位置に比例した速度でスクロール*/
		case 0x6000:	/*上へのスムーススクロールで中断*/
		case 0x6001:	/*下へのスムーススクロールで中断*/
			if ((err = cget_pos(scrbarPAID, &sbarRECT)) < 0) {
				continue;
			}
			pressval = (wev->s.pos.y - sbarRECT.c.top)*
				(*hi - *lo)/(sbarRECT.c.bottom-sbarRECT.c.top) + *lo;
			smoothscrolldiff = (pressval - (*chi+*clo)/2)/5;
			if (smoothscrolldiff == 0) {
				continue;
			}
			if ((*clo + smoothscrolldiff) < *lo) {
				if (*lo >= *clo) {
					continue;
				}
				dh = 0;
				dv = -(*lo - *clo);
			} else if ((*chi + smoothscrolldiff) > *hi) {
				if (*hi <= *chi) {
					continue;
				}
				dh = 0;
				dv = -(*hi - *chi);
			} else {
				dh = 0;
				dv = -smoothscrolldiff;
			}
			wscr->work_t -= dv;
			wscr->work_b -= dv;
			*dh_ret = -dh;
			*dv_ret = -dv;
			hmi_windowscroll_updaterbar(wscr);
			return 1;
		case 0x6002:	/*左へのスムーススクロールで中断*/
		case 0x6003:	/*右へのスムーススクロールで中断*/
			if ((err = cget_pos(scrbarPAID, &sbarRECT)) < 0) {
				continue;
			}
			pressval = (wev->s.pos.x - sbarRECT.c.left)*
				(*lo - *hi)/(sbarRECT.c.right-sbarRECT.c.left) + *hi;
			smoothscrolldiff = (pressval - (*clo+*chi)/2)/5;
			if (smoothscrolldiff == 0) {
				continue;
			}
			if ((*clo + smoothscrolldiff) > *lo) {
				if (*lo <= *clo) {
					continue;
				}
				dh = -(*lo - *clo);
				dv = 0;
			} else if ((*chi + smoothscrolldiff) < *hi) {
				if (*hi >= *chi) {
					continue;
				}
				dh = -(*hi - *chi);
				dv = 0;
			} else {
				dh = -smoothscrolldiff;
				dv = 0;
			}
			wscr->work_l -= dh;
			wscr->work_r -= dh;
			*dh_ret = -dh;
			*dv_ret = -dv;
			hmi_windowscroll_updatebbar(wscr);
			return 1;
		case 0x5004:	/*上へのエリアスクロールで終了*/
			if ((wscr->work_t - (*chi-*clo)) < *lo) {
				dh = 0;
				dv = -(*lo - wscr->work_t);
			} else {
				dh = 0;
				dv = (*chi - *clo);
			}
			wscr->work_t -= dv;
			wscr->work_b -= dv;
			*dh_ret = -dh;
			*dv_ret = -dv;
			hmi_windowscroll_updaterbar(wscr);
			break;
		case 0x5005:	/*下へのエリアスクロールで終了*/
			if((wscr->work_b + (*chi-*clo)) > *hi){
				dh = 0;
				dv = -(*hi - wscr->work_b);
			} else {
				dh = 0;
				dv = -(*chi - *clo);
			}
			wscr->work_t -= dv;
			wscr->work_b -= dv;
			*dh_ret = -dh;
			*dv_ret = -dv;
			hmi_windowscroll_updaterbar(wscr);
			break;
		case 0x5006:	/*左へのエリアスクロールで終了*/
			if((wscr->work_l - (*clo-*chi)) < *hi){
				dh = -(*hi - wscr->work_l);
				dv = 0;
			} else {
				dh = *clo - *chi;
				dv = 0;
			}
			wscr->work_l -= dh;
			wscr->work_r -= dh;
			*dh_ret = -dh;
			*dv_ret = -dv;
			hmi_windowscroll_updatebbar(wscr);
			break;
		case 0x5007:	/*右へのエリアスクロールで終了*/
			if((wscr->work_r + (*clo-*chi)) > *lo){
				dh = -(*lo - wscr->work_r);
				dv = 0;
			} else {
				dh = -(*clo - *chi);
				dv = 0;
			}
			wscr->work_l -= dh;
			wscr->work_r -= dh;
			*dh_ret = -dh;
			*dv_ret = -dv;
			hmi_windowscroll_updatebbar(wscr);
			break;
		}
		return 0;
	}

	return 0;
}

EXPORT W hmi_windowscroll_weventrbar(hmi_windowscroll_t *wscr, WEVENT *wev, W *dh, W *dv)
{
	return hmi_windowscroll_scrollbar(wscr, wev, wscr->rbar, dh, dv);
}

EXPORT W hmi_windowscroll_weventbbar(hmi_windowscroll_t *wscr, WEVENT *wev, W *dh, W *dv)
{
	return hmi_windowscroll_scrollbar(wscr, wev, wscr->bbar, dh, dv);
}

EXPORT W hmi_windowscroll_scrollworkrect(hmi_windowscroll_t *wscr, W dh, W dv)
{
	wscr->work_l += dh;
	wscr->work_t += dv;
	wscr->work_r += dh;
	wscr->work_b += dv;
	return hmi_windowscroll_updatebar(wscr);
}

EXPORT W hmi_windowscroll_setworkrect(hmi_windowscroll_t *wscr, W l, W t, W r, W b)
{
	wscr->work_l = l;
	wscr->work_t = t;
	wscr->work_r = r;
	wscr->work_b = b;
	return hmi_windowscroll_updatebar(wscr);
}

EXPORT W hmi_windowscroll_setdrawrect(hmi_windowscroll_t *wscr, W l, W t, W r, W b)
{
	wscr->draw_l = l;
	wscr->draw_t = t;
	wscr->draw_r = r;
	wscr->draw_b = b;
	return hmi_windowscroll_updatebar(wscr);
}

EXPORT VOID hmi_windowscroll_settarget(hmi_windowscroll_t *wscr, WID target)
{
	if (target < 0) {
		wscr->rbar = -1;
		wscr->bbar = -1;
	} else {
		wget_bar(target, &(wscr->rbar), &(wscr->bbar), NULL);
		cchg_par(wscr->rbar, P_NORMAL|P_NOFRAME|P_ENABLE|P_ACT|P_DRAGBREAK);
		cchg_par(wscr->bbar, P_NORMAL|P_NOFRAME|P_ENABLE|P_ACT|P_DRAGBREAK);
	}
}

EXPORT W hmi_windowscroll_initialize(hmi_windowscroll_t *wscr, WID target)
{
	hmi_windowscroll_settarget(wscr, target);
	wscr->work_l = 0;
	wscr->work_t = 0;
	wscr->work_r = 0;
	wscr->work_b = 0;
	wscr->draw_l = 0;
	wscr->draw_t = 0;
	wscr->draw_r = 0;
	wscr->draw_b = 0;

	return 0;
}

EXPORT VOID hmi_windowscroll_finalize(hmi_windowscroll_t *wscr)
{
}

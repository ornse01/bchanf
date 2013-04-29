/*
 * cssmetric.h
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

#include <basic.h>

/* Vendor name: */
/* Functionality name: cssmetric */
/* Detail name: */

#ifndef __CSSMETRIC_H__
#define __CSSMETRIC_H__

struct cssmetric_point_t_ {
	W x;
	W y;
};
typedef struct cssmetric_point_t_ cssmetric_point_t;

union cssmetric_rectangle_t_ {
	struct {
		W	left;
		W	top;
		W	right;
		W	bottom;
	} c;
	struct {
		cssmetric_point_t	lefttop;
		cssmetric_point_t	rightbot;
	} p;
};
typedef union cssmetric_rectangle_t_ cssmetric_rectangle_t;

#endif

/*
 * base64encoder.h
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
/* Functionality name: base64encoder */
/* Detail name: */

#include    <basic.h>

/* rfc3548 3 Base 64 Encoding */

#ifndef __BASE64ENCODER_H__
#define __BASE64ENCODER_H__

/* Functionality name: base64encoder */
/* Detail name: */
struct base64encoder_t_ {
	enum {
		BASE64ENCODER_STATE_FIRSTBYTE,
		BASE64ENCODER_STATE_SECONDBYTE,
		BASE64ENCODER_STATE_THIRDBYTE,
	} state;
	UB buffer[3];
	UB save;
};
typedef struct base64encoder_t_ base64encoder_t;

IMPORT W base64encoder_initialize(base64encoder_t *encoder);
IMPORT VOID base64encoder_finalize(base64encoder_t *encoder);
IMPORT VOID base64encoder_inputchar(base64encoder_t *encoder, UB ch, UB **result, W *result_len);
IMPORT VOID base64encoder_endinput(base64encoder_t *encoder, UB **result, W *result_len);
IMPORT VOID base64encoder_clear(base64encoder_t *encoder);

#endif

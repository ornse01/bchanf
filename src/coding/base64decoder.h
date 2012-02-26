/*
 * base64decoder.h
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

/* Vendor name: */
/* Functionality name: base64decoder */
/* Detail name: */

#include    <basic.h>

/* rfc3548 3 Base 64 Encoding */

#ifndef __BASE64DECODER_H__
#define __BASE64DECODER_H__

/* Functionality name: base64decoder */
/* Detail name: */
struct base64decoder_t_ {
	enum {
		BASE64ENCODER_STATE_FIRSTCHAR,
		BASE64ENCODER_STATE_SECONDCHAR,
		BASE64ENCODER_STATE_THIRDCHAR,
		BASE64ENCODER_STATE_FORTHCHAR,
		BASE64ENCODER_STATE_FORTHCHAR_PAD,
		BASE64ENCODER_STATE_PAD,
	} state;
	UB buffer[3];
};
typedef struct base64decoder_t_ base64decoder_t;

IMPORT VOID base64decoder_initialize(base64decoder_t *decoder);
IMPORT VOID base64decoder_finalize(base64decoder_t *decoder);
IMPORT W base64decoder_inputchar(base64decoder_t *decoder, UB ch, UB **result, W *result_len);
IMPORT VOID base64decoder_endinput(base64decoder_t *decoder, UB **result, W *result_len);
IMPORT VOID base64decoder_clear(base64decoder_t *decoder);

#endif

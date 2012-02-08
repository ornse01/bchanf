/*
 * base64encoder.c
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

#include    "base64encoder.h"

#include	<basic.h>
#include	<bstdio.h>

#ifdef BCHAN_CONFIG_DEBUG
# define DP(arg) printf arg
# define DP_ER(msg, err) printf("%s (%d/%x)\n", msg, err>>16, err)
#else
# define DP(arg) /**/
# define DP_ER(msg, err) /**/
#endif

LOCAL UB encode_table[] = {"ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/"};

EXPORT VOID base64encoder_inputchar(base64encoder_t *encoder, UB ch, UB **result, W *result_len)
{
	switch (encoder->state) {
	case BASE64ENCODER_STATE_FIRSTBYTE:
		encoder->buffer[0] = encode_table[ch>>2];
		encoder->save = (ch & 0x3) << 4;
		*result = encoder->buffer;
		*result_len = 1;
		encoder->state = BASE64ENCODER_STATE_SECONDBYTE;
		break;
	case BASE64ENCODER_STATE_SECONDBYTE:
		encoder->buffer[0] = encode_table[encoder->save | (ch>>4)];
		encoder->save = (ch & 0xF) << 2;
		*result = encoder->buffer;
		*result_len = 1;
		encoder->state = BASE64ENCODER_STATE_THIRDBYTE;
		break;
	case BASE64ENCODER_STATE_THIRDBYTE:
		encoder->buffer[0] = encode_table[encoder->save | (ch>>6)];
		encoder->buffer[1] = encode_table[ch & 0x3F];
		*result = encoder->buffer;
		*result_len = 2;
		encoder->state = BASE64ENCODER_STATE_FIRSTBYTE;
		break;
	}
}

EXPORT VOID base64encoder_endinput(base64encoder_t *encoder, UB **result, W *result_len)
{
	switch (encoder->state) {
	case BASE64ENCODER_STATE_FIRSTBYTE:
		*result = NULL;
		*result_len = 0;
		break;
	case BASE64ENCODER_STATE_SECONDBYTE:
		encoder->buffer[0] = encode_table[encoder->save];
		encoder->buffer[1] = '=';
		encoder->buffer[2] = '=';
		*result = encoder->buffer;
		*result_len = 3;
		break;
	case BASE64ENCODER_STATE_THIRDBYTE:
		encoder->buffer[0] = encode_table[encoder->save];
		encoder->buffer[1] = '=';
		*result = encoder->buffer;
		*result_len = 2;
		break;
	}
}

EXPORT VOID base64encoder_clear(base64encoder_t *encoder)
{
	encoder->state = BASE64ENCODER_STATE_FIRSTBYTE;
}

EXPORT W base64encoder_initialize(base64encoder_t *encoder)
{
	encoder->state = BASE64ENCODER_STATE_FIRSTBYTE;
	return 0;
}

EXPORT VOID base64encoder_finalize(base64encoder_t *encoder)
{
}

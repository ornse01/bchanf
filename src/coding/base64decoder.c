/*
 * base64decoder.c
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

#include    "base64decoder.h"

#include	<basic.h>
#include	<bstdio.h>

#ifdef BCHAN_CONFIG_DEBUG
# define DP(arg) printf arg
# define DP_ER(msg, err) printf("%s (%d/%x)\n", msg, err>>16, err)
#else
# define DP(arg) /**/
# define DP_ER(msg, err) /**/
#endif

LOCAL W base64decoder_convertchar(UB ch)
{
	if (('A' <= ch)&&(ch <= 'Z')) {
		return ch - 'A' + 0;
	}
	if (('a' <= ch)&&(ch <= 'z')) {
		return ch - 'a' + 26;
	}
	if (('0' <= ch)&&(ch <= '9')) {
		return ch - '0' + 52;
	}
	if (ch == '+') {
		return 62;
	}
	if (ch == '/') {
		return 63;
	}
	if (ch == '=') {
		return -2;
	}
	return -1;
}

EXPORT W base64decoder_inputchar(base64decoder_t *decoder, UB ch, UB **result, W *result_len)
{
	W num = base64decoder_convertchar(ch);

	if (num == -1) {
		return -1; /* TODO: error value */
	}

	*result = NULL;
	*result_len = 0;

	switch (decoder->state) {
	case BASE64ENCODER_STATE_FIRSTCHAR:
		if (num == -2) { /* PAD */
			return -1;
		}
		decoder->buffer[0] = num << 2;
		decoder->state = BASE64ENCODER_STATE_SECONDCHAR;
		break;
	case BASE64ENCODER_STATE_SECONDCHAR:
		if (num == -2) { /* PAD */
			return -1;
		}
		decoder->buffer[0] |= (num >> 4);
		decoder->buffer[1] = (num & 0xF) << 4;
		decoder->state = BASE64ENCODER_STATE_THIRDCHAR;
		break;
	case BASE64ENCODER_STATE_THIRDCHAR:
		if (num == -2) { /* PAD */
			*result = decoder->buffer;
			*result_len = 1;
			decoder->state = BASE64ENCODER_STATE_FORTHCHAR_PAD;
		} else {
			decoder->buffer[1] |= (num >> 2);
			decoder->buffer[2] = (num & 0x3) << 6;
			decoder->state = BASE64ENCODER_STATE_FORTHCHAR;
		}
		break;
	case BASE64ENCODER_STATE_FORTHCHAR:
		if (num == -2) { /* PAD */
			*result = decoder->buffer;
			*result_len = 2;
			decoder->state = BASE64ENCODER_STATE_PAD;
		} else {
			decoder->buffer[2] |= num;
			decoder->state = BASE64ENCODER_STATE_FIRSTCHAR;
			*result = decoder->buffer;
			*result_len = 3;
		}
		break;
	case BASE64ENCODER_STATE_FORTHCHAR_PAD:
		if (num != -2) { /* PAD */
			return -1;
		}
		decoder->state = BASE64ENCODER_STATE_PAD;
		break;
	case BASE64ENCODER_STATE_PAD:
		return -1;
	}

	return 0;
}

EXPORT VOID base64decoder_endinput(base64decoder_t *decoder, UB **result, W *result_len)
{
	*result = NULL;
	*result_len = 0;
	return;
}

EXPORT VOID base64decoder_clear(base64decoder_t *decoder)
{
	decoder->state = BASE64ENCODER_STATE_FIRSTCHAR;
}

EXPORT VOID base64decoder_initialize(base64decoder_t *decoder)
{
	decoder->state = BASE64ENCODER_STATE_FIRSTCHAR;
}

EXPORT VOID base64decoder_finalize(base64decoder_t *decoder)
{
}

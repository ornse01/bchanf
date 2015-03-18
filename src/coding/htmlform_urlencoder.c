/*
 * htmlform_urlencoder.c
 *
 * Copyright (c) 2015 project bchan
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

#include "htmlform_urlencoder.h"

#include	<basic.h>
#include	<bctype.h>

#ifdef BCHAN_CONFIG_DEBUG
# define DP(arg) printf arg
# define DP_ER(msg, err) printf("%s (%d/%x)\n", msg, err>>16, err)
#else
# define DP(arg) /**/
# define DP_ER(msg, err) /**/
#endif

#if 0
# define DP_STATE(state) printf("%s\n", state)
#else
# define DP_STATE(state) /**/
#endif

LOCAL VOID htmlform_urlencoder_convert(htmlform_urlencoder_t *encoder, UB ch, UB **str, W *len)
{
	static UB num[] = "0123456789ABCDEF";
	UB *buffer = encoder->buf;

	if (ch == 0x2E) { /* '.' */
		buffer[0] = ch;
		*len = 1;
	} else if (ch == 0x2D) { /* '-' */
		buffer[0] = ch;
		*len = 1;
	} else if (ch == 0x5F) { /* '_' */
		buffer[0] = ch;
		*len = 1;
	} else if (ch == 0x20) { /* ' ' */
		buffer[0] = 0x2B; /* '+' */
		*len = 1;
	} else if (isalnum(ch)) {
		buffer[0] = ch;
		*len = 1;
	} else {
		buffer[0] = 0x25; /* '%' */
		buffer[1] = num[ch >> 4];
		buffer[2] = num[ch & 0xF];
		*len = 3;
	}

	*str = buffer;
}

LOCAL UB equal[] = "=";
LOCAL UB amp[] = "&";

EXPORT Bool htmlform_urlencoder_next(htmlform_urlencoder_t *encoder, UB **str, W *len)
{
	htmlform_field *field;

	if (encoder->field_index == encoder->fields_len) {
		return False;
	}

	field = encoder->fields + encoder->field_index;

	switch (encoder->state) {
	case HTMLFORM_URLENCODER_STATE_NAME:
		DP_STATE("NAME");
		if (encoder->src_pos == field->name_len) {
			encoder->state = HTMLFORM_URLENCODER_STATE_EQUAL;
			*str = encoder->buf;
			*len = 0;
			return True;
		}
		htmlform_urlencoder_convert(encoder, field->name[encoder->src_pos], str, len);
		encoder->src_pos++;
		return True;
	case HTMLFORM_URLENCODER_STATE_EQUAL:
		DP_STATE("EQUAL");
		*str = equal;
		*len = 1;
		encoder->state = HTMLFORM_URLENCODER_STATE_VALUE;
		encoder->src_pos = 0;
		return True;
	case HTMLFORM_URLENCODER_STATE_VALUE:
		DP_STATE("VALUE");
		if (encoder->src_pos == field->value_len) {
			encoder->state = HTMLFORM_URLENCODER_STATE_AMP;
			encoder->field_index++;
			*str = encoder->buf;
			*len = 0;
			return True;
		}
		htmlform_urlencoder_convert(encoder, field->value[encoder->src_pos], str, len);
		encoder->src_pos++;
		return True;
	case HTMLFORM_URLENCODER_STATE_AMP:
		DP_STATE("AMP");
		*str = amp;
		*len = 1;
		encoder->state = HTMLFORM_URLENCODER_STATE_NAME;
		encoder->src_pos = 0;
		return True;
	}

	return False;
}

EXPORT VOID htmlform_urlencoder_initialize(htmlform_urlencoder_t *encoder, htmlform_field *fields, W fields_len)
{
	encoder->fields = fields;
	encoder->fields_len = fields_len;
	encoder->state = HTMLFORM_URLENCODER_STATE_NAME;
	encoder->field_index = 0;
	encoder->src_pos = 0;
}

EXPORT VOID htmlform_urlencoder_finalize(htmlform_urlencoder_t *encoder)
{
}

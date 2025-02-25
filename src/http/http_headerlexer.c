/*
 * http_headerlexer.c
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

#include    "http_headerlexer.h"

#include	<basic.h>
#include	<bstdlib.h>
#include	<bstdio.h>

#ifdef BCHAN_CONFIG_DEBUG
# define DP(arg) printf arg
# define DP_ER(msg, err) printf("%s (%d/%x)\n", msg, err>>16, err)
#else
# define DP(arg) /**/
# define DP_ER(msg, err) /**/
#endif

#if 0
#define DP_STATE(arg) printf arg
#else
#define DP_STATE(arg) /**/
#endif

EXPORT VOID http_headerlexer_inputchar(http_headerlexer_t *lexer, UB ch, http_headerlexer_result **result, W *result_len)
{
	*result = lexer->buf;
	lexer->buf[0].type = HTTP_HEADERLEXER_RESULT_NONE;
	*result_len = 0;

	switch (lexer->state) {
	case HTTP_HEADERLEXER_STATE_START:
		DP_STATE(("state = START: %c[%02x]\n", ch, ch));
		if (ch == '\r') {
			lexer->state = HTTP_HEADERLEXER_STATE_SEARCH_HEADER_CR;
			break;
		}
		/* TODO: handling error charactors for "token" */
		lexer->state = HTTP_HEADERLEXER_STATE_READ_HEADER_FIELDNAME;
		lexer->buf[0].type = HTTP_HEADERLEXER_RESULT_FIELDNAME;
		lexer->buf[0].ch = ch;
		*result_len = 1;
		break;
	case HTTP_HEADERLEXER_STATE_SEARCH_HEADER:
		DP_STATE(("state = SEARCH_HEADER: %c[%02x]\n", ch, ch));
		if (ch == '\r') {
			lexer->state = HTTP_HEADERLEXER_STATE_SEARCH_HEADER_CR;
			lexer->buf[0].type = HTTP_HEADERLEXER_RESULT_FIELDVALUE_END;
			lexer->buf[0].ch = '\0';
			*result_len = 1;
			break;
		}
		if ((ch == ' ')&&(ch = '\t')) {
			lexer->state = HTTP_HEADERLEXER_STATE_READ_HEADER_FIELDVALUE_LWS;
			lexer->buf[0].type = HTTP_HEADERLEXER_RESULT_LWS;
			*result_len = 1;
			/* TODO: return value in "LWS CRLF LWS". */
			break;
		}
		/* TODO: handling error charactors for "token" */
		lexer->state = HTTP_HEADERLEXER_STATE_READ_HEADER_FIELDNAME;
		lexer->buf[0].type = HTTP_HEADERLEXER_RESULT_FIELDVALUE_END;
		lexer->buf[0].ch = '\0';
		lexer->buf[1].type = HTTP_HEADERLEXER_RESULT_FIELDNAME;
		lexer->buf[1].ch = ch;
		*result_len = 2;
		break;
	case HTTP_HEADERLEXER_STATE_SEARCH_HEADER_CR:
		DP_STATE(("state = SEARCH_HEADER_CR: %c[%02x]\n", ch, ch));
		if (ch == '\n') {
			lexer->state = HTTP_HEADERLEXER_STATE_READ_VALUE_MESSAGE;
			lexer->buf[0].type = HTTP_HEADERLEXER_RESULT_MESSAGEHEADER_END;
			*result_len = 1;
			break;
		}
		/* TODO: below error case. */
		if ((ch == ' ')&&(ch = '\t')) {
			lexer->state = HTTP_HEADERLEXER_STATE_READ_HEADER_FIELDVALUE_LWS;
			lexer->buf[0].type = HTTP_HEADERLEXER_RESULT_LWS;
			*result_len = 1;
			break;
		}
		lexer->state = HTTP_HEADERLEXER_STATE_READ_HEADER_FIELDNAME;
		lexer->buf[0].type = HTTP_HEADERLEXER_RESULT_FIELDNAME;
		lexer->buf[0].ch = ch;
		*result_len = 1;
		break;
	case HTTP_HEADERLEXER_STATE_READ_HEADER_FIELDNAME:
		DP_STATE(("state = READ_HEADER_FIELDNAME: %c[%02x]\n", ch, ch));
		if (ch == '\r') {
			lexer->state = HTTP_HEADERLEXER_STATE_HERDER_CR;
			break;
		}
		if (ch == ':') {
			lexer->state = HTTP_HEADERLEXER_STATE_READ_HEADER_FIELDVALUE;
			lexer->buf[0].type = HTTP_HEADERLEXER_RESULT_FIELDNAME_END;
			*result_len = 1;
			break;
		}
		/* TODO: handling error charactors for "token" */
		lexer->buf[0].type = HTTP_HEADERLEXER_RESULT_FIELDNAME;
		lexer->buf[0].ch = ch;
		*result_len = 1;
		break;
	case HTTP_HEADERLEXER_STATE_READ_HEADER_FIELDVALUE:
		DP_STATE(("state = READ_HEADER_FIELDVALUE: %c[%02x]\n", ch, ch));
		if (ch == '\r') {
			lexer->state = HTTP_HEADERLEXER_STATE_HERDER_CR;
			break;
		}
		if ((ch == ' ')&&(ch = '\t')) {
			lexer->state = HTTP_HEADERLEXER_STATE_READ_HEADER_FIELDVALUE_LWS;
			lexer->buf[0].type = HTTP_HEADERLEXER_RESULT_LWS;
			*result_len = 1;
			break;
		}
		lexer->buf[0].type = HTTP_HEADERLEXER_RESULT_FIELDCONTENT;
		lexer->buf[0].ch = ch;
		*result_len = 1;
		break;
	case HTTP_HEADERLEXER_STATE_READ_HEADER_FIELDVALUE_LWS:
		DP_STATE(("state = READ_HEADER_FIELDVALUE_LWS: %c[%02x]\n", ch, ch));
		if (ch == '\r') {
			lexer->state = HTTP_HEADERLEXER_STATE_HERDER_CR;
			break;
		}
		if ((ch != ' ')&&(ch != '\t')) {
			lexer->state = HTTP_HEADERLEXER_STATE_READ_HEADER_FIELDVALUE;
			lexer->buf[0].type = HTTP_HEADERLEXER_RESULT_FIELDCONTENT;
			lexer->buf[0].ch = ch;
			*result_len = 1;
			break;
		}
		break;
	case HTTP_HEADERLEXER_STATE_HERDER_CR:
		DP_STATE(("state = HERDER_CR: %c[%02x]\n", ch, ch));
		if (ch == '\n') {
			lexer->state = HTTP_HEADERLEXER_STATE_SEARCH_HEADER;
		}
		break;
	case HTTP_HEADERLEXER_STATE_READ_VALUE_MESSAGE:
		DP_STATE(("state = READ_VALUE_MESSAGE: %c[%02x]\n", ch, ch));
		break;
	}
	return;
}

EXPORT W http_headerlexer_initialize(http_headerlexer_t *lexer)
{
	lexer->state = HTTP_HEADERLEXER_STATE_START;
	return 0;
}

EXPORT VOID http_headerlexer_finalize(http_headerlexer_t *lexer)
{
}

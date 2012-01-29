/*
 * http_defaultheaderparser.c
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

#include "http_defaultheaderparser.h"

#include	<basic.h>
#include	<bstdio.h>
#include	<bctype.h>

#include    "http_typedef.h"
#include    "http_headerlexer.h"
#include    <parse/tokenchecker.h>

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

enum {
	HTTP_DHP_FIELDNAME_CONTENT_ENCODING = 1,
	HTTP_DHP_FIELDNAME_CONTENT_LENGTH,
	HTTP_DHP_FIELDNAME_TRANSER_ENCODING,
	HTTP_DHP_FIELDNAME_CONNECTION,
};

LOCAL VOID http_defaultheaderparser_inputhlex(http_defaultheaderparser_t *parser, http_headerlexer_result *cmd, HTTP_DEFAULTHEADERPARSER_RESULT *result)
{
	TOKENCHECKER_RESULT tchk;
	W val;

	switch (parser->state) {
	case HTTP_DEFAULTHEADERPARSER_STATE_FIELDNAME:
		DP_STATE("FIELDNAME");
		if (cmd->type == HTTP_HEADERLEXER_RESULT_FIELDNAME) {
			tchk = tokenchecker_inputchar(&parser->checker.fieldname, cmd->ch, &val);
			if (tchk == TOKENCHECKER_RESULT_CONTINUE_NOMATCH) {
				parser->state = HTTP_DEFAULTHEADERPARSER_STATE_SKIP_FIELDNAME;
			} else if (tchk != TOKENCHECKER_RESULT_CONTINUE) {
				/* format error */
				DP(("format error 0\n"));
			}
		} else if (cmd->type == HTTP_HEADERLEXER_RESULT_FIELDNAME_END) {
			tchk = tokenchecker_endinput(&parser->checker.fieldname, &val);
			if (tchk == TOKENCHECKER_RESULT_DETERMINE) {
				switch (val) {
				case HTTP_DHP_FIELDNAME_CONTENT_ENCODING:
					parser->state = HTTP_DEFAULTHEADERPARSER_STATE_CONTENT_ENCODING_LWS;
					break;
				case HTTP_DHP_FIELDNAME_CONTENT_LENGTH:
					parser->state = HTTP_DEFAULTHEADERPARSER_STATE_CONTENT_LENGTH_LWS;
					break;
				case HTTP_DHP_FIELDNAME_TRANSER_ENCODING:
					parser->state = HTTP_DEFAULTHEADERPARSER_STATE_TRANSFER_ENCODING_LWS;
					break;
				case HTTP_DHP_FIELDNAME_CONNECTION:
					parser->state = HTTP_DEFAULTHEADERPARSER_STATE_CONNECTION_LWS;
					break;
				default:
					parser->state = HTTP_DEFAULTHEADERPARSER_STATE_SKIP_FIELDVALUE;
					break;
				}
			} else if (tchk == TOKENCHECKER_RESULT_NOMATCH) {
			} else {
				/* format error */
				DP(("format error 1\n"));
			}
		} else if (cmd->type == HTTP_HEADERLEXER_RESULT_MESSAGEHEADER_END) { 
			*result = HTTP_DEFAULTHEADERPARSER_RESULT_MESSAGE_HEADER_END;
		} else {
			/* format error */
			DP(("format error 2\n"));
		}
		break;
	case HTTP_DEFAULTHEADERPARSER_STATE_SKIP_FIELDNAME:
		DP_STATE("SKIP_FIELDNAME");
		if (cmd->type == HTTP_HEADERLEXER_RESULT_FIELDNAME_END) {
			parser->state = HTTP_DEFAULTHEADERPARSER_STATE_SKIP_FIELDVALUE;
		} else if (cmd->type != HTTP_HEADERLEXER_RESULT_FIELDNAME) {
			/* format error */
			DP(("format error 3\n"));
		}
		break;
	case HTTP_DEFAULTHEADERPARSER_STATE_SKIP_FIELDVALUE:
		DP_STATE("SKIP_FIELDVALUE");
		if (cmd->type == HTTP_HEADERLEXER_RESULT_FIELDVALUE_END) {
			parser->state = HTTP_DEFAULTHEADERPARSER_STATE_FIELDNAME;
			tokenchecker_clear(&parser->checker.fieldname);			
		} else if ((cmd->type != HTTP_HEADERLEXER_RESULT_FIELDCONTENT)&&(cmd->type != HTTP_HEADERLEXER_RESULT_LWS)) {
			/* format error */
			DP(("format error 4\n"));
		}
		break;
	case HTTP_DEFAULTHEADERPARSER_STATE_CONTENT_ENCODING_LWS:
		DP_STATE("CONTENT_ENCODING_LWS");
		if (cmd->type == HTTP_HEADERLEXER_RESULT_LWS) {
			break;
		} else if (cmd->type == HTTP_HEADERLEXER_RESULT_FIELDVALUE_END) {
			parser->state = HTTP_DEFAULTHEADERPARSER_STATE_FIELDNAME;
			tokenchecker_clear(&parser->checker.fieldname);
		} else if (cmd->type != HTTP_HEADERLEXER_RESULT_FIELDCONTENT) {
			/* format error */
			DP(("format error 5\n"));
		}
		parser->state = HTTP_DEFAULTHEADERPARSER_STATE_CONTENT_ENCODING;
		tokenchecker_clear(&parser->checker.contentcoding);
	case HTTP_DEFAULTHEADERPARSER_STATE_CONTENT_ENCODING:
		DP_STATE("CONTENT_ENCODING");
		if (cmd->type == HTTP_HEADERLEXER_RESULT_FIELDCONTENT) {
			tchk = tokenchecker_inputchar(&parser->checker.contentcoding, cmd->ch, &val);
			if (tchk == TOKENCHECKER_RESULT_CONTINUE_NOMATCH) {
				parser->state = HTTP_DEFAULTHEADERPARSER_STATE_SKIP_FIELDVALUE;
			} else if (tchk != TOKENCHECKER_RESULT_CONTINUE) {
				/* format error */
				DP(("format error 6\n"));
			}
		} else if (cmd->type == HTTP_HEADERLEXER_RESULT_FIELDVALUE_END) {
			tchk = tokenchecker_endinput(&parser->checker.contentcoding, &val);
			if (tchk == TOKENCHECKER_RESULT_DETERMINE) {
				parser->result.contentcoding = val;
			} else if (tchk == TOKENCHECKER_RESULT_NOMATCH) {
			} else {
				/* format error */
				DP(("format error 7\n"));
			}
			parser->state = HTTP_DEFAULTHEADERPARSER_STATE_FIELDNAME;
			tokenchecker_clear(&parser->checker.fieldname);
		} else {
			/* format error */
			DP(("format error 8\n"));
		}
		break;
	case HTTP_DEFAULTHEADERPARSER_STATE_TRANSFER_ENCODING_LWS:
		DP_STATE("TRANSFER_ENCODING_LWS");
		if (cmd->type == HTTP_HEADERLEXER_RESULT_LWS) {
			break;
		} else if (cmd->type == HTTP_HEADERLEXER_RESULT_FIELDVALUE_END) {
			parser->state = HTTP_DEFAULTHEADERPARSER_STATE_FIELDNAME;
			tokenchecker_clear(&parser->checker.fieldname);
		} else if (cmd->type != HTTP_HEADERLEXER_RESULT_FIELDCONTENT) {
			/* format error */
			DP(("format error 9\n"));
		}
		parser->state = HTTP_DEFAULTHEADERPARSER_STATE_TRANSFER_ENCODING;
		tokenchecker_clear(&parser->checker.transfercoding);
	case HTTP_DEFAULTHEADERPARSER_STATE_TRANSFER_ENCODING:
		DP_STATE("TRANSFER_ENCODING");
		if (cmd->type == HTTP_HEADERLEXER_RESULT_FIELDCONTENT) {
			tchk = tokenchecker_inputchar(&parser->checker.transfercoding, cmd->ch, &val);
			if (tchk == TOKENCHECKER_RESULT_CONTINUE_NOMATCH) {
				parser->state = HTTP_DEFAULTHEADERPARSER_STATE_SKIP_FIELDVALUE;
			} else if (tchk != TOKENCHECKER_RESULT_CONTINUE) {
				/* format error */
				DP(("format error 10\n"));
			}
		} else if (cmd->type == HTTP_HEADERLEXER_RESULT_FIELDVALUE_END) {
			tchk = tokenchecker_endinput(&parser->checker.transfercoding, &val);
			if (tchk == TOKENCHECKER_RESULT_DETERMINE) {
				parser->result.transfercoding = val;
			} else if (tchk == TOKENCHECKER_RESULT_NOMATCH) {
			} else {
				/* format error */
				DP(("format error 11\n"));
			}
			parser->state = HTTP_DEFAULTHEADERPARSER_STATE_FIELDNAME;
			tokenchecker_clear(&parser->checker.fieldname);
		} else {
			/* format error */
			DP(("format error 12\n"));
		}
		break;
	case HTTP_DEFAULTHEADERPARSER_STATE_CONNECTION_LWS:
		DP_STATE("CONNECTION_LWS");
		if (cmd->type == HTTP_HEADERLEXER_RESULT_LWS) {
			break;
		} else if (cmd->type == HTTP_HEADERLEXER_RESULT_FIELDVALUE_END) {
			parser->state = HTTP_DEFAULTHEADERPARSER_STATE_FIELDNAME;
			tokenchecker_clear(&parser->checker.fieldname);
		} else if (cmd->type != HTTP_HEADERLEXER_RESULT_FIELDCONTENT){
			/* format error */
			DP(("format error 13\n"));
		}
		parser->state = HTTP_DEFAULTHEADERPARSER_STATE_CONNECTION;
		tokenchecker_clear(&parser->checker.connection);
	case HTTP_DEFAULTHEADERPARSER_STATE_CONNECTION:
		DP_STATE("CONNECTION");
		if (cmd->type == HTTP_HEADERLEXER_RESULT_FIELDCONTENT) {
			tchk = tokenchecker_inputchar(&parser->checker.connection, cmd->ch, &val);
			if (tchk == TOKENCHECKER_RESULT_CONTINUE_NOMATCH) {
				parser->state = HTTP_DEFAULTHEADERPARSER_STATE_SKIP_FIELDVALUE;
			} else if (tchk != TOKENCHECKER_RESULT_CONTINUE) {
				/* format error */
				DP(("format error 14\n"));
			}
		} else if (cmd->type == HTTP_HEADERLEXER_RESULT_FIELDVALUE_END) {
			tchk = tokenchecker_endinput(&parser->checker.connection, &val);
			if (tchk == TOKENCHECKER_RESULT_DETERMINE) {
				if (val == 1) {
					parser->result.connectionclosed = True;
				} else {
					/* unexpected token */
					DP(("unexpected token\n"));
				}
			} else if (tchk == TOKENCHECKER_RESULT_NOMATCH) {
			} else {
				/* format error */
				DP(("format error 15\n"));
			}
			parser->state = HTTP_DEFAULTHEADERPARSER_STATE_FIELDNAME;
			tokenchecker_clear(&parser->checker.fieldname);
		} else {
			/* format error */
			DP(("format error 16\n"));
		}
		break;
	case HTTP_DEFAULTHEADERPARSER_STATE_CONTENT_LENGTH_LWS:
		DP_STATE("CONTENT_LENGTH_LWS");
		if (cmd->type == HTTP_HEADERLEXER_RESULT_LWS) {
			break;
		} else if (cmd->type == HTTP_HEADERLEXER_RESULT_FIELDVALUE_END) {
			parser->state = HTTP_DEFAULTHEADERPARSER_STATE_FIELDNAME;
			tokenchecker_clear(&parser->checker.fieldname);
		} else if (cmd->type != HTTP_HEADERLEXER_RESULT_FIELDCONTENT){
			/* format error */
			DP(("format error 17\n"));
		}
		parser->state = HTTP_DEFAULTHEADERPARSER_STATE_CONTENT_LENGTH;
		parser->result.contentlength = 0;
	case HTTP_DEFAULTHEADERPARSER_STATE_CONTENT_LENGTH:
		DP_STATE("CONTENT_LENGTH");
		if (cmd->type == HTTP_HEADERLEXER_RESULT_FIELDCONTENT) {
			if (isdigit(cmd->ch) != 0) {
				parser->result.contentlength = parser->result.contentlength * 10 + (cmd->ch - '0');
			} else {
				parser->state = HTTP_DEFAULTHEADERPARSER_STATE_SKIP_FIELDVALUE;
			}
		} else if (cmd->type == HTTP_HEADERLEXER_RESULT_LWS) { 
			parser->state = HTTP_DEFAULTHEADERPARSER_STATE_SKIP_FIELDVALUE;
		} else if (cmd->type == HTTP_HEADERLEXER_RESULT_FIELDVALUE_END) {
			parser->state = HTTP_DEFAULTHEADERPARSER_STATE_FIELDNAME;
			tokenchecker_clear(&parser->checker.fieldname);
		} else {
			/* format error */
			DP(("format error 18\n"));
		}
		break;
	}
}

EXPORT VOID http_defaultheaderparser_inputchar(http_defaultheaderparser_t *parser, UB ch, HTTP_DEFAULTHEADERPARSER_RESULT *result)
{
	http_headerlexer_result *hlex;
	W i, result_len;

	http_headerlexer_inputchar(&parser->header, ch, &hlex, &result_len);

	for (i = 0; i < result_len; i++) {
		*result = HTTP_DEFAULTHEADERPARSER_RESULT_NONE;
		if (hlex[i].type == HTTP_HEADERLEXER_RESULT_NONE) {
			continue;
		}
		http_defaultheaderparser_inputhlex(parser, hlex+i, result);
	}
}

EXPORT W http_defaultheaderparsr_contentlength(http_defaultheaderparser_t *parser)
{
	return parser->result.contentlength;
}

EXPORT HTTP_TRANSFERCODING_TYPE http_defaultheaderparsr_transferencoding(http_defaultheaderparser_t *parser)
{
	return parser->result.transfercoding;
}

EXPORT HTTP_CONTENTCODING_VALUE http_defaultheaderparsr_contentencoding(http_defaultheaderparser_t *parser)
{
	return parser->result.contentcoding;
}

EXPORT Bool http_defaultheaderparser_connectionclosed(http_defaultheaderparser_t *parser)
{
	return parser->result.connectionclosed;
}

LOCAL tokenchecker_valuetuple_t nList_fieldname[] = {
	{"Connection", HTTP_DHP_FIELDNAME_CONNECTION},
	{"Content-Encoding", HTTP_DHP_FIELDNAME_CONTENT_ENCODING},
	{"Content-Length", HTTP_DHP_FIELDNAME_CONTENT_LENGTH},
	{"Transfer-Encoding", HTTP_DHP_FIELDNAME_TRANSER_ENCODING},
};
LOCAL B eToken_fieldname[] = ":";

LOCAL tokenchecker_valuetuple_t nList_transfercoding[] = {
	{"chunked", HTTP_TRANSFERCODING_TYPE_CHUNKED},
	{"identity", HTTP_TRANSFERCODING_TYPE_IDENTITY},
};
LOCAL B eToken_transfercoding[] = "\r";

LOCAL tokenchecker_valuetuple_t nList_contentcoding[] = {
	{"compress", HTTP_CONTENTCODING_VALUE_COMPRESS},
	{"deflate", HTTP_CONTENTCODING_VALUE_DEFLATE},
	{"gzip", HTTP_CONTENTCODING_VALUE_GZIP},
	{"identity", HTTP_CONTENTCODING_VALUE_IDENTITY},
};
LOCAL B eToken_contentcoding[] = "\r";

LOCAL tokenchecker_valuetuple_t nList_connection[] = {
	{"close", 1},
};
LOCAL B eToken_connection[] = "\r";

EXPORT VOID http_defaultheaderparser_initialize(http_defaultheaderparser_t *parser)
{
	parser->state = HTTP_DEFAULTHEADERPARSER_STATE_FIELDNAME;
	http_headerlexer_initialize(&parser->header);
	tokenchecker_initialize(&(parser->checker.fieldname), nList_fieldname, 4, eToken_fieldname);
	tokenchecker_initialize(&(parser->checker.transfercoding), nList_transfercoding, 2, eToken_transfercoding);
	tokenchecker_initialize(&(parser->checker.contentcoding), nList_contentcoding, 4, eToken_contentcoding);
	tokenchecker_initialize(&(parser->checker.connection), nList_connection, 1, eToken_connection);
	parser->result.contentlength = 0;
	parser->result.transfercoding = HTTP_TRANSFERCODING_TYPE_IDENTITY;
	parser->result.contentcoding = HTTP_CONTENTCODING_VALUE_IDENTITY;
	parser->result.connectionclosed = False;
}

EXPORT VOID http_defaultheaderparser_finalize(http_defaultheaderparser_t *parser)
{
	tokenchecker_finalize(&(parser->checker.connection));
	tokenchecker_finalize(&(parser->checker.contentcoding));
	tokenchecker_finalize(&(parser->checker.transfercoding));
	tokenchecker_finalize(&(parser->checker.fieldname));
	http_headerlexer_finalize(&parser->header);
}

/*
 * http_statuslineparser.c
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

#include "http_statuslineparser.h"

#include	<basic.h>
#include	<bstdio.h>

#include    <parse/tokenchecker.h>
#include    "http_typedef.h"

EXPORT Bool http_statuslineparser_inputchar(http_statuslineparser_t *parser, UB ch, http_statuslineparser_result *result)
{
	TOKENCHECKER_RESULT ret;
	W val;

	switch (parser->state) {
	case HTTP_STATUSLINEPARSER_STATE_HTTP_VERSION:
		ret = tokenchecker_inputchar(&parser->checker.version, ch, &val);
		if (ret == TOKENCHECKER_RESULT_DETERMINE) {
			parser->state = HTTP_STATUSLINEPARSER_STATE_STATUS_CODE;
			parser->result.version = val;
		} else if (ret == TOKENCHECKER_RESULT_NOMATCH) {
			parser->state = HTTP_STATUSLINEPARSER_STATE_STATUS_CODE;
			parser->result.error = HTTP_STATUSLINEPARSER_ERROR_UNSUPPORTED_HTTP_VERSION;
		}
		break;
	case HTTP_STATUSLINEPARSER_STATE_STATUS_CODE:
		ret = tokenchecker_inputchar(&parser->checker.status, ch, &val);
		if (ret == TOKENCHECKER_RESULT_DETERMINE) {
			parser->state = HTTP_STATUSLINEPARSER_STATE_SKIP;
			parser->result.statuscode = val;
		} else if (ret == TOKENCHECKER_RESULT_NOMATCH) {
			parser->state = HTTP_STATUSLINEPARSER_STATE_SKIP;
			parser->result.error = HTTP_STATUSLINEPARSER_ERROR_UNSUPPORTED_STATUS_CODE;
		}
		break;
	case HTTP_STATUSLINEPARSER_STATE_SKIP:
		if (ch == '\r') {
			parser->state = HTTP_STATUSLINEPARSER_STATE_SKIP_CR;
		}
		break;
	case HTTP_STATUSLINEPARSER_STATE_SKIP_CR:
		if (ch == '\n') {
			result->error = parser->result.error;
			result->version = parser->result.version;
			result->statuscode = parser->result.statuscode;
			return True;
		}
		break;
	}

	return False;
}

LOCAL tokenchecker_valuetuple_t nList_httpversion[] = {
	{"HTTP/0.9", HTTP_HTTPVERSION_0_9},
	{"HTTP/1.0", HTTP_HTTPVERSION_1_0},
	{"HTTP/1.1", HTTP_HTTPVERSION_1_1},
};
LOCAL B eToken_httpversion[] = " ";

LOCAL tokenchecker_valuetuple_t nList_statuscode[] = {
	{"100", HTTP_STATUSCODE_100_Continue},
	{"101", HTTP_STATUSCODE_101_Switching_Protocols},
	{"200", HTTP_STATUSCODE_200_OK},
	{"201", HTTP_STATUSCODE_201_Created},
	{"202", HTTP_STATUSCODE_202_Accepted},
	{"203", HTTP_STATUSCODE_203_Non_Authoritative_Information},
	{"204", HTTP_STATUSCODE_204_No_Content},
	{"205", HTTP_STATUSCODE_205_Reset_Content},
	{"206", HTTP_STATUSCODE_206_Partial_Content},
	{"300", HTTP_STATUSCODE_300_Multiple_Choices},
	{"301", HTTP_STATUSCODE_301_Moved_Permanently},
	{"302", HTTP_STATUSCODE_302_Found},
	{"303", HTTP_STATUSCODE_303_See_Other},
	{"304", HTTP_STATUSCODE_304_Not_Modified},
	{"305", HTTP_STATUSCODE_305_Use_Proxy},
	{"307", HTTP_STATUSCODE_307_Temporary_Redirect},
	{"400", HTTP_STATUSCODE_400_Bad_Request},
	{"401", HTTP_STATUSCODE_401_Unauthorized},
	{"402", HTTP_STATUSCODE_402_Payment_Required},
	{"403", HTTP_STATUSCODE_403_Forbidden},
	{"404", HTTP_STATUSCODE_404_Not_Found},
	{"405", HTTP_STATUSCODE_405_Method_Not_Allowed},
	{"406", HTTP_STATUSCODE_406_Not_Acceptable},
	{"407", HTTP_STATUSCODE_407_Proxy_Authentication_Required},
	{"408", HTTP_STATUSCODE_408_Request_Time_out},
	{"409", HTTP_STATUSCODE_409_Conflict},
	{"410", HTTP_STATUSCODE_410_Gone},
	{"411", HTTP_STATUSCODE_411_Length_Required},
	{"412", HTTP_STATUSCODE_412_Precondition_Failed},
	{"413", HTTP_STATUSCODE_413_Request_Entity_Too_Large},
	{"414", HTTP_STATUSCODE_414_Request_URI_Too_Large},
	{"415", HTTP_STATUSCODE_415_Unsupported_Media_Type},
	{"416", HTTP_STATUSCODE_416_Requested_range_not_satisfiable},
	{"417", HTTP_STATUSCODE_417_Expectation_Failed},
	{"500", HTTP_STATUSCODE_500_Internal_Server_Error},
	{"501", HTTP_STATUSCODE_501_Not_Implemented},
	{"502", HTTP_STATUSCODE_502_Bad_Gateway},
	{"503", HTTP_STATUSCODE_503_Service_Unavailable},
	{"504", HTTP_STATUSCODE_504_Gateway_Time_out},
	{"505", HTTP_STATUSCODE_505_HTTP_Version_not_supported},
};
LOCAL B eToken_statuscode[] = " ";

EXPORT VOID http_statuslineparser_initialize(http_statuslineparser_t *parser)
{
	parser->state = HTTP_STATUSLINEPARSER_STATE_HTTP_VERSION;
	tokenchecker_initialize(&(parser->checker.version), nList_httpversion, 3, eToken_httpversion);
	tokenchecker_initialize(&(parser->checker.status), nList_statuscode, 40, eToken_statuscode);
	parser->result.error = HTTP_STATUSLINEPARSER_ERROR_NONE;
	parser->result.version = -1;
	parser->result.statuscode = -1;
}

EXPORT VOID http_statuslineparser_finalize(http_statuslineparser_t *parser)
{
	tokenchecker_finalize(&(parser->checker.status));
	tokenchecker_finalize(&(parser->checker.version));
}

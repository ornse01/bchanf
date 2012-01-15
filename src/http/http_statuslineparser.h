/*
 * http_transferdecoder.h
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
/* Functionality name: http */
/* Detail name: statuslineparser */

#include    <basic.h>

#include    <parse/tokenchecker.h>

/* rfc2616 6.1 Status-Line */

#ifndef __HTTP_STATUSLINEPARSER_H__
#define __HTTP_STATUSLINEPARSER_H__

/* Functionality name: http */
/* Detail name: */
/* Data structure identifier: httpversion */
/*  rfc2616 3.1 HTTP Version */
typedef enum {
	HTTP_HTTPVERSION_0_9, /* HTTP/0.9 */
	HTTP_HTTPVERSION_1_0, /* HTTP/1.0 */
	HTTP_HTTPVERSION_1_1, /* HTTP/1.1 */
} HTTP_HTTPVERSION;

/* Functionality name: http */
/* Detail name: */
/* Data structure identifier: statuscode */
/*  rfc2616 6.1.1 Status Code and Reason Phrase */
typedef enum {
	HTTP_STATUSCODE_100_Continue                        = 100,
	HTTP_STATUSCODE_101_Switching_Protocols             = 101,
	HTTP_STATUSCODE_200_OK	                            = 200,
	HTTP_STATUSCODE_201_Created                         = 201,
	HTTP_STATUSCODE_202_Accepted                        = 202,
	HTTP_STATUSCODE_203_Non_Authoritative_Information   = 203,
	HTTP_STATUSCODE_204_No_Content                      = 204,
	HTTP_STATUSCODE_205_Reset_Content                   = 205,
	HTTP_STATUSCODE_206_Partial_Content                 = 206,
	HTTP_STATUSCODE_300_Multiple_Choices                = 300,
	HTTP_STATUSCODE_301_Moved_Permanently               = 301,
	HTTP_STATUSCODE_302_Found                           = 302,
	HTTP_STATUSCODE_303_See_Other                       = 303,
	HTTP_STATUSCODE_304_Not_Modified                    = 304,
	HTTP_STATUSCODE_305_Use_Proxy                       = 305,
	HTTP_STATUSCODE_307_Temporary_Redirect              = 307,
	HTTP_STATUSCODE_400_Bad_Request                     = 400,
	HTTP_STATUSCODE_401_Unauthorized                    = 401,
	HTTP_STATUSCODE_402_Payment_Required                = 402,
	HTTP_STATUSCODE_403_Forbidden                       = 403,
	HTTP_STATUSCODE_404_Not_Found                       = 404,
	HTTP_STATUSCODE_405_Method_Not_Allowed              = 405,
	HTTP_STATUSCODE_406_Not_Acceptable                  = 406,
	HTTP_STATUSCODE_407_Proxy_Authentication_Required   = 407,
	HTTP_STATUSCODE_408_Request_Time_out                = 408,
	HTTP_STATUSCODE_409_Conflict                        = 409,
	HTTP_STATUSCODE_410_Gone                            = 410,
	HTTP_STATUSCODE_411_Length_Required                 = 411,
	HTTP_STATUSCODE_412_Precondition_Failed             = 412,
	HTTP_STATUSCODE_413_Request_Entity_Too_Large        = 413,
	HTTP_STATUSCODE_414_Request_URI_Too_Large           = 414,
	HTTP_STATUSCODE_415_Unsupported_Media_Type          = 415,
	HTTP_STATUSCODE_416_Requested_range_not_satisfiable = 416,
	HTTP_STATUSCODE_417_Expectation_Failed              = 417,
	HTTP_STATUSCODE_500_Internal_Server_Error           = 500,
	HTTP_STATUSCODE_501_Not_Implemented                 = 501,
	HTTP_STATUSCODE_502_Bad_Gateway                     = 502,
	HTTP_STATUSCODE_503_Service_Unavailable             = 503,
	HTTP_STATUSCODE_504_Gateway_Time_out                = 504,
	HTTP_STATUSCODE_505_HTTP_Version_not_supported      = 505,
} HTTP_STATUSCODE;

/* Functionality name: http */
/* Detail name: statuslineparser */
/* Data structure identifier: statuscode */
struct http_statuslineparser_result_ {
	enum {
		HTTP_STATUSLINEPARSER_ERROR_NONE,
		HTTP_STATUSLINEPARSER_ERROR_UNSUPPORTED_HTTP_VERSION,
		HTTP_STATUSLINEPARSER_ERROR_UNSUPPORTED_STATUS_CODE,
		HTTP_STATUSLINEPARSER_ERROR_INVALID_FORMAT,
	} error;
	HTTP_HTTPVERSION version;
	HTTP_STATUSCODE statuscode;
};
typedef struct http_statuslineparser_result_ http_statuslineparser_result;

/* Functionality name: http */
/* Detail name: statuslineparser */
struct http_statuslineparser_t_ {
	enum {
		HTTP_STATUSLINEPARSER_STATE_HTTP_VERSION,
		HTTP_STATUSLINEPARSER_STATE_STATUS_CODE,
		HTTP_STATUSLINEPARSER_STATE_SKIP,
		HTTP_STATUSLINEPARSER_STATE_SKIP_CR,
	} state;
	struct {
		tokenchecker_t version;
		tokenchecker_t status;
	} checker;
	http_statuslineparser_result result;
};
typedef struct http_statuslineparser_t_ http_statuslineparser_t;

IMPORT VOID http_statuslineparser_initialize(http_statuslineparser_t *parser);
IMPORT VOID http_statuslineparser_finalize(http_statuslineparser_t *parser);
IMPORT Bool http_statuslineparser_inputchar(http_statuslineparser_t *parser, UB ch, http_statuslineparser_result *result);

#endif

/*
 * http_transferdecoder.c
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

#include "http_transferdecoder.h"

#include	<basic.h>
#include	<bstdio.h>
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

LOCAL W http_transferdecoderidentity_inputentitybody(http_transferdecoderidentity_t *decoder, UB *data, W data_len, http_transferdecoder_result **result, W *result_len)
{
	*result = decoder->result;

	if (decoder->pushed_len + data_len >= decoder->content_length) {
		decoder->result[0].type = HTTP_TRANSFERDECODER_RESULTTYPE_DATA;
		decoder->result[0].data = data;
		decoder->result[0].len = decoder->content_length - decoder->pushed_len;
		decoder->result[1].type = HTTP_TRANSFERDECODER_RESULTTYPE_END;
		decoder->result[1].data = NULL;
		decoder->result[1].len = 0;
		decoder->pushed_len = decoder->content_length;
		*result_len = 2;
		return decoder->result[0].len;
	}

	decoder->result[0].type = HTTP_TRANSFERDECODER_RESULTTYPE_DATA;
	decoder->result[0].data = data;
	decoder->result[0].len = data_len;
	decoder->pushed_len += data_len;
	*result_len = 1;
	return data_len;
}

LOCAL VOID http_transferdecoderidentity_initialize(http_transferdecoderidentity_t *decoder, W content_length)
{
	decoder->content_length = content_length;
	decoder->pushed_len = 0;
}

LOCAL VOID http_transferdecoderidentity_finalize(http_transferdecoderidentity_t *decoder)
{
}

typedef enum {
	HTTP_CHUNKEDBODYPARSER_RESULT_DATA,
	HTTP_CHUNKEDBODYPARSER_RESULT_OTHER,
	HTTP_CHUNKEDBODYPARSER_RESULT_END,
	HTTP_CHUNKEDBODYPARSER_RESULT_ERROR
} HTTP_CHUNKEDBODYPARSER_RESULT;

LOCAL HTTP_CHUNKEDBODYPARSER_RESULT http_chunkedbodyparser_inputchar(http_chunkedbodyparser_t *parser, UB ch)
{
	switch (parser->state) {
	case HTTP_CHUNKEDBODYPARSER_STATE_CHUNK_SIZE:
		DP_STATE("CHUNK_SIZE");
		if (isdigit(ch)) {
			parser->chunk_size = parser->chunk_size*16 + 0 + ch - '0';
			break;
		}
		if (('a' <= ch)&&(ch <= 'h')) {
			parser->chunk_size = parser->chunk_size*16 + 0xa + ch - 'a';
			break;
		}
		if (('A' <= ch)&&(ch <= 'H')) {
			parser->chunk_size = parser->chunk_size*16 + 0xa + ch - 'A';
			break;
		}
		if (ch == ';') {
			if (parser->chunk_size == 0) {
				parser->state = HTTP_CHUNKEDBODYPARSER_STATE_LAST_CHUNK_EXTENTION;
			} else {
				parser->state = HTTP_CHUNKEDBODYPARSER_STATE_CHUNK_EXTENTION;
			}
			break;
		}
		if (ch == '\r') {
			parser->state = HTTP_CHUNKEDBODYPARSER_STATE_CHUNK_SIZE_CR;
			break;
		}
		return HTTP_CHUNKEDBODYPARSER_RESULT_ERROR;
	case HTTP_CHUNKEDBODYPARSER_STATE_CHUNK_SIZE_CR:
		DP_STATE("CHUNK_SIZE_CR");
		if (ch == '\n') {
			if (parser->chunk_size == 0) {
				parser->state = HTTP_CHUNKEDBODYPARSER_STATE_AFTER_CHUNK;
			} else {
				parser->state = HTTP_CHUNKEDBODYPARSER_STATE_CHUNK_DATA;
				parser->chunk_pushed_len = 0;
			}
			break;
		}
		return HTTP_CHUNKEDBODYPARSER_RESULT_ERROR;
	case HTTP_CHUNKEDBODYPARSER_STATE_CHUNK_DATA:
		DP_STATE("CHUNK_DATA");
		parser->chunk_pushed_len++;
		if (parser->chunk_size == parser->chunk_pushed_len) {
			parser->state = HTTP_CHUNKEDBODYPARSER_STATE_CHUNK_DATA_END;
		}
		return HTTP_CHUNKEDBODYPARSER_RESULT_DATA;
	case HTTP_CHUNKEDBODYPARSER_STATE_CHUNK_DATA_END:
		DP_STATE("CHUNK_DATA_END");
		if (ch == '\r') {
			parser->state = HTTP_CHUNKEDBODYPARSER_STATE_CHUNK_DATA_END_CR;
			break;
		}
		return HTTP_CHUNKEDBODYPARSER_RESULT_ERROR;
	case HTTP_CHUNKEDBODYPARSER_STATE_CHUNK_DATA_END_CR:
		DP_STATE("CHUNK_DATA_END_CR");
		if (ch == '\n') {
			parser->state = HTTP_CHUNKEDBODYPARSER_STATE_CHUNK_SIZE;
			parser->chunk_size = 0;
			break;
		}
		return HTTP_CHUNKEDBODYPARSER_RESULT_ERROR;
	case HTTP_CHUNKEDBODYPARSER_STATE_CHUNK_EXTENTION:
		DP_STATE("CHUNK_EXTENTION");
		if (ch == '\r') {
			parser->state = HTTP_CHUNKEDBODYPARSER_STATE_CHUNK_EXTENTION_CR;
			break;
		}
		break;
	case HTTP_CHUNKEDBODYPARSER_STATE_CHUNK_EXTENTION_CR:
		DP_STATE("CHUNK_EXTENTION_CR");
		if (ch == '\n') {
			parser->state = HTTP_CHUNKEDBODYPARSER_STATE_CHUNK_DATA;
			parser->chunk_pushed_len = 0;
			break;
		}
		break;
	case HTTP_CHUNKEDBODYPARSER_STATE_LAST_CHUNK_EXTENTION:
		DP_STATE("LAST_CHUNK_EXTENTION");
		if (ch == '\r') {
			parser->state = HTTP_CHUNKEDBODYPARSER_STATE_LAST_CHUNK_EXTENTION_CR;
			break;
		}
		break;
	case HTTP_CHUNKEDBODYPARSER_STATE_LAST_CHUNK_EXTENTION_CR:
		DP_STATE("LAST_CHUNK_EXTENTION_CR");
		if (ch == '\n') {
			parser->state = HTTP_CHUNKEDBODYPARSER_STATE_AFTER_CHUNK;
			parser->chunk_pushed_len = 0;
			break;
		}
		return HTTP_CHUNKEDBODYPARSER_RESULT_ERROR;
	case HTTP_CHUNKEDBODYPARSER_STATE_AFTER_CHUNK:
		DP_STATE("AFTER_CHUNK");
		if (ch == '\r') {
			parser->state = HTTP_CHUNKEDBODYPARSER_STATE_AFTER_CHUNK_CR;
		} else {
			parser->state = HTTP_CHUNKEDBODYPARSER_STATE_TRAILER;
		}
		break;
	case HTTP_CHUNKEDBODYPARSER_STATE_AFTER_CHUNK_CR:
		DP_STATE("AFTER_CHUNK_CR");
		if (ch == '\n') {
			parser->state = HTTP_CHUNKEDBODYPARSER_STATE_END;
			return HTTP_CHUNKEDBODYPARSER_RESULT_END;
			break;
		}
		return HTTP_CHUNKEDBODYPARSER_RESULT_ERROR;
	case HTTP_CHUNKEDBODYPARSER_STATE_TRAILER:
		DP_STATE("TRAILER");
		if (ch == '\r') {
			parser->state = HTTP_CHUNKEDBODYPARSER_STATE_TRAILER_CR;
		}
		break;
	case HTTP_CHUNKEDBODYPARSER_STATE_TRAILER_CR:
		DP_STATE("TRAILER_CR");
		if (ch == '\n') {
			parser->state = HTTP_CHUNKEDBODYPARSER_STATE_AFTER_CHUNK;
			parser->chunk_pushed_len = 0;
			break;
		}
		return HTTP_CHUNKEDBODYPARSER_RESULT_END;
	case HTTP_CHUNKEDBODYPARSER_STATE_END:
		DP_STATE("END");
	}

	return HTTP_CHUNKEDBODYPARSER_RESULT_OTHER;
}

LOCAL VOID http_chunkedbodyparser_initialize(http_chunkedbodyparser_t *parser)
{
	parser->state = HTTP_CHUNKEDBODYPARSER_STATE_CHUNK_SIZE;
	parser->chunk_size = 0;
	parser->chunk_pushed_len = 0;
}

LOCAL VOID http_chunkedbodyparser_finalize(http_chunkedbodyparser_t *parser)
{
}

LOCAL W http_transferdecoderchunked_inputentitybody(http_transferdecoderchunked_t *decoder, UB *data, W data_len, http_transferdecoder_result **result, W *result_len)
{
	W i_data, i_result;
	enum {
		START,
		READING_DATA,
		READING_OTHER,
	} state = START;
	HTTP_CHUNKEDBODYPARSER_RESULT result_parser;

	i_result = 0;

	for (i_data = 0; i_data < data_len; i_data++) {
		result_parser = http_chunkedbodyparser_inputchar(&decoder->parser, data[i_data]);
		if (result_parser == HTTP_CHUNKEDBODYPARSER_RESULT_ERROR) {
			return -1; /* TODO */
		}

		if (state == START) {
			if (result_parser == HTTP_CHUNKEDBODYPARSER_RESULT_DATA) {
				state = READING_DATA;
				decoder->result[i_result].type = HTTP_TRANSFERDECODER_RESULTTYPE_DATA;
				decoder->result[i_result].data = data;
			} else if (result_parser == HTTP_CHUNKEDBODYPARSER_RESULT_OTHER) {
				state = READING_OTHER;
			} else if (result_parser == HTTP_CHUNKEDBODYPARSER_RESULT_END) {
				decoder->result[i_result].type = HTTP_TRANSFERDECODER_RESULTTYPE_END;
				i_result++;
				break;
			}
		} else if (state == READING_DATA) {
			if (result_parser == HTTP_CHUNKEDBODYPARSER_RESULT_OTHER) {
				state = READING_OTHER;
				decoder->result[i_result].len = data + i_data - decoder->result[i_result].data;
				i_result++;
				if (i_result >= HTTP_TRANSFERDECODER_CHUNKED_RESULTBUFLEN - 1) {
					break;
				}
			} else if (result_parser == HTTP_CHUNKEDBODYPARSER_RESULT_END) {
				decoder->result[i_result].len = data + i_data - decoder->result[i_result].data;
				i_result++;
				decoder->result[i_result].type = HTTP_TRANSFERDECODER_RESULTTYPE_END;
				i_result++;
				break;
			}
		} else if (state == READING_OTHER) {
			if (result_parser == HTTP_CHUNKEDBODYPARSER_RESULT_DATA) {
				state = READING_DATA;
				decoder->result[i_result].type = HTTP_TRANSFERDECODER_RESULTTYPE_DATA;
				decoder->result[i_result].data = data + i_data;
			} else if (result_parser == HTTP_CHUNKEDBODYPARSER_RESULT_END) {
				decoder->result[i_result].type = HTTP_TRANSFERDECODER_RESULTTYPE_END;
				i_result++;
				break;
			}
		}
	}

	*result = decoder->result;
	*result_len = i_result;	

	return i_data;
}

LOCAL VOID http_transferdecoderchunked_initialize(http_transferdecoderchunked_t *decoder)
{
	http_chunkedbodyparser_initialize(&decoder->parser);
}

LOCAL VOID http_transferdecoderchunked_finalize(http_transferdecoderchunked_t *decoder)
{
	http_chunkedbodyparser_finalize(&decoder->parser);
}

EXPORT W http_transferdecoder_inputentitybody(http_transferdecoder_t *decoder, UB *data, W data_len, http_transferdecoder_result **result, W *result_len)
{
	switch (decoder->type) {
	case HTTP_TRANSFERCODING_TYPE_IDENTITY:
		return http_transferdecoderidentity_inputentitybody(&decoder->d.identity, data, data_len, result, result_len);
	case HTTP_TRANSFERCODING_TYPE_CHUNKED:
		return http_transferdecoderchunked_inputentitybody(&decoder->d.chunked, data, data_len, result, result_len);
	}
	return -1; /* TODO */
}

EXPORT VOID http_transferdecoder_initialize(http_transferdecoder_t *decoder, HTTP_TRANSFERCODING_TYPE type, W content_length)
{
	switch (type) {
	case HTTP_TRANSFERCODING_TYPE_IDENTITY:
		http_transferdecoderidentity_initialize(&decoder->d.identity, content_length);
		break;
	case HTTP_TRANSFERCODING_TYPE_CHUNKED:
		http_transferdecoderchunked_initialize(&decoder->d.chunked);
		break;
	}
	decoder->type = type;
}

EXPORT VOID http_transferdecoder_finalize(http_transferdecoder_t *decoder)
{
	switch (decoder->type) {
	case HTTP_TRANSFERCODING_TYPE_IDENTITY:
		http_transferdecoderidentity_finalize(&decoder->d.identity);
		break;
	case HTTP_TRANSFERCODING_TYPE_CHUNKED:
		http_transferdecoderchunked_finalize(&decoder->d.chunked);
		break;
	}
}

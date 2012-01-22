/*
 * http_contentdecoder.c
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

#include "http_contentdecoder.h"

#include	<basic.h>
#include	<bstdio.h>
#include	<util/zlib.h>

#include    "http_typedef.h"

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

LOCAL W http_contentdecoderidentity_inputentitybody(http_contentdecoderidentity_t *decoder, UB *data, W data_len)
{
	decoder->state = HTTP_CONTENTDECODERIDENTITY_STATE_DATA;
	decoder->data = data;
	decoder->data_len = data_len;
	return 0;
}

LOCAL W http_contentdecoderidentity_inputendofdata(http_contentdecoderidentity_t *decoder)
{
	decoder->state = HTTP_CONTENTDECODERIDENTITY_STATE_END;
	return 0;
}

LOCAL W http_contentdecoderidentity_outputdata(http_contentdecoderidentity_t *decoder, http_contentdecoder_result **result, W *result_len, Bool *need_next)
{
	switch (decoder->state) {
	case HTTP_CONTENTDECODERIDENTITY_STATE_DATA:
		decoder->result.type = HTTP_CONTENTDECODER_RESULTTYPE_DATA;
		decoder->result.data = decoder->data;
		decoder->result.len = decoder->data_len;
		*result = &decoder->result;
		*result_len = 1;
		*need_next = True;
		break;
	case HTTP_CONTENTDECODERIDENTITY_STATE_END:
		decoder->result.type = HTTP_CONTENTDECODER_RESULTTYPE_END;
		decoder->result.data = NULL;
		decoder->result.len = 0;
		*result = &decoder->result;
		*result_len = 1;
		*need_next = False;
		break;
	}

	return 0;
}

LOCAL W http_contentdecoderidentity_initialize(http_contentdecoderidentity_t *decoder)
{
	decoder->state = HTTP_CONTENTDECODERIDENTITY_STATE_DATA;
	decoder->data = NULL;
	decoder->data_len = 0;
	return 0;
}

LOCAL VOID http_contentdecoderidentity_finalize(http_contentdecoderidentity_t *decoder)
{
}

/**/

LOCAL W gzip_headerskip_inputchar(gzip_headerskip_t *gzip, UB ch, Bool *is_end)
{
	*is_end = False;

	switch (gzip->state) {
	case GZIP_HEADERSKIP_STATE_MAGIC_NUMBER_FIRST:
		if (ch != 0x1F) {
			return -1; /* TODO: format error */
		}
		gzip->state = GZIP_HEADERSKIP_STATE_MAGIC_NUMBER_SECOND;
		break;
	case GZIP_HEADERSKIP_STATE_MAGIC_NUMBER_SECOND:
		if (ch != 0x8B) {
			return -1; /* TODO: format error */
		}
		gzip->state = GZIP_HEADERSKIP_STATE_METHOD;
		break;
	case GZIP_HEADERSKIP_STATE_METHOD:
		if (ch != Z_DEFLATED) {
			return -1; /* TODO: format error */
		}
		gzip->state = GZIP_HEADERSKIP_STATE_FLAGS;
		break;
	case GZIP_HEADERSKIP_STATE_FLAGS:
		gzip->flags = ch;
		gzip->state = GZIP_HEADERSKIP_STATE_SKIP_UNUSED_FIELD;
		break;
	  case GZIP_HEADERSKIP_STATE_SKIP_UNUSED_FIELD:
		gzip->unused--;
		if (gzip->unused != 0) {
			break;
		}
		if (gzip->flags & 0x04) {
			gzip->state = GZIP_HEADERSKIP_STATE_EXTRA_FIELD_LEN_FIRST;
		} else if (gzip->flags & 0x08) {
			gzip->state = GZIP_HEADERSKIP_STATE_SKIP_FILE_NAME;
		} else if (gzip->flags & 0x10) {
			gzip->state = GZIP_HEADERSKIP_STATE_SKIP_FILE_COMMENT;
		} else if (gzip->flags & 0x02) {
			gzip->state = GZIP_HEADERSKIP_STATE_SKIP_HEADER_CRC_FIRST;
		} else {
			*is_end = True;
		}
		break;
	case GZIP_HEADERSKIP_STATE_EXTRA_FIELD_LEN_FIRST:
		gzip->extra_len = (W)ch;
		gzip->state = GZIP_HEADERSKIP_STATE_EXTRA_FIELD_LEN_SECOND;
		break;
	case GZIP_HEADERSKIP_STATE_EXTRA_FIELD_LEN_SECOND:
		gzip->extra_len += (W)ch << 8;
		gzip->state = GZIP_HEADERSKIP_STATE_SKIP_EXTRA_FIELD;
		break;
	case GZIP_HEADERSKIP_STATE_SKIP_EXTRA_FIELD:
		gzip->extra_len--;
		if (gzip->extra_len != 0) {
			break;
		}
		if (gzip->flags & 0x08) {
			gzip->state = GZIP_HEADERSKIP_STATE_SKIP_FILE_NAME;
		} else if (gzip->flags & 0x10) {
			gzip->state = GZIP_HEADERSKIP_STATE_SKIP_FILE_COMMENT;
		} else if (gzip->flags & 0x02) {
			gzip->state = GZIP_HEADERSKIP_STATE_SKIP_HEADER_CRC_FIRST;
		} else {
			*is_end = True;
		}
		break;
	case GZIP_HEADERSKIP_STATE_SKIP_FILE_NAME:
		if (ch != '\0') {
			break;
		}
		if (gzip->flags & 0x10) {
			gzip->state = GZIP_HEADERSKIP_STATE_SKIP_FILE_COMMENT;
		} else if (gzip->flags & 0x02) {
			gzip->state = GZIP_HEADERSKIP_STATE_SKIP_HEADER_CRC_FIRST;
		} else {
			*is_end = True;
		}
		break;
	  case GZIP_HEADERSKIP_STATE_SKIP_FILE_COMMENT:
		if (ch != '\0') {
			break;
		}
		if (gzip->flags & 0x02) {
			gzip->state = GZIP_HEADERSKIP_STATE_SKIP_HEADER_CRC_FIRST;
		} else {
			*is_end = True;
		}
		break;
	  case GZIP_HEADERSKIP_STATE_SKIP_HEADER_CRC_FIRST:
		gzip->state = GZIP_HEADERSKIP_STATE_SKIP_HEADER_CRC_SECOND;
		break;
	  case GZIP_HEADERSKIP_STATE_SKIP_HEADER_CRC_SECOND:
		*is_end = True;
	}

	return 0;
}

LOCAL VOID gzip_headerskip_initialize(gzip_headerskip_t *gzip)
{
	gzip->state = GZIP_HEADERSKIP_STATE_MAGIC_NUMBER_FIRST;
	gzip->flags = 0;
	gzip->extra_len = 0;
	gzip->unused = 6;
}

LOCAL VOID gzip_headerskip_finalize(gzip_headerskip_t *gzip)
{
}

/**/

LOCAL W http_contentdecodergzip_inputentitybody(http_contentdecodergzip_t *decoder, UB *data, W data_len)
{
	W i, err;
	Bool is_end;

	switch (decoder->state) {
	case HTTP_CONTENTDECODERGZIP_STATE_SKIP_HEADER:
		DP_STATE("HTTP_CONTENTDECODERGZIP_STATE_SKIP_HEADER");
		for (i = 0; i < data_len; i++) {
			err = gzip_headerskip_inputchar(&decoder->gzipheader, data[i], &is_end);
			if (err < 0) {
				return err;
			}
			if (is_end != False) {
				decoder->state = HTTP_CONTENTDECODERGZIP_STATE_DECODE;
				decoder->z.next_in = data + i + 1;
				decoder->z.avail_in = data_len - (i + 1);
				break;
			}
		}
		break;
	case HTTP_CONTENTDECODERGZIP_STATE_DECODE:
		DP_STATE("HTTP_CONTENTDECODERGZIP_STATE_DECODE");
		decoder->z.next_in = data;
		decoder->z.avail_in = data_len;
		break;
	case HTTP_CONTENTDECODERGZIP_STATE_END_OF_DATA:
		DP_STATE("HTTP_CONTENTDECODERGZIP_STATE_END_OF_DATA");
		return -1; /* TODO */
	}

	return 0;
}

LOCAL W http_contentdecodergzip_inputendofdata(http_contentdecodergzip_t *decoder)
{
	decoder->state = HTTP_CONTENTDECODERGZIP_STATE_END_OF_DATA;
	return 0;
}

LOCAL W http_contentdecodergzip_outputdata(http_contentdecodergzip_t *decoder, http_contentdecoder_result **result, W *result_len, Bool *need_input)
{
	W err;

	switch (decoder->state) {
	case HTTP_CONTENTDECODERGZIP_STATE_SKIP_HEADER:
		*need_input = True;
		*result = NULL;
		*result_len = 0;
		break;
	case HTTP_CONTENTDECODERGZIP_STATE_DECODE:
		if (decoder->z.avail_in == 0) {
			*need_input = True;
			*result = NULL;
			*result_len = 0;
			break;
		}
		*need_input = False;

		*result = decoder->result;

		err = inflate(&(decoder->z), Z_NO_FLUSH);
		if (err == Z_STREAM_END) {
			decoder->result[0].type = HTTP_CONTENTDECODER_RESULTTYPE_DATA;
			decoder->result[0].data = decoder->buffer;
			decoder->result[0].len = HTTP_CONTENTDECODERGZIP_BUFLEN - decoder->z.avail_out;
			*result_len = 1;
			decoder->state = HTTP_CONTENTDECODERGZIP_STATE_END_OF_DATA;
			break;
		}
		if (err != Z_OK) {
			DP_ER("inflate error:", err);
			return -1;
		}
		if (decoder->z.avail_out == 0) {
			decoder->result[0].type = HTTP_CONTENTDECODER_RESULTTYPE_DATA;
			decoder->result[0].data = decoder->buffer;
			decoder->result[0].len = HTTP_CONTENTDECODERGZIP_BUFLEN;
			*result_len = 1;
			decoder->z.next_out = decoder->buffer;
			decoder->z.avail_out = HTTP_CONTENTDECODERGZIP_BUFLEN;
			return 0;
		}

		*result_len = 0;

		break;
	case HTTP_CONTENTDECODERGZIP_STATE_END_OF_DATA:
		decoder->result[0].type = HTTP_CONTENTDECODER_RESULTTYPE_END;
		decoder->result[0].data = NULL;
		decoder->result[0].len = 0;
		*result_len = 1;
		*need_input = False;
		break;
	}

	return 0;
}

LOCAL W http_contentdecodergzip_initialize(http_contentdecodergzip_t *decoder)
{
	W err;

	decoder->z.zalloc = Z_NULL;
	decoder->z.zfree = Z_NULL;
	decoder->z.opaque = Z_NULL;
	decoder->z.next_in = Z_NULL;
	decoder->z.avail_in = 0;
	err = inflateInit2(&(decoder->z), -MAX_WBITS);
	if (err != Z_OK) {
		DP_ER("inflateInit2 error:", err);
		return -1;
	}
	decoder->z.next_out = decoder->buffer;
	decoder->z.avail_out = HTTP_CONTENTDECODERGZIP_BUFLEN;

	gzip_headerskip_initialize(&decoder->gzipheader);
	decoder->state = HTTP_CONTENTDECODERGZIP_STATE_SKIP_HEADER;

	return 0;
}

LOCAL VOID http_contentdecodergzip_finalize(http_contentdecodergzip_t *decoder)
{
	gzip_headerskip_finalize(&decoder->gzipheader);
	inflateEnd(&(decoder->z));
}

/**/

EXPORT W http_contentdecoder_inputentitybody(http_contentdecoder_t *decoder, UB *data, W data_len)
{
	switch (decoder->type) {
	case HTTP_CONTENTCODING_VALUE_IDENTITY:
		return http_contentdecoderidentity_inputentitybody(&decoder->d.identity, data, data_len);
	case HTTP_CONTENTCODING_VALUE_GZIP:
		return http_contentdecodergzip_inputentitybody(&decoder->d.gzip, data, data_len);
	case HTTP_CONTENTCODING_VALUE_COMPRESS: /* unsupported yet */
	case HTTP_CONTENTCODING_VALUE_DEFLATE: /* unsupported yet */
	}
	return -1;
}

EXPORT W http_contentdecoder_inputendofdata(http_contentdecoder_t *decoder)
{
	switch (decoder->type) {
	case HTTP_CONTENTCODING_VALUE_IDENTITY:
		return http_contentdecoderidentity_inputendofdata(&decoder->d.identity);
	case HTTP_CONTENTCODING_VALUE_GZIP:
		return http_contentdecodergzip_inputendofdata(&decoder->d.gzip);
	case HTTP_CONTENTCODING_VALUE_COMPRESS: /* unsupported yet */
	case HTTP_CONTENTCODING_VALUE_DEFLATE: /* unsupported yet */
	}
	return -1;
}

EXPORT W http_contentdecoder_outputdata(http_contentdecoder_t *decoder, http_contentdecoder_result **result, W *result_len, Bool *need_input)
{
	switch (decoder->type) {
	case HTTP_CONTENTCODING_VALUE_IDENTITY:
		return http_contentdecoderidentity_outputdata(&decoder->d.identity, result, result_len, need_input);
	case HTTP_CONTENTCODING_VALUE_GZIP:
		return http_contentdecodergzip_outputdata(&decoder->d.gzip, result, result_len, need_input);
	case HTTP_CONTENTCODING_VALUE_COMPRESS: /* unsupported yet */
	case HTTP_CONTENTCODING_VALUE_DEFLATE: /* unsupported yet */
	}
	return -1;
}

EXPORT W http_contentdecoder_initialize(http_contentdecoder_t *decoder, HTTP_CONTENTCODING_VALUE type)
{
	decoder->type = type;
	switch (type) {
	case HTTP_CONTENTCODING_VALUE_IDENTITY:
		return http_contentdecoderidentity_initialize(&decoder->d.identity);
	case HTTP_CONTENTCODING_VALUE_GZIP:
		return http_contentdecodergzip_initialize(&decoder->d.gzip);
	case HTTP_CONTENTCODING_VALUE_COMPRESS: /* unsupported yet */
	case HTTP_CONTENTCODING_VALUE_DEFLATE: /* unsupported yet */
	}
	return -1;
}

EXPORT VOID http_contentdecoder_finalize(http_contentdecoder_t *decoder)
{
	switch (decoder->type) {
	case HTTP_CONTENTCODING_VALUE_IDENTITY:
		http_contentdecoderidentity_finalize(&decoder->d.identity);
		break;
	case HTTP_CONTENTCODING_VALUE_GZIP:
		http_contentdecodergzip_finalize(&decoder->d.gzip);
		break;
	case HTTP_CONTENTCODING_VALUE_COMPRESS: /* unsupported yet */
	case HTTP_CONTENTCODING_VALUE_DEFLATE: /* unsupported yet */
	}
}

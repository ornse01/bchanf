/*
 * texteditor_characterstate.h
 *
 * Copyright (c) 2013 project bchan
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

#include    <basic.h>

#include	<tad/tadfragment.h>
#include	<tad/tadlangcode.h>

#ifndef __TEXTEDITOR_CHARACTERSTATE_H__
#define __TEXTEDITOR_CHARACTERSTATE_H__

/* Functionality name: texteditor */
/* Detail name: characterstate */
struct texteditor_characterstate_t_ {
	RATIO w_ratio;
	tadlangcode lang;
};
typedef struct texteditor_characterstate_t_ texteditor_characterstate_t;

IMPORT VOID texteditor_characterstate_initialize(texteditor_characterstate_t *state);
IMPORT VOID texteditor_characterstate_finalize(texteditor_characterstate_t *state);
IMPORT W texteditor_charactorstate_input(texteditor_characterstate_t *state, tadfragment_cursor_segment *segment);
IMPORT Bool texteditor_characterstate_ishankaku(texteditor_characterstate_t *state);
IMPORT W texteditor_characterstate_getlang(texteditor_characterstate_t *state, TC *str, W len);
IMPORT Bool texteditor_characterstate_islang(texteditor_characterstate_t *state, TC *str, W len);

#endif

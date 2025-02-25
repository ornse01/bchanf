/*
 * tokenchecker.c
 *
 * Copyright (c) 2009-2011 project bchan
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

#include	<basic.h>
#include	<bstdlib.h>
#include	<bstdio.h>
#include	<bstring.h>
#include	<bctype.h>

#include    "tokenchecker.h"

#ifdef BCHAN_CONFIG_DEBUG
# define DP(arg) printf arg
# define DP_ER(msg, err) printf("%s (%d/%x)\n", msg, err>>16, err)
#else
# define DP(arg) /**/
# define DP_ER(msg, err) /**/
#endif

#define TOKENCHECKER_FLAG_NOTEXIST 0x00000001
#define TOKENCHECKER_FLAG_AFTERENDCHAR 0x00000002

EXPORT VOID tokenchecker_initialize(tokenchecker_t *checker, tokenchecker_valuetuple_t *namelist, W namelistnum, B *endchars)
{
	checker->namelist = namelist;
	checker->namelistnum = namelistnum;
	checker->endtokens = endchars;
	checker->stringindex = 0;
	checker->listindex_start = 0;
	checker->listindex_end = checker->namelistnum;
	checker->flag = 0;
}

EXPORT VOID tokenchecker_finalize(tokenchecker_t *checker)
{
}

EXPORT VOID tokenchecker_clear(tokenchecker_t *checker)
{
	checker->stringindex = 0;
	checker->listindex_start = 0;
	checker->listindex_end = checker->namelistnum;
	checker->flag = 0;
}

EXPORT TOKENCHECKER_RESULT tokenchecker_inputchar(tokenchecker_t *checker, UB c, W *val)
{
	W i;
	tokenchecker_valuetuple_t *namelist = checker->namelist;

	if ((checker->flag & TOKENCHECKER_FLAG_AFTERENDCHAR) != 0) {
		return TOKENCHECKER_RESULT_AFTER_END;
	}

	for (i = 0;; i++) {
		if ((checker->endtokens)[i] == '\0') {
			break;
		}
		if (c == (checker->endtokens)[i]) {
			checker->flag |= TOKENCHECKER_FLAG_AFTERENDCHAR;
			if ((checker->flag & TOKENCHECKER_FLAG_NOTEXIST) != 0) {
				return TOKENCHECKER_RESULT_NOMATCH;
			}
			if ((namelist[checker->listindex_start]).name[checker->stringindex] == '\0') {
				/*List's Name End and receive EndToken = found match string*/
				*val = (namelist[checker->listindex_start]).val;
				return TOKENCHECKER_RESULT_DETERMINE;
			}
			/*List's Name continue but receive endtoken.*/
			return TOKENCHECKER_RESULT_NOMATCH;
		}
	}

	if ((checker->flag & TOKENCHECKER_FLAG_NOTEXIST) != 0) {
		return TOKENCHECKER_RESULT_CONTINUE_NOMATCH;
	}

	for (i = checker->listindex_start; i < checker->listindex_end; i++) {
		if ((namelist[i]).name[checker->stringindex] == c) {
			break;
		}
	}
	if (i == checker->listindex_end) { /*receive char is not matched.*/
		checker->flag &= TOKENCHECKER_FLAG_NOTEXIST;
		return TOKENCHECKER_RESULT_CONTINUE_NOMATCH;
	}
	checker->listindex_start = i;
	for (i = i+1; i < checker->listindex_end; i++) {
		if ((namelist[i]).name[checker->stringindex] != c) {
			break;
		}
	}
	checker->listindex_end = i;

	if ((namelist[checker->listindex_start]).name[checker->stringindex] == '\0') {
		/*Don't recive endtoken but List's Name is end.*/
		checker->flag |= TOKENCHECKER_FLAG_NOTEXIST;
		return TOKENCHECKER_RESULT_CONTINUE_NOMATCH;
	}
	checker->stringindex++;

	return TOKENCHECKER_RESULT_CONTINUE;
}

EXPORT TOKENCHECKER_RESULT tokenchecker_endinput(tokenchecker_t *checker, W *val)
{
	/* should be more efficient? */
	return tokenchecker_inputchar(checker, checker->endtokens[0], val);
}

EXPORT VOID tokenchecker_getlastmatchedstring(tokenchecker_t *checker, UB **str, W *len)
{
	*str = (checker->namelist[checker->listindex_start]).name;
	*len = checker->stringindex;
}

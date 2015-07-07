/*
  Copyright 1988-1998 Mark Emmer, Catspaw, Inc.  All Rights Reserved.
  Use Of this source code is governed by a BSD-style
  license that can be found in the LICENSE file.
*/

#include "extrn88.h"

/*
 * strncpynf(s1, s2, n) - copy n characters from far C string s2 to near string s1
 */
char near *strncpynf(char near *s1, char far *s2, size_t n)
{
	char near *p = s1;

	while (n-- && ((*s1++ = *s2++) != 0))
		;
	while (n-- > 0)
		*s1++ = 0;
	return p;
}

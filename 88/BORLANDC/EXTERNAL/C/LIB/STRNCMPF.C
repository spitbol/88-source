/*
  Copyright 1988-1998 Mark Emmer, Catspaw, Inc.  All Rights Reserved.
  Use Of this source code is governed by a BSD-style
  license that can be found in the LICENSE file.
*/

#include "extrn88.h"

/*
 * strncmpfn(s1, s2, n) - compare n characters, far string s1, near string s2
 */
int strncmpfn(char far *s1, char near *s2, size_t n)
{
	register char c1, c2;

	while (n--)
		if ((c1 = *s1++) != (c2 = *s2++))
			return c1 > c2 ? 1 : -1;
	return 0;
}

/*
  Copyright 1988-1998 Mark Emmer, Catspaw, Inc.  All Rights Reserved.
  Use Of this source code is governed by a BSD-style
  license that can be found in the LICENSE file.
*/
`
#include "extrn88.h"

/*
 * memcpyfn(s1, s2, n) - copy n characters from near string s2 to far string s1.
 * 						 Does not stop on null character, no padding either.
 */
char far *memcpyfn(char far *s1, char near *s2, size_t n)
{
	char far *p = s1;

	while (n--)
		*s1++ = *s2++;
	return p;
}

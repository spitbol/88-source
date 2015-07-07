/*
  Copyright 1988-1998 Mark Emmer, Catspaw, Inc.  All Rights Reserved.
  Use Of this source code is governed by a BSD-style
  license that can be found in the LICENSE file.
*/

#include "extrn88.h"

/*
 * strcpyfn(s1, s2) - copy near C string s2 to far C string s1
 */

char far *strcpyfn(char far *s1, char near *s2)
{
	char far *p = s1;

	while ((*s1++ = *s2++) != 0)
		;
	return p;
}

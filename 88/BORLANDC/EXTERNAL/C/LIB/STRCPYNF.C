/*
  Copyright 1988-1998 Mark Emmer, Catspaw, Inc.  All Rights Reserved.
  Use Of this source code is governed by a BSD-style
  license that can be found in the LICENSE file.
*/

#include "extrn88.h"

/*
 * strcpynf(s1, s2) - copy far C string s2 to near C string s1
 */

char near *strcpynf(char near *s1, char far *s2)
{
	char near *p = s1;

	while ((*s1++ = *s2++) != 0)
		;
	return p;
}

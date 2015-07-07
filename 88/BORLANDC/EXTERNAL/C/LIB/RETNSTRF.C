/*
  Copyright 1988-1998 Mark Emmer, Catspaw, Inc.  All Rights Reserved.
  Use Of this source code is governed by a BSD-style
  license that can be found in the LICENSE file.
*/

#include "extrn88.h"

/*
 * retnstrf(s, n, presult) - Return n-char far string
 */

word retnstrf(char far *s, size_t n, union block far *presult)
{
	presult->fsb.fsptr = s;			/* return far string pointer	*/
	presult->fsb.fslen = n;			/* set string length			*/
	return BL_FS;
}

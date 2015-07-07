/*
  Copyright 1988-1998 Mark Emmer, Catspaw, Inc.  All Rights Reserved.
  Use Of this source code is governed by a BSD-style
  license that can be found in the LICENSE file.
*/

#include "extrn88.h"

/*
 * retnxdtf(s, n, presult) - Return n-char far external data
 */

word retnxdtf(void far *s, size_t n, union block far *presult)
{
	presult->fxb.fxptr = s;			/* return far data pointer		*/
	presult->fxb.fxlen = n;			/* set data length				*/
	return BL_FX;
}

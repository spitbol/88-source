/*
  Copyright 1988-1998 Mark Emmer, Catspaw, Inc.  All Rights Reserved.
  Use Of this source code is governed by a BSD-style
  license that can be found in the LICENSE file.
*/

#include "extrn88.h"

/*
 * retnstrn(s, n, presult) - Return n-char near string
 */

word retnstrn(char near *s, size_t n, union block far *presult)
{
	if (n > buflen)					/* limit to size of result block*/
		n = buflen;
	memcpyfn(presult->scb.scstr, s, n);	/* copy near to far			*/
	presult->scb.sclen = n;			/* set string length			*/
	return BL_SC;
}

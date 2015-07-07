/*
  Copyright 1988-1998 Mark Emmer, Catspaw, Inc.  All Rights Reserved.
  Use Of this source code is governed by a BSD-style
  license that can be found in the LICENSE file.
*/

#include "extrn88.h"

/*
 * retnxdtn(s, n, presult) - Return n-char near external data
 */

word retnxdtn(void near *s, size_t n, union block far *presult)
{
	if (n > buflen)					/* limit to size of result block*/
		n = buflen;
	memcpyfn((void far *)presult->xnb.xnu.xndta, s, n);	/* copy near to far	*/
	presult->xnb.xnlen = n;			/* set data length				*/
	return BL_XN;
}

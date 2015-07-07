/*
  Copyright 1988-1998 Mark Emmer, Catspaw, Inc.  All Rights Reserved.
  Use Of this source code is governed by a BSD-style
  license that can be found in the LICENSE file.
*/

#include "extrn88.h"

/*
 * retstrn(s, presult) - Return near C string
 */

word retstrn(char near *s, union block far *presult)
{
	strcpyfn(presult->scb.scstr, s);/* copy near to far				*/
	presult->scb.sclen = strlen(s);	/* set string length			*/
	return BL_SC;
}

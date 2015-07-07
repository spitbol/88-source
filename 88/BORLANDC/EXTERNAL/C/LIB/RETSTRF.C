/*
  Copyright 1988-1998 Mark Emmer, Catspaw, Inc.  All Rights Reserved.
  Use Of this source code is governed by a BSD-style
  license that can be found in the LICENSE file.
*/

#include "extrn88.h"

/*
 * retstrf(s, presult) - Return far C string
 */

word retstrf(char far *s, union block far *presult)
{
	presult->fsb.fsptr = s;			/* return far string pointer	*/
	presult->fsb.fslen = strlenf(s);/* set string length			*/
	return BL_FS;
}

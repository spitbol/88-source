/*
  Copyright 1988-1998 Mark Emmer, Catspaw, Inc.  All Rights Reserved.
  Use Of this source code is governed by a BSD-style
  license that can be found in the LICENSE file.
*/

#define IA32 1				/* always pass 32-bit integer arguments */
#include "extrn88.h"

/*
 * retint(val, presult) - Return integer
 */

word retint(long val, union block far *presult)
{
	presult->icb.icval = val;
	return BL_IC;
}

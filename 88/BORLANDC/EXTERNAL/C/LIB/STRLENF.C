/*
  Copyright 1988-1998 Mark Emmer, Catspaw, Inc.  All Rights Reserved.
  Use Of this source code is governed by a BSD-style
  license that can be found in the LICENSE file.
*/

#include "extrn88.h"

/*
 * strlenf(s) - return length of far string
 */

size_t strlenf(char far *s)
{
	size_t i = 0;

	while (*s++)
		i++;
	return i;
}

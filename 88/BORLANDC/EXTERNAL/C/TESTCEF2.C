/*
  Copyright 1988-1998 Mark Emmer, Catspaw, Inc.  All Rights Reserved.
  Use Of this source code is governed by a BSD-style
  license that can be found in the LICENSE file.
*/

/*
 * testcef2.c
 *
 * Test access to arguments and return of integer:
 * 	LOAD("TESTCEF2(INTEGER,REAL,STRING)")
 *
 * 	Call with TESTCEF2(100, 1000.0, "ABCD")
 *
 * 	Returns arg 1 plus 1 if arguments were found as expected,
 * 	otherwise a message about which argument was bad.
 */

#include "system.h"
#include "extrn88.h"
#include <string.h>

#define testarg1 100
#define testarg2 1000.0
#define testarg3 "ABCD"

entry(TESTCEF2)(presult, pinfo, parg3, larg3, rarg2, iarg1)
long	   		iarg1;			 /* arg1 integer			*/
double	   		rarg2;			 /* arg2 real number		*/
word	   		larg3;			 /* arg3 length				*/
char far   		*parg3;			 /* pointer to arg3 string	*/
misc far    	*pinfo;		     /* pointer to misc info	*/
union block far	*presult;		 /* pointer to result area	*/
{

	if (iarg1 != testarg1)
		return retstrn("First argument not integer 100", presult);

	if (rarg2 != testarg2)
		return retstrn("Second argument not real 1000.0", presult);

	if (larg3 != strlen(testarg3))
		return retstrn("Third argument string length wrong", presult);

	if (strncmpfn(parg3, testarg3, larg3))
		return retstrn("Third argument not string 'ABCD'", presult);

	return retint(iarg1 + 1, presult);	/* Arguments are OK, return arg1+1	*/
}

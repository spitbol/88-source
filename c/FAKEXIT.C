/*
 Copyright (C)  1988-1998  Mark Emmer

 SPITBOL is free software: you can redistribute it and/or modify
 it under the terms of the GNU General Public License as published by
 the Free Software Foundation, either version 2 of the License, or
 (at your option) any later version.

 This program is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 GNU General Public License for more details.

 You should have received a copy of the GNU General Public License
 along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

/*
/	File:  FAKEXIT.C	Version:  01.00
/	---------------------------------------
/
/	Contents:	Function exit
*/
/*  Copyright 1991 Robert Goldberg and Catspaw, Inc. */

/*
/   exit()
/
/   This is a "fake" exit() function that prevents the linker from linking
/   in the standard C exit() function with all the associated stdio library
/   functions.
*/
#include "port.h"
#if !VCC
void exit(status)
int status;
{}
#endif

extern void _exit Params((int status));
#if BCC16 | BCC32 | VCC | AIX
extern void exit_custom Params((int code));
#endif
					/* BCC16 */
void __exit(code)
int code;
{
#if BCC16 | BCC32 | VCC | AIX
	exit_custom(code);				/* Perform system specific shutdown */
#endif					/* BCC16 */

	_exit(code);
}

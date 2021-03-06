/*
 Copyright 1991 Robert Goldberg and Catspaw, Inc.

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
/	File:  SYSEN.C		Version:  01.02
/	---------------------------------------
/
/	Contents:	Function zysen
*/

/*
/	zysen - endfile
/
/	endfile is an artifact from the FORTRAN days and is supposed to
/	close a file.  However, the file may be reopened, etc.  We just
/	close it.
/
/	Parameters:
/	    WA - FCBLK pointer or 0
/	    XR - SCBLK pointer (ENDFILE argument)
/	Returns:
/	    Nothing
/	Exits:
/	    1 - file does not exist
/	    2 - inappropriate file
/	    3 - i/o error
/
/	V1.02 29-Mar-91	Check for error after calling osclose().
*/

#include "port.h"

zysen()
{
	register struct fcblk *fcb = WA (struct fcblk *);
	register struct ioblk *iob = MK_MP(fcb->iob, struct ioblk *);

	/* ensure the file is open */
	if ( !(iob->flg1 & IO_OPN) )
		return EXIT_1;

	/* now close it */
	if (osclose( iob ))
		return EXIT_3;

	return NORMAL_RETURN;
}

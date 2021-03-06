/*
 Copyright (c)  1991 Robert Goldberg and Catspaw, Inc.

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
/	File:  SYSIN.C		Version:  01.03
/	---------------------------------------
/
/	Contents:	Function zysin
*/

/*
/	zysin - read input record
/
/	zysin reads and returns the next input record from a file.
/
/	Parameters:
/	    WA - pointer to FCBLK or 0
/	    XR - pointer to SCBLK containing buffer to receive record read
/	Returns:
/	    Nothing
/	Exits:
/	    1 - EOF or file not available after SYSXI
/	    2 - i/o error
/	    3 - record format error
/
/	V1.02	05-Mar-88	When reading an EOF from fd 0, call swcinp()
/				before calling it a true EOF.
/	V1.03	01-Feb-93	New osread calling sequence with separate mode and
/				line length fields.
*/

#include "port.h"

word wabs(x)
word x;
{
	return (x >= 0) ? x : -x;
}

zysin()
{
	register word	reclen;
	register struct fcblk *fcb = WA (struct fcblk *);
	register struct scblk *scb = XR (struct scblk *);
	register struct ioblk *ioptr = MK_MP(fcb->iob, struct ioblk *);

	/* ensure iob is open, fail if unsuccessful */
	if ( !(ioptr->flg1 & IO_OPN) )
		return EXIT_3;

	/* read the data, fail if unsuccessful */
	while( (reclen = osread( fcb->mode, fcb->rsz, ioptr, scb )) < 0)
	{
		if ( reclen == (word)-1 )		/* EOF?			*/
		{
			if ( ioptr->fdn )	/* If not fd 0, true EOF*/
			    return EXIT_1;
			else			/* Fd 0 - try to switch files */
			    if ( swcinp( inpcnt, inpptr ) < 0 )
				return EXIT_1;     /* If can't switch      */

		    ioptr->flg2 &= ~IO_RAW; /* Switched. Set IO_RAW */
		    if ( (testty( ioptr->fdn ) == 0 ) && /* If TTY */
			 ( fcb->mode == 0 ) )	/* and raw mode,   */
				ioptr->flg2 |= IO_RAW;   /* then set IO_RAW */
#if MSDOS | OS2 | WINNT
			if ( cindev( ioptr->fdn ) == 0 )	/* Test for character input */
				ioptr->flg1 |= IO_CIN;
			if ( fcb->mode == 0 )					/* set/clear binary bit for doset */
				ioptr->flg2 |= IO_BIN;
			else
				ioptr->flg2 &= ~IO_BIN;
#endif               /* MSDOS | OS2 | WINNT */

		}
		else				/* I/O Error		*/
			return EXIT_2;
	}
	scb->len = reclen;		/* set record length	*/

	/* normal return */
	return NORMAL_RETURN;
}

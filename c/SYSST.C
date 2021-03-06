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
/  File:  SYSST.C    Version:  01.07
/	---------------------------------------
/
/	Contents:	Function zysst
/
/  01.02 10-Oct-87 MBE	Return EXIT_5 if I/O error.
/			Return resulting file position in IA
/			(requiring non-standard mod to V36A.MIN).
/
/  01.03 29-Nov-89 MBE	Allow SET of a system file.
/
/  01.04 18-May-92 MBE  Provide PC-SPITBOL-style support for SET.
/
/  01.05 01-Feb-93 MBE	fcb->rsz is always positive now.
/
/  01.06 21-Oct-94 MBE  Use uppercase function to fold case letters.
/
/  01.07 19-Jul-97 MBE  Add SETREAL support to force SET to use real for
/                       offset argument and return value.  Used only for
/                       special version for select customers.
/
*/

/*
/   zysst - set file position
/
/   Parameters:
/	WA - FCBLK pointer
#if SETREAL
/  RA - 2nd argument (real number), offset
#else
/	WB - 2nd argument (might require conversion), offset
#endif
/	WC - 3rd argument (might require conversion), whence
/    Returns:
#if SETREAL
/  RA - File position
#else
/	IA - File position
#endif
/    Exits:
/	1 - invalid 2nd argument
/	2 - invlaid 3rd argument
/	3 - file does not exist
/	4 - set not allowed
/	5 - i/o error
/  
/  PC-SPITBOL option form of SET:
/    WB = 'P':
/  		  set position to WC
/    WB = 'H'
/  		  set position to WC * 32768 + (current_position mod 32768)
/    WB = 'R'
/  		  set position to current_position + WC
/    WB = 'E'
/  		  set position to end_of_file + WC
/    WB = 'C'
/  		  set record length to WC for byte-stream file
/    WB = 'D'
/  		  delete record -- not supported
/  
*/

#include "port.h"

zysst()

{
	IATYPE	whence, temp;
   FILEPOS  offset;
	register struct fcblk *fcb = WA (struct fcblk *);
	register struct ioblk *iob = MK_MP(fcb->iob, struct ioblk *);
	register struct icblk *icp;

	/* ensure iob is open, fail if unsuccessful */
	if ( !(iob->flg1 & IO_OPN) )
		return EXIT_3;

#if PIPES
	/* not allowed to do a set of a pipe */
	if ( iob->flg2 & IO_PIP )
		return EXIT_4;
#endif					/* PIPES */

	/* whence may come in either integer or string form */
	icp = WC( struct icblk * );
	if ( !getint(icp,&whence) )
		return EXIT_1;

#if SETREAL
   /* offset comes in as a real in RA */
   offset = RA(FILEPOS);
#else
   /* offset may come in either integer or string form */
	icp = WB( struct icblk * );
	if ( !getint(icp,&temp) ) {
		struct scblk *scp;
		scp = (struct scblk *)icp;
		if (!checkstr(scp) || scp->len != 1)
			return EXIT_1;
		temp = whence;
		switch (uppercase(scp->str[0])) {
			case 'P':
				whence = 0;
				break;

			case 'H':
				temp = (whence << 15) + ((int)doset(iob,0,1) & 0x7FFFL);
				whence = 0;
				break;

			case 'R':
				whence = 1;
				break;
				
			case 'E':
				whence = 2;
				break;
				
			case 'C':
				if ( fcb->mode == 0 && temp > 0 && temp <= (word)maxsize ) {
					fcb->rsz = temp;
					temp = 0;
					whence = 1;			/* return current position */
					break;
					}
				else {
					if (temp < 0 || temp > (word)maxsize)
						return EXIT_2;
					else
						return EXIT_1;
					}		

			default:
				return EXIT_1;		/* Unrecognised control */
			}	
		}	
   offset = (FILEPOS)temp;
#endif
	/*  finally, set the file position  */
	offset = doset( iob, offset, (int)whence );

	/*  test for error.  01.02 */
   if ( offset < (FILEPOS)0 )
		return EXIT_5;
#if SETREAL
   /*  return resulting position in RA.  01.07  */
   SET_RA( offset );
#else
	/*  return resulting position in IA.  01.02  */
	SET_IA( (IATYPE)offset );
#endif

	/* normal return */
	return NORMAL_RETURN;
}

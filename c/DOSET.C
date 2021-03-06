
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
/	File:  DOSET.C		Version:  01.04
/	---------------------------------------
/
/	Contents:	Function doset
*/

/*
/   doset( ioptr, offset, whence )
/
/   doset() does an "LSEEK" function call on the file described by ioptr.
/   For output files, the buffer must be flushed before doing the LSEEK.
/   For input file, any "unread" characters in the buffer must be seeked
/   over as well.
/
/   Parameters:
/	ioptr	pointer to IOBLK describing file
/  offset   offset for LSEEK call
/  whence   type of LSEEK to perform
/   Returns:
/  Value returned by LSEEK (-1 if error).
/
/   History:
/
/   v1.01  05-Feb-1991	Modified for read/write I/O.  Also, on seek to
/			EOF on MSDOS, back up over any EOT characters.
/
/	v1.02  13-May-1992  Changed offset argument and result type from
/						word to long.
/
/	V1.03	01-Aug-93	Add IO_EOT flag to ignore EOT char in DOS-mode
/						 text files.
/
/	V1.04	04-Jun-95	No longer have to clear IO_LF since it is only
/						used on a non-seekable device like a pipe.
*/

#include "port.h"

#if MSDOS | OS2 | WINNT
#define EOT	26			/* MS-DOS and OS/2 End of Text character	*/
#endif               /* MSDOS | OS2 | WINNT */

#if SETREAL
#include <math.h>       /* for floor() */
#endif

FILEPOS doset( ioptr, offset, whence )

struct	ioblk	*ioptr;
FILEPOS  offset;
int	whence;

{
	register struct	bfblk *bfptr = MK_MP(ioptr->bfb, struct bfblk *);
   FILEPOS target, newoffset;
	
	if (ioptr->flg2 & IO_PIP)
		return -1L;

	
	switch (whence) {
		case 0:								/* absolute position */
			target = offset;
			break;
		case 1:								/* relative to current position */
         target = offset + 
           (bfptr ? bfptr->offset + bfptr->next : LSEEK(ioptr->fdn, (FILEPOS)0, 1));
			break;
		case 2:								/* relative to EOF */
			target = offset + geteof(ioptr);
			break;
#if APPLE
/*******  KLUDGE **********/
		case 3:			/* designate/undesignate this file for keyboard input */
			if (fconsole(&_file[ioptr->fdn], offset ? TRUE : FALSE))
				return -1L;
         target = offset ? geteof(ioptr) : (FILEPOS)0;
			break;
/********* END OF KLUDGE **********/
#endif					/* APPLE */
		default:
			return -1;
		}
	
   if (target < (FILEPOS)0)
      target = (FILEPOS)0;
		
	if (bfptr) {
		/*
		 * see if target is within the present buffer
		 */
		if (bfptr->offset <= target &&
		 target <= bfptr->offset + bfptr->fill) {
			bfptr->next = (word)(target - bfptr->offset);
			return target;
			}

		/*
      /  Flush any dirty buffer before doing LSEEK.
		*/
		if (flush(ioptr))
			return -1;						/* return if error */

		/*
		/	Seek to a position that is a multiple of the buffer size.
		*/
#if SETREAL
      newoffset = floor(target / bfptr->size) * bfptr->size;
#else
      newoffset = (target / bfptr->size) * bfptr->size;
#endif      
      if (newoffset != bfptr->curpos)
      {
         /* physical file position differs from desired new offset */
         FILEPOS newcurrent;
         newcurrent = LSEEK(ioptr->fdn, newoffset, 0);        
         if (newcurrent < (FILEPOS)0)
           return -1;
         bfptr->offset = bfptr->curpos = newcurrent;        
      }
      else
      {
        /* file is properly positined already */
        bfptr->offset = newoffset;
      }  
		
		/*
		/	Now fill the buffer and position the next pointer carefully.
		*/
		if (testty(ioptr->fdn) && fillbuf(ioptr) < 0)
			return -1;

		bfptr->next = (word)(target - bfptr->offset);
		if (bfptr->next > bfptr->fill)	{			/* if extending beyond EOF */
			if (ioptr->flg1 & IO_OUP)
				bfptr->fill = bfptr->next;			/* only allow if output file */
			else
				bfptr->next = bfptr->fill;			/* otherwise, limit to true EOF */
			}

		return bfptr->offset + bfptr->next;
		}
	else
      return LSEEK(ioptr->fdn, target, 0); /* unbuffered I/O */
}

FILEPOS geteof(ioptr)
struct ioblk *ioptr;
{
	register struct	bfblk *bfptr = MK_MP(ioptr->bfb, struct bfblk *);
   FILEPOS eofpos, curpos;

	if (!bfptr)								/* if unbuffered file */
      curpos = LSEEK(ioptr->fdn, (FILEPOS)0, 1);   /*  record current position */
	
   eofpos = LSEEK(ioptr->fdn, (FILEPOS)0, 2);      /* get eof position */

#if MSDOS | OS2 | WINNT
	/* If end-of-file seek on text file, back up over any EOT characters */
	if (!(ioptr->flg2 & IO_BIN) && !(ioptr->flg1 & IO_EOT) && eofpos > 0) {
		char c;
		do {
         if (LSEEK(ioptr->fdn, --eofpos, 0) == -1)
				break;
			if (read(ioptr->fdn, &c, 1) != 1)
				break;
			} while (c == EOT && eofpos > 0);
		eofpos++;
		}
#endif               /* MSDOS | OS2 | WINNT */
	
	if (bfptr) {
		bfptr->curpos = eofpos;				/* buffered - record position */
		if (bfptr->offset + bfptr->fill > eofpos)	/* if buffer extended */
			eofpos = bfptr->offset + bfptr->fill;	/* beyond physical file */
		}
	else
      LSEEK(ioptr->fdn, curpos, 0);    /* unbuffered - restore position */
	
	
	return eofpos;
}

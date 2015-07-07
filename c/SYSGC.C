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
/	File:  SYSGC.C		Version:  01.01
/	---------------------------------------
/
/	zysgc - notification of system garbage collection
/
/	zysgc is called before and after a garbage collection.
/	Some systems may wish to take special action using this information.
/
/	Parameters:
/	    XR - flag for garbage collection
/		 <>0 garbage collection commencing
/		 =0  garbage collection concluding
/		WA - starting location of dynamic area
/		WB - next available location
/		WC - last available location
/	Returns
/	    Nothing
/	    Preserves all registers
/
/  Version history:
/	  v1.01 17-May-91 MBE
/		Add arguments in WA, WB, WC for use in discarding page
/       contents of freed memory on virtual memory systems.
/
*/

#include "port.h"
#include "save.h"

#if APPLE
#include "PEdit.h"
#endif

#if SUN3
#include <sys/vadvise.h>
#endif					/* SUN3 */

#if INTEL | HIGHC23
#include <dos.h>
#endif					/* INTEL | HIGH23 */


zysgc()
{
#if SUN3
	/* See if suppressing vadvise() calls	*/
	if ( !(spitflag & NOVADV) )
	{
	    /* Give advice to paging system */
		if ( XR(word) )
			vadvise(VA_ANOM);	/* collection beginning	*/
		else
			vadvise(VA_NORM);	/* collection ending	*/
	}
#endif					/* SUN3 */

#if APPLE
    /* Give visual indication by subtling changing cursor */
	if ( XR(word) )
		DifferBallCursor();	/* collection beginning	*/
	else
		UndifferBallCursor();	/* collection ending	*/
#endif					/* APPLE */

#if MSDOS && (INTEL || BCC32 | VCC)
	unsigned long firstloc, length;
	extern void dpmiDiscard(unsigned long firstloc, unsigned long length);

	if (!XR(word)) {			/* collection ending */
		firstloc = (WB(long) + PAGESIZE - 1) & ~(PAGESIZE-1);
		length = ((WC(long) + PAGESIZE) & ~(PAGESIZE-1)) - firstloc;
		dpmiDiscard(firstloc,length);
		}
#endif					/* INTEL */

#if HIGHC23
	union _REGS r;
	unsigned long firstloc, length;

	if (!XR(word)) {			/* collection ending */
		firstloc = (WB(long) + PAGESIZE - 1) & ~(PAGESIZE-1);
		length = (((WC(long) + PAGESIZE) & ~(PAGESIZE-1)) - firstloc) >> L2PGSZ;
		if (length) {
			r.x.ax = 0x251C;		/* VCPI Free Physical Memory Pages */
			r.x.bx = 0x0100;		/* discard page contents */
			r.x.cx = firstloc;		/* linear address of first page to free */
			r.x.dx = length;		/* number of pages to free */
			_intdos(&r, &r);
			}
		}
#endif					/* HIGHC23 */

	in_gbcol = XR(word);	/* retain information */
	return NORMAL_RETURN;
}

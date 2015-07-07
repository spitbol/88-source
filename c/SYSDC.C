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
/	zysdc - check system expiration date
/
/	zysdc prints any header messages and may check
/	the date to see if execution is allowed to proceed.
/
/	Parameters:
/	    Nothing
/	Returns
/	    Nothing
/	    No return if execution not permitted
/
*/

#include "port.h"

#if BCC16
#include <dos.h>
#endif

zysdc()
{
#if !APPLE & !(BCC16 & ENGINE)
	struct scblk *pHEADV = GET_DATA_OFFSET(HEADV,struct scblk *);
	/* announce name and copyright */
	if (!dcdone && !(spitflag & NOBRAG))
	{
		dcdone = 1;				/* Only do once per run */
#if HIGHC | INTEL | BCC32 | VCC | WINNT
		write( STDERRFD, "SPITBOL-386", 11);
#endif					/* HIGHC | INTEL */

#if BCC16
#if IA32
		write( STDERRFD, "SPITBOL-8088/32", 15);
#else
		write( STDERRFD, "SPITBOL-8088/16", 15);
#endif
#endif					/* BCC16 */

#if OS2
		write( STDERRFD, "SPITBOL-OS/2 2.0", 16);
#endif					/* OS2 */

#if ATT73 | SUN2 | SUN3 | NCR | UNISYS | APOLLO | HPUX | HPBSD | DSI
		write( STDERRFD, "SPITBOL-68K", 11);
#endif					/* ATT73 | ... | DSI */

#if VAX
		write( STDERRFD, "VAX SPITBOL", 11);
#endif					/* VAX */

#if SUN4
		write( STDERRFD, "SPARC SPITBOL", 13);
#endif					/* SUN4 */

#if MIPS
		write( STDERRFD, "MIPS SPITBOL", 12);
#endif

#if LINUX
		write( STDERRFD, "LINUX SPITBOL", 13);
#endif

#if AIX
		write( STDERRFD, "AIX SPITBOL", 11);
#endif

#if RUNTIME
		write( STDERRFD, " Runtime", 8);
#endif					/* RUNTIME */

		write( STDERRFD, "   Release ", 11);
		write( STDERRFD, pHEADV->str, pHEADV->len );
		write( STDERRFD, pID1->str, pID1->len );
		write( STDERRFD, "   Serial ", 10 );
		wrterr( SERIAL );
		wrterr( cprtmsg );
	}
#endif					/* !APPLE & !(BCC16 & ENGINE)  */

#if DATECHECK
#if WINNT
   {
      extern void date_check(void);
      date_check();
   }
#endif
#endif
		return NORMAL_RETURN;
}

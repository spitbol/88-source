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
/   File:  OSWAIT.C     Version:  01.02
/	---------------------------------------
/
/	Contents:	Function oswait
*/

/*
/   oswait( pid )
/
/   oswait() waits for the termination of the process with id pid.
/
/   Parameters:
/	pid	prcoess id
/   Returns:
/   nothing
/
/   V1.01 MBE 07-29-91	Modify for OS/2 signals and process mechanism.
/   V1.02 MBE 12-31-96  Modify for WinNT.
/
*/

#include "port.h"
#if PIPES

#if UNIX | OS2
#include <signal.h>

#if OS2
#undef SigType
#define SigType void
#define SIGQUIT SIGABRT
#define SIGHUP SIGBREAK
#endif					/* OS2 */
#endif                  /* UNIX | OS2 */

#if WINNT
#include <process.h>
#if _MSC_VER
extern int wait(int *status);
#endif
#endif

void oswait( pid )
int	pid;
{
	int	deadpid, status;
    struct  chfcb   *chptr;
#if UNIX | OS2
#if PROTOTYPES
	SigType (*hstat)Params((int)),
			(*istat)Params((int)),
			(*qstat)Params((int));
#else
	SigType (*hstat)(),
			(*istat)(),
			(*qstat)();
#endif             /* PROTOTYPES */

	istat	= signal( SIGINT, SIG_IGN );
	qstat	= signal( SIGQUIT ,SIG_IGN );
	hstat	= signal( SIGHUP, SIG_IGN );
#endif            /* UNIX | OS2 */

    while ( (deadpid = wait( &status )) != pid  &&  deadpid != -1 )
    {
		for ( chptr = GET_MIN_VALUE(R_FCB,struct chfcb *); chptr != 0;
         chptr = MK_MP(chptr->nxt, struct chfcb *) )
        {
			if ( deadpid == MK_MP(MK_MP(chptr->fcp, struct fcblk *)->iob,
             struct ioblk *)->pid )
            {
				MK_MP(MK_MP(chptr->fcp, struct fcblk *)->iob,
				 struct ioblk *)->flg2 |= IO_DED;
				break;
            }
        }
    }

#if UNIX | OS2
    signal( SIGINT,istat );
	signal( SIGQUIT,qstat );
	signal( SIGHUP,hstat );
#endif                  /* UNIX | OS2 */
}
#endif					/* PIPES */

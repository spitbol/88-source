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
/	File:  SYSPL.C		Version:  01.01
/	---------------------------------------
/
/	zyspl - interface polling from SPITBOL
/
/	zyspl is called before statement execution to allow the interface
/         to regain control if desired.
/	Parameters:
/	    WA - reason for call
/		     =0  periodic polling
/		     =1  breakpoint hit
/		     =2  completion of statement stepping
/     WB - current statement number
/           XL - SCBLK of result if WA = 3.
/	Normal Return
/	    WA - number of statements to elapse before calling SYSPL again.
/	Exits:
/	    1 - set breakpoint
/	    2 - single step
/	    3 - evaluate expression
/       normal exit - no special action
/
/  Version history:
/
/
*/

#include "port.h"

#if APPLE
#if ENGINE
#define	PollCount	50		/* Statement interval between polling */
#else					/* ENGINE */
#include "PEdit.h"
#include "iproto.h"
int pollDelay = PollInterval;
#endif					/* ENGINE */
int stmtDelay=PollCount;
#endif					/* APPLE */

#if POLLING & (HIGHC | BCC16)
int	pollevent Params((void));
#define rearmbrk()
extern char	brkpnd;
#define stmtDelay PollCount
#endif					/* POLLING & (HIGHC | BCC16 ) */

#if POLLING & (UNIX | INTEL | OS2 | BCC32 | VCC)
#if INTEL | (BCC32 | VCC)
int	pollevent Params((void));
#endif					/* INTEL */
#if UNIX | OS2
#define pollevent()
#endif					/* UNIX | OS2 */
extern  rearmbrk Params((void));
extern	int	brkpnd;
#define stmtDelay PollCount
#endif					/* POLLING & (UNIX | INTEL | OS2) */

#if POLLING & APPLE
#define rearmbrk()
char	brkpnd = 0;
#endif					/* POLLING & APPLE */


zyspl()
{
#if POLLING

	/* Make simple polling case the fastest by avoiding switch statement */
	if (WA(word) == 0) {
#if !ENGINE
		pollevent();
#endif					/* !ENGINE */
		SET_WA(stmtDelay);	/* Poll finished or Continue */
#if !ENGINE & (HIGHC | INTEL | BCC16 | BCC32 | VCC | OS2 | WINNT | UNIX | APPLE)
		if (brkpnd) {
			brkpnd = 0;		/* User interrupt */
			rearmbrk();		/* allow breaks again */
			return EXIT_1;
			}
#endif					/* !ENGINE & (HIGHC | INTEL | BCC16 | OS2 | UNIX | APPLE) */
		}
#else					/* POLLING */
	SET_WA((word)MAXPOSWORD);			/* Effectively shut off polling */
#endif					/* POLLING */
	return NORMAL_RETURN;
}




#if APPLE & !ENGINE
/*
 * pollevent - Check for user interruption.  PollInterval ticks must have elapsed since
 *			the last time poll for this to occur.
 */
int
pollevent()
{
	if (Ticks >= polltick) {
		if (PollEvent()) {
			/* User selected Pause from menu */
			if (!setjmp(resume_jump))			/* Record location for future Continue */
				EventLoop();					/* Run the user interface -- No Return */
			}
		polltick = Ticks + pollDelay;			/* Setup time for next poll */
		}
}


/*
 * HostPoll - Allow user program to initiate a poll.
 *  Selection changes are may be selectively enabled IF the beachball is off.
 *  Keystroke editing may be selectively enabled, independent of beachball.
 */
int
HostPoll(allowSelect, allowKey)
Boolean allowSelect, allowKey;
{
	int result;
	result = HostPollEvent(!beachBall & allowSelect, allowKey);
	if (SysState == PauseState) {
		/* User selected Pause from menu */
		if (!setjmp(resume_jump))			/* Record location for future Continue */
			EventLoop();					/* Run the user interface -- No Return */
		}
	polltick = Ticks + pollDelay;			/* Setup time for next poll */
	return result;
}




/*
 * DoContinueItem - After pausing, the user has selected Continue from the Compile menu.
 */
void
DoContinueItem()
{
	/* Restore the stack to where it was when compiler first polled us, and resume */
	longjmp(resume_jump,1);
}

#endif					/* APPLE & !ENGINE */

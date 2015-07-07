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
/	File:  SYSTM.C		Version:  01.03
/	---------------------------------------
/
/	Contents:	Function zystm
*/

/*
/	zystm - get execution time so far
/
/	zystm is called to obtain the amount of execution time used so far
/	since spitbol began execution.  The returned value is assumed to be
/	in milliseonds, except for 16-bit implementations, which return deciseconds.
/
/	Parameters:
/	    None
/	Returns:
/	    IA - execution time so far in milliseconds or deciseconds.
/
/	v1.03	27-May-95	For AIX, corrected use of tms_utime.  Was
/						multiplying by 100 / 6.  Should be 1000/CLK_TCK.
/						Was running fast by factor of 1.6.
*/

#include "port.h"

#if !APPLE
#if DSI | HIGHC | INTEL | BCC16 | BCC32 | VCC | OS2 | WINNT
extern long msec( void );
#else					/* DSI | HIGHC | INTEL | BCC16 | OS2 */
#include <sys/types.h>
#include <sys/times.h>
#if AIX
#include <time.h>		/* pick up CLK_TCK definition (100) */
#endif
#if SUN4
#include <sys/param.h>		/* pick up HZ definition (60) */
#define CLK_TCK HZ
#endif
#endif					/* DSI | HIGHC | INTEL | BCC16 | OS2 */
#endif					/* APPLE */
#if LINUX
#include <sys/times.h>
#define CLK_TCK sysconf(_SC_CLK_TCK)
#endif

zystm()
{
	/*
	/	process times are in 60ths of second, multiply by 100
	/	to get 6000ths of second, divide by 6 to get 100ths
	*/
#if APPLE
	/*  Apple returns ELAPSED time only */
	SET_IA( ((TickCount() - start_time) * 100) / 6 );
#else					/* APPLE */
#if DSI | HIGHC | INTEL | BCC16 | BCC32 | VCC | OS2 | WINNT
	SET_IA( msec() );
#else					/* DSI | HIGHC | INTEL | BCC16 | OS2 */
	struct tms	timebuf;

	timebuf.tms_utime = 0;	/* be sure to init in case failure	*/
	times( &timebuf );	/* get process times			*/

	/* CLK_TCK is clock ticks/second:
	 * # of seconds = tms_utime / CLK_TCK
	 * # of milliseconds = tms_utime * 1000 / CLK_TCK
	 *
	 * To avoid overflow, use
	 * # of milliseconds = tms_utime * (1000/10) / (CLK_TCK / 10)
	 */
	SET_IA( (timebuf.tms_utime * (1000/10)) / (CLK_TCK/10) );
#endif					/* DSI | HIGHC | INTEL | BCC16 | OS2 */
#endif					/* APPLE */
	return NORMAL_RETURN;
}

#if HIGHC | INTEL | BCC16 | OS2
#include <time.h>

#if 0		/* Insert system names here that don't define clock_t */
typedef long clock_t;
#endif

long msec()
{
    clock_t t;

    if (first_time == -1) {
		start_time = (clock_t) clock();
		first_time = -first_time;
		return 0L;
		}

    t = (clock_t) clock();

#if HIGHC | INTEL | OS2 | (BCC16 & IA32)
    if (t < start_time)
		t += 86400L * CLOCKS_PER_SEC;	/* sec/day * clocks/sec */

    /*
     * Convert to milliseconds.
     *  elapsed clocks * (1000 msec/sec) / (CLOCKS_PER_SEC clocks/sec) =
     *  elapsed clocks * (1000 / CLOCKS_PER_SEC) msec/clocks
     *  elapsed * (1000 / CLOCKS_PER_SEC) msec
     */
    return (t - start_time) * (1000 / CLOCKS_PER_SEC);
#endif					/* HIGHC | INTEL | OS2 | (BCC16 & IA32) */

#if BCC16 & !IA32
#define CLOCKS_PER_TENSEC ((int)(CLOCKS_PER_SEC * 10))	/* Clocks per ten seconds (182) */
    if (t < start_time)
		t += (86400L / 10) * CLOCKS_PER_TENSEC;	/* tensec/day * clocks/tensec */

    /*
     * Convert to deciseconds
     *  elapsed clocks * (100 dsec/tensec) / (CLOCKS_PER_TENSEC clocks/tensec) =
     *  elapsed clocks * (100 / CLOCKS_PER_TENSEC) dsec/clocks =
     *  (elapsed * 100) / CLOCKS_PER_TENSEC dsec
     *
     * On 16-bit 8088 systems, pin return value at max word value.
     */
    t = ((t - start_time) * 100L) / CLOCKS_PER_TENSEC;
    return t > (clock_t)(word)MAXPOSWORD ? (word)MAXPOSWORD : t;
#endif					/* BCC16 & !IA32 */

}
#endif					/* HIGHC | INTEL | BCC16 | OS2 */

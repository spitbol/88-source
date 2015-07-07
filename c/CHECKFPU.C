
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
 * checkfpu - check if floating point hardware is present.
 *
 * Used on those systems where hardware floating point is
 * optional.  On those systems where it is standard, the
 * floating point ops are coded in line, and this module
 * is not linked in.
 *
 * Returns 0 if absent, -1 if present.
 */


#include "port.h"

#if FLOAT
#if FLTHDWR
checkfpu()
{
	return -1;			/* Hardware flting pt always present */
}
#else					/* FLTHDWR */

#if HIGHC
#include "87.cf"		/* Intel Bug.  Should be angle brackets */
checkfpu()
{
	return _87_exists != 0 ? -1 : 0;
}
#endif					/* HIGHC */

#if INTEL

#if HighC387
/* Variables and code to allow use of HighC 8087 emulation library
 * with Intel C compiler.  Only works if modules containing floating
 * point operations are compiled with HighC.
 *
 * Warning: HighC returns double results in EDX:EAX.  Intel returns
 * them in ST(0) (even if emulating 387).  This means that functions
 * declared as returning a double compiled with HighC can not be called
 * routines compiled with Intel.  This module (checkfpu.c) is compiled
 * with Intel.
 */
#include <dos.h>

short int _mw8087;
short int _mw387;
short int _mwemc87;
long	  _mwno87[2];

void _fpreset()
{
 extern short int _fpstatus, _fpcontrol;
 _fpstatus = 0;
 _fpcontrol = 0x133F;
}

checkfpu()
{
#if 0
	return 0;
#else
	extern int _no387_windows(void);
	extern void _mwinit_87(void);		/* Initialize HighC 387 system */

	_mw8087 = _mw387 = _mwemc87 = 0;
	_mwno87[0] = _mwno87[1] = 0;
	if (!_no387_windows()) {			/* If not 387 and windows */
		_mwno87[0] = (long)getenv("NO87");	/* check for HighC NO87 string */
		if (_mwno87[0]) {				/* if NO87 string is present */
			struct SREGS s;				/* place harmless ds in far pointer */
			segread(&s);				/* that HighC init routine will use */
			_mwno87[1] = s.es;			/* when it displays NO87 string */
			}
		_mwinit_87();					/* init 387 or HighC emulator */
		}
	return _mw8087 ? -1 : 0;
#endif
}
#else					/* HighC387 */
#include <bios.h>
#include "sproto.h"

checkfpu()
{
	extern int _no387_windows(void);
	if (_no387_windows()) {
		wrterr("SPITBOL uses real arithmetic, cannot run under Windows!");
		__exit(1);
	return _bios_equiplist() & 2 ? -1 : 0;
#endif					/* HighC387 */
#endif					/* INTEL */

#if OS2
#define INCL_DOSDEVICES
#include <os2.h>
checkfpu()
{
	char result[4];			/* Documentation wrong!  Need 4-byte area */
	DosDevConfig((PVOID)&result, DEVINFO_COPROCESSOR);
	return result[0] ? -1 : 0;
}
#endif					/* OS2 */

#if BCC16 | BCC32 | VCC
#if MSDOS
#include <dos.h>
checkfpu()
{
#ifdef __TURBOC__
#ifdef __DLL__
	return 0;
#else
	return _8087 != 0 ? -1 : 0;
#endif					/* __DLL__ */
#endif					/* __TURBOC__ */
#ifdef __QCWIN__
	return 0;
#endif
}
#endif					/* MSDOS */
#if WINNT
checkfpu()
{
	return -1;		/* dunno how to test under NT -- assume all modern machines have FPU */
}
#endif
#endif          /* BCC16 | BCC32 | VCC */

#if LINUX
checkfpu()
{
	return -1;		/* Assume all modern machines have FPU */
}
#endif

#if BSD42 | SYSTEM5 | SOLARIS | SUNOS
#include <signal.h>
#include <setjmp.h>

static jmp_buf	env;

void fputrap Params((int sig));

void fputrap(sig)
int sig;
{
	longjmp(env,1);			/* Here if trap occurs */
}

checkfpu()
{
	SigType (*fstat)Params((int));
	int result;

	fstat = signal(SIGEMT,fputrap);	/* Set to trap floating op */
	result = -1;					/* assume floating point present */

	if (!setjmp(env))
		tryfpu();					/* Try a floating point op */
	else
		result = 0;					/* floating point not present */

	signal(SIGEMT, fstat);			/* restore old trap value */
	return result;
}
#endif					/* BSD42 | SYSTEM5 | OS2 */
#endif					/* FLTHDWR */
#endif					/* FLOAT */

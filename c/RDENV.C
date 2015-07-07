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
/	File:  RDENV.C		Version:  01.02
/	---------------------------------------
/
/	Contents:	Function rdenv
*/

/*
/   rdenv( varname, result )
/
/   rdenv() reads the environment variable named "varname", and if it can
/   be read, puts its value in "result.
/
/   Parameters:
/	varname	pointer to character string containing variable name
/	result	pointer to character string to receive result
/   Returns:
/	0 if successful / -1 on failure
/
/	v1.02 02-Jan-91 Changed rdenv to use cpys2sc instead of mystrncpy.
/					Add private getenv().
*/

#include "port.h"
#if MSDOS && (BCC16 | BCC32 | VCC)
#ifdef __TURBOC__
#include <dos.h>
#endif					/* __TURBOC__ */
#endif					/* BCC16 */


/*
/   Find environment variable vq of length vn.  Return
/   pointer to value (just past '='), or 0 if not found.
*/
char *findenv( vq, vn )
char *vq;
int  vn;
{
#if HIGHC | INTEL | BCC16 | BCC32 | VCC | UNIX | OS2 | MIPS
	char savech;
	char *p;

	savech = make_c_str(&vq[vn]);
	p = (char *)getenv(vq);			/* use library lookup routine */
	unmake_c_str(&vq[vn], savech);
	return p;
#endif					/* HIGHC | INTEL | BCC16 | UNIX | OS2 */


#if DSI
	extern char **environ;
	register int n;
	register char *p, *q, **env;

	/* one iteration per environment variable */
	env = environ;
	while ( (p = *env) != 0 )
	{
		q = vq;
		n = vn;

		/* match the variable against the environment */
                while ( n > 0  &&  *p == *q  &&  *p != '\0'  &&  *p != '=' )
		{
			p++;
			q++;
			n--;
		}

		/* if successful, return */
                if ( *p == '=' && n == 0 )
			return ++p;
		env++;
	}
	return (char *) 0L;
#endif					/* DSI */
}

#if !APPLE
rdenv( varname, result )
register struct scblk *varname, *result;
{
	register char *p;


	if ( (p = findenv(varname->str, varname->len)) == 0 )
		return -1;

	cpys2sc(p, result, TSCBLK_LENGTH);

	return 0;
}
#endif					/* !APPLE */

#if HIGHC
/*
 * getenv - under SPITBOL, we want to avoid use of HighC's getenv routine,
 * because its initializer copies the entire environment block to the
 * heap, resulting in a variable starting address for the SPITBOL work-
 * space.  This makes it impossible to reload SAVE files at the same address.
 */
char *getenv(char *name)
{
	extern FAR char *_mwenvp;		/* far pointer to environment block */
	extern char *_mwgetenvbp;		/* buffer to return result in */
	extern long _mwgetenvbz;		/* size of buffer */
	register FAR char *p;
	register char *q;
	long i;

	p = _mwenvp;					/* point to environment block */

	/* one iteration per environment variable */
	while ( *p != 0 ) {
		q = name;

		/* match the variable against the environment */
	while ( *p == *q && *p != '\0' && *p!='=' ) {
			p++;
			q++;
			}

		/* if successful, copy to buffer and return */
        if ( *p == '=' && *q == '\0' ) {
			i = _mwgetenvbz;
			q = _mwgetenvbp;
			while ((*q++ = *++p) != 0 && --i > 0)
				;
			*q = '\0';
			return _mwgetenvbp;
			}

		/* no match.  Advance to end of variable */
		while (*p++)
			;
		}
	return (char *) 0L;
}

char *_mwgetenv2(char *name)
{
	return getenv(name);
}
#endif					/* HIGHC */


#if MSDOS && (BCC16 | BCC32 | VCC)
#ifdef __TURBOC__
/*
 * getenv - under SPITBOL, we want to avoid use of TurboC's getenv routine,
 * because its initializer copies the entire environment block to the
 * heap, resulting in a loss of data segment space.
 *
 * To conserve data segment memory, we copy the environment string
 * into the end of tscblk.  This does not cause a problem for callers
 * who may copy the result to the front of tscblk.
 */
char *getenv(char *name)
{
#define MAXENVS 128
#if BCC16
	register char far *p;
	extern unsigned _envseg;
#else
	register char *p;
	extern char *_osenv;
#endif
	register char *q;
	int i;

#if BCC16
	p = (char far *)MK_FP(_envseg,0);	/* point to environment block */
#else
	p = _osenv;							/* point to environment block */
#endif

	/* one iteration per environment variable */
	while ( *p != 0 ) {
		q = name;

		/* match the variable against the environment */
        while ( *p == *q && *p != '\0' && *p!='=' ) {
			p++;
			q++;
			}

		/* if successful, copy to buffer and return */
        if ( *p == '=' && *q == '\0' ) {
			i = MAXENVS;
			q = &(pTSCBLK->str[TSCBLK_LENGTH - MAXENVS - 1]);
			while ((*q++ = *++p) != 0 && --i > 0)
				;
			*q = '\0';
			return &(pTSCBLK->str[TSCBLK_LENGTH - MAXENVS - 1]);
			}

		/* no match.  Advance to end of variable */
		while (*p++)
			;
		}
	return (char *) 0L;
}
#endif					/* __TURBOC __ */
#ifdef __QCWIN__
char *getenv(char *name)
{
	return (char *) 0L;
}
#endif					/* __QCWIN__ */
#endif					/* BCC16 */



/* make a string into a C string by changing the last character to null,
 * returning the old character at that location.
 * If the old character was already null, no change is made, so that
 * this works if passed a read-only C-string.
 */
char make_c_str(p)
char *p;
{
	char rtn;

	rtn = *p;
	if (rtn)
		*p = 0;
	return rtn;
}


/* Intel compiler bug? */
#if INTEL | OS2
void unmake_c_str(char *p, char savech)
#else
void unmake_c_str(p, savech)
char *p;
char savech;
#endif
{
	if (savech)
		*p = savech;
}

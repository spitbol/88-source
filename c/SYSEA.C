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
/	File:  SYSEA.C		Version:  01.00
/	---------------------------------------
/
/	Contents:	Function zysea
*/

/*
/
/	zysea - error advise
/
/	Here we catch errors before they are printed.
/
/	Parameters:
/	    XR - Error stage
/			if XR = STGIC, STGCE, STGXT then
/				WA - error number (1-330)
/				WB - column number
/				WC - line number
/				XL - scblk containing source file name
/	Returns:
/	    XR - SCBLK of message to print, or 0 if none
/	Exits:
/	    1 - suppress printing of error message
/
/  1.30.20 3/18/2000 - fix bug displaying column number - 1
*/

#include "port.h"
#if APPLE
#include "PEdit.h"
#include "compiler.h"
#endif					/* APPLE */

#if !APPLE
static char *eacpy Params((char *s1, char *s2, int n));

static char *eacpy(s1, s2, n)
char *s1, *s2;
int n;
{
	char *s0 = s1+n;

	while (n--)
		*s1++ = *s2++;
	return s0;
}
#endif					/* !APPLE */


/*
 * Error stage states
 */
enum stage {
	STGIC=0,			/* Initial compile				*/
	STGXC,				/* Execution compile (CODE)			*/
	STGEV,				/* Expression eval during execution		*/
	STGXT,				/* Execution time				*/
	STGCE,				/* Initial compile after scanning END line	*/
	STGXE,				/* Execute time compile after scanning END line	*/
	STGEE,				/* EVAL evaluating expression			*/
	STGNO				/* Number of codes				*/
	};

zysea()
{
#if APPLE
	register word tempxr;
	register word fnlen;
#endif					/* APPLE */

	register struct scblk *fnscblk = XL(struct scblk *);
	register char *p;


#if APPLE
			tempxr = XR(word);
			zysem();							/* Get text of message to tscblk	*/
			SET_XR(tempxr);						/* Restore calling XR */
			pTSCBLK->str[pTSCBLK->len] = '\0';

			/* Return result */
			if (WA(int) != 320)
				compResult.rcode = (tempxr == STGXT) ? ExeErr : CompErr;
			else
				compResult.rcode = Stopped;
			compResult.errno = WA(int);			/* error number */
			compResult.line = WC(word);			/* line number	*/
      compResult.col = WB(word);    /* column number */
			strcpy(compResult.msg, pTSCBLK->str);	/* error text   */

			/* Return file name if present */
			if (fnlen = fnscblk->len) {
				fnscblk->str[fnlen] = '\0';				/* Make file name into C string */
				strcpy(compResult.file,fnscblk->str);
				}
			else
				compResult.file[0] = '\0';
			if (tempxr != STGIC && tempxr != STGCE)		/* If execution error, allow dump to proceed */
				return EXIT_1;

			SET_XL(0L);							/* Other error, stop right away */
			SET_WB(0L);							/* Set &code = 0 because error is in rcode */
			zysej();							/* no return	*/
#else					/* APPLE */
			/* Display file name if present */
			if (fnscblk->len) {
				p = pTSCBLK->str;
				p = eacpy(p, fnscblk->str, (int)fnscblk->len);
				/* Display line number if present */
				if (WC(unsigned int)) {
					*p++ = '(';
					p += stcu_d(p, WC(unsigned int), 16);
					/* Display character position if present */
					if (WB(unsigned int)) {
						*p++ = ',';
            p += stcu_d(p, WB(unsigned int)+1, 16);
						}
					*p++ = ')';
					}
				p = eacpy(p, " : ", 3);
				pTSCBLK->len = p - pTSCBLK->str;
				SET_XR( pTSCBLK );
				return NORMAL_RETURN;
				}
#endif					/* APPLE */
	SET_XR(0L);
	return NORMAL_RETURN;	/* Other errors be processed normally */
}


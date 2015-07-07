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
 *  gencmp - general compare for Apple
 */
#include "port.h"

extern int lIUMagString(Ptr aPtr, Ptr bPtr, long aLen, long bLen);
static long Prefix(Ptr aPtr, Ptr bPtr, long aLen, long bLen);

#if ALTCOMP
#include <ScriptMgr.h>

/*
 * lstrncmp - compare two strings using long count.
 *
 *
 * long lstrncmp(aPtr, bPtr, aLen, bLen)
 *
 * It differs from strncmp in five respects:
 *	1) the count is a long, not an int.
 *  2) the counts for each string are specified separately.
 *  3) it does not check for '\0' chars as string terminators.
 *  4) it returns a long result, rather than an int.
 *  5) it makes *unsigned* compares of the characters if inequality.
 *
 * It is also faster, because the cmpm.b, dbne loop fits in the 68020's on-chip cache.
 *
 * Returns:
 *	<0 && != 0x80000000 if A string < B string
 *  =0 if A string = B string
 *  >0 if A string > B string
 */
long
lstrncmp(aPtr, bPtr, aLen, bLen)
char *aPtr, *bPtr;
long aLen, bLen;
{

	asm {
		movem.l	8(a7),a0-a1				/* aPtr to a0, bPtr to a1 */
		movem.l	16(a7),d0-d1			/* alen to d0, blen to d1 */
		sub.l	d1,d0					/* set d0 <0 if aLen < bLen, 0 if =, >0 if aLen >bLen */
		bge		@0						/* jmp if bLen is the limiting string */
		move.l	16(a7),d1				/* aLen is the limiting string */
	@0:	subq.l	#1,d1					/* preadjust for dbne	*/
		bmi		@2						/* if min(aLen, bLen) = 0 */
		move.l	d1,d2
		swap	d2						/* high order count to d2 low */
	@1:	cmpm.b	(a0)+,(a1)+
		dbne	d1,@1					/* low 16 bits of count */
		dbne	d2,@1					/* high 16 bits of count */
										/* here on mismatch or count exhausted */
		beq		@2						/* if strings equal, string length determines result */
		moveq.l	#0,d0					/* prepare unsigned byte compare */
		move.b	-(a0),d0				/* retest last character pair */
		moveq.l	#0,d1
		move.b	-(a1),d1
		sub.w	d1,d0					/* negative, or positive non-zero */
		ext.l	d0						/* return as long <0 or >0	*/
	@2:	return
		}
}

/*
 * gencmp - generic comparison routine
 *
 * Compares two strings with long string lengths using either ASCII or
 * International comparison system depending upon current setting of keyword.
 *
 * Returns:
 *  0x80000000 if either string too long for comparison routine
 *	<0 && != 0x8000 if A string < B string
 *  =0 if A string = B string
 *  >0 if A string > B string
 */
long
gencmp(aPtr, bPtr, aLen, bLen, keyword)
char	*aPtr, *bPtr;
long	aLen, bLen, keyword;
{
	if (keyword)
		return ((unsigned long) lIUMagString((Ptr)aPtr, (Ptr)bPtr, aLen, bLen)) << 16;
	else
		return lstrncmp(aPtr, bPtr, aLen, bLen);
}

/*
 * Prefix - Find the longest common prefix between two strings
 *
 * Prefix(Ptr aPtr, Ptr bPtr, long aLen, long bLen)
 */
static long
Prefix(aPtr, bPtr, aLen, bLen)
Ptr aPtr, bPtr;
long aLen, bLen;
{

	asm {
		movem.l	8(a7),a0-a1				/* aPtr to a0, bPtr to a1 */
		movem.l	16(a7),d0-d1			/* alen to d0, blen to d1 */
		cmp.l	d1,d0					/* compare lengths */
		bgt		@0						/* jmp if bLen is the limiting string */
		move.l	d0,d1					/* aLen is the limiting string */
	@0:	moveq.l	#0,d0					/* result counter */
		subq.l	#1,d1					/* preadjust for dbne	*/
		bmi		@2						/* if min(aLen, bLen) = 0 */
		move.l	d1,d2
		swap	d2						/* high order count to d2 low */
	@1:	addq.l	#1,d0					/* count matching chars */
		cmpm.b	(a0)+,(a1)+
		dbne	d1,@1					/* low 16 bits of count */
		dbne	d2,@1					/* high 16 bits of count */
		beq		@2						/* if count exhausted */
		subq.l	#1,d0					/* adjust if last char did not match */
	@2:	return
		}
}


/*
 * lIUMagString - IU String compare with long counts
 *
 * Returns:
 *  0x8000 if either string too long for comparison routine
 *	<0 && != 0x8000 if A string < B string
 *  =0 if A string = B string
 *  >0 if A string > B string
 */
int
lIUMagString(aPtr, bPtr, aLen, bLen)
register Ptr	aPtr, bPtr;
register long	aLen, bLen;
{
	register long	cLen, dLen;
		
	if ((aLen <= 32767) && (bLen <= 32767))
		return IUMagString(aPtr, bPtr, (int)aLen, (int)bLen);
	else {
		/* IU package restricts byte count to integer value.  Because of problems with
		   ligatures, we cannot break long operations into repeated smaller calls to
		   the IU package.
		   We attempt to remove any common substring prefix, allowing the operation
		   to proceed if the remaining strings are both < 32K bytes.  */
	
		cLen = Prefix(aPtr, bPtr, aLen, bLen);
		aPtr += cLen;
		bPtr += cLen;
		aLen -= cLen;
		bLen -= cLen;
		
		/* check for one or both strings now zero length after prefix removal */
		if (!aLen) {
			if (!bLen)
				return 0;
			else
				return 1;
				}
		if (!bLen)
			return -1;
			
		if (scriptsInstalled > 1) {
			dLen = cLen;
			/* Make sure we don't break in the middle of a multi-character sequence */
			while ((cLen-- > 0) && (CharByte(aPtr,0) >= smLastByte)) {
				aPtr--;						/* Backup one character */
				aLen++;
				}
			while ((dLen-- > 0) && (CharByte(bPtr,0) >= smLastByte)) {
				bPtr--;
				bLen++;
				}
			}
		if ((aLen <= 32767) && (bLen <= 32767))
			return IUMagString(aPtr, bPtr, (int)aLen, (int)bLen);
		else
			/* String longer than 32K */
			return 0x8000;
		}
}


#endif					/* ALTCOMP */

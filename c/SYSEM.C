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
/	File:  SYSEM.C		Version:  2.01
/	---------------------------------------
/
/	Contents:	Function zysem
*/

/*
/	zysem - get error message text
/
/	zysem returns the error message associated with an error number.
/
/	An assembly language file, errors.s contains a compressed form
/	of the error messages.  On the Macintosh, error messages are
/	in the resource fork of the application, uncompressed.  This
/	allows the user to easily translate them into any language.
/
/	Error messages are compressed into two character arrays.  Segments
/	within these arrays are delineated by \0 characters.  To find the
/	Nth segment, it is necessary to scan the array for the Nth \0.  The
/	segment begins at the next character position.
/
/	The first array, errors, contains 330 segments for the primary
/	error messages.  Within a segment, there are ascii characters
/	in the range 32-127, which are taken verbatim, and character
/	values 1-31 and 128-255, which are special characters.
/
/	The special characters are mapped into the range [1-159], where
/	they index segments in the second array, phrases.  Segments within
/	phrases follow the same rules as those within arrays, and may
/	contain special characters themself.
/
/	This expansion code is by necessity recursive.  This code and
/	the data within errors.s were coded for minimum space, not speed,
/	since it is used infrequently.
/
/	Parameters:
/	    WA - error number
/	Returns:
/	    XR - pointer to SCBLK containing error message (null string is ok)
/	Exits:
/	    None
/
/   V2.01 23-Dec-91
/		Add ERRDIST to allow the errors and phrases arrays to be
/       accessed as FAR pointers for those systems that point to them
/		in another segment.
*/

#include "port.h"

#if APPLE
#define	ErrStr				258
#define LastErrStr 			268
#define   ErrPrts			(LastErrStr - ErrStr + 1)
#define   ErrMax            340
#define   ErrPartSize 		(ErrMax/ErrPrts)
#else
extern unsigned char ERRDIST errors[];
extern unsigned char ERRDIST phrases[];
word msgcopy Params((word n, unsigned char ERRDIST *source, char *dest ));
word special Params((word c));
#endif					/* APPLE */

zysem()
{
#if APPLE
	/*
	 * Compute STR# resource number (msg/number of partitions) and
	 * string within resource ((msg-1) mod partition size)+1
	 */
	GetIndString((unsigned char *) pTSCBLK->str,
	 	((WA(int)-1) / ErrPartSize) + ErrStr,
		((WA(int)-1) % ErrPartSize) + 1 );
	pTSCBLK->len = (unsigned char) pTSCBLK->str[0];
	BlockMove(&(pTSCBLK->str[1]),&(pTSCBLK->str[0]),pTSCBLK->len);
#else					/* APPLE */
	pTSCBLK->len = msgcopy( WA(word), errors, pTSCBLK->str );
#endif					/* APPLE */
	SET_XR( pTSCBLK );
	return NORMAL_RETURN;
}

#if !APPLE
/*
/	special(c)
/
/	Return 0 if argument character is normal ascii.
/	Return index to phrase array if c is a special character.
*/
word special(c)
word c;
{
	if ( c == 0 )
		return 0;
	if ( c < 32 )
		return c;
	if ( c < 128 )
		return 0;
	return (c - 96);
}

/*
/	msgcopy(n, source, dest)
/
/	msgcopy() locates segment n in the source array, and copies its
/	characters to the destination array.  If any special characters
/	are encountered, msgcopy() is called recursively to expand them.
/
/	The function returns the number of characters copied.
*/

word msgcopy(n, source, dest )
word	n;
unsigned char ERRDIST *source;
char		*dest;
{
	word 	k;
	unsigned char	c;
	char		*dstart;

	/*
	/   Save starting destination pointer
	*/
	dstart = dest;

	/*
	/   Scan to first character of Nth string
	*/
	for ( ; n--; )
	{
		for ( ; *source++; )
			;
	}

	/*
	/   Examine next character of string.
	/	If it is a special character, recurse to unpack it
	/	   from phrases array.
	/	If normal character, just copy it.
	*/
	for ( ; (c = *source++) != 0; )
	{
		if ( (k = special(c)) != 0 )
			dest += msgcopy( k, phrases, dest );
		else
			*dest++ = c;
	}

	/*
	/   Return number of characters transferred.
	*/
	return dest - dstart;
}
#endif					/* !APPLE */

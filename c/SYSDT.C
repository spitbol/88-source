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
/	File:  SYSDT.C		Version:  01.06
/	---------------------------------------
/
/	Contents:	Function zysdt
/			Function conv
*/

/*
/	zysdt - get current date
/
/	zysdt is called when executing a Spitbol date function.
/
/	Parameters:
/       XR - optional integer argument describing date format desired
/	Returns:
/	    XL - pointer to SCBLK containing date string
/	Exits:
/	    None
*/

#include "port.h"
#if !APPLE
#if 0 /* !HIGHC */
#include <sys/types.h>
#endif					/* HIGHC */
#if BSD42
#include <sys/timeb.h>
#else					/* BSD42 */
#include <time.h>
#endif					/* BSD42 */
#endif					/* APPLE */
#if 0       /* Insert system names here that don't define time_t */
typedef unsigned long time_t;
extern struct	tm *localtime(time_t *tod);
#endif

static int datecvt Params(( char *cp, int type ));
static int timeconv Params(( char *tp, struct tm *tm ));

/* conv() rewritten to avoid dependence on library remainder routine */
void conv (dest, value)

register char *dest;
register int value;

{
	register short int i;
	i = value / 10;
	dest[0] = i + '0';
	dest[1] = value - i*10 + '0';
}

zysdt()
{
    struct icblk *dtscb = XR (struct icblk *);

    pTSCBLK->len = datecvt( pTSCBLK->str, dtscb->val );
	SET_XL( pTSCBLK );
	return NORMAL_RETURN;
}

/*
 * Write date/time in SPITBOL form to a string
 */
int storedate( cp, maxlen )
char	*cp;
word	maxlen;
{
   if (maxlen < 18)
       return 0;

   return datecvt(cp, 0);
}

/*
 * Write date/time in several different forms to a string
 */
int datecvt( cp, type )
char	*cp;
int     type;
{

    time_t      tod;

	register struct tm *tm;
#if !(DSI | BCC16 | BCC32 | VCC)
	time( &tod );
#endif					/* !(DSI | BCC16) */

    tm = localtime( &tod );

    switch (type)
    {
        default:
        case 0:     /* "MM/DD/YY hh:mm:ss" */
            conv( cp, tm->tm_mon+1 );
                cp[2] = '/';
            conv( cp+3, tm->tm_mday );
                cp[5] = '/';
            conv( cp+6, tm->tm_year % 100 );    /* Prepare for year 2000! */
            return 8 + timeconv(&cp[8], tm);

        case 1:     /* "MM/DD/YYYY hh:mm:ss" */
            conv( cp, tm->tm_mon+1 );
                cp[2] = '/';
            conv( cp+3, tm->tm_mday );
                cp[5] = '/';
            conv( cp+6, (tm->tm_year + 1900) / 100 );
            conv( cp+8, tm->tm_year % 100 );    /* Prepare for year 2000! */
            return 10 + timeconv(&cp[10], tm);

        case 2:     /* "YYYY-MM-DD/YYYY hh:mm:ss" */
            conv( cp+0, (tm->tm_year + 1900) / 100 );
            conv( cp+2, tm->tm_year % 100 );    /* Prepare for year 2000! */
                cp[4] = '-';
            conv( cp+5, tm->tm_mon+1 );
                cp[7] = '-';
            conv( cp+8, tm->tm_mday );
            return 10 + timeconv(&cp[10], tm);
    }
}

static int timeconv( tp, tm)
char *tp;
struct tm *tm;
{
     tp[0] = ' ';
     conv( tp+1, tm->tm_hour );
       tp[3] = ':';
     conv( tp+4, tm->tm_min );
       tp[6] = ':';
     conv( tp+7, tm->tm_sec );
       *(tp+9) = '\0';
     return 9;
}


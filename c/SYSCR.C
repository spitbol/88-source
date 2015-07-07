/*
 Copyright (C)  1988-1998  Mark Emmer

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
/	File:  SYSCR.C		Version:  01.00
/	---------------------------------------
/
/	Contents:	Function zyscr
*/

/*
/
/	zyscr - convert real value to string
/
/   This function is used in those implementations that define
/   the conditional symbol .CNCR.  Normally, real to string
/   conversion happens within the MINIMAL routine GTSTG.  But for
/   those systems where integers are too small to produce a
/   reasonable number of significant digits, or where additional
/   conversion flexibility is desired, SYSCR can be used instead
/   for the conversion.
/
/	Parameters:
/      RA - value to be converted
/      WA - no. of significant digits desired
/      WB - conversion type:
/             negative for E-type conversion
/             zero for G-type conversion
/             positive for F-type conversion
/      WC - character positions in result scblk
/      XR - scblk for result
/
/	Returns:
/	   XR - SCBLK with converted number.
/      WA - number of characters placed in result
/	Exits:
/	    None
*/

#include "port.h"

static int realst Params((char *realbf, double val, int preccl, int flotcl));

zyscr()

{
	struct scblk *scb = XR( struct scblk * );

	SET_WA(realst(scb->str, RA(double), WA(int), WB(int)));
	return NORMAL_RETURN;
}

static double pgten[]={1e1,1e2,1e4,1e8,1e16,1e32,1e64,1e128,1e256};
static double plten[]={1e-1,1e-2,1e-4,1e-8,1e-16,1e-32,1e-64,1e-128,1e-256};
static double rounds[]={5e-1,5e-2,5e-3,5e-4,5e-5,5e-6,5e-7,5e-8,
                        5e-9,5e-10,5e-11,5e-12,5e-13,5e-14,5e-15};

static int dscale(int round, double *valp)
   {
   int pow, sign, j;

   pow = 0;
   sign = 1;

   if (*valp < 0.0)
      *valp = -*valp;
   else
      sign = 0;
   if (*valp == 0.0)
      return 0;
   if (round < 0)
      return 0;
   if (round > 0 && round <= 15)
      *valp += rounds[round-1];
   if (*valp >= 10.) {
      for (j=9; j--;) {
         pow <<= 1;
         if (*valp >= pgten[j]) {
            *valp *= plten[j];
            pow += 1;
            }
         }
      }
   else if (*valp < 1.) {
      for (j=9; j--;) {
         pow <<= 1;
         if (*valp < plten[j]) {
            *valp *= pgten[j];
            pow += 1;
            }
         }
      pow = -pow;
      *valp *= 10.0;
      --pow;
      }
   if (sign)
      *valp = -*valp;
   return pow;
   }




/* flotcl -  <0 - E, =0 - G, >0 - F
 * realbf -  315 characters
 */

static int realst(char *realbf, double val, int preccl, int flotcl)
   {
   char *cp;
   int  j, fplace, iplace, temp, temp2, exp;

   cp = realbf;

   exp = dscale(0, &val);
   if (flotcl == 0  && exp < preccl /*9*/  &&  exp > -2)
      flotcl++;
  
   if (flotcl > 0)
      temp = exp;
   else {
      temp = -1;
      exp += 1;
      }
  
   j = dscale(preccl, &val);
   exp += j;
   temp += (flotcl > 0 ? j : 0);
   iplace = temp + 1;
   fplace = (preccl < 1 || preccl > 16 ? 16 : preccl);

   if (val < 0.0) {
      val = -val;
      *cp++ = '-';
      }
   if (temp < 0) {
      *cp++ = '0';
      *cp++ = '.';
      while (iplace++ < 0)
         *cp++ = '0';
      }
   else {
      do {
         if (fplace-- > 0) {
            j = (int)val;
            *cp++ = j + '0';
            val = (val - j) * 10.0;
            }
         else
            *cp++ = '0';
         } while (--iplace);
      *cp++ = '.';
      }
   while (fplace-- > 0) {
      j = (int)val;                    /* get the integer part */
      *cp++ = j + '0';
      val = (val - j) * 10.0;
      }

   while (*--cp == '0')           /* remove trailing zeros */
      ;
#if 0
   if (*cp == '.')                /* but always leave one */
      *++cp = '0';
#endif
  
   cp++;
   if (flotcl <= 0) {
      *cp++ = 'E';
      if (exp < 0) {
         *cp++ = '-';
         exp = -exp;
         }
      else
         *cp++ = '+';
      temp = exp / 100;
      if (temp) {
         exp = exp - temp * 100;
         *cp++ = temp + '0';
         }  
      temp2 = exp / 10;
      exp = exp - temp2 * 10;
      if (temp || temp2)
         *cp++ = temp2 + '0';
      *cp++ = exp + '0';
      }
   return (cp - realbf);
   }

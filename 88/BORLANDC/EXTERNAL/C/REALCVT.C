/*
  Copyright 1988-1998 Mark Emmer, Catspaw, Inc.  All Rights Reserved.
  Use Of this source code is governed by a BSD-style
  license that can be found in the LICENSE file.
*/

/*	Convert Long Real to String
					
	Provides real to string conversion with the same controls
 	offered by Catspaw's SNOBOL4+ &PRECISION and &REAL keywords.
 
 	To use, execute:

 		LOAD("REALCVT(REAL,INTEGER,INTEGER)")

 	Call with:

 		RESULT = REALCVT(NUMBER, PRECISION, TYPE)

 	Where:
 		NUMBER		- a real number to convert
 		PRECISION	- The number of digits of precision to produce,
 					  an integer in the range 1 - 315.
 		TYPE		- -1 to force "E" type conversion (exponent notation).
 					- +1 to force "F" type conversion (no exponent).
 					- 0 for "G" type conversion (algorithm selects "E" or
 					  "F" conversion depending on magnitude of number).

 		RESULT		- a string representation of NUMBER.
 */

#include "system.h"
#include "extrn88.h"

static int dscale(int round, double far *pdw);
int errno;

entry(REALCVT)(presult, pinfo, flotcl, preccl, real)
double			real;
long			preccl;			/* precision */
long			flotcl;			/* <0 - E, =0 - G, >0 - F */
misc far     	*pinfo;		    /* pointer to misc info	*/
union block far	*presult;		/* pointer to result area */
{
  char far	*cp;
  char far	*cpi;
  int   	j, fplace, iplace, mode, temp, exp;
  char		realbuf[316];

  cp = cpi = realbuf;

  mode = (int)flotcl;
  exp = dscale(0, &real);
  if (mode == 0  && exp < 5  &&  exp > -5)
    mode++;
  temp = (mode > 0 ? exp : 0);

  j = dscale((int)preccl, &real);
  exp += j;
  temp += (mode > 0 ? j : 0);
  iplace = temp + 1;
  fplace = ((int)preccl < 1 || (int)preccl > 16 ? 16 : (int)preccl);

  if (real < 0.0)
  {
    real = -real;
    *cp++ = '-';
  }
  if (temp < 0)
  {
    *cp++ = '0';
    *cp++ = '.';
    while (iplace++ < 0)
	  *cp++ = '0';
  }
  else
  {
    do
	{
      if (fplace-- > 0)
	  {
        j = real;
        *cp++ = j + '0';
        real = (real - j) * 10.0;
      }
      else
	    *cp++ = '0';
    } while (--iplace);
    *cp++ = '.';
  }
  while (fplace-- > 0)
  {
    j = real;			/* get the integer part */
    *cp++ = j + '0';
    real = (real - j) * 10.0;
  }

  /* remove trailing zeros from mantissa if G format */
  if ((int)flotcl == 0)
  {
  	while (cp[-1] == '0')
		cp--;
  }
  		   
  if (mode <= 0)
  {
    *cp++ = 'E';
    if (exp < 0)
	{
      *cp++ = '-';
      exp = -exp;
    }
    else
	  *cp++ = '+';
    temp = exp / 100;
    exp = exp - temp * 100;
    *cp++ = temp + '0';
    temp = exp / 10;
    exp = exp - temp * 10;
    *cp++ = temp + '0';
    *cp++ = exp + '0';
  }
  presult->scb.sclen = FP_OFF(cp) - FP_OFF(cpi);	/* string length */
/*  memcpyff(MK_FP(_SS,presult->scb.scstr), cpi, presult->scb.sclen); */
  memcpyff(presult->scb.scstr, cpi, presult->scb.sclen);
  return BL_SC;
}

static double pgten[]={1e1,1e2,1e4,1e8,1e16,1e32,1e64,1e128,1e256};
static double plten[]={1e-1,1e-2,1e-4,1e-8,1e-16,1e-32,1e-64,1e-128,1e-256};
static double rounds[]={5e-1,5e-2,5e-3,5e-4,5e-5,5e-6,5e-7,5e-8,
			 5e-9,5e-10,5e-11,5e-12,5e-13,5e-14,5e-15};

/*	DSCALE		Scale *pdw into range 1.00000 to 9.999999.
 *				If round > 0, add 5.0E-round to *pdw before scaling.
 *				Power of 10 needed to restore result returned in as result.
 */
static int dscale(int round, double far *pdw)
{
  int pow=0, sign=1, j;

  if (*pdw < 0.0)
    *pdw = -*pdw;
  else
    sign = 0;
  if (*pdw == 0.0)
    return 0;
  if (round < 0)
    return 0;
  if (round > 0 && round <= 15)
    *pdw += rounds[round-1];
  if (*pdw - 10. >= 0.)
  {
    for (j=9; j--;)
	{
      pow <<= 1;
      if (*pdw - pgten[j] >= 0.)
	  {
	    *pdw *= plten[j];
	    pow += 1;
      }
    }
  }
  else if (*pdw - 1. < 0.)
  {
    for (j=9; j--;)
	{
      pow <<= 1;
      if (*pdw - plten[j] < 0.)
	  {
	    *pdw *= pgten[j];
	    pow += 1;
      }
    }
    pow = -pow;
    *pdw *= 10.0;
    --pow;
  }
  if (sign)
    *pdw = -*pdw;
  return pow;
}

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
 * math.c - extended math support for spitbol
 *
 * Routines not provided by hardware floating point.
 *
 * These routines are not called from other C routines.  Rather they
 * are called by inter.*, and by external functions (HighC 386 version).
 */
#include "port.h"

#if FLOAT & !MATHHDWR

/* If compiling under HighC using Intel headers, avoid generation of
 * in-line, built-in functions
 */
#if INTEL
#ifdef HighC387
#define _mathh_builtin
#endif
#endif

#include <math.h>

#ifndef errno
#if LINUX
int errno;
#else
extern int errno; /* system error number */
#endif
#endif

extern double inf;	/* infinity */

#if HIGHC | BCC16
#define dist FAR		/* needed for access by external functions */
#else
#define dist
#endif


/*
 * f_atn - arctangent
 */
double dist f_atn(ra)
double ra;
   {
   return atan(ra);
   }


/*
 * f_chp - chop
 */
double dist f_chp(ra)
double ra;
   {
   if (ra >= 0.0)
      return floor(ra);
   else
      return ceil(ra);
   }



/*
 * f_cos - cosine
 */
double dist f_cos(ra)
double ra;
   {
   return cos(ra);
   }



/*
 * f_etx - e to the x
 */
double dist f_etx(ra)
double ra;
   {
   double result;
   errno = 0;
   result = exp(ra);
   return errno ? inf : result;
   }



/*
 * f_lnf - natural log
 */
double dist f_lnf(ra)
double ra;
   {
   double result;
   errno = 0;
   result = log(ra);
   return errno ? inf : result;
   }



/*
 * f_sin - sine
 */
double dist f_sin(ra)
double ra;
   {
   return sin(ra);
   }


/*
 * f_sqr - square root  (range checked by caller)
 */
double dist f_sqr(ra)
double ra;
   {
   return sqrt(ra);
   }


/*
 * f_tan - tangent
 */
double dist f_tan(ra)
double ra;
   {
   double result;
   errno = 0;
   result = tan(ra);
   return errno ? inf : result;
   }

#endif					/* FLOAT & !MATHHDWR */

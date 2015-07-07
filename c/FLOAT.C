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
 * float.c - floating point support for spitbol
 *
 * These routines are not called from other C routines.  Rather they
 * are called by inter.*, and by external functions (HighC 386 version).
 */
#include "port.h"
#if (FLOAT & !FLTHDWR) | (EXTFUN & (SUN4 | AIX))

#if HIGHC | BCC16
#define dist FAR		/* needed for access by external functions */
#else
#define dist
#endif

/*
 * f_2_i - float to integer
 */
IATYPE dist f_2_i(ra)
double ra;
   {
   return (IATYPE)ra;
   }


/*
 * i_2_f - integer to float
 */
double dist i_2_f(ia)
IATYPE ia;
   {
   return ia;
   }

/*
 * f_add - floating add to accumulator
 */
double dist f_add(arg, ra)
double arg,ra;
   {
   return ra+arg;
   }

/*
 * f_sub - floating subtract from accumulator
 */
double dist f_sub(arg, ra)
double arg,ra;
   {
   return ra-arg;
   }

/*
 * f_mul - floating multiply to accumulator
 */
double dist f_mul(arg, ra)
double arg,ra;
   {
   return ra*arg;
   }


/*
 * f_div - floating divide into accumulator
 */
double dist f_div(arg, ra)
double arg,ra;
   {
   return ra/arg;
   }

/*
 * f_neg - negate accumulator
 */
double dist f_neg(ra)
double ra;
   {
   return -ra;
   }

#endif					/* (FLOAT & !FLTHDWR) | (EXTFUN & (SUN4 | AIX)) */

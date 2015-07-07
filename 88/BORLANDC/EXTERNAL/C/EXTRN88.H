/*
  Copyright 1988-1998 Mark Emmer, Catspaw, Inc.  All Rights Reserved.
  Use Of this source code is governed by a BSD-style
  license that can be found in the LICENSE file.
*/

/*----------------------------  extrn88.h  ------------------------------*/
#ifndef __extrn88__
#define __extrn88__

/*
 *  Definitions of routines and data available to C-language
 *  external function to be called from SPITBOL-88.
 *
 * 	V1.00  05-25-92 06:47am
 * 		   Initial 8088 version derived from 80386 version.
 *
 * 	(c) Copyright 1992 Catspaw, Inc.  All Rights Reserved.
 *
 *  Definition of information placed on stack prior to pushing arguments to
 *  an external function.
 *
 *  Many of these items can be ignored, and are provided only for the
 *  benefit of those wishing to operate directly on SPITBOL's internal
 *  data structures.
 *
 *  However, the pointer in presult *must* be used by the external
 *  function to locate the area in which results are returned.
 * 
 *  Note that regardless of whether the 16-bit or 32-bit integer
 *  version of SPITBOL is in use, integers are always passed as 32-bit
 *  long values.
 */

typedef int word;
#if IA32
typedef long iatype;
#else
typedef int iatype;
#endif

#define entry(x) word far mainslf		/* Borland C++ 3.0 */

#include "blocks88.h"
#include <string.h>
#include <dos.h>

/* array of pointers to double functions */
typedef double far (*APDF[])();

/*
 * Miscellaneous information provided by SPITBOL in pointer above the arguments.
 */

typedef struct misc {
	short			 vers;			/* version number of interface			*/
	unsigned char	 ext;			/* host environment, see ext_type below	*/
	unsigned char	 spare;			/* reserved 							*/
	word			 nargs;		    /* number of args to function			*/
 	word far		 (*ptyptab)[1];	/* pointer to array of data types		*/
 	struct xnblk far *pxnblk;	    /* ptr to xnblk describing function		*/
 	struct efblk far *pefblk;		/* ptr to efblk describing function		*/
	APDF             *pflttab;		/* ptr to array of floating point fncs	*/
	short			 spds;			/* SPITBOL's DS segment selector		*/
	short			 spcs;			/* SPITBOL's CS segment selector		*/
 	} misc;

enum ext_type {						/* Executing under:						*/
	t_pharlap,						/*  PharLap DOS Extender				*/
	t_intel,						/*  Intel DOS Extender					*/
	t_os2,							/*  OS/2 2.0							*/
	t_tc16,						 	/*  MS-DOS TurboC with 16-bit IA		*/
	t_tc32,							/*  MS-DOS TurboC with 32-bit IA		*/
	t_w1616,						/*  16-bit Windows, 16-bit SPITBOL		*/
	t_w1632,						/*  16-bit Windows, 32-bit SPITBOL		*/
	t_wnt8,							/*  Windows NT on 386/486				*/
	t_sparc,						/*  Sun 4 / SPARC						*/
	t_mac,							/*  Apple Macintosh						*/
	t_mips			              	/*  MIPS R3000							*/
	};

/*
 * Sample usage.  Definition for function arguments, assuming
 * calling function in SPITBOL with:
 *
 * 	 F(INTEGER,REAL,STRING)
 *
 * Because SPITBOL pushes arguments left to right, a Pascal
 * calling sequence is used.  This is provided by the entry() macro.
 *
 *   entry(F)(iarg1, rarg2, larg3, parg3, pinfo, presult)
 *   long		   		iarg1;			 arg1 integer
 *   double		   		rarg2;			 arg2 real number
 *   word		 		larg3;			 arg3 length
 *   char far	   		*parg3;			 pointer to arg3 string
 *   misc far		   	*pinfo;		     miscellaneous info
 *   union block far	*presult;		 pointer to result area
 * {
 *    ....  start of function body
 */


/*
 * Simple names for datatypes.  Performs a lookup in SPITBOL's type
 * table to fetch a 16-bit type word for specific data types.
 * 
 * Borland C++ 3.0 seems to have problems with the double far pointers
 * in the definitions below, which used to be carried out in the form:
 *   #define sc (*((*pinfo).ptyptab))[BL_SC]
 * Instead, do it by brute force manipulation of the segment and offset
 * of the pointers.
 * 
 */
#define tseg FP_SEG(pinfo)
#define toff FP_OFF((*pinfo).ptyptab)
#define tcode(x) ((word far *)MK_FP(tseg,toff+x*sizeof(word)))

#define ar	(*tcode(BL_AR))			/* Array			*/
#define bc	(*tcode(BL_BC))			/* Buffer Control	*/
#define bf	(*tcode(BL_BF))			/* String Buffer 	*/
#define cd	(*tcode(BL_CD))			/* Code				*/
#define ex	(*tcode(BL_EX))			/* Expression		*/
#define ic	(*tcode(BL_IC))			/* Integer			*/
#define nm	(*tcode(BL_NM))			/* Name				*/
#define rc	(*tcode(BL_RC))			/* Real				*/
#define	sc	(*tcode(BL_SC))			/* String			*/
#define tb	(*tcode(BL_TB))			/* Table			*/
#define vc	(*tcode(BL_VC))			/* Vector			*/
#define xn	(*tcode(BL_XN))			/* External			*/


/*
 * Non-standard block-type values that may be returned in register AX.
 */

#define FAIL	(-1)			/* Signal function failure	*/
#define	BL_NC	100				/* Unconverted result		*/
#define BL_FS	101				/* Far string				*/
#define	BL_FX	102				/* Far external block		*/

/*
 * Length of string area in result buffer
 */

#define	buflen	256


/*
 * SPITBOL's Real Number Functions are not available to external
 * functions coded in C.  Use the normal C floating point library
 * to provide such support.
 */

/*
 * Function definitions for routines in extrnlib.c
 */
extern int memcmpff(char far *s1, char far *s2, size_t n);
extern char far *memcpyff(char far *s1, char far *s2, size_t n);
extern char far *memcpyfn(char far *s1, char near *s2, size_t n);
extern char far *strcpyff(char far *s1, char far *s2);
extern char far *strcpyfn(char far *s1, char near *s2);
extern char near *strcpynf(char near *s1, char far *s2);
extern size_t strlenf(char far *s);
extern int strncmpfn(char far *s1, char near *s2, size_t n);
extern char far *strncpyff(char far *s1, char far *s2, size_t n);
extern char far *strncpyfn(char far *s1, char near *s2, size_t n);
extern char near *strncpynf(char near *s1, char far *s2, size_t n);
extern word retint(long val, union block far *presult);
extern word retnstrf(char far *s, size_t n, union block far *presult);
extern word retnstrn(char near *s, size_t n, union block far *presult);
extern word retnxdtf(void far *s, size_t n, union block far *presult);
extern word retnxdtn(void near *s, size_t n, union block far *presult);
extern word retreal(double val, union block far *presult);
extern word retstrf(char far *s, union block far *presult);
extern word retstrn(char near *s, union block far *presult);

#pragma argsused		/* suppress noisy messages */

#endif					/* __extrn88__ */
/*-------------------------- end of extrn88.h ------------------------*/

/*
  Copyright 1988-1998 Mark Emmer, Catspaw, Inc.  All Rights Reserved.
  Use Of this source code is governed by a BSD-style
  license that can be found in the LICENSE file.
*/

/*---------------------------  extrnlib.c  -------------------------------*/

/*
 * Handy support routines for SPITBOL-88 functions coded in
 * Borland C++ 3.0 (Tiny Model).
 *
 * V1.00	02/17/90 04:31pm
 * 			Initial version for 80386 SPITBOL.
 *
 * V1.01	10-19-91 06:51am
 * 			Modify for OS/2, where far mem* and str* function are not
 * 			needed (they are provided by normal C library functions).
 * 
 * V1.02	07-09-92 01:53pm
 * 			Modify for 8088 SPITBOL where far pointers are needed.
 *
 * (c) Copyright 1992 Catspaw, Inc.  All Rights Reserved.
 *
 * Contents:
 *
 * int		  memcmpff(s1, s2, n)		compare two far areas of memory
 * char  far *memcpyff(s1, s2, n)		n-char copy far str s2 to far str s1
 * char  far *memcpyfn(s1, s2, n)		n-char copy near str s2 to far str s1
 * char  far *strcpyff(s1, s2)			copy far C string s2 to far string s1
 * char  far *strcpyfn(s1, s2)			copy near C string s2 to far string s1
 * char near *strcpynf(s1, s2)			copy far C string s2 to near string s1
 * size_t	  strlenf(s)				size of far C string
 * int		  strncmpfn(s1, s2, n)		n-char compare far s1 with near s2
 * char  far *strncpyff(s1, s2, n)		n-char copy far C str s2 to far str s1
 * char  far *strncpyfn(s1, s2, n)		n-char copy near C str s2 to far str s1
 * char near *strncpynf(s1, s2, n)		n-char copy far C str s2 to near str s1
 *
 * word 	  retint(val, presult)		return integer result
 * word 	  retnstrf(s, n, presult)	return n-char far string
 * word 	  retnstrn(s, n, presult)	return n-char near string
 * word 	  retnxdtf(s, n, presult)	return n-chars of far external data
 * word 	  retnxdtn(s, n, presult)	return n-chars of near external data
 * word 	  retreal(val, presult)		return real result
 * word 	  retstrf(s, presult)		return far C string
 * word 	  retstrn(s, presult)		return near C string
 */


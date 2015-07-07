/*
  Copyright 1988-1998 Mark Emmer, Catspaw, Inc.  All Rights Reserved.
  Use Of this source code is governed by a BSD-style
  license that can be found in the LICENSE file.
*/

/*
/   The following manifest constants define the type of system.
/
/			running on a:
/   APOLLO		 APOLLO Domain (BSD42)
/   APPLE		 APPLE Macintosh
/   ATT73		 AT&T 7300 (Un*x PC) (System V)
/   DSI			 Definicon (System V on MSDOS)
/   HPUX		 HP 350 (System V)
/   HPBSD		 HP 350 (BSD42)
/   HIGHC		 MS-DOS, Metaware High C 80386 compiler
/     HIGHC16		   Metaware High C version 1.6
/     HIGHC23		   Metaware High C version 2.3
/   INTEL		 MS-DOS, Intel Code Builder 80386 compiler
/   BCC16			 Intel 8088 (MSDOS, Turbo-C compiler)
/   NCRXP		 NCR Tower XP (68010, System V)
/   NCR32		 NCR Tower 32 (68020, System V)
/   SUN2		 SUN2 or SUN3 (BSD42)
/   SUN3		 SUN3 ONLY	(BSD42)
/   SUN4		 SUN4 (SPARC) ONLY	(BSD42)
/   UNISYS		 UNISYS 5000/70 (System V)
/   VAX			 VAX
/   WICAT		 WICAT
/
/   BSD41		Berkeley release: BSD 4.1
/   BSD42		Berkeley release: BSD 4.2
/   BSD43		Berkeley release: BSD 4.3
/   SYSTEM3		ATT release: System III
/   SYSTEM5		ATT release: System V
/   MSDOS		Microsoft DOS
/
/
/	DIRECT      Define to access Minimal data directly (default)
/
/   EXECSAVE	Executable files produced via SAVE files.
/
/   EXTFUN		Include code for external functions
/
/   FAR			Used for far-away objects on segmented architecture.
/				Do not define (or define to nothing) on other systems.
/
/   HOST386		Produce special 386 HOST Screen/Keyboard/Sound
/				functions.
/
/   INTBITS		Number of bits in C compiler's native int (default 32).
/   MAXINT		Max positive value using INTBITS.
/
/   RUNTIME		Produce runtime version capable of executing saved
/				impure data segments (.spx) only.  Cannot compile
/				source files.
/
/   SAVEFILE		Produce SAVE files.
/
/	WORDBITS	Number of bits in a Minimal word (default 32).
/   MAXPOSWORD  Max positive value using WORDBITS.
/
*/
/*  Copyright 1991 Robert Goldberg and Catspaw, Inc. */

#define	BCC16		1
#define	DIRECT		0
#define	EOL1		'\r'
#define	EOL2		'\n'
#define ERRDIST		far
#define EXECSAVE	1
#define	EXTFUN		1
#define	FAR			far
#define HOST386		1
#define INTBITS		16
#define IRECSIZ		512
#define MAXINT		0x7FFF
#define	MSDOS		1
#define NEAR		near		/* Pointers within Minimal space are NEAR */
#define	PIPES		0
#define	SAVEFILE	1
#define TSCBLK_LENGTH 256
#define WORDBITS	16
#define MAXPOSWORD  0x7FFF

#define const

/* Data pointers are FAR by default if the model is Compact, Large, or Huge */
#if defined(__COMPACT__) | defined(__LARGE__) | defined(__HUGE__)
#define __FARDATA__
#else
#define __NEARDATA__
#endif


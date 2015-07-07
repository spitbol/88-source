/*
 Copyright 2009 Robert Goldberg and Catspaw, Inc.

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
/   File:  PORT.H   (SPITBOL)   Version:  01.11
/   ---------------------------------------------------
/
/   This header file contains manifest constants that describe system
/   dependencies.  Many of these values will be changed when porting
/   the OS interface to another machine/operating system.
*/
#include "systype.h"

/*
/	Turn off system-specific features unless specifically called for
/	in systype.h.
*/

#ifndef	ALTCOMP
#define	ALTCOMP		0	/* no alternate string comparison */
#endif

#ifndef DATECHECK
#define DATECHECK   0   /* no check for expiration date */
#endif

#ifndef ENGINE
#define ENGINE		0	/* not building engine version */
#endif

#ifndef EXECSAVE
#define EXECSAVE	0	/* executable modules via save files */
#endif

#ifndef	EXTFUN
#define EXTFUN		0	/* no external functions */
#endif

#ifndef	FLTHDWR
#define	FLTHDWR		0	/* floating point hardware not present */
#endif

#ifndef	HOST386
#define HOST386		0	/* no 80386 host functions */
#endif

#ifndef	MATHHDWR
#define	MATHHDWR	0	/* extended math hardware not present */
#endif

#ifndef	RUNTIME
#define	RUNTIME		0	/* not making runtime version */
#endif

#ifndef	SAVEFILE
#define	SAVEFILE	0	/* can't create save files */
#endif

#ifndef SETREAL
#define SETREAL     0   /* don't use reals for SET()  */
#endif

#ifndef	USEQUIT
#define	USEQUIT		0	/* use quit() to report errors instead of wrterr */
#endif

/*
 * 	Turn on system-specific features unless specifically turned off
 * 	in systype.h
 */
#ifndef DIRECT
#define	DIRECT		1	/* access Minimal data directly */
#endif

#ifndef EXECFILE
#define	EXECFILE	1	/* create executable modules */
#endif

#ifndef	FLOAT
#define	FLOAT		1	/* include floating point code */
#endif

#ifndef IA32
#define	IA32		1	/* Integer accumulator (IA) is 32 bits wide */
#endif

#ifndef	IO
#define	IO			1	/* include input/output code */
#endif

#ifndef	MATH
#define	MATH		1	/* include extended math (i.e., trig) code */
#endif

#ifndef PIPES
#define	PIPES		1	/* include pipe code */
#endif

#ifndef	POLLING
#define	POLLING		1	/* enable polling of operating system */
#endif

#ifndef PROTOTYPES
#define	PROTOTYPES	1	/* assume compiler can handle prototypes */
#endif

#ifndef USEFD0FD1
#define	USEFD0FD1	1	/* use file descriptor 0 & 1 for stdio	*/
#endif

/*
 *  Other defaulted values that may be overridden in systype.h
 */
#ifndef INTBITS
#define INTBITS		32			/* assume int will be 32 bits */
#define MAXINT		0x7FFFFFFFL	/* maximum positive value in int */
#endif
#ifndef WORDBITS
#define WORDBITS	32			/* assume word will be 32 bits */
#define MAXPOSWORD	0x7FFFFFFFL	/* maximum positive value in word */
#endif


/*
 *  If not defined in systype.h, disable it here.
 */
/* compiler defs */
#ifndef APOLLO
#define APOLLO		0
#endif
#ifndef APPLE
#define APPLE		0
#endif
#ifndef ATT73
#define ATT73       0
#endif
#ifndef BCC16
#define BCC16		0			/* 16-bit Borland C++ 3.x or 4.x */
#endif
#ifndef BCC32
#define BCC32		0			/* 32-bit Borland C++ 4.x */
#endif
#ifndef VCC
#define VCC         0           /* 32-bit Microsoft Visual C++ */
#endif
#ifndef DSI
#define DSI			0
#endif
#ifndef HPBSD
#define HPBSD		0
#endif
#ifndef HPUX
#define HPUX		0
#endif
#ifndef HIGHC
#define HIGHC		0			/* MetaWare HighC */
#endif
#ifndef HIGHC16
#define HIGHC16		0			/* MetaWare HighC version 1.6 */
#endif
#ifndef HIGHC23
#define HIGHC23		0			/* MetaWare HighC version 2.3 */
#endif
#ifndef INTEL
#define INTEL		0			/* Intel CodeBuilder */
#endif
#ifndef GCCx86
#define GCCx86      0
#endif
#ifndef GCC32
#define GCC32       0
#endif
#ifndef GCC64
#define GCC64       0
#endif
#ifndef MIPSSGI
#define MIPSSGI 	0
#endif
#ifndef NCR32
#define NCR32		0
#endif
#ifndef NCRXP
#define NCRXP		0
#endif
#ifndef OS2
#define OS2			0
#endif
#ifndef RS6
#define RS6			0
#endif
#ifndef SUN2
#define SUN2		0
#endif
#ifndef SUN3
#define SUN3		0
#endif
#ifndef SUN4
#define SUN4		0
#endif
#ifndef UNISYS
#define UNISYS		0
#endif
#ifndef VAX
#define VAX			0
#endif
#ifndef WICAT
#define WICAT		0
#endif

/* operating system defs */
#ifndef AIX3
#define AIX3		0
#endif
#ifndef AIX4
#define AIX4		0
#endif
#ifndef BSD41
#define BSD41		0
#endif
#ifndef BSD42
#define BSD42		0
#endif
#ifndef BSD43
#define BSD43		0
#endif
#ifndef IRIX
#define IRIX        0
#endif
#ifndef LINUX
#define LINUX       0
#endif
#ifndef MSDOS
#define MSDOS		0
#endif
#ifndef OS2
#define OS2         0
#endif
#ifndef SOLARIS
#define SOLARIS		0
#endif
#ifndef SUNOS
#define SUNOS       0
#endif
#ifndef SYSTEM3
#define SYSTEM3		0
#endif
#ifndef SYSTEM5
#define SYSTEM5		0
#endif
#ifndef WINNT
#define WINNT  		0
#endif

#if HIGHC | INTEL | OS2 | BCC32 | VCC | GCC32
#define SYSVERSION 0
#endif
#if APPLE
#define SYSVERSION 1
#endif
#if BCC16
#define SYSVERSION 2
#endif
#if SUN4
#define SYSVERSION 3
#endif
#if MIPSSGI
#define SYSVERSION 4
#endif
#if AIX3 | AIX4
#define SYSVERSION 6
#endif
#if GCC64
#define SYSVERSION 7
#endif
#ifndef SYSVERSION
#define SYSVERSION 255
#endif

#if EXECSAVE			/* EXECSAVE requires EXECFILE & SAVEFILE on */
#undef EXECFILE
#undef SAVEFILE
#define EXECFILE	1
#define SAVEFILE	1
#endif

/* Define how the errors and phrases arrays will be accessed (see sysem.c) */
#ifndef ERRDIST
#define ERRDIST
#endif

#ifndef FAR
#define FAR
#define readfar read
#define writefar write
#endif

#ifdef NEAR
#define __NEAR__ 1
#else
#define NEAR
#define __NEAR__ 0
#endif

#define AIX (AIX3 | AIX4)

#define MIPS MIPSSGI

#define NCR	(NCRXP | NCR32)

#define SUN	(SUN2 | SUN3 | SUN4)

#define UNIX (AIX | BSD41 | BSD42 | BSD43 | SYSTEM3 | SYSTEM5 | IRIX | LINUX | SOLARIS | SUNOS)

#if APPLE
typedef long	word;
typedef unsigned long uword;
#else
typedef int		word;
typedef unsigned int uword;
#endif					/* APPLE */

/* Size of integer accumulator */
#if IA32
typedef long IATYPE;
#else					/* IA32 */
typedef short int IATYPE;
#endif					/* IA32 */

/*
/   Define the default end of line characters.  Use Unix definitions
/   as the default.  Override in systype.h.
*/
#ifndef EOL1
#define EOL1	'\n'
#endif

#ifndef EOL2
#define EOL2	0
#endif

/*
 * Define the data type returned by a call to signal()
 */
#if BSD41 | BSD42 | BSD43 | AIX | LINUX | SOLARIS | SUNOS
#define SigType void
#else                   /* BSD41 | ... | SUNOS */
#define	SigType int
#endif                  /* BSD41 | ... | SUNOS */

/*
/   The following manifest constants define the page size used when the
/   compiler produces a source listing.
/
/   PAGE_DEPTH		number of lines to print on a page
/   PAGE_WIDTH		number of characters to print on a line
/					also the default record length for OUTPUT, TERMINAL
*/
#if APPLE
#define PAGE_DEPTH	0
#else
#define PAGE_DEPTH	60
#endif

#define PAGE_WIDTH	120

/*
/	The following constant defines the size of the code word for
/	LZW compression of a save file.  See file compress.c.
*/
#if WORDBITS == 16
#define BITS 10
#else
#define	BITS 12
#endif

/*
/   The following manifest contants describe the constraints on the heap
/   managed by the spitbol compiler.
/
/   All values can be overriden via command line options.
/
/   CHUNK_SIZE		the size of an allocation unit (chunk) used to
/                   create the heap.  Defined in WORDS!
/
/   CHUNK_B_SIZE	CHUNK_SIZE in bytes.
/
/   HEAP_SIZE		the maximum size that spitbol's heap (dynamic area)
/                   can become.  Defined in WORDS!
/
/   OBJECT_SIZE		the maximum size of any object created in the heap.
/                   Defined in WORDS!
/	Note: It was necessary to reduce this value from 8M to 1M  because
/   some DPMI hosts (like OS/2 DOS session and 386MAX) use much smaller
/	starting address for data section.  4MB seems to be a good lowest
/	common denominator so that Save files can move between all the
/	different DPMI platforms.
*/
#if VAX
#define CHUNK_SIZE	4096
#define CHUNK_B_SIZE	(CHUNK_SIZE * sizeof(word))
#define HEAP_SIZE	262144
#define OBJECT_SIZE	16384
#define TEXT_START      0
#endif

#if ATT73 | SUN | APOLLO | NCR | UNISYS | APPLE | HPUX | HPBSD | DSI | INTEL | HIGHC | LINUX | OS2 | MIPS | BCC32 | VCC | AIX | SOLARIS | SUNOS
#define CHUNK_SIZE	32768
#define CHUNK_B_SIZE	(CHUNK_SIZE * sizeof(word))
#define HEAP_SIZE	16777216	/* 16Mwords = 64Mbytes */
#if HIGHC | INTEL | OS2 | SUN4 | MIPS | LINUX | BCC32 | VCC | AIX
#define OBJECT_SIZE	1048576		/* 1 Mword = 4 Mbytes */
#else					/* HIGHC | INTEL | OS2 | SUN4 | MIPS */
#define OBJECT_SIZE	16384
#endif					/* HIGHC | INTEL | OS2 */
#endif					/* ATT73 | ... | OS2 */

#if BCC16
#define CHUNK_SIZE  0 /* computed dynamically in main() to fill out 64K segment */
#define CHUNK_B_SIZE	(CHUNK_SIZE * sizeof(word))
#define HEAP_SIZE	CHUNK_SIZE
#define OBJECT_SIZE	4500
#endif					/* BCC16 */

#if SUN | HPBSD
#define TEXT_START      8192
#endif

/*
 *  Define the maximum nesting allowed of INCLUDE files
 */
#define	INCLUDE_DEPTH	9


/*
 *  Define the standard file ids
 */
#ifndef STDINFD
#define STDINFD 0
#endif
#ifndef STDOUTFD
#define STDOUTFD 1
#endif
#ifndef STDERRFD
#define STDERRFD 2
#endif

/*
 *   Define number of SPITBOL statements to be executed between
 *   interface polling intervals.  Only used if POLLING is 1.
 *   Unix and OS/2 systems can get away with an infinite polling interval,
 *   because their interrupts are asynchronous, and do not require
 *   true polling.
 */
#ifndef PollCount
#if OS2 | UNIX
#define PollCount MAXPOSWORD
#else					/* OS2 | UNIX */
#define PollCount 2500
#endif					/* OS2 | UNIX */
#endif					/* PollCount */


/*
 *   Define Params macro to use or ignore function prototypes.
 */
#if PROTOTYPES
#define Params(a) a
#else
#define Params(a) ()
#endif


/*
/   The following manifest contant describes the constraints on the
/   run-time stack.
/
/   The value can be overriden via command line option.
/
/   STACK_SIZE		the maximum size of the run-time stack.  Any attempt
/                   to make the stack larger results in a stack overflow
/                   error.  Defined in BYTES!
*/
#if VAX
#define STACK_SIZE	8192
#endif

#if BCC16
#define STACK_SIZE	2812
#endif

#if APPLE
#define STACK_SIZE	16384
#endif

#if ATT73 | SUN | APOLLO | NCR | UNISYS | HPUX | HPBSD | DSI | HIGHC | INTEL | OS2 | MIPS | LINUX | BCC32 | VCC | AIX | SOLARIS | SUNOS
#define STACK_SIZE  (0x100000)      /* Set to 1MB 6/28/09 */
#endif


/*
/   The following manifest constant defines the location of the host file
/   which contains a one line description of the system environment under
/   which spitbol is running.
/
/   HOST_FILE		pathname for host text file used by function syshs
*/
#define HOST_FILE	"/usr/lib/spithost"

/*
/   The following manifest constant defines the names the files created
/   by the EXIT(3) and EXIT(-3) function.
/
/   AOUT_FILE		pathname for load module created by sysxi
/   SAVE_FILE		pathname for save file created by sysxi
*/
#define SAVE_FILE	"a.spx"
#if MSDOS | OS2 | WINNT
#define AOUT_FILE	"a.exe"
#else
#define AOUT_FILE	"a.out"
#endif

/*
/ PSEP is the separator between multiple paths.
/ FSEP is the separator between directories in a path.
/ EXT is separator between name and extension.
/ COMPEXT is extension for source files.
/ EFNEXT is extension for external functions.
/ RUNEXT is extension for save files.
/ BINEXT is extension for load modules
*/

#if APPLE
#define FSEP ':'
#endif

#if UNIX
#define PSEP  ':'
#define PSEP2 ' '
#define FSEP '/'
#endif

#if MSDOS | OS2 | WINNT
#define PSEP ';'
#define FSEP '\\'
#define FSEP2 '/'
#endif					/* MSDOS | OS2 */

#define EXT '.'
#if MSDOS | OS2 | WINNT
#define	BINEXT ".exe"
#else					/* MSDOS | OS2 */
#define	BINEXT ".out"
#endif					/* MSDOS | OS2 */
#define COMPEXT ".spt"
#if OS2
#define EFNEXT ".dll"
#else
#define EFNEXT ".slf"
#endif
#define LISTEXT ".lst"
#if APPLE
#define RUNEXT ""
#else					/* APPLE */
#define RUNEXT ".spx"
#endif					/* APPLE */

#define SPITFILEPATH  "snolib"  /* path for include and external files */

/*
/   The following manifest constant determines the maximum number of
/   files that can be open at a time.
/
/   OPEN_FILES		the maximum number of files that can be open at
/			a time.  Used by function ospipe to close files
/			given by a parent process to a child process.
*/
#define OPEN_FILES	32

/*
/   The following manifest constants determines the size of the temporary
/   SCBLKs defined by the interface.
/
/   TSCBLK_LENGTH	the maximum length of a string that can be stored
/                   in structure 'tscblk'.  'tscblk' is defined in
/                   file inter.s.
/
/   ID2BLK_LENGTH	the maximum length of a string that can be stored
/                   in structure 'id2blk'.  'id2blk' is defined in
/                   inter.c.  ID2BLK_LENGTH should be long enough
/                   to hold the computer name type string (htype)
/                   plus the date/time and a few blanks (typically
/                   20 characters).  It should also be a multiple of
/                   the word size.
/
*/
#ifndef TSCBLK_LENGTH
#define TSCBLK_LENGTH	512
#endif
#define ID2BLK_LENGTH	52

/*
/   The following manifest constants determine the default environment
/   variable name for the shell and it
/
/   SHELL_ENV_NAME	the name under which then shell path is stored
/                   in the environment
/
/   SHELL_PATH		a default shell to use in event one cannot be
/                   located in the environment
*/

#if MSDOS | OS2
#define SHELL_ENV_NAME	"COMSPEC"
#if OS2
#define SHELL_PATH	"cmd.exe"
#else					/* OS2 */
#define SHELL_PATH	"command.com"
#endif					/* OS2 */
#elif WINNT             /* MSDOS | OS2 */
extern char borland32rtm;             /* True if using DOS Extender */
extern char isWin95;                  /* True if running under WinNT */
#define SHELL_ENV_NAME  "COMSPEC"
#define SHELL_PATH  ((borland32rtm || isWin95) ? "command.com" : "cmd.exe")
#else                   /* MSDOS | OS2 */
#define SHELL_ENV_NAME	"SHELL"
#define SHELL_PATH      "/bin/sh"
#endif					/* MSDOS | OS2 */

/*
/   Compiler flags (see compiler listing for more details):
/
/   ERRORS	send errors to terminal
/   PRTICH	terminal is standard output file
/   NOLIST	suppress compilation listing
/   NOCMPS	suppress compilation statistics
/   NOEXCS	suppress execution statistics
/   LNGLST	generate long listing (WITH page ejects)
/   NOEXEC	suppress program execution
/   TRMNAL	support terminal i/o association
/   STDLST	standard listing (intermediate)
/   NOHDER	suppress spitbol compiler header
/   PRINTC      list control cards
/   WRTEXE	write executable module after compilation
/   CASFLD      fold upper and lower case names
/   NOFAIL	no fail mode
/   IUSTRG	international string compares (APPLE only)
/   FTRACE	function trace (APPLE only)
/   PROFIL	statement profiling (APPLE only)
/   PRTDMP	partial dump (APPLE only)
/   FULDMP	full dump (APPLE only)
/   NOVADV	supress vadvise() calls (SUN only)
/	RUNTIM  launch runtime (APPLE only)
/
/   DFLT_FLAGS	reasonable defaults for UN*X environment
*/

#define	ERRORS		0x00000001L
#define PRTICH		0x00000002L
#define NOLIST		0x00000004L
#define NOCMPS		0x00000008L
#define NOEXCS		0x00000010L
#define LNGLST		0x00000020L
#define NOEXEC		0x00000040L
#define TRMNAL		0x00000080L
#define STDLST		0x00000100L
#define NOHEDR		0x00000200L
#define PRINTC		0x00000400L
#define NOERRO		0x00000800L
#define CASFLD		0x00001000L
#define NOFAIL		0x00002000L
#define WRTEXE		0x00004000L
#define WRTSAV		0x00008000L

#if APPLE
#define IUSTRG		0x00010000L
#define FTRACE		0x00020000L
#define PROFIL		0x00040000L
#define PRTDMP		0x00080000L
#define FULDMP		0x00100000L
#define	RUNTIM		0x00200000L
#endif

#if SUN
#define NOVADV		0x01000000L	/*  interpreted by zysgc only	*/
#endif

#define NOBRAG		0x02000000L	/*	No signon header when loading save file */

#if APPLE
#define DFLT_FLAGS	(ERRORS+NOLIST+NOCMPS+NOEXCS+TRMNAL+PRINTC+CASFLD+NOERRO)
#else
#define DFLT_FLAGS	(ERRORS+PRTICH+NOLIST+NOCMPS+NOEXCS+TRMNAL+PRINTC+CASFLD+NOERRO)
#endif

#if !OS2
#define _Optlink
#endif					/* OS2 */

#define const

#include "osint.h"

#ifdef PRIVATEBLOCKS
#if BCC16
#include "extrn88.h"
#endif					/* BCC16 */
#if WINNT | SUN4 | AIX | LINUX
#include "extern32.h"
#endif          /* WINNT | SUN4 */
#else					/* PRIVATEBLOCKS */
#include "spitblks.h"
#include "spitio.h"
#endif					/* PRIVATEBLOCKS */


#include "globals.h"
#include "sproto.h"

#if APPLE
#include "uproto.h"
#endif
